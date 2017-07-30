/* -*- c++ -*- */
/* 
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "sensing_energy_impl.h"

namespace gr
{
  namespace spy
  {
    sensing_energy::sptr
    sensing_energy::make (size_t fft_size, double samp_rate, bool nf_est,
			  float noise_floor, int num_channels, float threshold,
			  uint8_t occup_percent)
    {
      return gnuradio::get_initial_sptr (
	  new sensing_energy_impl (fft_size, samp_rate, nf_est, noise_floor,
				   num_channels, threshold, occup_percent));
    }

    /*
     * The private constructor
     */
    sensing_energy_impl::sensing_energy_impl (size_t fft_size, double samp_rate,
    bool nf_est,
					      float noise_floor,
					      int num_channels, float threshold,
					      uint8_t occup_percent) :
	    gr::sync_block ("sensing_energy",
			    gr::io_signature::make (1, 1, sizeof(gr_complex)),
			    gr::io_signature::make (0, 0, 0)),
	    d_fft_size (fft_size),
	    d_num_channels (num_channels),
	    d_fft_queue (0),
	    d_fft_cnt (1),
	    d_cycles_nf_left (floor(0.1 / d_sec_per_fft)),
	    d_subs_per_channel (floor (d_fft_size / d_num_channels)),
	    d_samp_rate (samp_rate),
	    d_nf_est (nf_est),
	    d_occup_percent (occup_percent * 0.01),
	    d_noise_floor (noise_floor),
	    d_sec_per_fft (d_fft_size / d_samp_rate),
	    d_threshold (threshold)
    {
      d_fft = new fft::fft_complex (d_fft_size, true, 1);
      d_fft_in = d_fft->get_inbuf ();

      d_fftshift = (gr_complex*) volk_malloc ((d_fft_size) * sizeof(gr_complex),
					      volk_get_alignment ());
      d_psd = (float*) volk_malloc ((d_fft_size) * sizeof(float),
				    volk_get_alignment ());
      d_flat_top_win = (float*) volk_malloc ((d_fft_size) * sizeof(float),
					     volk_get_alignment ());
      d_noise_floor_vec_db = (float*) volk_malloc ((d_fft_size) * sizeof(float),
						   volk_get_alignment ());
      d_channel_stats = (float*) volk_malloc ((d_num_channels) * sizeof(float),
					      volk_get_alignment ());

      /* Calculation of Flat-top window */
      for (size_t i = 0; i < d_fft_size; i++) {

	d_flat_top_win[i] = 0.215578950
	    - 0.416631580 * cos ((2 * M_PI * i) / (d_fft_size - 1))
	    + 0.2772631580 * cos ((4 * M_PI * i) / (d_fft_size - 1))
	    - 0.0835789470 * cos ((6 * M_PI * i) / (d_fft_size - 1))
	    + 0.0069473680 * cos ((8 * M_PI * i) / (d_fft_size - 1));

	d_noise_floor_vec_db[i] = -400;
      }

      set_output_multiple (d_fft_size);
      message_port_register_out (pmt::mp ("out"));

      if (d_nf_est == true) {
	d_mode = NOISE_FLOOR_ESTIMATION;
      }
      else {
	set_nf (d_noise_floor);
	d_mode = SPECTRUM_SENSING;
      }
    }

    /*
     * Our virtual destructor.
     */
    sensing_energy_impl::~sensing_energy_impl ()
    {
      delete d_fft;
      volk_free (d_fftshift);
      volk_free (d_psd);
      volk_free (d_flat_top_win);
      volk_free (d_noise_floor_vec_db);
      volk_free (d_channel_stats);
    }

    int
    sensing_energy_impl::work (int noutput_items,
			       gr_vector_const_void_star &input_items,
			       gr_vector_void_star &output_items)
    {
      /* Create pointer to the input data */
      const gr_complex *in = (const gr_complex *) input_items[0];

      d_fft_queue = noutput_items / d_fft_size;
      d_fft_cnt = 0;

      switch (d_mode)
	{
	case NOISE_FLOOR_ESTIMATION:

	  while (d_fft_cnt < d_fft_queue) {
	    if (d_cycles_nf_left > 0) {

	      refresh_nf (in);

	      d_cycles_nf_left--;
	      d_fft_cnt++;
	    }
	    else {
	      d_mode = SPECTRUM_SENSING;
	      add_to_nf (d_threshold);
	      break;
	    }
	  }
	  break;

	case SPECTRUM_SENSING:

	  while (d_fft_cnt < d_fft_queue) {

	    sense_and_decide (in);
	    message_print (d_channel_stats, d_num_channels);

	    d_fft_cnt++;
	  }
	  break;
	}
      return noutput_items;
    }

    void
    sensing_energy_impl::sense_and_decide (const gr_complex *in)
    {
      calculate_psd (in + d_fft_cnt * d_fft_size);
      volk_32f_x2_subtract_32f (d_psd, d_noise_floor_vec_db, d_psd, d_fft_size);

      /* Get the signs */
      for (int i = 0; i < d_fft_size; i++) {

	d_psd[i] = d_psd[i] < 0;
      }

      for (int i = 0; i < d_num_channels; i++) {

	volk_32f_accumulator_s32f (d_channel_stats + i,
				   d_psd + i * d_subs_per_channel,
				   d_subs_per_channel);
	d_channel_stats[i] = d_channel_stats[i]
	    > d_occup_percent * d_subs_per_channel;
      }
    }

    void
    sensing_energy_impl::refresh_nf (const gr_complex *in)
    {
      calculate_psd (in + d_fft_cnt * d_fft_size);

      volk_32f_x2_max_32f (d_noise_floor_vec_db, d_psd, d_noise_floor_vec_db,
			   d_fft_size);
    }

    void
    sensing_energy_impl::message_print (float *vector, int vector_size)
    {
      pmt::pmt_t vector_combined = pmt::init_f32vector (vector_size, vector);
      message_port_pub (pmt::mp ("out"), vector_combined);
    }

    void
    sensing_energy_impl::calculate_psd (const gr_complex *in)
    {
      /* Apply flat-top window */
      volk_32fc_32f_multiply_32fc (d_fft_in, in, d_flat_top_win, d_fft_size);

      /* Perform fft */
      d_fft->execute ();
      /* Perform fftshift  */
      memcpy (&d_fftshift[0], &d_fft->get_outbuf ()[(d_fft_size / 2)],
	      sizeof(gr_complex) * (d_fft_size / 2));
      memcpy (&d_fftshift[(d_fft_size / 2)], &d_fft->get_outbuf ()[0],
	      sizeof(gr_complex) * (d_fft_size / 2));

      /* Calculate psd */
      volk_32fc_s32f_x2_power_spectral_density_32f (d_psd, d_fftshift,
						    d_fft_size, 1, d_fft_size);
    }

    void
    sensing_energy_impl::set_nf (float val)
    {
      for (int i = 0; i < d_fft_size; i++) {
	d_noise_floor_vec_db[i] = val;
      }
    }

    void
    sensing_energy_impl::add_to_nf (float val)
    {
      for (int i = 0; i < d_fft_size; i++) {
	d_noise_floor_vec_db[i] += val;
      }
    }

  } /* namespace spy */
} /* namespace gr */


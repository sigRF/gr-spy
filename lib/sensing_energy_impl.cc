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
    sensing_energy::make (size_t fft_size, double samp_rate)
    {
      return gnuradio::get_initial_sptr (
	  new sensing_energy_impl (fft_size, samp_rate));
    }

    /*
     * The private constructor
     */
    sensing_energy_impl::sensing_energy_impl (size_t fft_size, double samp_rate) :
	    gr::sync_block ("sensing_energy",
			    gr::io_signature::make (1, 1, sizeof(gr_complex)),
			    gr::io_signature::make (0, 0, 0)),
	    d_fft_size (fft_size),
	    d_samp_rate (samp_rate)
    {
      d_fft = new fft::fft_complex (d_fft_size, true, 1);
      d_fftshift = (gr_complex*) volk_malloc ((d_fft_size) * sizeof(gr_complex),
					      32);
      d_psd = (float*) volk_malloc ((d_fft_size) * sizeof(float), 32);
      d_flat_top_win = (float*) volk_malloc ((d_fft_size) * sizeof(float), 32);

      /* Calculation of Flat-top window */
      for (size_t i = 0; i < d_fft_size; i++) {

	d_flat_top_win[i] = 0.215578950
	    - 0.416631580 * cos ((2 * M_PI * i) / (d_fft_size - 1))
	    + 0.2772631580 * cos ((4 * M_PI * i) / (d_fft_size - 1))
	    - 0.0835789470 * cos ((6 * M_PI * i) / (d_fft_size - 1))
	    + 0.0069473680 * cos ((8 * M_PI * i) / (d_fft_size - 1));
      }

      set_output_multiple (d_fft_size);
      message_port_register_out (pmt::mp ("out"));
    }

    /*
     * Our virtual destructor.
     */
    sensing_energy_impl::~sensing_energy_impl ()
    {
      delete d_fft;
      volk_free(d_fftshift);
      volk_free (d_psd);
      volk_free(d_flat_top_win);
    }

    int
    sensing_energy_impl::work (int noutput_items,
			       gr_vector_const_void_star &input_items,
			       gr_vector_void_star &output_items)
    {
      std::cout << "ENERGY METHOD!" << std::endl;
      /* Create pointer to the fft inbuf */
      gr_complex *fft_in = d_fft->get_inbuf ();
      /* Create pointer to the input data */
      const gr_complex *in = (const gr_complex *) input_items[0];

      /* Apply flat-top window */
      volk_32fc_32f_multiply_32fc (fft_in, in, d_flat_top_win, d_fft_size);

      /* Perform fft */
      d_fft->execute ();
      /* Perform fftshift  */
      memcpy (&d_fftshift[0], &d_fft->get_outbuf ()[(d_fft_size / 2)],
	      sizeof(gr_complex) * (d_fft_size / 2));
      memcpy (&d_fftshift[(d_fft_size / 2)], &d_fft->get_outbuf ()[0],
	      sizeof(gr_complex) * (d_fft_size / 2));

      /* Calculate psd */
      volk_32fc_s32f_x2_power_spectral_density_32f (d_psd, d_fftshift,
						    1, d_fft_size*d_samp_rate, d_fft_size);

      pmt::pmt_t vector_combined = pmt::init_f32vector (d_fft_size, d_psd);
      message_port_pub (pmt::mp ("out"), vector_combined);

      return noutput_items;
    }

  } /* namespace spy */
} /* namespace gr */


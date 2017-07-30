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

#ifndef INCLUDED_SPY_SENSING_ENERGY_IMPL_H
#define INCLUDED_SPY_SENSING_ENERGY_IMPL_H

#include <spy/sensing_energy.h>
#include <gnuradio/fft/fft.h>
#include <volk/volk.h>
#include <math.h>

namespace gr
{
  namespace spy
  {

    class sensing_energy_impl : public sensing_energy
    {
    private:
      size_t d_fft_size;
      size_t d_num_channels;
      size_t d_fft_queue;
      size_t d_fft_cnt;
      size_t d_subs_per_channel;

      double d_samp_rate;

      bool d_nf_est;

      fft::fft_complex *d_fft;

      gr_complex *d_fftshift;
      gr_complex *d_fft_in;

      float *d_psd;
      float *d_flat_top_win;
      float *d_noise_floor_vec_db;
      float *d_channel_stats;

      uint8_t d_mode;
      float d_occup_percent;

      float d_noise_floor;
      float d_sec_per_fft;
      float d_threshold;

      size_t d_cycles_nf_left;

      void
      sense_and_decide (const gr_complex *in);
      void
      refresh_nf (const gr_complex *in);
      void
      message_print (float *vector, int vector_size);
      void
      set_nf (float val);
      void
      calculate_psd (const gr_complex *in);
      void
      add_to_nf (float val);

    public:
      sensing_energy_impl (size_t fft_size, double samp_rate, bool nf_est,
			   float noise_floor, int num_channels, float threshold,
			   uint8_t occup_percent);
      ~sensing_energy_impl ();

      // Where all the action really happens
      int
      work (int noutput_items, gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
    };

  } // namespace spy
} // namespace gr

#endif /* INCLUDED_SPY_SENSING_ENERGY_IMPL_H */


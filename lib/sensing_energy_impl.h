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

namespace gr {
  namespace spy {

    class sensing_energy_impl : public sensing_energy
    {
     private:
      size_t d_fft_size;
      double d_samp_rate;
      fft::fft_complex *d_fft;
      gr_complex *d_fftshift;
      float *d_psd;
      float *d_flat_top_win;

     public:
      sensing_energy_impl(size_t fft_size, double samp_rate);
      ~sensing_energy_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace spy
} // namespace gr

#endif /* INCLUDED_SPY_SENSING_ENERGY_IMPL_H */


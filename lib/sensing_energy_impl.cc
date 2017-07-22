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

namespace gr {
  namespace spy {

    sensing_energy::sptr
    sensing_energy::make(size_t fft_size, double samp_rate)
    {
      return gnuradio::get_initial_sptr
        (new sensing_energy_impl(fft_size, samp_rate));
    }

    /*
     * The private constructor
     */
    sensing_energy_impl::sensing_energy_impl(size_t fft_size, double samp_rate)
      : gr::sync_block("sensing_energy",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(0, 0, 0))
    {}

    /*
     * Our virtual destructor.
     */
    sensing_energy_impl::~sensing_energy_impl()
    {
    }

    int
    sensing_energy_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      //<+OTYPE+> *out = (<+OTYPE+> *) output_items[0];

      // Do <+signal processing+>
      std::cout << "ENERGY METHOD!" << std::endl;

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace spy */
} /* namespace gr */


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
#include "sensing_eigen_impl.h"

namespace gr
{
  namespace spy
  {

    sensing_eigen::sptr
    sensing_eigen::make (size_t fft_size, double samp_rate, bool nf_est,
			 float noise_floor)
    {
      return gnuradio::get_initial_sptr (
	  new sensing_eigen_impl (fft_size, samp_rate, nf_est, noise_floor));
    }

    /*
     * The private constructor
     */
    sensing_eigen_impl::sensing_eigen_impl (size_t fft_size, double samp_rate, bool nf_est,
						   float noise_floor) :
	    gr::sync_block ("sensing_eigen",
			    gr::io_signature::make (1, 1, sizeof(gr_complex)),
			    gr::io_signature::make (0, 0, 0))
    {
    }

    /*
     * Our virtual destructor.
     */
    sensing_eigen_impl::~sensing_eigen_impl ()
    {
    }

    int
    sensing_eigen_impl::work (int noutput_items,
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

	      d_cycles_nf_left--;
	      d_fft_cnt++;
	    }
	    else {
	      d_mode = SPECTRUM_SENSING;

	      break;
	    }
	  }
	  break;

	case SPECTRUM_SENSING:

	  while (d_fft_cnt < d_fft_queue) {

	    d_fft_cnt++;
	  }
	  break;
	}
      return noutput_items;
    }

  } /* namespace spy */
} /* namespace gr */


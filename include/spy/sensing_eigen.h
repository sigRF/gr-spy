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


#ifndef INCLUDED_SPY_SENSING_EIGEN_H
#define INCLUDED_SPY_SENSING_EIGEN_H

#include <spy/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace spy {

    /*!
     * \brief <+description of block+>
     * \ingroup spy
     *
     */
    class SPY_API sensing_eigen : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<sensing_eigen> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of spy::sensing_eigen.
       *
       * To avoid accidental use of raw pointers, spy::sensing_eigen's
       * constructor is in a private implementation
       * class. spy::sensing_eigen::make is the public interface for
       * creating new instances.
       */
      static sptr make(size_t fft_size, double samp_rate);
    };

  } // namespace spy
} // namespace gr

#endif /* INCLUDED_SPY_SENSING_EIGEN_H */


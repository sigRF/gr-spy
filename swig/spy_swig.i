/* -*- c++ -*- */

#define SPY_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "spy_swig_doc.i"

%{
#include "spy/sensing_energy.h"
#include "spy/sensing_eigen.h"
%}

%include "spy/sensing_energy.h"
GR_SWIG_BLOCK_MAGIC2(spy, sensing_energy);
%include "spy/sensing_eigen.h"
GR_SWIG_BLOCK_MAGIC2(spy, sensing_eigen);

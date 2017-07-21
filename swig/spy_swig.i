/* -*- c++ -*- */

#define SPY_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "spy_swig_doc.i"

%{
#include "spy/sensing.h"
%}


%include "spy/sensing.h"
GR_SWIG_BLOCK_MAGIC2(spy, sensing);

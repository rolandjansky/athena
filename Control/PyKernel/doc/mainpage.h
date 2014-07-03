/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage

@section PyKernelIntro Introduction 

The PyKernel package contains some python modules 
and python-bindings for interactive anaysis using Athena.
Note that this package is for python but doxygen doesn't
treat python very well. See <a href="http://tmaeno.home.cern.ch/tmaeno/PyKernel/index.html">PyKernel doc</a> instead.


@section PyKernelPythonMod Python modules

  - PyKernel.py: core module to help StoreGate access, histogramming and visualization
  - PyKHist.py: wrapper module for histograms

@section PyKernelPythonBind Python bindings

  - PyItPatch: contains patches to treat C++ iterators as python's iterator
  - PyReverseProxy: converts a C++ object to a PyObject

@section PyKernelExtras Extra Pages

  - @ref PyKernelUsed
  - @ref PyKernelReq
*/

/**
@page PyKernelUsed Used Packages
@htmlinclude used_packages.html
*/


/**
@page PyKernelReq Requirements
@include requirements

*/

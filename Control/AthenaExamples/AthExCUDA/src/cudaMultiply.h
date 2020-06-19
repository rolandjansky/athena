// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHEXCUDA_CUDAMULTIPLY_H
#define ATHEXCUDA_CUDAMULTIPLY_H

// System include(s).
#include <vector>

namespace AthCUDAExamples {

   /// Function used to multiple a vector of variables by some amount
   void cudaMultiply( std::vector< float >& array, float multiplier );

} // namespace AthCUDAExamples

#endif // ATHEXCUDA_CUDAMULTIPLY_H

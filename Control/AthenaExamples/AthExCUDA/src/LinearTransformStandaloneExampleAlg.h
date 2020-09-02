// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHEXCUDA_LINEARTRANSFORMSTANDALONEEXAMPLEALG_H
#define ATHEXCUDA_LINEARTRANSFORMSTANDALONEEXAMPLEALG_H

// Framework include(s).
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

namespace AthCUDAExamples {

   /// Example algorithm running a very simple operation using CUDA
   ///
   /// This is just to demonstrate how to organise C++ + CUDA code in Athena
   /// to execute CUDA code "directly".
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class LinearTransformStandaloneExampleAlg : public AthReentrantAlgorithm {

   public:
      /// Inherit the base class's constructor
      using AthReentrantAlgorithm::AthReentrantAlgorithm;

      /// The function executing this algorithm
      virtual StatusCode execute( const EventContext& ctx ) const override;

   }; // class LinearTransformStandaloneExampleAlg

} // namespace AthCUDAExamples

#endif // ATHEXCUDA_LINEARTRANSFORMSTANDALONEEXAMPLEALG_H

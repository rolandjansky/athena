// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHEXCUDA_LINEARTRANSFORMEXAMPLEALG_H
#define ATHEXCUDA_LINEARTRANSFORMEXAMPLEALG_H

// Framework include(s).
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

namespace AthCUDA {

   /// Example algorithm running a very simple operation using CUDA
   ///
   /// This is just to demonstrate how to organise C++ + CUDA code in Athena
   /// to execute CUDA code "directly".
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class LinearTransformExampleAlg : public AthReentrantAlgorithm {

   public:
      /// Inherit the base class's constructor
      using AthReentrantAlgorithm::AthReentrantAlgorithm;

      /// The function executing this algorithm
      StatusCode execute( const EventContext& ctx ) const;

   }; // class LinearTransformExampleAlg

} // namespace AthCUDA

#endif // ATHEXCUDA_LINEARTRANSFORMEXAMPLEALG_H

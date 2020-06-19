// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHEXCUDA_LINEARTRANSFORMTASKEXAMPLEALG_H
#define ATHEXCUDA_LINEARTRANSFORMTASKEXAMPLEALG_H

// AthCUDA include(s).
#include "AthCUDAInterfaces/IKernelRunnerSvc.h"

// Framework include(s).
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

namespace AthCUDAExamples {

   /// Example algorithm exercising the @c AthCUDA core code/services
   ///
   /// This algorithm does pretty much the same thing as
   /// @c AthCUDAExamples::LinearTransformStandaloneExampleAlg, it just does it
   /// in a nicer/more extensible way.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class LinearTransformTaskExampleAlg : public AthReentrantAlgorithm {

   public:
      // Inherit the base class's constructor(s).
      using AthReentrantAlgorithm::AthReentrantAlgorithm;

      /// Function initialising the algorithm
      virtual StatusCode initialize() override;

      /// Function executing the algorithm
      virtual StatusCode execute( const EventContext& ctx ) const override;

   private:
      /// The kernel runner service to use
      ServiceHandle< AthCUDA::IKernelRunnerSvc > m_kernelRunnerSvc{ this,
         "KernelRunnerSvc", "AthCUDA::KernelRunnerSvc",
         "The kernel runner service to use" };

   }; // class LinearTransformTaskExampleAlg

} // namespace AthCUDAExamples

#endif // ATHEXCUDA_LINEARTRANSFORMTASKEXAMPLEALG_H

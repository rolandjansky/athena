// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHEXSYCL_LINEARTRANSFORMEXAMPLEALG_H
#define ATHEXSYCL_LINEARTRANSFORMEXAMPLEALG_H

// Local include(s).
#include "DeviceSelector.h"

// Framework include(s).
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

namespace AthSYCL {

   /// Example algorithm running a very simple operation using SYCL
   ///
   /// This is just to demonstrate how to organise C++ + SYCL code in Athena
   /// to execute such code "directly".
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class LinearTransformExampleAlg : public AthReentrantAlgorithm {

   public:
      /// Inherit the base class's constructor
      using AthReentrantAlgorithm::AthReentrantAlgorithm;

      /// The function executing this algorithm
      StatusCode execute( const EventContext& ctx ) const;

   private:
      /// SYCL device selector
      DeviceSelector m_deviceSelector;

   }; // class LinearTransformExampleAlg

} // namespace AthSYCL

#endif // ATHEXSYCL_LINEARTRANSFORMEXAMPLEALG_H

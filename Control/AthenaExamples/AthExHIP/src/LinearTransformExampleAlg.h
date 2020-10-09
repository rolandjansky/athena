// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHEXHIP_LINEARTRANSFORMEXAMPLEALG_H
#define ATHEXHIP_LINEARTRANSFORMEXAMPLEALG_H

// Framework include(s).
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "Gaudi/Property.h"

namespace AthHIPExamples {

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

      /// @name Function(s) inherited from @c AthReentrantAlgortihm
      /// @{

      /// The function initialising the algorithm
      virtual StatusCode initialize() override;
      /// The function executing this algorithm
      virtual StatusCode execute( const EventContext& ctx ) const override;

      /// @}

   private:
      /// The HIP device (ID) to use
      Gaudi::Property< int > m_hipDevice{ this, "HIPDevice", 0,
         "The HIP device to use with the algorithm" };

      /// Size of the dummy array to use
      Gaudi::Property< unsigned int > m_arraySize{ this, "ArraySize", 10000,
         "Size of the float array to perform a linear transformation on" };

      /// The multiplier of the linear transformation
      Gaudi::Property< float > m_multiplier{ this, "Multiplier", 1.23f,
         "Multiplier used in the example linear transformation" };
      /// Shift used in the linear transformation
      Gaudi::Property< float > m_shift{ this, "Shift", 2.34f,
         "Shift used in the example linear transformation" };

   }; // class LinearTransformExampleAlg

} // namespace AthHIPExamples

#endif // ATHEXHIP_LINEARTRANSFORMEXAMPLEALG_H

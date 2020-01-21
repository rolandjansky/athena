// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//
#ifndef EVENTLOOPTEST_UNITTESTALG3_H
#define EVENTLOOPTEST_UNITTESTALG3_H

// EventLoop include(s).
#include "AnaAlgorithm/AnaAlgorithm.h"

namespace EL {

   /// Algorithm capable of leaking a set amount of memory per event
   ///
   /// This algorithm is used to test the ability of EventLoop to detect
   /// memory leaks in user code.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class UnitTestAlg3 final : public AnaAlgorithm {

   public:
      /// Standard algorithm constructor
      UnitTestAlg3( const std::string& name, ISvcLocator* svcLoc );

      /// @name Interface inherited from @c EL::AnaAlgorithm
      /// @{

      /// Function executing the algorithm
      virtual ::StatusCode execute() override;

      /// @}

   private:
      /// @name Algorithm properties
      /// @{

      /// The number of bytes to leak per event
      int m_leakBytes;

      /// @}

   }; // class UnitTestAlg3

} // namespace EL

#endif // EVENTLOOPTEST_UNITTESTALG3_H

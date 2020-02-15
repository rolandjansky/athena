// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//
#ifndef EVENTLOOPTEST_UNITTESTALG4_H
#define EVENTLOOPTEST_UNITTESTALG4_H

// EventLoop include(s).
#include "AnaAlgorithm/AnaAlgorithm.h"

namespace EL {

   /// Algorithm testing file metadata access
   ///
   /// This is meant to serve as a test of the metadata capabilities of
   /// @c EL::AnaAlgorithm.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class UnitTestAlg4 final : public AnaAlgorithm {

   public:
      /// Standard algorithm constructor
      UnitTestAlg4( const std::string& name, ISvcLocator* svcLoc );

      /// @name Interface inherited from @c EL::AnaAlgorithm
      /// @{

      /// Function initialising the algorithm
      virtual ::StatusCode initialize() override;

      /// Function executing the algorithm
      virtual ::StatusCode execute() override;

      /// Function finalising the algorithm
      virtual ::StatusCode finalize() override;

      /// Function called whenever a new input file's processing starts
      virtual ::StatusCode beginInputFile() override;

      /// @}

   private:
      /// Function reading some in-file metadata
      StatusCode readFileMetaData();

      /// Internal status flag
      int m_callCount = 0;

   }; // class UnitTestAlg4

} // namespace EL

#endif // EVENTLOOPTEST_UNITTESTALG4_H

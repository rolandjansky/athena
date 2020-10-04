// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODEVENTFORMATCNV_TEST_ELECTRONCREATORALG_H
#define XAODEVENTFORMATCNV_TEST_ELECTRONCREATORALG_H

// Framework include(s).
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"

// EDM include(s).
#include "xAODEventFormatCnv/test/AVec.h"

/// Namespace holding test code for the xAOD maker components
namespace xAODMakerTest {

   /// Algorithm creating a dummy @c xAODMakerTest::AVec
   ///
   /// This algorithm can be used to test the functionality of the
   /// xAODEventFormatCnv code. It just creates a dummy
   /// @c xAODMkerTest::AVec container that can then be used to create
   /// a test file.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   /// @author Frank Berghaus <fberghaus@anl.gov>
   ///
   class ACreatorAlg : public AthReentrantAlgorithm {

   public:
      // Use the base class's constructor
      using AthReentrantAlgorithm::AthReentrantAlgorithm;

      /// @name Interface inherited from @c AthReentrantAlgorithm
      /// @{

      /// Initialise the algorithm
      virtual StatusCode initialize() override;
      /// Execute the algorithm for one event
      virtual StatusCode execute( const EventContext& ctx ) const override;

      /// @}

   private:
      /// Key for the electron container created by the algorithm
      SG::WriteHandleKey< AVec > m_outputKey { this,
         "OutputKey", "TestObject",
         "Key for the object container created by the algorithm" };

   };  // class ElectronCreatorAlg

}  // namespace xAODMakerTest

#endif  // XAODEVENTFORMATCNV_TEST_ELECTRONCREATORALG_H

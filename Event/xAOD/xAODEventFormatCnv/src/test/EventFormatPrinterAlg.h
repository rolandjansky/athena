// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODEVENTFORMATCNV_TEST_EVENTFORMATPRINTERALG_H
#define XAODEVENTFORMATCNV_TEST_EVENTFORMATPRINTERALG_H

// Framework include(s).
#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/IMetaDataSvc.h"
#include "xAODEventFormat/EventFormat.h"

// System include(s).
#include <string>

namespace xAODMakerTest {

   /// Algorithm printing an @c xAOD::EventFormat object's payload
   ///
   /// This is used to debug/test the functionality of the @c xAOD::EventFormat
   /// creator components of the package.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class EventFormatPrinterAlg : public AthAlgorithm {

   public:
      /// Inherit the base class's constructor
      using AthAlgorithm::AthAlgorithm;

      /// @name Interface inherited from @c AthAlgorithm
      /// @{

      /// Initialise the algorithm
      virtual StatusCode initialize() override;
      /// Execute the algorithm for one event
      virtual StatusCode execute() override;

      /// @}

   private:
      /// Key for the @c xAOD::EventFormat object to read
      Gaudi::Property< std::string > m_eventFormatKey{this, "EventFormatKey",
        "EventFormat", "Key to retrieve xAOD::EventFormat, default: EventFormat"
      };

      ServiceHandle< IMetaDataSvc > m_metaDataStore{this, "MetaDataSvc",
        "MetaDataSvc", name()
      };
   };  // class EventFormatPrinterAlg

} // namespace xAODMakerTest

#endif // XAODEVENTFORMATCNV_TEST_EVENTFORMATPRINTERALG_H

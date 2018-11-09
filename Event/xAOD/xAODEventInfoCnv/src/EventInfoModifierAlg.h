// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODEVENTINFOCNV_EVENTINFOMODIFIERALG_H
#define XAODEVENTINFOCNV_EVENTINFOMODIFIERALG_H

// System include(s).
#include <string>

// Gaudi/Athena include(s).
#include "AthenaBaseComps/AthAlgorithm.h"

namespace xAODReader {

   /// Algorithm making modifications to a non-const @c xAOD::EventInfo
   ///
   /// This algorithm is here to allow for some tests that modify an
   /// @c xAOD::EventInfo object. That was created some "some way".
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class EventInfoModifierAlg final : public AthAlgorithm {

   public:
      /// Algorithm constructor
      EventInfoModifierAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function executing the algorithm
      virtual StatusCode execute() override;

   private:
      /// Key for the xAOD::EventInfo object
      std::string m_key;

   }; // class EventInfoModifierAlg

} // namespace xAODReader

#endif // XAODEVENTINFOCNV_EVENTINFOMODIFIERALG_H

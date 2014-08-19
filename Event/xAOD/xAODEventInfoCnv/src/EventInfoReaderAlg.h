// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoReaderAlg.h 592116 2014-04-09 12:04:38Z krasznaa $
#ifndef XAODEVENTINFOCNV_EVENTINFOREADERALG_H
#define XAODEVENTINFOCNV_EVENTINFOREADERALG_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

namespace xAODReader {

   /**
    *  @short Simple algorithm for testing the xAOD::EventInfo reading
    *
    *         This algorithm is just used for running simple tests with the I/O
    *         of the xAOD::EventInfo class.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 592116 $
    * $Date: 2014-04-09 14:04:38 +0200 (Wed, 09 Apr 2014) $
    */
   class EventInfoReaderAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      EventInfoReaderAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// StoreGate key of the object to be tested
      std::string m_key;

   }; // class EventInfoReaderAlg

} // namespace xAODReader

#endif // XAODEVENTINFOCNV_EVENTINFOREADERALG_H

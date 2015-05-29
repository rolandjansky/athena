// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventDuplicateFinderAlg.h 663741 2015-04-29 11:52:21Z krasznaa $
#ifndef XAODEVENTINFOCNV_EVENTDUPLICATEFINDERALG_H
#define XAODEVENTINFOCNV_EVENTDUPLICATEFINDERALG_H

// System include(s):
#include <string>
#include <map> // For std::pair...
#include <set>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

namespace xAODReader {

   /**
    *  @short Algorithm for finding event duplicates in xAOD files
    *
    *         This algorithm is used in simple validation jobs to make sure
    *         that we didn't put the same event multiple times into an output
    *         dataset in the production system.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 663741 $
    * $Date: 2015-04-29 13:52:21 +0200 (Wed, 29 Apr 2015) $
    */
   class EventDuplicateFinderAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      EventDuplicateFinderAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// StoreGate key of the EI object to be tested
      std::string m_key;

      /// Variable keeping track of the events seen
      std::set< std::pair< unsigned int, unsigned int > > m_seenEvents;

   }; // class EventDuplicateFinderAlg

} // namespace xAODReader

#endif // XAODEVENTINFOCNV_EVENTDUPLICATEFINDERALG_H

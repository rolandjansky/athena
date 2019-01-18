// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCREATORALGS_EVENTINFOAODFIXALG_H
#define XAODCREATORALGS_EVENTINFOAODFIXALG_H

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

// xAOD include
#include "xAODEventInfo/EventInfo.h"

namespace xAOD {

   /**
    *  @short Algorithm to fix AOD EventInfo  
    */
   class EventInfoRunNumberFixAlg : public AthAlgorithm {

   public:
      /// Regular algorithm constructor
      EventInfoRunNumberFixAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:

      /// Correct number number to be used to update EventInfo
      uint32_t m_runNumber;

   }; // class EventInfoRunNumberFixAlg

} // namespace xAODMaker

#endif 

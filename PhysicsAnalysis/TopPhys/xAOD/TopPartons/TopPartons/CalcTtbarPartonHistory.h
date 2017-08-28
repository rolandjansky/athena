/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTOP_TOPPARTONS_CALCTTBARPARTONHISTORY_H
#define ANALYSISTOP_TOPPARTONS_CALCTTBARPARTONHISTORY_H

/**
  * @author John Morris <john.morris@cern.ch>
  * @author Silvestre Marino Romano <sromanos@cern.ch>
  * @author Samuel Calvet <scalvet@cern.ch>
  * 
  * @brief CalcTtbarPartonHistory
  *   Class derived from CalcTopPartonHistory, used to store ttbar variables
  * 
  * $Revision: 658996 $
  * $Date: 2015-04-04 17:57:15 +0200 (Sat, 04 Apr 2015) $
  * 
  **/ 


// Framework include(s):
#include "TopPartons/CalcTopPartonHistory.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "TopPartons/PartonHistory.h"

// forward declaration(s):
namespace top{
  class TopConfig;
}

namespace top{

  class CalcTtbarPartonHistory : public CalcTopPartonHistory{
    public:      
      explicit CalcTtbarPartonHistory( const std::string& name );
      virtual ~CalcTtbarPartonHistory() {}
      
      //Storing parton history for ttbar resonance analysis      
      CalcTtbarPartonHistory(const CalcTtbarPartonHistory& rhs) = delete;
      CalcTtbarPartonHistory(CalcTtbarPartonHistory&& rhs) = delete;
      CalcTtbarPartonHistory& operator=(const CalcTtbarPartonHistory& rhs) = delete; 
      
      void ttbarHistorySaver(const xAOD::TruthParticleContainer* truthParticles, xAOD::PartonHistory* ttbarPartonHistory); 
             
      virtual StatusCode execute();
      
  };
  
}

#endif

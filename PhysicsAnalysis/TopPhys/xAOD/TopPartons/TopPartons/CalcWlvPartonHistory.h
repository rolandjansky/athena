/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTOP_TOPPARTONS_CALCWLVPARTONHISTORY_H
#define ANALYSISTOP_TOPPARTONS_CALCWLVPARTONHISTORY_H

/**
  * @author John Morris <john.morris@cern.ch>
  * @author Silvestre Marino Romano <sromanos@cern.ch>
  * @author Samuel Calvet <scalvet@cern.ch>
  * 
  * @brief CalcWlvPartonHistory
  *   Class derived from CalcTopPartonHistory, used to store ttbar variables
  * 
  * $Revision: 782353 $
  * $Date: 2016-11-04 13:11:17 +0100 (Fri, 04 Nov 2016) $
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

  class CalcWlvPartonHistory : public CalcTopPartonHistory{
    public:      
      explicit CalcWlvPartonHistory( const std::string& name );
      virtual ~CalcWlvPartonHistory() {}
      
      //Storing parton history for ttbar resonance analysis      
      CalcWlvPartonHistory(const CalcWlvPartonHistory& rhs) = delete;
      CalcWlvPartonHistory(CalcWlvPartonHistory&& rhs) = delete;
      CalcWlvPartonHistory& operator=(const CalcWlvPartonHistory& rhs) = delete; 
      
      void WlvHistorySaver(const xAOD::TruthParticleContainer* truthParticles, xAOD::PartonHistory* wlvPartonHistory); 
             
      virtual StatusCode execute();
      
  };
  
}

#endif

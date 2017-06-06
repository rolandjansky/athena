/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTOP_TOPPARTONS_CALCTBBARPARTONHISTORY_H
#define ANALYSISTOP_TOPPARTONS_CALCTBBARPARTONHISTORY_H

/**
  * @author John Morris <john.morris@cern.ch>
  * @author Silvestre Marino Romano <sromanos@cern.ch>
  * @author Samuel Calvet <scalvet@cern.ch>
  * 
  * @brief CalcTbbarPartonHistory
  *   Class derived from CalcTopPartonHistory, used to store tbbar variables
  * 
  * $Revision: 777122 $
  * $Date: 2016-10-07 15:57:57 +0200 (Fri, 07 Oct 2016) $
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

  class CalcTbbarPartonHistory : public CalcTopPartonHistory{
    public:      
      explicit CalcTbbarPartonHistory( const std::string& name );
      virtual ~CalcTbbarPartonHistory() {}
      
      //Storing parton history for tbbar resonance analysis      
      CalcTbbarPartonHistory(const CalcTbbarPartonHistory& rhs) = delete;
      CalcTbbarPartonHistory(CalcTbbarPartonHistory&& rhs) = delete;
      CalcTbbarPartonHistory& operator=(const CalcTbbarPartonHistory& rhs) = delete; 
      
      void tbbarHistorySaver(const xAOD::TruthParticleContainer* truthParticles, xAOD::PartonHistory* tbbarPartonHistory); 
             
      virtual StatusCode execute();
      
  };
  
}

#endif

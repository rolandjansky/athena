/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetAnalysisEDM/KinematicUtils.h"
#include "JetAnalysisEDM/ParticleContainer.h"


namespace JetAnalysisEDM {


  ParticleSelector::~ParticleSelector(){
    for(size_t i=0; i<m_selectors.size(); i++) delete m_selectors[i];
  }

  void ParticleSelector::filter(const ParticleContainer &in , ParticleContainer& out, bool preClear){
    size_t nSel = m_selectors.size();
    if(preClear) out.clear();
    for(size_t i=0; i<nSel; i++) m_selectors[i]->init(in);
    std::vector<int> passIndex; passIndex.reserve(in.size());
    for(size_t i=0; i<in.size(); i++) {
      const Particle& p = *in[i];
      bool pass = true; size_t sel=0;
      while(pass){
        pass = (*m_selectors[sel])(p);
        sel++;
        if(sel==nSel) break;
      }
      if(pass) passIndex.push_back(i);
    }

    out.setSubsetOfInputContainer(&in);
    
    out.copyFrom(in, passIndex);

  }


};

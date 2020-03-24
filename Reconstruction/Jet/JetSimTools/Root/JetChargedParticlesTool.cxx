/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetChargedParticlesTool.cxx

#include "JetSimTools/JetChargedParticlesTool.h"

#include "xAODJet/JetConstituentVector.h"
#include "xAODTruth/TruthParticleContainer.h"


//**********************************************************************

JetChargedParticlesTool::JetChargedParticlesTool(std::string myname)
  : JetModifierBase(myname)
{
   declareProperty("MinPt",m_minPt);
   declareProperty("MaxEta",m_maxEta);
}

//**********************************************************************

int JetChargedParticlesTool::modifyJet(xAOD::Jet& jet) const {

  int nc = ncharged(jet);
  if(nc==-1) return 1;
  jet.setAttribute("nChargedParticles", nc);
  return 0;
}

//**********************************************************************

int JetChargedParticlesTool::ncharged(const xAOD::Jet& jet) const {

  // Get the constituents of the jet
  
  int n_charged = 0; 
  
  size_t nconstit = jet.numConstituents();
  for(size_t i=0;i<nconstit;i++){
    if(const xAOD::TruthParticle * tp = dynamic_cast<const xAOD::TruthParticle*>( jet.rawConstituent(i) ) ){

      if( (tp->pt()>m_minPt) && 
  	  (fabs(tp->eta())<m_maxEta) && 
	  (tp->charge()!=0) ){ 
        n_charged++;
      }

    }
    else {
      ATH_MSG_ERROR(" Unable to dynamic_cast raw constituent to truth particle "); 
      return -1;
    }
  }
  
  return n_charged;
}
  //

//**********************************************************************

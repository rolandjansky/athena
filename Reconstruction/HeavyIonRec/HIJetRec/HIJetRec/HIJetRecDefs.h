/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIJETREC_HIJETRECDEFS_H
#define HIJETREC_HIJETRECDEFS_H

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODJet/JetTypes.h"
#include "HIEventUtils/HIEventDefs.h"
#include "FourMomUtils/xAODP4Helpers.h"


namespace HIJetRec{

  //define conventions for HIJets in terms of various signal states
  constexpr const char* unsubtractedJetState() { return "JetUnsubtractedScaleMomentum";}
  constexpr const char* subtractedJetState() { return "JetSubtractedScaleMomentum";}
  constexpr xAOD::JetConstitScale subtractedConstitState() {return xAOD::UncalibratedJetConstituent;}

  constexpr xAOD::CaloCluster::State unsubtractedClusterState() {return xAOD::CaloCluster::ALTCALIBRATED;}
  constexpr xAOD::CaloCluster::State subtractedClusterState() {return xAOD::CaloCluster::UNCALIBRATED;}

  inline bool inTowerBoundary(float eta0, float phi0, float eta, float phi)
  {
    if( 2.*std::abs(eta-eta0) > HI::TowerBins::getBinSizeEta() ) return false;
    if( 2.*std::abs(xAOD::P4Helpers::deltaPhi(phi0,phi)) > HI::TowerBins::getBinSizePhi() ) return false;
    return true;
  }
  
  inline void setClusterP4(const xAOD::CaloCluster::FourMom_t& p, xAOD::CaloCluster* cl, xAOD::CaloCluster::State s)
  {
    float energy=p.Energy();
    float eta=p.Eta();
    float phi=p.Phi();
    if(energy < 0.) 
    {
      eta*=-1.;
      if(phi > 0.) phi-=M_PI;
      else phi+=M_PI;
    }
    if(s==xAOD::CaloCluster::UNCALIBRATED)
    {
      cl->setRawE(energy);
      cl->setRawEta(eta);
      cl->setRawPhi(phi);
      cl->setRawM(0);
    }
    else if(s==xAOD::CaloCluster::ALTCALIBRATED)
    {
      cl->setAltE(energy);
      cl->setAltEta(eta);
      cl->setAltPhi(phi);
      cl->setAltM(0);
    }
    else if(s==xAOD::CaloCluster::CALIBRATED)
    {
      cl->setCalE(energy);
      cl->setCalEta(eta);
      cl->setCalPhi(phi);
      cl->setCalM(0);
    }
  }

}
#endif

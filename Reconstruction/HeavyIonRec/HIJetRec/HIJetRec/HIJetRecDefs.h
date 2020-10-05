/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIJETREC_HIJETRECDEFS_H
#define HIJETREC_HIJETRECDEFS_H

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODJet/JetTypes.h"
#include "HIEventUtils/HIEventDefs.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include "TLorentzVector.h"


namespace HIJetRec{

  //define conventions for HIJets in terms of various signal states
  constexpr const char* unsubtractedJetState()                { return "JetUnsubtractedScaleMomentum";}
  constexpr const char* subtractedJetState()                  { return "JetSubtractedScaleMomentum";}
  constexpr const char* subtractedOriginCorrectedJetState()   { return "JetSubtractedOriginCorrectedScaleMomentum";}

  constexpr xAOD::JetConstitScale subtractedConstitState()                { return xAOD::UncalibratedJetConstituent;}
  constexpr xAOD::JetConstitScale subtractedOriginCorrectedConstitState() { return xAOD::CalibratedJetConstituent;}

  constexpr xAOD::CaloCluster::State unsubtractedClusterState() {return xAOD::CaloCluster::ALTCALIBRATED;}
  constexpr xAOD::CaloCluster::State subtractedClusterState() {return xAOD::CaloCluster::UNCALIBRATED;}
  constexpr xAOD::CaloCluster::State subtractedOriginCorrectedClusterState() {return xAOD::CaloCluster::CALIBRATED;}

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

/// \brief : getClusterP4 Added during Rel22 migration. We should never use CaloCluster_v1::p4() because this four vector can
/// never have a negative energy. Therefore, if one tries to use setClusterP4 and then retrieve the cluster four momenta,
/// must use this new method that keeps the cluster information as set in setClusterP4 (negative E and flipped eta phi)
  inline TLorentzVector getClusterP4( const xAOD::CaloCluster* cl, xAOD::CaloCluster::State s ){
    TLorentzVector correct_clusP4;
    float energy=cl->e(s);
    float eta=cl->eta(s);
    float phi=cl->phi(s);
    if(energy < 0.)
    {
      eta*=-1.;
      if(phi > 0.) phi-=M_PI;
      else phi+=M_PI;
    }
    float pt=energy/std::cosh(eta);
    correct_clusP4.SetPtEtaPhiE(pt, eta,phi ,energy);
    return correct_clusP4;
  }
}
#endif

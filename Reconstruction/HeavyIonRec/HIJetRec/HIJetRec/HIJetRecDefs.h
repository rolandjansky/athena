/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIJETREC_HIJETRECDEFS_H
#define HIJETREC_HIJETRECDEFS_H

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODJet/JetTypes.h"

namespace HIJetRec{

  //define conventions for HIJets in terms of various signal states
  constexpr const char* unsubtractedJetState() { return "JetUnsubtractedScaleMomentum";}
  constexpr const char* subtractedJetState() { return "JetSubtractedScaleMomentum";}
  constexpr xAOD::JetConstitScale subtractedConstitState() {return xAOD::UncalibratedJetConstituent;}

  constexpr xAOD::CaloCluster::State unsubtractedClusterState() {return xAOD::CaloCluster::ALTCALIBRATED;}
  constexpr xAOD::CaloCluster::State subtractedClusterState() {return xAOD::CaloCluster::UNCALIBRATED;}

  inline void setClusterP4(const xAOD::CaloCluster::FourMom_t& p, xAOD::CaloCluster* cl, xAOD::CaloCluster::State s)
  {
    if(s==xAOD::CaloCluster::UNCALIBRATED)
    {
      cl->setRawE(p.Energy());
      cl->setRawEta(p.Eta());
      cl->setRawPhi(p.Phi());
      cl->setRawM(0);
    }
    else if(s==xAOD::CaloCluster::ALTCALIBRATED)
    {
      cl->setAltE(p.Energy());
      cl->setAltEta(p.Eta());
      cl->setAltPhi(p.Phi());
      cl->setAltM(0);
    }
    else if(s==xAOD::CaloCluster::CALIBRATED)
    {
      cl->setCalE(p.Energy());
      cl->setCalEta(p.Eta());
      cl->setCalPhi(p.Phi());
      cl->setCalM(0);
    }
  }

}
#endif

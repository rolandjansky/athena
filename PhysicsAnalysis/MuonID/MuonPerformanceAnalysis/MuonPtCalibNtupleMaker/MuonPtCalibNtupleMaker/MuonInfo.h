/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MuonInfo_h__
#define __MuonInfo_h__

//::: STL includes
#include <string>

//::: ROOT includes
#include <TTree.h>

//::: Framework includes
#include "CLHEP/Units/SystemOfUnits.h"

//::: EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

class MuonInfo { 
  
private:
  Float_t m_d0Significance, m_z0;
  Float_t m_PtVarCone30, m_TopoEtCone20, m_PtVarCone20,m_PtVarCone40;
  Float_t m_PtCone30, m_PtCone20, m_PtCone40;
  Int_t   m_EnergyLossType;
  Float_t m_EnergyLoss, m_EnergyLossSigma;
  Int_t   m_Quality, m_Author, m_PrimarySector, m_SecondarySector, m_Type;
  Float_t m_TruthPt, m_TruthEta, m_TruthPhi; 
  Bool_t m_passHighPt, m_passIDHits;
public:
  MuonInfo();
  ~MuonInfo() {}

  void Reset();
  void Register( TTree* t, std::string p );
  void Fill( const xAOD::Muon* muon, const xAOD::Vertex* pv, const xAOD::EventInfo* ei );

};

#endif

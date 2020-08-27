/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/CaloTag.h"

namespace MuonCombined {

/** constructors */
CaloTag::CaloTag (void)
    :   TagBase(xAOD::Muon::CaloTag,xAOD::Muon::CaloTagged), 
	Trk::EnergyLoss(0,0,0,0),
        m_caloLRLikelihood      (0),
        m_caloMuonScore         (0),
        m_caloMuonIdTag         (0),
        m_fsrCandidateEnergy    (0),
        m_deposits              (),
        m_etCore                (0.0),
        m_author2(xAOD::Muon::unknown),
        m_author3(xAOD::Muon::unknown)
{ m_energyLossType = static_cast<CaloTag::EnergyLossType>(0); }

CaloTag::CaloTag (const Trk::EnergyLoss& eloss)
    :   TagBase(xAOD::Muon::CaloTag,xAOD::Muon::CaloTagged), 
	Trk::EnergyLoss( eloss ),
        m_caloLRLikelihood      (0),
        m_caloMuonScore         (0),
        m_caloMuonIdTag         (0),
        m_fsrCandidateEnergy    (0),
        m_deposits              (),
        m_etCore                (0.0),
        m_author2(xAOD::Muon::unknown),
        m_author3(xAOD::Muon::unknown)
{ m_energyLossType = static_cast<CaloTag::EnergyLossType>(0); }

CaloTag::CaloTag (xAOD::Muon::Author author,
		  float   deltaE,
                  float   sigmaDeltaE,
                  float   sigmaMinusDeltaE,
                  float   sigmaPlusDeltaE,
                  unsigned short energyLossType,
                  float          likelihood,
                  float          muonScore,
                  unsigned short tag)
    :   TagBase(author,xAOD::Muon::CaloTagged), 
	Trk::EnergyLoss(deltaE, sigmaDeltaE, sigmaMinusDeltaE, sigmaPlusDeltaE ),
        m_caloLRLikelihood      (likelihood),
        m_caloMuonScore         (muonScore),
        m_caloMuonIdTag         (tag),
        m_deposits              (),
        m_etCore                (0.0),
        m_author2(xAOD::Muon::unknown),
        m_author3(xAOD::Muon::unknown)
{
        m_energyLossType = static_cast<CaloTag::EnergyLossType>(energyLossType);
        m_fsrCandidateEnergy = 0.0;
}


CaloTag::CaloTag (xAOD::Muon::Author author,
		  float   deltaE,
                  float   sigmaDeltaE,
                  float   sigmaMinusDeltaE,
                  float   sigmaPlusDeltaE,
                  unsigned short energyLossType,
                  float          likelihood,
                  float          muonScore,
                  unsigned short tag,
                  const std::vector<DepositInCalo>& deposits)

    :   TagBase(author,xAOD::Muon::CaloTagged), 
	Trk::EnergyLoss(deltaE, sigmaDeltaE, sigmaMinusDeltaE, sigmaPlusDeltaE ),
        m_caloLRLikelihood      (likelihood),
        m_caloMuonScore         (muonScore),
        m_caloMuonIdTag         (tag),
        m_etCore                (0.0),
        m_author2(xAOD::Muon::unknown),
        m_author3(xAOD::Muon::unknown)
{
        m_energyLossType = static_cast<CaloTag::EnergyLossType>(energyLossType);
        m_fsrCandidateEnergy = 0.0;
        m_deposits = deposits;
}

  CaloTag::~CaloTag() {
  }

}

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 calorimeter energy deposit (measured or parametrized) with asymmetric error
 Author: Alan Poppleton
 Author: Gustavo Ordenez
 Integration with Analysis::Muon.h - Ketevi A. Assamagan
 ---------------------------------------------------------------------------
 ***************************************************************************/

#include "muonEvent/CaloEnergy.h"

using namespace Trk;

/** constructors */
CaloEnergy::CaloEnergy (void)
    :	EnergyLoss(0,0,0,0),
        m_caloLRLikelihood      (0),
        m_caloMuonIdTag         (0),
        m_fsrCandidateEnergy    (0),
        m_deposits              (),
        m_etCore                (0.0),
	m_deltaE_param           (0.0),
	m_sigmaMinusDeltaE_param (0.0),
	m_sigmaPlusDeltaE_param  (0.0),
	m_deltaE_meas            (0.0),
	m_sigmaDeltaE_meas       (0.0)
{ m_energyLossType = static_cast<CaloEnergy::EnergyLossType>(0); }

CaloEnergy::CaloEnergy (const Trk::EnergyLoss& eloss)
  :  EnergyLoss( eloss ),
     m_caloLRLikelihood      (0),
     m_caloMuonIdTag         (0),
     m_fsrCandidateEnergy    (0),
     m_deposits              (),
     m_etCore                (0.0),
     m_deltaE_param           (0.0),
     m_sigmaMinusDeltaE_param (0.0),
     m_sigmaPlusDeltaE_param  (0.0),
     m_deltaE_meas            (0.0),
     m_sigmaDeltaE_meas       (0.0)
{ m_energyLossType = static_cast<CaloEnergy::EnergyLossType>(0); }

CaloEnergy::CaloEnergy (float   deltaE,
                        float   sigmaDeltaE,
                        float   sigmaMinusDeltaE,
                        float   sigmaPlusDeltaE,
			unsigned short energyLossType,
		        float          likelihood,
		        unsigned short tag)

    :   EnergyLoss(deltaE, sigmaDeltaE, sigmaMinusDeltaE, sigmaPlusDeltaE ),	
        m_caloLRLikelihood      (likelihood),
        m_caloMuonIdTag         (tag),
        m_deposits              (),
        m_etCore                (0.0),
	m_deltaE_param           (0.0),
	m_sigmaMinusDeltaE_param (0.0),
	m_sigmaPlusDeltaE_param  (0.0),
	m_deltaE_meas            (0.0),
	m_sigmaDeltaE_meas       (0.0)
{
	m_energyLossType = static_cast<CaloEnergy::EnergyLossType>(energyLossType);
        m_fsrCandidateEnergy = 0.0;
}

CaloEnergy::CaloEnergy (float   deltaE,
                        float   sigmaDeltaE,
                        float   sigmaMinusDeltaE,
                        float   sigmaPlusDeltaE,
			unsigned short energyLossType,
		        float          likelihood,
		        unsigned short tag,
			const std::vector<DepositInCalo>& deposits)

    :   EnergyLoss(deltaE, sigmaDeltaE, sigmaMinusDeltaE, sigmaPlusDeltaE ),
        m_energyLossType (static_cast<CaloEnergy::EnergyLossType>(energyLossType)),
        m_caloLRLikelihood      (likelihood),
        m_caloMuonIdTag         (tag),
	m_fsrCandidateEnergy    (0.0),
        m_deposits              (deposits),
        m_etCore                (0.0), 
	m_deltaE_param           (0.0),
	m_sigmaMinusDeltaE_param (0.0),
	m_sigmaPlusDeltaE_param  (0.0),
	m_deltaE_meas            (0.0),
	m_sigmaDeltaE_meas       (0.0)
{
}

/** copy constructor */
CaloEnergy::CaloEnergy (const CaloEnergy& caloEnergy)
    :   EnergyLoss ( caloEnergy ),
	m_energyLossType	(caloEnergy.m_energyLossType),
        m_caloLRLikelihood      (caloEnergy.m_caloLRLikelihood),
        m_caloMuonIdTag         (caloEnergy.m_caloMuonIdTag),
        m_fsrCandidateEnergy    (caloEnergy.m_fsrCandidateEnergy),
        m_deposits              (caloEnergy.m_deposits),
        m_etCore                (caloEnergy.m_etCore),
	m_deltaE_param          (caloEnergy.m_deltaE_param),
	m_sigmaMinusDeltaE_param (caloEnergy.m_sigmaMinusDeltaE_param),
	m_sigmaPlusDeltaE_param (caloEnergy.m_sigmaPlusDeltaE_param),
	m_deltaE_meas           (caloEnergy.m_deltaE_meas),
	m_sigmaDeltaE_meas      (caloEnergy.m_sigmaDeltaE_meas)         
{}

/** destructor */
CaloEnergy::~CaloEnergy() {}

/** Assignment operator */
CaloEnergy& CaloEnergy::operator=(const CaloEnergy& rhs) {
  if ( this != &rhs ) {
     EnergyLoss::operator=( rhs );
     m_energyLossType     = rhs.m_energyLossType;
     m_caloLRLikelihood   = rhs.m_caloLRLikelihood;
     m_caloMuonIdTag      = rhs.m_caloMuonIdTag;
     m_fsrCandidateEnergy = rhs.m_fsrCandidateEnergy;
     m_deposits           = rhs.m_deposits; 
     m_etCore             = rhs.m_etCore;
     m_deltaE_param           = rhs.m_deltaE_param;
     m_sigmaMinusDeltaE_param = rhs.m_sigmaMinusDeltaE_param;
     m_sigmaPlusDeltaE_param  = rhs.m_sigmaPlusDeltaE_param;
     m_deltaE_meas            = rhs.m_deltaE_meas;
     m_sigmaDeltaE_meas       = rhs.m_sigmaDeltaE_meas;
  }
  return *this;
}



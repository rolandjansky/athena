/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "muonEvent/DepositInCalo.h"

#include <iostream>

void DepositInCalo::print() const 
{
 
  std::cout << "DepositInCalo: "; 
  if( subCaloId() == CaloCell_ID::PreSamplerB ) std::cout << "PreSamplerB";
  else if( subCaloId() == CaloCell_ID::EMB1 ) std::cout << "EMB1";
  else if( subCaloId() == CaloCell_ID::EMB2 ) std::cout << "EMB2";
  else if( subCaloId() == CaloCell_ID::EMB3 ) std::cout << "EMB3";
  else if( subCaloId() == CaloCell_ID::PreSamplerE ) std::cout << "PreSamplerE";
  else if( subCaloId() == CaloCell_ID::EME1 ) std::cout << "EME1";
  else if( subCaloId() == CaloCell_ID::EME2 ) std::cout << "EME2";
  else if( subCaloId() == CaloCell_ID::EME3 ) std::cout << "EME3";
  else if( subCaloId() == CaloCell_ID::HEC0 ) std::cout << "HEC0";
  else if( subCaloId() == CaloCell_ID::HEC1 ) std::cout << "HEC1";
  else if( subCaloId() == CaloCell_ID::HEC2 ) std::cout << "HEC2";
  else if( subCaloId() == CaloCell_ID::HEC3 ) std::cout << "HEC3";
  else if( subCaloId() == CaloCell_ID::TileBar0 ) std::cout << "TileBar0";
  else if( subCaloId() == CaloCell_ID::TileBar1 ) std::cout << "TileBar1";
  else if( subCaloId() == CaloCell_ID::TileBar2 ) std::cout << "TileBar2";
  else if( subCaloId() == CaloCell_ID::TileGap1 ) std::cout << "TileGap1";
  else if( subCaloId() == CaloCell_ID::TileGap2 ) std::cout << "TileGap2";
  else if( subCaloId() == CaloCell_ID::TileGap3 ) std::cout << "TileGap3";
  else if( subCaloId() == CaloCell_ID::TileExt0 ) std::cout << "TileExt0";
  else if( subCaloId() == CaloCell_ID::TileExt1 ) std::cout << "TileExt1";
  else if( subCaloId() == CaloCell_ID::TileExt2 ) std::cout << "TileExt2";
  else if( subCaloId() == CaloCell_ID::FCAL0 ) std::cout << "FCAL0";
  else if( subCaloId() == CaloCell_ID::FCAL1 ) std::cout << "FCAL1";
  else if( subCaloId() == CaloCell_ID::FCAL2 ) std::cout << "FCAL2";
  else std::cout << "Unknown";

  std::cout << " EnergyDeposited " << energyDeposited() 
	    << " Energy loss " << muonEnergyLoss() 
            << " Et Deposited " << etDeposited() << std::endl;
}

/** copy constructor */
DepositInCalo::DepositInCalo (const DepositInCalo& rhs)
  :	m_subCaloId       ( rhs.m_subCaloId ),
	m_energyDeposited ( rhs.m_energyDeposited ),
	m_muonEnergyLoss  ( rhs.m_muonEnergyLoss ),
        m_etDeposited     ( rhs.m_etDeposited )
{}

/** assignment operator */
DepositInCalo & DepositInCalo::operator= (const DepositInCalo & rhs) {
  if ( this != &rhs ) {
    m_subCaloId       = rhs.m_subCaloId;
    m_energyDeposited = rhs.m_energyDeposited;
    m_muonEnergyLoss  = rhs.m_muonEnergyLoss;
    m_etDeposited     = rhs.m_etDeposited;
  }
  return *this;
}


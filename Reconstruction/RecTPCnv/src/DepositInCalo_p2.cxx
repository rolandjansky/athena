///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// DepositInCalo_p2.cxx 
// Implementation file for class DepositInCalo_p2
// Author: Ketevi A. Assamagan
/////////////////////////////////////////////////////////////////// 


// RecTPCnv includes
#include "RecTPCnv/DepositInCalo_p2.h"
#include "RecTPCnv/DepositInCalo_p1.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

DepositInCalo_p2::DepositInCalo_p2 (const DepositInCalo_p1& other)
  : m_subCaloId (other.m_subCaloId),
    m_energyDeposited (other.m_energyDeposited),
    m_muonEnergyLoss (other.m_muonEnergyLoss),
    m_etDeposited (0)
{
}

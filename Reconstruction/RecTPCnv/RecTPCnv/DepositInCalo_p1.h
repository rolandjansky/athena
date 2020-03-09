///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// DepositInCalo_p1.h 
// Header file for class DepositInCalo_p1
// Author: S.Binet<binet@cern.ch>
// Date:   March 2007
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_DEPOSITINCALO_P1_H 
#define RECTPCNV_DEPOSITINCALO_P1_H 


class DepositInCalo_p1 
{
public: 

  /** Default constructor: 
   */
  DepositInCalo_p1()
    : m_subCaloId(0),
      m_energyDeposited(0),
      m_muonEnergyLoss(0)
  {}

  /** Destructor: 
   */
  ~DepositInCalo_p1() = default;


  /** calorimeter sampling identifier */
  unsigned short m_subCaloId;

  /** energy Desposited */
  float m_energyDeposited;

  /** energy Loss of the muons computed using the extrapolator */
  float m_muonEnergyLoss;
}; 


#endif //> RECTPCNV_DEPOSITINCALO_P1_H

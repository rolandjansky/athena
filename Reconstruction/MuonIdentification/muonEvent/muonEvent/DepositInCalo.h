/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENT_DEPOSITINCALO_H
#define MUONEVENT_DEPOSITINCALO_H

#include "CaloIdentifier/CaloCell_ID.h"

/*****************************************
 * Energy deposit in one calo layer
Author: Gustavo Ordenez
Author: adapted by Ketevi A. Assamagan
Date: Feb 28 2007
******************************************/

/** @class DepositInCalo
    @brief class describing the measured energy loss associated to muons
           on the detector level, that is per calo layer.
    @author K. Assamagan, G. Ordonez
*/

class DepositInCalo  {
 public:

  /** constructors */
  DepositInCalo() :  m_subCaloId(CaloCell_ID::PreSamplerB),
                     m_energyDeposited(0.0),
                     m_muonEnergyLoss(0.0),
                     m_etDeposited(0.0) {};

  /** constructors */
  DepositInCalo( CaloCell_ID::CaloSample subCaloId,
                 float energyDeposited, float muonEnergyLoss, float etDeposited=0.0 )
    : m_subCaloId(subCaloId),
      m_energyDeposited(energyDeposited),
      m_muonEnergyLoss(muonEnergyLoss),
      m_etDeposited(etDeposited)
  {}

  /** copy constructor */
  DepositInCalo (const DepositInCalo&);

  /** assignment operator */
  DepositInCalo &operator= (const DepositInCalo &);

  /** destructor */
  virtual ~DepositInCalo() {}

  /** printing for debugging */
  void print() const;

  /** Calorimeter detailed information */
  CaloCell_ID::CaloSample subCaloId() const { return m_subCaloId; }
  /** Calorimeter detailed information - Energy Deposited*/
  double energyDeposited() const { return m_energyDeposited; }
  /** Calorimeter detailed information - Muon Energy Loss */
  double muonEnergyLoss() const {return m_muonEnergyLoss;}
  /** Calorimeter detailed information - et Deposited*/
  double etDeposited() const { return m_etDeposited; }

 private:

  /** calorimeter sampling identifier */
  CaloCell_ID::CaloSample m_subCaloId;

  /** energy Desposited */
  float m_energyDeposited;

  /** energy Loss of the muons computed using the extrapolator */
  float m_muonEnergyLoss;

  /** Et of the energy deposit */
  float m_etDeposited;

};

#endif //


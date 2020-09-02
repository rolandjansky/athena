/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// HIClusterMaker.h

#ifndef __HIJETREC_HICLUSTERMAKER_H__
#define __HIJETREC_HICLUSTERMAKER_H__

////////////////////////////////////////////////////////////////////////////////
///
/// \class HIClusterMaker
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Jan, 2015
///
/// \brief Algorithm to build HI-style clusters, which are essentially towers.
/// Cluster energy is sum of cell energies x geometric weights
/// eta/phi are (energy x geometric weight)-averaged positions.
/// eta0/phi0 are original tower coordinates.
/// If cluster energy is below m_E_min_mom ent, eta=eta0, phi=phi0.
///
////////////////////////////////////////////////////////////////////////////////


#include <AthenaBaseComps/AthAlgorithm.h>
//Ideally use forward class decl. for CaloClusterContainer
//Cannot because this class is defined via typedef
#include <xAODCaloEvent/CaloClusterContainer.h>
#include "NavFourMom/INavigable4MomentumCollection.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class CaloCellContainer;

class HIClusterMaker : public AthAlgorithm
{

public:

  HIClusterMaker(const std::string& name, ISvcLocator* pSvcLocator);
  ~HIClusterMaker() {};

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

  //Simple helper to dump clusters for debugging
  StatusCode dumpClusters(xAOD::CaloClusterContainer* clusColl);
private:
  /// \brief Name of input CaloTowerContainer, e.g CmbTower
  SG::ReadHandleKey<INavigable4MomentumCollection>   m_towerContainerKey { this, "InputTowerKey"     , "CombinedTower"   , "InputTowerKey"};
  /// \brief Name of input CaloCellContainer, e.g. AllCalo
  SG::ReadHandleKey<CaloCellContainer>            m_cellContainerKey  { this, "CaloCellContainerKey"      , "AllCalo"         , "InputCellKey" };
  /// \brief Name of output CaloClusterContainer, e.g. HIClusters
  SG::WriteHandleKey<xAOD::CaloClusterContainer>  m_outputKey     { this, "OutputContainerKey"     , "PseudoJet"       , "Read version of output Container Key"};
  /// \brief For clusters w/ E less than this, set their eta/phi to tower eta/phi
  Gaudi::Property< float > m_EminMoment { this, "MinimumEnergyForMoments", 50., "> E, cluster given tower coordinates" };

};
#endif

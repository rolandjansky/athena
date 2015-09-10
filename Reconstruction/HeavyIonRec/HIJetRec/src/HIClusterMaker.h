/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

class HIClusterMaker : public AthAlgorithm
{

public:

  HIClusterMaker(const std::string& name, ISvcLocator* pSvcLocator);
  ~HIClusterMaker() {};

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

  //Simple helper to dump clusters for debugging
  StatusCode DumpClusters(xAOD::CaloClusterContainer* clusColl);
private:

  std::string m_tower_container_key; /// \brief Name of input CaloTowerContainer, e.g CmbTower
  std::string m_cell_container_key;/// \brief Name of input CaloCellContainer, e.g. AllCalo
  std::string m_output_key; /// \brief Name of output CaloClusterContainer, e.g. HIClusters
  float m_E_min_moment; /// \brief For clusters w/ E less than this, set their eta/phi to tower eta/phi

};
#endif

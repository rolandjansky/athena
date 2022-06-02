/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTCALOSIM_CELLINFOCONTAINERCONDALG_H
#define FASTCALOSIM_CELLINFOCONTAINERCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "FastCaloSim/CellInfoContainer.h"

/**
 * @class CellInfoContainerCondAlg
 *
 * @brief Condition Algorithm for building CellInfoContainer from CaloDetDescrManager
 *
 **/

class CellInfoContainerCondAlg final : public AthAlgorithm
{
 public:
  using AthAlgorithm::AthAlgorithm;
  virtual ~CellInfoContainerCondAlg() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

 private:
  typedef cellinfo_map::cellinfo cellinfo;
  typedef cellinfo_map::cellinfo_vec cellinfo_vec;

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG Key for CaloDetDescrManager in the Condition Store" };

  SG::WriteCondHandleKey<CellInfoContainer> m_cellInfoContKey { this
      , "CellInfoContainer"
      , "CellInfoContainer"
      , "SG Key for CellInfoContainer in the Condition Store" };

  // Helper functions
  void init_all_maps(const CaloDetDescrManager* caloDDM
		     , CellInfoContainer* cont) const;
  void init_cell(cellinfo_map& map
		 ,const CaloDetDescrElement* theDDE) const;
  void init_volume(cellinfo_map& map) const;
  void find_phi0(const CaloDetDescrManager* caloDDM
		 , CellInfoContainer* cont) const;
  void findCells(const CaloDetDescrManager* caloDDM
                 , cellinfo_vec & cell_vec
		 , double eta_min
		 , double eta_max
		 , double phi_min
		 , double phi_max
		 , int layer) const;
};

#endif

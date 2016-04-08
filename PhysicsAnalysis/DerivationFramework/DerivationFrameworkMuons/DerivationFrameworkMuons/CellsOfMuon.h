/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ======================================================================
// Cells of Muon
// dongliang.zhang@cern.ch
// Given an xAOD::Muon and an CaloCellContainer, checks
// which calo cells are associated with muon.
// Sets a mask accordingly. Intended for thinning use.
// ======================================================================
#pragma once

#include "xAODMuon/Muon.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloClusterCellLink.h"

namespace DerivationFramework {
  struct CellsOfMuon{ 
    void select(const xAOD::Muon* mu, std::vector<bool> &mask) {
      auto cluster = mu->cluster();
      if(cluster){
        const CaloClusterCellLink* links=cluster->getCellLinks();
        unsigned int i(0);
        CaloClusterCellLink::const_iterator c=links->begin();
        CaloClusterCellLink::const_iterator e=links->end();
        for(; c!=e; ++c, i++) mask[c.index()] = true;
      }
    }
  };
}

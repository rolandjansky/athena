/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief implementation of photon shot variable calculation 
 * 
 * @author Will Davey <will.davey@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch>
 * @author Stephanie Yuen <stephanie.yuen@cern.ch> 
 */

#ifndef TAURECTOOLS_TAUSHOTVARIABLEHELPERS_H
#define TAURECTOOLS_TAUSHOTVARIABLEHELPERS_H

#include "xAODPFlow/PFO.h"
#include "CaloInterface/IHadronicCalibrationTool.h"
#include "CaloIdentifier/LArNeighbours.h"

#include "AsgMessaging/MessageCheck.h"
#include "GaudiKernel/ToolHandle.h"

class CaloCell_ID;

namespace TauShotVariableHelpers {
  
  ANA_MSG_HEADER(msgHelperFunction)

  /** @brief Obtain the required neighbour cell */
  const CaloCell* getNeighbour(const CaloCell* cell,
                               const CaloClusterCellLink& links,
                               const CaloCell_ID* calo_id,
                               const LArNeighbours::neighbourOption& option);

  /** @brief Get cell block with (currently) 2 x 5 cells in correct order for variable calculations */
  std::vector<std::vector<const CaloCell*>> getCellBlock(const xAOD::PFO& shot,
                                                         const CaloCell_ID* calo_id);

  /** @brief pt in a window of (currently) 2 x windowSize cells */
  float ptWindow(const std::vector<std::vector<const CaloCell*>>& shotCells, 
                 int windowSize, 
                 const ToolHandle<IHadronicCalibrationTool>& caloWeightTool);

}

#endif // TAURECTOOLS_TAUSHOTVARIABLEHELPERS_H

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOUTILS_EGAMMAMIDDLESHAPE_H
#define EGAMMACALOUTILS_EGAMMAMIDDLESHAPE_H

/// @class egammaMiddleShape
/// @brief   EM cluster shower shape calculations in 2nd ECAL sampling
/// Calculate the width in the strip layer around the eta,phi of
/// the hottest cell in the **middle layer**.
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
///
///

class CaloCellContainer;
class LArEM_ID;
class CaloDetDescrManager;

#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "GaudiKernel/StatusCode.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

struct egammaMiddleShape
{
  struct Info
  {
    double e233 = 0;
    double e235 = 0;
    double e237 = 0;
    double e255 = 0;
    double e277 = 0;
    double etaw = 0;
    double phiw = 0;
    double width = 0;
    double poscs2 = 0;
  };

  static StatusCode execute(const xAOD::CaloCluster& cluster,
                     const CaloDetDescrManager& cmgr,
                     const CaloCellContainer& cell_container,
                     Info& info,
                     bool doRetaOnly = false) ;
};

#endif

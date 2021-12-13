/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOUTILS_EGAMMAPRESAMPLERSHAPE_H
#define EGAMMACALOUTILS_EGAMMAPRESAMPLERSHAPE_H

/// @class egammaPreSamplerShape
/// @brief   EM cluster shower shape calculations in presampler of ECAL.
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos

#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "GaudiKernel/StatusCode.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
class CaloCellContainer;
class CaloDetDescrManager;

struct egammaPreSamplerShape
{

  struct Info
  {
  public:
    double e011 = 0;
    double e033 = 0;
  };

  static StatusCode execute(const xAOD::CaloCluster& cluster,
                     const CaloDetDescrManager& cmgr,
                     const CaloCellContainer& cell_container,
                     Info& info) ;
};

#endif

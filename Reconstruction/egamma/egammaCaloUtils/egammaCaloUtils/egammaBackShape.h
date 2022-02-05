/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOUTILS_EGAMMABACKSHAPE_H
#define EGAMMACALOUTILS_EGAMMABACKSHAPE_H

/// @class egammaBackShape
/// @brief     EM cluster shower shape calculations in 3rd sampling.
///  Calculate the width in the back layer around the eta,phi of
///  the hottest cell in the **middle layer**.
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
///
///

class CaloCellContainer;
class CaloDetDescrManager;

#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "GaudiKernel/StatusCode.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

struct egammaBackShape
{

  struct Info
  {
    double f3 = 0.;
    double f3core = 0.;
    double e333 = 0.;
    double e335 = 0.;
    double e355 = 0.;
    double e337 = 0.;
    double e377 = 0.;
  };

  static StatusCode execute(const xAOD::CaloCluster& cluster,
                     const CaloDetDescrManager& cmgr,
                     const CaloCellContainer& cell_container,
                     Info& info,
                     bool ExecOtherVariables = true) ;
};

#endif

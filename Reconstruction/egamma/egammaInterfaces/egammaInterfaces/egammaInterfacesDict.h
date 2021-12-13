/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_EGAMMAINTERFACESDICT_H
#define EGAMMAINTERFACES_EGAMMAINTERFACESDICT_H
/**
 * @file egammaInterfacesDict.h
 *
 * @brief Header file for Reflex dictionary generation
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 */

#include "egammaInterfaces/IEMClusterTool.h"
#include "egammaInterfaces/IEMConversionBuilder.h"
#include "egammaInterfaces/IEMFourMomBuilder.h"
#include "egammaInterfaces/IEMShowerBuilder.h"
#include "egammaInterfaces/IegammaBaseTool.h"
#include "egammaInterfaces/IegammaCheckEnergyDepositTool.h"
#include "egammaInterfaces/IegammaIso.h"
#include "egammaInterfaces/IegammaShowerShape.h"
#include "egammaInterfaces/IegammaSwTool.h"
#include "egammaInterfaces/IegammaOQFlagsBuilder.h"
#ifndef XAOD_ANALYSIS
#   include "egammaInterfaces/ICaloCluster_OnTrackBuilder.h"
#   include "egammaInterfaces/IEMExtrapolationTools.h"
#   include "egammaInterfaces/IEMTrackMatchBuilder.h"
#   include "egammaInterfaces/IegammaTrkRefitterTool.h"
#endif // not XAOD_ANALYSIS

#endif // EGAMMAINTERFACES_EGAMMAINTERFACESDICT_H

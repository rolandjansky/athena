/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TileConditions/TileInfoLoader.h"
#include "TileConditions/TileCablingSvc.h"
#include "TileConditions/TileCondIdTransforms.h"
#include "TileConditions/TileBadChanTool.h"
#include "TileConditions/TileCondToolOfc.h"
#include "TileConditions/TileCondToolOfcCool.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileExpertToolEmscale.h"
#include "TileConditions/TileCondToolIntegrator.h"
#include "TileConditions/TileCondToolMuID.h"
#include "TileConditions/TileCondToolTiming.h"
#include "TileConditions/TileCondToolPulseShape.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileConditions/TileCondToolNoiseRawChn.h"
#include "TileConditions/TileCondProxyWrapper.h"
#include "TileConditions/TileCondDCS_Data.h"
#include "TileConditions/TileCondToolAutoCr.h"
#include "TileConditions/TileCondToolTMDB.h"
#include "TileConditions/TileCondToolDspThreshold.h"
#include "../TileDCSTool.h"
#include "../TileCalibCondAlg.h"
#include "../TileBadChannelsCondAlg.h"
#include "../TileEMScaleCondAlg.h"
#include "../TileDCSCondAlg.h"
#include "../TileCondAlg.h"

DECLARE_COMPONENT( TileInfoLoader )
DECLARE_COMPONENT( TileCablingSvc )
DECLARE_COMPONENT( TileCondIdTransforms )
DECLARE_COMPONENT( TileBadChanTool )
DECLARE_COMPONENT( TileCondToolOfc )
DECLARE_COMPONENT( TileCondToolOfcCool )
DECLARE_COMPONENT( TileCondToolEmscale )
DECLARE_COMPONENT( TileExpertToolEmscale )
DECLARE_COMPONENT( TileCondToolIntegrator )
DECLARE_COMPONENT( TileCondToolMuID )
DECLARE_COMPONENT( TileCondToolTiming )
DECLARE_COMPONENT( TileCondToolPulseShape )
DECLARE_COMPONENT( TileCondToolNoiseSample )
DECLARE_COMPONENT( TileCondToolAutoCr )
DECLARE_COMPONENT( TileCondToolNoiseRawChn )
DECLARE_COMPONENT( TileCondToolTMDB )
DECLARE_COMPONENT( TileCondToolDspThreshold )
DECLARE_COMPONENT( TileCondProxyCoolFlt )
DECLARE_COMPONENT( TileCondProxyCoolBch )
DECLARE_COMPONENT( TileCondProxyCoolOfc )
DECLARE_COMPONENT( TileCondProxyFileFlt )
DECLARE_COMPONENT( TileCondProxyFileBch )
DECLARE_COMPONENT( TileCalibFltCondAlg )
DECLARE_COMPONENT( TileCalibOfcCondAlg )
DECLARE_COMPONENT( TileBadChannelsCondAlg )
DECLARE_COMPONENT( TileEMScaleCondAlg )
DECLARE_COMPONENT( TileDCSCondAlg )
DECLARE_COMPONENT( TileDCSTool )
DECLARE_COMPONENT( TileSamplingFractionCondAlg )

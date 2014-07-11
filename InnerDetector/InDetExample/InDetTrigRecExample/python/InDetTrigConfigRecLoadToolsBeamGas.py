# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Beam gas specific tools setup
"""

__author__ = "J. Masik"
__version__= "$Revision: 1.1 $"
__doc__    = "InDetTrigConfigrecLoadToolsBeamGas"


from AthenaCommon.AppMgr import ToolSvc
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPrdAssociationTool

from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCutsBeamGas

if InDetTrigFlags.doAmbiSolving():
  InDetTrigAmbiTrackSelectionToolBeamGas = \
      InDet__InDetAmbiTrackSelectionTool(name            = 'InDetTrigAmbiTrackSelectionToolBeamGas',
                                         AssociationTool = InDetTrigPrdAssociationTool,
                                         minHits         = InDetTrigCutValuesBeamGas.minClusters(),
                                         minNotShared    = InDetTrigCutValuesBeamGas.minSiNotShared(),
                                         maxShared       = InDetTrigCutValuesBeamGas.maxShared(),
                                         minTRTHits      = 0 ) # used for Si only tracking !!!
  ToolSvc += InDetTrigAmbiTrackSelectionToolBeamGas


  if (InDetTrigFlags.doPrintConfigurables()):
    print      InDetTrigAmbiTrackSelectionToolBeamGas


if InDetTrigFlags.doSiSPSeededTrackFinder():

  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPrdAssociationTool
  from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import \
       InDet__SiSpacePointsSeedMaker_BeamGas
  
  InDetTrigSiSpacePointsSeedMakerBeamGas = \
                                         InDet__SiSpacePointsSeedMaker_BeamGas(
    name                   = 'InDetTrigSiSpacePointsSeedMaker_'+'beamgas',
    pTmin                  = EFIDTrackingCutsBeamGas.minPT(),
    maxdImpact             = EFIDTrackingCutsBeamGas.maxPrimaryImpactBeamGas(),
    maxZ                   = EFIDTrackingCutsBeamGas.maxZImpact(),
    minZ                   = -EFIDTrackingCutsBeamGas.maxZImpact(),
    SpacePointsPixelName   = 'SCT_TrigSpacePoints',
    SpacePointsSCTName     = 'PixelTrigSpacePoints',
    #SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
    UseAssociationTool     = True,
    AssociationTool        = InDetTrigPrdAssociationTool,
    radMax                 = EFIDTrackingCutsBeamGas.radMax()
    )
  
  ToolSvc += InDetTrigSiSpacePointsSeedMakerBeamGas
  if (InDetTrigFlags.doPrintConfigurables()):
    print InDetTrigSiSpacePointsSeedMakerBeamGas
  

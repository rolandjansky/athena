# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
TrackingTools configuration for ISF
Elmar Ritsch, 24/06/2015
"""

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
        getService,getServiceClone,getAlgorithm,getAlgorithmClone

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.DetFlags import DetFlags

from ISF_Config.ISF_jobProperties import ISF_Flags # IMPORTANT: Flags must be set before tools are retrieved

def getCaloEntryTrkExtrapolator(name="ISF_CaloEntryTrkExtrapolator", **kwargs):
    kwargs.setdefault( 'TrackingVolumeName'  , 'InDet::Containers::EntryVolume'       )
    kwargs.setdefault( 'TrackingGeometrySvc' , getService('AtlasTrackingGeometrySvc') )
    atlasExtrapolator = getPublicTool('AtlasExtrapolator')
    atlasExtrapolator.ApplyMaterialEffects=False
    kwargs.setdefault( 'Extrapolator'        , atlasExtrapolator                      )

    from ISF_TrackingTools.ISF_TrackingToolsConf import ISF__TrkExtrapolator
    return ISF__TrkExtrapolator(name, **kwargs)

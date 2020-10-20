# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
TrackingTools configuration for ISF
Elmar Ritsch, 24/06/2015
"""

from AthenaCommon.CfgGetter import getPublicTool, getService

def getCaloEntryTrkExtrapolator(name="ISF_CaloEntryTrkExtrapolator", **kwargs):
    kwargs.setdefault( 'TrackingVolumeName'  , 'InDet::Containers::EntryVolume'       )
    atlasExtrapolator = getPublicTool('AtlasExtrapolator')
    atlasExtrapolator.ApplyMaterialEffects=False
    kwargs.setdefault( 'Extrapolator'        , atlasExtrapolator                      )

    from ISF_TrackingTools.ISF_TrackingToolsConf import ISF__TrkExtrapolator
    return ISF__TrkExtrapolator(name, **kwargs)

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


def getActsFatrasSimTool(name="ISF_ActsFatrasSimTool", **kwargs):
#    kwargs.setdefault("TrackingGeometryTool" , getPublicTool('ActsTrackingGeometryTool'))
#    kwargs.setdefault("ActsExtrapolationTool" ,  getPublicTool('ActsExtrapolationTool'))

    from ISF_ActsTools.ISF_ActsToolsConf import ISF__ActsFatrasSimTool
    return ISF__ActsFatrasSimTool(name, **kwargs)


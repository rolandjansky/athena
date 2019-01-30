# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getAthRNGSvc(name='AthRNGSvc', **kwargs):
    return CfgMgr.AthRNGSvc(name, **kwargs)

def getAthMixMaxRNGSvc(name='AthMixMaxRNGSvc', **kwargs):
    kwargs.setdefault('EngineType', 'MixMax')
    return CfgMgr.AthRNGSvc(name, **kwargs)

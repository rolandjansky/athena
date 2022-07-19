# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getGEN_AOD2xAOD(name="GEN_AOD2xAOD", **kwargs):
    writeInTimePileUpTruth=False
    try:
        from PyUtils.MetaReaderPeekerFull import metadata
        if metadata['/Digitization/Parameters']['digiSteeringConf'] in ['StandardPileUpToolsAlg', 'StandardInTimeOnlyTruthPileUpToolsAlg', 'StandardInTimeOnlyGeantinoTruthPileUpToolsAlg']:
            writeInTimePileUpTruth = True
    except Exception:
        pass
    kwargs.setdefault('WriteInTimePileUpTruth',  writeInTimePileUpTruth)
    return CfgMgr.xAODMaker__xAODTruthCnvAlg(name, **kwargs)

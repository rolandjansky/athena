from AthenaCommon import CfgMgr

def getGEN_AOD2xAOD(name="GEN_AOD2xAOD", **kwargs):
    writeInTimePileUpTruth=False
    try:
        from PyUtils.MetaReaderPeekerFull import metadata
        if metadata['/Digitization/Parameters']['digiSteeringConf'] == 'StandardInTimeOnlyGeantinoTruthPileUpToolsAlg':
            writeInTimePileUpTruth = True
    except:
        pass
    kwargs.setdefault('WriteInTimePileUpTruth',  writeInTimePileUpTruth)
    return CfgMgr.xAODMaker__xAODTruthCnvAlg(name, **kwargs)

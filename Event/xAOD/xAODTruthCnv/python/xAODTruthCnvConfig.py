from AthenaCommon import CfgMgr

def getGEN_AOD2xAOD(name="GEN_AOD2xAOD", **kwargs):
    writeInTimePileUpTruth=False
    try:
        from RecExConfig.InputFilePeeker import inputFileSummary
        if inputFileSummary['metadata']['/Digitization/Parameters']['digiSteeringConf'] == 'StandardInTimeOnlyGeantinoTruthPileUpToolsAlg':
            writeInTimePileUpTruth=True
    except:
        pass
    kwargs.setdefault('WriteInTimePileUpTruth',  writeInTimePileUpTruth)
    return CfgMgr.xAODMaker__xAODTruthCnvAlg(name, **kwargs)

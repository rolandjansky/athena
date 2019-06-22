# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def LArADC2MeVCondAlgDefault(configFlags):
    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg 
    from LArRecUtils.LArRecUtilsConf import LArADC2MeVCondAlg 
    
    result=ComponentAccumulator()
    result.merge(LArOnOffIdMappingCfg(configFlags))

    theADC2MeVCondAlg=LArADC2MeVCondAlg()

    isMC=configFlags.Input.isMC
    
    
    if isMC:
        if not configFlags.LAr.hasMphys():
            theADC2MeVCondAlg.LArMphysOverMcalKey="" #No MphysOVerMcal
        else:
            theADC2MeVCondAlg.LArMphysOverMcalKey="LArMphysOverMcalSym"

        if not larCondFlags.hasHVCorr():
            theADC2MeVCondAlg.LArHVScaleCorrKey=""
      
        theADC2MeVCondAlg.LAruA2MeVKey="LAruA2MeVSym"
        theADC2MeVCondAlg.LArDAC2uAKey="LArDAC2uASym"
        theADC2MeVCondAlg.LArRampKey="LArRampSym"


        theADC2MeVCondAlg.UseFEBGainTresholds=False
    else: # not MC:
        from LArRecUtils.LArFebConfigCondAlgDefault import LArFebConfigCondAlgDefault
        LArFebConfigCondAlgDefault()


    result.addCondAlg(theADC2MeVCondAlg,primary=True)
    return result

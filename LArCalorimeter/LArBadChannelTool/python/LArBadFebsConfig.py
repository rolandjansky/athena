# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
LArBadFebCondAlg=CompFactory.LArBadFebCondAlg
from IOVDbSvc.IOVDbSvcConfig import addFolders

def LArKnownBadFebCfg(configFlags, tag=""):
    result=ComponentAccumulator()

    if configFlags.GeoModel.Run == "RUN1":
        foldername=""
    else:    
        if configFlags.Common.isOnline or configFlags.Input.isMC:
            foldername="/LAR/BadChannels/KnownBADFEBs"
        else:
            foldername="/LAR/BadChannelsOfl/KnownBADFEBs"
        pass
 
        if configFlags.Common.isOnline: 
            dbname="LAR"
        else:
            dbname="LAR_OFL"
        pass
    pass    

    result.merge(addFolders(configFlags,foldername + tag,detDb=dbname,className="AthenaAttributeList"))
    result.addCondAlgo(LArBadFebCondAlg("LArKonwnBadFebCondAlg",ReadKey=foldername,WriteKey="LArKnownBadFEBs"))
    return result

def LArKnownMNBFebCfg(configFlags, tag=""):
    result=ComponentAccumulator()

    if configFlags.GeoModel.Run == "RUN1":
        foldername=""
    else:    
        if configFlags.Common.isOnline or configFlags.Input.isMC:
            foldername="/LAR/BadChannels/KnownMNBFEBs"
        else:
            foldername="/LAR/BadChannelsOfl/KnownMNBFEBs"
        pass
 
        if configFlags.Common.isOnline: 
            dbname="LAR"
        else:
            dbname="LAR_OFL"
        pass
    pass    

    result.merge(addFolders(configFlags,foldername + tag,detDb=dbname,className="AthenaAttributeList"))
    result.addCondAlgo(LArBadFebCondAlg("LArKonwnMNBFebCondAlg",ReadKey=foldername,WriteKey="LArKnownMNBFEBs"))
    return result



if __name__=="__main__":
    
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    log.setLevel(DEBUG)

    ConfigFlags.Input.isMC = False
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data17_13TeV.00330470.physics_Main.daq.RAW._lb0310._SFO-1._0001.data"]
    ConfigFlags.lock()

    cfg=ComponentAccumulator()
    
    cfg.merge(LArKnownBadFebCfg(ConfigFlags))
    cfg.merge(LArKnownMNBFebCfg(ConfigFlags))
    f=open("LArBadFebCondAlgos.pkl","wb")
    cfg.store(f)
    f.close()
    

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelCondAlg, LArBadFebCondAlg, LArBadChannelMasker
from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
from IOVDbSvc.IOVDbSvcConfig import addFolders

def LArBadChannelCfg(configFlags):
    result=ComponentAccumulator()

    result.addConfig(LArOnOffIdMappingCfg,configFlags)
    
    if configFlags.get("global.isOnline") or configFlags.get("global.isMC"):
        foldername="/LAR/BadChannels/BadChannels"
    else:
        foldername="/LAR/BadChannelsOfl/BadChannels"
        pass
    
    if configFlags.get("global.isOnline"): 
        dbname="LAR"
    else:
        dbname="LAR_OFL"

    result.addConfig(addFolders,configFlags,foldername,detDb=dbname,className="CondAttrListCollection")

    result.addCondAlgo(LArBadChannelCondAlg(ReadKey=foldername))
    return result


def LArBadFebCfg(configFlags):
    result=ComponentAccumulator()

    if configFlags.get("global.isOnline") or configFlags.get("global.isMC"):
        foldername="/LAR/BadChannels/MissingFEBs"
    else:
        foldername="/LAR/BadChannelsOfl/MissingFEBs"
        pass

    if configFlags.get("global.isOnline"): 
        dbname="LAR"
    else:
        dbname="LAR_OFL"

    result.addConfig(addFolders,configFlags,foldername,detDb=dbname,className="AthenaAttributeList")

    result.addCondAlgo(LArBadFebCondAlg(ReadKey=foldername))
    return result


def LArBadChannelMaskerCfg(configFlags,problemsToMask,doMasking=True,
                           ToolName="LArBadChannelMasker"):
     result=ComponentAccumulator()
     result.addConfig(LArBadChannelCfg,configFlags)
     
     bcMasker=LArBadChannelMasker(ToolName,ProblemsToMask=problemsToMask,
                                  DoMasking=doMasking)

     return result,bcMasker
                    


if __name__=="__main__":
    
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)

    ConfigFlags.set("global.isMC",False)
    ConfigFlags.set("global.InputFiles",["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data17_13TeV.00330470.physics_Main.daq.RAW._lb0310._SFO-1._0001.data"])
    ConfigFlags.lock()

    cfg=ComponentAccumulator()
    
    cfg.addConfig(LArBadChannelCfg,ConfigFlags)
    cfg.addConfig(LArBadFebCfg,ConfigFlags)
    cfg.addConfig(LArBadChannelMaskerCfg,ConfigFlags,["allDead",])
    f=open("LArBCCondAlgos.pkl","w")
    cfg.store(f)
    f.close()
    

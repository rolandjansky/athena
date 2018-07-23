from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelCondAlg, LArBadFebCondAlg, LArBadChannelMasker
from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
from IOVDbSvc.IOVDbSvcConfig import addFolders

def LArBadChannelCfg(configFlags):

    result=LArOnOffIdMappingCfg(configFlags)[0]
    
    if configFlags.get("global.isOnline") or configFlags.get("global.isMC"):
        foldername="/LAR/BadChannels/BadChannels"
    else:
        foldername="/LAR/BadChannelsOfl/BadChannels"
        pass
    
    if configFlags.get("global.isOnline"): 
        dbname="LAR"
    else:
        dbname="LAR_OFL"

    result.merge(addFolders(configFlags,foldername,detDb=dbname,className="CondAttrListCollection")[0])
    
    theLArBadChannelCondAlgo=LArBadChannelCondAlg(ReadKey=foldername)
    result.addCondAlgo(theLArBadChannelCondAlgo)
    return result,None


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

    result.merge(addFolders(configFlags,foldername,detDb=dbname,className="AthenaAttributeList")[0])
    result.addCondAlgo(LArBadFebCondAlg(ReadKey=foldername))
    return result,None


def LArBadChannelMaskerCfg(configFlags,problemsToMask,doMasking=True,ToolName="LArBadChannelMasker"):
    result=LArBadChannelCfg(configFlags)[0]
     
    bcMasker=LArBadChannelMasker(ToolName,ProblemsToMask=problemsToMask, DoMasking=doMasking)
    return result,bcMasker
                    


if __name__=="__main__":
    
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    log.setLevel(DEBUG)

    ConfigFlags.set("global.isMC",False)
    ConfigFlags.set("global.InputFiles",["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data17_13TeV.00330470.physics_Main.daq.RAW._lb0310._SFO-1._0001.data"])
    ConfigFlags.lock()

    cfg=ComponentAccumulator()
    
    cfg.merge(LArBadChannelCfg(ConfigFlags))
    cfg.merge(LArBadFebCfg(ConfigFlags))
    acc,privTool=LArBadChannelMaskerCfg(ConfigFlags,["allDead",])
    cfg.merge(acc)
    f=open("LArBCCondAlgos.pkl","w")
    cfg.store(f)
    f.close()
    

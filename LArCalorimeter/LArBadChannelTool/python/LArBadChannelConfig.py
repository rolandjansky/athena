from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelCondAlg, LArBadFebCondAlg, LArBadChannelMasker
from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
from IOVDbSvc.IOVDbSvcConfig import addFolders

def LArBadChannelCfg(configFlags):

    result=LArOnOffIdMappingCfg(configFlags)
    
    if configFlags.Common.isOnline or configFlags.Input.isMC:
        foldername="/LAR/BadChannels/BadChannels"
    else:
        foldername="/LAR/BadChannelsOfl/BadChannels"
        pass
    
    if configFlags.Common.isOnline: 
        dbname="LAR"
    else:
        dbname="LAR_OFL"

    result.merge(addFolders(configFlags,foldername,detDb=dbname,className="CondAttrListCollection"))
    
    theLArBadChannelCondAlgo=LArBadChannelCondAlg(ReadKey=foldername)
    result.addCondAlgo(theLArBadChannelCondAlgo)
    return result


def LArBadFebCfg(configFlags):
    result=ComponentAccumulator()

    if configFlags.Common.isOnline or configFlags.Input.isMC:
        foldername="/LAR/BadChannels/MissingFEBs"
    else:
        foldername="/LAR/BadChannelsOfl/MissingFEBs"
        pass

    if configFlags.Common.isOnline: 
        dbname="LAR"
    else:
        dbname="LAR_OFL"

    result.merge(addFolders(configFlags,foldername,detDb=dbname,className="AthenaAttributeList"))
    result.addCondAlgo(LArBadFebCondAlg(ReadKey=foldername))
    return result


def LArBadChannelMaskerCfg(configFlags,problemsToMask,doMasking=True,ToolName="LArBadChannelMasker"):
    result=LArBadChannelCfg(configFlags)
     
    bcMasker=LArBadChannelMasker(ToolName,ProblemsToMask=problemsToMask, DoMasking=doMasking)
    result.setPrivateTools(bcMasker)
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
    
    cfg.merge(LArBadChannelCfg(ConfigFlags))
    cfg.merge(LArBadFebCfg(ConfigFlags))
    acc=LArBadChannelMaskerCfg(ConfigFlags,["allDead",])
    masker=acc.popPrivateTools()
    cfg.merge(acc)
    f=open("LArBCCondAlgos.pkl","w")
    cfg.store(f)
    f.close()
    

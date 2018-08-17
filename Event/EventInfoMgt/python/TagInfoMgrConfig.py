from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator, ConfigurationError
import os
import collections


def TagInfoMgrCfg(configFlags,tagValuePairs=[]):

    #Sanity check:
    if not isinstance(tagValuePairs,collections.Sequence) or len(tagValuePairs)%2!=0:
        raise ConfigurationError("Parameter extraTagValuePairs is supposed to be an even-numbered list of strings")

    result=ComponentAccumulator()

    from EventInfoMgtConf import TagInfoMgr
    from SGComps.SGCompsConf import ProxyProviderSvc
    from GaudiSvc.GaudiSvcConf import EvtPersistencySvc

    #Build project-version string for the TagInfoMgr 
    project = os.getenv('AtlasProject',"Unknown")
    version = os.getenv('AtlasVersion',"Unknown")     
    atlasRelease=project+"-"+version
    
    releasetag=["AtlasRelease", atlasRelease ]

    tagInfoMgr=TagInfoMgr(ExtraTagValuePairs = releasetag+list(tagValuePairs))
    result.addService(tagInfoMgr)
    
    #Add to EventPersistencySvc 
    result.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[tagInfoMgr.getName(),]))
    
    #Add to ProxyProviderSvc
    result.addService(ProxyProviderSvc(ProviderNames=[tagInfoMgr.getName(),]))
                       
    return result,tagInfoMgr
    

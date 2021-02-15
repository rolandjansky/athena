# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator, ConfigurationError
import os


def TagInfoMgrCfg(configFlags,tagValuePairs={}):

    #Sanity check:
    if not isinstance(tagValuePairs,dict):
        raise ConfigurationError("Parameter extraTagValuePairs is supposed to be a dictionary")

    #Build project-version string for the TagInfoMgr 
    project = os.getenv('AtlasProject',"Unknown")
    version = os.getenv('AtlasVersion',"Unknown")     
    atlasRelease=project+"-"+version

    tagValuePairs.update({"AtlasRelease" : atlasRelease})

    TagInfoMgr=CompFactory.TagInfoMgr
    tagInfoMgr=TagInfoMgr(ExtraTagValuePairs = tagValuePairs)

    result=ComponentAccumulator()
    result.addService(tagInfoMgr)
    return result,tagInfoMgr
    
if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True    

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc, tagInfoMgr = TagInfoMgrCfg( ConfigFlags, {"SomeKey": "SomeValue"} )
    acc2,_ =  TagInfoMgrCfg( ConfigFlags, {"OtherKey":"OtherValue", "SomeKey": "SomeValue"} )
    acc.merge(acc2)

    assert "SomeKey" in acc.getService("TagInfoMgr").ExtraTagValuePairs
    assert "OtherKey" in acc.getService("TagInfoMgr").ExtraTagValuePairs
    acc.store( open( "test.pkl", "wb" ) )
    print("All OK")

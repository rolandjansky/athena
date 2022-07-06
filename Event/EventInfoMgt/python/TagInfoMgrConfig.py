# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator, ConfigurationError
from AthenaConfiguration.ComponentFactory import CompFactory
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

    result=ComponentAccumulator()
    result.addService(CompFactory.TagInfoMgr(ExtraTagValuePairs = tagValuePairs), primary=True)
    return result


if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RDO_RUN2
    ConfigFlags.lock()

    acc = TagInfoMgrCfg( ConfigFlags, {"SomeKey": "SomeValue"} )
    acc2 =  TagInfoMgrCfg( ConfigFlags, {"OtherKey":"OtherValue", "SomeKey": "SomeValue"} )
    acc.merge(acc2)

    assert "SomeKey" in acc.getService("TagInfoMgr").ExtraTagValuePairs
    assert "OtherKey" in acc.getService("TagInfoMgr").ExtraTagValuePairs
    acc.store( open( "test.pkl", "wb" ) )
    print("All OK")

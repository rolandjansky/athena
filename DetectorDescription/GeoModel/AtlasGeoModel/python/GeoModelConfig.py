# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Configurable import Configurable
from AthenaCommon import Logging

def GeoModelCfg(configFlags):
    version=configFlags.GeoModel.AtlasVersion

    from AthenaCommon.AppMgr import release_metadata
    rel_metadata = release_metadata()
    relversion = rel_metadata['release'].split('.')
    if len(relversion) < 3:
        relversion = rel_metadata['base release'].split('.')


    result=ComponentAccumulator()
    GeoModelSvc=CompFactory.GeoModelSvc
    gms=GeoModelSvc(AtlasVersion=version,
                    SupportedGeometry = int(relversion[0]))
    if configFlags.Detector.Simulate:
        ## Protects GeoModelSvc in the simulation from the AlignCallbacks
        gms.AlignCallbacks = False
    result.addService(gms, primary=True, create=True)


    #Get DetDescrCnvSvc (for identifier dictionaries (identifier helpers)
    from DetDescrCnvSvc.DetDescrCnvSvcConfig import DetDescrCnvSvcCfg
    result.merge(DetDescrCnvSvcCfg(configFlags))

    from EventInfoMgt.TagInfoMgrConfig import TagInfoMgrCfg	
    tim_ca,tagInfoMgr=TagInfoMgrCfg(configFlags)
    result.addService(tagInfoMgr)
    result.merge(tim_ca)
    #TagInfoMgr used by GeoModelSvc but no ServiceHandle. Relies on string-name

    return result



if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    Configurable.configurableRun3Behavior=1

    acc = GeoModelCfg( ConfigFlags )
    acc.store( open( "test.pkl", "wb" ) )
    Logging.log.info("All OK")

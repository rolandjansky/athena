from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=1

def GeoModelCfg(configFlags):
    version=configFlags.GeoModel.AtlasVersion

    from AthenaCommon.AppMgr import release_metadata
    rel_metadata = release_metadata()
    relversion = rel_metadata['release'].split('.')
    if len(relversion) < 3:
        relversion = rel_metadata['base release'].split('.')


    result=ComponentAccumulator()
    from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
    gms=GeoModelSvc(AtlasVersion=version,
                    SupportedGeometry = int(relversion[0]))
    if configFlags.Detector.Simulate:
        ## Protects GeoModelSvc in the simulation from the AlignCallbacks
        gms.AlignCallbacks = False
    result.addService(gms,primary=True)
    
    from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc
    from GaudiSvc.GaudiSvcConf import EvtPersistencySvc

    # Specify primary Identifier dictionary to be used
    detDescrCnvSvc=DetDescrCnvSvc(IdDictName = "IdDictParser/ATLAS_IDS.xml",IdDictFromRDB = True)
    # this flag is set as it was in https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/Reconstruction/RecExample/RecExCond/share/AllDet_detDescr.py#0050
    if configFlags.Detector.Geometry:
        detDescrCnvSvc.DecodeIdDict = True
    result.addService(detDescrCnvSvc)
    result.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[detDescrCnvSvc.getName(),])) #No service handle yet???

    from EventInfoMgt.TagInfoMgrConfig import TagInfoMgrCfg
    tim_ca,tagInfoMgr=TagInfoMgrCfg(configFlags)
    result.addService(tagInfoMgr)
    result.merge(tim_ca)
    #TagInfoMgr used by GeoModelSvc but no ServiceHandle. Relies on string-name

    return result



if __name__ == "__main__":
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW

    acc = GeoModelCfg( ConfigFlags )
    acc.store( file( "test.pkl", "w" ) )
    print "All OK"

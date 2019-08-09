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


    #Get DetDescrCnvSvc (for identifier dictionaries (identifier helpers)
    from DetDescrCnvSvc.DetDescrCnvSvcConfig import DetDescrCnvSvcCfg
    result.merge(DetDescrCnvSvcCfg(configFlags)
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

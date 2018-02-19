from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def GeoModelCfg(ConfigFlagContainer):
    version=ConfigFlagContainer.get("AtlasGeoModel.GeoModelFlags.AtlasVersion")

    from AthenaCommon.AppMgr import release_metadata
    rel_metadata = release_metadata()
    relversion = rel_metadata['release'].split('.')
    if len(relversion) < 3:
        relversion = rel_metadata['base release'].split('.')


    result=ComponentAccumulator()
    from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
    result.addService(GeoModelSvc(AtlasVersion=version,
                                  SupportedGeometry = int(relversion[0])))
    
    from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc
    from GaudiSvc.GaudiSvcConf import EvtPersistencySvc

    # Specify primary Identifier dictionary to be used
    detDescrCnvSvc=DetDescrCnvSvc(IdDictName = "IdDictParser/ATLAS_IDS.xml",IdDictFromRDB = True)
    result.addService(detDescrCnvSvc)
    result.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[detDescrCnvSvc.getName(),])) #No service handle yet???

    return result

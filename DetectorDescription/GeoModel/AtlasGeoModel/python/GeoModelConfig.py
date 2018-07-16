from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def GeoModelCfg(configFlags):
    version=configFlags.get("GeoModel.AtlasVersion")

    from AthenaCommon.AppMgr import release_metadata
    rel_metadata = release_metadata()
    relversion = rel_metadata['release'].split('.')
    if len(relversion) < 3:
        relversion = rel_metadata['base release'].split('.')


    result=ComponentAccumulator()
    from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
    gms=GeoModelSvc(AtlasVersion=version,
                    SupportedGeometry = int(relversion[0]))

    result.addService(gms)
    
    from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc
    from GaudiSvc.GaudiSvcConf import EvtPersistencySvc

    # Specify primary Identifier dictionary to be used
    detDescrCnvSvc=DetDescrCnvSvc(IdDictName = "IdDictParser/ATLAS_IDS.xml",IdDictFromRDB = True)
    result.addService(detDescrCnvSvc)
    result.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[detDescrCnvSvc.getName(),])) #No service handle yet???

    from EventInfoMgt.TagInfoMgrConfig import TagInfoMgrCfg
    tim_ca,tagInfoMgr=TagInfoMgrCfg(configFlags)
    result.addService(tagInfoMgr)
    result.merge(tim_ca)
    #TagInfoMgr used by GeoModelSvc but no ServiceHandle. Relies on string-name

    return result,gms

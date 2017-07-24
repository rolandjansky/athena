from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
import GaudiSvc.GaudiSvcConf as GaudiSvcConf




def GeoModelCfg():
    result=ComponentAccumulator()
    from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
    theGeoSvc=GeoModelSvc()
    result.addService(GeoModelSvc())
    
    from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc
    
    # Specify primary Identifier dictionary to be used
    result.addService(DetDescrCnvSvc(IdDictName = "IdDictParser/ATLAS_IDS.xml"))
    
    evtPersSvc=GaudiSvcConf.EvtPersistencySvc( "EventPersistencySvc" )
    evtPersSvc.CnvServices += [ "DetDescrCnvSvc" ]
    result.addService(evtPersSvc)

    return result


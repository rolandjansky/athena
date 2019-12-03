# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def DetDescrCnvSvcCfg(configFlags):
    from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc
    from GaudiSvc.GaudiSvcConf import EvtPersistencySvc
    acc=ComponentAccumulator()
    # Specify primary Identifier dictionary to be use
    detDescrCnvSvc=DetDescrCnvSvc(IdDictName = "IdDictParser/ATLAS_IDS.xml",
                                  IdDictFromRDB = True,
                                  HasCSC=configFlags.Detector.GeometryCSC,
                                  HasSTgc=configFlags.Detector.GeometrysTGC,
                                  HasMM=configFlags.Detector.GeometryMM)
    acc.addService(detDescrCnvSvc)
    acc.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[detDescrCnvSvc.getName(),])) #No service handle yet???
    return acc

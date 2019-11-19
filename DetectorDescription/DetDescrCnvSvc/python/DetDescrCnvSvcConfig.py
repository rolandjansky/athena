# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

def DetDescrCnvSvcCfg(configFlags=None):
    from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc
    from GaudiSvc.GaudiSvcConf import EvtPersistencySvc
    acc=ComponentAccumulator()
    # Specify primary Identifier dictionary to be use
    detDescrCnvSvc=DetDescrCnvSvc(IdDictName = "IdDictParser/ATLAS_IDS.xml",
                                  IdDictFromRDB = True,
                                  HasCSC=MuonGeometryFlags.hasCSC(),
                                  HasSTgc=MuonGeometryFlags.hasSTGC(),
                                  HasMM=MuonGeometryFlags.hasMM())
    acc.addService(detDescrCnvSvc)
    acc.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[detDescrCnvSvc.getName(),])) #No service handle yet???
    return acc

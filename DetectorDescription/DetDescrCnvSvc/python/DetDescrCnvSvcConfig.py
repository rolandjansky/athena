# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

def DetDescrCnvSvcCfg(configFlags=None):
    from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc
    from GaudiSvc.GaudiSvcConf import EvtPersistencySvc
    acc=ComponentAccumulator()
    # Specify primary Identifier dictionary to be use
    detDescrCnvSvc=DetDescrCnvSvc(IdDictName = "IdDictParser/ATLAS_IDS.xml",
                                  IdDictFromRDB = True,
                                  HasCSC=MuonGeometryFlags.hasCSC(),
                                  HasSTgc=(CommonGeometryFlags.Run() in ["RUN3", "RUN4"]),
                                  HasMM=(CommonGeometryFlags.Run() in ["RUN3", "RUN4"]))
    acc.addService(detDescrCnvSvc)
    acc.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[detDescrCnvSvc.getName(),])) #No service handle yet???
    return acc

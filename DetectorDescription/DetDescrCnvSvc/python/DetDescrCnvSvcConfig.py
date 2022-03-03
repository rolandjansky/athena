# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def DetDescrCnvSvcCfg(flags, **kwargs):
    kwargs.setdefault("IdDictName", "IdDictParser/ATLAS_IDS.xml")
    kwargs.setdefault("HasCSC", flags.Detector.GeometryCSC)
    kwargs.setdefault("HasSTgc", flags.Detector.GeometrysTGC)
    kwargs.setdefault("HasMM", flags.Detector.GeometryMM)
    
    # Use GEOM DB to read InDet information 
    if flags.Detector.GeometryITk:
       kwargs.setdefault("useGeomDB_InDet", True)
    else:
       kwargs.setdefault("useGeomDB_InDet", False)   


    if flags.GeoModel.useLocalGeometry:
        kwargs.setdefault("IdDictFromRDB", False)
        kwargs.setdefault("InDetIDFileName", "ITKLayouts/IdDictInnerDetector_ITK_LOCAL.xml")
        kwargs.setdefault("MuonIDFileName", "IdDictParser/IdDictMuonSpectrometer_R.09.NSW.xml")
        kwargs.setdefault("LArIDFileName", "IdDictParser/IdDictLArCalorimeter_HGTD_TDR_01.xml")
        kwargs.setdefault("TileIDFileName", "IdDictParser/IdDictTileCalorimeter.xml")
        kwargs.setdefault("CaloIDFileName", "IdDictParser/IdDictCalorimeter_L1Onl.xml")
        kwargs.setdefault("ForwardIDFileName", "IdDictParser/IdDictForwardDetectors_2010.xml")
    else:
        kwargs.setdefault("IdDictFromRDB", True)

    DetDescrCnvSvc=CompFactory.DetDescrCnvSvc
    EvtPersistencySvc=CompFactory.EvtPersistencySvc
    acc=ComponentAccumulator()
    # Specify primary Identifier dictionary to be use
    detDescrCnvSvc=DetDescrCnvSvc(**kwargs)
    acc.addService(detDescrCnvSvc,create=True)
    acc.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[detDescrCnvSvc.getName(),])) #No service handle yet???
    return acc

# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def DetDescrCnvSvcCfg(flags, **kwargs):
    kwargs.setdefault("IdDictName", "IdDictParser/ATLAS_IDS.xml")
    kwargs.setdefault("HasCSC", flags.Detector.GeometryCSC)
    kwargs.setdefault("HasSTgc", flags.Detector.GeometrysTGC)
    kwargs.setdefault("HasMM", flags.Detector.GeometryMM)

    # Use GEOM DB to read InDet dictionary information 
    if flags.Detector.GeometryITk:
       kwargs.setdefault("useGeomDB_InDet", True)
    else:
       kwargs.setdefault("useGeomDB_InDet", False)   

    if (flags.Detector.GeometryITk or flags.Detector.GeometryHGTD) \
        and flags.ITk.Geometry.DictionaryLocal:
        kwargs.setdefault("IdDictFromRDB", False)
        kwargs.setdefault("InDetIDFileName", flags.ITk.Geometry.DictionaryFilename)
        kwargs.setdefault("MuonIDFileName", "IdDictParser/IdDictMuonSpectrometer_R.10.00.xml")
        kwargs.setdefault("LArIDFileName", "IdDictParser/IdDictLArCalorimeter_DC3-05-Comm-01.xml")
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

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# use this file as postInclude to initialize a muon geometry from local files (can be amdb/passive material AGDD/NSW AGDD)

didOverwriteAnything=False

# check if existing muon layout was given and initialize geometry from there
try:
    if database_layout:
        print ("INFO: specified %s as muon geometry, using it"%database_layout)
        from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
        GeoModelSvc = GeoModelSvc()
        GeoModelSvc.MuonVersionOverride=database_layout
        didOverwriteAnything=True
except NameError:
    pass

# check if amdb file was given and initialize geometry from there
try:
    if input_amdb_simrec:
        include('MuonGeoModel/InitGeoFromAmdb_postIncl.py')
        didOverwriteAnything=True
except NameError:
    pass

# check if AGDD xml file was given and initialize passive materials from there
try:
    if input_agdd_xml:
        print ("INFO: specified AGDD xml file %s for passive materials, using it"%input_agdd_xml)
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if hasattr(svcMgr,"AGDDtoGeoSvc"):
            for b in getattr(svcMgr,"AGDDtoGeoSvc").Builders:
                if b.name()=="MuonSpectrometer":
                    b.ReadAGDD=False
                    b.XMLFiles=[input_agdd_xml]
                    try:
                        if agddToGeoSwitches: b.Volumes=agddToGeoSwitches
                    except NameError:
                        b.Volumes=[
                            "ECT_Toroids",
                            "BAR_Toroid",
                            "Feet",
                            "RailAssembly",
                            "JFSH_Shield",
                            "JDSH_Shield",
                            "JTSH_Shield",
                            "pp2",
                            "MBAP_AccessPlatform",
                            "MBWH_BigWheels",
                            "SADL_CalorimeterSaddle",
                            "TBWH_BigWheels",
                            "TGC3_BigWheels",
                            "TGC1_BigWheels",
                            "MDTRail",
                            "servicesAtZ0",
                            "HFTruckRail",
                            "RUN2_Services"
                        ]
        didOverwriteAnything=True
except NameError:
    pass

# check if NSW AGDD xml file was given and initialize NSW geometry from there
try:
    if input_nsw_xml:
        print ("INFO: specified AGDD xml file %s for NSW, using it"%input_nsw_xml)
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if hasattr(svcMgr,"AGDDtoGeoSvc"):
            for b in getattr(svcMgr,"AGDDtoGeoSvc").Builders:
                if b.name()=="NewSmallWheel":
                    b.ReadAGDD=False
                    b.XMLFiles=[input_nsw_xml]
        didOverwriteAnything=True
except NameError:
    pass

if not didOverwriteAnything:
    print ('WARNING: you included InitGeoFromLocal_postIncl.py, but you did not overwrite any setting, why?')
    import sys
    sys.exit(0)


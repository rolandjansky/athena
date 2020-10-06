# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

try:
    if muonLayout:
        from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
        GeoModelSvc = GeoModelSvc()
        GeoModelSvc.MuonVersionOverride=muonLayout
except NameError:
    pass

from MuonAGDD.MuonAGDDConfig import NSWAGDDTool
NSWAGDDDumper = NSWAGDDTool(name="NSWAGDDDumper")
NSWAGDDDumper.WriteAGDDFile=True # creates Out.AmdcOracle.AM.AGDDtemp.data and Out.AmdcOracle.AM.AGDD.PREsql
NSWAGDDDumper.Locked=False
try:
    if input_nsw_xml:
        NSWAGDDDumper.ReadAGDD = False # turn off reading from data base (true means read from DB)
        NSWAGDDDumper.XMLFiles=[input_nsw_xml]
except NameError:
    pass
NSWAGDDDumper.Volumes=["NewSmallWheel"] # for the NSWAGDDTool volumes must always be given ("NewSmallWheel" usually includes everything)
NSWAGDDDumper.DefaultDetector="Muon"

from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
AGDD2Geo = AGDDtoGeoSvc()
AGDD2Geo.Builders += [NSWAGDDDumper]
theApp.CreateSvc += ["AGDDtoGeoSvc"]
ServiceMgr += AGDD2Geo

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from MuonAGDD.MuonAGDDConfig import MuonAGDDTool
MuonAGDDDumper = MuonAGDDTool(name="AGDDDumper")
MuonAGDDDumper.WriteAGDDFile=True
MuonAGDDDumper.BuildNSW=False
MuonAGDDDumper.Locked=True
try:
    if input_agdd_xml:
        MuonAGDDDumper.ReadAGDD = False # turn off reading from data base (true means read from DB)
        MuonAGDDDumper.XMLFiles=[input_agdd_xml]
        try:
            if amdc_name: MuonAGDDDumper.AmdcName=amdc_name
        except NameError:
            pass
except NameError:
    pass

from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
AGDD2Geo = AGDDtoGeoSvc()
AGDD2Geo.Builders += [MuonAGDDDumper]
theApp.CreateSvc += ["AGDDtoGeoSvc"]
ServiceMgr += AGDD2Geo

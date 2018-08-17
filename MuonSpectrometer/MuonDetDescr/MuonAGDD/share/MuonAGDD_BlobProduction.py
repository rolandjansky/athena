from MuonAGDD.MuonAGDDConfig import MuonAGDDTool
MuonAGDDDumper = MuonAGDDTool(name="AGDDDumper")
MuonAGDDDumper.WriteAGDDFile=True
MuonAGDDDumper.BuildNSW=False
MuonAGDDDumper.Locked=True
# at the moment the blob is retrieved from AmdcAth the following line would produced it from input xml
# MuonAGDDDumper.ReadAGDD = False
# MuonAGDDDumper.XMLFiles=["inputXMLfile.xml"]
ToolSvc += MuonAGDDDumper

from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
AGDD2Geo = AGDDtoGeoSvc()
AGDD2Geo.Builders += ["MuonAGDDTool/AGDDDumper"]
theApp.CreateSvc += ["AGDDtoGeoSvc"]
ServiceMgr += AGDD2Geo


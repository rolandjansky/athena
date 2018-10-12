from MuonAGDD.MuonAGDDConfig import NSWAGDDTool
NSWAGDDDumperFromFile = NSWAGDDTool(name="NSWAGDDDumperFromFile")
NSWAGDDDumperFromFile.WriteAGDDFile=True
NSWAGDDDumperFromFile.Locked=False
# turn off reading from data base (true means read from DB)
NSWAGDDDumperFromFile.ReadAGDD=False
NSWAGDDDumperFromFile.XMLFiles=[input_nsw_xml]
# for the NSWAGDDTool volumes must always be given ("NewSmallWheel" usually includes everything)
NSWAGDDDumperFromFile.Volumes=["NewSmallWheel"]
NSWAGDDDumperFromFile.DefaultDetector="Muon"
ToolSvc += NSWAGDDDumperFromFile

# write blob from DB in Generated_NSWD_pool.txt
from MuonAGDD.MuonAGDDConfig import NSWAGDDTool
NSWAGDDDumperFromDB = NSWAGDDTool(name="NSWAGDDDumperFromDB")
NSWAGDDDumperFromDB.ReadAGDD=True
NSWAGDDDumperFromDB.DumpAGDD=True
NSWAGDDDumperFromDB.Locked=False
# for the NSWAGDDTool volumes must always be given ("NewSmallWheel" usually includes everything)
NSWAGDDDumperFromDB.Volumes=["NewSmallWheel"]
NSWAGDDDumperFromDB.DefaultDetector="Muon"
ToolSvc += NSWAGDDDumperFromDB

from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
AGDD2Geo = AGDDtoGeoSvc()
AGDD2Geo.Builders += ["NSWAGDDTool/NSWAGDDDumperFromFile"]
AGDD2Geo.Builders += ["NSWAGDDTool/NSWAGDDDumperFromDB"]
theApp.CreateSvc += ["AGDDtoGeoSvc"]
ServiceMgr += AGDD2Geo

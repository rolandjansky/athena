from MuonAGDD.MuonAGDDConfig import NSWAGDDTool
NSWAGDDDumper = NSWAGDDTool(name="NSWAGDDDumper")
NSWAGDDDumper.WriteAGDDFile=True
NSWAGDDDumper.Locked=False
# turn off reading from data base (true means read from DB)
NSWAGDDDumper.ReadAGDD=False
NSWAGDDDumper.XMLFiles=[input_nsw_xml]
# for the NSWAGDDTool volumes must always be given ("NewSmallWheel" usually includes everything)
NSWAGDDDumper.Volumes=["NewSmallWheel"]
NSWAGDDDumper.DefaultDetector="Muon"
# setting of not automatically generated rows in database table
# NSWAGDDDumper.OutputFileACTVERS  = 0         # active version number
# NSWAGDDDumper.OutputFileACTVNAME = ""        # active version string
# NSWAGDDDumper.OutputFileALGVERS  = 0         # alignment version number
# NSWAGDDDumper.OutputFileALGVNAME = ""        # alignment version string
# NSWAGDDDumper.OutputFilePASVERS  = 0         # passive structure version number
# NSWAGDDDumper.OutputFilePASVNAME = ""        # passive structure version string
# NSWAGDDDumper.OutputFileFORMAT   = "AGDDXML" # format of output file
# NSWAGDDDumper.OutputFileType     = "NSWD"    # name for database table
ToolSvc += NSWAGDDDumper

from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
AGDD2Geo = AGDDtoGeoSvc()
AGDD2Geo.Builders += ["NSWAGDDTool/NSWAGDDDumper"]
theApp.CreateSvc += ["AGDDtoGeoSvc"]
ServiceMgr += AGDD2Geo

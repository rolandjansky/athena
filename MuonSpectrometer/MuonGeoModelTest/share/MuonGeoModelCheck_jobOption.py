# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AthenaCommonFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import jobproperties,athenaCommonFlags
# import service manager
import AthenaCommon.AtlasUnixStandardJob
## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr
## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

# Turn off all detector systems except the Muon Spectrometer
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Muon_setOn()

# Select geometry version
if not 'DetDescrVersion' in dir():
    DetDescrVersion="ATLAS-GEO-20-00-01"
    printfunc ('MuonGeoModelTest/runAmdcMGM DetDescrVersion now set to ', DetDescrVersion)
else:
    printfunc ('MuonGeoModelTest/runAmdcMGM DetDescrVersion is already ', DetDescrVersion)
                    

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
######################################################################## end setup GeoModel

# 0 to run MuonGM checks init time only
# 1 to run MuonGM checks at each event
GMCheckLoop = 0

if GMCheckLoop:
    #import AthenaPoolCnvSvc.ReadAthenaPool
    #ServiceMgr.EventSelector.InputCollections = [ "/afs/le.infn.it/user/s/spagnolo/projAtlas/hlt/stef/data/mc12.csc120.007218.digit.muminus_pt20GeV._00001.pool.root" ]
    theApp.EvtMax = 10
else:
    #theApp.EvtSel = "NONE"
    theApp.EvtMax = 1

# Modify A line parameters
# GeneralControlAlines = I1 * 100000 + I2 * 10000 + I3 * 1000 + I4 * 100 + I5 * 10 + I6
# I1 = 0/1 zerofy or not the s translation
# I2 = 0/1 zerofy or not the z translation
# I3 = 0/1 zerofy or not the t translation
# I4 = 0/1 zerofy or not the s rotation
# I5 = 0/1 zerofy or not the z rotation
# I6 = 0/1 zerofy or not the t rotation
GeneralControlAlines = 111111

# Set output level threshold 
#(0=NIL 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel = INFO


#***************************************************** HERE override MuonSpectrometer tag
from AtlasGeoModel import SetGeometryVersion
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()

printfunc (GeoModelSvc)

#***************************************************** HERE setup MuonDetectorManager
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
MuonDetectorTool = MuonDetectorTool()
MuonDetectorTool.HasCSC=MuonGeometryFlags.hasCSC()
MuonDetectorTool.HasSTgc=MuonGeometryFlags.hasSTGC()
MuonDetectorTool.HasMM=MuonGeometryFlags.hasMM()
MuonDetectorTool.StationSelection = 0
MuonDetectorTool.SelectedStations = [ "BOG" ]
MuonDetectorTool.SelectedStJzz = [3]
MuonDetectorTool.SelectedStJff = [6]
MuonDetectorTool.ControlAlines    = GeneralControlAlines
MuonDetectorTool.ForceSwitchOnOff_BUILDINERTMATERIALS = 1000; # avoid building the dead matter
MuonDetectorTool.FillCacheInitTime=1
MuonDetectorTool.DumpMemoryBreakDown=True

printfunc (MuonDetectorTool)

#***************************************************** HERE setup MuonGMCheck
from MuonGeoModelTest.MuonGeoModelTestConf import MuonGMCheck
MuonGMCheck = MuonGMCheck()
MuonGMCheck.EventLoopMode = GMCheckLoop
MuonGMCheck.check_mdt = 1
MuonGMCheck.check_rpc = 1
MuonGMCheck.check_tgc = 1
MuonGMCheck.check_csc = 1
from MuonIdHelpers.MuonIdHelpersConf import Muon__MuonIdHelperSvc
ServiceMgr += Muon__MuonIdHelperSvc("MuonIdHelperSvc",HasCSC=MuonGeometryFlags.hasCSC(), HasSTgc=MuonGeometryFlags.hasSTGC(), HasMM=MuonGeometryFlags.hasMM())
MuonGMCheck.MuonIdHelperSvc=ServiceMgr.MuonIdHelperSvc

printfunc (MuonGMCheck)


######################################################
############### HERE DEFINE THE SEQUENCE #############
######################################################

topSequence += MuonGMCheck

printfunc (" ")
printfunc ("List of all Dlls")
printfunc (theApp.Dlls)
printfunc (" ")
printfunc ("List of all Ext services")
printfunc (theApp.ExtSvc)
printfunc (" ")
printfunc ("List of all top algorithms")
printfunc (theApp.TopAlg)


# setup AMDC for the H8 TestBeam geometry 
include ( "AmdcAth/AmdcAth_jobOptions.py" )

# Geomodel configuratio
#if (MuonLayout == "P03-DC1" or MuonLayout == "P03-DC2"):
#    DetDescrVersion = "DC2"
#elif (MuonLayout == "Q02_initial"):
#    DetDescrVersion = "Rome-Initial"


#--------------------------------------------------------------
# Configure GeoModel without the NOVA converters
#--------------------------------------------------------------

# Load RDBAcessSvc (Access to parameter Database)
include ("RDBAccessSvc/RDBAccessSvcPdb_jobOptions.py")

#load relevant libraries
theApp.Dlls += [ "GeoModelSvc" ]
# Setup Geometry Model Service
theApp.ExtSvc += [ "GeoModelSvc"]

# Set the ATLAS version. This is taken
# care of by AtlasGeoModel/SetGeometryVersion.py
# which looks at the variable DetDescrVersion
# If DetDescrVersion is already defined we use that
# otherwise we decided based on whats in GlobalFlags

if not "DetDescrVersion" in dir():
    # Default to DC2
    DetDescrVersion = "DC2"

    if not "initialLayout" in dir():
        initialLayout = False
    
    if (initialLayout):
        # default to Rome-Initial for initial layout
        DetDescrVersion = "Rome-Initial"
        
    if ("GlobalFlags" in dir()):
        
        if GlobalFlags.DetGeo.is_ctb():
            DetDescrVersion = "CTB"
            
        elif GlobalFlags.DataSource.is_geant3():
            if (initialLayout):
                DetDescrVersion = "DC1-Initial"
            else:
                DetDescrVersion = "DC1"
    
print  "Selected version is: ", DetDescrVersion
include ("AtlasGeoModel/SetGeometryVersion.py")

#    include( "AtlasGeoModel/GeoModelCommon.py" )
include( "AtlasGeoModel/MuonGeoModel.py" )

DetDescrCnvSvc.DecodeIdDict = TRUE


#--------------------------------------------------------------
# configure the MDT calibration Service
#--------------------------------------------------------------
theApp.Dlls += [ "MdtCalibSvc" ]
theApp.ExtSvc += [ "MdtCalibrationSvc" ]
MdtCalibrationSvc = Service( "MdtCalibrationSvc" )
MdtCalibrationSvc.implementation = "default"
MdtCalibrationSvc.T0_InputFiles = [ "MdtCalibSvc/DC2_t0.dat" ]
MdtCalibrationSvc.RT_InputFiles = [ "MdtCalibSvc/DC2_rt.dat" ]


#--------------------------------------------------------------
# LVL1 Result
#--------------------------------------------------------------
theApp.DLLs += [ "TrigT1ResultByteStream" ]


# -------------------------------------------------------------
# L1 configuration
# -------------------------------------------------------------
include( "TrigT1Config/TrigT1ConfigJobOptions.py" )

# L1 Configuration
include( "TrigT1RPCRecRoiSvc/TrigT1RPCRecRoiSvcJobOptions.py" )
include( "TrigT1TGCRecRoiSvc/TrigT1TGCRecRoiSvcJobOptions.py" )

# load MuonByteStreamCnv
theApp.DLLs += [ "MuonByteStream" ]

# load the L2 standalone configuration for RPC cabling and geometry
theApp.DLLs += [ "RPCgeometry" ]
theApp.DLLs += [ "RPCcabling" ]

theApp.ExtSvc += [ "RPCgeometrySvc" ]
theApp.ExtSvc += [ "RPCcablingSvc" ]

RPCcablingSvc  = Service ( "RPCcablingSvc" )
RPCgeometrySvc = Service ( "RPCgeometrySvc" )


if (DetDescrVersion == "DC1" or DetDescrVersion == "DC2"):
    RPCcablingSvc.ConfFileName    = "P03conf.data"
    RPCcablingSvc.CorrFileName    = "P03conf.corr"
else:
    RPCcablingSvc.ConfFileName    = "LVL1conf.data"
    RPCcablingSvc.CorrFileName    = "LVL1conf.corr"

RPCgeometrySvc.DumpRPCStations = TRUE
RPCgeometrySvc.DumpRPCGeometry = TRUE

# load the configuration for the TGC cabling
theApp.Dlls += [ "TGCcabling" ]
theApp.ExtSvc += [ "TGCcablingSvc" ]
if (DetDescrVersion == "DC1" or DetDescrVersion == "DC2" ):
	TGCcablingSvc = Service ("TGCcablingSvc")
	TGCcablingSvc.databaseASDToPP = "ASD2PP_P03.db"
	TGCcablingSvc.databaseInPP    = "PP_P03.db"


# load the L2 standalone configuration for MDT cabling and geometry
theApp.DLLs   += [ "MDTcabling" ]
theApp.ExtSvc += [ "MDTcablingSvc" ]

MDTcablingSvc = Service ( "MDTcablingSvc" )

if (DetDescrVersion == "DC1" or DetDescrVersion == "DC2"):
    MDTcablingSvc.RODfile    = "P03_RODmap.data"
else:
    MDTcablingSvc.RODfile    = "RODmap.data"


MDTcablingSvc.MapFlag = TRUE
MDTcablingSvc.CheFlag = TRUE
MDTcablingSvc.GeoFlag = TRUE

# ----------------------------------------------------------------
# configure the Region Selector
# ----------------------------------------------------------------

theApp.DLLs += [ "RegionSelector" ]
ToolSvc = Service( "ToolSvc" )
RegionSelector = Algorithm('ToolSvc.RegionSelector')
RegionSelector.GeometryLayout = DetDescrVersion
    
    
# -------------------------------------------------------------
# run HLTSSW for LVL2
# -------------------------------------------------------------
include( "TrigSteering/jobOfragment_HLTSSW_forL2.py" )
Lvl1Conversion_L2 = Algorithm( "Lvl1Conversion_L2" )
Lvl1Conversion_L2.lvl1Result = "MUONROI"

Lvl1Conversion_L2.useL1CaloSimulation=False
Lvl1Conversion_L2.useL1MuonSimulation=True

TriggerConfig.sequenceListFileLocation = "HLTsequenceMuonOnly.xml" 
TriggerConfig.signatureListFileLocation = "HLTsignatureMuonOnly.xml"


L2Result = Algorithm( "L2Result" )
L2Result.OutputLevel = FATAL


#--------------------------------------------------------------
# Configure muFast algorithm
#--------------------------------------------------------------
theApp.DLLs   += [ "TrigmuFast" ]

# load other L2 services
theApp.ExtSvc += [ "MuRoadsSvc", "MuLUTSvc" ]

muFast = Algorithm ( "muFast_1_L2" )


muFast.MUlvl1INFO  = True
muFast.MUtrackINFO = True
muFast.MUroadsINFO = True
muFast.MUdecoINFO  = True
muFast.MUcontINFO  = True
muFast.MUfitINFO   = True
muFast.MUsagINFO   = True
muFast.MUptINFO    = True

muFast.TestString  = ""

muFast.UseCalibration = True
muFast.OnlineRun = True

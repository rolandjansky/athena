# setup AMDC for the H8 TestBeam geometry 
include ( "AmdcAth/AmdcAth_jobOptions.py" )
AmdcsimrecAthenaSvc.NameOfTheAmdbFile = "amdb_simrec.H8_2004_CTB.a.04"

# -------------------------------------------------------------
# L1 configuration
# -------------------------------------------------------------
include( "TrigT1Config/TrigT1ConfigJobOptions.py" )

# L1 Configuration
include( "TrigT1RPCRecRoiSvc/TrigT1RPCRecRoiSvcJobOptions.py" )
#include( "TrigT1TGCRecRoiSvc/TrigT1TGCRecRoiSvcJobOptions.py" )

# load the L2 standalone configuration for the H8 RPC geometry
theApp.DLLs += [ "RPCgeometry" ]
theApp.DLLs += [ "RPCcabling" ]

theApp.ExtSvc += [ "RPCgeometrySvc" ]
theApp.ExtSvc += [ "RPCcablingSvc" ]

RPCcablingSvc  = Service ( "RPCcablingSvc" )
RPCgeometrySvc = Service ( "RPCgeometrySvc" )


if ( CombinedRun ):
	RPCcablingSvc.ConfFileName = "TB_25ns_LVL1conf.data"
	RPCcablingSvc.CorrFileName = "TB_25ns_LVL1conf.corr"
else:
	RPCcablingSvc.ConfFileName = "TB_1_LVL1conf.data"
	RPCcablingSvc.CorrFileName = "TB_1_LVL1conf.corr"


RPCgeometrySvc.H8testbeam  = TRUE

RPCgeometrySvc.DumpRPCStations = TRUE
RPCgeometrySvc.DumpRPCGeometry = TRUE

# load the L2 standalone configuration for the H8 MDT geometry
theApp.DLLs   += [ "MDTcabling" ]
theApp.ExtSvc += [ "MDTcablingSvc" ]

MDTcablingSvc = Service ( "MDTcablingSvc" )

#==>MDTcablingSvc.RODfile    = "TB_1_RODmap.data" ;
if ( CombinedRun ):
	MDTcablingSvc.RODfile    = "TB_3_RODmap.data"
else:
	MDTcablingSvc.RODfile    = "TB_2_RODmap.data"

MDTcablingSvc.H8testbeam = TRUE

MDTcablingSvc.MapFlag = TRUE
MDTcablingSvc.CheFlag = TRUE
MDTcablingSvc.GeoFlag = TRUE

# ----------------------------------------------------------------
# jobOFragment to run HLT Steering & Selection Algorithms
# ----------------------------------------------------------------

theApp.DLLs += [ "RegionSelector" ]

# -------------------------------------------------------------
# run HLTSSW for LVL2
# -------------------------------------------------------------
include( "TrigSteering/jobOfragment_HLTSSW_forL2.py" )
Lvl1Conversion_L2 = Algorithm( "Lvl1Conversion_L2" )

Lvl1Conversion_L2.useL1CaloSimulation=False
Lvl1Conversion_L2.useL1MuonSimulation=True
        
include( "TrigFake/jobOfragment_TrigFake.py" )
ResultBuilder_L2.useL1Simulation= "NO"
StepController_L2 = Algorithm( "StepController_L2" )
StepController_L2.Lvl1Conversion="FakeLvl1ConversionTestBeam"
FakeLvl1ConversionTestBeam_L2 = Algorithm( "FakeLvl1ConversionTestBeam_L2" )
#FakeLvl1ConversionTestBeam_L2.FakeEmTauRoiParticleIDs=[11,-11]
#FakeLvl1ConversionTestBeam_L2.FakeEmTauRoiPtMin=20.*GeV
#FakeLvl1ConversionTestBeam_L2.FakeMuonRoiParticleIDs=[13,-13]
#FakeLvl1ConversionTestBeam_L2.FakeMuonRoiPtMin=20.*GeV

#give a list of RoI you want to create (as many as the shortest list below)
FakeLvl1ConversionTestBeam_L2.RoiIDs = ["MUONROI","MUONROI","MUONROI","EMTAUROI"]
#their positions in eta
#FakeLvl1Conversion_L2.RoiPositionsEta = [-0.405654,0.87,99.]
FakeLvl1ConversionTestBeam_L2.RoiPositionsEta = [2.75]
#and phi
FakeLvl1ConversionTestBeam_L2.RoiPositionsPhi = [4.09706,0.17,0.]
FakeLvl1ConversionTestBeam_L2.MUONROIsubsysId=1
FakeLvl1ConversionTestBeam_L2.MUONROIsectorId=31

# Configure the RoI to be processed for the Combined run setup
if ( RoISaleve ):
	FakeLvl1ConversionTestBeam_L2.MUONROIroiNumber=30
else:
	FakeLvl1ConversionTestBeam_L2.MUONROIroiNumber=6
	
#FakeLvl1ConversionTestBeam_L2.OutputLevel = FATAL

TriggerConfig.sequenceListFileLocation = "muFastOnlineSeq.xml" 
TriggerConfig.signatureListFileLocation = "muonSignatures.xml"


L2Result = Algorithm( "L2Result" )
L2Result.OutputLevel = FATAL

# Load the DataMonitor Algorithm
theApp.DLLs   += [ "TrigmuFast" ]

# load other L2 services
theApp.ExtSvc += [ "MuRoadsSvc", "MuLUTSvc" ]

muFast = Algorithm ( "muFast_l2_L2" )

muFast.Mdt1id = [ 1,3,5,4,6,12 ]
muFast.Mdt2id = [ 2,4,6,3,5,12 ]

#DataMonitor.RobId = [ 91649,  90378,  90379  ]
#                     RPCROB  MDTROB  MDTROB
muFast.RobId = [ 91649,  90369,  90370,  90883 ]
#                    RPCROB  MDTROB  MDTROB  MDTROB

muFast.MUlvl1INFO  = TRUE;
muFast.MUtrackINFO = TRUE;
muFast.MUroadsINFO = TRUE;
muFast.MUdecoINFO  = TRUE;
muFast.MUcontINFO  = TRUE;
muFast.MUfitINFO   = TRUE;
muFast.MUsagINFO   = TRUE;
muFast.MUptINFO    = TRUE;

muFast.OnlineRun = TRUE
muFast.TestbeamRun = TRUE

# configure for using MDT calibration
if ( muFastUseCalib ):
	muFast.UseCalibration = True

# Configure the RoI to be processed for the Combined run setup
if ( RoISaleve ):
	muFast.RoiNumber = 30
else:
	muFast.RoiNumber = 6

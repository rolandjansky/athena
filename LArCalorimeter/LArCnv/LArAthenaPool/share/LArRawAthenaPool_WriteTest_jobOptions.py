from AthenaCommon.DetFlags import DetFlags

#by default everything is off
#switch on tasks for all detectors

DetFlags.detdescr.all_setOff()
DetFlags.detdescr.LAr_setOn()
DetFlags.detdescr.Tile_setOn()
DetFlags.makeRIO.LAr_setOn()
DetFlags.makeRIO.Tile_setOn()

doCaloNoise=False; 
readG3= False

if readG3:
	theApp.setup( ZEBRA )
	EventSelector = Service( "EventSelector" )
	EventSelector.firstRun=1; 
	EventSelector.lastRun=100000; 
	EventSelector.calos = FALSE
	EventSelector.muons = FALSE
	EventSelector.indet = FALSE
else:
	include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
	EventSelector = Service( "EventSelector" )
	EventSelector.InputCollections = ["/afs/cern.ch/atlas/offline/data/testfile/dc2.002843.pyt_min_bias.g4sim780._0001.pool.root"
]

include ("RecExCommon/AllDet_detDescr.py") 

if readG3:
	include( "LArROD/LArROD_MC_jobOptions.py" )
else:
	include( "LArDigitization/LArDigitization_G4_jobOptions.py" )
	include( "LArROD/LArROD_G4_jobOptions.py" )


theApp.TopAlg += [ "ReadLArRaw" ]
ReadLArRaw = Algorithm( "ReadLArRaw" )
ReadLArRaw.LArRawChannelContainerName = "LArRawChannels" 
ReadLArRaw.HistogramID = 128 
ReadLArRaw.NtupleLocID = "/FILE1/LARG" 
# Tuning algorithm options...  Check LArDigitization_MC_jobOtpions.txt
digitmaker1 = Algorithm( "digitmaker1" )
digitmaker1.NoiseOnOff = FALSE 
digitmaker1.SubDetectors = "LAr_All"
digitmaker1.EnergyThresh = -99.
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------

include ("AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py")
Stream1 = AthenaPoolOutputStream( "Stream1" )

theApp.Dlls   += [ "LArCalibUtils" ]
theApp.Dlls   += [ "LArAthenaPoolPoolCnv" ]

Stream1.ItemList+=["EventInfo#*"]
Stream1.ItemList+=["LArDigitContainer#*"]; # LArDigitContainer
Stream1.ItemList+=["LArRawChannelContainer#*"]; # LArRawChannelContainer
Stream1.OutputFile    = "LArRawTest.root"
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = INFO
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10
#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
theApp.Dlls   += [ "RootHistCnv" ]
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output =  [ "FILE1 DATAFILE='larraw_write.root' OPT='NEW'" ]
theApp.HistogramPersistency="ROOT"
# Histogram output, if any. 
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "histo.root"

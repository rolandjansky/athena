###############################################################################
#
# jobOptions to compute OFC Timing Offsets using the Time Tuning algorithm
#
###############################################################################

# Data file to be used
#
InputDirectory = "/castor/cern.ch/atlas/testbeam/combined/2004" 
FilePrefix     = "daq_SFI-51_calo"
RunNumber      = 1004054

# OFC to be used
#
useTCMOFC      = False
useRTMOFC      = True
LArTB04FolderTag_OFC_RTM = "TB04-Default"
#LArTB04FolderTag_OFC_RTM = "TB04-2"
#LArTB04FolderTag_OFC_RTM = "OFC-test-3"
#LArTB04FolderTag_OFC_RTM = "OFC-test-4"
#LArTB04FolderTag_OFC_RTM = "OFC-test-5"
#LArTB04FolderTag_OFC_RTM = "OFC-test-6"

# Preset timing offsets...
#
useTimeOffsets = False # use LArTBRec/LArTimeOffsets_OFC_XXX.py (XXX = RTM, TCM)

# ... or directly set the timing offsets by hand:
#
#theApp.TopAlg +=["FakeLArTimeOffset"]
#FakeLArTimeOffset=Algorithm("FakeLArTimeOffset")
#FakeLArTimeOffset.OutputLevel = INFO
#FakeLArTimeOffset.GlobalTimeOffset = 0.
#FakeLArTimeOffset.FEBids          += [ 0x39000000, 0x39008000, 0x39010000, 0x39018000, 0x39020000, 0x39028000, 0x39030000, 0x39038000, 0x39040000, 0x39048000, 0x39050000, 0x39058000, 0x39060000, 0x39068000, 0x39080000, 0x39088000, 0x39090000, 0x39098000, 0x390a0000, 0x390a8000, 0x390b0000, 0x390b8000, 0x390c0000, 0x390c8000, 0x390d0000, 0x390d8000, 0x390e0000, 0x390e8000 ]
#FakeLArTimeOffset.FEbTimeOffsets  += [ 6.13258, 0, 0, 15.3924, 0, 0, 0, 0, 0, 0, 0, 17.7146, 0, 0, 0.949009, 0, 0, 11.9441, 0, 0, 0, 0, 9.79891, 0, 0, 7.14886, 0, 0 ]

#
# Time Tuning main steering options
#
doTest         = True	# use only the first 1000 events
doGlobal       = False	# compute Global Timing Offsets
doFEB          = True	# compute FEB Timing Offsets
doCell         = False	# compute Cell Timing Offsets

###############################################################################

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.em_setOn() 

# Read Beam Instruments
include( "TBCnv/TBReadH8BS_jobOptions.py")

# File location
include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )
ByteStreamInputSvc=Service("ByteStreamInputSvc")
ByteStreamInputSvc.InputDirectory += [ InputDirectory ]
ByteStreamInputSvc.FilePrefix     += [ FilePrefix ]
ByteStreamInputSvc.RunNumber       = [ RunNumber ]

# Read LArDigit
theApp.Dlls += [ "LArByteStream" ]
ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"]

include( "LArDetDescr/LArDetDescr_H8_joboptions.py" )
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DoIdChecks
DetDescrCnvSvc.LArIDFileName ="IdDictParser/IdDictLArCalorimeter_H8_2004.xml"

ToolSvc = Service( "ToolSvc" )
ToolSvc.LArRodDecoder.FirstSample = 2

#
# Time tuning
#

theApp.Dlls += ["LArRawUtils", "LArROD", "LArTools", "LArEventTest", "LArCalibUtils" ]

#
# Fix for swapped LArFEBs
#
ToolSvc = Service( "ToolSvc" )
if ( RunNumber >= 1000931 ):
	if (RunNumber<=1000969):
		ToolSvc.LArRodDecoder.FebExchange = True
		ToolSvc.LArRodDecoder.FebId1 = 0x39020000
		ToolSvc.LArRodDecoder.FebId2 = 0x39040000

# OFC database
#include("LArCondCnv/LArCondCnv_TB04_jobOptions.py")
include("LArCondCnv_TB04_jobOptions.py")

# TBPhase reconstruction
theApp.Dlls+=["TBRec"]
theApp.TopAlg+=["TBPhaseRec"]
TBPhaseRec=Algorithm("TBPhaseRec")
include("TBRec/H8PhaseRec_jobOptions.py")

# Time offsets
if useTimeOffsets:
	if useRTMOFC :
		include("LArTBRec/LArTimeOffsets_OFC_RTM.py")
	if useTCMOFC :
		include("LArTBRec/LArTimeOffsets_OFC_TCM.py")

# Time tuning iterations, monitoring and output
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )

theApp.TopAlg+=["LArTimeTuning","LArTimeTuningNtuple",]
LArTimeTuning = Algorithm("LArTimeTuning")
LArTimeTuningNtuple= Algorithm("LArTimeTuningNtuple");

LArTimeTuning.NOFCPhases          = 50
LArTimeTuning.NOFCTimeBins        = 24
LArTimeTuning.UseOFCPhase         = False
LArTimeTuning.BinHalfOffset       = False
LArTimeTuning.AllowTimeSampleJump = True

LArTimeTuning.OFCTimePrecision    = 0.5*ns
LArTimeTuning.SaveRemainingError  = True
LArTimeTuning.MaxIterations       = 10
LArTimeTuning.ADCCut              = 1400

LArTimeTuning.PhaseInversion      = True
LArTimeTuning.CorrectionSign      = +1

#LArTimeTuning.LayerSelection      = 2
#LArTimeTuning.GainSelection       = "HIGH"

LArTimeTuning.OutputLevel          = INFO

if doGlobal:
	LArTimeTuning.Scope                     = "GLOBAL"
	LArTimeTuning.GlobalTimeOffsetOutKey    = "TimeOffset"
	LArTimeTuningNtuple.GlobalTimeOffsetKey = "TimeOffset"
	NTupleSvc.Output = [ "FILE1 DATAFILE='LArTimeTuning_GLOBAL.root' OPT='NEW'" ]

if doFEB:
	LArTimeTuning.Scope                     = "FEB"
	LArTimeTuning.FebTimeOffsetOutKey       = "LArFebTimeOffset"
	LArTimeTuningNtuple.FebTimeOffsetKey    = "LArFebTimeOffset"
	NTupleSvc.Output = [ "FILE1 DATAFILE='LArTimeTuning_FEB.root' OPT='NEW'" ]

if doCell:
	LArTimeTuning.Scope                     = "CELL"
	LArTimeTuning.CellTimeOffsetOutKey      = "LArCellTimeOffset"
	LArTimeTuningNtuple.CellTimeOffsetKey   = "LArCellTimeOffset"
	NTupleSvc.Output = [ "FILE1 DATAFILE='LArTimeTuning_CELL.root' OPT='NEW'" ]

if doTest :
	theApp.EvtMax = 1000
	
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel  = INFO
MessageSvc.defaultLimit = 1000000;
MessageSvc.Format       = "% F%40W%S%7W%R%T %0W%M"


AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
AthenaEventLoopMgr.FailureMode = 2
AthenaEventLoopMgr.OutputLevel = INFO

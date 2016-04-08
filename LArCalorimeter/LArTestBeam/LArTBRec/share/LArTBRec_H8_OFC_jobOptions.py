#*******************************************************************
#
# JobOption to analyse LAr testbeam data (LArDigit to LArRawChannel)
# using OFC (TCM or RTM).
#
# Last update: 06/10/2005 (Marco.Delmastro@cern.ch)
#
#*******************************************************************

# read LArDigit
#theApp.Dlls += [ "LArByteStream" ]
#ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"] 
ByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
ToolSvc = Service( "ToolSvc" )

#theApp.Dlls += ["LArRawUtils", "LArROD", "LArTools" , "LArEventTest","LArCalibUtils" ]

# OFC folders options (should be set by top jobOptions)
if not 'useTCMOFC' in dir():
	useTCMOFC = False
if not 'useRTMOFC' in dir():
	useRTMOFC = True

# Prevents OFC options unconsistency...
if (useTCMOFC and useRTMOFC) or (not useTCMOFC and not useRTMOFC):
	useTCMOFC = False
	useRTMOFC = True

if not 'LArShapeMode' in dir():
	LArShapeMode = 10

# Time Offset(s)
if useRTMOFC:
	include("LArTBRec/LArTimeOffsets_OFC_RTM.py")
	
if useTCMOFC:
	include("LArTBRec/LArTimeOffsets_OFC_TCM.py")

include("LArConditionsCommon/LArConditionsCommon_H8_jobOptions.py")

# Default CaloNoiseTool, to be used by all:
#from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
#theCaloNoiseTool = CaloNoiseToolDefault('tool')
#theCaloNoiseTool.NMinBias                = 0
#theCaloNoiseTool.UseTile                 = False
#theCaloNoiseTool.IsMC                    = False
#theCaloNoiseTool.UseSymmetry             = False
#theCaloNoiseTool.WorkMode                = 1
#theCaloNoiseTool.WithOF                  = True

#ToolSvc += theCaloNoiseTool
include( "CaloTools/CaloNoiseTool_TB_jobOptions.py" )

#theApp.TopAlg += [ "LArRawChannelBuilder" ]
#LArRawChannelBuilder = Algorithm("LArRawChannelBuilder");
from LArROD.LArRODFlags import larRODFlags
larRODFlags.readDigits.set_On()
larRODFlags.doBuildBadChannel.set_On()

#from LArROD.LArRODConf import LArRawChannelBuilder
#LArRawChannelBuilder = LArRawChannelBuilder()
from LArROD.LArRawChannelGetter import LArRawChannelGetter
LArRawChannelGetter()
topSequence.LArRawChannelBuilder.DataLocation        = "FREE" 
topSequence.LArRawChannelBuilder.UseTDC              = True
topSequence.LArRawChannelBuilder.BinHalfOffset       = False
topSequence.LArRawChannelBuilder.AllowTimeSampleJump = True
topSequence.LArRawChannelBuilder.ADC2MeVTool = ToolSvc.LArADC2MeVTool

# skip events with al leat one saturating channel
topSequence.LArRawChannelBuilder.SkipSaturCellsMode  = 2
topSequence.LArRawChannelBuilder.ADCMax              = 4095

if not 'ECutQuality' in dir():
	ECutQuality = 256 # correponds to hardcoded default
topSequence.LArRawChannelBuilder.Ecut = ECutQuality*MeV

if useRTMOFC:
	topSequence.LArRawChannelBuilder.UseOFCPhase    = False
	topSequence.LArRawChannelBuilder.PhaseInversion = True
	topSequence.LArRawChannelBuilder.NOFCTimeBins   = 24
	topSequence.LArRawChannelBuilder.NOFCPhases     = 50
	#topSequence.LArRawChannelBuilder.ShapeMode      = 10 # special for RTM OFC 50 phases
	topSequence.LArRawChannelBuilder.ShapeMode      = LArShapeMode

if useTCMOFC:
	topSequence.LArRawChannelBuilder.UseOFCPhase    = True
	topSequence.LArRawChannelBuilder.NOFCTimeBins   = 25

#topSequence += LArRawChannelBuilder

# Switch this to VERBOSE to get a dmp of Ramp coefficients in log file
#ToolSvc.LArADC2MeVTool.OutputLevel = VERBOSE

#ToolSvc.LArADC2MeVToolDefault.BeginRunPriority = 100
from LArByteStream.LArByteStreamConf import LArRodDecoder
svcMgr.ToolSvc += LArRodDecoder()
ToolSvc.LArRodDecoder.FirstSample = 2
#ToolSvc.LArRoI_Map.Print = False

if not 'useLArStripsXtalkRampCorr' in dir():
	useLArStripsXtalkRampCorr = True

if (useLArStripsXtalkRampCorr):
	ToolSvc.LArADC2MeVTool.UseMphysOverMcal = True

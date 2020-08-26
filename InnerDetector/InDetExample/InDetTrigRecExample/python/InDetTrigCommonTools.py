# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


"""
#
#the configuration of tools shared between L2 and EF
#

"""
___author___ = "Jiri Masik"
___version___ = "$Id: $"


from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.Logging import logging
log = logging.getLogger('InDetTrigCommonTools')

# --- set Data/MC flag
isMC = False
if globalflags.DataSource == "geant4" :
    isMC = True

# Calibration DB Service
from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
InDetTRTCalDbTool = TRT_CalDbTool(name = "TRT_CalDbTool")


# TRT_DriftFunctionTool
from TRT_DriftFunctionTool.TRT_DriftFunctionToolConf import TRT_DriftFunctionTool

InDetTrigTRT_DriftFunctionTool = TRT_DriftFunctionTool(name = "InDetTrigTRT_DriftFunctionTool",
                                                       TRTCalDbTool        = InDetTRTCalDbTool,
                                                       AllowDataMCOverride = True,
                                                       ForceData = True,
                                                       IsMC = isMC)

# Second calibration DB Service in case pile-up and physics hits have different calibrations
if DetFlags.overlay.TRT_on() :

    InDetTrigTRTCalDbTool2 = TRT_CalDbTool(name = "TRT_CalDbSvc2")
    InDetTrigTRTCalDbTool2.RtFolderName = "/TRT/Calib/MC/RT"             
    InDetTrigTRTCalDbTool2.T0FolderName = "/TRT/Calib/MC/T0"             
    InDetTrigTRT_DriftFunctionTool.TRTCalDbTool2 = InDetTrigTRTCalDbTool2
    InDetTrigTRT_DriftFunctionTool.IsOverlay = True
    InDetTrigTRT_DriftFunctionTool.IsMC = False

# --- set HT corrections
InDetTrigTRT_DriftFunctionTool.HTCorrectionBarrelXe = 1.5205
InDetTrigTRT_DriftFunctionTool.HTCorrectionEndcapXe = 1.2712
InDetTrigTRT_DriftFunctionTool.HTCorrectionBarrelAr = 1.5205
InDetTrigTRT_DriftFunctionTool.HTCorrectionEndcapAr = 1.2712
         
# --- set ToT corrections
InDetTrigTRT_DriftFunctionTool.ToTCorrectionsBarrelXe = [0., 4.358121, 3.032195, 1.631892, 0.7408397, -0.004113, -0.613288, -0.73758, -0.623346, -0.561229,-0.29828, -0.21344, -0.322892, -0.386718, -0.534751, -0.874178, -1.231799, -1.503689, -1.896464, -2.385958]
InDetTrigTRT_DriftFunctionTool.ToTCorrectionsEndcapXe = [0., 5.514777, 3.342712, 2.056626, 1.08293693, 0.3907979, -0.082819, -0.457485, -0.599706, -0.427493, -0.328962, -0.403399, -0.663656, -1.029428, -1.46008, -1.919092, -2.151582, -2.285481, -2.036822, -2.15805]
InDetTrigTRT_DriftFunctionTool.ToTCorrectionsBarrelAr = [0., 4.358121, 3.032195, 1.631892, 0.7408397, -0.004113, -0.613288, -0.73758, -0.623346, -0.561229, -0.29828, -0.21344, -0.322892, -0.386718, -0.534751, -0.874178, -1.231799, -1.503689, -1.896464, -2.385958]
InDetTrigTRT_DriftFunctionTool.ToTCorrectionsEndcapAr = [0., 5.514777, 3.342712, 2.056626, 1.08293693, 0.3907979, -0.082819, -0.457485, -0.599706, -0.427493, -0.328962, -0.403399, -0.663656, -1.029428, -1.46008, -1.919092, -2.151582, -2.285481, -2.036822, -2.15805]


ToolSvc += InDetTrigTRT_DriftFunctionTool

  
# Straw status DB Tool
from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummaryTool
InDetTrigTRTStrawStatusSummaryTool = TRT_StrawStatusSummaryTool(name = "InDetTrigTRTStrawStatusSummaryTool",
                                                                  isGEANT4 = (globalflags.DataSource == 'geant4'))


# TRT_RodDecoder
from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRT_RodDecoder

InDetTrigTRTRodDecoder = TRT_RodDecoder(name = "InDetTrigTRTRodDecoder",
                                        LoadCompressTableDB = (globalflags.DataSource() != 'geant4'))
ToolSvc += InDetTrigTRTRodDecoder

# TRT_DriftCircleTool
from TRT_DriftCircleTool.TRT_DriftCircleToolConf import InDet__TRT_DriftCircleTool
#these settings offline keeps for MC
#MinTrailingEdge = 16.0*ns
#MaxDriftTime = 65.0*ns
import AthenaCommon.SystemOfUnits as Units
MinTrailingEdge = 11.0*Units.ns
MaxDriftTime = 60.0*Units.ns
LowGate         = 14.0625*Units.ns # 4.5*3.125 ns
HighGate        = 42.1875*Units.ns # LowGate + 9*3.125 ns
LowGateArgon         = LowGate
HighGateArgon        = HighGate

from AthenaCommon.GlobalFlags import globalflags
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags

if globalflags.DataSource == 'data':
    MinTrailingEdge = 11.0*Units.ns
    MaxDriftTime    = 60.0*Units.ns
    LowGate         = 17.1875*Units.ns
    HighGate        = 45.3125*Units.ns
    LowGateArgon    = 18.75*Units.ns
    HighGateArgon   = 43.75*Units.ns


  
InDetTrigTRT_DriftCircleTool = InDet__TRT_DriftCircleTool( name = "InDetTrigTRT_DriftCircleTool",
                                                           TRTDriftFunctionTool = InDetTrigTRT_DriftFunctionTool,
                                                           ConditionsSummaryTool           = InDetTrigTRTStrawStatusSummaryTool,
                                                           UseConditionsStatus  = True,
                                                           UseConditionsHTStatus  = True,
                                                           SimpleOutOfTimePileupSupression = False,
                                                           RejectIfFirstBit                = False, # fixes 50 nsec issue 
                                                           MinTrailingEdge                 = MinTrailingEdge,
                                                           MaxDriftTime                    = MaxDriftTime,
                                                           ValidityGateSuppression         = InDetTrigFlags.InDet25nsec(),
                                                           LowGate = LowGate,
                                                           HighGate = HighGate,
                                                           SimpleOutOfTimePileupSupressionArgon = False,# no OOT rejection for argon
                                                           RejectIfFirstBitArgon                = False, # no OOT rejection for argon
                                                           MinTrailingEdgeArgon                 = MinTrailingEdge,
                                                           MaxDriftTimeArgon                    = MaxDriftTime,
                                                           ValidityGateSuppressionArgon         = InDetTrigFlags.InDet25nsec(),
                                                           LowGateArgon                         = LowGateArgon,
                                                           HighGateArgon                        = HighGateArgon,
                                                           useDriftTimeHTCorrection        = True,
                                                           useDriftTimeToTCorrection       = True, # reenable ToT
                                                           )


ToolSvc += InDetTrigTRT_DriftCircleTool
log.debug(InDetTrigTRT_DriftCircleTool)


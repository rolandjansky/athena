# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
#
#the configuration of tools shared between L2 and EF
#
"""
___author___ = "Jiri Masik"
___version___ = "$Id: $"


from InDetTrigRecExample.InDetTrigConditionsAccess import TRT_ConditionsSetup
from AthenaCommon.AppMgr import ToolSvc

from TRT_DriftFunctionTool.TRT_DriftFunctionToolConf import TRT_DriftFunctionTool
InDetTrigTRT_DriftFunctionTool = TRT_DriftFunctionTool(name = "InDetTrigTRT_DriftFunctionTool",
                                                       AllowDataMCOverride = True,
                                                       ForceData = True )

#
from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRT_RodDecoder
from AthenaCommon.GlobalFlags import globalflags
InDetTrigTRTRodDecoder = TRT_RodDecoder(name = "InDetTrigTRTRodDecoder",
                                        LoadCompressTableDB = (globalflags.DataSource() != 'geant4'))
ToolSvc += InDetTrigTRTRodDecoder

#InDetTrigTRT_DriftFunctionTool.TRTCalDbTool='TRT_CalDbSvc/'+TRT_ConditionsSetup.instanceName("TRT_CalDbSvc")
InDetTrigTRT_DriftFunctionTool.TRTCalDbTool='TRT_CalDbSvc/TRT_CalDbSvc'
ToolSvc += InDetTrigTRT_DriftFunctionTool

# TRT_DriftCircleTool
from TRT_DriftCircleTool.TRT_DriftCircleToolConf import InDet__TRT_DriftCircleTool
#these settings offline keeps for MC
#MinTrailingEdge = 16.0*ns
#MaxDriftTime = 65.0*ns
import AthenaCommon.SystemOfUnits as Units
MinTrailingEdge = 11.0*Units.ns
MaxDriftTime = 60.0*Units.ns
LowGate = 18.0*Units.ns
HighGate = 38.0*Units.ns
from AthenaCommon.GlobalFlags import globalflags
if  globalflags.DataSource != 'data':
    MinTrailingEdge = 16.0*Units.ns
    MaxDriftTime    = 65.0*Units.ns
    LowGate         = 23.0*Units.ns
    HighGate        = 43.0*Units.ns

  
InDetTrigTRT_DriftCircleTool = InDet__TRT_DriftCircleTool( name = "InDetTrigTRT_DriftCircleTool",
                                                           TRTDriftFunctionTool = InDetTrigTRT_DriftFunctionTool,
                                                           UseConditionsStatus  = True,
                                                           UseConditionsHTStatus  = True,
                                                           SimpleOutOfTimePileupSupression = True,
                                                           RejectIfFirstBit                = False, # fixes 50 nsec issue 
                                                           MinTrailingEdge                 = MinTrailingEdge,
                                                           MaxDriftTime                    = MaxDriftTime,
                                                           ValidityGateSuppression         = False,
                                                           LowGate = LowGate,
                                                           HighGate = HighGate,
                                                           MaskFirstHTBit                  = False,
                                                           MaskMiddleHTBit                 = False,
                                                           MaskLastHTBit                   = False,
                                                           SimpleOutOfTimePileupSupressionArgon = False,# no OOT rejection for argon
                                                           RejectIfFirstBitArgon                = False, # no OOT rejection for argon
                                                           MinTrailingEdgeArgon                 = 0, # no OOT rejection for argon
                                                           MaxDriftTimeArgon                    = 99*Units.ns,# no OOT rejection for argon
                                                           ValidityGateSuppressionArgon         = False,# no OOT rejection for argon
                                                           LowGateArgon                         = 0,# no OOT rejection for argon
                                                           HighGateArgon                        = 75*Units.ns,# no OOT rejection for argon
                                                           MaskFirstHTBitArgon                  = False,
                                                           MaskMiddleHTBitArgon                 = False,
                                                           MaskLastHTBitArgon                   = False,
                                                           useDriftTimeHTCorrection        = True,
                                                           
                                                           useDriftTimeToTCorrection       = True, # reenable ToT
                                                           )

from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
if InDetTrigFlags.InDet25nsec():
    InDetTrigTRT_DriftCircleTool.ValidityGateSuppression=True  
    InDetTrigTRT_DriftCircleTool.SimpleOutOfTimePileupSupression=False  
#if jobproperties.Beam.beamType()=="cosmics": 
#    InDetTRT_DriftCircleTool.SimpleOutOfTimePileupSupression=False 


InDetTrigTRT_DriftCircleTool.ConditionsSummaryTool= \
    "TRT_StrawStatusSummarySvc/"+TRT_ConditionsSetup.instanceName("InDetTRTStrawStatusSummarySvc")
#    "TRT_ConditionsSummarySvc/"+TRT_ConditionsSetup.instanceName("InDetTRTConditionsSummaryService")

ToolSvc += InDetTrigTRT_DriftCircleTool
print InDetTrigTRT_DriftCircleTool



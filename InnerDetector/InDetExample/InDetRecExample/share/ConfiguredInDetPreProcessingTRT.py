# Blocking the include for after first inclusion
include.block ('InDetRecExample/ConfiguredInDetPreProcessingTRT.py')

# ------------------------------------------------------------
#
# ----------- TRT Data-Preparation stage
#
# ------------------------------------------------------------
#
class ConfiguredInDetPreProcessingTRT:

   def __init__(self, useTimeInfo = True, usePhase = False):

      from InDetRecExample.InDetJobProperties import InDetFlags
      from AthenaCommon.DetFlags              import DetFlags
      from AthenaCommon.GlobalFlags           import globalflags
      from AthenaCommon.BeamFlags             import jobproperties
      #
      # get ToolSvc and topSequence
      #
      from AthenaCommon.AppMgr      import ToolSvc
      from AthenaCommon.AlgSequence import AlgSequence
      topSequence = AlgSequence()
      
      if InDetFlags.doPRDFormation() and DetFlags.makeRIO.TRT_on() and InDetFlags.doTRT_PRDFormation():

         #
         # --- setup naming of tools and algs
         #
         if useTimeInfo:
            prefix     = "InDetTRT_"
            collection = InDetKeys.TRT_DriftCircles()
            if InDetFlags.doSplitReco() :
               collectionPU = InDetKeys.TRT_PU_DriftCircles()
         else:
            prefix     = "InDetTRT_noTime_"
            collection = InDetKeys.TRT_DriftCirclesUncalibrated()
            if InDetFlags.doSplitReco() :
               collectionPU = InDetKeys.TRT_PU_DriftCirclesUncalibrated()
         #
         # --- TRT_DriftFunctionTool
         #


         from TRT_DriftFunctionTool.TRT_DriftFunctionToolConf import TRT_DriftFunctionTool
         InDetTRT_DriftFunctionTool = TRT_DriftFunctionTool(name                = prefix+"DriftFunctionTool",
                                                            TRTCalDbTool        = InDetTRTCalDbSvc)
         # --- overwrite for uncalibrated DC production
         if (not useTimeInfo) or InDetFlags.noTRTTiming():
            InDetTRT_DriftFunctionTool.DummyMode      = True
            InDetTRT_DriftFunctionTool.UniversalError = 1.15

         # --- set Data/MC flag
         if(globalflags.DataSource != 'geant4') :
             InDetTRT_DriftFunctionTool.IsMC = False
         else :
             InDetTRT_DriftFunctionTool.IsMC = True

         # --- overwrite for calibration of MC
         if usePhase and jobproperties.Beam.beamType()=='cosmics' and globalflags.DataSource == "geant4":
            InDetTRT_DriftFunctionTool.AllowDigiVersionOverride = True
            InDetTRT_DriftFunctionTool.ForcedDigiVersion        = 9
            
         # --- set HT corrections
         InDetTRT_DriftFunctionTool.HTCorrectionBarrelXe = 1.5205
         InDetTRT_DriftFunctionTool.HTCorrectionEndcapXe = 1.2712
         InDetTRT_DriftFunctionTool.HTCorrectionBarrelAr = 1.5205
         InDetTRT_DriftFunctionTool.HTCorrectionEndcapAr = 1.2712
         
         # --- set ToT corrections
         InDetTRT_DriftFunctionTool.ToTCorrectionsBarrelXe = [0., 4.358121, 3.032195, 1.631892, 0.7408397, -0.004113, -0.613288, -0.73758, -0.623346, -0.561229,
                                                              -0.29828, -0.21344, -0.322892, -0.386718, -0.534751, -0.874178, -1.231799, -1.503689, -1.896464, -2.385958]
         InDetTRT_DriftFunctionTool.ToTCorrectionsEndcapXe = [0., 5.514777, 3.342712, 2.056626, 1.08293693, 0.3907979, -0.082819, -0.457485, -0.599706, -0.427493, 
                                                              -0.328962, -0.403399, -0.663656, -1.029428, -1.46008, -1.919092, -2.151582, -2.285481, -2.036822, -2.15805]
         InDetTRT_DriftFunctionTool.ToTCorrectionsBarrelAr = [0., 4.358121, 3.032195, 1.631892, 0.7408397, -0.004113, -0.613288, -0.73758, -0.623346, -0.561229,
                                                              -0.29828, -0.21344, -0.322892, -0.386718, -0.534751, -0.874178, -1.231799, -1.503689, -1.896464, -2.385958]
         InDetTRT_DriftFunctionTool.ToTCorrectionsEndcapAr = [0., 5.514777, 3.342712, 2.056626, 1.08293693, 0.3907979, -0.082819, -0.457485, -0.599706, -0.427493, 
                                                              -0.328962, -0.403399, -0.663656, -1.029428, -1.46008, -1.919092, -2.151582, -2.285481, -2.036822, -2.15805]

         ToolSvc += InDetTRT_DriftFunctionTool
         if (InDetFlags.doPrintConfigurables()):
            print InDetTRT_DriftFunctionTool
         #
         # --- TRT_DriftCircleTool
         #
         if usePhase:
            from TRT_DriftCircleTool.TRT_DriftCircleToolConf import InDet__TRT_DriftCircleToolCosmics as InDet__TRT_DriftCircleTool
         else:
            from TRT_DriftCircleTool.TRT_DriftCircleToolConf import InDet__TRT_DriftCircleTool

         #
         # set gating values for MC/DATA
         MinTrailingEdge = 11.0*ns
         MaxDriftTime    = 60.0*ns
         LowGate         = 14.0625*ns # 4.5*3.125 ns
         HighGate        = 42.1875*ns # LowGate + 9*3.125 ns
         if InDetFlags.doCosmics():
            LowGate         = 19.0*ns
            HighGate        = 44.0*ns
         if globalflags.DataSource == 'data':
            MinTrailingEdge = 11.0*ns
            MaxDriftTime    = 60.0*ns
            LowGate         = 14.0625*ns # 4.5*3.125 ns
            HighGate        = 42.1875*ns # LowGate + 9*3.125 ns
            if InDetFlags.doCosmics():
               LowGate         = 19.0*ns
               HighGate        = 44.0*ns

         InDetTRT_DriftCircleTool = InDet__TRT_DriftCircleTool(name                            = prefix+"DriftCircleTool",
                                                               TRTDriftFunctionTool            = InDetTRT_DriftFunctionTool,
                                                               TrtDescrManageLocation          = InDetKeys.TRT_Manager(),
                                                               ConditionsSummaryTool           = InDetTRTStrawStatusSummarySvc,
                                                               #used to be InDetTRTConditionsSummaryService,
                                                               UseConditionsStatus             = True,
                                                               UseConditionsHTStatus           = True,
                                                               SimpleOutOfTimePileupSupression = InDetFlags.doCosmics(),
                                                               RejectIfFirstBit                = False, # fixes 50 nsec issue 
                                                               MinTrailingEdge                 = MinTrailingEdge,
                                                               MaxDriftTime                    = MaxDriftTime,
                                                               ValidityGateSuppression         = not InDetFlags.doCosmics(),
                                                               LowGate                         = LowGate,
                                                               HighGate                        = HighGate,
                                                               MaskFirstHTBit                  = False,
                                                               MaskMiddleHTBit                 = False,
                                                               MaskLastHTBit                   = False,
                                                               SimpleOutOfTimePileupSupressionArgon = InDetFlags.doCosmics(),
                                                               RejectIfFirstBitArgon                = False, # fixes 50 nsec issue 
                                                               MinTrailingEdgeArgon                 = MinTrailingEdge,
                                                               MaxDriftTimeArgon                    = MaxDriftTime,
                                                               ValidityGateSuppressionArgon         = not InDetFlags.doCosmics(),
                                                               LowGateArgon                         = LowGate,
                                                               HighGateArgon                        = HighGate,
                                                               MaskFirstHTBitArgon                  = False,
                                                               MaskMiddleHTBitArgon                 = False,
                                                               MaskLastHTBitArgon                   = False,
                                                               useDriftTimeHTCorrection        = True,
                                                               useDriftTimeToTCorrection       = True) # reenable ToT

         from AthenaCommon.BeamFlags import jobproperties 
         if InDetFlags.InDet25nsec() and jobproperties.Beam.beamType()=="collisions": 
            InDetTRT_DriftCircleTool.ValidityGateSuppression=True  
            InDetTRT_DriftCircleTool.SimpleOutOfTimePileupSupression=False  
         if jobproperties.Beam.beamType()=="cosmics": 
            InDetTRT_DriftCircleTool.SimpleOutOfTimePileupSupression=False 

# --- overwrite for phase usage
         if usePhase:
            InDetTRT_DriftCircleTool.ComTimeName = "TRT_Phase"

         ToolSvc += InDetTRT_DriftCircleTool
         if (InDetFlags.doPrintConfigurables()):
            print InDetTRT_DriftCircleTool
         
         #
         # --- TRT_RIO_Maker Algorithm
         #
         from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__TRT_RIO_Maker
         InDetTRT_RIO_Maker = InDet__TRT_RIO_Maker(name                   = prefix+"RIO_Maker",
                                                   TRT_DriftCircleTool    = InDetTRT_DriftCircleTool,
                                                   TrtDescrManageLocation = InDetKeys.TRT_Manager(),
                                                   TRTRDOLocation         = InDetKeys.TRT_RDOs(),
                                                   TRTRIOLocation         = collection)
         topSequence += InDetTRT_RIO_Maker
         if (InDetFlags.doPrintConfigurables()):
            print InDetTRT_RIO_Maker
         if InDetFlags.doSplitReco() :
            InDetTRT_RIO_MakerPU = InDet__TRT_RIO_Maker(name                   = prefix+"RIO_MakerPU",
                                                        TRT_DriftCircleTool    = InDetTRT_DriftCircleTool,
                                                        TrtDescrManageLocation = InDetKeys.TRT_Manager(),
                                                        TRTRDOLocation         = InDetKeys.TRT_PU_RDOs(),
                                                        TRTRIOLocation         = collectionPU)
            topSequence += InDetTRT_RIO_MakerPU
            if (InDetFlags.doPrintConfigurables()):
               print InDetTRT_RIO_MakerPU

         #
         #    Include alg to save the local occupancy inside xAOD::EventInfo
         #
         if InDetFlags.doTRTGlobalOccupancy():
          from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_LocalOccupancy
          InDetTRT_LocalOccupancy = InDet__TRT_LocalOccupancy(  name 		= "InDet_TRT_LocalOccupancy",
								isTrigger	= False,
                                                                TRTDriftFunctionTool = InDetTRT_DriftFunctionTool
	  )

          ToolSvc += InDetTRT_LocalOccupancy
          if (InDetFlags.doPrintConfigurables()):
              print InDetTRT_LocalOccupancy


          from TRT_CalibAlgs.TRT_CalibAlgsConf import TRTOccupancyInclude
          TRTOccupancyInclude = TRTOccupancyInclude( name = prefix+"TRTOccupancyInclude",
                                                   TRT_LocalOccupancyTool = InDetTRT_LocalOccupancy)
         
          topSequence += TRTOccupancyInclude
          if (InDetFlags.doPrintConfigurables()):
                  print TRTOccupancyInclude

       
         #
         # --- we need to do truth association if requested (not for uncalibrated hits in cosmics)
         #
         if InDetFlags.doTruth() and useTimeInfo:
            from InDetTruthAlgs.InDetTruthAlgsConf import InDet__PRD_MultiTruthMaker
            InDetPRD_MultiTruthMakerTRT = InDet__PRD_MultiTruthMaker (name                        = prefix+"PRD_MultiTruthMaker",
                                                                      PixelClusterContainerName   = "",
                                                                      SCTClusterContainerName     = "",
                                                                      TRTDriftCircleContainerName = InDetKeys.TRT_DriftCircles(),
                                                                      SimDataMapNamePixel         = "",
                                                                      SimDataMapNameSCT           = "",
                                                                      SimDataMapNameTRT           = InDetKeys.TRT_SDOs(),
                                                                      TruthNamePixel              = "",
                                                                      TruthNameSCT                = "",
                                                                      TruthNameTRT                = InDetKeys.TRT_DriftCirclesTruth())
            topSequence += InDetPRD_MultiTruthMakerTRT
            if (InDetFlags.doPrintConfigurables()):
               print InDetPRD_MultiTruthMakerTRT

            if InDetFlags.doSplitReco() :
               InDetPRD_MultiTruthMakerTRTPU = InDet__PRD_MultiTruthMaker (name                        = prefix+"PRD_MultiTruthMakerPU",
                                                                           PixelClusterContainerName   = "",
                                                                           SCTClusterContainerName     = "",
                                                                           TRTDriftCircleContainerName = InDetKeys.TRT_PU_DriftCircles(),
                                                                           SimDataMapNamePixel         = "",
                                                                           SimDataMapNameSCT           = "",
                                                                           SimDataMapNameTRT           = InDetKeys.TRT_PU_SDOs(),
                                                                           TruthNamePixel              = "",
                                                                           TruthNameSCT                = "",
                                                                           TruthNameTRT                = InDetKeys.TRT_PU_DriftCirclesTruth())
               topSequence += InDetPRD_MultiTruthMakerTRTPU
               if (InDetFlags.doPrintConfigurables()):
                  print InDetPRD_MultiTruthMakerTRTPU

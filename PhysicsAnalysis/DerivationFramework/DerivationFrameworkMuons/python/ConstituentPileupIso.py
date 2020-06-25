#Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def ConstituentPileupIso(privateSeq):
  from AthenaCommon.AppMgr import ToolSvc
  from JetRecTools.JetRecToolsConfig import ctm
  from JetRecTools.JetRecToolsConf import ConstituentSubtractorTool, ConstitTimeCutTool, SoftKillerWeightTool
  ctm.add( ConstitTimeCutTool("JetConstit_Timing"),
           alias = 'timing' )
  ctm.add( ConstituentSubtractorTool("JetConstit_ConstSub"),
           alias = 'constsub' )
  ctm.add( ConstitTimeCutTool("JetPF_Timing", ApplyToChargedPFO=False),
           alias = 'timingPF' )
  ctm.add( SoftKillerWeightTool("JetPF_SoftKiller", SKGridSize=0.6, ApplyToChargedPFO=False) ,
           alias = 'softkillerPF' )
  ctm.add( ConstituentSubtractorTool("JetPF_ConstSub", ApplyToChargedPFO=False),
           alias = 'constsubPF' )
  clustOrigSeq = ctm.buildConstitModifSequence( 'ConstitOrigSeq',
                                                OutputContainer = 'OrigTopoClusters',
                                                InputContainer= 'CaloCalTopoClusters',
                                                modList = [  'clus_origin'] )
  
  xAOD_Type_CaloCluster = 1
  clustTimeSeq = ctm.buildConstitModifSequence( 'ConstitTimeSeq',
                                              OutputContainer = 'OrigTimeTopoClusters',
                                              InputContainer= 'OrigTopoClusters',
                                              modList = [  'timing'] , InputType=xAOD_Type_CaloCluster)
  clustTimeSKSeq = ctm.buildConstitModifSequence( 'ConstitOrigTimeSKSeq',
                                              OutputContainer = 'OrigTimeSKTopoClusters',
                                              InputContainer= 'OrigTimeTopoClusters',
                                              modList = [  'softkiller'] , InputType=xAOD_Type_CaloCluster)
  clustTimeCSSeq = ctm.buildConstitModifSequence( 'ConstitOrigTimeCSSeq',
                                              OutputContainer = 'OrigTimeCSTopoClusters',
                                              InputContainer= 'OrigTimeTopoClusters',
                                              modList = [  'constsub'] , InputType=xAOD_Type_CaloCluster)
  clustTimeCSSKSeq = ctm.buildConstitModifSequence( 'ConstitOrigTimeCSSKSeq',
                                              OutputContainer = 'OrigTimeCSSKTopoClusters',
                                              InputContainer= 'OrigTimeCSTopoClusters',
                                              modList = [  'softkiller'] , InputType=xAOD_Type_CaloCluster)
  
  pfTimeSeq = ctm.buildConstitModifSequence( 'pfTimeSeq',
                                              InputContainer= 'JetETMiss',
                                              OutputContainer = 'TimePF',
                                              modList = [  'timingPF'], InputType=3)
  pfTimeSKSeq = ctm.buildConstitModifSequence( "pfTimeSKSeq",
                                        InputContainer = "TimePF",
                                        OutputContainer = 'TimePFSK',
                                        modList = ['softkillerPF'] , InputType=3)
  pfTimeCSSeq = ctm.buildConstitModifSequence( "pfTimeCSSeq",
                                        InputContainer = "TimePF",
                                        OutputContainer = 'TimePFCS',
                                        modList = ['constsubPF'] , InputType=3)
  pfTimeCSSKSeq = ctm.buildConstitModifSequence( 'pfTimeCSSKSeq',
                                              InputContainer= 'TimePFCS',
                                              OutputContainer = 'TimePFCSSK',
                                              modList = [  'softkillerPF'], InputType=3)
  
  from JetRec.JetRecConf import JetAlgorithm
  clustSeqAlg = JetAlgorithm("ClusterModifiers", Tools = [clustOrigSeq, clustTimeSeq, clustTimeSKSeq, clustTimeCSSeq, clustTimeCSSKSeq])
  pfSeqAlg = JetAlgorithm("pfModifiers", Tools = [pfTimeSeq, pfTimeSKSeq, pfTimeCSSeq, pfTimeCSSKSeq])
  privateSeq += clustSeqAlg
  privateSeq += pfSeqAlg
  
  from EventShapeTools.EventDensityConfig import configEventDensityTool, EventDensityAthAlg
  from JetRec.JetRecStandard import jtm
  tccc = configEventDensityTool("EDtpIsoCentralTool", jtm.lcget,
                                radius          = 0.5,
                                AbsRapidityMin  = 0.0,
                                AbsRapidityMax  = 1.5,
                                OutputContainer = "LCTopoClusterIsoCentralEventShape",
                               )
  ToolSvc += tccc
  privateSeq += EventDensityAthAlg("LCEDtpIsoCentralAlg", EventDensityTool = tccc)
  
  tfcc = configEventDensityTool("EDtpIsoForwardTool", jtm.lcget,
                                radius          = 0.5,
                                AbsRapidityMin  = 1.5,
                                AbsRapidityMax  = 3.0,
                                OutputContainer = "LCTopoClusterIsoForwardEventShape",
                                )
  ToolSvc += tfcc
  privateSeq += EventDensityAthAlg("LCEDtpIsoForwardAlg", EventDensityTool = tfcc)
  
  from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
  LCCaloClustersInConeTool = xAOD__CaloClustersInConeTool(name = "LCCaloClustersInConeTool",
                                       CaloClusterLocation = "CaloCalTopoClusters")
  ToolSvc += LCCaloClustersInConeTool
  OrigTimeCSCaloClustersInConeTool = xAOD__CaloClustersInConeTool(name = "OrigTimeCSCaloClustersInConeTool",
                                       CaloClusterLocation = "OrigTimeCSTopoClusters")
  ToolSvc += OrigTimeCSCaloClustersInConeTool
  OrigTimeSKCaloClustersInConeTool = xAOD__CaloClustersInConeTool(name = "OrigTimeSKCaloClustersInConeTool",
                                       CaloClusterLocation = "OrigTimeSKTopoClusters")
  ToolSvc += OrigTimeSKCaloClustersInConeTool
  OrigTimeCSSKCaloClustersInConeTool = xAOD__CaloClustersInConeTool(name = "OrigTimeCSSKCaloClustersInConeTool",
                                       CaloClusterLocation = "OrigTimeCSSKTopoClusters")
  ToolSvc += OrigTimeCSSKCaloClustersInConeTool
  
  from PFlowUtils.PFlowUtilsConf import CP__RetrievePFOTool as RetrievePFOTool
  TimePFCSTool = RetrievePFOTool("TimePFCSTool",NeutralInputContainer="TimePFCSNeutralParticleFlowObjects")
  ToolSvc += TimePFCSTool
  TimePFSKTool = RetrievePFOTool("TimePFSKTool",NeutralInputContainer="TimePFSKNeutralParticleFlowObjects")
  ToolSvc += TimePFSKTool
  TimePFCSSKTool = RetrievePFOTool("TimePFCSSKTool",NeutralInputContainer="TimePFCSSKNeutralParticleFlowObjects")
  ToolSvc += TimePFCSSKTool
  
  from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__PFlowObjectsInConeTool
  TimePFCSObjectsInConeTool = xAOD__PFlowObjectsInConeTool(
                                       name = "TimePFCSObjectsInConeTool",
                                       RetrievePFOTool = TimePFCSTool)
  ToolSvc += TimePFCSObjectsInConeTool
  TimePFSKObjectsInConeTool = xAOD__PFlowObjectsInConeTool(
                                       name = "TimePFSKObjectsInConeTool",
                                       RetrievePFOTool = TimePFSKTool)
  ToolSvc += TimePFSKObjectsInConeTool
  TimePFCSSKObjectsInConeTool = xAOD__PFlowObjectsInConeTool(
                                       name = "TimePFCSSKObjectsInConeTool",
                                       RetrievePFOTool = TimePFCSSKTool)
  ToolSvc += TimePFCSSKObjectsInConeTool
  
  from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
  LCCaloIsolationTool = xAOD__CaloIsolationTool(name = "LCCaloIsolationTool",
                                  CaloFillRectangularClusterTool  = None,
                                  ClustersInConeTool              = LCCaloClustersInConeTool,
                                  PFlowObjectsInConeTool          = None,
                                  IsoLeakCorrectionTool           = None,
                                  saveOnlyRequestedCorrections    = False,
                                  EMCaloNums                      = [],
                                  HadCaloNums                     = [],
                                  TopoClusterEDCentralContainer   = "LCTopoClusterIsoCentralEventShape",
                                  TopoClusterEDForwardContainer   = "LCTopoClusterIsoForwardEventShape",
                                  UseEMScale                      = False,
                                  addCaloExtensionDecoration      = False)
  ToolSvc += LCCaloIsolationTool
  
  OrigTimeCSCaloIsolationTool = xAOD__CaloIsolationTool(name = "OrigTimeCSCaloIsolationTool",
                                  CaloFillRectangularClusterTool  = None,
                                  ClustersInConeTool              = OrigTimeCSCaloClustersInConeTool,
                                  PFlowObjectsInConeTool          = None,
                                  IsoLeakCorrectionTool           = None,
                                  saveOnlyRequestedCorrections    = False,
                                  EMCaloNums                      = [],
                                  HadCaloNums                     = [],
                                  UseEMScale                      = False,
                                  addCaloExtensionDecoration      = False)
  ToolSvc += OrigTimeCSCaloIsolationTool
  OrigTimeSKCaloIsolationTool = xAOD__CaloIsolationTool(name = "OrigTimeSKCaloIsolationTool",
                                  CaloFillRectangularClusterTool  = None,
                                  ClustersInConeTool              = OrigTimeSKCaloClustersInConeTool,
                                  PFlowObjectsInConeTool          = None,
                                  IsoLeakCorrectionTool           = None,
                                  saveOnlyRequestedCorrections    = False,
                                  EMCaloNums                      = [],
                                  HadCaloNums                     = [],
                                  UseEMScale                      = False,
                                  addCaloExtensionDecoration      = False)
  ToolSvc += OrigTimeSKCaloIsolationTool
  OrigTimeCSSKCaloIsolationTool = xAOD__CaloIsolationTool(name = "OrigTimeCSSKCaloIsolationTool",
                                  CaloFillRectangularClusterTool  = None,
                                  ClustersInConeTool              = OrigTimeCSSKCaloClustersInConeTool,
                                  PFlowObjectsInConeTool          = None,
                                  IsoLeakCorrectionTool           = None,
                                  saveOnlyRequestedCorrections    = False,
                                  EMCaloNums                      = [],
                                  HadCaloNums                     = [],
                                  UseEMScale                      = False,
                                  addCaloExtensionDecoration      = False)
  ToolSvc += OrigTimeCSSKCaloIsolationTool

  TimePFCSCaloIsolationTool = xAOD__CaloIsolationTool(name = "TimePFCSCaloIsolationTool",
                                  CaloFillRectangularClusterTool  = None,
                                  ClustersInConeTool              = None,
                                  PFlowObjectsInConeTool          = TimePFCSObjectsInConeTool,
                                  IsoLeakCorrectionTool           = None,
                                  saveOnlyRequestedCorrections    = False,
                                  EMCaloNums                      = [],
                                  HadCaloNums                     = [],
                                  UseEMScale                      = True,
                                  addCaloExtensionDecoration      = False)
  ToolSvc += TimePFCSCaloIsolationTool
  TimePFSKCaloIsolationTool = xAOD__CaloIsolationTool(name = "TimePFSKCaloIsolationTool",
                                  CaloFillRectangularClusterTool  = None,
                                  ClustersInConeTool              = None,
                                  PFlowObjectsInConeTool          = TimePFSKObjectsInConeTool,
                                  IsoLeakCorrectionTool           = None,
                                  saveOnlyRequestedCorrections    = False,
                                  EMCaloNums                      = [],
                                  HadCaloNums                     = [],
                                  UseEMScale                      = True,
                                  addCaloExtensionDecoration      = False)
  ToolSvc += TimePFSKCaloIsolationTool
  TimePFCSSKCaloIsolationTool = xAOD__CaloIsolationTool(name = "TimePFCSSKCaloIsolationTool",
                                  CaloFillRectangularClusterTool  = None,
                                  ClustersInConeTool              = None,
                                  PFlowObjectsInConeTool          = TimePFCSSKObjectsInConeTool,
                                  IsoLeakCorrectionTool           = None,
                                  saveOnlyRequestedCorrections    = False,
                                  EMCaloNums                      = [],
                                  HadCaloNums                     = [],
                                  UseEMScale                      = True,
                                  addCaloExtensionDecoration      = False)
  ToolSvc += TimePFCSSKCaloIsolationTool

  # Import the xAOD isolation parameters.
  from xAODPrimitives.xAODIso import xAODIso as isoPar

  from IsolationAlgs.IsolationAlgsConf import IsolationBuilder
  isoBuilderLC = IsolationBuilder(
                                        name                  = "isoBuilderLC",
                                        ElectronCollectionContainerName    = "",
                                        PhotonCollectionContainerName      = "",
                                        MuonCollectionContainerName        = "Muons",
                                        FwdElectronCollectionContainerName = "",
                                        CaloCellIsolationTool = None,
                                        CaloTopoIsolationTool = LCCaloIsolationTool,
                                        PFlowIsolationTool    = None,
                                        TrackIsolationTool    = None, 
                                        FeIsoTypes            = [[]] ,
                                        FeCorTypes            = [[]],
                                        EgIsoTypes            = [[]],
                                        EgCorTypes            = [[]],
                                        MuIsoTypes            = [[isoPar.topoetcone20]],
                                        MuCorTypes            = [[isoPar.coreCone, isoPar.pileupCorrection ]],
                                        CustomConfigurationNameMu = "LC",
                                        CustomConfigurationNameEl = "",
                                        CustomConfigurationNamePh = "",
                                        CustomConfigurationName   = "LC",
                                        IsAODFix              = False,
                                        LeakageTool           = None,
                                        IsolateEl             = True)
  privateSeq += isoBuilderLC
  
  isoBuilderOrigTimeCS = IsolationBuilder(
                                        name                  = "isoBuilderOrigTimeCS",
                                        ElectronCollectionContainerName    = "",
                                        PhotonCollectionContainerName      = "",
                                        MuonCollectionContainerName        = "Muons",
                                        FwdElectronCollectionContainerName = "",
                                        CaloCellIsolationTool = None,
                                        CaloTopoIsolationTool = OrigTimeCSCaloIsolationTool,
                                        PFlowIsolationTool    = None,
                                        TrackIsolationTool    = None, 
                                        FeIsoTypes            = [[]] ,
                                        FeCorTypes            = [[]],
                                        EgIsoTypes            = [[]],
                                        EgCorTypes            = [[]],
                                        MuIsoTypes            = [[isoPar.topoetcone20]],
                                        MuCorTypes            = [[isoPar.coreCone]],
                                        CustomConfigurationNameMu = "OrigTimeCS",
                                        CustomConfigurationNameEl = "",
                                        CustomConfigurationNamePh = "",
                                        CustomConfigurationName   = "OrigTimeCS",
                                        IsAODFix              = False,
                                        LeakageTool           = None,
                                        IsolateEl             = True)
  privateSeq += isoBuilderOrigTimeCS
  
  isoBuilderOrigTimeSK = IsolationBuilder(
                                        name                  = "isoBuilderOrigTimeSK",
                                        ElectronCollectionContainerName    = "",
                                        PhotonCollectionContainerName      = "",
                                        MuonCollectionContainerName        = "Muons",
                                        FwdElectronCollectionContainerName = "",
                                        CaloCellIsolationTool = None,
                                        CaloTopoIsolationTool = OrigTimeSKCaloIsolationTool,
                                        PFlowIsolationTool    = None,
                                        TrackIsolationTool    = None, 
                                        FeIsoTypes            = [[]] ,
                                        FeCorTypes            = [[]],
                                        EgIsoTypes            = [[]],
                                        EgCorTypes            = [[]],
                                        MuIsoTypes            = [[isoPar.topoetcone20]],
                                        MuCorTypes            = [[isoPar.coreCone]],
                                        CustomConfigurationNameMu = "OrigTimeSK",
                                        CustomConfigurationNameEl = "",
                                        CustomConfigurationNamePh = "",
                                        CustomConfigurationName   = "OrigTimeSK",
                                        IsAODFix              = False,
                                        LeakageTool           = None,
                                        IsolateEl             = True)
  privateSeq += isoBuilderOrigTimeSK
  isoBuilderOrigTimeCSSK = IsolationBuilder(
                                        name                  = "isoBuilderOrigTimeCSSK",
                                        ElectronCollectionContainerName    = "",
                                        PhotonCollectionContainerName      = "",
                                        MuonCollectionContainerName        = "Muons",
                                        FwdElectronCollectionContainerName = "",
                                        CaloCellIsolationTool = None,
                                        CaloTopoIsolationTool = OrigTimeCSSKCaloIsolationTool,
                                        PFlowIsolationTool    = None,
                                        TrackIsolationTool    = None, 
                                        FeIsoTypes            = [[]] ,
                                        FeCorTypes            = [[]],
                                        EgIsoTypes            = [[]],
                                        EgCorTypes            = [[]],
                                        MuIsoTypes            = [[isoPar.topoetcone20]],
                                        MuCorTypes            = [[isoPar.coreCone]],
                                        CustomConfigurationNameMu = "OrigTimeCSSK",
                                        CustomConfigurationNameEl = "",
                                        CustomConfigurationNamePh = "",
                                        CustomConfigurationName   = "OrigTimeCSSK",
                                        IsAODFix              = False,
                                        LeakageTool           = None,
                                        IsolateEl             = True)
  privateSeq += isoBuilderOrigTimeCSSK
  
  isoBuilderTimePFCS = IsolationBuilder(
                                        name                  = "isoBuilderTimePFCS",
                                        ElectronCollectionContainerName    = "",
                                        PhotonCollectionContainerName      = "",
                                        MuonCollectionContainerName        = "Muons",
                                        FwdElectronCollectionContainerName = "",
                                        CaloCellIsolationTool = None,
                                        CaloTopoIsolationTool = None,
                                        PFlowIsolationTool    = TimePFCSCaloIsolationTool,
                                        TrackIsolationTool    = None, 
                                        FeIsoTypes            = [[]] ,
                                        FeCorTypes            = [[]],
                                        EgIsoTypes            = [[]],
                                        EgCorTypes            = [[]],
                                        MuIsoTypes            = [[isoPar.neflowisol20]],
                                        MuCorTypes            = [[isoPar.coreCone]],
                                        CustomConfigurationNameMu = "TimePFCS",
                                        CustomConfigurationNameEl = "",
                                        CustomConfigurationNamePh = "",
                                        CustomConfigurationName   = "TimePFCS",
                                        IsAODFix              = False,
                                        LeakageTool           = None,
                                        IsolateEl             = True)
  privateSeq += isoBuilderTimePFCS
  
  isoBuilderTimePFSK = IsolationBuilder(
                                        name                  = "isoBuilderTimePFSK",
                                        ElectronCollectionContainerName    = "",
                                        PhotonCollectionContainerName      = "",
                                        MuonCollectionContainerName        = "Muons",
                                        FwdElectronCollectionContainerName = "",
                                        CaloCellIsolationTool = None,
                                        CaloTopoIsolationTool = None,
                                        PFlowIsolationTool    = TimePFSKCaloIsolationTool,
                                        TrackIsolationTool    = None, 
                                        FeIsoTypes            = [[]] ,
                                        FeCorTypes            = [[]],
                                        EgIsoTypes            = [[]],
                                        EgCorTypes            = [[]],
                                        MuIsoTypes            = [[isoPar.neflowisol20]],
                                        MuCorTypes            = [[isoPar.coreCone]],
                                        CustomConfigurationNameMu = "TimePFSK",
                                        CustomConfigurationNameEl = "",
                                        CustomConfigurationNamePh = "",
                                        CustomConfigurationName   = "TimePFSK",
                                        IsAODFix              = False,
                                        LeakageTool           = None,
                                        IsolateEl             = True)
  privateSeq += isoBuilderTimePFSK

  isoBuilderTimePFCSSK = IsolationBuilder(
                                        name                  = "isoBuilderTimePFCSSK",
                                        ElectronCollectionContainerName    = "",
                                        PhotonCollectionContainerName      = "",
                                        MuonCollectionContainerName        = "Muons",
                                        FwdElectronCollectionContainerName = "",
                                        CaloCellIsolationTool = None,
                                        CaloTopoIsolationTool = None,
                                        PFlowIsolationTool    = TimePFCSSKCaloIsolationTool,
                                        TrackIsolationTool    = None, 
                                        FeIsoTypes            = [[]] ,
                                        FeCorTypes            = [[]],
                                        EgIsoTypes            = [[]],
                                        EgCorTypes            = [[]],
                                        MuIsoTypes            = [[isoPar.neflowisol20]],
                                        MuCorTypes            = [[isoPar.coreCone]],
                                        CustomConfigurationNameMu = "TimePFCSSK",
                                        CustomConfigurationNameEl = "",
                                        CustomConfigurationNamePh = "",
                                        CustomConfigurationName   = "TimePFCSSK",
                                        IsAODFix              = False,
                                        LeakageTool           = None,
                                        IsolateEl             = True)
  privateSeq += isoBuilderTimePFCSSK

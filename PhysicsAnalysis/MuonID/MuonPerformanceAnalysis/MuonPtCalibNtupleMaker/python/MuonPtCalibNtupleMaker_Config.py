# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def MuonScaleConfig( OutputName ):

  from AthenaCommon.AppMgr import ServiceMgr
  from AthenaCommon.AlgSequence import AlgSequence
  theJob = AlgSequence()

  # THistSvc configuration
  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()
  ServiceMgr.THistSvc.Output += [ "MCPCALIB DATAFILE='{}' OPT='RECREATE'".format( OutputName ) ]

  # Setup MST
  from AthenaCommon import CfgMgr
  ToolSvc = ServiceMgr.ToolSvc
  ToolSvc += CfgMgr.CP__MuonSelectionTool( 'MuonSelectorTool' )

  from MuonPtCalibNtupleMaker.MuonPtCalibNtupleMakerConf import CalibMuonsSelectorTool
  SelectorTool = CalibMuonsSelectorTool( 'SelectorTool' ) 
  SelectorTool.MuonSelectorTool = ToolSvc.MuonSelectorTool
  ToolSvc += SelectorTool

  # Setup algorithm
  from MuonPtCalibNtupleMaker.MuonPtCalibNtupleMakerConf import CalibMuonsNtupleMaker
  NtupleMaker = CalibMuonsNtupleMaker( 'NtupleMaker' )
  NtupleMaker.SelectorTool = SelectorTool
  NtupleMaker.ExtendedVersion = False
  theJob += NtupleMaker


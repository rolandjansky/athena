# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon import CfgMgr

def setupScaleCalibrationInput():
    print "Setting up setupScaleCalibrationInput"

    topSequence = AlgSequence()
    # THistSvc configuration
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
    ServiceMgr.THistSvc.Output += [ "MCPCALIB DATAFILE='MCPCalibNtuple_xAOD.root' OPT='RECREATE'" ]
    
    # Setup GRL
    GRL = os.path.expandvars( '$TestArea/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPtCalibNtupleMaker/grl/data12_8TeV.periodAllYear_HEAD_DQDefects-00-01-02_PHYS_CombinedPerf_Muon_Muon_calo.xml' )
    ToolSvc += CfgMgr.GoodRunsListSelectionTool( 'GRLTool' , GoodRunsListVec = [ GRL ] )
	
    # Setup MST
    ToolSvc += CfgMgr.CP__MuonSelectionTool( 'MuonSelectorTool', OutputLevel = INFO )
    ToolSvc += CfgMgr.CalibMuonsSelectorTool( 'CalibMuonsSelectorTool', MuonSelectorTool = ToolSvc.MuonSelectorTool, OutputLevel = INFO )
    
    # Setup algorithm
    topSequence += CfgMgr.CalibMuonsNtupleMaker( 'CalibMuonsNtupleMaker', OutputLevel = INFO, SelectorTool = SelectorTool, ExtendedVersion = False )

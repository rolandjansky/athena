# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from DerivationFrameworkCore.DerivationFrameworkMaster import *

HIGlobalVars=[ "CaloSums",
              "ZdcModules",
              "ZdcTriggerTowers",
              "MBTSForwardEventInfo",
              "MBTSModules"]

def getHIEventSelector() : 
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,"HIEventSelector") : return getattr(ToolSvc,"HIEventSelector")

    from HIEventUtils.HIEventUtilsConf import HI__HIEventSelectionTool
    HIEventSelector=HI__HIEventSelectionTool("HIEventSelector")
    ToolSvc+=HIEventSelector
    from HIEventUtils.HIEventUtilsConf import HI__HIVertexSelectionTool
    HIVertexSelector=HI__HIVertexSelectionTool("HIVertexSelector")
    ToolSvc+=HIVertexSelector

    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool 
    HITrackSelector=InDet__InDetTrackSelectionTool("InDetTrackSelectionTool_HILoose")
    HITrackSelector.CutLevel = "LoosePrimary"
    ToolSvc+=HITrackSelector
    HIVertexSelector.TrackSelectionTool=HITrackSelector
    HIEventSelector.VertexSelectionTool=HIVertexSelector
    
    return HIEventSelector
    
    
def GetConditionsFromMetaData() :

    from PyUtils import AthFile
    af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0])
    if 'project_name' in af.fileinfos['metadata']['/TagInfo']: project_tag = af.fileinfos['metadata']['/TagInfo']['project_name']
    else: project_tag = ""
    beam_energy = af.fileinfos['metadata']['/TagInfo']['beam_energy']
    if 'triggerStreamOfFile' in af.fileinfos['metadata']['/TagInfo']: physics_stream = af.fileinfos['metadata']['/TagInfo']['triggerStreamOfFile']
    else: physics_stream = ""
    
    
    print '+++++++++++++++++++++++++++++++ project tag: ',project_tag,' +++++++++++++++++++++++++++++++'
    print '+++++++++++++++++++++++++++++++ beam energy: ',beam_energy,' +++++++++++++++++++++++++++++++'
    print '+++++++++++++++++++++++++++++++ physics stream: ',physics_stream,' +++++++++++++++++++++++++++++++'


    from DerivationFrameworkHI.HIDerivationFlags import HIDerivationFlags
    
    #Set physics stream
    if physics_stream=='MinBias' or physics_stream=='CC' or physics_stream=='PC':
      HIDerivationFlags.doMinBiasSelection = True
    else:
      HIDerivationFlags.doMinBiasSelection = False
    
    #Set data/MC
    isMC = IsMC()
    if isMC :
      print "Dataset Type: MC"
      if HIDerivationFlags.isSimulation.is_locked() and not HIDerivationFlags.isSimulation :
          print 'WARNING: Can not change locked simulation flag to True'
      HIDerivationFlags.isSimulation = True
      if IsHIMC(project_tag) : HIDerivationFlags.isPP = False
      else : HIDerivationFlags.isPP = True
    else :
      if HIDerivationFlags.isSimulation.is_locked() and HIDerivationFlags.isSimulation :
          print 'WARNING: Can not change locked simulation flag to False'
      HIDerivationFlags.isSimulation = False
    
    #Set project     
    if not HIDerivationFlags.isSimulation :
      if project_tag=='data15_hi':
        if HIDerivationFlags.isPP.is_locked() and HIDerivationFlags.isPP :
          print 'WARNING: Can not change locked project tag to HI'  
        HIDerivationFlags.isPP = False
        print "Dataset Type: HeavyIon 2015"
      elif project_tag=='data15_5TeV':
        if HIDerivationFlags.isPP.is_locked() and not HIDerivationFlags.isPP :
          print 'WARNING: Can not change locked project tag to pp' 
        HIDerivationFlags.isPP = True
        print "Dataset Type: pp 2015"   
      elif project_tag=='data16_hip8TeV':
        if HIDerivationFlags.isPPb.is_locked() and not HIDerivationFlags.isPPb :
          print 'WARNING: Can not change locked project tag to pPb' 
        HIDerivationFlags.isPPb = True
        print "Dataset Type: proton-Ion 8.16TeV 2016" 
      elif project_tag=='data16_hip5TeV':
        if HIDerivationFlags.isPPb.is_locked() and not HIDerivationFlags.isPPb :
          print 'WARNING: Can not change locked project tag to pPb' 
        HIDerivationFlags.isPPb = True
        print "Dataset Type: proton-Ion 5.02TeV 2016" 
      elif project_tag=='data17_5TeV':
        if HIDerivationFlags.isPP.is_locked() and not HIDerivationFlags.isPP :
          print 'WARNING: Can not change locked project tag to pp' 
        HIDerivationFlags.isPP = True
        print "Dataset Type: pp 2017"  
      elif project_tag=='data12_7TeV':
        if HIDerivationFlags.isPP.is_locked() and not HIDerivationFlags.isPP :
          print 'WARNING: Can not change locked project tag to pp'
        HIDerivationFlags.isPP = True
        print "Dataset Type: pp 7 TeV 2012"
      elif project_tag=='data12_8TeV':
        if HIDerivationFlags.isPP.is_locked() and not HIDerivationFlags.isPP :
          print 'WARNING: Can not change locked project tag to pp'
        HIDerivationFlags.isPP = True
        print "Dataset Type: pp 8 TeV 2012"
      elif project_tag=='data13_8TeV':
        if HIDerivationFlags.isPP.is_locked() and not HIDerivationFlags.isPP :
          print 'WARNING: Can not change locked project tag to pp'
        HIDerivationFlags.isPP = True
        print "Dataset Type: pp 8 TeV 2013"
      elif project_tag=='data15_13TeV':
        if HIDerivationFlags.isPP.is_locked() and not HIDerivationFlags.isPP :
          print 'WARNING: Can not change locked project tag to pp'
        HIDerivationFlags.isPP = True
        print "Dataset Type: pp 13 TeV 2015"
      elif project_tag=='data16_13TeV':
        if HIDerivationFlags.isPP.is_locked() and not HIDerivationFlags.isPP :
          print 'WARNING: Can not change locked project tag to pp'
        HIDerivationFlags.isPP = True
        print "Dataset Type: pp 13 TeV 2016"
      elif project_tag=='data17_13TeV':
        if HIDerivationFlags.isPP.is_locked() and not HIDerivationFlags.isPP :
          print 'WARNING: Can not change locked project tag to pp'
        HIDerivationFlags.isPP = True
        print "Dataset Type: pp 13 TeV 2017"
      elif project_tag=='data18_13TeV':
        if HIDerivationFlags.isPP.is_locked() and not HIDerivationFlags.isPP :
          print 'WARNING: Can not change locked project tag to pp'
        HIDerivationFlags.isPP = True
        print "Dataset Type: pp 13 TeV 2018"
      elif project_tag=='data17_hi':
        if HIDerivationFlags.isPP.is_locked() and HIDerivationFlags.isPP :
          print 'WARNING: Can not change locked project tag to HI'  
        HIDerivationFlags.isPP = False
        print "Dataset Type: HeavyIon 2018"
      elif project_tag=='data18_hi':
        if HIDerivationFlags.isPP.is_locked() and HIDerivationFlags.isPP :
          print 'WARNING: Can not change locked project tag to HI'  
        HIDerivationFlags.isPP = False
        print "Dataset Type: HeavyIon 2018"
      else:
        print "Unknown Dataset: Quitting" 
        exit()
    else:
      if project_tag=='data16_hip8TeV':
        if HIDerivationFlags.isPPb.is_locked() and not HIDerivationFlags.isPPb :
          print 'WARNING: Can not change locked project tag to pPb' 
        HIDerivationFlags.isPPb = True
        print "Dataset Type: proton-Ion 8.16TeV data overlay MC" 
      elif beam_energy==4080000.:
        if HIDerivationFlags.isPPb.is_locked() and not HIDerivationFlags.isPPb :
          print 'WARNING: Can not change locked project tag to pPb' 
        HIDerivationFlags.isPPb = True
        print "Dataset Type: proton-Ion 8.16TeV MC" 
    print 'Running with ', HIDerivationFlags.isSimulation, ', ', HIDerivationFlags.isPP, ', ', HIDerivationFlags.isPPb, ', and ', HIDerivationFlags.doMinBiasSelection    

def IsMC() :
	if globalflags.DataSource()=='geant4': return True
	from PyUtils import AthFile
	af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0])
	containers=af.fileinfos['eventdata_items']
	for c in containers:
		if c[0] is not None and "Truth" in c[0] : return True
	return False
	
def IsHIMC(project_tag="") :    
	from RecExConfig.RecFlags import rec
	isMC = IsMC()
	if not globalflags.DataSource()=='geant4' and isMC: return True #=> Overlay
	if 'hi' in project_tag : return True # =>  test for the key word in project tag (some validation samples)
	if not rec.doHIP and isMC : return True #=> HIJING
	return False     
   
def GetGapSkimmingTool() :
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.SystemOfUnits import MeV
    from DerivationFrameworkHI.DerivationFrameworkHIConf import DerivationFramework__HIGapSkimmingTool
    HIGapSkimmingTool_UPC = DerivationFramework__HIGapSkimmingTool("HIGapSkimmingTool_UPCGap")

    ToolSvc += HIGapSkimmingTool_UPC
    from HIEventUtils.HIEventUtilsConf import HI__TopoClusterSelectionTool
    tc_selection_tool=HI__TopoClusterSelectionTool("TopoClusterSelectionTool_UPCGap")
    tc_selection_tool.MinPt=200.*MeV
    tc_selection_tool.ApplySignificanceCuts=True
    tc_selection_tool.CalibFile="HIEventUtils/TCSigCuts.root"
    tc_selection_tool.CalibHisto="h1_TC_sig_cuts"
    ToolSvc += tc_selection_tool

    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
    track_selection_tool=InDet__InDetTrackSelectionTool("InDetTrackSelectionTool_UPCGap");  
    track_selection_tool.CutLevel="NoCut"
    track_selection_tool.minPt=200.*MeV
    ToolSvc += track_selection_tool
    
    HIGapSkimmingTool_UPC.ClusterContainerName="CaloCalTopoClusters"
    HIGapSkimmingTool_UPC.TrackContainerName="InDetTrackParticles"
    HIGapSkimmingTool_UPC.ClusterSelectionTool=tc_selection_tool
    HIGapSkimmingTool_UPC.TrackSelectionTool=track_selection_tool
    HIGapSkimmingTool_UPC.GapMinimum=0.5
    HIGapSkimmingTool_UPC.UseJetGaps=False
    HIGapSkimmingTool_UPC.SumOfGapsMax=2;

    return HIGapSkimmingTool_UPC

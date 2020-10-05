# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
import math
def defineHistograms(monAlg, group,helper,histoNameSuffix=""):
#    name = histoNameSuffix + 'x'
#    title = histoNameSuffix + 'x;L/TrkMETx [GeV];Events'
#    group.defineHistogram(name,title=title,path='ToRuleThemAll',xbins=100,xmin=-500,xmax=500.0)
    
    for kinesuffix in ['_et', '_ex','_ey','_phi','_sumet']:
        name =  histoNameSuffix  + kinesuffix
        title = name +';'+name+'[GeV];Events'
        xmin = 0
        xmax = 500
        if kinesuffix == '_ex' or kinesuffix == '_ey':
            xmin = -500
            xmax = 500
        if kinesuffix == '_phi' :    
            xmin = -math.pi  
            xmax = math.pi
            title = name +';'+name+';Events'
        if kinesuffix == '_sumet':
            xmin = 0
            xmax = 5000  
        group.defineHistogram(name,title=title,xbins=100,xmin=xmin,xmax=xmax)

def defineHistogramsCalo(monAlg, group,helper,histoNameSuffix=""):
#    name = histoNameSuffix + 'x'
#    title = histoNameSuffix + 'x;L/TrkMETx [GeV];Events'
#    group.defineHistogram(name,title=title,path='ToRuleThemAll',xbins=100,xmin=-500,xmax=500.0)
    
    for kinesuffix in ['_et', '_ex','_ey','_phi','_sumet']:
        name =  "MET_"+histoNameSuffix  + kinesuffix 
        title = name +';'+name+'[GeV];Events'
        xmin = 0
        xmax = 500
        if kinesuffix == '_ex' or kinesuffix == '_ey':
            xmin = -500
            xmax = 500
        if kinesuffix == '_phi' :    
            xmin = -math.pi  
            xmax = math.pi
            title = name +';'+name+';Events'
        if kinesuffix == '_sumet':
            xmin = 0
            xmax = 5000  
        group.defineHistogram(name,title=title,xbins=100,xmin=xmin,xmax=xmax)



def METMonitoringConfig(inputFlags):    
# '''Function to configures some algorithms in the monitoring system.'''

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # do not run monitoring in RAWtoESD
    if inputFlags.DQ.Environment == 'tier0Raw':
        return result

    from AthenaMonitoring import AthMonitorCfgHelper     
#    helper = AthMonitorCfgHelper(inputFlags,'AthMonitorCfg') 
    helper = AthMonitorCfgHelper(inputFlags,'METMonitor') 
 

    from AthenaConfiguration.ComponentFactory import CompFactory  
    METRefFinal_MonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'METRefFinal_MonAlg')
#    anotherExampleMonAlg = helper.addAlgorithm(METMonitoringExampleAlg,'AnotherExampleMonAlg')
    met_types = ["MET_RefFinal","MET_RefJet","MET_Muon","MET_RefEle","MET_RefGamma","MET_RefTau","MET_PVSoftTrk"]



    METRefFinal_MonAlg.METContainer="MET_Reference_AntiKt4EMTopo"
    METRefFinal_MonAlg.metKeys = met_types
    METRefFinal_MonAlg.alltrigger = True
    group = helper.addGroup(METRefFinal_MonAlg, "METMonitor", "MissingEt/AllTriggers/MET_AntiKt4EMTopo/")

    for mets in met_types:
        defineHistograms(METRefFinal_MonAlg, group,helper,mets)

    if inputFlags.DQ.DataType != 'cosmics':
        METPflow_MonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'METPflow_MonAlg')   
        pfmet_types = ["MET_PFlow","MET_PFlow_RefJet","MET_PFlow_Muon","MET_PFlow_RefEle","MET_PFlow_RefGamma","MET_PFlow_RefTau","MET_PFlow_PVSoftTrk"]
        METPflow_MonAlg.METContainer="MET_Reference_AntiKt4EMPFlow"
        METPflow_MonAlg.metKeys = pfmet_types
        METPflow_MonAlg.alltrigger = True

        group = helper.addGroup(METPflow_MonAlg, "METMonitor", "MissingEt/AllTriggers/MET_AntiKt4EMPFlow/")
        for mets in pfmet_types:
            defineHistograms(METPflow_MonAlg, group,helper,mets)

        
    METEMTopo_MonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'METEMTopo_MonAlg')   
    emtopomet_types = ["MET_Topo"]
    METEMTopo_MonAlg.METContainer="MET_EMTopo"
    METEMTopo_MonAlg.metKeys = emtopomet_types
    METEMTopo_MonAlg.alltrigger = True

    group = helper.addGroup(METEMTopo_MonAlg, "METMonitor", "MissingEt/AllTriggers/MET_Calo/EMTopo")
    for mets in emtopomet_types:
        defineHistograms(METEMTopo_MonAlg, group,helper,mets)


    METCalo_MonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'METCalo_MonAlg')   
    metcalo_types = [ "PEMB", "EMB", "PEME", "EME", "TILE", "HEC", "FCAL" ]
    METCalo_MonAlg.METContainer="MET_Calo"
    METCalo_MonAlg.METCaloKeys = metcalo_types
    METCalo_MonAlg.alltrigger = True

    group = helper.addGroup(METCalo_MonAlg, "METMonitor", "MissingEt/AllTriggers/MET_Calo/MET_Cell")
    for mets in metcalo_types:
        defineHistogramsCalo(METCalo_MonAlg, group,helper,mets)
#trigger
    METRefFinal_XE30_MonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'METRefFinal_XE30_MonAlg')
    METRefFinal_XE30_MonAlg.METContainer="MET_Reference_AntiKt4EMTopo"
    METRefFinal_XE30_MonAlg.metTotalKey="FinalTrk"
    METRefFinal_XE30_MonAlg.metKeys = met_types
    METRefFinal_XE30_MonAlg.dotrigger = True
    group = helper.addGroup(METRefFinal_XE30_MonAlg,"METMonitor","MissingEt/TrigXE30/MET_AntiKt4EMTopo/")
    for mets in met_types:
        defineHistograms(METRefFinal_XE30_MonAlg, group,helper,mets)

    if inputFlags.DQ.DataType != 'cosmics':
        METPflow_XE30_MonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'METPflow_XE30_MonAlg')
        METPflow_XE30_MonAlg.METContainer="MET_Reference_AntiKt4EMPFlow"
        METPflow_XE30_MonAlg.metTotalKey="FinalTrk"
        METPflow_XE30_MonAlg.metKeys = pfmet_types
        METPflow_XE30_MonAlg.dotrigger = True
        group = helper.addGroup(METPflow_XE30_MonAlg,"METMonitor","MissingEt/TrigXE30/MET_AntiKt4EMPflow/")
        for mets in pfmet_types:
            defineHistograms(METPflow_XE30_MonAlg, group,helper,mets)

    METCalo_XE30_MonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'METCalo_XE30_MonAlg')
    METCalo_XE30_MonAlg.METCaloContainer="MET_Calo"
    METCalo_XE30_MonAlg.METCaloKeys = metcalo_types
    METCalo_XE30_MonAlg.dotrigger = True
    group = helper.addGroup(METCalo_XE30_MonAlg,"METMonitor","MissingEt/TrigXE30/MET_Calo/MET_Cell")
    for mets in metcalo_types:
        defineHistogramsCalo(METCalo_XE30_MonAlg, group,helper,mets)

    METEMTopo_XE30_MonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'METEMTopo_XE30_MonAlg')
    METEMTopo_XE30_MonAlg.METContainer="MET_EMTopo"
    METEMTopo_XE30_MonAlg.METAntiKt4EMTopoContainer="MET_Reference_AntiKt4EMTopo"
    emtopomet_types= ["MET_Topo"]
    METEMTopo_XE30_MonAlg.metKeys = emtopomet_types
    METEMTopo_XE30_MonAlg.dotrigger = True
    METEMTopo_XE30_group = helper.addGroup(METEMTopo_XE30_MonAlg,"METMonitor","MissingEt/TrigXE30/MET_Calo/EMTopo") 
    for mets in emtopomet_types:
        defineHistograms(METEMTopo_XE30_MonAlg, METEMTopo_XE30_group,helper,mets) 

# metcut
    METRefFinal_METCut_MonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'METRefFinal_METCut_MonAlg')
    METRefFinal_METCut_MonAlg.METContainer="MET_Reference_AntiKt4EMTopo"
    METRefFinal_METCut_MonAlg.metTotalKey="FinalTrk"
    METRefFinal_METCut_MonAlg.metKeys = met_types
    METRefFinal_METCut_MonAlg.dometcut = True
    METRefFinal_METCut_MonAlg.metcut = 80
    group = helper.addGroup(METRefFinal_METCut_MonAlg,"METMonitor","MissingEt/CutMet80/MET_AntiKt4EMTopo/") 
    for mets in met_types:
        defineHistograms(METRefFinal_METCut_MonAlg, group,helper,mets) 
   
    if inputFlags.DQ.DataType != 'cosmics':
        METPflow_METCut_MonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'METPflow_METCut_MonAlg')
        METPflow_METCut_MonAlg.METContainer="MET_Reference_AntiKt4EMPFlow"
        METPflow_METCut_MonAlg.metKeys = pfmet_types
        METPflow_METCut_MonAlg.dometcut = True
        METPflow_METCut_MonAlg.metcut = 80
        group = helper.addGroup(METPflow_METCut_MonAlg,"METMonitor","MissingEt/CutMet80/MET_AntiKt4EMPflow/") 
        for mets in pfmet_types:
            defineHistograms(METPflow_METCut_MonAlg, group,helper,mets)
        
    METCalo_METCut_MonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'METCalo_METCut_MonAlg')
    metcalo_types = [ "PEMB", "EMB", "PEME", "EME", "TILE", "HEC", "FCAL" ]
    METCalo_METCut_MonAlg.METCaloContainer="MET_Calo"
    METCalo_METCut_MonAlg.METCaloKeys = metcalo_types
    METCalo_METCut_MonAlg.dometcut = True
    METCalo_METCut_MonAlg.metcut = 80
    METCalo_METCut_group = helper.addGroup(METCalo_METCut_MonAlg,"METMonitor","MissingEt/CutMet80/MET_Calo/MET_Cell") 
    for mets in metcalo_types:
        defineHistogramsCalo(METCalo_METCut_MonAlg, METCalo_METCut_group,helper,mets) 
   
    METEMTopo_METCut_MonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'METEMTopo_METCut_MonAlg')
    METEMTopo_METCut_MonAlg.METContainer="MET_EMTopo"
    METEMTopo_METCut_MonAlg.METAntiKt4EMTopoContainer="MET_Reference_AntiKt4EMTopo"
    emtopomet_types = ["MET_Topo"]
    METEMTopo_METCut_MonAlg.metKeys = emtopomet_types
    METEMTopo_METCut_MonAlg.dometcut = True
    METEMTopo_METCut_MonAlg.metcut = 80
    METEMTopo_METCut_group = helper.addGroup(METEMTopo_METCut_MonAlg,"METMonitor","MissingEt/CutMet80/MET_Calo/EMTopo") 
    for mets in emtopomet_types:
        defineHistograms(METEMTopo_METCut_MonAlg, METEMTopo_METCut_group,helper,mets)    
# Jet cleaning

    jetCleaningTool = CompFactory.JetCleaningTool()
    jetCleaningTool.CutLevel = "LooseBad"       
#    jetCleaningTool.CutLevel = "TightBad"       
    jetCleaningTool.DoUgly = False
    
    JetCleaning_METMonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'JetCleaning_METMonAlg')    
    JetCleaning_METMonAlg.metKeys = met_types
    JetCleaning_METMonAlg.DoJetCleaning = True
    JetCleaning_METMonAlg.JetCleaningTool = jetCleaningTool
    JetCleaningGroup = helper.addGroup(JetCleaning_METMonAlg,"METMonitor","MissingEt/Jetcleaning/MET_AntiKt4EMTopo/")
    JetCleaning_METMonAlg.JetContainerName = "AntiKt4EMTopoJets"
    for mets in met_types:
        if mets == 'MET_PFlow_PVSoftTrk':
            JetCleaning_METMonAlg.JetContainerName = "AntiKt4EMPFlowJets"
        defineHistograms(JetCleaning_METMonAlg, JetCleaningGroup,helper,mets)
        

    if inputFlags.DQ.DataType != 'cosmics':
        PflowJetCleaning_METMonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'PflowJetCleaning_METMonAlg') 
        PflowJetCleaning_METMonAlg.METContainer="MET_Reference_AntiKt4EMPFlow"
        PflowJetCleaning_METMonAlg.metKeys = pfmet_types
        PflowJetCleaning_METMonAlg.DoJetCleaning = True
        PflowJetCleaning_METMonAlg.JetCleaningTool = jetCleaningTool
        PflowJetCleaningGroup = helper.addGroup(PflowJetCleaning_METMonAlg,"METMonitor","MissingEt/Jetcleaning/MET_AntiKt4EMPflow/")    
        PflowJetCleaning_METMonAlg.JetContainerName = "AntiKt4EMPFlowJets"
        for mets in pfmet_types:
            defineHistograms(PflowJetCleaning_METMonAlg, PflowJetCleaningGroup,helper,mets)
         
    METCaloJetCleaning_MonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'METCaloJetCleaning_MonAlg')   
    metcalo_types = [ "PEMB", "EMB", "PEME", "EME", "TILE", "HEC", "FCAL" ]
    METCaloJetCleaning_MonAlg.METCaloContainer="MET_Calo"
    METCaloJetCleaning_MonAlg.METCaloKeys = metcalo_types
    METCaloJetCleaning_MonAlg.DoJetCleaning = True
    METCaloJetCleaning_MonAlg.JetCleaningTool = jetCleaningTool  
    group = helper.addGroup(METCaloJetCleaning_MonAlg, "METMonitor", "MissingEt/Jetcleaning/MET_Calo/MET_Cell")
    for mets in metcalo_types:
        defineHistogramsCalo(METCaloJetCleaning_MonAlg, group,helper,mets) 
   
    METEMTopoJetCleaning_MonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'METEMTopoJetCleaning_MonAlg')   
    emtopomet_types = ["MET_Topo"]
    METEMTopoJetCleaning_MonAlg.METContainer="MET_EMTopo"
    METEMTopoJetCleaning_MonAlg.metKeys = emtopomet_types
    METEMTopoJetCleaning_MonAlg.DoJetCleaning = True
    METEMTopoJetCleaning_MonAlg.JetCleaningTool = jetCleaningTool  
    group = helper.addGroup(METEMTopoJetCleaning_MonAlg, "METMonitor", "MissingEt/Jetcleaning/MET_Calo/EMTopo/")
    for mets in emtopomet_types:
        defineHistograms(METEMTopoJetCleaning_MonAlg, group,helper,mets)


    # Badjets

    jetCleaningTool.CutLevel = "LooseBad"       
    #    jetCleaningTool.CutLevel = "TightBad"       
    jetCleaningTool.DoUgly = False
    
    BadJets_METMonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'BadJets_METMonAlg')    
    BadJets_METMonAlg.metKeys = met_types
    BadJets_METMonAlg.DoJetCleaning = True
    BadJets_METMonAlg.alltrigger = True
    BadJets_METMonAlg.DoBadJets = True
    BadJets_METMonAlg.JetCleaningTool = jetCleaningTool
    BadJets_METMonAlg.JetContainerName = "AntiKt4EMTopoJets"
    BadJetsGroup = helper.addGroup(BadJets_METMonAlg,"METMonitor","MissingEt/AllTriggers/BadJets/MET_AntiKt4EMTopo")
    for mets in met_types:
        defineHistograms(BadJets_METMonAlg, BadJetsGroup,helper,mets)

    if inputFlags.DQ.DataType != 'cosmics':
        BadPFJets_METMonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'BadPFJets_METMonAlg')    
        BadPFJets_METMonAlg.METContainer="MET_Reference_AntiKt4EMPFlow"
        BadPFJets_METMonAlg.metKeys = pfmet_types
        BadPFJets_METMonAlg.DoJetCleaning = True
        BadPFJets_METMonAlg.alltrigger = True
        BadPFJets_METMonAlg.DoBadJets = True
        BadPFJets_METMonAlg.JetCleaningTool = jetCleaningTool
        BadPFJets_METMonAlg.JetContainerName = "AntiKt4EMPFlowJets"
        BadPFJetsGroup = helper.addGroup(BadPFJets_METMonAlg,"METMonitor","MissingEt/AllTriggers/BadJets/MET_AntiKt4EMPflow/")
        for mets in pfmet_types:
            defineHistograms(BadPFJets_METMonAlg, BadPFJetsGroup,helper,mets)


    BadJets_CaloMETMonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'BadJets_CaloMETMonAlg')    
    metcalo_types = [ "PEMB", "EMB", "PEME", "EME", "TILE", "HEC", "FCAL" ]
    BadJets_CaloMETMonAlg.METCaloContainer="MET_Calo"
    BadJets_CaloMETMonAlg.METCaloKeys = metcalo_types
    BadJets_CaloMETMonAlg.DoJetCleaning = True
    BadJets_CaloMETMonAlg.alltrigger = True
    BadJets_CaloMETMonAlg.DoBadJets = True
    BadJets_CaloMETMonAlg.JetContainerName = "AntiKt4EMTopoJets"
    BadJets_CaloMETMonAlg.JetCleaningTool = jetCleaningTool
    BadJetsGroup_CaloMETMonAlg = helper.addGroup(BadJets_CaloMETMonAlg,"METMonitor","MissingEt/AllTriggers/BadJets/MET_Calo/MET_Cell")
    for mets in metcalo_types:
        defineHistogramsCalo(BadJets_CaloMETMonAlg, BadJetsGroup_CaloMETMonAlg,helper,mets)


    BadJets_EMTopoMETMonAlg = helper.addAlgorithm(CompFactory.METMonitoringAlg,'BadJets_EMTopoMETMonAlg')    
    BadJets_EMTopoMETMonAlg.metKeys = emtopomet_types
    BadJets_EMTopoMETMonAlg.METContainer="MET_EMTopo"
    BadJets_EMTopoMETMonAlg.DoJetCleaning = True
    BadJets_EMTopoMETMonAlg.alltrigger = True
    BadJets_EMTopoMETMonAlg.DoBadJets = True
    BadJets_EMTopoMETMonAlg.JetCleaningTool = jetCleaningTool
    BadJetsGroup = helper.addGroup(BadJets_EMTopoMETMonAlg,"METMonitor","MissingEt/AllTriggers/BadJets/MET_Calo/EMTopo")
    for mets in emtopomet_types:
        defineHistograms(BadJets_EMTopoMETMonAlg, BadJetsGroup,helper,mets)

    result.merge(helper.result())
    return result

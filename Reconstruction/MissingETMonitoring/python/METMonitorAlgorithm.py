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
        group.defineHistogram(name,title=title,path='ToRuleThemAll',xbins=100,xmin=xmin,xmax=xmax)

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
        group.defineHistogram(name,title=title,path='ToRuleThemAll',xbins=100,xmin=xmin,xmax=xmax)



def METMonitoringConfig(inputFlags):    
# '''Function to configures some algorithms in the monitoring system.'''

#    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator  
#    result = ComponentAccumulator()

    from AthenaMonitoring import AthMonitorCfgHelper     
#    helper = AthMonitorCfgHelper(inputFlags,'AthMonitorCfg') 
    helper = AthMonitorCfgHelper(inputFlags,'METMonitor') 
 

    from MissingETMonitoring.MissingETMonitoringConf import  METMonitoringAlg     
    METRefFinal_MonAlg = helper.addAlgorithm(METMonitoringAlg,'METRefFinal_MonAlg')
#    anotherExampleMonAlg = helper.addAlgorithm(METMonitoringExampleAlg,'AnotherExampleMonAlg')
#    met_types = ["MET_RefEle", "MET_RefGamma"]
#    met_types = ["MET_RefFinal"]
    met_types = ["MET_RefFinal","MET_RefJet","MET_Muons","MET_RefEle","MET_RefGamma","MET_RefTau","MET_PVSoftTrk"]
#    met_types = ["MET_RefFinal","MET_RefJet","MET_PFlow_PVSoftTrk","MET_PFlow_RefJet","MET_Track"]
#    exampleMonAlg.JetContainerName = "AntiKt4EMTopoJets"
#    jet_types = ["AntiKt4EMTopoJets","AntiKt4EMPFlowJets","AntiKt4EMTopoJets"]
    METRefFinal_MonAlg.metKeys = met_types
    METRefFinal_MonAlg.alltrigger = True
    group = helper.addGroup(METRefFinal_MonAlg, "METMonitor", "MissingEt/AllTriggers/MET_AntiKt4EMTopo/")

    for mets in met_types:
        defineHistograms(METRefFinal_MonAlg, group,helper,mets)


    METPflow_MonAlg = helper.addAlgorithm(METMonitoringAlg,'METPflow_MonAlg')   
    pfmet_types = ["MET_PFlow","MET_PFlow_RefJet","MET_Fflow_Muons","MET_PFlow_RefEle","MET_PFlow_RefGamma","MET_PFlow_RefTau","MET_PFlow_PVSoftTrk"]
    METPflow_MonAlg.metKeys = pfmet_types
    METPflow_MonAlg.alltrigger = True

    group = helper.addGroup(METPflow_MonAlg, "METMonitor", "MissingEt/AllTriggers/MET_AntiKt4EMPFlow/")
    for mets in pfmet_types:
        defineHistograms(METPflow_MonAlg, group,helper,mets)



    METCalo_MonAlg = helper.addAlgorithm(METMonitoringAlg,'METCalo_MonAlg')   
    metcalo_types = [ "PEMB", "EMB", "PEME", "EME", "TILE", "HEC", "FCAL" ]
    METCalo_MonAlg.alltrigger = True

    group = helper.addGroup(METCalo_MonAlg, "METMonitor", "MissingEt/AllTriggers/MET_Calo/")
    for mets in metcalo_types:
        defineHistogramsCalo(METCalo_MonAlg, group,helper,mets)

    
# return helper.result(),exampleMonAlg

#    anotherExampleMonAlg = helper.addAlgorithm(exampleMonAlg,'AnotherExampleMonAlg')
    anotherMonAlg = helper.addAlgorithm(METMonitoringAlg,'AnotherMonAlg')
    anotherMonAlg.metKeys = met_types
    anotherMonAlg.dometcut = True
    anotherMonAlg.metcut = 80
    anotherGroup = helper.addGroup(anotherMonAlg,"METMonitor","MissingEt/CutMet80/") 
    for mets in met_types:
        defineHistograms(anotherMonAlg, anotherGroup,helper,mets)

# Jet cleaning

    from AthenaCommon import CfgMgr
    jetCleaningTool = CfgMgr.JetCleaningTool()
    jetCleaningTool.CutLevel = "LooseBad"       
#    jetCleaningTool.CutLevel = "TightBad"       
    jetCleaningTool.DoUgly = False
    
    JetCleaning_METMonAlg = helper.addAlgorithm(METMonitoringAlg,'JetCleaning_METMonAlg')    
    JetCleaning_METMonAlg.metKeys = met_types
    JetCleaning_METMonAlg.DoJetCleaning = True
    JetCleaning_METMonAlg.JetCleaningTool = jetCleaningTool
    JetCleaningGroup = helper.addGroup(JetCleaning_METMonAlg,"METMonitor","MissingEt/Jetcleaning/")
    JetCleaning_METMonAlg.JetContainerName = "AntiKt4EMTopoJets"
    for mets in met_types:
        print mets 
        if mets == 'MET_PFlow_PVSoftTrk':
            JetCleaning_METMonAlg.JetContainerName = "AntiKt4EMPFlowJets"
        defineHistograms(JetCleaning_METMonAlg, JetCleaningGroup,helper,mets)




    # Badjets

    jetCleaningTool.CutLevel = "LooseBad"       
#    jetCleaningTool.CutLevel = "TightBad"       
    jetCleaningTool.DoUgly = False
    
    BadJets_METMonAlg = helper.addAlgorithm(METMonitoringAlg,'BadJets_METMonAlg')    
    BadJets_METMonAlg.metKeys = met_types
    BadJets_METMonAlg.DoJetCleaning = True
    BadJets_METMonAlg.alltrigger = True
    BadJets_METMonAlg.DoBadJets = True
    BadJets_METMonAlg.JetCleaningTool = jetCleaningTool
    BadJets_METMonAlg.JetContainerName = "AntiKt4EMTopoJets"
    BadJetsGroup = helper.addGroup(BadJets_METMonAlg,"METMonitor","MissingEt/AllTriggers/BadJets/")
    for mets in met_types:
        defineHistograms(BadJets_METMonAlg, BadJetsGroup,helper,mets)


    return helper.result()

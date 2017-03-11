# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Authors: Ben Smart (ben.smart@cern.ch)
# See https://twiki.cern.ch/twiki/bin/view/Atlas/MaDQM for more information

# put all storage of information on tool package names etc. in a single class
class PackagesToInterrogate:

    PackagesToInterrogate_test = {
        "Muon"        : { "PackageName" : "TrigMuonMonitoring.TrigMuonMonitoringConfig",       "ToolName" : "TrigMuonMonitoringTool",      "MonitCategoryName" : "TrigMuonMonitoring.TrigMuonMonitCategory",       "Dll" : "TrigMuonMonitoring"}
    }

    PackagesToInterrogate = {
        "General"     : { "PackageName" : "TrigHLTMonitoring.TrigHLTMonitoringConfig",         "ToolName" : "HLTGeneralTool",              "MonitCategoryName" : "",                                               "Dll" : "TrigHLTMonitoring"},
        "Bjet"        : { "PackageName" : "TrigBjetMonitoring.TrigBjetMonitoringConfig",       "ToolName" : "TrigBjetMonitoringConfig",    "MonitCategoryName" : "TrigBjetMonitoring.TrigBjetMonitCategory",       "Dll" : "TrigBjetMonitoring"},
        "Bphys"       : { "PackageName" : "TrigBphysMonitoring.TrigBphysMonitoringConfig",     "ToolName" : "TrigBphysMonitoringTool",     "MonitCategoryName" : "TrigBphysMonitoring.TrigBphysMonitCategory",     "Dll" : "TrigBphysMonitoring"},
        "Calo"        : { "PackageName" : "TrigCaloMonitoring.TrigCaloMonitoringConfig",       "ToolName" : "HLTCaloMonitoringTool",       "MonitCategoryName" : "",                                               "Dll" : "TrigCaloMonitoring"},
        "Egamma"      : { "PackageName" : "TrigEgammaMonitoring.TrigEgammaMonitoringConfig",   "ToolName" : "TrigEgammaMonitoringTool",    "MonitCategoryName" : "TrigEgammaMonitoring.TrigEgammaMonitCategory",   "Dll" : "TrigEgammaAnalysisTools"},
        "IDJpsi"      : { "PackageName" : "TrigIDJpsiMonitoring.TrigIDJpsiMonitoringConfig",   "ToolName" : "TrigIDJpsiMonitoringTool",    "MonitCategoryName" : "",                                               "Dll" : "TrigIDJpsiMonitoring"},
        "IDtrk"       : { "PackageName" : "TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig",     "ToolName" : "TrigIDtrkMonitoringTool",     "MonitCategoryName" : "",                                               "Dll" : "TrigInDetAnalysisExample"},
        "Jet"         : { "PackageName" : "TrigJetMonitoring.TrigJetMonitoringConfig",         "ToolName" : "TrigJetMonitoringTool",       "MonitCategoryName" : "TrigJetMonitoring.TrigJetMonitCategory",         "Dll" : "TrigJetMonitoring"},
        "MET"         : { "PackageName" : "TrigMETMonitoring.TrigMETMonitoringConfig",         "ToolName" : "HLTMETMonitoringTool",        "MonitCategoryName" : "TrigMETMonitoring.TrigMETMonitCategory",         "Dll" : "TrigMETMonitoring"},
        "MinBias"     : { "PackageName" : "TrigMinBiasMonitoring.TrigMinBiasMonitoringConfig", "ToolName" : "TrigMinBiasMonitoringTool",   "MonitCategoryName" : "TrigMinBiasMonitoring.TrigMinBiasMonitCategory", "Dll" : "TrigMinBiasMonitoring"},
        "Muon"        : { "PackageName" : "TrigMuonMonitoring.TrigMuonMonitoringConfig",       "ToolName" : "TrigMuonMonitoringTool",      "MonitCategoryName" : "TrigMuonMonitoring.TrigMuonMonitCategory",       "Dll" : "TrigMuonMonitoring"},
        "Tau"         : { "PackageName" : "TrigTauMonitoring.TrigTauMonitoringConfig",         "ToolName" : "TrigTauMonitoringTool",       "MonitCategoryName" : "TrigTauMonitoring.TrigTauMonitCategory",         "Dll" : "TrigTauMonitoring"}
    }
    #    "Egamma"      : { "PackageName" : "TrigEgammaMonitoring.TrigEgammaMonitoringConfig",   "ToolName" : "HLTEgammaMonitoringTool",     "MonitCategoryName" : "TrigEgammaMonitoring.TrigEgammaMonitCategory",   "Dll" : "TrigEgammaMonitoring"},
    #    "Egamma_Dump" : { "PackageName" : "TrigEgammaMonitoring.TrigEgammaMonitoringConfig",   "ToolName" : "HLTEgammaMonitoringDumpTool", "MonitCategoryName" : "TrigEgammaMonitoring.TrigEgammaMonitCategory",   "Dll" : "TrigEgammaMonitoring"}
    #}

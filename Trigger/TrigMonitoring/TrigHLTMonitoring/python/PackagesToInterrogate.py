# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# Author: Ben Smart (bsmart@cern.ch)
#

# put all storage of information on tool package names etc. in a single class
class PackagesToInterrogate:

    PackagesToInterrogate_test = {
        "Muon"        : { "PackageName" : "TrigMuonMonitoring.TrigMuonMonitoringConfig",       "ToolName" : "TrigMuonMonitoringTool",      "Dll" : "TrigMuonMonitoring"}
    }

    PackagesToInterrogate = {
        "General"     : { "PackageName" : "TrigHLTMonitoring.TrigHLTMonitoringConfig",         "ToolName" : "HLTGeneralTool",              "Dll" : "TrigHLTMonitoring"},
        "Bjet"        : { "PackageName" : "TrigBjetMonitoring.TrigBjetMonitoringConfig",       "ToolName" : "TrigBjetMonitoringConfig",    "Dll" : "TrigBjetMonitoring"},
        "Bphys"       : { "PackageName" : "TrigBphysMonitoring.TrigBphysMonitoringConfig",     "ToolName" : "TrigBphysMonitoringTool",     "Dll" : "TrigBphysMonitoring"}, 
        "Calo"        : { "PackageName" : "TrigCaloMonitoring.TrigCaloMonitoringConfig",       "ToolName" : "HLTCaloMonitoringTool",       "Dll" : "TrigCaloMonitoring"}, 
        "Egamma"      : { "PackageName" : "TrigEgammaMonitoring.TrigEgammaMonitoringConfig",   "ToolName" : "HLTEgammaMonitoringTool",     "Dll" : "TrigEgammaMonitoring"}, 
        "IDJpsi"      : { "PackageName" : "TrigIDJpsiMonitoring.TrigIDJpsiMonitoringConfig",   "ToolName" : "TrigIDJpsiMonitoringTool",    "Dll" : "TrigIDJpsiMonitoring"}, 
        "IDtrk"       : { "PackageName" : "TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig",     "ToolName" : "TrigIDtrkMonitoringTool",     "Dll" : "TrigInDetAnalysisExample"}, 
        "Jet"         : { "PackageName" : "TrigJetMonitoring.TrigJetMonitoringConfig",         "ToolName" : "TrigJetMonitoringTool",       "Dll" : "TrigJetMonitoring"}, 
        "MET"         : { "PackageName" : "TrigMETMonitoring.TrigMETMonitoringConfig",         "ToolName" : "HLTMETMonitoringTool",        "Dll" : "TrigMETMonitoring"}, 
        "MinBias"     : { "PackageName" : "TrigMinBiasMonitoring.TrigMinBiasMonitoringConfig", "ToolName" : "TrigMinBiasMonitoringTool",   "Dll" : "TrigMinBiasMonitoring"}, 
        "Muon"        : { "PackageName" : "TrigMuonMonitoring.TrigMuonMonitoringConfig",       "ToolName" : "TrigMuonMonitoringTool",      "Dll" : "TrigMuonMonitoring"}, 
        "Tau"         : { "PackageName" : "TrigTauMonitoring.TrigTauMonitoringConfig",         "ToolName" : "TrigTauMonitoringTool",       "Dll" : "TrigTauMonitoring"},
        "Egamma_Dump" : { "PackageName" : "TrigEgammaMonitoring.TrigEgammaMonitoringConfig",   "ToolName" : "HLTEgammaMonitoringDumpTool", "Dll" : "TrigEgammaMonitoring"}
    }

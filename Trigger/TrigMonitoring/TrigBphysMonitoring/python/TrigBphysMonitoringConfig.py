# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



#JW: quick methods to access as many triggers as possible

#from TrigEDMConfig import TriggerEDM
#containers = [x[0] for x in TriggerEDM.TriggerHLTList if "TrigBphysContainer" in x[0]]

from TrigHLTMonitoring.HLTMonTriggerList import HLTMonTriggerList
hltmonList = HLTMonTriggerList()

# default configuration for pp
#if hltmonList.pp_mode :
containers = [
              "HLT_xAOD__TrigBphysContainer_L2BMuMuFex",  "HLT_xAOD__TrigBphysContainer_EFBMuMuFex",
              #"HLT_xAOD__TrigBphysContainer_L2BMuMuXFex", 
              "HLT_xAOD__TrigBphysContainer_EFBMuMuXFex",
              "HLT_xAOD__TrigBphysContainer_L2MultiMuFex","HLT_xAOD__TrigBphysContainer_EFMultiMuFex",
              "HLT_xAOD__TrigBphysContainer_L2TrackMass", "HLT_xAOD__TrigBphysContainer_EFTrackMass"
              ]
DetailedChains = {
                  "BMuMu"  : 'HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_(bDimu|bJpsimumu)',
                  "BMuMuX" : 'HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_bBmumuxv[23]',
                  "MultiMu": 'HLT_((mu[0-9]+_[2-9]mu[0-9]+)|(3mu[0-9]+))_(b.*)',
                  "MuTrack": 'HLT_(mu[0-9]+)_(b.*)_Trk(.*)',
                  "Tau3mu" : 'HLT_([2-3]?mu[0-9]+).*_bTau.*',
                  "FTK"    : 'HLT_(2mu[0-9]+)_bBmumuxv2_Ftk'
                 }
DetailedL1TopoChains = {
                      "L1BPH-M-DR-low"    : "HLT_2mu6_bJpsimumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6",
                      "L1BPH-M-DR"        : "HLT_2mu6_bUpsimumu_L1BPH-8M15-2MU6_BPH-0DR22-2MU6",
                      #"L1BPH-M"           : "HLT_2mu6_bDimu_L1BPH-8M15-2MU6"
                       }
EfficiencyChains = {
                    "BMuMu"  : "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_(bDimu|bJpsimumu)",
                    "BMuMuX" : "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_bBmumuxv[23]",
                    #"MultiMu": 'HLT_((mu[0-9]+_[2-9]mu[0-9]+)|(3mu[0-9]+))_(b.*)',
                    #"MuTrack": "HLT_(mu[0-9]+)_(b.*)_Trk(.*)"
                   }
EffTrigDenom_noVtxOS = "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_bDimu_novtx_noos"

# configuration for HI and pPb
if hltmonList.HI_mode :
    containers = ["HLT_xAOD__TrigBphysContainer_L2BMuMuFex",  "HLT_xAOD__TrigBphysContainer_EFBMuMuFex"]
    DetailedChains = {"BMuMu"  : 'HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_(bDimu|bJpsimumu)'}
    DetailedL1TopoChains = {}
    EfficiencyChains = {"BMuMu"  : "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_(bDimu|bJpsimumu)"}
    EffTrigDenom_noVtxOS = "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_bDimu_novtx_noos"

# change nothing for cosmic
if hltmonList.cosmic_mode :
    pass
  

    
## Attempt some simple matchin of trigger group to monitoring group;
## Not complete, as non matched items fall into a 'misc' group
#monGroups = []
#import re
#re_dimuon  = re.compile("HLT_(2mu\d+|mu\d+_?mu\d+)_(b.*)")
#re_trimuon = re.compile("HLT_(3mu\d+)_(b.*)")


##Code to determine the form of the trigger
#def getForm(trigger_name):
    #"""Get the class of trigger type from the name. Output warning if not found."""
    #allowed_values = {"BMuMu"  :[re.compile("HLT_(2mu\d+|mu\d+_?mu\d+)_(b.*)")],
                #"BMuMuX" :[re.compile("HLT_(2mu\d+|mu\d+_?mu\d+)_(b.*)")],
                #"MultiMu":[re.compile("HLT_(3mu\d+)_(b.*)")],
                #"TrkMass":[re.compile("HLT_(\d?mu\d+)_(b")],
                #}
    #pass

#from AthenaCommon.AppMgr import ToolSvc
#from AthenaCommon import CfgMgr
#ToolSvc += CfgMgr.TrigConf__xAODConfigTool( "xAODConfigTool" )
#ToolSvc += CfgMgr.Trig__TrigDecisionTool( "TrigDecisionTool", ConfigTool = ToolSvc.xAODConfigTool, TrigDecisionKey = "xTrigDecision" )


#for trig in bphysTriggers:
    #res_dimuon  = re_dimuon.search(trig)
    #res_trimuon = re_trimuon.search(trig)
    #if res_dimuon:
        #monGroups.append("HLT_%s_Dimuon" % res_dimuon.group(1))
    #elif res_trimuon:
        #monGroups.append("HLT_%s_Trimuon"% res_trimuon.group(1))
    #else:
        #monGroups.append("HLT_bphys_misc")
    #pass

from AthenaCommon.AppMgr import ToolSvc
def TrigBphysMonitoringTool():
    from TrigBphysMonitoring.TrigBphysMonitoringConf import HLTXAODBphysMonTool
    from TrigHLTMonitoring.HLTMonTriggerList import hltmonList
    HLTBphysMon = HLTXAODBphysMonTool(name       = 'HLTBphysMon',
                                  #OutputLevel = 1,
                                  JpsiCandidates = "JpsiCandidates",
                                  BphysShifterPath='HLT/BphysMon/shifter',
                                  BphysExpertPath ='HLT/BphysMon/expert',
                                  BphysHistPrefix ='TrigBphys',
                                  monitoring_bphys = hltmonList.monitoring_bphys,
                                  primary_bphys = hltmonList.primary_bphys,
                                  GenerateChainDictsFromDB = True, #if True, the following dictionaries are overwritten
                                  
                                  ContainerList   =containers,
                                  DetailedChains_patterns = DetailedChains,
                                  DetailedL1TopoChains_patterns = DetailedL1TopoChains,
                                  EfficiencyChains_patterns = EfficiencyChains,
                                  EffTrigDenom_noVtxOS_pattern = EffTrigDenom_noVtxOS,
                                  
                                  MW_jpsi_forward_min =  2800,
                                  MW_jpsi_forward_max =  3400,
                                  MW_upsi_forward_min =  8000,
                                  MW_upsi_forward_max =  12000,
                                  MW_jpsi_central_min =  2950,
                                  MW_jpsi_central_max =  3250,
                                  MW_upsi_central_min =  9000,
                                  MW_upsi_central_max =  11500,
                                  Eta_min             =  -3.,
                                  Eta_max             =  3.,
                                  Mu_pt_min           =  0.,
                                  Mu_pt_max           =  50.,
                                  Trk_pt_min          =  0.,
                                  Trk_pt_max          =  25.,
                                  Z0_min              =  -150.,
                                  Z0_max              =  150.,
                                  D0_min              =  -11.,
                                  D0_max              =  11.,
                                  DeltaPhi_min        =  0.,
                                  DeltaPhi_max        =  3.2,
                                  DeltaEta_min        =  0.,
                                  DeltaEta_max        =  3.,
                                  DeltaR_min          =  0.,
                                  DeltaR_max          =  3.,
                                  PtSum_min           =  0.,
                                  PtSum_max           =  100.,
                                  OniaMass_min        =  2000,
                                  OniaMass_max        =  12000,
                                  TauMass_min         =  0,
                                  TauMass_max         =  3000,
                                  OniaPt_min          =  8,
                                  OniaPt_max          =  100,
                                  MassErr_min         =  0.,
                                  MassErr_max         =  200.,
                                  Lxy_min             =  -2.,
                                  Lxy_max             =  20.,
                                  LxyErr_min          =  0.,
                                  LxyErr_max          =  2.,
                                  Tau_min             =  -2.,
                                  Tau_max             =  20.,
                                  TauErr_min          =  0.,
                                  TauErr_max          =  2.,
                                  pT_min              =  0.,
                                  pT_max              =  100.,
                                  pTErr_min           =  0.,
                                  pTErr_max           =  10.
                                  )
    
    from AthenaCommon.AppMgr import ToolSvc
    #ToolSvc += HLTBphysMon
    items = [ HLTBphysMon ]
    return items


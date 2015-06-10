# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



#JW: quick methods to access as many triggers as possible

#from TrigEDMConfig import TriggerEDM
#containers = [x[0] for x in TriggerEDM.TriggerHLTList if "TrigBphysContainer" in x[0]]
containers = ["HLT_xAOD__TrigBphysContainer_L2BMuMuFex",  "HLT_xAOD__TrigBphysContainer_EFBMuMuFex",
              #"HLT_xAOD__TrigBphysContainer_L2BMuMuXFex", 
              #"HLT_xAOD__TrigBphysContainer_EFBMuMuXFex",
              "HLT_xAOD__TrigBphysContainer_L2MultiMuFex","HLT_xAOD__TrigBphysContainer_EFMultiMuFex",
              "HLT_xAOD__TrigBphysContainer_L2TrackMass", "HLT_xAOD__TrigBphysContainer_EFTrackMass"]


#from TriggerMenu.menu import DC14 as menu  #note should move to MC asap
#from TriggerJobOpts.TriggerFlags          import TriggerFlags
#menu.setupMenu()
#print TriggerFlags.BphysicsSlice.signatures
#bphysTriggers = sorted(['HLT_'+x[0] for x in TriggerFlags.BphysicsSlice.signatures()])

#bphysTriggers = ['HLT_2mu4_bDimu', 'HLT_2mu4_bDimu_novtx_noos', 'HLT_2mu6_bDimu', 'HLT_2mu6_bDimu_novtx_noos',
                     #'HLT_2mu6_bJpsimumu','HLT_mu6_bJpsi_Trkloose','HLT_2mu6_bBmumuxv2',
                                      #'HLT_2mu6_bBmumux_BpmumuKp','HLT_mu18_bJpsi_Trkloose']
#bphysTriggers += ['HLT_2mu4_bDimu_L1BPH-2M-2MU4', 'HLT_2mu4_bDimu_L1BPH-2M-2MU4-B', 'HLT_2mu4_bDimu_L1BPH-2M-2MU4-BO', 'HLT_2mu4_bDimu_L1BPH-4M8-2MU4', 'HLT_2mu4_bDimu_L1BPH-4M8-2MU4-B', 'HLT_2mu4_bDimu_L1BPH-4M8-2MU4-BO', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-B', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BO', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BPH-2M-2MU4', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BPH-4M8-2MU4', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4-B', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4-BO', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4-B', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4-BO', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BPH-2M-2MU4', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BPH-4M8-2MU4']

# Attempt some simple matchin of trigger group to monitoring group;
# Not complete, as non matched items fall into a 'misc' group
monGroups = []
import re
re_dimuon  = re.compile("HLT_(2mu\d+|mu\d+_?mu\d+)_(b.*)")
re_trimuon = re.compile("HLT_(3mu\d+)_(b.*)")


#Code to determine the form of the trigger
def getForm(trigger_name):
    """Get the class of trigger type from the name. Output warning if not found."""
    allowed_values = {"BMuMu"  :[re.compile("HLT_(2mu\d+|mu\d+_?mu\d+)_(b.*)")],
                "BMuMuX" :[re.compile("HLT_(2mu\d+|mu\d+_?mu\d+)_(b.*)")],
                "MultiMu":[re.compile("HLT_(3mu\d+)_(b.*)")],
                "TrkMass":[re.compile("HLT_(\d?mu\d+)_(b")],
                }
    pass

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
                                  
                                  #DetailedChains = {"BMuMu"  : "HLT_2mu4_bJpsimumu",
                                  #                  "BMuMuX" : "HLT_2mu4_bBmumuxv2",
                                  #                  "MultiMu": "HLT_3mu4_bJpsi",
                                  #                  "MuTrack": "HLT_mu6_bJpsi_Trkloose"
                                  #                 },
                                  #DetailedL1TopoChains = {"L1BPH-2M"     : "HLT_2mu4_bDimu_L1BPH-2M-2MU4",
                                  #                        "L1BPH-4M8"    : "HLT_2mu4_bDimu_L1BPH-4M8-2MU4",
                                  #                        "L1BPH-DR"     : "HLT_2mu4_bDimu_L1BPH-DR-2MU4",
                                  #                        "L1BPH-DR-2M"  : "HLT_2mu4_bDimu_L1BPH-DR-2MU4-BPH-2M-2MU4",
                                  #                        "L1BPH-DR-4M8" : "HLT_2mu4_bDimu_L1BPH-DR-2MU4-BPH-4M8-2MU4",
                                  #                        "L1BPH-B"      : "HLT_2mu4_bDimu_L1BPH-2M-2MU4-B",
                                  #                        "L1BPH-BO"     : "HLT_2mu4_bDimu_L1BPH-2M-2MU4-BO"
                                  #                       },
                                  #EfficiencyChains = {"BMuMu"  : "HLT_2mu4_bJpsimumu",
                                  #                    "BMuMuX" : "HLT_2mu4_bBmumuxv2",
                                  #                    "MultiMu": "HLT_3mu4_bJpsi",
                                  #                    "MuTrack": "HLT_mu6_bJpsi_Trkloose"
                                  #                   },
                                  #EffTrigDenom_noVtxOS = "HLT_2mu4_bDimu_novtx_noos",
                                  
                                  DetailedChains_patterns = {"BMuMu"  : 'HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_(bDimu|bJpsimumu)',
                                                             "BMuMuX" : 'HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_bBmumuxv[23]',
                                                             "MultiMu": 'HLT_(3mu[0-9]+)_(b.*)',
                                                             "MuTrack": 'HLT_(mu[0-9]+)_(b.*)_Trkloose',
                                                             "BMuMu_express"  : 'HLT_2mu6_(bDimu|bJpsimumu)',
                                                             "BMuMuX_express" : 'HLT_2mu6_bBmumuxv[23]',
                                                             "MultiMu_express": 'HLT_3mu6_(b.*)',
                                                             "MuTrack_express": 'HLT_mu6_(b.*)_Trkloose'
                                                            },
                                  DetailedL1TopoChains_patterns = {"L1BPH-2M"     : "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_(b.*)_L1BPH-2M-2MU4",
                                                                   "L1BPH-4M8"    : "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_(b.*)_L1BPH-4M8-2MU4",
                                                                   "L1BPH-DR"     : "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_(b.*)_L1BPH-DR-2MU4",
                                                                   "L1BPH-DR-2M"  : "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_(b.*)_L1BPH-DR-2MU4-BPH-2M-2MU4",
                                                                   "L1BPH-DR-4M8" : "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_(b.*)_L1BPH-DR-2MU4-BPH-4M8-2MU4",
                                                                   "L1BPH-B"      : "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_(b.*)_L1BPH.*-2MU4-B",
                                                                   "L1BPH-BO"     : "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_(b.*)_L1BPH.*-2MU4-BO",
                                                                   "L1BPH-2M_express"     : "HLT_2mu6_(b.*)_L1BPH-2M-2MU4",
                                                                   "L1BPH-4M8_express"    : "HLT_2mu6_(b.*)_L1BPH-4M8-2MU4",
                                                                   "L1BPH-DR_express"     : "HLT_2mu6_(b.*)_L1BPH-DR-2MU4",
                                                                   "L1BPH-DR-2M_express"  : "HLT_2mu6_(b.*)_L1BPH-DR-2MU4-BPH-2M-2MU4",
                                                                   "L1BPH-DR-4M8_express" : "HLT_2mu6_(b.*)_L1BPH-DR-2MU4-BPH-4M8-2MU4",
                                                                   "L1BPH-B_express"      : "HLT_2mu6_(b.*)_L1BPH.*-2MU4-B",
                                                                   "L1BPH-BO_express"     : "HLT_2mu6_(b.*)_L1BPH.*-2MU4-BO"
                                                                  },
                                  EfficiencyChains_patterns = {"BMuMu"  : "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_(bDimu|bJpsimumu)",
                                                               "BMuMuX" : "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_bBmumuxv[23]",
                                                               "MultiMu": "HLT_(3mu[0-9]+)_(b.*)",
                                                               "MuTrack": "HLT_(mu[0-9]+)_(b.*)_Trkloose"
                                                              },
                                  EffTrigDenom_noVtxOS_pattern = "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_bDimu_novtx_noos",

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
                                  pTErr_max           =  10.,
                                  )
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += HLTBphysMon
    items = [ "HLTXAODBphysMonTool/HLTBphysMon" ]
    return items


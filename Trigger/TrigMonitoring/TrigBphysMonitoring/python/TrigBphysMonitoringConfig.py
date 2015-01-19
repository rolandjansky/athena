# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



#JW: quick methods to access as many triggers as possible

#from TrigEDMConfig import TriggerEDM
#containers = [x[0] for x in TriggerEDM.TriggerHLTList if "TrigBphysContainer" in x[0]]
containers = ["HLT_xAOD__TrigBphysContainer_EFBMuMuFex"]

#from TriggerMenu.menu import DC14 as menu  #note should move to MC asap
#from TriggerJobOpts.TriggerFlags          import TriggerFlags
#menu.setupMenu()
#print TriggerFlags.BphysicsSlice.signatures
#bphysTriggers = = sorted(['HLT_'+x[0] for x in TriggerFlags.BphysicsSlice.signatures()])


bphysTriggers = ['HLT_2mu10_bBmumu', 'HLT_2mu10_bBmumux_BcmumuDsloose', 'HLT_2mu10_bBmumuxv2', 'HLT_2mu10_bJpsimumu', 'HLT_2mu10_bUpsimumu', 'HLT_2mu6_bBmumu', 'HLT_2mu6_bBmumux_BcmumuDsloose', 'HLT_2mu6_bBmumuxv2', 'HLT_2mu6_bDimu', 'HLT_2mu6_bDimu_novtx_noos', 'HLT_2mu6_bJpsimumu', 'HLT_2mu6_bUpsimumu', 'HLT_3mu6_bDimu', 'HLT_3mu6_bJpsi', 'HLT_3mu6_bTau', 'HLT_mu13_mu13_idperf_Zmumu', 'HLT_mu4_iloose_mu4_7invm9_noos', 'HLT_mu4_mu4_idperf_bJpsimumu_noid', 'HLT_mu6_bJpsi_Trkloose']


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



for trig in bphysTriggers:
    res_dimuon  = re_dimuon.search(trig)
    res_trimuon = re_trimuon.search(trig)
    if res_dimuon:
        monGroups.append("HLT_%s_Dimuon" % res_dimuon.group(1))
    elif res_trimuon:
        monGroups.append("HLT_%s_Trimuon"% res_trimuon.group(1))
    else:
        monGroups.append("HLT_bphys_misc")
    pass

from AthenaCommon.AppMgr import ToolSvc
def TrigBphysMonitoringTool():
    from TrigBphysMonitoring.TrigBphysMonitoringConf import HLTXAODBphysMonTool
    HLTBphysMon = HLTXAODBphysMonTool(name       = 'HLTBphysMon',
                                  JpsiCandidates = "JpsiCandidates",
                                  BphysShifterPath='HLT/BphysMon/shifter',
                                  BphysExpertPath ='HLT/BphysMon/expert',
                                  BphysHistPrefix ='TrigBphys',
                                  MonitoredChains =bphysTriggers, # carfeful here
                                  ContainerList   =containers,
                                  DetailedChains = {"BMuMu"  : "HLT_2mu6_bJpsimumu",
                                                    "BMuMuX" : "HLT_2mu6_bBmumuxv2",
                                                    "MuTrack": "HLT_mu6_bJpsi_Trkloose"
                                                   },
                                  EfficiencyChains = {"BMuMu"  : "HLT_2mu6_bJpsimumu",
                                                      "BMuMuX" : "HLT_2mu6_bBmumuxv2",
                                                      "MuTrack": "HLT_mu6_bJpsi_Trkloose"
                                                     },
                                  EffTrigDenom_noVtxOS = "HLT_2mu6_bDimu_novtx_noos",

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
                                  D0_min              =  -20.,
                                  D0_max              =  20.,
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
                                  )
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += HLTBphysMon
    items = [ "HLTXAODBphysMonTool/HLTBphysMon" ]
    return items


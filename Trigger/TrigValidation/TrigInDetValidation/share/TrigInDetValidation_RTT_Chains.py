from TriggerMenu import useNewTriggerMenu 
use_new_tm = useNewTriggerMenu() 

def electronChains(runMergedChain):

  idTrigChainlist = []
  tidaAnalysischains = ["Truth",
                        "Offline",
                        "Electrons",
                        ]
  
  if not use_new_tm:
    idTrigChainlist += [
      'e22vh_medium_IDTrkNoCut',
      ]
    tidaAnalysischains += [
      "L2_e22vh_medium_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:0",
      "L2_e22vh_medium_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:1",
      "L2_e22vh_medium_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:2",
      "EF_e22vh_medium_IDTrkNoCut:InDetTrigParticleCreation_Electron_EFID",  
      ]
  else:
    idTrigChainlist.append( ['e24_medium_L2Star_idperf', 'L1_EM18VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'],1])
    idTrigChainlist.append( ['e5_loose_L2Star_idperf', 'L1_EM3', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'],1])
    tidaAnalysischains += [
      "HLT_e24_medium_L2Star_idperf:TrigL2SiTrackFinder_eGamma:0",
      "HLT_e24_medium_L2Star_idperf:TrigL2SiTrackFinder_eGamma:1",
      "HLT_e24_medium_L2Star_idperf:TrigL2SiTrackFinder_eGamma:2",
      "HLT_e24_medium_L2Star_idperf:InDetTrigParticleCreation_Electron_EFID",  
      "HLT_e24_medium_L2Star_idperf:InDetTrigTrackingxAODCnv_Electron_EFID",
      "HLT_e5_loose_L2Star_idperf:TrigL2SiTrackFinder_eGamma:0",
      "HLT_e5_loose_L2Star_idperf:TrigL2SiTrackFinder_eGamma:1",
      "HLT_e5_loose_L2Star_idperf:TrigL2SiTrackFinder_eGamma:2",
      "HLT_e5_loose_L2Star_idperf:InDetTrigParticleCreation_Electron_EFID",  
      "HLT_e5_loose_L2Star_idperf:InDetTrigTrackingxAODCnv_Electron_EFID",  
      ]
  
  if 'runMergedChain' in dir() and runMergedChain==True:
    if use_new_tm:
      idTrigChainlist.append(  ['e24_medium_idperf', 'L1_EM18VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'],1])
      idTrigChainlist.append(   ['e5_loose_idperf', 'L1_EM3', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'],1])
      tidaAnalysischains.append('HLT_e24_medium_idperf:TrigFastTrackFinder_eGamma')
      tidaAnalysischains.append('HLT_e24_medium_idperf:InDetTrigParticleCreation_Electron_EFID')
      tidaAnalysischains.append('HLT_e24_medium_idperf:InDetTrigTrackingxAODCnv_Electron_EFID')
      tidaAnalysischains.append('HLT_e5_loose_idperf:TrigFastTrackFinder_eGamma')
      tidaAnalysischains.append('HLT_e5_loose_idperf:InDetTrigParticleCreation_Electron_EFID')
      tidaAnalysischains.append('HLT_e5_loose_idperf:InDetTrigTrackingxAODCnv_Electron_EFID')
    else:
      idTrigChainlist.append('e24vh_medium1_IDTrkNoCut_IDT')
      tidaAnalysischains.append('EF_e24vh_medium1_IDTrkNoCut_IDT:TrigFastTrackFinder_eGamma')
      tidaAnalysischains.append('EF_e24vh_medium1_IDTrkNoCut_IDT:InDetTrigParticleCreation_Electron_EFID')

  return (idTrigChainlist, tidaAnalysischains)

def muonChains(runMergedChain):
  idTrigChainlist = []
  tidaAnalysischains = ["Truth",
                        "Offline",
                        "Muons",
                        ]
  
  if not use_new_tm:
    idTrigChainlist += [
      'mu22_IDTrkNoCut_tight'
      ]
    tidaAnalysischains.append("L2_mu22_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:0")
    tidaAnalysischains.append("L2_mu22_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:1")
    tidaAnalysischains.append("L2_mu22_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:2")
    tidaAnalysischains.append("EF_mu22_IDTrkNoCut_tight:InDetTrigParticleCreation_Muon_EFID")
  
  else:
    idTrigChainlist.append(['mu24_L2Star_idperf', 'L1_MU20', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'],   1])
    idTrigChainlist.append(['mu6_L2Star_idperf', 'L1_MU4', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'],   1])
    tidaAnalysischains.append('HLT_mu24_L2Star_idperf:TrigL2SiTrackFinder_Muon:0')
    tidaAnalysischains.append('HLT_mu24_L2Star_idperf:TrigL2SiTrackFinder_Muon:1')
    tidaAnalysischains.append('HLT_mu24_L2Star_idperf:TrigL2SiTrackFinder_Muon:2')
    tidaAnalysischains.append('HLT_mu24_L2Star_idperf:InDetTrigParticleCreation_Muon_EFID')
    tidaAnalysischains.append('HLT_mu24_L2Star_idperf:InDetTrigTrackingxAODCnv_Muon_EFID')
    tidaAnalysischains.append('HLT_mu6_L2Star_idperf:TrigL2SiTrackFinder_Muon:0')
    tidaAnalysischains.append('HLT_mu6_L2Star_idperf:TrigL2SiTrackFinder_Muon:1')
    tidaAnalysischains.append('HLT_mu6_L2Star_idperf:TrigL2SiTrackFinder_Muon:2')
    tidaAnalysischains.append('HLT_mu6_L2Star_idperf:InDetTrigParticleCreation_Muon_EFID')
    tidaAnalysischains.append('HLT_mu6_L2Star_idperf:InDetTrigTrackingxAODCnv_Muon_EFID')

  if runMergedChain==True:
    if use_new_tm:
      idTrigChainlist.append(['mu24_idperf',  'L1_MU20', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'],   1])
      idTrigChainlist.append(['mu6_idperf',   'L1_MU4', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'],   1])
      tidaAnalysischains.append('HLT_mu24_idperf:TrigFastTrackFinder_Muon')
      tidaAnalysischains.append('HLT_mu24_idperf:InDetTrigParticleCreation_Muon_EFID')
      tidaAnalysischains.append('HLT_mu24_idperf:InDetTrigTrackingxAODCnv_Muon_EFID')
      tidaAnalysischains.append('HLT_mu6_idperf:TrigFastTrackFinder_Muon')
      tidaAnalysischains.append('HLT_mu6_idperf:InDetTrigParticleCreation_Muon_EFID')
      tidaAnalysischains.append('HLT_mu6_idperf:InDetTrigTrackingxAODCnv_Muon_EFID')
    else:
      idTrigChainlist.append('mu22_IDTrkNoCut_tight_IDT')
      tidaAnalysischains.append('EF_mu22_IDTrkNoCut_tight_IDT:TrigFastTrackFinder_Muon')
      tidaAnalysischains.append('EF_mu22_IDTrkNoCut_tight_IDT:InDetTrigParticleCreation_Muon_EFID')

  return (idTrigChainlist, tidaAnalysischains)

def tauChains(runMergedChain):
  idTrigChainlist = []
  tidaAnalysischains = ["Truth",
                        "Offline",
                        "Taus",
                        ]
  
  if not use_new_tm:
    idTrigChainlist += [
      'tau29_IDTrkNoCut'
      ]
    tidaAnalysischains.append("L2_tau29_IDTrkNoCu:tTrigL2SiTrackFinder_Tau:0")
    tidaAnalysischains.append("L2_tau29_IDTrkNoCut:TrigL2SiTrackFinder_Tau:1")
    tidaAnalysischains.append("L2_tau29_IDTrkNoCut:TrigL2SiTrackFinder_Tau:2")
    tidaAnalysischains.append("EF_tau29_IDTrkNoCut:InDetTrigParticleCreation_Tau_EFID")
  
  else:
    idTrigChainlist.append(['tau20_r1_idperf',  'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1])
    tidaAnalysischains.append('HLT_tau20_r1_idperf:TrigFastTrackFinder_Tau')
    tidaAnalysischains.append('HLT_tau20_r1_idperf:TrigFastTrackFinder_Tau_IDTrig')
    tidaAnalysischains.append('HLT_tau20_r1_idperf:TrigL2SiTrackFinder_Tau:0')
    tidaAnalysischains.append('HLT_tau20_r1_idperf:TrigL2SiTrackFinder_Tau:1')
    tidaAnalysischains.append('HLT_tau20_r1_idperf:TrigL2SiTrackFinder_Tau:2')
    tidaAnalysischains.append('HLT_tau20_r1_idperf:InDetTrigParticleCreation_Tau_EFID')
    tidaAnalysischains.append('HLT_tau20_r1_idperf:InDetTrigTrackingxAODCnv_Tau_EFID')
  
  
  if runMergedChain==True:
    if use_new_tm:
      idTrigChainlist.append(['tau29_idperf', 'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1])
      tidaAnalysischains.append('HLT_tau29_idperf:TrigFastTrackFinder_Tau')
      tidaAnalysischains.append('HLT_tau29_idperf:TrigFastTrackFinder_Tau_IDTrig')
      tidaAnalysischains.append('HLT_tau29_idperf:InDetTrigParticleCreation_Tau_EFID')
      tidaAnalysischains.append('HLT_tau29_idperf:InDetTrigTrackingxAODCnv_Tau_EFID')
    else:
      idTrigChainlist.append('tau29_IDTrkNoCut_IDT')
      tidaAnalysischains.append('EF_tau29_IDTrkNoCut_IDT:TrigFastTrackFinder_Tau')
      tidaAnalysischains.append('EF_tau29_IDTrkNoCut_IDT:TrigFastTrackFinder_Tau_IDTrig')
      tidaAnalysischains.append('EF_tau29_IDTrkNoCut_IDT:InDetTrigParticleCreation_Tau_EFID')

  return (idTrigChainlist, tidaAnalysischains)


def bjetChains(runMergedChain):
  idTrigChainlist = []
  tidaAnalysischains = ["Truth",
                        "Offline",
                        ]
  
  if not use_new_tm:
    idTrigChainlist += [
      'b55_NoCut_j55_a4tchad'
      ]
    tidaAnalysischains += [
      "L2_b50_NoCut_j50_c4cchad:TrigL2SiTrackFinder_Jet:0",
      "L2_b50_NoCut_j50_c4cchad:TrigL2SiTrackFinder_Jet:1",
      "L2_b50_NoCut_j50_c4cchad:TrigL2SiTrackFinder_Jet:2",
      "EF_b55_NoCut_j55_a4tchad:InDetTrigParticleCreation_Bjet_EFID"
      ] 
  else :
    idTrigChainlist.append( ['j55_bperf',        'L1_J20', [], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],)
    idTrigChainlist.append( ['j55_EFID_bperf',   'L1_J20', [], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],)
    idTrigChainlist.append( ['j55_bperf_split',  'L1_J20', [], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],)
    tidaAnalysischains += [
      'HLT_j55_bperf:TrigFastTrackFinder_Jet',
      'HLT_j55_bperf:InDetTrigParticleCreation_Bjet_EFID',
      'HLT_j55_bperf:InDetTrigTrackingxAODCnv_Bjet_EFID',
      #
      'HLT_j55_bperf_split:TrigFastTrackFinder_Jet',
      'HLT_j55_bperf_split:InDetTrigParticleCreation_Bjet_EFID',
      'HLT_j55_bperf_split:InDetTrigTrackingxAODCnv_Bjet_EFID',
      #
      'HLT_j55_EFID_bperf:InDetTrigParticleCreation_Bjet_EFID',
      'HLT_j55_EFID_bperf:InDetTrigTrackingxAODCnv_Bjet_EFID',
      ]
  return (idTrigChainlist, tidaAnalysischains)

def minBiasChains(runMergedChain):
  idTrigChainlist = []
  tidaAnalysischains = ["Truth",
                        "Offline",
                        ]

  if not use_new_tm:
    idTrigChainlist += [ 'InDetMonMergeDP_FS']
    tidaAnalysischains.append["L2_InDetMonMergeDP_FS:TrigL2SiTrackFinder_FullScan:0"]
    tidaAnalysischains.append["L2_InDetMonMergeDP_FS:TrigL2SiTrackFinder_FullScan:1"]
    tidaAnalysischains.append["L2_InDetMonMergeDP_FS:TrigL2SiTrackFinder_FullScan:2"]
    tidaAnalysischains.append["L2_InDetMonMergeDP_FS:TrigL2SiTrackFinder_FullScan:3"]
    tidaAnalysischains.append["EF_InDetMonMergeDP_FS:InDetTrigParticleCreation_FullScan_EFID"]
  else:
    idTrigChainlist.append(['mb_idperf_L1MBTS_2', 'L1_MBTS_2', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1])
    tidaAnalysischains.append('HLT_mb_idperf_L1MBTS_2:InDetTrigParticleCreation_minBias_EFID')
    tidaAnalysischains.append('HLT_mb_idperf_L1MBTS_2:InDetTrigTrackingxAODCnv_minBias_EFID') 
  
  return (idTrigChainlist, tidaAnalysischains)


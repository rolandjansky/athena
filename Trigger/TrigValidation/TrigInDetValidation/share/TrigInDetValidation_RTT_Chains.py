from TriggerMenu import useNewTriggerMenu 
use_new_tm = useNewTriggerMenu() 

def electronChains(doIDNewTracking):

  idTrigChainlist = []
  tidaAnalysischains = ["Truth"]

  if doIDNewTracking:
    tidaAnalysischains += ["Offline"]
  
  idTrigChainlist.append(  ['e24_medium_idperf', 'L1_EM18VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'],1])
  idTrigChainlist.append(   ['e5_loose_idperf', 'L1_EM3', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'],1])
  tidaAnalysischains.append('HLT_e24_medium_idperf:TrigFastTrackFinder_eGamma')
  tidaAnalysischains.append('HLT_e24_medium_idperf:InDetTrigTrackingxAODCnv_Electron_IDTrig')
  tidaAnalysischains.append('HLT_e24_medium_idperf:InDetTrigTrackingxAODCnv_Electron_FTF')
  tidaAnalysischains.append('HLT_e5_loose_idperf:TrigFastTrackFinder_eGamma')
  tidaAnalysischains.append('HLT_e5_loose_idperf:InDetTrigTrackingxAODCnv_Electron_IDTrig')
  tidaAnalysischains.append('HLT_e5_loose_idperf:InDetTrigTrackingxAODCnv_Electron_FTF')

  return (idTrigChainlist, tidaAnalysischains)

def muonChains(runMergedChain, doIDNewTracking, doFTK):
  idTrigChainlist = []

  tidaAnalysischains = ["Truth"]

  if doIDNewTracking:
    tidaAnalysischains += ["Offline"]
  
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
    idTrigChainlist.append(['mu6_L2Star_idperf', 'L1_MU6', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'],   1])
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
      idTrigChainlist.append(['mu6_idperf',   'L1_MU6', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'],   1])
      tidaAnalysischains.append('HLT_mu24_idperf:InDetTrigTrackingxAODCnv_Muon_FTF')
      tidaAnalysischains.append('HLT_mu24_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig')
      tidaAnalysischains.append('HLT_mu6_idperf:InDetTrigTrackingxAODCnv_Muon_FTF')
      tidaAnalysischains.append('HLT_mu6_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig')
      if doFTK:
        idTrigChainlist.append(['mu6_FTK_idperf',   'L1_MU6', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'],   1])
        idTrigChainlist.append(['mu24_FTK_idperf',  'L1_MU20', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'],   1])
        idTrigChainlist.append(['mu6_FTKRefit_idperf',   'L1_MU6', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'],   1])
        idTrigChainlist.append(['mu24_FTKRefit_idperf',  'L1_MU20', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'],   1])
      tidaAnalysischains.append('HLT_mu24_FTK_idperf:InDetTrigTrackingxAODCnv_Muon_FTK')
      tidaAnalysischains.append('HLT_mu24_FTK_idperf:InDetTrigTrackingxAODCnv_Muon_FTK_IDTrig')
      tidaAnalysischains.append('HLT_mu6_FTK_idperf:InDetTrigTrackingxAODCnv_Muon_FTK')
      tidaAnalysischains.append('HLT_mu6_FTK_idperf:InDetTrigTrackingxAODCnv_Muon_FTK_IDTrig')
      tidaAnalysischains.append('HLT_mu24_FTKRefit_idperf:InDetTrigTrackingxAODCnv_Muon_FTKRefit')
      tidaAnalysischains.append('HLT_mu24_FTKRefit_idperf:InDetTrigTrackingxAODCnv_Muon_FTKRefit_IDTrig')
      tidaAnalysischains.append('HLT_mu6_FTKRefit_idperf:InDetTrigTrackingxAODCnv_Muon_FTKRefit')
      tidaAnalysischains.append('HLT_mu6_FTKRefit_idperf:InDetTrigTrackingxAODCnv_Muon_FTKRefit_IDTrig')
    else:
      idTrigChainlist.append('mu22_IDTrkNoCut_tight_IDT')
      tidaAnalysischains.append('EF_mu22_IDTrkNoCut_tight_IDT:TrigFastTrackFinder_Muon')
      tidaAnalysischains.append('EF_mu22_IDTrkNoCut_tight_IDT:InDetTrigParticleCreation_Muon_EFID')

  return (idTrigChainlist, tidaAnalysischains)

def tauChains(runMergedChain, doIDNewTracking, doFTK):
  idTrigChainlist = []

  tidaAnalysischains = ["Truth"]

  if doIDNewTracking:
    tidaAnalysischains += ["Offline"]

  if not use_new_tm:
    idTrigChainlist += [
      'tau29_IDTrkNoCut'
      ]
    tidaAnalysischains.append("L2_tau29_IDTrkNoCut:TrigL2SiTrackFinder_Tau:0")
    tidaAnalysischains.append("L2_tau29_IDTrkNoCut:TrigL2SiTrackFinder_Tau:1")
    tidaAnalysischains.append("L2_tau29_IDTrkNoCut:TrigL2SiTrackFinder_Tau:2")
    tidaAnalysischains.append("EF_tau29_IDTrkNoCut:InDetTrigParticleCreation_Tau_EFID")
  
  else:
    idTrigChainlist.append(['tau20_r1_idperf',  'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1])
    tidaAnalysischains.append('HLT_tau20_r1_idperf:TrigL2SiTrackFinder_Tau:0')
    tidaAnalysischains.append('HLT_tau20_r1_idperf:TrigL2SiTrackFinder_Tau:1')
    tidaAnalysischains.append('HLT_tau20_r1_idperf:TrigL2SiTrackFinder_Tau:2')
    tidaAnalysischains.append('HLT_tau20_r1_idperf:InDetTrigParticleCreation_Tau_EFID')
    tidaAnalysischains.append('HLT_tau20_r1_idperf:InDetTrigTrackingxAODCnv_Tau_EFID')
  
  
  if runMergedChain==True:
    if use_new_tm:
      idTrigChainlist.append(['tau25_idperf_track', 'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1]) 
      idTrigChainlist.append(['tau25_idperf_tracktwo', 'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1])
      if doFTK:
        idTrigChainlist.append(['tau25_idperf_FTK', 'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1]) 
        idTrigChainlist.append(['tau25_idperf_FTKRefit', 'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1]) 

      tidaAnalysischains.append('HLT_tau25_idperf_track:TrigFastTrackFinder_Tau')
#      tidaAnalysischains.append('HLT_tau25_idperf_track:InDetTrigParticleCreation_Tau_IDTrig')
      tidaAnalysischains.append('HLT_tau25_idperf_track:InDetTrigTrackingxAODCnv_Tau_FTF')
      tidaAnalysischains.append('HLT_tau25_idperf_track:InDetTrigTrackingxAODCnv_Tau_IDTrig')
#      tidaAnalysischains.append('HLT_tau25_idperf_tracktwo:key=TrigFastTrackFinder_TauCore:roi=forID1')
#      tidaAnalysischains.append('HLT_tau25_idperf_tracktwo:key=TrigFastTrackFinder_TauIso:roi=forID3')
      tidaAnalysischains.append('HLT_tau25_idperf_tracktwo:key=InDetTrigTrackingxAODCnv_TauCore_FTF:roi=forID1')
      tidaAnalysischains.append('HLT_tau25_idperf_tracktwo:key=InDetTrigTrackingxAODCnv_TauIso_FTF:roi=forID3')
      tidaAnalysischains.append('HLT_tau25_idperf_tracktwo:key=InDetTrigTrackingxAODCnv_Tau_IDTrig:roi=forID3')
      tidaAnalysischains.append('HLT_tau25_idperf_FTK:InDetTrigTrackingxAODCnv_Tau_FTK')
      tidaAnalysischains.append('HLT_tau25_idperf_FTK:InDetTrigTrackingxAODCnv_Tau_FTK_IDTrig')
      tidaAnalysischains.append('HLT_tau25_idperf_FTKRefit:InDetTrigTrackingxAODCnv_Tau_FTKRefit')
      tidaAnalysischains.append('HLT_tau25_idperf_FTKRefit:InDetTrigTrackingxAODCnv_Tau_FTKRefit_IDTrig')

    else:
      idTrigChainlist.append('tau29_IDTrkNoCut_IDT')
      tidaAnalysischains.append('EF_tau29_IDTrkNoCut_IDT:TrigFastTrackFinder_Tau')
      tidaAnalysischains.append('EF_tau29_IDTrkNoCut_IDT:TrigFastTrackFinder_Tau_IDTrig')
      tidaAnalysischains.append('EF_tau29_IDTrkNoCut_IDT:InDetTrigParticleCreation_Tau_EFID')

  return (idTrigChainlist, tidaAnalysischains)


def bjetChains(runMergedChain, doIDNewTracking, doFTK, doBperf):
  idTrigChainlist = []

  tidaAnalysischains = ["Truth"]

  if doIDNewTracking:
    tidaAnalysischains += ["Offline"]
  
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
    idTrigChainlist.append( ['j55_boffperf',        'L1_J20', [], ['Jet'], ['RATE:SingleBJet', 'BW:BJet'], 1],)
##    idTrigChainlist.append( ['j55_EFID_bperf',   'L1_J20', [], ['Jet'], ['RATE:SingleBJet', 'BW:BJet'], 1],)
    idTrigChainlist.append( ['j55_boffperf_split',  'L1_J20', [], ['Jet'], ['RATE:SingleBJet', 'BW:BJet'], 1],)
    if (doBperf):
      idTrigChainlist.append( ['j55_bperf',        'L1_J20', [], ['Jet'], ['RATE:SingleBJet', 'BW:BJet'], 1],)
      idTrigChainlist.append( ['j55_bperf_split',  'L1_J20', [], ['Jet'], ['RATE:SingleBJet', 'BW:BJet'], 1],)
    if (doFTK):
      idTrigChainlist.append( ['j55_boffperf_split_FTKVtx',  'L1_J20', [], ['Jet'], ['RATE:SingleBJet', 'BW:BJet'], 1],)
      idTrigChainlist.append( ['j55_boffperf_split_FTK',  'L1_J20', [], ['Jet'], ['RATE:SingleBJet', 'BW:BJet'], 1],)
      idTrigChainlist.append( ['j55_boffperf_split_FTKRefit',  'L1_J20', [], ['Jet'], ['RATE:SingleBJet', 'BW:BJet'], 1],)      
      if (doBperf):
        idTrigChainlist.append( ['j55_bperf_split_FTKVtx',  'L1_J20', [], ['Jet'], ['RATE:SingleBJet', 'BW:BJet'], 1],)
        idTrigChainlist.append( ['j55_bperf_split_FTK',  'L1_J20', [], ['Jet'], ['RATE:SingleBJet', 'BW:BJet'], 1],)
        idTrigChainlist.append( ['j55_bperf_split_FTKRefit',  'L1_J20', [], ['Jet'], ['RATE:SingleBJet', 'BW:BJet'], 1],)      
    tidaAnalysischains += [
      'HLT_j55_bperf:InDetTrigTrackingxAODCnv_Bjet_IDTrig',
      'HLT_j55_bperf:key=InDetTrigTrackingxAODCnv_Bjet_FTF:roi=forID',
      'HLT_j55_boffperf:InDetTrigTrackingxAODCnv_Bjet_IDTrig',
      'HLT_j55_boffperf:key=InDetTrigTrackingxAODCnv_Bjet_FTF:roi=forID',
      #
      'HLT_j55_bperf_split:key=InDetTrigTrackingxAODCnv_Bjet_IDTrig:roi=forID',
      'HLT_j55_bperf_split:key=InDetTrigTrackingxAODCnv_Bjet_FTF:roi=forID',
      'HLT_j55_bperf_split:key=TrigFastTrackFinder_Jet:roi=forID',
      'HLT_j55_boffperf_split:key=InDetTrigTrackingxAODCnv_Bjet_IDTrig:roi=forID',
      'HLT_j55_boffperf_split:key=InDetTrigTrackingxAODCnv_Bjet_FTF:roi=forID',
      'HLT_j55_boffperf_split:key=TrigFastTrackFinder_Jet:roi=forID',
      #
      'HLT_j55_bperf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi',
      'HLT_j55_bperf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx',
      'HLT_j55_bperf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=EFHistoVtx',
      'HLT_j55_boffperf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi',
      'HLT_j55_boffperf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx',
      'HLT_j55_boffperf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=EFHistoVtx',
      #
      'HLT_j55_bperf_split_FTKVtx:key=InDetTrigTrackingxAODCnv_Bjet_IDTrig',
      'HLT_j55_bperf_split_FTKVtx:key=InDetTrigTrackingxAODCnv_Bjet_FTF',
      'HLT_j55_bperf_split_FTK:key=InDetTrigTrackingxAODCnv_Bjet_FTK_IDTrig',
      'HLT_j55_bperf_split_FTK:key=InDetTrigTrackingxAODCnv_Bjet_FTK',
      'HLT_j55_bperf_split_FTKRefit:key=InDetTrigTrackingxAODCnv_Bjet_FTKRefit_IDTrig',
      'HLT_j55_bperf_split_FTKRefit:key=InDetTrigTrackingxAODCnv_Bjet_FTKRefit',
      'HLT_j55_boffperf_split_FTKVtx:key=InDetTrigTrackingxAODCnv_Bjet_IDTrig',
      'HLT_j55_boffperf_split_FTKVtx:key=InDetTrigTrackingxAODCnv_Bjet_FTF',
      'HLT_j55_boffperf_split_FTK:key=InDetTrigTrackingxAODCnv_Bjet_FTK_IDTrig',
      'HLT_j55_boffperf_split_FTK:key=InDetTrigTrackingxAODCnv_Bjet_FTK',
      'HLT_j55_boffperf_split_FTKRefit:key=InDetTrigTrackingxAODCnv_Bjet_FTKRefit_IDTrig',
      'HLT_j55_boffperf_split_FTKRefit:key=InDetTrigTrackingxAODCnv_Bjet_FTKRefit',
      ]
  return (idTrigChainlist, tidaAnalysischains)

def beamspotChains(doIDNewTracking, doFTK):
  idTrigChainlist = []
  tidaAnalysischains = ["Truth"]

  if doIDNewTracking:
    tidaAnalysischains += ["Offline"]

##  idTrigChainlist.append(['beamspot_allTE_L2StarB',           'L1_4J15',    [], ['Main'], ['RATE:BeamSpot',  'BW:BeamSpot'], 1])
  idTrigChainlist.append(['beamspot_allTE_trkfast',           'L1_4J15',    [], ['Main'], ['RATE:BeamSpot',  'BW:BeamSpot'], 1])
  if doFTK:
    idTrigChainlist.append(['beamspot_allTE_FTK',           'L1_4J15',    [], ['Main'], ['RATE:BeamSpot',  'BW:BeamSpot'], 1])
    idTrigChainlist.append(['beamspot_allTE_FTKRefit',           'L1_4J15',    [], ['Main'], ['RATE:BeamSpot',  'BW:BeamSpot'], 1])
    idTrigChainlist.append(['beamspot_idperf_FTK',           'L1_4J15',    [], ['Main'], ['RATE:BeamSpot',  'BW:BeamSpot'], 1])
  
  tidaAnalysischains += [
    #'HLT_beamspot_allTE_L2StarB:TrigL2SiTrackFinder_BeamSpotB',
    'HLT_beamspot_allTE_trkfast:TrigFastTrackFinder_BeamSpot_IDTrig',
    'HLT_beamspot_allTE_trkfast:InDetTrigTrackingxAODCnv_BeamSpot_FTF',
    'HLT_beamspot_allTE_FTK:InDetTrigTrackingxAODCnv_BeamSpot_FTK',
    'HLT_beamspot_allTE_FTKRefit:InDetTrigTrackingxAODCnv_BeamSpot_FTKRefit',
    'HLT_beamspot_idperf_FTK:InDetTrigTrackingxAODCnv_BeamSpot_FTKMon',
    'HLT_beamspot_idperf_FTK:InDetTrigTrackingxAODCnv_BeamSpot_FTF',
    ]
  return (idTrigChainlist, tidaAnalysischains)

def minBiasChains(runMergedChain, doIDNewTracking):
  idTrigChainlist = []

  tidaAnalysischains = ["Truth"]

  if doIDNewTracking:
    tidaAnalysischains += ["Offline"]

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


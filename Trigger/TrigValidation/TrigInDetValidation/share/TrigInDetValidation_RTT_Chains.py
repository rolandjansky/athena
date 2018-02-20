def electronChains(doIDNewTracking):

  idTrigChainlist = []
  tidaAnalysischains = ["Truth"]

  if doIDNewTracking:
    tidaAnalysischains += ["Offline"]

  idTrigChainlist.append(  ['e24_medium_idperf', 'L1_EM20VH', [], ['Main'], ['RATE:SingleElectron', 'BW:Egamma'],1])
  idTrigChainlist.append(   ['e5_loose_idperf', 'L1_EM3', [], ['Main'], ['RATE:SingleElectron', 'BW:Egamma'],1])
  tidaAnalysischains.append('HLT_e24_medium_idperf:TrigFastTrackFinder_eGamma')
  tidaAnalysischains.append('HLT_e24_medium_idperf:InDetTrigTrackingxAODCnv_Electron_IDTrig')
  tidaAnalysischains.append('HLT_e24_medium_idperf:InDetTrigTrackingxAODCnv_Electron_FTF')
  tidaAnalysischains.append('HLT_e5_loose_idperf:TrigFastTrackFinder_eGamma')
  tidaAnalysischains.append('HLT_e5_loose_idperf:InDetTrigTrackingxAODCnv_Electron_IDTrig')
  tidaAnalysischains.append('HLT_e5_loose_idperf:InDetTrigTrackingxAODCnv_Electron_FTF')

  return (idTrigChainlist, tidaAnalysischains)

def muonChains(doIDNewTracking, doFTK):

  idTrigChainlist = []
  tidaAnalysischains = ["Truth"]

  if doIDNewTracking:
    tidaAnalysischains += ["Offline"]
  
  idTrigChainlist.append(['mu24_L2Star_idperf', 'L1_MU20', [], ['Main'], ['RATE:SingleMuon','BW:Muon'],   1])
  idTrigChainlist.append(['mu6_L2Star_idperf', 'L1_MU6', [], ['Main'], ['RATE:SingleMuon','BW:Muon'],   1])
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

  idTrigChainlist.append(['mu24_idperf',  'L1_MU20', [], ['Main'], ['RATE:SingleMuon','BW:Muon'],   1])
  idTrigChainlist.append(['mu6_idperf',   'L1_MU6', [], ['Main'], ['RATE:SingleMuon','BW:Muon'],   1])
  tidaAnalysischains.append('HLT_mu24_idperf:InDetTrigTrackingxAODCnv_Muon_FTF')
  tidaAnalysischains.append('HLT_mu24_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig')
  tidaAnalysischains.append('HLT_mu6_idperf:InDetTrigTrackingxAODCnv_Muon_FTF')
  tidaAnalysischains.append('HLT_mu6_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig')

  if doFTK:
    idTrigChainlist.append(['mu6_FTK_idperf',   'L1_MU6', [], ['Main'], ['RATE:SingleMuon','BW:Muon'],   1])
    idTrigChainlist.append(['mu24_FTK_idperf',  'L1_MU20', [], ['Main'], ['RATE:SingleMuon','BW:Muon'],   1])
    idTrigChainlist.append(['mu6_FTKRefit_idperf',   'L1_MU6', [], ['Main'], ['RATE:SingleMuon','BW:Muon'],   1])
    idTrigChainlist.append(['mu24_FTKRefit_idperf',  'L1_MU20', [], ['Main'], ['RATE:SingleMuon','BW:Muon'],   1])

  tidaAnalysischains.append('HLT_mu24_FTK_idperf:InDetTrigTrackingxAODCnv_Muon_FTK')
  tidaAnalysischains.append('HLT_mu24_FTK_idperf:InDetTrigTrackingxAODCnv_Muon_FTK_IDTrig')
  tidaAnalysischains.append('HLT_mu6_FTK_idperf:InDetTrigTrackingxAODCnv_Muon_FTK')
  tidaAnalysischains.append('HLT_mu6_FTK_idperf:InDetTrigTrackingxAODCnv_Muon_FTK_IDTrig')
  tidaAnalysischains.append('HLT_mu24_FTKRefit_idperf:InDetTrigTrackingxAODCnv_Muon_FTKRefit')
  tidaAnalysischains.append('HLT_mu24_FTKRefit_idperf:InDetTrigTrackingxAODCnv_Muon_FTKRefit_IDTrig')
  tidaAnalysischains.append('HLT_mu6_FTKRefit_idperf:InDetTrigTrackingxAODCnv_Muon_FTKRefit')
  tidaAnalysischains.append('HLT_mu6_FTKRefit_idperf:InDetTrigTrackingxAODCnv_Muon_FTKRefit_IDTrig')

  return (idTrigChainlist, tidaAnalysischains)

def tauChains(doIDNewTracking, doFTK):

  idTrigChainlist = []
  tidaAnalysischains = ["Truth"]

  if doIDNewTracking:
    tidaAnalysischains += ["Offline"]

  idTrigChainlist.append(['tau25_idperf_track', 'L1_TAU12', [], ['Main'], ['RATE:SingleTau', 'BW:Tau'], 1]) 
  idTrigChainlist.append(['tau25_idperf_tracktwo', 'L1_TAU12', [], ['Main'], ['RATE:SingleTau', 'BW:Tau'], 1])
  if doFTK:
    idTrigChainlist.append(['tau25_idperf_FTK', 'L1_TAU12', [], ['Main'], ['RATE:SingleTau', 'BW:Tau'], 1]) 
    idTrigChainlist.append(['tau25_idperf_FTKRefit', 'L1_TAU12', [], ['Main'], ['RATE:SingleTau', 'BW:Tau'], 1]) 

  tidaAnalysischains.append('HLT_tau25_idperf_track:TrigFastTrackFinder_Tau')
  tidaAnalysischains.append('HLT_tau25_idperf_track:InDetTrigTrackingxAODCnv_Tau_FTF')
  tidaAnalysischains.append('HLT_tau25_idperf_track:InDetTrigTrackingxAODCnv_Tau_IDTrig')
#  tidaAnalysischains.append('HLT_tau25_idperf_tracktwo:key=TrigFastTrackFinder_TauCore:roi=forID1')
#  tidaAnalysischains.append('HLT_tau25_idperf_tracktwo:key=TrigFastTrackFinder_TauIso:roi=forID3')
  tidaAnalysischains.append('HLT_tau25_idperf_tracktwo:key=InDetTrigTrackingxAODCnv_TauCore_FTF:roi=forID1')
  tidaAnalysischains.append('HLT_tau25_idperf_tracktwo:key=InDetTrigTrackingxAODCnv_TauIso_FTF:roi=forID3')
  tidaAnalysischains.append('HLT_tau25_idperf_tracktwo:key=InDetTrigTrackingxAODCnv_Tau_IDTrig:roi=forID3')
  tidaAnalysischains.append('HLT_tau25_idperf_FTK:InDetTrigTrackingxAODCnv_Tau_FTK')
  tidaAnalysischains.append('HLT_tau25_idperf_FTK:InDetTrigTrackingxAODCnv_Tau_FTK_IDTrig')
  tidaAnalysischains.append('HLT_tau25_idperf_FTKRefit:InDetTrigTrackingxAODCnv_Tau_FTKRefit')
  tidaAnalysischains.append('HLT_tau25_idperf_FTKRefit:InDetTrigTrackingxAODCnv_Tau_FTKRefit_IDTrig')

  return (idTrigChainlist, tidaAnalysischains)


def bjetChains(doIDNewTracking, doFTK, doBperf):
  idTrigChainlist = []

  tidaAnalysischains = ["Truth"]

  if doIDNewTracking:
    tidaAnalysischains += ["Offline"]
  
  idTrigChainlist.append( ['j55_boffperf',        'L1_J20', [], ['Main'], ['RATE:SingleBJet', 'BW:BJet'], 1],)
  idTrigChainlist.append( ['j55_boffperf_split',  'L1_J20', [], ['Main'], ['RATE:SingleBJet', 'BW:BJet'], 1],)

  if (doBperf):
    idTrigChainlist.append( ['j55_bperf',        'L1_J20', [], ['Main'], ['RATE:SingleBJet', 'BW:BJet'], 1],)
    idTrigChainlist.append( ['j55_bperf_split',  'L1_J20', [], ['Main'], ['RATE:SingleBJet', 'BW:BJet'], 1],)

  if (doFTK):
    idTrigChainlist.append( ['j55_boffperf_split_FTKVtx',  'L1_J20', [], ['Main'], ['RATE:SingleBJet', 'BW:BJet'], 1],)
    idTrigChainlist.append( ['j55_boffperf_split_FTK',  'L1_J20', [], ['Main'], ['RATE:SingleBJet', 'BW:BJet'], 1],)
    idTrigChainlist.append( ['j55_boffperf_split_FTKRefit',  'L1_J20', [], ['Main'], ['RATE:SingleBJet', 'BW:BJet'], 1],)      
    if (doBperf):
      idTrigChainlist.append( ['j55_bperf_split_FTKVtx',  'L1_J20', [], ['Main'], ['RATE:SingleBJet', 'BW:BJet'], 1],)
      idTrigChainlist.append( ['j55_bperf_split_FTK',  'L1_J20', [], ['Main'], ['RATE:SingleBJet', 'BW:BJet'], 1],)
      idTrigChainlist.append( ['j55_bperf_split_FTKRefit',  'L1_J20', [], ['Main'], ['RATE:SingleBJet', 'BW:BJet'], 1],)      

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

  idTrigChainlist.append(['beamspot_allTE_trkfast',           'L1_4J15',    [], ['Main'], ['RATE:BeamSpot',  'BW:BeamSpot'], 1])
  if doFTK:
    idTrigChainlist.append(['beamspot_allTE_FTK',           'L1_4J15',    [], ['Main'], ['RATE:BeamSpot',  'BW:BeamSpot'], 1])
    idTrigChainlist.append(['beamspot_allTE_FTKRefit',           'L1_4J15',    [], ['Main'], ['RATE:BeamSpot',  'BW:BeamSpot'], 1])
    idTrigChainlist.append(['beamspot_idperf_FTK',           'L1_4J15',    [], ['Main'], ['RATE:BeamSpot',  'BW:BeamSpot'], 1])
  
  tidaAnalysischains += [
    'HLT_beamspot_allTE_trkfast:TrigFastTrackFinder_BeamSpot_IDTrig',
    'HLT_beamspot_allTE_trkfast:InDetTrigTrackingxAODCnv_BeamSpot_FTF',
    'HLT_beamspot_allTE_FTK:InDetTrigTrackingxAODCnv_BeamSpot_FTK',
    'HLT_beamspot_allTE_FTKRefit:InDetTrigTrackingxAODCnv_BeamSpot_FTKRefit',
    'HLT_beamspot_idperf_FTK:InDetTrigTrackingxAODCnv_BeamSpot_FTKMon',
    'HLT_beamspot_idperf_FTK:InDetTrigTrackingxAODCnv_BeamSpot_FTF',
    ]
  return (idTrigChainlist, tidaAnalysischains)

def minBiasChains(doIDNewTracking):
  idTrigChainlist = []

  tidaAnalysischains = ["Truth"]

  if doIDNewTracking:
    tidaAnalysischains += ["Offline"]

  idTrigChainlist.append(['mb_idperf_L1MBTS_2', 'L1_MBTS_2', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1])
  tidaAnalysischains.append('HLT_mb_idperf_L1MBTS_2:InDetTrigParticleCreation_minBias_EFID')
  tidaAnalysischains.append('HLT_mb_idperf_L1MBTS_2:InDetTrigTrackingxAODCnv_minBias_EFID') 
  
  return (idTrigChainlist, tidaAnalysischains)


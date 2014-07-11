#RTT wrapper around testEFID_basic.py
#     - the name follows a convention for RTT tests
#
#  26/03/2008   Jiri Masik


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

if athenaCommonFlags.FilesInput()==[]:
  athenaCommonFlags.FilesInput=[
    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/vchavda/TrigInDetValidation_bjet5200/mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_tid254598_00/RDO.254598._000016.pool.root.1",
    ]

def resetSigs():
  print 'keep full menu'                                

chainNames = [
#  "Offline",
  "Truth",
#  "Muons",
#  "Electrons",
#  "Taus",
  #egamma
  "L2_e22vh_medium_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:0",
  "L2_e22vh_medium_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:1",
  "L2_e22vh_medium_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:2",
  "L2_e22vh_medium_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:3",
  "EF_e22vh_medium_IDTrkNoCut:InDetTrigParticleCreation_Electron_EFID",
  #  #mu
  "L2_mu22_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:0",
  "L2_mu22_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:1",
  "L2_mu22_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:2",
  "L2_mu22_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:3",
  "EF_mu22_IDTrkNoCut_tight:InDetTrigParticleCreation_Muon_EFID",
  # #tau
  "L2_tau29_IDTrkNoCut:TrigL2SiTrackFinder_Tau:0",
  "L2_tau29_IDTrkNoCut:TrigL2SiTrackFinder_Tau:1",
  "L2_tau29_IDTrkNoCut:TrigL2SiTrackFinder_Tau:2",
  "L2_tau29_IDTrkNoCut:TrigL2SiTrackFinder_Tau:3",
  "EF_tau29_IDTrkNoCut:InDetTrigParticleCreation_Tau_EFID",
  # #bjet
  "L2_b50_NoCut_j50_c4cchad:TrigL2SiTrackFinder_Jet:0",
  "L2_b50_NoCut_j50_c4cchad:TrigL2SiTrackFinder_Jet:1",
  "L2_b50_NoCut_j50_c4cchad:TrigL2SiTrackFinder_Jet:2",
  "L2_b50_NoCut_j50_c4cchad:TrigL2SiTrackFinder_Jet:3",
  "EF_b55_NoCut_j55_a4tchad:InDetTrigParticleCreation_Bjet_EFID"
]

include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")

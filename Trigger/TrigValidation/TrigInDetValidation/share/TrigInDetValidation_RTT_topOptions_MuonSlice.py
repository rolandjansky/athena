#####################################################################################################
#
# top level jobOptions to run Muon chains in the RTT or standalone
# sets some global variables that adjust the execution of TrigInDetValidation_RTT_Common.py
#
# Jiri.Masik@manchester.ac.uk
#
#####################################################################################################

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#set athenaCommonFlags.FilesInput to be able to use this job options standalone without RTT
#secondSet of files can be activated by the if statement below 

if athenaCommonFlags.FilesInput()==[]:
  athenaCommonFlags.FilesInput=[
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000001.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000002.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000003.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000004.pool.root.2",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000005.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000006.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000007.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000008.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000009.pool.root.1",
      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc12_14TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.RDO.e1564_s1499_s1504_r4033_tid01012184_00/RDO.01012184._000010.pool.root.1",
    ]

###XMLDataSet='TrigInDetValidation_mu_single_mu_100' # <-- RTT jobID

from AthenaCommon.AppMgr import release_metadata
d = release_metadata()
##TestMonTool.releaseMetaData = d['nightly name'] + " " + d['nightly release'] + " " + d['date'] + " " + d['platform'] + " " + d['release']
print d['nightly name']
if d['nightly name']=='20.1.X.Y.Z-VAL-TrigMC' or d['nightly name']=='20.X.Y-VAL' or d['nightly name']=='21.X.Y' or d['nightly name']=='20.7.X-VAL' or '20.7.3.Y-VAL' in d['nightly name'] or '20.7.4.Y-VAL' in d['nightly name'] :
  print '***JK This is a realease with FTK, will include chains '
else:
  print '***JK This release does not include FTK, will set doFTK=False'
  doFTK=False

include("TrigInDetValidation/TrigInDetValidation_RTT_Chains.py")

rMC = False
if 'runMergedChain' in dir() and runMergedChain==True:
  rMC = True
rID=False
if 'doIDNewTracking' in dir() and doIDNewTracking==True:
  rID = True
rFTK=False
if 'doFTK' in dir() and doFTK==True:
  from TriggerJobOpts.TriggerFlags import TriggerFlags
  TriggerFlags.doFTK=True
  rFTK=True

(idtrigChainlist, tidaAnalysischains) = muonChains(rMC,rID,rFTK)

def resetSigs():
  TriggerFlags.Slices_all_setOff()
  TriggerFlags.MuonSlice.setAll();
  TriggerFlags.MuonSlice.signatures = idtrigChainlist


PdgId=13


include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")



if 'doFTK' in dir() and doFTK==True:
##  ServiceMgr.TrigFTK_DataProviderSvc.OutputLevel=DEBUG
  ServiceMgr.TrigFTK_DataProviderSvc.TrainingBeamspotX= -0.0497705
  ServiceMgr.TrigFTK_DataProviderSvc.TrainingBeamspotY=1.06299
  ServiceMgr.TrigFTK_DataProviderSvc.TrainingBeamspotZ = 0.0
  ServiceMgr.TrigFTK_DataProviderSvc.TrainingBeamspotTiltX= 0.0 # -1.51489e-05
  ServiceMgr.TrigFTK_DataProviderSvc.TrainingBeamspotTiltY= 0.0 # -4.83891e-05
##  topSequence.TrigSteer_HLT.TrigFastTrackFinder_Muon.OutputLevel=DEBUG
##  topSequence.TrigSteer_HLT.TrigFastTrackFinder_Muon_IDTrig.FTK_Mode=True
##  topSequence.TrigSteer_HLT.TrigFastTrackFinder_Muon_IDTrig.FTK_Refit=False

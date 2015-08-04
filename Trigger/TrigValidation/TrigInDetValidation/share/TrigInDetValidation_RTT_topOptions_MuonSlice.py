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

include("TrigInDetValidation/TrigInDetValidation_RTT_Chains.py")

rMC = False
if 'runMergedChain' in dir() and runMergedChain==True:
  rMC = True
rID=False
if 'doIDNewTracking' in dir() and doIDNewTracking==True:
  rID = True

(idtrigChainlist, tidaAnalysischains) = muonChains(rMC,rID)

def resetSigs():
  TriggerFlags.Slices_all_setOff()
  TriggerFlags.MuonSlice.setAll();
  TriggerFlags.MuonSlice.signatures = idtrigChainlist


PdgId=13

if 'doFTK' in dir() and doFTK==True:
  from TriggerJobOpts.TriggerFlags import TriggerFlags
  TriggerFlags.doFTK=True

include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")

# JK Need to check if FTKDataProviderSvc is available in this release.
import imp
try:
  imp.find_module('TrigFTK_DataProviderSvc')
except:
  print 'FTK_DataProviderSvc not available, will switch to FTF'
  doFTK=False


if 'doFTK' in dir() and doFTK==True:
##  ServiceMgr.TrigFTK_DataProviderSvc.OutputLevel=DEBUG
  ServiceMgr.TrigFTK_DataProviderSvc.TrainingBeamspotX= -0.0497705
  ServiceMgr.TrigFTK_DataProviderSvc.TrainingBeamspotY=1.06299
  ServiceMgr.TrigFTK_DataProviderSvc.TrainingBeamspotZ = 0.0
  ServiceMgr.TrigFTK_DataProviderSvc.TrainingBeamspotTiltX= 0.0 # -1.51489e-05
  ServiceMgr.TrigFTK_DataProviderSvc.TrainingBeamspotTiltY= 0.0 # -4.83891e-05
##  topSequence.TrigSteer_HLT.TrigFastTrackFinder_Muon.OutputLevel=DEBUG
  topSequence.TrigSteer_HLT.TrigFastTrackFinder_Muon.FTK_Mode=True
  topSequence.TrigSteer_HLT.TrigFastTrackFinder_Muon.FTK_Refit=False

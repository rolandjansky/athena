from RecExConfig.RecFlags import rec as rec
from RecExConfig.RecAlgsFlags import recAlgs as recAlgs
from MuonRecExample.MuonRecFlags import muonRecFlags,muonStandaloneFlags
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.AllowIgnoreConfigError = False

# configure all flags to run all Subset + Muon Combined
import MuonCombinedRecExample.MuonCombinedRecOnlySetup
#import MuonRecExample.MuonRecStandaloneOnlySetup

#--------------------------------------------------------------------------------
# Input setup
#--------------------------------------------------------------------------------
### choose one of the pre-defined data files from DataFiles.py
run1_fileListZ = [
"root://eosatlas//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0001.rdo.pool.root",
"root://eosatlas//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0002.rdo.pool.root",
"root://eosatlas//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0003.rdo.pool.root",
"root://eosatlas//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0004.rdo.pool.root",
"root://eosatlas//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0005.rdo.pool.root",
"root://eosatlas//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0006.rdo.pool.root",
"root://eosatlas//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0007.rdo.pool.root",
"root://eosatlas//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0008.rdo.pool.root",
"root://eosatlas//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0009.rdo.pool.root",
"root://eosatlas//eos/atlas/user/j/jomeyer/mc12_8TeV.129681.PowhegPythia8_AU2CT10_Zmumu_DiLeptonFilter/RDO_20131115/Zmumu_devval_20131115.0010.rdo.pool.root"
]

run1_fileListT1 = [
"root://eosatlas//eos/atlas/atlascerngroupdisk/phys-rig/pileupSamples/LS1Samples/valid1.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.recon.RDO.e1565_s1499_s1504_r4242_tid01185596_00/RDO.01185596._000001.pool.root.1",
"root://eosatlas//eos/atlas/atlascerngroupdisk/phys-rig/pileupSamples/LS1Samples/valid1.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.recon.RDO.e1565_s1499_s1504_r4242_tid01185596_00/RDO.01185596._000002.pool.root.1",
"root://eosatlas//eos/atlas/atlascerngroupdisk/phys-rig/pileupSamples/LS1Samples/valid1.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.recon.RDO.e1565_s1499_s1504_r4242_tid01185596_00/RDO.01185596._000003.pool.root.1",
"root://eosatlas//eos/atlas/atlascerngroupdisk/phys-rig/pileupSamples/LS1Samples/valid1.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.recon.RDO.e1565_s1499_s1504_r4242_tid01185596_00/RDO.01185596._000004.pool.root.1"
]

run1_fileListMu10 = [ "root://eosatlas//eos/atlas/user/n/nrbern/mc12_8TeV.107211.ParticleGenerator_mu_Pt10.recon.RDO.e1717/Pt10.rdo.pool.root" ]

run2_fileListDiMu300to1000GeVpileup = [ # SUBSET OF group.det-muon.DiMuon300_1000GeV_task10155951.pileup.HITStoRDO.20161211.v02_EXT0
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/group/det-muon/e7/0c/group.det-muon.10191728.EXT0._000717.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/group/det-muon/cb/63/group.det-muon.10191728.EXT0._001003.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/group/det-muon/b9/57/group.det-muon.10191728.EXT0._001687.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/group/det-muon/1b/03/group.det-muon.10191728.EXT0._001693.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/group/det-muon/af/04/group.det-muon.10191728.EXT0._001706.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/group/det-muon/4c/f4/group.det-muon.10191728.EXT0._001724.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/group/det-muon/76/cd/group.det-muon.10191728.EXT0._001757.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/group/det-muon/50/8f/group.det-muon.10191728.EXT0._002021.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/group/det-muon/37/3c/group.det-muon.10191728.EXT0._002022.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/group/det-muon/f9/6c/group.det-muon.10191728.EXT0._002023.RDO.pool.root"
]

run2_fileListMu3GeV = [ # user.zhidong.SingleMuon_3GeV_task12089572.nopileup.HITStoRDO.r9707.20170907.v01_EXT0
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/c5/37/user.zhidong.12099670.EXT0._000001.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/e5/11/user.zhidong.12099670.EXT0._000002.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/6c/04/user.zhidong.12099670.EXT0._000003.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/0a/d6/user.zhidong.12099670.EXT0._000004.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/7d/10/user.zhidong.12099670.EXT0._000005.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/4d/83/user.zhidong.12099670.EXT0._000006.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/91/c8/user.zhidong.12099670.EXT0._000007.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/38/e3/user.zhidong.12099670.EXT0._000008.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/1c/6b/user.zhidong.12099670.EXT0._000009.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/f5/e7/user.zhidong.12099670.EXT0._000010.RDO.pool.root"
]

run2_fileListMu5GeV = [ # user.zhidong.SingleMuon_5GeV_task12089629.nopileup.HITStoRDO.r9707.20170907.v01_EXT0
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/14/a2/user.zhidong.12100112.EXT0._000001.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/5a/04/user.zhidong.12100112.EXT0._000002.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/11/7b/user.zhidong.12100112.EXT0._000003.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/1b/72/user.zhidong.12100112.EXT0._000004.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/58/07/user.zhidong.12100112.EXT0._000005.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/30/ab/user.zhidong.12100112.EXT0._000006.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/90/7d/user.zhidong.12100112.EXT0._000007.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/2e/89/user.zhidong.12100112.EXT0._000008.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/35/47/user.zhidong.12100112.EXT0._000009.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/c9/85/user.zhidong.12100112.EXT0._000010.RDO.pool.root"
]

run2_fileListMu10GeV = [ # user.zhidong.SingleMuon_10GeV_task12089631.nopileup.HITStoRDO.r9707.20170907.v01_EXT0
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/83/be/user.zhidong.12100138.EXT0._000001.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/d4/9b/user.zhidong.12100138.EXT0._000002.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/7c/00/user.zhidong.12100138.EXT0._000003.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/91/12/user.zhidong.12100138.EXT0._000004.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/15/78/user.zhidong.12100138.EXT0._000005.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/4c/48/user.zhidong.12100138.EXT0._000006.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/81/99/user.zhidong.12100138.EXT0._000007.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/da/80/user.zhidong.12100138.EXT0._000008.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/91/94/user.zhidong.12100138.EXT0._000009.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/f1/68/user.zhidong.12100138.EXT0._000010.RDO.pool.root"
]

run2_fileListMu20GeV = [
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/0b/24/user.zhidong.12100153.EXT0._000001.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/bd/2c/user.zhidong.12100153.EXT0._000002.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/d1/13/user.zhidong.12100153.EXT0._000003.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/24/a0/user.zhidong.12100153.EXT0._000004.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/e8/d1/user.zhidong.12100153.EXT0._000005.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/95/f9/user.zhidong.12100153.EXT0._000006.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/10/05/user.zhidong.12100153.EXT0._000007.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/cf/53/user.zhidong.12100153.EXT0._000008.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/39/53/user.zhidong.12100153.EXT0._000009.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/df/57/user.zhidong.12100153.EXT0._000010.RDO.pool.root"
]

run2_fileListMu50GeV = [
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/7c/00/user.zhidong.12100173.EXT0._000001.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/ff/60/user.zhidong.12100173.EXT0._000002.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/c3/04/user.zhidong.12100173.EXT0._000003.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/cf/32/user.zhidong.12100173.EXT0._000004.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/a0/a3/user.zhidong.12100173.EXT0._000005.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/5b/45/user.zhidong.12100173.EXT0._000006.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/83/7e/user.zhidong.12100173.EXT0._000007.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/9f/1c/user.zhidong.12100173.EXT0._000008.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/6d/7c/user.zhidong.12100173.EXT0._000009.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/45/26/user.zhidong.12100173.EXT0._000010.RDO.pool.root"
]

run2_fileListMu100GeV = [
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/a2/55/user.zhidong.12100201.EXT0._000001.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/3d/a7/user.zhidong.12100201.EXT0._000002.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/56/76/user.zhidong.12100201.EXT0._000003.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/3f/81/user.zhidong.12100201.EXT0._000004.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/bc/d0/user.zhidong.12100201.EXT0._000005.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/13/b5/user.zhidong.12100201.EXT0._000006.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/53/c0/user.zhidong.12100201.EXT0._000007.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/d7/13/user.zhidong.12100201.EXT0._000008.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/e6/19/user.zhidong.12100201.EXT0._000009.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/3a/63/user.zhidong.12100201.EXT0._000010.RDO.pool.root"
]

run2_fileListMu200GeV = [
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/ee/c5/user.zhidong.12100226.EXT0._000001.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/c7/a4/user.zhidong.12100226.EXT0._000002.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/31/92/user.zhidong.12100226.EXT0._000003.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/71/b5/user.zhidong.12100226.EXT0._000004.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/aa/3e/user.zhidong.12100226.EXT0._000005.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/ed/19/user.zhidong.12100226.EXT0._000006.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/5d/d3/user.zhidong.12100226.EXT0._000007.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/aa/e0/user.zhidong.12100226.EXT0._000008.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/20/95/user.zhidong.12100226.EXT0._000009.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/ca/5e/user.zhidong.12100226.EXT0._000010.RDO.pool.root"
]

run2_fileListMu500GeV = [
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/15/8d/user.zhidong.12100254.EXT0._000001.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/7e/88/user.zhidong.12100254.EXT0._000002.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/79/6c/user.zhidong.12100254.EXT0._000003.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/ea/18/user.zhidong.12100254.EXT0._000004.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/7c/5d/user.zhidong.12100254.EXT0._000005.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/8b/5d/user.zhidong.12100254.EXT0._000006.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/ae/1f/user.zhidong.12100254.EXT0._000007.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/4f/55/user.zhidong.12100254.EXT0._000008.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/2b/4d/user.zhidong.12100254.EXT0._000009.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/57/3e/user.zhidong.12100254.EXT0._000010.RDO.pool.root"
]

run2_fileListMu1TeV = [
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/0d/74/user.zhidong.12100281.EXT0._000001.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/7b/18/user.zhidong.12100281.EXT0._000002.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/94/64/user.zhidong.12100281.EXT0._000003.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/c6/57/user.zhidong.12100281.EXT0._000004.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/a3/94/user.zhidong.12100281.EXT0._000005.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/69/8c/user.zhidong.12100281.EXT0._000006.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/42/9d/user.zhidong.12100281.EXT0._000007.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/a5/02/user.zhidong.12100281.EXT0._000008.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/4d/64/user.zhidong.12100281.EXT0._000009.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/c2/de/user.zhidong.12100281.EXT0._000010.RDO.pool.root"
]

run2_fileListMu2TeV = [
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/ad/89/user.zhidong.12100304.EXT0._000001.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/0f/d8/user.zhidong.12100304.EXT0._000002.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/8b/0a/user.zhidong.12100304.EXT0._000003.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/09/aa/user.zhidong.12100304.EXT0._000004.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/4e/71/user.zhidong.12100304.EXT0._000005.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/ee/34/user.zhidong.12100304.EXT0._000006.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/4d/5b/user.zhidong.12100304.EXT0._000007.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/98/00/user.zhidong.12100304.EXT0._000008.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/8b/41/user.zhidong.12100304.EXT0._000009.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/8b/61/user.zhidong.12100304.EXT0._000010.RDO.pool.root"
]

run2_fileListMu3TeV = [
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/fb/a9/user.zhidong.12100448.EXT0._000001.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/dd/e9/user.zhidong.12100448.EXT0._000002.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/a4/32/user.zhidong.12100448.EXT0._000003.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/9d/f6/user.zhidong.12100448.EXT0._000004.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/2a/4f/user.zhidong.12100448.EXT0._000005.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/b7/21/user.zhidong.12100448.EXT0._000006.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/32/40/user.zhidong.12100448.EXT0._000007.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/c8/dd/user.zhidong.12100448.EXT0._000008.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/09/fc/user.zhidong.12100448.EXT0._000009.RDO.pool.root",
"root://eosatlas//eos/atlas/atlasgroupdisk/det-muon/dq2/rucio/user/zhidong/bc/f1/user.zhidong.12100448.EXT0._000010.RDO.pool.root"
]

run2_fileListLate = [
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/c5/56/RDO.11801749._000002.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/48/d7/RDO.11801749._000015.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/e6/33/RDO.11801749._000016.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/08/8a/RDO.11801749._000020.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/5b/a5/RDO.11801749._000021.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/82/3a/RDO.11801749._000022.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/97/3e/RDO.11801749._000023.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/ac/5c/RDO.11801749._000024.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/16/90/RDO.11801749._000025.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/1c/72/RDO.11801749._000026.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/1d/3b/RDO.11801749._000028.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/29/38/RDO.11801749._000029.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/35/19/RDO.11801749._000030.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/b8/13/RDO.11801749._000031.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/be/e0/RDO.11801749._000032.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/a7/37/RDO.11801749._000033.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/37/43/RDO.11801749._000035.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/2d/29/RDO.11801749._000036.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/2d/9e/RDO.11801749._000037.pool.root.1",
"root://eosatlas//eos/atlas/atlasgroupdisk/perf-muons/dq2/rucio/valid1/5f/f3/RDO.11801749._000038.pool.root.1"
]

# old RUN-1 samples
###################
# Zmumu
#athenaCommonFlags.FilesInput = run1_fileListZ
# ttbar
#athenaCommonFlags.FilesInput = run1_fileListT1
# 10GeV muons
#athenaCommonFlags.FilesInput = run1_fileListMu10

# new RUN-2 samples
###################
athenaCommonFlags.FilesInput = run2_fileListDiMu300to1000GeVpileup
#athenaCommonFlags.FilesInput = run2_fileListMu3GeV
#athenaCommonFlags.FilesInput = run2_fileListMu5GeV
#athenaCommonFlags.FilesInput = run2_fileListMu10GeV
#athenaCommonFlags.FilesInput = run2_fileListMu20geV
#athenaCommonFlags.FilesInput = run2_fileListMu50GeV
#athenaCommonFlags.FilesInput = run2_fileListMu100GeV
#athenaCommonFlags.FilesInput = run2_fileListMu200GeV
#athenaCommonFlags.FilesInput = run2_fileListMu500GeV
#athenaCommonFlags.FilesInput = run2_fileListMu1TeV
#athenaCommonFlags.FilesInput = run2_fileListMu2TeV
#athenaCommonFlags.FilesInput = run2_fileListMu3TeV
#athenaCommonFlags.FilesInput = run2_fileListLate

#--------------------------------------------------------------------------------
# Reco setup
#--------------------------------------------------------------------------------
from InDetRecExample.InDetJobProperties import InDetFlags
#InDetFlags.doConversions           = False
#InDetFlags.doVertexFinding         = False
#InDetFlags.doSecVertexFinder       = False
#InDetFlags.doTRTPhaseCalculation   = False
#InDetFlags.doCaloSeededBrem = False

rec.doBTagging=False
rec.doMonitoring=False
rec.doEgamma=False
rec.doJetMissingETTag=False
rec.doPerfMon=False
rec.doLucid=False
rec.doZdc=False
rec.doForwardDet=False
rec.doTrigger = False
#rec.doFloatingPointException.set_Value_and_Lock(True)
#rec.doAOD.set_Value_and_Lock(True)
rec.doWritexAOD.set_Value_and_Lock(True)
rec.doTruth.set_Value_and_Lock(True)
from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.JetTag = False
AODFlags.ParticleJet = False
AODFlags.FastSimulation = False
AODFlags.Streaming = False
AODFlags.MuonTrackSlimmer = False

#muonRecFlags.doNSWNewThirdChain = True
#muonCombinedRecFlags.doCombinedFit = True
muonCombinedRecFlags.doStatisticalCombination = True
muonCombinedRecFlags.doMuGirl = True
muonCombinedRecFlags.doMuonSegmentTagger = True
#muonCombinedRecFlags.doxAOD = True
muonCombinedRecFlags.doAOD = False
muonCombinedRecFlags.doCaloTrkMuId = True
muonCombinedRecFlags.doMuGirlLowBeta = True

muonRecFlags.doCreateClusters = False 
muonRecFlags.prdToxAOD = False

muonRecFlags.doTGCClusterSegmentFinding = False
muonRecFlags.doRPCClusterSegmentFinding = False

muonStandaloneFlags.printSummary = True
muonRecFlags.doTrackPerformance = True
muonRecFlags.TrackPerfSummaryLevel = 2
muonRecFlags.TrackPerfDebugLevel = 5

muonCombinedRecFlags.printSummary = True
muonCombinedRecFlags.doTrackPerformance = True
muonCombinedRecFlags.TrackPerfSummaryLevel = 2
muonCombinedRecFlags.TrackPerfDebugLevel = 5

#--------------------------------------------------------------------------------
# General setup
#--------------------------------------------------------------------------------

#recFlags.doNameAuditor = True
#muonRecFlags.doVP1 = True    # Decide whether to run Virtual Point1 graphical event display
athenaCommonFlags.EvtMax = -1
athenaCommonFlags.SkipEvents = 0
#EventList = "missed.txt"

#--------------------------------------------------------------------------------
# Output setup
#--------------------------------------------------------------------------------
#recFlags.doTruth=False
rec.doWriteESD = True
rec.doWriteAOD = True


##### no more flags after this line #####
try:
    include("MuonRecExample/MuonRec_topOptions.py")
    ###### put any user finetuning after this line #####
#    topSequence.MergeMuonCollections.OutputLevel = DEBUG
#    topSequence.MakeAODMuons.OutputLevel = DEBUG
    if muonCombinedRecFlags.doxAOD():
        #ToolSvc.MuonCreatorTool.OutputLevel = VERBOSE
        #ToolSvc.MuonCombinedFitTagTool.OutputLevel = VERBOSE
        #ToolSvc.MuonSegmentTagTool.OutputLevel = DEBUG
        #ToolSvc.MuonCandidateTool.OutputLevel = VERBOSE
        #ToolSvc.InDetCandidateTool.OutputLevel = VERBOSE
        #ToolSvc.MuonCombinedParticleCreator.OutputLevel = VERBOSE
        #topSequence.ExtrapolatedMuonTracksTruthAlg.OutputLevel = VERBOSE
        #topSequence.CombinedMuonTracksTruthAlg.OutputLevel = VERBOSE
        #ToolSvc += CfgMgr.Trk__TrackParticleCaloExtensionTool("TrackParticleCaloExtensionTool",OutputLevel = VERBOSE)
        #ToolSvc.MuonCombinedTrackFitter.OutputLevel = VERBOSE
        #ToolSvc.OutwardsTrackCleaner.OutputLevel = VERBOSE
        #ToolSvc.MuidTrackCleaner.OutputLevel = VERBOSE
        #ToolSvc += CfgMgr.Rec__MuidCaloMaterialParam("MuidCaloMaterialParam",OutputLevel = VERBOSE)
        #ToolSvc += CfgMgr.Rec__MuidCaloTrackStateOnSurface("MuidCaloTrackStateOnSurface",OutputLevel = VERBOSE)
        #ToolSvc.CombinedMuonTrackBuilder.OutputLevel=VERBOSE
        #ToolSvc.iPatFitter.OutputLevel=DEBUG
        #ToolSvc.TrackDepositInCaloTool.OutputLevel = VERBOSE
        #ToolSvc +=  CfgMgr.xAOD__TrackIsolationTool("TrackIsolationTool",OutputLevel=DEBUG);
        #ToolSvc +=  CfgMgr.xAOD__CaloIsolationTool("CaloIsolationTool",OutputLevel=DEBUG);
        #ToolSvc.MuonCombinedInDetDetailedTrackSelectorTool.OutputLevel = VERBOSE
        pass
    #ToolSvc.MuonTrackSteering.OutputLevel = VERBOSE
    #ToolSvc.MuonTrackSteering.OnlyMdtSeeding = True
    #ToolSvc.MuonPatternSegmentMaker.OutputLevel = VERBOSE
    #ToolSvc.DCMathSegmentMaker.OutputLevel = DEBUG
    #ToolSvc.DCMathSegmentMaker.CurvedErrorScaling = False
    #ToolSvc += CfgMgr.Muon__MuonSegmentInOverlapResolvingTool("MuonSegmentInOverlapResolvingTool",OutputLevel = VERBOSE)
    #ToolSvc.MuonTrackCleaner.OutputLevel = VERBOSE

    #ToolSvc.MuonExtrapolator.OutputLevel = VERBOSE
    #ToolSvc.MCTBExtrapolator.OutputLevel = VERBOSE
    #ToolSvc.MuonNavigator.OutputLevel = VERBOSE
    #ToolSvc.AtlasNavigator.OutputLevel = VERBOSE
    #ToolSvc.AtlasExtrapolator.OutputLevel = VERBOSE
    #ToolSvc.MuSt_MCTBFitter.OutputLevel = VERBOSE
    #ToolSvc.TrackingVolumeArrayCreator.OutputLevel = VERBOSE
    #ToolSvc.TrackingVolumeHelper.OutputLevel = VERBOSE
    #ToolSvc.MuonStationBuilder.OutputLevel = VERBOSE
    #ToolSvc.MuonInertMaterialBuilder.OutputLevel = VERBOSE
    #ToolSvc.MCTBSLFitter.OutputLevel = VERBOSE
    #ToolSvc.MooMuonTrackBuilder.OutputLevel = VERBOSE
    #ToolSvc.MuSt_MCTBSLFitter.OutputLevel = VERBOSE
    #ToolSvc.AtlasSTEP_Propagator.OutputLevel=VERBOSE
    #ToolSvc.MdtMathSegmentFinder.FinderDebugLevel = 10
    #ToolSvc.MuSt_MooTrackFitter.OutputLevel = VERBOSE
    #ToolSvc.MuSt_MooSLTrackFitter.OutputLevel = VERBOSE
    #ToolSvc.MuSt_MooCandidateMatchingTool.OutputLevel = VERBOSE
    #ToolSvc.MooMuonTrackBuilder.OutputLevel = VERBOSE    #topSequence.MuonSegmentCnvAlg.OutputLevel = VERBOSE
    #topSequence.TrackParticleTruthAlg.OutputLevel = VERBOSE
    #from MuonTruthAlgs.MuonTruthAlgsConf import Muon__MuonTruthDecorationAlg
    #from MuonIdHelpers.MuonIdHelpersConf import Muon__MuonIdHelperTool
    #ToolSvc += Muon__MuonIdHelperTool("MuonIdHelperTool",OutputLevel=VERBOSE)
    #ToolSvc.MdtDriftCircleOnTrackCreator.OutputLevel=DEBUG
    #ToolSvc.MuonLayerHoughTool.OutputLevel = VERBOSE
    #ToolSvc.MuonLayerHoughTool.DebugHough = True
    #ToolSvc.MuonLayerHoughTool.DoNtuple = True
    #ToolSvc.MuonLayerHoughTool.UseSeeds = False
    colname = "Muons"
    #from MuonTrackPerformance.MuonTrackPerformanceConf import MuonSegmentPerformanceAlg
    #topSequence += MuonSegmentPerformanceAlg(name="MuonSegmentPerformanceAlg",OutputLevel=VERBOSE)
    #topSequence.MuonSegmentPerformanceAlg.OutputLevel=VERBOSE
    #topSequence.MuonTruthDecorationAlg.OutputLevel=VERBOSE
    if rec.doTruth() and muonRecFlags.doTrackPerformance():
        topSequence.MuonStandalonePerformanceAlg.ProduceEventListMissedTracks = 2 # "0: off, 1: two station, 2: + one station" );
        if( muonCombinedRecFlags.doxAOD() ):
            topSequence.ExtrapolatedMuonPerformanceAlg.ProduceEventListMissedTracks = 2
            topSequence.CombinedMuonPerformanceAlg.ProduceEventListMissedTracks = 2
    
    #include("MuonTestEDM/MuonTestEDM_jobOptions.py")

    if muonCombinedRecFlags.doMuGirlLowBeta():
        ToolSvc.MuonTrackTruthTool.ConsideredPDGs=[13,-13,1000015,-1000015]
        topSequence.ExtrapolatedMuonPerformanceAlg.ConsideredPDGs=[13,-13,1000015,-1000015]
        topSequence.CombinedMuonPerformanceAlg.ConsideredPDGs=[13,-13,1000015,-1000015]
        topSequence.MSOnlyExtrapolatedMuonPerformanceAlg.ConsideredPDGs=[13,-13,1000015,-1000015]
        topSequence.CombinedStauPerformanceAlg.ConsideredPDGs=[13,-13,1000015,-1000015]
        topSequence.ExtrapolatedStauPerformanceAlg.ConsideredPDGs=[13,-13,1000015,-1000015]
        topSequence.MuonPerformanceAlg.ConsideredPDGs=[13,-13,1000015,-1000015]

    doMon = False
    if( doMon ):
        from MuonPhysValMonitoring.MuonPhysValMonitoringConf import MuonPhysValMonitoring__MuonPhysValMonitoringTool
        tool1 = MuonPhysValMonitoring__MuonPhysValMonitoringTool()
        tool1.EnableLumi = False
        tool1.DetailLevel = 1
        tool1.OutputLevel = VERBOSE
        tool1.MuonContainerName = colname
        
        ToolSvc += tool1

        from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
        topSequence += AthenaMonManager( "PhysValMonManager" )

        from AthenaCommon.AppMgr import ServiceMgr
        from GaudiSvc.GaudiSvcConf import THistSvc
        ServiceMgr += THistSvc()
        svcMgr.THistSvc.Output += ["MuonMonExample DATAFILE='MuonMonExample.root' OPT='RECREATE'"]
        
        monMan = topSequence.PhysValMonManager
        monMan.ManualDataTypeSetup = True
        monMan.DataType            = "monteCarlo"
        monMan.Environment         = "altprod"
        monMan.ManualRunLBSetup    = True
        monMan.Run                 = 1
        monMan.LumiBlock           = 1
        
        monMan.FileKey = "MuonMonExample"
        monMan.AthenaMonTools += [ tool1 ]



    ###### put any user finetuning before this line #####
except Exception as occurrederror:
    print 'ERROR:   Exception in try-block of MuonCombinedRec_myTopOptions.py'
    print(occurrederror)
    print 'WARNING: not crashing, but going on and ignoring any further line in the jobOptions after the line causing this exception!'

from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt")
from MuonRecExample.MuonRecUtils import dumpDetFlags
dumpDetFlags("config.txt")

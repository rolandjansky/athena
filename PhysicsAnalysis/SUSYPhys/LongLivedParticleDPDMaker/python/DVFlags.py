# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

class DV_containerFlags(JobProperty):
    statusOn = True
    photonCollectionName='Photons'
    electronCollectionName='Electrons'
    muonCollectionName='Muons'
    jetCollectionName="AntiKt4EMTopoJets"
    METCollectionName="MET_LocHadTopo"
    pass
primRPVLLDESDM.add_JobProperty(DV_containerFlags)

class DV_MultiJetTriggerFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    triggers = ["HLT_4j100","HLT_5j85","HLT_5j85_lcw","HLT_5j75_0eta250","EF_6j70","HLT_6j45_0eta240","HLT_7j45"]
    triggers += ["HLT_4j110","HLT_5j85","HLT_5j65_0eta240_L14J150ETA25","HLT_6j85","HLT_6j55_0eta240_L14J150ETA25"]  #2017
    triggers += ["HLT_4j120","HLT_4j130","HLT_5j60","HLT_5j70","HLT_5j90","HLT_5j100","HLT_6j45","HLT_6j60","HLT_6j70","HLT_7j50"] #L34
    triggers += ["HLT_4j60_gsc100_boffperf_split","HLT_4j60_gsc110_boffperf_split","HLT_4j60_gsc115_boffperf_split",
			"HLT_4j70_gsc120_boffperf_split","HLT_4j70_gsc130_boffperf_split","HLT_5j50_gsc70_boffperf_split",
			"HLT_5j55_gsc75_boffperf_split","HLT_5j60_gsc85_boffperf_split","HLT_5j60_gsc90_boffperf_split",
			"HLT_5j60_gsc100_boffperf_split","HLT_6j45_gsc60_boffperf_split","HLT_6j50_gsc65_boffperf_split", 
			"HLT_6j50_gsc70_boffperf_split","HLT_6j60_gsc85_boffperf_split","HLT_7j25_gsc45_boffperf_split_L14J20",
			"HLT_7j25_gsc50_boffperf_split_L14J20" ]#gsc 
    triggers += ["HLT_2j275_j140","HLT_2j250_j120","HLT_2j220_j120"]#3jet
    pass
primRPVLLDESDM.add_JobProperty(DV_MultiJetTriggerFlags)

### Additional multijet filter for DV+Jets

class DV_2JetFilterFlags_HighpTCut(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    cutEtMin=500.0*Units.GeV
    nPassed=2
    pass
primRPVLLDESDM.add_JobProperty(DV_2JetFilterFlags_HighpTCut)

class DV_3JetFilterFlags_HighpTCut(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    cutEtMin=180.0*Units.GeV
    nPassed=3
    pass
primRPVLLDESDM.add_JobProperty(DV_3JetFilterFlags_HighpTCut)

class DV_4JetFilterFlags_HighpTCut(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    cutEtMin=220.0*Units.GeV
    nPassed=4
    pass
primRPVLLDESDM.add_JobProperty(DV_4JetFilterFlags_HighpTCut)

class DV_5JetFilterFlags_HighpTCut(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    cutEtMin=170.0*Units.GeV
    nPassed=5
    pass
primRPVLLDESDM.add_JobProperty(DV_5JetFilterFlags_HighpTCut)

class DV_6JetFilterFlags_HighpTCut(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    cutEtMin=100.0*Units.GeV
    nPassed=6
    pass
primRPVLLDESDM.add_JobProperty(DV_6JetFilterFlags_HighpTCut)

class DV_7JetFilterFlags_HighpTCut(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    cutEtMin=75.0*Units.GeV
    nPassed=7
    pass
primRPVLLDESDM.add_JobProperty(DV_7JetFilterFlags_HighpTCut)




### multi-jet filter just to verify trigger - 4j80 OR 5j55 OR 6j45

class DV_2JetFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    cutEtMin=220.0*Units.GeV
    nPassed=2
    pass
primRPVLLDESDM.add_JobProperty(DV_2JetFilterFlags)

class DV_3JetFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    cutEtMin=120.0*Units.GeV
    nPassed=3
    pass
primRPVLLDESDM.add_JobProperty(DV_3JetFilterFlags)

class DV_4JetFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    cutEtMin=100.0*Units.GeV
    nPassed=4
    pass
primRPVLLDESDM.add_JobProperty(DV_4JetFilterFlags)

class DV_5JetFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    cutEtMin=75.0*Units.GeV
    nPassed=5
    pass
primRPVLLDESDM.add_JobProperty(DV_5JetFilterFlags)

class DV_6JetFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    cutEtMin=50.0*Units.GeV
    nPassed=6
    pass
primRPVLLDESDM.add_JobProperty(DV_6JetFilterFlags)

class DV_7JetFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    cutEtMin=45.0*Units.GeV
    nPassed=7
    pass
primRPVLLDESDM.add_JobProperty(DV_7JetFilterFlags)

class DV_MuonBarrelFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=["bool"]
    StoredValue=True
    cutEtMin=60.0*Units.GeV
    cutEtaMax=1.1
    triggers=["HLT_mu60_0eta105_msonly"]
    nPassed=1
    pass
primRPVLLDESDM.add_JobProperty(DV_MuonBarrelFilterFlags)

class DV_MuonFullMSFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=["bool"]
    StoredValue=True
    cutEtMin=80.0*Units.GeV
    cutEtaMax=2.5
    triggers=["HLT_mu80_msonly_3layersEC"]
    nPassed=1
    pass
primRPVLLDESDM.add_JobProperty(DV_MuonFullMSFilterFlags)

class DV_PhotonFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue= True
    cutEtMin=150.0*Units.GeV
    cutEtaMax=2.5
    cutIsEM="Loose"
    triggers=["HLT_g140_loose"]
    triggers+=["HLT_g140_tight","HLT_g200_loose"]#2017
    prescale=20
    nPassed=1
    pass
primRPVLLDESDM.add_JobProperty(DV_PhotonFilterFlags)


class DV_METFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=["bool"]
    StoredValue=True
    cutMetMin=180.0*Units.GeV
    deltaPhiCut=0.1
    triggers=["HLT_xe90_L1XE50","HLT_xe100_L1XE50","HLT_xe120_L1XE50",
		          "HLT_xe90_L1XE55","HLT_xe100_L1XE55","HLT_xe120_L1XE55",
		          "HLT_xe90_L1XE60","HLT_xe100_L1XE60","HLT_xe120_L1XE60",
		          "HLT_xe90_tc_lcw_L1XE50","HLT_xe100_tc_lcw_L1XE50","HLT_xe120_tc_lcw_L1XE50",
		          "HLT_xe90_tc_lcw_L1XE55","HLT_xe100_tc_lcw_L1XE55","HLT_xe120_tc_lcw_L1XE55",
		          "HLT_xe90_tc_lcw_L1XE60","HLT_xe100_tc_lcw_L1XE60","HLT_xe120_tc_lcw_L1XE60",
		          "HLT_xe90_mht_L1XE50","HLT_xe100_mht_L1XE50","HLT_xe110_mht_LXE50","HLT_xe120_mht_L1XE50",
		          "HLT_xe90_mht_L1XE55","HLT_xe100_mht_L1XE55","HLT_xe110_mht_LXE55","HLT_xe120_mht_L1XE55",
		          "HLT_xe90_mht_L1XE60","HLT_xe100_mht_L1XE60","HLT_xe110_mht_LXE60","HLT_xe120_mht_L1XE60",
		          "HLT_xe90_topoclPS_L1XE50","HLT_xe100_topoclPS_L1XE50","HLT_xe120_topoclPS_L1XE50",
		          "HLT_xe90_topoclPS_L1XE55","HLT_xe100_topoclPS_L1XE55","HLT_xe120_topoclPS_L1XE55",
		          "HLT_xe90_topoclPS_L1XE60","HLT_xe100_topoclPS_L1XE60","HLT_xe120_topoclPS_L1XE60",
		          "HLT_xe90_topoclPUC_L1XE50","HLT_xe100_topoclPUC_L1XE50","HLT_xe120_topoclPUC_L1XE50",
		          "HLT_xe90_topoclPUC_L1XE55","HLT_xe100_topoclPUC_L1XE55","HLT_xe120_topoclPUC_L1XE55",
	      	    "HLT_xe90_topoclPUC_L1XE60","HLT_xe100_topoclPUC_L1XE60","HLT_xe120_topoclPUC_L1XE60",
              "HLT_xe130_mht_L1XE50","HLT_xe110_mht_L1XE50_AND_xe65_L1XE50"
              "HLT_xe110_mht_L1XE50_AND_xe70_L1XE50","HLT_xe110_mht_L1XE50_AND_xe75_L1XE50","HLT_xe110_mht_L1XE50_AND_xe80_L1XE50",
              "HLT_xe110_mht_L1XE55_AND_xe65_L1XE55","HLT_xe110_mht_L1XE55_AND_xe70_L1XE55","HLT_xe110_mht_L1XE55_AND_xe75_L1XE55",
              "HLT_xe110_mht_L1XE55_AND_xe80_L1XE55"
		         ]
    triggers+=[ "HLT_xe110_pufit_L1XE60", "HLT_xe120_mht_L1XE60_xe80_L1XE60" ]  #2017
    triggers+=[ "HLT_xe120_pufit_L1XE60", "HLT_xe120_mht_xe80_L1XE60" ] #L34
    pass
primRPVLLDESDM.add_JobProperty(DV_METFilterFlags)

class DV_SingleTracklessJetFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=["bool"]
    StoredValue=True
    cutEtMin=70.0*Units.GeV
    cutEtaMax=2.5
    cutSumPtTrkMax=5.0*Units.GeV
    pass
primRPVLLDESDM.add_JobProperty(DV_SingleTracklessJetFilterFlags)

class DV_DoubleTracklessJetFilterFlags(JobProperty):
    statusOn=True
    allowedTypes=["bool"]
    StoredValue=True
    cutEtMin=25.0*Units.GeV
    cutEtaMax=2.5
    cutSumPtTrkMax=5.0*Units.GeV
    pass
primRPVLLDESDM.add_JobProperty(DV_DoubleTracklessJetFilterFlags)

class DV_MeffFilterFlags(JobProperty):
    statusOn=False
    allowedTypes=['bool']
    StoredValue=True
    cutMeffMin=1.0*Units.TeV ##
    cutMEToverMeffMin=0.3   ## 
    cutMEToverMeffMax=0.7   ## 
    cutJetPtMin=40.0*Units.GeV
    cutJetEtaMax=2.5
    cutMETMin=150.0*Units.GeV
    triggers=["HLT_xe90_L1XE50","HLT_xe100_L1XE50","HLT_xe120_L1XE50",
		          "HLT_xe90_L1XE55","HLT_xe100_L1XE55","HLT_xe120_L1XE55",
		          "HLT_xe90_L1XE60","HLT_xe100_L1XE60","HLT_xe120_L1XE60",
		          "HLT_xe90_tc_lcw_L1XE50","HLT_xe100_tc_lcw_L1XE50","HLT_xe120_tc_lcw_L1XE50",
		          "HLT_xe90_tc_lcw_L1XE55","HLT_xe100_tc_lcw_L1XE55","HLT_xe120_tc_lcw_L1XE55",
		          "HLT_xe90_tc_lcw_L1XE60","HLT_xe100_tc_lcw_L1XE60","HLT_xe120_tc_lcw_L1XE60",
		          "HLT_xe90_mht_L1XE50","HLT_xe100_mht_L1XE50","HLT_xe110_mht_L1XE50","HLT_xe120_mht_L1XE50",
		          "HLT_xe90_mht_L1XE55","HLT_xe100_mht_L1XE55","HLT_xe110_mht_L1XE51","HLT_xe120_mht_L1XE55",
		          "HLT_xe90_mht_L1XE60","HLT_xe100_mht_L1XE60","HLT_xe110_mht_L1XE60","HLT_xe120_mht_L1XE60",
		          "HLT_xe90_topoclPS_L1XE50","HLT_xe100_topoclPS_L1XE50","HLT_xe120_topoclPS_L1XE50",
		          "HLT_xe90_topoclPS_L1XE55","HLT_xe100_topoclPS_L1XE55","HLT_xe120_topoclPS_L1XE55",
		          "HLT_xe90_topoclPS_L1XE60","HLT_xe100_topoclPS_L1XE60","HLT_xe120_topoclPS_L1XE60",
		          "HLT_xe90_topoclPUC_L1XE50","HLT_xe100_topoclPUC_L1XE50","HLT_xe120_topoclPUC_L1XE50",
		          "HLT_xe90_topoclPUC_L1XE55","HLT_xe100_topoclPUC_L1XE55","HLT_xe120_topoclPUC_L1XE55",
	      	    "HLT_xe90_topoclPUC_L1XE60","HLT_xe100_topoclPUC_L1XE60","HLT_xe120_topoclPUC_L1XE60"
		         ]
    pass
#primRPVLLDESDM.add_JobProperty(DV_MeffFilterFlags)

class DV_PrescalerFlags(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    prescale=20
    pass
primRPVLLDESDM.add_JobProperty(DV_PrescalerFlags)


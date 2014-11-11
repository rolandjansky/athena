# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def TrigIDtrkMonitoringTool():


	# dataTypes: userDefined = 0, monteCarlo, collisions, cosmics
	if not 'DQMonFlags' in dir():
		from AthenaMonitoring.DQMonFlags import DQMonFlags
	dataType = DQMonFlags.monManDataType()

	from AthenaCommon.AppMgr import ToolSvc

	# Primary Vertex Tool
	from TrigIDtrkMonitoring.TrigIDtrkMonitoringConf import HLTIDpvtxTool
	HLTIDpvtx = HLTIDpvtxTool(name               = 'HLTIDpvtx',
				  histoPathBase      = "/Trigger/HLT");

	#HLTIDpvtx.ChainName                        = "InDetMon_???"
	#HLTIDpvtx.ChainName                        = "L2_b10_IDTrkNoCut"
	#HLTIDpvtx.ChainName                        = "L2_b10_j30_a4tc_EFFS_IDTrkNoCut"
	#HLTIDpvtx.ChainName                        = "L2_b50_NoCut_j50_c4cchad"
	#HLTIDpvtx.ChainName                        = "EF_b10_IDTrkNoCut"
	#HLTIDpvtx.ChainName                        = "EF_b10_j30_a4tc_EFFS_IDTrkNoCut"
	HLTIDpvtx.ChainName                        = "EF_b55_NoCut_j55_a4tchad"
	#HLTIDpvtx.OnlinePrimaryVertexContainerName = "L2BjetFex"
	HLTIDpvtx.OnlinePrimaryVertexContainerName = "PrimVx"
	HLTIDpvtx.OnlineEfficiancyRangeCutX         = 0.01 # cut in mm
	HLTIDpvtx.OnlineEfficiancyRangeCutY         = 0.01
	HLTIDpvtx.OnlineEfficiancyRangeCutZ         = 0.01

	ToolSvc += HLTIDpvtx;
	list = [ "HLTIDpvtxTool/HLTIDpvtx" ];

	# DumpTool
	from TrigIDtrkMonitoring.TrigIDtrkMonitoringConf import HLTIDtrkDumpTool
	HLTIDtrkDump = HLTIDtrkDumpTool(name               = 'HLTIDtrkDump',
					histoPathBase      = "/Trigger/HLT");
	HLTIDtrkDump.UseTrigDecisionTool = True
	HLTIDtrkDump.OfflineCollection   = "TrackParticleCandidate"
	HLTIDtrkDump.OfflineCollectionStaco = "StacoTrackParticles" # JWH
	HLTIDtrkDump.TrackVariables      = ["pT", "eta", "phi", "d0" ,"z0"]      # pT, phi, eta

	if (dataType == 'cosmics'):
		HLTIDtrkDump.MinPt       = 1000.0
		HLTIDtrkDump.MaxAbsEta   = 2.5
		HLTIDtrkDump.MaxAbsD0    = 1000.
		HLTIDtrkDump.MaxAbsZ0    = 3000.
		HLTIDtrkDump.UseSiHits   = True
		HLTIDtrkDump.MinSiHits   = 8
		HLTIDtrkDump.MinTrtHits  = 0
		HLTIDtrkDump.DeltaEta    = 9.99
		HLTIDtrkDump.DeltaEtaTRT = -10
		HLTIDtrkDump.DeltaPhi    = 9.99
		HLTIDtrkDump.CosMon                  = True
		HLTIDtrkDump.CosFS                   = False
		HLTIDtrkDump.CosROIHalfWidth         = 3.2
		HLTIDtrkDump.CosTrigSigTracks        = ["CosNoTDT"]                        # The size of the vector of signatures has to match
		HLTIDtrkDump.CosTrigSigSelect        = ["CosNoTDT"]                        # "selecting signature" vector size
		HLTIDtrkDump.CosIDSCAN               = [True]                              # and the following parameters
		HLTIDtrkDump.CosSiTrack              = [True]
		HLTIDtrkDump.CosTRTSegF              = [True]
		HLTIDtrkDump.CosEF                   = [True]
		HLTIDtrkDump.CosEFtrt                = [True]
		HLTIDtrkDump.CosIDSCANCollection     = ["HLT_TrigIDSCAN_Cosmics"]
		HLTIDtrkDump.CosSiTrackCollection    = ["HLT_TrigSiTrack_Cosmics"]
		HLTIDtrkDump.CosTRTSegFCollection    = ["HLT_TRTSegmentFinder"]
		HLTIDtrkDump.CosEFCollection         = ["HLT_InDetTrigParticleCreationIOTRT_CosmicsN_EFID"]
		HLTIDtrkDump.CosEFtrtCollection      = ["HLT_InDetTrigParticleCreationIOTRT_CosmicsN_EFID"]
	elif (dataType == 'monteCarlo'):
		HLTIDtrkDump.Data         = False
		HLTIDtrkDump.MCCollection = ""
		HLTIDtrkDump.MinPt        = 1000.0
		HLTIDtrkDump.MaxAbsEta    = 2.5
		HLTIDtrkDump.MaxAbsD0     = 2.0
		HLTIDtrkDump.MaxAbsZ0     = 200.
		HLTIDtrkDump.MinPixHits   = 1
		HLTIDtrkDump.MinSctHits   = 5
		HLTIDtrkDump.MinTrtHits   = 15
		HLTIDtrkDump.DeltaEta     = 0.02
		HLTIDtrkDump.DeltaEtaTRT  = -1
		HLTIDtrkDump.DeltaPhi     = 0.01

		HLTIDtrkDump.eGammaMon               = True
		HLTIDtrkDump.eGammaFS                = False
		HLTIDtrkDump.eGammaROIHalfWidth      = 0.1
		HLTIDtrkDump.eGammaTrigSigTracks     = ["e24vh_medium1_IDTrkNoCut"] # The size of the vector of signatures has to match
		HLTIDtrkDump.eGammaTrigSigSelect     = ["e24vh_medium1_IDTrkNoCut"] # "selecting signature" vector size
		HLTIDtrkDump.eGammaTrigSigSelectL2   = ["L2_e24vh_medium1_IDTrkNoCut"] # "selecting signature" vector size
		HLTIDtrkDump.eGammaIDSCAN            = [True]                                    # and the following parameters
		HLTIDtrkDump.eGammaSiTrack           = [True]
		HLTIDtrkDump.eGammaL2Star            = [True]
		HLTIDtrkDump.eGammaTRTSegF           = [True]
		HLTIDtrkDump.eGammaEF                = [True]
		HLTIDtrkDump.eGammaEFtrt             = [True]
		HLTIDtrkDump.eGammaIDSCANCollection  = ["TrigIDSCAN_eGamma"]
		HLTIDtrkDump.eGammaSiTrackCollection = ["TrigSiTrack_eGamma"]
		HLTIDtrkDump.eGammaL2StarCollection  = ["TrigL2SiTrackFinder"]
		HLTIDtrkDump.eGammaTRTSegFCollection = ["TRTSegmentFinder"]
		HLTIDtrkDump.eGammaEFCollection      = ["InDetTrigParticleCreation_Electron_EFID"]
		HLTIDtrkDump.eGammaEFtrtCollection   = ["InDetTrigParticleCreationTRTOnly_Electron_EFID"]

		HLTIDtrkDump.TauMon                  = True
		HLTIDtrkDump.TauFS                   = False
		HLTIDtrkDump.TauROIHalfWidth         = 0.3
		HLTIDtrkDump.TauTrigSigTracks        = ["tau29_IDTrkNoCut","tau125_IDTrkNoCut"]
		HLTIDtrkDump.TauTrigSigSelect        = ["tau29_IDTrkNoCut","tau125_IDTrkNoCut"]
		HLTIDtrkDump.TauTrigSigSelectL2      = ["L2_tau29_IDTrkNoCut","tau125_IDTrkNoCut"]
		HLTIDtrkDump.TauIDSCAN               = [True,True]
		HLTIDtrkDump.TauSiTrack              = [True,True]
		HLTIDtrkDump.TauL2Star               = [True,True]
		HLTIDtrkDump.TauTRTSegF              = [True,True]
		HLTIDtrkDump.TauEF                   = [True,True]
		HLTIDtrkDump.TauEFtrt                = [True,True]
		HLTIDtrkDump.TauIDSCANCollection     = ["TrigIDSCAN_Tau","TrigIDSCAN_Tau"]
		HLTIDtrkDump.TauSiTrackCollection    = ["TrigSiTrack_Tau_robust","TrigSiTrack_Tau_robust"]
		HLTIDtrkDump.TauL2StarCollection     = ["TrigL2SiTrackFinder","TrigL2SiTrackFinder"]
		HLTIDtrkDump.TauTRTSegFCollection    = ["TRTSegmentFinder","TRTSegmentFinder"]
		HLTIDtrkDump.TauEFCollection         = ["InDetTrigParticleCreation_Tau_EFID","InDetTrigParticleCreation_Tau_EFID"]
		HLTIDtrkDump.TauEFtrtCollection      = ["InDetTrigParticleCreationTRTOnly_Tau_EFID","InDetTrigParticleCreationTRTOnly_Tau_EFID"]

		HLTIDtrkDump.MuMon                   = True
		HLTIDtrkDump.MuFS                    = False
		HLTIDtrkDump.MuROIHalfWidth          = 0.1
		HLTIDtrkDump.MuTrigSigTracks         = ["mu18_IDTrkNoCut_tight","mu22_IDTrkNoCut_tight"]
		HLTIDtrkDump.MuTrigSigSelect         = ["mu18_IDTrkNoCut_tight","mu22_IDTrkNoCut_tight"]
		HLTIDtrkDump.MuTrigSigSelectL2       = ["L2_mu18_IDTrkNoCut_tight","mu22_IDTrkNoCut_tight"]
		HLTIDtrkDump.MuIDSCAN                = [True,True]
		HLTIDtrkDump.MuSiTrack               = [True,True]
		HLTIDtrkDump.MuL2Star                = [True,True]
		HLTIDtrkDump.MuTRTSegF               = [True,True]
		HLTIDtrkDump.MuEF                    = [True,True]
		HLTIDtrkDump.MuEFtrt                 = [True,True]
		HLTIDtrkDump.MuIDSCANCollection      = ["TrigIDSCAN_Muon","TrigIDSCAN_Muon"]
		HLTIDtrkDump.MuSiTrackCollection     = ["TrigSiTrack_Muon","TrigSiTrack_Muon"]
		HLTIDtrkDump.MuL2StarCollection      = ["TrigL2SiTrackFinder","TrigL2SiTrackFinder"]
		HLTIDtrkDump.MuTRTSegFCollection     = ["TRTSegmentFinder","TRTSegmentFinder"]
		HLTIDtrkDump.MuEFCollection          = ["InDetTrigParticleCreation_Muon_EFID","InDetTrigParticleCreation_Muon_EFID"]
		HLTIDtrkDump.MuEFtrtCollection       = ["InDetTrigParticleCreationTRTOnly_Muon_EFID","InDetTrigParticleCreationTRTOnly_Muon_EFID"]

		HLTIDtrkDump.FSMon                   = True
		HLTIDtrkDump.FSFS                    = True
		HLTIDtrkDump.FSROIHalfWidth          = 9.99
		HLTIDtrkDump.FSTrigSigTracks         = ["InDetMon_FS"]
		HLTIDtrkDump.FSTrigSigSelect         = ["InDetMon_FS"]
		HLTIDtrkDump.FSTrigSigSelectL2       = ["L2_InDetMon_FS"]
		HLTIDtrkDump.FSIDSCAN                = [True]
		HLTIDtrkDump.FSSiTrack               = [True]
		HLTIDtrkDump.FSL2Star                = [True]
		HLTIDtrkDump.FSTRTSegF               = [False]
		HLTIDtrkDump.FSEF                    = [True]
		HLTIDtrkDump.FSEFtrt                 = [True]
		HLTIDtrkDump.FSIDSCANCollection      = ["TrigIDSCAN_FullScan"]
		HLTIDtrkDump.FSSiTrackCollection     = ["TrigSiTrack_FullScan"]
		HLTIDtrkDump.FSL2StarCollection      = ["TrigL2SiTrackFinder"]
		HLTIDtrkDump.FSTRTSegFCollection     = ["TRTSegmentFinder","TRTSegmentFinder","TRTSegmentFinder","TRTSegmentFinder","TRTSegmentFinder"]
		HLTIDtrkDump.FSEFCollection          = ["InDetTrigParticleCreation_FullScan_EFID"]
		HLTIDtrkDump.FSEFtrtCollection       = ["InDetTrigParticleCreationTRTOnly_FullScan_EFID"]

		HLTIDtrkDump.MBMon                   = True
		HLTIDtrkDump.MBFS                    = True
		HLTIDtrkDump.MBROIHalfWidth          = 9.99
		HLTIDtrkDump.MBTrigSigTracks         = ["mbSpTrk"]
		HLTIDtrkDump.MBTrigSigSelect         = ["mbSpTrk"]
		HLTIDtrkDump.MBTrigSigSelectL2       = ["mbSpTrk"]
		HLTIDtrkDump.MBIDSCAN                = [False]
		HLTIDtrkDump.MBSiTrack               = [False]
		HLTIDtrkDump.MBL2Star                = [False]
		HLTIDtrkDump.MBTRTSegF               = [False]
		HLTIDtrkDump.MBEF                    = [True]
		HLTIDtrkDump.MBEFtrt                 = [False]
		HLTIDtrkDump.MBIDSCANCollection      = ["TrigIDSCAN_minBias"]
		HLTIDtrkDump.MBSiTrackCollection     = ["TrigSiTrack_minBias"]
		HLTIDtrkDump.MBL2StarCollection      = ["TrigL2SiTrackFinder"]
		HLTIDtrkDump.MBTRTSegFCollection     = ["TRTSegmentFinder"]
		HLTIDtrkDump.MBEFCollection          = ["InDetTrigParticleCreation_minBias_EFID"]
		HLTIDtrkDump.MBEFtrtCollection       = ["InDetTrigParticleCreationTRTOnly_minBias_EFID"]

		HLTIDtrkDump.PhysMbMon               = True  # CURRENTLY BEING TESTED...

	else:
		HLTIDtrkDump.MinPt        = 1000.0 #1000.0
		HLTIDtrkDump.MaxAbsEta    = 2.5
		HLTIDtrkDump.MaxAbsD0     = 2.0
		HLTIDtrkDump.MaxAbsZ0     = 200.
		HLTIDtrkDump.MinPixHits   = 1
		HLTIDtrkDump.MinSctHits   = 5
		HLTIDtrkDump.MinTrtHits   = 15
		HLTIDtrkDump.DeltaEta     = 0.02
		HLTIDtrkDump.DeltaEtaTRT  = -1
		HLTIDtrkDump.DeltaPhi     = 0.01

		HLTIDtrkDump.eGammaMon               = True
		HLTIDtrkDump.eGammaFS                = False
		HLTIDtrkDump.eGammaROIHalfWidth      = 0.1
		HLTIDtrkDump.eGammaTrigSigTracks     = ["e24vh_medium1_IDTrkNoCut","e24vh_tight1_e15_NoCut_Zee"] # The size of the vector of signatures has to match
		HLTIDtrkDump.eGammaTrigSigSelect     = ["e24vh_medium1_IDTrkNoCut","e24vh_tight1_e15_NoCut_Zee"] # "selecting signature" vector size
		HLTIDtrkDump.eGammaTrigSigSelectL2   = ["L2_e24vh_medium1_IDTrkNoCut","L2_e24vh_tight1_e15_NoCut_Zee"] # "selecting signature" vector size
		HLTIDtrkDump.eGammaIDSCAN            = [False,False]                                    # and the following parameters
		HLTIDtrkDump.eGammaSiTrack           = [False,False]
		HLTIDtrkDump.eGammaL2Star            = [True,True]
		HLTIDtrkDump.eGammaTRTSegF           = [False,False]
		HLTIDtrkDump.eGammaEF                = [True,True]
		HLTIDtrkDump.eGammaEFtrt             = [False,False]
		HLTIDtrkDump.eGammaIDSCANCollection  = ["TrigIDSCAN_eGamma","TrigIDSCAN_eGamma"]
		HLTIDtrkDump.eGammaSiTrackCollection = ["TrigSiTrack_eGamma","TrigSiTrack_eGamma"]
		HLTIDtrkDump.eGammaL2StarCollection  = ["TrigL2SiTrackFinder_eGamma","TrigL2SiTrackFinder_eGamma"]
		HLTIDtrkDump.eGammaTRTSegFCollection = ["TRTSegmentFinder","TRTSegmentFinder"]
		HLTIDtrkDump.eGammaEFCollection      = ["InDetTrigParticleCreation_Electron_EFID","InDetTrigParticleCreation_Electron_EFID"]
		HLTIDtrkDump.eGammaEFtrtCollection   = ["InDetTrigParticleCreationTRTOnly_Electron_EFID","InDetTrigParticleCreationTRTOnly_Electron_EFID"]
		HLTIDtrkDump.eGammaStaco             = False
		HLTIDtrkDump.eGammaName              = "EGamma"
		HLTIDtrkDump.eGammaRoi               = "initialRoi"
		HLTIDtrkDump.eGammaSuffix            = ""

		HLTIDtrkDump.eGammaCombMon               = True
		HLTIDtrkDump.eGammaCombFS                = False
		HLTIDtrkDump.eGammaCombROIHalfWidth      = 0.1
		HLTIDtrkDump.eGammaCombTrigSigTracks     = ["e24vh_medium1_IDTrkNoCut","e24vh_tight1_e15_NoCut_Zee"] # The size of the vector of signatures has to match
		HLTIDtrkDump.eGammaCombTrigSigSelect     = ["e24vh_medium1_IDTrkNoCut","e24vh_tight1_e15_NoCut_Zee"] # "selecting signature" vector size
		HLTIDtrkDump.eGammaCombTrigSigSelectL2   = ["L2_e24vh_medium1_IDTrkNoCut","L2_e24vh_tight1_e15_NoCut_Zee"] # "selecting signature" vector size
		HLTIDtrkDump.eGammaCombIDSCAN            = [False,False]                                    # and the following parameters
		HLTIDtrkDump.eGammaCombSiTrack           = [False,False]
		HLTIDtrkDump.eGammaCombL2Star            = [True,True]
		HLTIDtrkDump.eGammaCombTRTSegF           = [False,False]
		HLTIDtrkDump.eGammaCombEF                = [True,True]
		HLTIDtrkDump.eGammaCombEFtrt             = [False,False]
		HLTIDtrkDump.eGammaCombIDSCANCollection  = ["TrigIDSCAN_eGamma","TrigIDSCAN_eGamma"]
		HLTIDtrkDump.eGammaCombSiTrackCollection = ["TrigSiTrack_eGamma","TrigSiTrack_eGamma"]
		HLTIDtrkDump.eGammaCombL2StarCollection  = ["TrigL2SiTrackFinder_eGamma","TrigL2SiTrackFinder_eGamma"]
		HLTIDtrkDump.eGammaCombTRTSegFCollection = ["TRTSegmentFinder","TRTSegmentFinder"]
		HLTIDtrkDump.eGammaCombEFCollection      = ["InDetTrigParticleCreation_Electron_EFID","InDetTrigParticleCreation_Electron_EFID"]
		HLTIDtrkDump.eGammaCombEFtrtCollection   = ["InDetTrigParticleCreationTRTOnly_Electron_EFID","InDetTrigParticleCreationTRTOnly_Electron_EFID"]
		HLTIDtrkDump.eGammaCombStaco             = True
		HLTIDtrkDump.eGammaCombName              = "EGammaComb"
		HLTIDtrkDump.eGammaCombRoi               = "initialRoi"
		HLTIDtrkDump.eGammaCombSuffix            = "_comb"

		HLTIDtrkDump.TauMon                  = True
		HLTIDtrkDump.TauFS                   = False
		HLTIDtrkDump.TauROIHalfWidth         = 0.3
		HLTIDtrkDump.TauTrigSigTracks        = ["tau29_IDTrkNoCut","tau125_IDTrkNoCut"]
		HLTIDtrkDump.TauTrigSigSelect        = ["tau29_IDTrkNoCut","tau125_IDTrkNoCut"]
		HLTIDtrkDump.TauTrigSigSelectL2      = ["L2_tau29_IDTrkNoCut","L2_tau125_IDTrkNoCut"]
		HLTIDtrkDump.TauIDSCAN               = [False,False]
		HLTIDtrkDump.TauSiTrack              = [False,False]
		HLTIDtrkDump.TauL2Star               = [True,True]
		HLTIDtrkDump.TauTRTSegF              = [False,False]
		HLTIDtrkDump.TauEF                   = [True,True]
		HLTIDtrkDump.TauEFtrt                = [False,False]
		HLTIDtrkDump.TauIDSCANCollection     = ["TrigIDSCAN_Tau","TrigIDSCAN_Tau"]
		HLTIDtrkDump.TauSiTrackCollection    = ["TrigSiTrack_Tau_robust","TrigSiTrack_Tau_robust"]
		HLTIDtrkDump.TauL2StarCollection     = ["TrigL2SiTrackFinder_Tau","TrigL2SiTrackFinder_Tau"]
		HLTIDtrkDump.TauTRTSegFCollection    = ["TRTSegmentFinder","TRTSegmentFinder"]
		HLTIDtrkDump.TauEFCollection         = ["InDetTrigParticleCreation_Tau_EFID","InDetTrigParticleCreation_Tau_EFID"]
		HLTIDtrkDump.TauEFtrtCollection      = ["InDetTrigParticleCreationTRTOnly_Tau_EFID","InDetTrigParticleCreationTRTOnly_Tau_EFID"]
		HLTIDtrkDump.TauStaco                = False
		HLTIDtrkDump.TauName                 = "Tau"
		HLTIDtrkDump.TauRoi                  = "initialRoi"
		HLTIDtrkDump.TauSuffix               = ""

		HLTIDtrkDump.NewTauMon                  = True
		HLTIDtrkDump.NewTauFS                   = False
		HLTIDtrkDump.NewTauROIHalfWidth         = 0.3
		HLTIDtrkDump.NewTauTrigSigTracks        = ["tau29_IDTrkNoCut","tau125_IDTrkNoCut"]
		HLTIDtrkDump.NewTauTrigSigSelect        = ["tau29_IDTrkNoCut","tau125_IDTrkNoCut"]
		HLTIDtrkDump.NewTauTrigSigSelectL2      = ["L2_tau29_IDTrkNoCut","L2_tau125_IDTrkNoCut"]
		HLTIDtrkDump.NewTauIDSCAN               = [False,False]
		HLTIDtrkDump.NewTauSiTrack              = [False,False]
		HLTIDtrkDump.NewTauL2Star               = [True,True]
		HLTIDtrkDump.NewTauTRTSegF              = [False,False]
		HLTIDtrkDump.NewTauEF                   = [True,True]
		HLTIDtrkDump.NewTauEFtrt                = [False,False]
		HLTIDtrkDump.NewTauIDSCANCollection     = ["TrigIDSCAN_Tau","TrigIDSCAN_Tau"]
		HLTIDtrkDump.NewTauSiTrackCollection    = ["TrigSiTrack_Tau_robust","TrigSiTrack_Tau_robust"]
		HLTIDtrkDump.NewTauL2StarCollection     = ["TrigL2SiTrackFinder_Tau","TrigL2SiTrackFinder_Tau"]
		HLTIDtrkDump.NewTauTRTSegFCollection    = ["TRTSegmentFinder","TRTSegmentFinder"]
		HLTIDtrkDump.NewTauEFCollection         = ["InDetTrigParticleCreation_Tau_EFID","InDetTrigParticleCreation_Tau_EFID"]
		HLTIDtrkDump.NewTauEFtrtCollection      = ["InDetTrigParticleCreationTRTOnly_Tau_EFID","InDetTrigParticleCreationTRTOnly_Tau_EFID"]
		HLTIDtrkDump.NewTauStaco                = True
		HLTIDtrkDump.NewTauName                 = "NewTau"
		HLTIDtrkDump.NewTauRoi                  = "initialRoi"
		HLTIDtrkDump.NewTauSuffix               = "_comb"

		HLTIDtrkDump.MuMon                   = True
		HLTIDtrkDump.MuFS                    = False
		HLTIDtrkDump.MuROIHalfWidth          = 0.1 #0.1
		HLTIDtrkDump.MuTrigSigTracks         = ["mu18_IDTrkNoCut_tight","mu22_IDTrkNoCut_tight","2mu13_Zmumu_IDTrkNoCut","2mu4T_Jpsimumu_IDTrkNoCut"]
		HLTIDtrkDump.MuTrigSigSelect         = ["mu18_IDTrkNoCut_tight","mu22_IDTrkNoCut_tight","2mu13_Zmumu_IDTrkNoCut","2mu4T_Jpsimumu_IDTrkNoCut"]
		HLTIDtrkDump.MuTrigSigSelectL2       = ["L2_mu18_IDTrkNoCut_tight","L2_mu22_IDTrkNoCut_tight","L2_2mu13_Zmumu_IDTrkNoCut","L2_2mu4T_Jpsimumu_IDTrkNoCut"]
		HLTIDtrkDump.MuIDSCAN                = [False,False,False,False]
		HLTIDtrkDump.MuSiTrack               = [False,False,False,False]
		HLTIDtrkDump.MuL2Star                = [True,True,True,True]
		HLTIDtrkDump.MuTRTSegF               = [False,False,False,False]
		HLTIDtrkDump.MuEF                    = [True,True,True,True]
		HLTIDtrkDump.MuEFtrt                 = [False,False,False,False]
		HLTIDtrkDump.MuIDSCANCollection      = ["TrigIDSCAN_Muon","TrigIDSCAN_Muon","TrigIDSCAN_Muon","TrigIDSCAN_Muon"]
		HLTIDtrkDump.MuSiTrackCollection     = ["TrigSiTrack_Muon","TrigSiTrack_Muon","TrigSiTrack_Muon","TrigSiTrack_Muon"]
		HLTIDtrkDump.MuL2StarCollection      = ["TrigL2SiTrackFinder_Muon","TrigL2SiTrackFinder_Muon","TrigL2SiTrackFinder_Muon","TrigL2SiTrackFinder_Muon"]
		HLTIDtrkDump.MuTRTSegFCollection     = ["TRTSegmentFinder","TRTSegmentFinder","TRTSegmentFinder","TRTSegmentFinder"]
		HLTIDtrkDump.MuEFCollection          = ["InDetTrigParticleCreation_Muon_EFID","InDetTrigParticleCreation_Muon_EFID","InDetTrigParticleCreation_Muon_EFID","InDetTrigParticleCreation_Muon_EFID"]
		HLTIDtrkDump.MuEFtrtCollection       = ["InDetTrigParticleCreationTRTOnly_Muon_EFID","InDetTrigParticleCreationTRTOnly_Muon_EFID","InDetTrigParticleCreationTRTOnly_Muon_EFID","InDetTrigParticleCreationTRTOnly_Muon_EFID"]
		HLTIDtrkDump.MuStaco                 = False
		HLTIDtrkDump.MuName                  = "Mu"
		HLTIDtrkDump.MuRoi                   = "forID"
		HLTIDtrkDump.MuSuffix                = ""

		HLTIDtrkDump.MuCombMon               = True
		HLTIDtrkDump.MuCombFS                = False
		HLTIDtrkDump.MuCombROIHalfWidth      = 0.1
		HLTIDtrkDump.MuCombTrigSigTracks     = ["mu18_IDTrkNoCut_tight","mu22_IDTrkNoCut_tight","2mu13_Zmumu_IDTrkNoCut","2mu4T_Jpsimumu_IDTrkNoCut"]
		HLTIDtrkDump.MuCombTrigSigSelect     = ["mu18_IDTrkNoCut_tight","mu22_IDTrkNoCut_tight","2mu13_Zmumu_IDTrkNoCut","2mu4T_Jpsimumu_IDTrkNoCut"]
		HLTIDtrkDump.MuCombTrigSigSelectL2   = ["L2_mu18_IDTrkNoCut_tight","L2_mu22_IDTrkNoCut_tight","L2_2mu13_Zmumu_IDTrkNoCut","L2_2mu4T_Jpsimumu_IDTrkNoCut"]
		HLTIDtrkDump.MuCombIDSCAN            = [False,False,False,False]
		HLTIDtrkDump.MuCombSiTrack           = [False,False,False,False]
		HLTIDtrkDump.MuCombL2Star            = [True,True,True,True]
		HLTIDtrkDump.MuCombTRTSegF           = [False,False,False,False]
		HLTIDtrkDump.MuCombEF                = [True,True,True,True]
		HLTIDtrkDump.MuCombEFtrt             = [False,False,False,False]
		HLTIDtrkDump.MuCombIDSCANCollection  = ["TrigIDSCAN_Muon","TrigIDSCAN_Muon","TrigIDSCAN_Muon","TrigIDSCAN_Muon"]
		HLTIDtrkDump.MuCombSiTrackCollection = ["TrigSiTrack_Muon","TrigSiTrack_Muon","TrigSiTrack_Muon","TrigSiTrack_Muon"]
		HLTIDtrkDump.MuCombL2StarCollection  = ["TrigL2SiTrackFinder_Muon","TrigL2SiTrackFinder_Muon","TrigL2SiTrackFinder_Muon","TrigL2SiTrackFinder_Muon"]
		HLTIDtrkDump.MuCombTRTSegFCollection = ["TRTSegmentFinder","TRTSegmentFinder","TRTSegmentFinder","TRTSegmentFinder"]
		HLTIDtrkDump.MuCombEFCollection      = ["InDetTrigParticleCreation_Muon_EFID","InDetTrigParticleCreation_Muon_EFID","InDetTrigParticleCreation_Muon_EFID","InDetTrigParticleCreation_Muon_EFID"]
		HLTIDtrkDump.MuCombEFtrtCollection   = ["InDetTrigParticleCreationTRTOnly_Muon_EFID","InDetTrigParticleCreationTRTOnly_Muon_EFID","InDetTrigParticleCreationTRTOnly_Muon_EFID","InDetTrigParticleCreationTRTOnly_Muon_EFID"]
		HLTIDtrkDump.MuCombStaco             = True
		HLTIDtrkDump.MuCombName              = "MuComb"
		HLTIDtrkDump.MuRoi                   = "forID"
		HLTIDtrkDump.MuSuffix                = "_comb"

		HLTIDtrkDump.BMon                   = True
		HLTIDtrkDump.BFS                    = False
		HLTIDtrkDump.BROIHalfWidth          = 0.1
		HLTIDtrkDump.BTrigSigTracks         = ["b55_NoCut_j55_a4tchad"]
		HLTIDtrkDump.BTrigSigSelect         = ["b55_NoCut_j55_a4tchad"]
		HLTIDtrkDump.BTrigSigSelectL2       = ["L2_b50_NoCut_j50_c4cchad"]
		HLTIDtrkDump.BIDSCAN                = [False]
		HLTIDtrkDump.BSiTrack               = [False]
		HLTIDtrkDump.BL2Star                = [True]
		HLTIDtrkDump.BTRTSegF               = [False]
		HLTIDtrkDump.BEF                    = [True]
		HLTIDtrkDump.BEFtrt                 = [False]
		HLTIDtrkDump.BIDSCANCollection      = ["TrigIDSCAN_Jet"]
		HLTIDtrkDump.BSiTrackCollection     = ["TrigSiTrack_Jet"]
		HLTIDtrkDump.BL2StarCollection      = ["TrigL2SiTrackFinder_Jet"]
		HLTIDtrkDump.BTRTSegFCollection     = ["TRTSegmentFinder"]
		HLTIDtrkDump.BEFCollection          = ["InDetTrigParticleCreation_Bjet_EFID"]
		HLTIDtrkDump.BEFtrtCollection       = ["InDetTrigParticleCreationTRTOnly_Muon_EFID"]
		HLTIDtrkDump.BName                  = "B"
		HLTIDtrkDump.BStaco                 = True
		HLTIDtrkDump.BRoi                   = "initialRoi"
		HLTIDtrkDump.BSuffix                = ""

		HLTIDtrkDump.FSMon                   = True
		HLTIDtrkDump.FSFS                    = True
		HLTIDtrkDump.FSROIHalfWidth          = 9.99
		HLTIDtrkDump.FSTrigSigTracks         = ["InDetMon_FS"]
		HLTIDtrkDump.FSTrigSigSelect         = ["InDetMon_FS"]
		HLTIDtrkDump.FSTrigSigSelectL2       = ["L2_InDetMon_FS"]
		HLTIDtrkDump.FSIDSCAN                = [True]
		HLTIDtrkDump.FSSiTrack               = [True]
		HLTIDtrkDump.FSL2Star                = [True]
		HLTIDtrkDump.FSTRTSegF               = [False]
		HLTIDtrkDump.FSEF                    = [True]
		HLTIDtrkDump.FSEFtrt                 = [True]
		HLTIDtrkDump.FSIDSCANCollection      = ["TrigIDSCAN_FullScan"]
		HLTIDtrkDump.FSSiTrackCollection     = ["TrigSiTrack_FullScan"]
		HLTIDtrkDump.FSL2StarCollection      = ["TrigL2SiTrackFinder_FullScan"]
		HLTIDtrkDump.FSTRTSegFCollection     = ["TRTSegmentFinder"]
		HLTIDtrkDump.FSEFCollection          = ["InDetTrigParticleCreation_FullScanHI_EFID"]
		HLTIDtrkDump.FSEFtrtCollection       = ["InDetTrigParticleCreationTRTOnly_FullScan_EFID"]
		HLTIDtrkDump.FSStaco                 = False
		HLTIDtrkDump.FSName                  = "FS"
		HLTIDtrkDump.FSRoi                   = "initialRoi"
		HLTIDtrkDump.FSSuffix                = ""

		HLTIDtrkDump.MBMon                   = True
		HLTIDtrkDump.MBFS                    = True
		HLTIDtrkDump.MBROIHalfWidth          = 9.99
		HLTIDtrkDump.MBTrigSigTracks         = ["mbSpTrk"]
		HLTIDtrkDump.MBTrigSigSelect         = ["mbSpTrk"]
		HLTIDtrkDump.MBTrigSigSelectL2       = ["L2_mbSpTrk"]
		HLTIDtrkDump.MBIDSCAN                = [False]
		HLTIDtrkDump.MBSiTrack               = [False]
		HLTIDtrkDump.MBL2Star                = [False]
		HLTIDtrkDump.MBTRTSegF               = [False]
		HLTIDtrkDump.MBEF                    = [True]
		HLTIDtrkDump.MBEFtrt                 = [False]
		HLTIDtrkDump.MBIDSCANCollection      = ["TrigIDSCAN_minBias"]
		HLTIDtrkDump.MBSiTrackCollection     = ["TrigSiTrack_minBias"]
		HLTIDtrkDump.MBL2StarCollection      = ["TrigL2SiTrackFinder_minBias"]
		HLTIDtrkDump.MBTRTSegFCollection     = ["TRTSegmentFinder"]
		HLTIDtrkDump.MBEFCollection          = ["InDetTrigParticleCreation_minBias_EFID"]
		HLTIDtrkDump.MBEFtrtCollection       = ["InDetTrigParticleCreationTRTOnly_minBias_EFID"]
		HLTIDtrkDump.MBStaco                 = False
		HLTIDtrkDump.MBName                  = "MB"
		HLTIDtrkDump.MBRoi                   = "initialRoi"
		HLTIDtrkDump.MBSuffix                = ""

		HLTIDtrkDump.PhysMbMon               = True  # CURRENTLY BEING TESTED...

	ToolSvc += HLTIDtrkDump;
	list += [ "HLTIDtrkDumpTool/HLTIDtrkDump" ];

	if not 'rec' in dir():
		from RecExConfig.RecFlags  import rec

	if rec.doInDet:
		from TrigIDtrkMonitoring.TrigIDtrkMonitoringConf import TIDAMonTool
		tidatool = TIDAMonTool(name = "tidatool",
				       histoPathBase = "/Trigger/HLT")
		tidatool.AnalysisConfig = "Tier0"
		tidatool.buildNtuple = True
		tidatool.mcTruth = False
		tidatool.ntupleChainNames += [
			#egamma
			"EF_e24vh_medium1_IDTrkNoCut:InDetTrigParticleCreation_Electron_EFID",
			"L2_e24vh_medium1_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:0",
			"L2_e24vh_medium1_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:1",
			"L2_e24vh_medium1_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:2",
			#mu
			"EF_mu18_IDTrkNoCut_tight:InDetTrigParticleCreation_Muon_EFID",
			"L2_mu18_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:0",
			"L2_mu18_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:1",
			"L2_mu18_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:2",
			"EF_mu22_IDTrkNoCut_tight:InDetTrigParticleCreation_Muon_EFID",
			"L2_mu22_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:0",
			"L2_mu22_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:1",
			"L2_mu22_IDTrkNoCut_tight:TrigL2SiTrackFinder_Muon:2",
			#tau
			"L2_tau29_IDTrkNoCut:TrigL2SiTrackFinder_Tau:0",
			"L2_tau29_IDTrkNoCut:TrigL2SiTrackFinder_Tau:1",
			"L2_tau29_IDTrkNoCut:TrigL2SiTrackFinder_Tau:2",
			"L2_tau125_IDTrkNoCut:TrigL2SiTrackFinder_Tau:0",
			"L2_tau125_IDTrkNoCut:TrigL2SiTrackFinder_Tau:1",
			"L2_tau125_IDTrkNoCut:TrigL2SiTrackFinder_Tau:2",
			"EF_tau29_IDTrkNoCut:InDetTrigParticleCreation_Tau_EFID",
			"EF_tau125_IDTrkNoCut:InDetTrigParticleCreation_Tau_EFID",
			#bjet
			"EF_b55_NoCut_j55_a4tchad:InDetTrigParticleCreation_Bjet_EFID",
			"L2_b50_NoCut_j50_c4cchad:TrigL2SiTrackFinder_Jet",
			#Full Scan
			"L2_InDetMon_FS:TrigIDSCAN_FullScan",
			"L2_InDetMon_FS:TrigSiTrack_FullScan",
			"L2_InDetMon_FS:TrigL2SiTrackFinder_FullScan:0",
			"L2_InDetMon_FS:TrigL2SiTrackFinder_FullScan:1",
			"L2_InDetMon_FS:TrigL2SiTrackFinder_FullScan:2",
			"EF_InDetMon_FS:InDetTrigParticleCreation_FullScan_EFID",
                        #Cosmics
			"HLT_id_cosmic_L1MU4:InDetTrigTrackingxAODCnv_CosmicsN_EFID",
			"HLT_id_cosmic:InDetTrigTrackingxAODCnv_CosmicsN_EFID",
		        #HI running
			"L2_mu4_IDTrkNoCut:TrigL2SiTrackFinder_Muon:0",
			"L2_mu4_IDTrkNoCut:TrigL2SiTrackFinder_Muon:1",
			"L2_mu4_IDTrkNoCut:TrigL2SiTrackFinder_Muon:2",
			"EF_mu4_IDTrkNoCut:InDetTrigParticleCreation_Muon_EFID",

			"L2_mu8_IDTrkNoCut:TrigL2SiTrackFinder_Muon:0",
			"L2_mu8_IDTrkNoCut:TrigL2SiTrackFinder_Muon:1",
			"L2_mu8_IDTrkNoCut:TrigL2SiTrackFinder_Muon:2",
			"EF_mu8_IDTrkNoCut:InDetTrigParticleCreation_Muon_EFID",

			"L2_mu10_IDTrkNoCut:TrigL2SiTrackFinder_Muon:0",
			"L2_mu10_IDTrkNoCut:TrigL2SiTrackFinder_Muon:1",
			"L2_mu10_IDTrkNoCut:TrigL2SiTrackFinder_Muon:2",
			"EF_mu10_IDTrkNoCut:InDetTrigParticleCreation_Muon_EFID",

			"L2_e15_loose1_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:0",
			"L2_e15_loose1_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:1",
			"L2_e15_loose1_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:2",
			"EF_e15_loose1_IDTrkNoCut:InDetTrigParticleCreation_Electron_EFID",

			"L2_e5_medium1_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:0",
			"L2_e5_medium1_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:1",
			"L2_e5_medium1_IDTrkNoCut:TrigL2SiTrackFinder_eGamma:2",
			"EF_e5_medium1_IDTrkNoCut:InDetTrigParticleCreation_Electron_EFID",


			]
		ToolSvc += tidatool;
		list += [ "TIDAMonTool/tidatool" ]

        ### Tag and Probe ###

	if not 'rec' in dir():
		from RecExConfig.RecFlags  import rec

	if not 'DQMonFlags' in dir():
		from AthenaMonitoring.DQMonFlags import DQMonFlags
		dataType = DQMonFlags.monManDataType()

	if dataType != 'cosmics' and rec.doCalo:
		# Tag and Probe
		from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
		theAtlasExtrapolator=AtlasExtrapolator(name =  "MyAtlasExtrapolator")
		theAtlasExtrapolator.DoCaloDynamic = False # this turns off dynamic
		ToolSvc += AtlasExtrapolator('MyAtlasExtrapolator')

		# from TrackToCalo.TrackToCaloConf import ExtrapolateToCaloTool
		# theExtrapolateToCaloTool=ExtrapolateToCaloTool(name =	 "MyCaloExtrapolatorTool", Extrapolator = theAtlasExtrapolator)
		# ToolSvc += ExtrapolateToCaloTool('MyCaloExtrapolatorTool')

		# from TrigIDtrkMonitoring.TrigIDtrkMonitoringConf import HLTIDZeeTagProbe
		# from AthenaCommon.AppMgr import ToolSvc
		# HLTIDZeeTag = HLTIDZeeTagProbe(name               =    'HLTIDZeeTag', histoPathBase      =  "/Trigger/HLT", ExtrapolateToCaloTool = theExtrapolateToCaloTool);


		#from TrigIDtrkMonitoring.TrigIDtrkMonitoringConf import HLTIDZeeTagProbe
		#from AthenaCommon.AppMgr import ToolSvc
		#HLTIDZeeTag = HLTIDZeeTagProbe(name               = 'HLTIDZeeTag',
		#			       histoPathBase      = "/Trigger/HLT");

#		HLTIDZeeTag.TrigChainName = "e20_tight_e15_NoCut_Zee"

		# ToolSvc += HLTIDZeeTag;
		# list += ["HLTIDZeeTagProbe/HLTIDZeeTag"];

	return list

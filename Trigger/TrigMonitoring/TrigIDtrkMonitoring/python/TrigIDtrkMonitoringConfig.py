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


	if not 'rec' in dir():
		from RecExConfig.RecFlags  import rec

	if rec.doInDet:
		# the old DumpTool has been removed, the old TIDAMonTool code has
		# been moved to TrigInDetAnalysisExample/TrigTestBase
		# from TrigIDtrkMonitoring.TrigIDtrkMonitoringConf import TIDAMonTool

		from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigTestBase
		tidacosmic = TrigTestBase(name = "TIDACosmic",
					histoPathBase = "/Trigger/HLT")
		tidacosmic.AnalysisConfig = "Tier0"
		tidacosmic.SliceTag = "Trigger/HLTCosmic"
		tidacosmic.ntupleChainNames += [
			"Offline",
			"HLT_id_cosmic.*:InDetTrigTrackingxAODCnv_CosmicsN_EFID",
			"HLT_id_cosmic.*:InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID",
			]
		ToolSvc += tidacosmic;
		list += [ "TrigTestBase/TIDACosmic" ]



		tidatool = TrigTestBase(name = "TIDATool",
					histoPathBase = "/Trigger/HLT")
		tidatool.AnalysisConfig = "Tier0"
		tidatool.SliceTag = "Trigger/HLT"
		tidatool.ntupleChainNames += [
			"Offline",
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
			"HLT_id_cosmic.*:InDetTrigTrackingxAODCnv_CosmicsN_EFID",
			"HLT_id_cosmic.*:InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID",
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
		list += [ "TrigTestBase/TIDATool" ]

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

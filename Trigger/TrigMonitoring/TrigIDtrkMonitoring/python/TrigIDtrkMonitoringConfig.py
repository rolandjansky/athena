# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def TrigIDtrkMonitoringTool():
	from AthenaCommon.Constants import INFO,ERROR,FALSE,TRUE,DEBUG,VERBOSE

	# dataTypes: userDefined = 0, monteCarlo, collisions, cosmics
	if not 'DQMonFlags' in dir():
		from AthenaMonitoring.DQMonFlags import DQMonFlags
	dataType = DQMonFlags.monManDataType()

	from AthenaCommon.AppMgr import ToolSvc

	list = []

	if not 'rec' in dir():
		from RecExConfig.RecFlags  import rec


#	if rec.doInDet:
	if True:
		# the old DumpTool has been removed, the old TIDAMonTool code has

		from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigTestBase


		##############################################################
		# Cosmic instances
		##############################################################

		
		# Cosmic Expert instance
		
		tidacos = TrigTestBase(name = "IDCosmicTool",
					histoPathBase = "/Trigger/HLT")
		tidacos.AnalysisConfig = "Tier0"
		tidacos.SliceTag = "HLT/TRIDT/Cosmic/Expert"
		# tidacos.OutputLevel = DEBUG
		tidacos.pixHitsOffline=-1
		tidacos.sctHitsOffline=-1
		tidacos.siHitsOffline=-1
		tidacos.ntupleChainNames += [
			"Offline",
			"HLT_id_cosmic.*:InDetTrigTrackingxAODCnv_CosmicsN_EFID",
			"HLT_id_cosmic.*:InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID"
			]
		ToolSvc += tidacos;
		list += [ "TrigTestBase/IDCosmicTool" ]


		# Cosmic Shifter instance

		tidacosshift = TrigTestBase(name = "IDCosmicShifterTool",
				       histoPathBase = "/Trigger/HLT")
		tidacosshift.AnalysisConfig = "Tier0"
		tidacosshift.SliceTag = "HLT/TRIDT/Cosmic/Shifter"
		# tidacos.OutputLevel = DEBUG
		tidacosshift.pixHitsOffline=-1
		tidacosshift.sctHitsOffline=-1
		tidacosshift.siHitsOffline=-1
		tidacosshift.ntupleChainNames += [
			"Offline",
			"HLT_id_cosmic.*:InDetTrigTrackingxAODCnv_CosmicsN_EFID",
			"HLT_id_cosmic.*:InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID"
			]
		ToolSvc += tidacosshift;
		list += [ "TrigTestBase/IDCosmicShifterTool" ]



		##############################################################
		# Egamma instances
		##############################################################

		# Expert instances 
		tidaegamma = TrigTestBase(name = "IDEgammaTool",
					  histoPathBase = "/Trigger/HLT")
		tidaegamma.AnalysisConfig = "Tier0"
		tidaegamma.SliceTag = "HLT/TRIDT/Egamma/Expert"
		# tidabase.OutputLevel = DEBUG
		tidaegamma.UseHighestPT = True
		tidaegamma.ntupleChainNames += [
			"Offline",
			"HLT_e.*idperf.*:InDetTrigTrackingxAODCnv_Electron_EFID",
			"HLT_e.*idperf.*:InDetTrigTrackingxAODCnv_Electron_IDTrig",
			"HLT_e.*idperf.*:InDetTrigTrackingxAODCnv_Electron_FTF"
			]
		ToolSvc += tidaegamma;
		list += [ "TrigTestBase/IDEgammaTool" ]



		# Shifter instances 
		tidaegammashift = TrigTestBase(name = "IDEgammaShifterTool",
					       histoPathBase = "/Trigger/HLT")
		tidaegammashift.AnalysisConfig = "Tier0"
		tidaegammashift.SliceTag = "HLT/TRIDT/Egamma/Shifter"
		tidaegammashift.UseHighestPT = True
		# tidabase.OutputLevel = DEBUG
		tidaegammashift.ntupleChainNames += [
			"Offline",
			"HLT_e.*idperf.*:InDetTrigTrackingxAODCnv_Electron_IDTrig",
			"HLT_e.*idperf.*:InDetTrigTrackingxAODCnv_Electron_FTF"
			]
		ToolSvc += tidaegammashift;
		list += [ "TrigTestBase/IDEgammaShifterTool" ]
		



		##############################################################
		# Muon instances
		##############################################################
		
		# Expert instances 
		tidamuon = TrigTestBase(name = "IDMuonTool",
					histoPathBase = "/Trigger/HLT")
		tidamuon.AnalysisConfig = "Tier0"
		tidamuon.SliceTag = "HLT/TRIDT/Muon/Expert"
		tidamuon.UseHighestPT = True
		# tidabase.OutputLevel = DEBUG
		tidamuon.ntupleChainNames += [
			"Offline",
			"HLT_mu.*idperf.*:InDetTrigTrackingxAODCnv_Muon_EFID",
			"HLT_mu.*idperf.*:InDetTrigTrackingxAODCnv_Muon_IDTrig",
			"HLT_mu.*idperf.*:InDetTrigTrackingxAODCnv_Muon_FTF"
			]
		ToolSvc += tidamuon;
		list += [ "TrigTestBase/IDMuonTool" ]


		# Shifter instances 
		tidamuonshift = TrigTestBase(name = "IDMuonShifterTool",
					histoPathBase = "/Trigger/HLT")
		tidamuonshift.AnalysisConfig = "Tier0"
		tidamuonshift.SliceTag = "HLT/TRIDT/Muon/Shifter"
		tidamuonshift.UseHighestPT = True
		tidamuonshift.ntupleChainNames += [
			"Offline",
			"HLT_mu.*idperf.*:InDetTrigTrackingxAODCnv_Muon_IDTrig",
			"HLT_mu.*idperf.*:InDetTrigTrackingxAODCnv_Muon_FTF"
			]
		ToolSvc += tidamuonshift;
		list += [ "TrigTestBase/IDMuonShifterTool" ]




		##############################################################
		# Tau instances
		##############################################################

		# Expert instances 
		tidatau = TrigTestBase(name = "IDTauTool",
				       histoPathBase = "/Trigger/HLT")
		tidatau.AnalysisConfig = "Tier0"
		tidatau.SliceTag = "HLT/TRIDT/Tau/Expert"
		tidatau.UseHighestPT = False
		tidatau.ntupleChainNames += [
			"Offline",
			"HLT_tau.*idperf.*:InDetTrigTrackingxAODCnv_Tau_EFID",
			"HLT_tau.*idperf.*:key=InDetTrigTrackingxAODCnv_Tau_IDTrig",
			"HLT_tau.*idperf.*:key=InDetTrigTrackingxAODCnv_Tau_IDTrig:roi=forID3",
			"HLT_tau.*idperf.*:key=InDetTrigTrackingxAODCnv_Tau_FTF:roi=forID",
			"HLT_tau.*idperf.*:key=InDetTrigTrackingxAODCnv_TauCore_FTF:roi=forID1",
			"HLT_tau.*idperf.*:key=InDetTrigTrackingxAODCnv_TauIso_FTF:roi=forID3"
			]
		ToolSvc += tidatau;
		list += [ "TrigTestBase/IDTauTool" ]

		
		# Expert instances 
		tidataushift = TrigTestBase(name = "IDTauShifterTool",
					    histoPathBase = "/Trigger/HLT")
		tidataushift.AnalysisConfig = "Tier0"
		tidataushift.SliceTag = "HLT/TRIDT/Tau/Shifter"
		tidataushift.UseHighestPT = False
		tidataushift.ntupleChainNames += [
			"Offline",
			"HLT_tau.*idperf.*_track.*:key=InDetTrigTrackingxAODCnv_Tau_IDTrig:roi=forID3",
			"HLT_tau.*idperf.*_track.*:key=InDetTrigTrackingxAODCnv_TauIso_FTF:roi=forID3"
			]
		ToolSvc += tidataushift;
		list += [ "TrigTestBase/IDTauShifterTool" ]
		



		
		##############################################################
                # Bjet instances - check track collection names
		##############################################################

                # Expert instances
		tidabjet = TrigTestBase(name = "IDBjetTool",
					histoPathBase = "/Trigger/HLT")
		tidabjet.AnalysisConfig = "Tier0"
		tidabjet.SliceTag = "HLT/TRIDT/Bjet/Expert"
		tidabjet.ntupleChainNames += [
			"Offline",
			# jet based chains
			"HLT_j.*bperf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi",
			"HLT_j.*bperf_split:key=InDetTrigTrackingxAODCnv_Bjet_IDTrig",
			"HLT_j.*bperf_split:key=InDetTrigTrackingxAODCnv_Bjet_FTF",
			"HLT_j.*bperf:key=InDetTrigTrackingxAODCnv_Bjet_IDTrig",
			"HLT_j.*bperf:key=InDetTrigTrackingxAODCnv_Bjet_FTF",
			# muon based chains
			# "HLT_mu.*bperf_dr05:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi",
			# "HLT_mu.*bperf_dr05:key=InDetTrigTrackingxAODCnv_Bjet_IDTrig",
			# "HLT_mu.*bperf_dr05:key=InDetTrigTrackingxAODCnv_Bjet_FTF"
			]
		ToolSvc += tidabjet;
		list += [ "TrigTestBase/IDBjetTool" ]


                # Shifter instances
		tidabjetshift = TrigTestBase(name = "IDBjetShifterTool",
					     histoPathBase = "/Trigger/HLT")
		tidabjetshift.AnalysisConfig = "Tier0"
		tidabjetshift.SliceTag = "HLT/TRIDT/Bjet/Shifter"
		tidabjetshift.ntupleChainNames += [
			"Offline",
			"HLT_j.*bperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
			"HLT_j.*bperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF"
			"HLT_j.*bperf:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
			"HLT_j.*bperf:InDetTrigTrackingxAODCnv_Bjet_FTF"
			]
		ToolSvc += tidabjetshift;
		list += [ "TrigTestBase/IDBjetShifterTool" ]



		##############################################################
		# Bphys instances
		##############################################################
		
		# Expert instances 
		tidabphys = TrigTestBase(name = "IDBphysTool",
					histoPathBase = "/Trigger/HLT")
		tidabphys.AnalysisConfig = "Tier0"
		tidabphys.SliceTag = "HLT/TRIDT/Bphys/Expert"
		# tidabase.OutputLevel = DEBUG
		tidabphys.ntupleChainNames += [
			"Offline",
			"HLT_.*Bmumux.*:InDetTrigTrackingxAODCnv_Bphysics_EFID",
			"HLT_.*Bmumux.*:InDetTrigTrackingxAODCnv_Bphysics_IDTrig",
			"HLT_.*Bmumux.*:InDetTrigTrackingxAODCnv_Bphysics_FTF"
			]
		ToolSvc += tidabphys;
		list += [ "TrigTestBase/IDBphysTool" ]


		# Shifter instances 
		tidabphysshift = TrigTestBase(name = "IDBphysShifterTool",
					histoPathBase = "/Trigger/HLT")
		tidabphysshift.AnalysisConfig = "Tier0"
		tidabphysshift.SliceTag = "HLT/TRIDT/Bphys/Shifter"
		tidabphysshift.ntupleChainNames += [
			"Offline",
			"HLT_.*Bmumux.*:InDetTrigTrackingxAODCnv_Bphysics_IDTrig",
			"HLT_.*Bmumux.*:InDetTrigTrackingxAODCnv_Bphysics_FTF"
			]
		ToolSvc += tidabphysshift;
		list += [ "TrigTestBase/IDBphysShifterTool" ]




		##############################################################
		# minBias instances
		##############################################################
		
		# Expert instances 
		tidaminbias = TrigTestBase(name = "IDminBiasTool",
					histoPathBase = "/Trigger/HLT")
		tidaminbias.AnalysisConfig = "Tier0"
		tidaminbias.SliceTag = "HLT/TRIDT/minBias/Expert"
		# tidabase.OutputLevel = DEBUG
		tidaminbias.ntupleChainNames += [
			"Offline",
			"HLT_mb_.*_hmtperf:InDetTrigTrackingxAODCnv_minBias_EFID",
			"HLT_mb_.*_hmtperf:InDetTrigTrackingxAODCnv_minBias_IDTrig",
			"HLT_mb_.*_hmtperf:InDetTrigTrackingxAODCnv_minBias_FTF"
			]
		ToolSvc += tidaminbias;
		list += [ "TrigTestBase/IDminBiasTool" ]


		# Shifter instances 
		tidaminbiasshift = TrigTestBase(name = "IDminBiasShifterTool",
					histoPathBase = "/Trigger/HLT")
		tidaminbiasshift.AnalysisConfig = "Tier0"
		tidaminbiasshift.SliceTag = "HLT/TRIDT/minBias/Shifter"
		tidaminbiasshift.ntupleChainNames += [
			"Offline",
			"HLT_mb_sp.*_hmtperf:InDetTrigTrackingxAODCnv_minBias_IDTrig",
			"HLT_mb_sp.*_hmtperf:InDetTrigTrackingxAODCnv_minBias_FTF"
			]
		ToolSvc += tidaminbiasshift;
		list += [ "TrigTestBase/IDminBiasShifterTool" ]




	if False:

		##############################################################
		# Generic test instances - these signatures are largely 
                #                          obsolete and incorrect now
		##############################################################

		# test instances
		tidatool = TrigTestBase(name = "TIDATool",
					histoPathBase = "/Trigger/HLT")
		tidatool.AnalysisConfig = "Tier0"
		tidatool.SliceTag = "HLT/TRIDT/MonTest"
		# tidatool.OutputLevel = DEBUG
		tidatool.ntupleChainNames += [
			"Offline",
			#egamma
			"EF_e24vh_medium1:InDetTrigParticleCreation_Electron_EFID",
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
			"EF_e5_medium1_IDTrkNoCut:InDetTrigParticleCreation_Electron_EFID"

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

	return list

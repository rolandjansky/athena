# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def TrigTauMonitoringTool():
	full_tau = [
		# put some chains in addition to the ones already in the TrigTauMonitCategory.py lists	                   
	]

	# Setup emulation
	emul_l1_tau = [
		#'L1_EM15HI_2TAU12IM',
		'L1_EM15HI_2TAU12IM_J25_3J12',
		'L1_EM15HI_2TAU12IM_XE35',
		#'L1_MU10_TAU12IM',
		'L1_MU10_TAU12IM_J25_2J12',
		'L1_MU10_TAU12IM_XE35',
		'L1_TAU20IM_2J20_XE45',
		#'L1_TAU20IM_2TAU12IM',
		'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',
		'L1_TAU20IM_2TAU12IM_XE35',
		'L1_TAU60',
		'L1_DR-TAU20ITAU12I',
		'L1_DR-TAU20ITAU12I-J25',
		'L1_DR-EM15TAU12I-J25',
		'L1_DR-MU10TAU12I_TAU12I-J25'
	]

	emul_hlt_tau = [
#		'HLT_tau25_perf_ptonly',
	]

	ditau_topo_chains = [
            'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM',
            'tau35_medium1_tracktwo_tau25_medium1_tracktwo', # 2tau non-L1TOPO
            'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I',
            'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25', # 2tau L1TOPO+J
            'tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR30',
            'tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR30_L1DR-TAU20ITAU12I',
            'tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR30_L1DR-TAU20ITAU12I-J25', # 2tau L1TOPO+J
            'tau80_medium1_tracktwo_L1TAU60_tau35_medium1_tracktwo_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I', # 2tau L1TOPO
	]
	
	mutau_topo_chains = [
            'mu14_ivarloose_tau25_medium1_tracktwo', # tau+mu non-L1TOPO
            'mu14_ivarloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25', # tau+mu L1TOPO
            'mu14_ivarloose_tau25_medium1_tracktwo_L1MU10_TAU20IM_J25_2J20', # tau+mu non-L1TOPO
			'mu14_ivarloose_tau25_medium1_tracktwo_03dR30',
			'mu14_ivarloose_tau25_medium1_tracktwo_03dR30_L1DR-MU10TAU12I_TAU12I-J25',
	]

	eltau_topo_chains = [
			'e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo', # tau+e+J non-L1TOPO
            'e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', # tau+e+J L1TOPO
			'e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_03dR30',
			'e17_lhmedium_nod0_tau25_medium1_tracktwo_03dR30_L1DR-EM15TAU12I-J25',
	]

	tau_topo_support_chains = [

	]

#	tau_track_test = [
#		'tau25_perf_track_emulate',
#		'tau25_perf_tracktwo_emulate',
#		'tau25_perf_track_ptmin_emulate',
#		'tau25_perf_tracktwo_ptmin_emulate',
#		'tau25_medium1_track_ptmin_emulate',
#		'tau25_medium1_tracktwo_ptmin_emulate',
# 		'tau25_perf_tracktwo_ptmin_no0reject_emulate',
#		'tau25_medium1_tracktwo_ptmin_no0reject_emulate'
#	]

	ztt_tau = [
		'tau25_medium1_tracktwo',
	]
	# removed ztt_tau chains
	# 'tau25_idperf_tracktwo',
	# 'tau25_perf_tracktwo',

	highpt_tau = [
		'tau80_medium1_tracktwo_L1TAU60',
		'tau160_idperf_tracktwo_L1TAU100',
		'tau160_medium1_tracktwo_L1TAU100',
	]
	# removed highpt_tau chains:
	# 'tau160_medium1HighptL_tracktwo',
	# 'tau160_medium1HighptM_tracktwo',
	# 'tau160_medium1HighptH_tracktwo'
 

	# get the Level1 Emulation tool from the emulation python config
	from TrigTauEmulation.TrigTauEmulationConfig import get_level1_emulator
	Level1Emulator = get_level1_emulator('Level1Emulator', emul_l1_tau)
	
	# Add the Level1 emulation tool to the tool service
	from AthenaCommon.AppMgr import ToolSvc
	ToolSvc += Level1Emulator

	from AthenaCommon.Constants import VERBOSE, DEBUG, INFO, ERROR
	from LumiBlockComps.LuminosityToolDefault import LuminosityToolOnline
	LumiToolOnline = LuminosityToolOnline("OnlLuminosityTool")
	#LumiToolOnline.OutputLevel = DEBUG
	ToolSvc += LumiToolOnline

	# get the HLT emulation tool from the emulation python config
#	from TrigTauEmulation.TrigTauEmulationConfig import get_hlt_emulator
#	HltEmulator = get_hlt_emulator('HltEmulator', emul_hlt_tau, Level1Emulator)
	# Add the HLT emulation tool to the tool service
#	ToolSvc += HltEmulator


	from TrigHLTMonitoring.HLTMonTriggerList import hltmonList  # access to central tool
	from TrigTauMonitoring.TrigTauMonitoringConf import HLTTauMonTool
	from AthenaCommon.GlobalFlags import globalflags

	HLTTauMon = HLTTauMonTool(name = 'HLTTauMon',
				histoPathBase		= "/Trigger/HLT",
				monitoring_tau		= hltmonList.monitoring_tau,
				primary_tau		= full_tau, #[]
				prescaled_tau		= [], #tau_track_test, #[],
				LowestSingleTau		= hltmonList.monitoring_singleTau, #"tau25_medium1_tracktwo",
				Highpt_tau		= highpt_tau,
				Ztt_tau		 	= ztt_tau,
				EffOffTauPtCut	 	= 25000.,  #MeV
				TurnOnCurves		= True,
				TurnOnCurvesDenom	= "Reco", # combined string with combination of "Truth", "Reco" and "Presel". For Truth doTruth=True!
				doTruth			= False,
				doRealZtautauEff	= True,
				dodijetFakeTausEff	= True,
				doBootstrap		= False,
				doEmulation		= True,
				emulation_l1_tau	= emul_l1_tau,
				emulation_hlt_tau	= emul_hlt_tau,
				L1EmulationTool		= Level1Emulator,
#				HltEmulationTool	= HltEmulator,
				doTrackCurves		= False,
#				doTestTracking		= False,
				doL1JetPlots		= False,
				doEFTProfiles		= False,
				domuCut40		= False,
				doEfficiencyRatioPlots = True, #False
				doL1TopoLeptonsMonitoringWarnings = False,
				topo_ditau_chains	= ditau_topo_chains,
				topo_mutau_chains	= mutau_topo_chains,
				topo_eltau_chains	= eltau_topo_chains,
				topo_support_chains	= tau_topo_support_chains,
				doTopoValidation	= False,
				L1TriggerCondition	= "Physics", #allowResurrectedDecision, alsoDeactivateTEs, Physics [default]
				HLTTriggerCondition	= "Physics",
				nTrkMax				= -1,
				nTrkMin				= -1,
				PtMax				= -1., #MeV
				PtMin				= -1., #MeV
				AbsEtaMax			= -1.,
				AbsEtaMin			= -1.,
				AbsPhiMax			= -1.,
				AbsPhiMin			= -1.,
				BDTMedium			= True,
                                isData                          = (globalflags.DataSource == 'data'));

	#from AthenaCommon.AppMgr import ToolSvc
	ToolSvc += HLTTauMon;
	print ToolSvc
	list = [ "HLTTauMonTool/HLTTauMon" ];
	return list
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter

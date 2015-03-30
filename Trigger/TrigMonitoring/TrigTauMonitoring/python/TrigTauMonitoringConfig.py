# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def TrigTauMonitoringTool():

##
## DC14 setup
##

##	mon_tau = ["Dump",
##
####		   "tau20_r1medium1",
####		   "tau20_r1perf",
####		   "tau20_r1_idperf",
##
##		   "tau25_medium1_calo",
####		   "tau25_medium1_calo_L1TAU12",
##		   "tau25_medium1_track",
##		   "tau25_medium1_tracktwo",
##		   "tau25_medium1_ptonly",
####		   "tau25_medium1_ptonly_L1TAU12",
#		   "tau25_medium1_caloonly",
#		   "tau25_medium1_trackonly",
####		   "tau25_medium1_mvonly",
####		   "tau25_r1perf",
####		   "tau25_r1perf_L1TAU8",
####		   "tau25_medium1_mvonly_L1TAU8",
#		   "tau25_perf",
####		   "tau25_perf_L1TAU8",
####		   "tau25_ptonly_idperf",
##
####		   "tau29_r1medium1",
####		   "tau29_r1perf",
##
#		   "tau35_medium1_calo",
####		   "tau35_medium1_calo_L1TAU20",
#		   "tau35_medium1_track",
#		   "tau35_medium1_ptonly",
####		   "tau35_medium1_ptonly_L1TAU20",
#		   "tau35_perf",
##
####		   "tau80_medium1_calo",
####		   "tau80_medium1_track",
##
####		   "tau125_r1medium1",
#		   "tau125_medium1_calo",
#		   "tau125_medium1_track",
####		   "tau125_r1perf",
#		   "tau125_perf"
##		   ]
##

##
## MC15 setup
##

	mon_tau = [
		'tau5_perf_ptonly_L1TAU8',
		'tau0_perf_ptonly_L1TAU12',               
		'tau0_perf_ptonly_L1TAU60',               

##		## Run-I comparison
##		'tau20_r1medium1',                        
##		'tau20_r1perf',                           
##		'tau20_r1_idperf',                        
##		'tau25_r1perf',                           
##		'tau29_r1medium1',                        
##		'tau29_r1perf',                           
##		'tau125_r1medium1',                       
##		'tau125_r1perf',                          

		## Run-II - No BDT: main track-based items
		'tau25_perf_tracktwo',                    
		'tau25_perf_tracktwo_L1TAU12',            
		'tau25_perf_track',                       
		'tau25_perf_track_L1TAU12',               

		## Run-II - No BDT: variations
		'tau25_perf_calo',                        
		'tau25_perf_calo_L1TAU12',                
		'tau25_perf_ptonly',                      
		'tau25_perf_ptonly_L1TAU12',              
		'tau25_perf_trackcalo',                   
		'tau25_perf_tracktwocalo',                

		## Run-II - With BDT: main track-based items
		'tau25_medium1_tracktwo',                 
		'tau25_medium1_track',                    
		'tau25_medium1_tracktwo_L1TAU12',         
		'tau25_medium1_tracktwo_L1TAU12IL',       
		'tau25_medium1_tracktwo_L1TAU12IT',       

		## Run-II - With BDT: main calo-based items
		'tau25_medium1_calo',                     
		'tau25_medium1_trackcalo',                
		'tau25_medium1_tracktwocalo',             

		## Run-II - With BDT: no pre-selection
		'tau25_medium1_mvonly',                   

		## Run-II - High-pT variations
		'tau35_medium1_tracktwo',                 
##		'tau35_medium1_tracktwo_L1TAU20',         
##		'tau35_medium1_tracktwo_L1TAU20IL',       
##		'tau35_medium1_tracktwo_L1TAU20IT',       
#		'tau35_medium1_track',                    
#		'tau35_medium1_calo',                     
#		'tau35_medium1_ptonly',                   
##		'tau35_medium1_ptonly_L1TAU20',           
#		'tau35_perf_tracktwo',                    
#		'tau35_perf_ptonly',                      

		'tau80_medium1_calo',                     
		'tau80_medium1_tracktwo',                 
		'tau80_medium1_track',                    
##		'tau80_medium1_tracktwo_L1TAU60',         

		'tau50_medium1_tracktwo_L1TAU12',         

		'tau125_medium1_calo',                    
		'tau125_medium1_tracktwo',                
		'tau125_medium1_track',                   
		'tau125_perf_tracktwo',                   
		'tau125_perf_ptonly',                     

		'tau160_medium1_tracktwo',

		# Ditau Items
#		'tau35_loose1_tracktwo_tau25_loose1_tracktwo',   # seeded by L1_TAU20IM_2TAU12IM_J25_2J20_3J12
#		'tau35_medium1_tracktwo_tau25_medium1_tracktwo',
#		'tau35_tight1_tracktwo_tau25_tight1_tracktwo',
#		'tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1TAU20IM_2TAU12IM',
#		'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM',
#		'tau35_tight1_tracktwo_tau25_tight1_tracktwo_L1TAU20IM_2TAU12IM',
		
#		'tau35_perf_ptonly_tau25_perf_ptonly_L1TAU20IM_2TAU12IM',
#		'tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_L1TAU20_2TAU12',

#		'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25',
#		'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I',
#		'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20ITAU12I-J25',
#		'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1BOX-TAU20ITAU12I',
	
		# High-pT DiTau seeding off of single-tau
#		'tau80_medium1_tracktwo_L1TAU60_tau50_medium1_tracktwo_L1TAU12',
#		'tau125_medium1_tracktwo_tau50_medium1_tracktwo_L1TAU12',
		
		# Tau + Electron
		# Low-threshold with jets
#		'e17_medium_tau25_medium1_tracktwo',
#		'e17_lhmedium_tau25_medium1_tracktwo',
		# Isolated Variant		
#		'e17_medium_iloose_tau25_medium1_tracktwo',
#		'e17_lhmedium_iloose_tau25_medium1_tracktwo',
		# Tau + Electron main L1Topo
#		'e17_medium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',
#		'e17_lhmedium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',
		# Isolated variant
#		'e17_medium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',
#		'e17_lhmedium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',
		# Tau + electron back-up L1Topo
#		'e17_medium_tau25_medium1_tracktwo_L1EM15TAU12I-J25',
#		'e17_lhmedium_tau25_medium1_tracktwo_L1EM15TAU12I-J25',
#		'e17_medium_tau25_medium1_tracktwo_L1EM15-TAU12I',
#		'e17_lhmedium_tau25_medium1_tracktwo_L1EM15-TAU12I',
		# High-threshold
#		'e17_medium_tau80_medium1_tracktwo',
#		'e17_lhmedium_tau80_medium1_tracktwo',
#		'e17_medium_tau80_medium1_tracktwo_L1EM15-TAU40',
#		'e17_lhmedium_tau80_medium1_tracktwo_L1EM15-TAU40',

		# Tau + Muon
#		'mu14_tau25_medium1_tracktwo',
#		'mu14_tau35_medium1_tracktwo_L1TAU20',
#		'mu14_tau35_medium1_tracktwo', 
#		'mu14_iloose_tau25_medium1_tracktwo', 
#		'mu14_iloose_tau35_medium1_tracktwo',
		# With L1Topo
#		'mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12-J25',
#		'mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I',
#		'mu14_tau25_medium1_tracktwo_L1MU10_TAU12I-J25',

		# Tau + MET
#		'tau35_medium1_tracktwo_xe70_L1XE45',
#		'tau35_medium1_tracktwo_L1TAU20_xe70_L1XE45',
		
		# Ditau + MET
#		'tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_xe50',
#		'tau35_medium1_tracktwo_tau25_medium1_tracktwo_xe50',

		# Tau + e + MET
#		'e17_medium_tau25_medium1_tracktwo_xe50',
#		'e17_lhmedium_tau25_medium1_tracktwo_xe50',
#		'e17_medium_iloose_tau25_medium1_tracktwo_xe50',
#		'e17_lhmedium_iloose_tau25_medium1_tracktwo_xe50',
		# With L1Topo
#		'e17_medium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I',
#		'e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I',
#		'e17_medium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I',
#		'e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I',

		# Tau + mu + MET
#		'mu14_tau25_medium1_tracktwo_xe50',
#		'mu14_iloose_tau25_medium1_tracktwo_xe50'

		]


	from TrigTauMonitoring.TrigTauMonitoringConf import HLTTauMonTool
	HLTTauMon = HLTTauMonTool(name			 = 'HLTTauMon',
				  histoPathBase		 = "/Trigger/HLT",
				  monitoring_tau	 = mon_tau,
				  primary_tau		 = [],
				  prescaled_tau		 = [],
				  LowestSingleTau 	 = "tau25_medium1_tracktwo",
                                  LowestDiTau	         = "tau35_medium1_tracktwo_tau25_medium1_tracktwo",
                                  LowestETau             = "e17_medium_tau25_medium1_tracktwo",
                                  LowestMuTau            = "mu14_tau25_medium1_tracktwo",
                                  LowestMetTau           = "tau35_medium1_tracktwo_xe70_L1XE45",
				  EffOffTauPtCut	 = 25000.,  #MeV
				  TurnOnCurves           = False,
				  doTruth		 = False,
				  doEmulation		 = False,
				  doTestTracking	 = False,
				  doIncludeL1deactivateTE 	= False,
				  doIncludePreseldeactivateTE 	= False,
				  doIncludeHLTdeactivateTE 	= False,
                                  nTrkMax	= -1,
				  nTrkMin	= -1,
				  PtMax		= -1., #MeV
				  PtMin		= -1., #MeV
				  AbsEtaMax	= -1.,
				  AbsEtaMin	= -1.,
				  AbsPhiMax	= -1.,
				  AbsPhiMin	= -1.);

	from AthenaCommon.AppMgr import ToolSvc
	ToolSvc += HLTTauMon;
	list = [ "HLTTauMonTool/HLTTauMon" ];
	return list
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter

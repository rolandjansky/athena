# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------------------------------------------------------------------#
def setupMenu():
    
    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenu.menu.Physics_HI_v4.py' )

    from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1 as tcl1
    
    if tcl1.current:
        log.info("L1 items: %s " % tcl1.current.menu.items.itemNames())
    else:
        log.info("ERROR L1 menu has not yet been defined")

    HardProbesStream="Main"
    MinBiasStream="Main"
    UPCStream="UPC"

    # Disable Calo offset correction for HI menus (ATR-13323)
    from CaloTools.CaloNoiseFlags import jobproperties
    jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(0)
    TriggerFlags.doCaloOffsetCorrection.set_Value_and_Lock(False)
    log.info("Calo offset correction for HI trigger menu:%s" % TriggerFlags.doCaloOffsetCorrection)
            
    # INPUT FORMAT FOR CHAINS:
    # ['chainName', 'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep]

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.TestSlice.signatures = [] 

    TriggerFlags.AFPSlice.signatures = []
    
    TriggerFlags.MuonSlice.signatures = [
	#ID tracking performance
    ['mu4_idperf',          'L1_MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu6_idperf',          'L1_MU4', [], [HardProbesStream, 'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu8_idperf',          'L1_MU6', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1], #no configuration
	['mu10_idperf',         'L1_MU10', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],

	#RoI muons
	['mu4',                 'L1_MU4', [], [HardProbesStream,  'express'], ['RATE:SingleMuon','BW:Muon'], -1],
        ['mu4_hf',              'L1_MU4', [], [HardProbesStream,  'express'], ['RATE:SingleMuon','BW:Muon'], -1],
        ['mu4_hf_0eta105',      'L1_MU4', [], [HardProbesStream,  'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu4_L1MU4_VTE10',     'L1_MU4_VTE10', [], [UPCStream,  'express'], ['RATE:UPCMuon','BW:Muon'], -1],
	['mu4_L1MU4_VZDC_A',    'L1_MU4_VZDC_A', [], [UPCStream,  'express'], ['RATE:UPCMuon','BW:Muon'], -1],
	['mu4_L1MU4_VZDC_C',    'L1_MU4_VZDC_C', [], [UPCStream,  'express'], ['RATE:UPCMuon','BW:Muon'], -1],
	['2mu4_L12MU4_VTE10',   'L1_2MU4_VTE10', [], [UPCStream,  'express'], ['RATE:UPCMuon','BW:Muon'], -1],
	['2mu4_L12MU4_VZDC_A',  'L1_2MU4_VZDC_A', [], [UPCStream,  'express'], ['RATE:UPCMuon','BW:Muon'], -1],
	['2mu4_L12MU4_VZDC_C',  'L1_2MU4_VZDC_C', [], [UPCStream,  'express'], ['RATE:UPCMuon','BW:Muon'], -1],

    #['mu4_L1MU4_VZDC_A_C',      'L1_MU4_VZDC_A_C', [], [UPCStream], ['RATE:UPCMuon','BW:Muon'], -1],
    	['mu4_L1MU4_VTE10_VZDC_A',     'L1_MU4_VTE10_VZDC_A', [], [UPCStream,  'express'], ['RATE:UPCMuon','BW:Muon'], -1],
    	['mu4_L1MU4_VTE10_VZDC_C',     'L1_MU4_VTE10_VZDC_C', [], [UPCStream,  'express'], ['RATE:UPCMuon','BW:Muon'], -1],
    	['2mu4_L12MU4_VTE10_VZDC_A',     'L1_2MU4_VTE10_VZDC_A', [], [UPCStream,  'express'], ['RATE:UPCMuon','BW:Muon'], -1],
    	['2mu4_L12MU4_VTE10_VZDC_C',     'L1_2MU4_VTE10_VZDC_C', [], [UPCStream,  'express'], ['RATE:UPCMuon','BW:Muon'], -1],
	['mu4_iloose',                 'L1_MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu6',                 'L1_MU6', [], [HardProbesStream, 'express'], ['RATE:SingleMuon','BW:Muon'], -1],
    #['mu6_L1MU6_VZDC_A_C',  'L1_MU6_VZDC_A_C', [], [UPCStream], ['RATE:SingleMuon','BW:Muon'], -1],
        ['mu6_L1MU6_VTE10_VZDC_A',  'L1_MU6_VTE10_VZDC_A', [], [UPCStream], ['RATE:UPCMuon','BW:Muon'], -1],
        ['mu6_L1MU6_VTE10_VZDC_C',  'L1_MU6_VTE10_VZDC_C', [], [UPCStream], ['RATE:UPCMuon','BW:Muon'], -1],
	['mu6_iloose',                 'L1_MU6', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],	
	['mu8',                 'L1_MU6', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu10_L1MU6',                'L1_MU6', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu10',                'L1_MU10', [], [HardProbesStream,  'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu10_iloose',                'L1_MU10', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu11',                'L1_MU10', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu14',                'L1_MU10', [], [HardProbesStream,  'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu14_iloose',                'L1_MU10', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu15_L1MU6',                'L1_MU6', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu15',                'L1_MU15', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu15_L1MU10',                'L1_MU10', [], [HardProbesStream, 'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu18',                'L1_MU15', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu20',                'L1_MU20', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
        ['mu20_L1MU15',                'L1_MU15', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	
        #msonly
	['mu4_msonly',            'L1_MU4',           [], [HardProbesStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
	['mu6_msonly',            'L1_MU6',           [], [HardProbesStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
	['mu10_msonly',            'L1_MU10',           [], [HardProbesStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
	['mu14_msonly',            'L1_MU10',           [], [HardProbesStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
	['mu15_msonly',            'L1_MU15',           [], [HardProbesStream, 'express'], ['RATE:SingleMuon', 'BW:Muon'], -1],
	['mu20_msonly',            'L1_MU20',           [], [HardProbesStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
	

	#di-muons RoI based
	['2mu4',                 'L1_2MU4', [], [HardProbesStream, 'express'], ['RATE:MultiMuon','BW:Muon'], -1],
	['2mu6',                 'L1_2MU6', [], [HardProbesStream], ['RATE:MultiMuon','BW:Muon'], -1],
	['2mu4_msonly',          'L1_2MU4', [], [HardProbesStream], ['RATE:MultiMuon','BW:Muon'], -1],
    ['2mu6_msonly',          'L1_2MU4', [], [HardProbesStream], ['RATE:MultiMuon','BW:Muon'], -1],
    ['mu6_nomucomb_mu4_nomucomb',          'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [HardProbesStream], ['RATE:MultiMuon','BW:Muon'], -1],

	
	
    # muon calibration
    ['mu0_muoncalib',             'L1_MU4',      [],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
    ['mu0_muoncalib_L1MU15',      'L1_MU15',     [],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
    ['mu0_muoncalib_L1MU4_EMPTY', 'L1_MU4_EMPTY',[],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
    
        
        ]

    TriggerFlags.JetSlice.signatures = [
	#performance
	['j0_perf_L1ZDC_A_C',     'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j0_perf_L1RD0_FILLED',     'L1_RD0_FILLED',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],


	['j10_0eta490_L1RD0_FILLED',     'L1_RD0_FILLED',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_0eta490_L1RD0_FILLED_VZDC_A',            'L1_RD0_FILLED_VZDC_A',[], [UPCStream], ['RATE:SingleJet', 'BW:UPC'], -1],
	['j10_0eta490_L1RD0_FILLED_VZDC_C',            'L1_RD0_FILLED_VZDC_C',[], [UPCStream], ['RATE:SingleJet', 'BW:UPC'], -1],

	['j10_0eta490_L1MBTS_1',         'L1_MBTS_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
                
	['j10_0eta490_L1MBTS_2_C_VZDC_A',            'L1_MBTS_2_C_VZDC_A',[], [UPCStream, 'express'], ['RATE:SingleJet', 'BW:UPC'], -1],
	['j10_0eta490_L1MBTS_2_A_VZDC_C',            'L1_MBTS_2_A_VZDC_C',[], [UPCStream, 'express'], ['RATE:SingleJet', 'BW:UPC'], -1],

	['j10_0eta490_L1TE5',            'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_0eta490_L1TE5_VZDC_A',            'L1_TE5_VZDC_A',[], [UPCStream], ['RATE:SingleJet', 'BW:UPC'], -1],
	['j10_0eta490_L1TE5_VZDC_C',            'L1_TE5_VZDC_C',[], [UPCStream], ['RATE:SingleJet', 'BW:UPC'], -1],

	['j10_0eta490_L1TE10',             'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_0eta490_L1TE10_VZDC_A',             'L1_TE10_VZDC_A',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_0eta490_L1TE10_VZDC_C',             'L1_TE10_VZDC_C',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_L1J5_VZDC_A',            'L1_J5_VZDC_A',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_L1J5_VZDC_C',            'L1_J5_VZDC_C',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	['j20_0eta490_L1TE10_VZDC_A',             'L1_TE10_VZDC_A',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_0eta490_L1TE10_VZDC_C',             'L1_TE10_VZDC_C',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	['j20_L1J5_VZDC_A',            'L1_J5_VZDC_A',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_L1J5_VZDC_C',            'L1_J5_VZDC_C',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_0eta490_L1TE20_VZDC_A',             'L1_TE20_VZDC_A',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_0eta490_L1TE20_VZDC_C',             'L1_TE20_VZDC_C',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	['j30_L1J5_VZDC_A',            'L1_J5_VZDC_A',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_L1J5_VZDC_C',            'L1_J5_VZDC_C',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_0eta490_L1TE20_VZDC_A',            'L1_TE20_VZDC_A',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_0eta490_L1TE20_VZDC_C',            'L1_TE20_VZDC_C',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	['j40_L1J5_VZDC_A',            'L1_J5_VZDC_A',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_L1J5_VZDC_C',            'L1_J5_VZDC_C',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_L1J10_VZDC_A',            'L1_J10_VZDC_A',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_L1J10_VZDC_C',            'L1_J10_VZDC_C',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	['j60_L1J15_VZDC_A',            'L1_J15_VZDC_A',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j60_L1J15_VZDC_C',            'L1_J15_VZDC_C',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j60_L1J20_VZDC_A',            'L1_J20_VZDC_A',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j60_L1J20_VZDC_C',            'L1_J20_VZDC_C',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],


	['j10_0eta490_L1ZDC_A',          'L1_ZDC_A',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_0eta490_L1ZDC_A_VZDC_C',          'L1_ZDC_A_VZDC_C',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_0eta490_L1ZDC_C',          'L1_ZDC_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_0eta490_L1ZDC_C_VZDC_A',          'L1_ZDC_C_VZDC_A',[], [UPCStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_0eta490_L1ZDC',            'L1_ZDC',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],

	#physics
	#R=0.4 jets ion jets
	#['j20_ion',         'L1_RD0_FILLED',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j20_ion_0eta490',         'L1_RD0_FILLED',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_ion_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_ion_0eta490_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j20_ion_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j20_ion_0eta490_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_ion_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_ion_0eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_ion_L1TE10',         'L1_TE10',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_ion_0eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],   
	['j40_ion_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_ion_0eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	['j20_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_ion_0eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_ion_0eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],   
	['j40_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_ion_0eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	['j20_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_ion_0eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_ion_0eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],   
	['j40_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_ion_0eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	['j30_ion_L1J5',         'L1_J5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_ion_L1J5',         'L1_J5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_ion_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j50_ion_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j50_ion_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j60_ion_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j60_ion_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j75_ion_L1J20',         'L1_J20',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j85_ion_L1J20',         'L1_J20',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j90_ion_L1J20',         'L1_J20',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j100_ion_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j100_ion_L1J30',         'L1_J30',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j110_ion_L1J30',         'L1_J30',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j125_ion_L1J30',         'L1_J30',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j150_ion_L1J30',         'L1_J30',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j175_ion_L1J50',         'L1_J50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j200_ion_L1J50',         'L1_J50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j250_ion_L1J50',         'L1_J50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	#R=0.4 jets pp jets
	#['j20',         'L1_RD0_FILLED',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j20_0eta490',         'L1_RD0_FILLED',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_0eta490_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j20_ion_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j20_0eta490_ion_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_0eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_L1TE10',         'L1_TE10',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_0eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],   
	['j40_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_0eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	['j20_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_0eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_0eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],   
	['j40_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_0eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	['j20_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_0eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_0eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],   
	['j40_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_0eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	['j30_L1J5',         'L1_J5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_L1J5',         'L1_J5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j50_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j50_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j60_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j60',         'L1_J20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j75_L1J20',         'L1_J20',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j85',         'L1_J20',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j90_L1J20',         'L1_J20',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j100_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j100_L1J30',         'L1_J30',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j110',         'L1_J30',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j125_L1J30',         'L1_J30',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j150_L1J30',         'L1_J30',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j175',         'L1_J50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j200',         'L1_J50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j250',         'L1_J50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	
	#forward jets ATR-12623
	['j10_320eta490_ion_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_320eta490_ion_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_320eta490_ion_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_320eta490_ion_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_320eta490_ion_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_320eta490_ion_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_320eta490_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_320eta490_ion_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_320eta490_ion_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_320eta490_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_320eta490_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_320eta490_ion_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_320eta490_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_320eta490_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_320eta490_ion',         'L1_J10.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j45_320eta490_ion',         'L1_J15.31ETA49',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j55_320eta490_ion',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j65_320eta490_ion',         'L1_J20.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j75_320eta490_ion',         'L1_J20.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	['j10_320eta490_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_320eta490_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_320eta490_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_320eta490_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_320eta490_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_320eta490',         'L1_J10.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j45_320eta490',         'L1_J15.31ETA49',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j55_320eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j65_320eta490',         'L1_J20.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j75_320eta490',         'L1_J20.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	#forward jets in negative direction 
	['j10_ion_n320eta490_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_ion_n320eta490_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_ion_n320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_ion_n320eta490_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_ion_n320eta490_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_ion_n320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_ion_n320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_ion_n320eta490_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_ion_n320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_ion_n320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_ion_n320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_ion_n320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_ion_n320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_ion_n320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_ion_n320eta490',         'L1_J10.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j45_ion_n320eta490',         'L1_J15.31ETA49',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j55_ion_n320eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j65_ion_n320eta490',         'L1_J20.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j75_ion_n320eta490',         'L1_J20.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	['j10_n320eta490_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_n320eta490_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_n320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_n320eta490_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_n320eta490_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_n320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_n320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_n320eta490_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_n320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_n320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_n320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_n320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_n320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_n320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_n320eta490',         'L1_J10.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j45_n320eta490',         'L1_J15.31ETA49',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j55_n320eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j65_n320eta490',         'L1_J20.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j75_n320eta490',         'L1_J20.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	#forward jets in positive direction 
	['j10_ion_p320eta490_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_ion_p320eta490_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_ion_p320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_ion_p320eta490_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_ion_p320eta490_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_ion_p320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_ion_p320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_ion_p320eta490_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_ion_p320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_ion_p320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_ion_p320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_ion_p320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_ion_p320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_ion_p320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_ion_p320eta490',         'L1_J10.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j45_ion_p320eta490',         'L1_J15.31ETA49',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j55_ion_p320eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j65_ion_p320eta490',         'L1_J20.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j75_ion_p320eta490',         'L1_J20.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	['j10_p320eta490_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_p320eta490_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_p320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_p320eta490_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_p320eta490_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_p320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_p320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_p320eta490_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_p320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_p320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_p320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_p320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_p320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_p320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_p320eta490',         'L1_J10.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j45_p320eta490',         'L1_J15.31ETA49',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j55_p320eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j65_p320eta490',         'L1_J20.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j75_p320eta490',         'L1_J20.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	#di-forward jest
	['2j10_320eta490_ion_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j10_320eta490_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j20_320eta490_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j20_320eta490_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j30_320eta490_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j30_320eta490_ion',         'L1_J10.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j40_320eta490_ion',         'L1_J10.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j40_320eta490_ion_L1J15.31ETA49',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	['2j10_320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j10_320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j20_320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j20_320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j30_320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j30_320eta490',         'L1_J10.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j40_320eta490',         'L1_J10.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j40_320eta490_L1J15.31ETA49',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	#positive
	['2j10_ion_p320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j10_ion_p320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j20_ion_p320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j20_ion_p320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j30_ion_p320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j30_ion_p320eta490',         'L1_J10.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j40_ion_p320eta490',         'L1_J10.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j40_ion_p320eta490_L1J15.31ETA49',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	['2j10_p320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j10_p320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j20_p320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j20_p320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j30_p320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j30_p320eta490',         'L1_J10.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j40_p320eta490',         'L1_J10.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j40_p320eta490_L1J15.31ETA49',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	#negative
	['2j10_ion_n320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j10_ion_n320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j20_ion_n320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j20_ion_n320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j30_ion_n320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j30_ion_n320eta490',         'L1_J10.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j40_ion_n320eta490',         'L1_J10.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j40_ion_n320eta490_L1J15.31ETA49',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	['2j10_n320eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j10_n320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j20_n320eta490_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j20_n320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j30_n320eta490_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j30_n320eta490',         'L1_J10.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j40_n320eta490',         'L1_J10.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['2j40_n320eta490_L1J15.31ETA49',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],

	
	#multijets, ion versions	
	['j75_ion_2j20_ion_0eta490_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j75_ion_2j30_ion_0eta490_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j75_ion_3j20_ion_0eta490_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j75_ion_3j30_ion_0eta490_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j75_ion_4j20_ion_0eta490_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j75_ion_4j30_ion_0eta490_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	
	['j60_ion_2j20_ion_0eta490_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j60_ion_2j30_ion_0eta490_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j60_ion_3j20_ion_0eta490_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j60_ion_3j30_ion_0eta490_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j60_ion_4j20_ion_0eta490_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j60_ion_4j30_ion_0eta490_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],	
	
	['j50_ion_2j20_ion_0eta490_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j50_ion_2j30_ion_0eta490_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j50_ion_3j20_ion_0eta490_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j50_ion_3j30_ion_0eta490_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j50_ion_4j20_ion_0eta490_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j50_ion_4j30_ion_0eta490_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	
	['j40_ion_2j20_ion_0eta490_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j40_ion_3j20_ion_0eta490_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j40_ion_4j20_ion_0eta490_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],

	#forward multijets
	['j45_ion_320eta490_2j20_ion_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j45_ion_320eta490_3j20_ion_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j45_ion_320eta490_4j20_ion_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j55_ion_320eta490_2j20_ion_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j55_ion_320eta490_3j20_ion_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j55_ion_320eta490_4j20_ion_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	
	#multijets, pp versions	
	['j75_2j20_0eta490_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j75_2j30_0eta490_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j75_3j20_0eta490_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j75_3j30_0eta490_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j75_4j20_0eta490_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j75_4j30_0eta490_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	
	['j60_2j20_0eta490_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j60_2j30_0eta490_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j60_3j20_0eta490_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j60_3j30_0eta490_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j60_4j20_0eta490_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j60_4j30_0eta490_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],	
	
	['j50_2j20_0eta490_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j50_2j30_0eta490_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j50_3j20_0eta490_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j50_3j30_0eta490_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j50_4j20_0eta490_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j50_4j30_0eta490_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	
	['j40_2j20_0eta490_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j40_3j20_0eta490_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j40_4j20_0eta490_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],

	#forward multijets
	#inclusive
	['j45_320eta490_2j20_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j45_320eta490_3j20_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j45_320eta490_4j20_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j55_320eta490_2j20_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j55_320eta490_3j20_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j55_320eta490_4j20_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	#negative
	['j45_n320eta490_2j20_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j45_n320eta490_3j20_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j45_n320eta490_4j20_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j55_n320eta490_2j20_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j55_n320eta490_3j20_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j55_n320eta490_4j20_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	#positive
	['j45_p320eta490_2j20_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j45_p320eta490_3j20_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j45_p320eta490_4j20_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j55_p320eta490_2j20_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j55_p320eta490_3j20_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j55_p320eta490_4j20_0eta490',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:MultiJet', 'BW:Jet'], -1],

        ]

    TriggerFlags.BjetSlice.signatures = [
    
    ['j40_bmv2c2060_split_L1J10', 'L1_J10', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j40_bmv2c2070_split_L1J10', 'L1_J10', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j40_bmv2c2077_split_L1J10', 'L1_J10', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j40_bmv2c2085_split_L1J10', 'L1_J10', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
    ['j50_bmv2c2060_split_L1J15', 'L1_J15', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j50_bmv2c2070_split_L1J15', 'L1_J15', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j50_bmv2c2077_split_L1J15', 'L1_J15', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j50_bmv2c2085_split_L1J15', 'L1_J15', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j60_bmv2c2060_split_L1J15', 'L1_J15', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j60_bmv2c2070_split_L1J15', 'L1_J15', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j60_bmv2c2077_split_L1J15', 'L1_J15', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j60_bmv2c2085_split_L1J15', 'L1_J15', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j60_bmv2c2060_split_L1J20', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j60_bmv2c2070_split_L1J20', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j60_bmv2c2077_split_L1J20', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j60_bmv2c2085_split_L1J20', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j75_bmv2c2060_split_L1J20', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j75_bmv2c2070_split_L1J20', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j75_bmv2c2077_split_L1J20', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j75_bmv2c2085_split_L1J20', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j85_bmv2c2060_split_L1J20', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j85_bmv2c2070_split_L1J20', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j85_bmv2c2077_split_L1J20', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j85_bmv2c2085_split_L1J20', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j100_bmv2c2060_split_L1J20', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j100_bmv2c2070_split_L1J20', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j100_bmv2c2077_split_L1J20', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j100_bmv2c2085_split_L1J20', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	
	['j40_boffperf_split', 'L1_J10', [], [HardProbesStream, 'express'], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j50_boffperf_split', 'L1_J15', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j60_boffperf_split', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j75_boffperf_split', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j85_boffperf_split', 'L1_J20', [], [HardProbesStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
	['j100_boffperf_split', 'L1_J20', [], [HardProbesStream, 'express'], ['RATE:SingleBJet', 'BW:BJet'], -1],
    
        ]

    TriggerFlags.METSlice.signatures = [
        ]

    TriggerFlags.TauSlice.signatures = [
        ]

    TriggerFlags.EgammaSlice.signatures = [

	#electrons
	['e5_etcut',                  'L1_EM3',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e5_etcut_L1EM3_VZDC_A',                  'L1_EM3_VZDC_A',  [], [UPCStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e5_etcut_L1EM3_VZDC_C',                  'L1_EM3_VZDC_C',  [], [UPCStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],


	['e5_loose',                  'L1_EM3',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['e5_lhloose_nod0',                  'L1_EM3',  [], [UPCStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e5_lhloose_L1EM3_VZDC_A',                  'L1_EM3_VZDC_A',  [], [UPCStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e5_lhloose_L1EM3_VZDC_C',                  'L1_EM3_VZDC_C',  [], [UPCStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	
	['e5_lhloose_nod0_L1EM3_VZDC_A',                  'L1_EM3_VZDC_A',  [], [UPCStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e5_lhloose_nod0_L1EM3_VZDC_C',                  'L1_EM3_VZDC_C',  [], [UPCStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
 

	['2e5_etcut',                  'L1_2EM3',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
        ['2e5_etcut_L12EM3_VZDC_A',                  'L1_2EM3_VZDC_A',  [], [UPCStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
        ['2e5_etcut_L12EM3_VZDC_C',                  'L1_2EM3_VZDC_C',  [], [UPCStream], ['RATE:DiElectron', 'BW:Egamma'], -1],

	['2e5_loose',                  'L1_2EM3',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
	['2e5_lhloose',                  'L1_2EM3',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
	['2e5_lhloose_L12EM3_VZDC_A',                  'L1_2EM3_VZDC_A',  [], [UPCStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
	['2e5_lhloose_L12EM3_VZDC_C',                  'L1_2EM3_VZDC_C',  [], [UPCStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
	['2e5_lhloose_nod0_L12EM3_VZDC_A',                  'L1_2EM3_VZDC_A',  [], [UPCStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
	['2e5_lhloose_nod0_L12EM3_VZDC_C',                  'L1_2EM3_VZDC_C',  [], [UPCStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
 

	['e7_etcut',                  'L1_EM5',  [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e7_loose',                  'L1_EM5',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e7_lhloose',                'L1_EM5',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e7_lhloose_nod0',                'L1_EM5',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['2e7_etcut',                  'L1_2EM5',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
	['2e7_loose',                  'L1_2EM5',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
	['2e7_lhloose',                'L1_2EM5',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
	['2e7_lhloose_nod0',                'L1_2EM5',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],


	['e10_etcut',                  'L1_EM7',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e10_loose',                  'L1_EM7',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e10_lhloose',                'L1_EM7',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e10_lhloose_nod0',                'L1_EM7',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['2e10_etcut',                  'L1_2EM7',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
	['2e10_loose',                  'L1_2EM7',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
	['2e10_lhloose',                'L1_2EM7',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
	['2e10_lhloose_nod0',                'L1_2EM7',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],

	['e12_etcut',                  'L1_EM8',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e12_loose',                  'L1_EM8',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e12_lhloose',                'L1_EM8',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e12_lhloose_nod0',                'L1_EM8',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],


	['e15_etcut',                  'L1_EM10',  [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_loose',                  'L1_EM10',  [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_lhloose',                'L1_EM10',  [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_lhloose_nod0',                'L1_EM10',  [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_lhloose_idperf',                'L1_EM10',  [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	
        ['e15_etcut_L1EM12',           'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e17_etcut',                  'L1_EM12',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e17_loose',                  'L1_EM12',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e17_lhloose',                'L1_EM12',  [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e17_lhloose_nod0',                'L1_EM12',  [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],


	['e18_etcut',                  'L1_EM14',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e18_loose',                  'L1_EM14',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e18_lhloose',                'L1_EM14',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e18_lhloose_nod0',                'L1_EM14',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['e20_etcut',                  'L1_EM14',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e20_loose',                  'L1_EM14',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e20_lhloose',                'L1_EM14',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e20_lhloose_nod0',                'L1_EM14',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['e20_etcut_L1EM16',                  'L1_EM16',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e20_loose_L1EM16',                  'L1_EM16',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e20_lhloose_L1EM16',                'L1_EM16',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e20_lhloose_nod0_L1EM16',                'L1_EM16',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['e22_etcut',                  'L1_EM16',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e22_loose',                  'L1_EM16',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e22_lhloose',                'L1_EM16',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e22_lhloose_nod0',                'L1_EM16',  [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['e24_etcut',                  'L1_EM20',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e24_loose',                  'L1_EM20',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e24_lhloose',                'L1_EM20',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e24_lhloose_nod0',                'L1_EM20',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],


        
        ['g5_loose_L1VZDC_A',             'L1_EM3_VZDC_A', [], [UPCStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g5_loose_L1VZDC_C',             'L1_EM3_VZDC_C', [], [UPCStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

        ['g10_loose_L1VZDC_A',             'L1_EM5_VZDC_A', [], [UPCStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g10_loose_L1VZDC_C',             'L1_EM5_VZDC_C', [], [UPCStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

        ['g5_etcut_L1VZDC_A',             'L1_EM3_VZDC_A', [], [UPCStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g5_etcut_L1VZDC_C',             'L1_EM3_VZDC_C', [], [UPCStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

        ['g10_etcut_L1VZDC_A',             'L1_EM5_VZDC_A', [], [UPCStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g10_etcut_L1VZDC_C',             'L1_EM5_VZDC_C', [], [UPCStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        

	#photons
	['2g10_loose',             'L1_2EM3', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['2g10_loose_L12EM5',       'L1_2EM5', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

	['g5_etcut',              'L1_EM3', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g5_loose',              'L1_EM3', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g7_loose',              'L1_EM5', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g7_medium',              'L1_EM5', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g8_etcut',              'L1_EM5', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	
	['g10_loose',              'L1_EM5', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g15_loose',              'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g13_etcut',              'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

	['g20_loose',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g18_etcut',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	
	['g23_etcut',              'L1_EM16', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

	['g25_loose',              'L1_EM20', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g28_etcut',              'L1_EM20', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g30_loose',              'L1_EM20', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

	['g35_loose',              'L1_EM20', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g33_etcut',              'L1_EM20', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

	['g60_loose',              'L1_EM20', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g58_etcut',              'L1_EM20', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],


#	['g12_etcut',                  'L1_EM10', [], [HardProbesStream], ['RATE:SinglPhoton', 'BW:Egamma'], -1],
#       	['g15_etcut',                  'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
#	['g18_etcut',                  'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
#	['g20_etcut',                  'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

	#Run 1 PID
#	['e15_loose',                  'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#        ['e15_medium',                 'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#	['e15_lhloose',                'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#	['e15_lhmedium',               'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

#	['e15_loose_L1EM12',           'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#        ['e15_medium_L1EM12',          'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#	['e15_lhloose_L1EM12',         'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#	['e15_lhmedium_L1EM12',        'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	
	#heavy ion instances	
        # they are to be removed 
#	['e10_etcut_ion',              'L1_EM7', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	##['e12_etcut_ion',              'L1_EM8', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#        ['e12_etcut_ion_L1EM10',       'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#        ['e15_etcut_ion',              'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#        ['e15_etcut_ion_L1EM12',       'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#	['g12_etcut_ion',              'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
#        ['g15_etcut_ion',              'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
#	['g18_etcut_ion',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
#	['g20_etcut_ion',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

#        ['e15_loose_ion',              'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#        ['e15_medium_ion',             'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#        ['e15_lhloose_ion',            'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#        ['e15_lhmedium_ion',           'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

#        ['e15_loose_ion_L1EM12',       'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#        ['e15_medium_ion_L1EM12',      'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#        ['e15_lhloose_ion_L1EM12',     'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#        ['e15_lhmedium_ion_L1EM12',    'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
 
	#backup for e15
#        ['e18_loose_ion',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#        ['e18_medium_ion',             'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#        ['e18_lhloose_ion',            'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#        ['e18_lhmedium_ion',           'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	#backup for e15
#	['e20_etcut_ion',              'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
#	['e20_loose_ion',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#       ['e20_lhloose_ion',            'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e20_etcut_L1EM12',                  'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e20_loose_L1EM12',                  'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e20_lhloose_L1EM12',                'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	#idperf
#	['e15_loose_ion_idperf',       'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
#	['e15_medium_ion_idperf',       'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
#	['e15_loose_ion_idperf_L1EM12', 'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
#	['e15_medium_ion_idperf_L1EM12', 'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],

	#di-electrons , timeouts ATR-10226
	##['2e10_loose_ion',               'L1_2EM7', [], [HardProbesStream, 'express'], ['RATE:MultiElectron', 'BW:Egamma'], -1],
	#['2e10_lhloose_ion',            'L1_2EM5', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

#	['g15_loose',                  'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
#	['g15_medium',                 'L1_EM10', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
#	['g20_loose',                  'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
#        ['g20_medium',                 'L1_EM12', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

#	['g15_loose_ion',              'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
#        ['g15_medium_ion',             'L1_EM10', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
#        ['g20_loose_ion',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
#        ['g20_medium_ion',             'L1_EM12', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

	#di-photon triggers
	['2g15_loose',                  'L1_2EM10', [], [HardProbesStream], ['RATE:MultiPhoton', 'BW:Egamma'], -1],
	##['2g15_loose_ion',              'L1_2EM10', [], [HardProbesStream], ['RATE:MultiPhoton', 'BW:Egamma'], -1],

	# Monopole triggers ATR-12467
#        ['g0_hiptrt_L1EM14',                    'L1_EM14', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
#	['g0_hiptrt_L1EM16',                    'L1_EM16', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

        ]

    TriggerFlags.BphysicsSlice.signatures = [
	['2mu4_bDimu',                    'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
	['2mu4_bJpsimumu',                    'L1_2MU4', [], [HardProbesStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],
	['2mu4_bUpsimumu',                    'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
	['2mu4_bBmumu',                    'L1_2MU4', [], [HardProbesStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1], 
	['2mu4_bBmumuxv2',                    'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
	['2mu4_bBmumux_BcmumuDsloose',                    'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1], 
	['2mu4_bBmumux_BpmumuKp',                    'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
	['2mu6_bDimu',                    'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1], 
	['2mu6_bJpsimumu',                'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
	['2mu6_bUpsimumu',                    'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
	['2mu6_bBmumu',                    'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1], 
	['2mu6_bBmumuxv2',                    'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
	['2mu6_bBmumux_BcmumuDsloose',                    'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1], 
	['2mu6_bBmumux_BpmumuKp',                    'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
	    
    ['mu4_mu4_idperf_bJpsimumu_noid',  'L1_2MU4', [], [HardProbesStream], ['RATE:IDMonitoring','BW:Bphys', 'BW:ID'], -1],
    ['mu13_mu13_idperf_Zmumu',     'L1_2MU6', [], [HardProbesStream], ['RATE:IDMonitoring','BW:Bphys', 'BW:ID'], -1],
        
    ['2mu4_bDimu_novtx_noos',         'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['2mu4_bDimu_noinvm_novtx_ss',         'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['2mu6_bDimu_noinvm_novtx_ss',         'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['2mu6_bDimu_novtx_noos',         'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    
    ['2mu4_nomucomb',                    'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['2mu6_nomucomb',                    'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['mu4_nomucomb',                    'L1_MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['mu6_nomucomb',                    'L1_MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    
    ['mu4_bJpsi_Trkloose',            'L1_MU4',  [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['mu6_bJpsi_Trkloose',            'L1_MU6',  [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['mu8_bJpsi_Trkloose',           'L1_MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    
    ['mu4_iloose_mu4_11invm60_noos', 'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['mu4_iloose_mu4_7invm9_noos', 'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['mu6_iloose_mu6_11invm24_noos', 'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['mu6_iloose_mu6_24invm60_noos', 'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['mu4_iloose_mu4_11invm60_noos_novtx', 'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['mu4_iloose_mu4_7invm9_noos_novtx', 'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['mu6_iloose_mu6_11invm60_noos_novtx', 'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['mu6_iloose_mu6_11invm24_noos_novtx', 'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['mu6_iloose_mu6_24invm60_noos_novtx', 'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
	
	
        ]

    TriggerFlags.CombinedSlice.signatures = [

        # inclusive UPC with tracking
        ['hi_upc_FgapA_mb_sptrk_L1RD0_FILLED',    'L1_RD0_FILLED',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapA', 'mb_sptrk'],False]],
        

        ['hi_upc_FgapC_mb_sptrk_L1RD0_FILLED',    'L1_RD0_FILLED',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapC', 'mb_sptrk'],False]],
        ['hi_upc_FgapAC_mb_sptrk_L1RD0_FILLED',    'L1_RD0_FILLED',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mb_sptrk'],False]],

        ['hi_upc_FgapA_mb_sptrk_L1MBTS_2_C',    'L1_MBTS_2_C',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapA', 'mb_sptrk'],False]],
        ['hi_upc_FgapC_mb_sptrk_L1MBTS_2_A',    'L1_MBTS_2_A',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapC', 'mb_sptrk'],False]],
        ['hi_upc_FgapAC_mb_sptrk_L1MBTS_2',    'L1_MBTS_2',   ['', ''], [UPCStream, 'express'],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mb_sptrk'],False]],

        ['hi_upc_FgapA_mb_sptrk_L1ZDC',    'L1_ZDC',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapA', 'mb_sptrk_L1ZDC'],False]],
        ['hi_upc_FgapC_mb_sptrk_L1ZDC',    'L1_ZDC',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapC', 'mb_sptrk_L1ZDC'],False]],
        ['hi_upc_FgapAC_mb_sptrk_L1ZDC',    'L1_ZDC',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mb_sptrk_L1ZDC'],False]],

        ['hi_upc_FgapAC_mb_sptrk_exclusiveloose_L1ZDC',        'L1_ZDC',        ['', ''], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mb_sptrk_exclusiveloose_L1ZDC'],False]],
        ['hi_upc_FgapAC_mb_sptrk_exclusiveloose',              'L1_RD0_FILLED', ['', ''], [UPCStream, 'express'],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mb_sptrk_exclusiveloose'],False]],
        ['hi_upc_FgapAC_mb_sptrk_exclusiveloose_L1VTE10_VZDC_A',     'L1_VTE10_VZDC_A',     ['', ''], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mb_sptrk_exclusiveloose'],False]],
        ['hi_upc_FgapAC_mb_sptrk_exclusiveloose_L1VTE10_VZDC_C',     'L1_VTE10_VZDC_C',     ['', ''], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mb_sptrk_exclusiveloose'],False]],

        ['hi_upc_FgapAC_mb_sptrk_exclusivetight_L1ZDC',        'L1_ZDC',        ['', ''], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mb_sptrk_exclusivetight_L1ZDC'],False]],
        ['hi_upc_FgapAC_mb_sptrk_exclusivetight',              'L1_RD0_FILLED', ['', ''], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mb_sptrk_exclusivetight'],False]],
        ['hi_upc_FgapAC_mb_sptrk_exclusivetight_L1VTE10_VZDC_A',     'L1_VTE10_VZDC_A',     ['', ''], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mb_sptrk_exclusivetight'],False]],
        ['hi_upc_FgapAC_mb_sptrk_exclusivetight_L1VTE10_VZDC_C',     'L1_VTE10_VZDC_C',     ['', ''], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mb_sptrk_exclusivetight'],False]],


        # UPC jets
        ['hi_upc_FgapA_j10_0eta490_L1RD0_FILLED',    'L1_RD0_FILLED',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j10_0eta490'],False]],
        ['hi_upc_FgapA_j10_0eta490_L1MBTS_2_C',        'L1_MBTS_2_C',   ['', ''], [UPCStream, 'express'],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j10_0eta490'],False]],


        ['hi_upc_FgapA_j10_0eta490_L1TE5',           'L1_TE5',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j10_0eta490_L1TE5'],False]],
        # Nils help here pls. ['hi_upc_j10_L1TE5_VZDC_A',           'L1_TE5_VZDC_A',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['j10_L1TE5_VZDC_A'],False]],

        ['hi_upc_FgapA_j10_L1J5',            'L1_J5',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j10_L1J5'],False]],
        ['hi_upc_FgapA_j10_0eta490_L1TE10',            'L1_TE10',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j10_0eta490_L1TE10'],False]],
        ['hi_upc_FgapA_j10_0eta490_L1TE20',           'L1_TE20',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j10_0eta490_L1TE20'],False]],
        ['hi_upc_FgapA_j10_0eta490_L1ZDC_C',         'L1_ZDC_C',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j10_0eta490'],False]],
        ['hi_upc_FgapA_j10_0eta490_L1ZDC',           'L1_ZDC',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j10_0eta490_L1ZDC'],False]],
		['hi_upc_FgapA_j20_L1J5',            'L1_J5',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j20_L1J5'],False]],
		['hi_upc_FgapA_j20_0eta490_L1TE10',            'L1_TE10',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j20_0eta490_L1TE10'],False]],
        ['hi_upc_FgapA_j20_0eta490_L1TE20',           'L1_TE20',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j20_0eta490_L1TE20'],False]],
        
        ['hi_upc_FgapA_j30_0eta490_L1TE20',           'L1_TE20',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j30_0eta490_L1TE20'],False]],
        ['hi_upc_FgapA_j30_L1J5',           'L1_J5',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j30_L1J5'],False]],
        
        ['hi_upc_FgapA_j40_L1J5',           'L1_J5',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j40_L1J5'],False]],
        ['hi_upc_FgapA_j40_L1J10',           'L1_J10',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j40_L1J10'],False]],
        
        ['hi_upc_FgapA_j60_L1J15',           'L1_J15',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j60_L1J15'],False]],
        ['hi_upc_FgapA_j60_L1J20',           'L1_J20',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j60_L1J20'],False]],
				
        ['hi_upc_L2FgapA_j10_0eta490_L1RD0_FILLED',    'L1_RD0_FILLED',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_L2FgapA', 'j10_0eta490'],False]],
        ['hi_upc_L2FgapA_j10_0eta490_L1MBTS_2_C',        'L1_MBTS_2_C',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_L2FgapA', 'j10_0eta490'],False]],
        ['hi_upc_EFFgapA_j10_0eta490_L1RD0_FILLED',    'L1_RD0_FILLED',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_EFFgapA', 'j10_0eta490'],False]],
        ['hi_upc_EFFgapA_j10_0eta490_L1MBTS_2_C',        'L1_MBTS_2_C',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_EFFgapA', 'j10_0eta490'],False]],

        ['hi_upc_FgapC_j10_0eta490_L1RD0_FILLED',    'L1_RD0_FILLED',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j10_0eta490'],False]],
        ['hi_upc_FgapC_j10_0eta490_L1MBTS_2_A',        'L1_MBTS_2_A',   ['', ''], [UPCStream, 'express'],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j10_0eta490'],False]],

        ['hi_upc_FgapC_j10_0eta490_L1TE5',           'L1_TE5',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j10_0eta490_L1TE5'],False]],
        ['hi_upc_FgapC_j10_L1J5',            'L1_J5',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j10_L1J5'],False]],
        ['hi_upc_FgapC_j10_0eta490_L1TE10',           'L1_TE10',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j10_0eta490_L1TE10'],False]],
        ['hi_upc_FgapC_j10_0eta490_L1TE20',           'L1_TE20',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j10_0eta490_L1TE20'],False]],
        ['hi_upc_FgapC_j10_0eta490_L1ZDC_A',         'L1_ZDC_A',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j10_0eta490'],False]],
        ['hi_upc_FgapC_j10_0eta490_L1ZDC',           'L1_ZDC',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j10_0eta490_L1ZDC'],False]],
		['hi_upc_FgapC_j20_L1J5',            'L1_J5',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j20_L1J5'],False]],
		['hi_upc_FgapC_j20_0eta490_L1TE10',           'L1_TE10',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j20_0eta490_L1TE10'],False]],
        ['hi_upc_FgapC_j20_0eta490_L1TE20',           'L1_TE20',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j20_0eta490_L1TE20'],False]],
        
        ['hi_upc_FgapC_j30_0eta490_L1TE20',           'L1_TE20',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j30_0eta490_L1TE20'],False]],
        ['hi_upc_FgapC_j30_L1J5',           'L1_J5',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j30_L1J5'],False]],
        
        ['hi_upc_FgapC_j40_L1J5',           'L1_J5',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j40_L1J5'],False]],
        ['hi_upc_FgapC_j40_L1J10',           'L1_J10',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j40_L1J10'],False]],
        
        ['hi_upc_FgapC_j60_L1J15',           'L1_J15',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j60_L1J15'],False]],
        ['hi_upc_FgapC_j60_L1J20',           'L1_J20',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j60_L1J20'],False]],
		
		
        ['hi_upc_L2FgapC_j10_0eta490_L1RD0_FILLED',    'L1_RD0_FILLED',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_L2FgapC', 'j10_0eta490'],False]],
        ['hi_upc_L2FgapC_j10_0eta490_L1MBTS_2_A',        'L1_MBTS_2_A',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_L2FgapC', 'j10_0eta490'],False]],
        ['hi_upc_EFFgapC_j10_0eta490_L1RD0_FILLED',    'L1_RD0_FILLED',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_EFFgapC', 'j10_0eta490'],False]],
        ['hi_upc_EFFgapC_j10_0eta490_L1MBTS_2_A',        'L1_MBTS_2_A',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_EFFgapC', 'j10_0eta490'],False]],

        # UPC muons
        ['hi_upc_FgapAC_mu4',    'L1_MU4',   ['', ''], [UPCStream, 'express'],["BW:UPC", "RATE:UPCMuons"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mu4'],False],],
        ['hi_upc_FgapAC_mu6',    'L1_MU6',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCMuons"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mu6'],False],],
        # do not knwo how to configure chain wiht 1 mu4 from L1 and another found by full scan
        ['hi_upc_FgapAC_2mu4',    'L1_2MU4',   ['', 'L1_2MU4', ''], [UPCStream],["BW:UPC", "RATE:UPCMuons"], 1, ['serial', -1, ['hi_upc_FgapAC', '2mu4'],False]],
        ['hi_upc_FgapAC_mu4_mu4noL1',    'L1_MU4',   ['', 'L1_MU4', ''], [UPCStream],["BW:UPC", "RATE:UPCMuons"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mu4', 'mu4noL1'],False]],
        #['hi_upc_FgapAC_mu4_mu4noL1',    'L1_MU4',   ['L1_MU4', ''], [UPCStream],["BW:UPC", "RATE:UPCMuons"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mu4_mu4noL1_L1MU4'],False]],
        ['hi_upc_FgapAC_mu4_L1MU4_VTE10',    'L1_MU4_VTE10',   ['', ''], [UPCStream, 'express'],["BW:UPC", "RATE:UPCMuons"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mu4'],False]],

        # UPC electrons
        ['hi_upc_FgapAC_e5_etcut',    'L1_EM3',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCElectrons"], 1, ['serial', -1, ['hi_upc_FgapAC', 'e5_etcut'],False]],
        ['hi_upc_FgapAC_e5_lhloose_nod0',    'L1_EM3',   ['', ''], [UPCStream, 'express'],["BW:UPC", "RATE:UPCElectrons"], 1, ['serial', -1, ['hi_upc_FgapAC', 'e5_lhloose_nod0'],False]],

        ['hi_upc_FgapAC_2e5_etcut',    'L1_2EM3',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCElectrons"], 1, ['serial', -1, ['hi_upc_FgapAC', '2e5_etcut'],False]],
        ['hi_upc_FgapAC_2e5_lhloose_nod0',    'L1_2EM3',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCElectrons"], 1, ['serial', -1, ['hi_upc_FgapAC', '2e5_lhloose_nod0'],False]],
        # UPC photons
        ['hi_upc_FgapA_g5_loose',    'L1_EM3',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCPhotons"], 1, ['serial', -1, ['hi_upc_FgapA', 'g5_loose'],False]],
        ['hi_upc_FgapC_g5_loose',    'L1_EM3',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCPhotons"], 1, ['serial', -1, ['hi_upc_FgapC', 'g5_loose'],False]],
        ['hi_upc_FgapA_g10_loose',    'L1_EM5',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCPhotons"], 1, ['serial', -1, ['hi_upc_FgapA', 'g10_loose'],False]],
        ['hi_upc_FgapC_g10_loose',    'L1_EM5',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCPhotons"], 1, ['serial', -1, ['hi_upc_FgapC', 'g10_loose'],False]],
        ['hi_upc_FgapA_g5_etcut',    'L1_EM3',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCPhotons"], 1, ['serial', -1, ['hi_upc_FgapA', 'g5_etcut'],False]],
        ['hi_upc_FgapC_g5_etcut',    'L1_EM3',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCPhotons"], 1, ['serial', -1, ['hi_upc_FgapC', 'g5_etcut'],False]],
        ['hi_upc_FgapA_g10_etcut',    'L1_EM5',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCPhotons"], 1, ['serial', -1, ['hi_upc_FgapA', 'g10_etcut'],False]],
        ['hi_upc_FgapC_g10_etcut',    'L1_EM5',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPCPhotons"], 1, ['serial', -1, ['hi_upc_FgapC', 'g10_etcut'],False]],

        # HMT +muon
        ['mu4_mb_sp1200_trk60_hmt_L1MU4_TE10.0ETA24', 'L1_MU4_TE10.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp1200_trk60_hmt'],False]],
		['mu4_mb_sp1600_trk80_hmt_L1MU4_TE15.0ETA24', 'L1_MU4_TE15.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp1600_trk80_hmt'],False]],        
		['mu4_mb_sp2100_trk100_hmt_L1MU4_TE15.0ETA24', 'L1_MU4_TE15.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp2100_trk100_hmt'],False]],
        ['mu4_mb_sp2400_trk120_hmt_L1MU4_TE15.0ETA24', 'L1_MU4_TE15.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp2400_trk120_hmt'],False]],
        ['mu4_mb_sp2800_trk140_hmt_L1MU4_TE50', 'L1_MU4_TE50',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp2800_trk140_hmt'],False]],
        ['mu4_mb_sp3100_trk160_hmt_L1MU4_TE20.0ETA24', 'L1_MU4_TE20.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp3100_trk160_hmt'],False]],
        ['mu4_mb_sp3500_trk180_hmt_L1MU4_TE70', 'L1_MU4_TE70',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp3500_trk180_hmt'],False]],
        ['mu4_mb_sp4100_trk200_hmt_L1MU4_TE90', 'L1_MU4_TE90',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp4100_trk200_hmt'],False]],
		['mu4_mb_sp4500_trk220_hmt_L1MU4_TE30.0ETA24', 'L1_MU4_TE30.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp4500_trk220_hmt'],False]],
        ['mu4_mb_sp4800_trk240_hmt_L1MU4_TE120', 'L1_MU4_TE120',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp4800_trk240_hmt'],False]],

		['mu4_mb_sp1200_trk60_hmt_L1MU4_TE15.0ETA24', 'L1_MU4_TE15.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp1200_trk60_hmt'],False]],
		['mu4_mb_sp1600_trk80_hmt_L1MU4_TE20.0ETA24', 'L1_MU4_TE20.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp1600_trk80_hmt'],False]],        
		['mu4_mb_sp2100_trk100_hmt_L1MU4_TE20.0ETA24', 'L1_MU4_TE20.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp2100_trk100_hmt'],False]],
        ['mu4_mb_sp2400_trk120_hmt_L1MU4_TE20.0ETA24', 'L1_MU4_TE20.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp2400_trk120_hmt'],False]],
        ['mu4_mb_sp2800_trk140_hmt_L1MU4_TE70', 'L1_MU4_TE70',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp2800_trk140_hmt'],False]],
        ['mu4_mb_sp3100_trk160_hmt_L1MU4_TE25.0ETA24', 'L1_MU4_TE25.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp3100_trk160_hmt'],False]],
        ['mu4_mb_sp3500_trk180_hmt_L1MU4_TE90', 'L1_MU4_TE90',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp3500_trk180_hmt'],False]],
        ['mu4_mb_sp4100_trk200_hmt_L1MU4_TE120', 'L1_MU4_TE120',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp4100_trk200_hmt'],False]],
		['mu4_mb_sp4500_trk220_hmt_L1MU4_TE35.0ETA24', 'L1_MU4_TE35.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp4500_trk220_hmt'],False]],
        ['mu4_mb_sp4800_trk240_hmt_L1MU4_TE160', 'L1_MU4_TE160',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp4800_trk240_hmt'],False]],

		['mu4_mb_sp1200_trk60_hmt_L1MU4_TE20.0ETA24', 'L1_MU4_TE20.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp1200_trk60_hmt'],False]],
		['mu4_mb_sp1600_trk80_hmt_L1MU4_TE25.0ETA24', 'L1_MU4_TE25.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp1600_trk80_hmt'],False]],        
		['mu4_mb_sp2100_trk100_hmt_L1MU4_TE25.0ETA24', 'L1_MU4_TE25.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp2100_trk100_hmt'],False]],
        ['mu4_mb_sp2400_trk120_hmt_L1MU4_TE25.0ETA24', 'L1_MU4_TE25.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp2400_trk120_hmt'],False]],
        ['mu4_mb_sp2800_trk140_hmt_L1MU4_TE90', 'L1_MU4_TE90',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp2800_trk140_hmt'],False]],
        ['mu4_mb_sp3100_trk160_hmt_L1MU4_TE30.0ETA24', 'L1_MU4_TE30.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp3100_trk160_hmt'],False]],
        ['mu4_mb_sp3500_trk180_hmt_L1MU4_TE120', 'L1_MU4_TE120',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp3500_trk180_hmt'],False]],
        ['mu4_mb_sp4100_trk200_hmt_L1MU4_TE160', 'L1_MU4_TE160',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp4100_trk200_hmt'],False]],
		['mu4_mb_sp4500_trk220_hmt_L1MU4_TE40.0ETA24', 'L1_MU4_TE40.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp4500_trk220_hmt'],False]],
        

		# HMT +muon + pileup suppresion
		['mu4_mb_sp1200_pusup200_trk60_hmt_L1MU4_TE10.0ETA24', 'L1_MU4_TE10.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp1200_pusup200_trk60_hmt'],False]],
		['mu4_mb_sp1600_pusup300_trk80_hmt_L1MU4_TE15.0ETA24', 'L1_MU4_TE15.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp1600_pusup300_trk80_hmt'],False]],        
		['mu4_mb_sp2100_pusup400_trk100_hmt_L1MU4_TE15.0ETA24', 'L1_MU4_TE15.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp2100_pusup400_trk100_hmt'],False]],
        ['mu4_mb_sp2400_pusup500_trk120_hmt_L1MU4_TE15.0ETA24', 'L1_MU4_TE15.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp2400_pusup500_trk120_hmt'],False]],
        ['mu4_mb_sp2800_pusup600_trk140_hmt_L1MU4_TE50', 'L1_MU4_TE50',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp2800_pusup600_trk140_hmt'],False]],
        ['mu4_mb_sp3100_pusup700_trk160_hmt_L1MU4_TE20.0ETA24', 'L1_MU4_TE20.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp3100_pusup700_trk160_hmt'],False]],
        ['mu4_mb_sp3500_pusup800_trk180_hmt_L1MU4_TE70', 'L1_MU4_TE70',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp3500_pusup800_trk180_hmt'],False]],
        ['mu4_mb_sp4100_pusup900_trk200_hmt_L1MU4_TE90', 'L1_MU4_TE90',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp4100_pusup900_trk200_hmt'],False]],
		['mu4_mb_sp4500_pusup1000_trk220_hmt_L1MU4_TE30.0ETA24', 'L1_MU4_TE30.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp4500_pusup1000_trk220_hmt'],False]],
        ['mu4_mb_sp4800_pusup1100_trk240_hmt_L1MU4_TE120', 'L1_MU4_TE120',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp4800_pusup1100_trk240_hmt'],False]],

		['mu4_mb_sp1200_pusup200_trk60_hmt_L1MU4_TE15.0ETA24', 'L1_MU4_TE15.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp1200_pusup200_trk60_hmt'],False]],
		['mu4_mb_sp1600_pusup300_trk80_hmt_L1MU4_TE20.0ETA24', 'L1_MU4_TE20.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp1600_pusup300_trk80_hmt'],False]],        
		['mu4_mb_sp2100_pusup400_trk100_hmt_L1MU4_TE20.0ETA24', 'L1_MU4_TE20.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp2100_pusup400_trk100_hmt'],False]],
        ['mu4_mb_sp2400_pusup500_trk120_hmt_L1MU4_TE20.0ETA24', 'L1_MU4_TE20.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp2400_pusup500_trk120_hmt'],False]],
        ['mu4_mb_sp2800_pusup600_trk140_hmt_L1MU4_TE70', 'L1_MU4_TE70',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp2800_pusup600_trk140_hmt'],False]],
        ['mu4_mb_sp3100_pusup700_trk160_hmt_L1MU4_TE25.0ETA24', 'L1_MU4_TE25.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp3100_pusup700_trk160_hmt'],False]],
        ['mu4_mb_sp3500_pusup800_trk180_hmt_L1MU4_TE90', 'L1_MU4_TE90',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp3500_pusup800_trk180_hmt'],False]],
        ['mu4_mb_sp4100_pusup900_trk200_hmt_L1MU4_TE120', 'L1_MU4_TE120',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp4100_pusup900_trk200_hmt'],False]],
		['mu4_mb_sp4500_pusup1000_trk220_hmt_L1MU4_TE35.0ETA24', 'L1_MU4_TE35.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp4500_pusup1000_trk220_hmt'],False]],
        ['mu4_mb_sp4800_pusup1100_trk240_hmt_L1MU4_TE160', 'L1_MU4_TE160',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp4800_pusup1100_trk240_hmt'],False]],

		['mu4_mb_sp1200_pusup200_trk60_hmt_L1MU4_TE20.0ETA24', 'L1_MU4_TE20.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp1200_pusup200_trk60_hmt'],False]],
		['mu4_mb_sp1600_pusup300_trk80_hmt_L1MU4_TE25.0ETA24', 'L1_MU4_TE25.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp1600_pusup300_trk80_hmt'],False]],        
		['mu4_mb_sp2100_pusup400_trk100_hmt_L1MU4_TE25.0ETA24', 'L1_MU4_TE25.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp2100_pusup400_trk100_hmt'],False]],
        ['mu4_mb_sp2400_pusup500_trk120_hmt_L1MU4_TE25.0ETA24', 'L1_MU4_TE25.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp2400_pusup500_trk120_hmt'],False]],
        ['mu4_mb_sp2800_pusup600_trk140_hmt_L1MU4_TE90', 'L1_MU4_TE90',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp2800_pusup600_trk140_hmt'],False]],
        ['mu4_mb_sp3100_pusup700_trk160_hmt_L1MU4_TE30.0ETA24', 'L1_MU4_TE30.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp3100_pusup700_trk160_hmt'],False]],
        ['mu4_mb_sp3500_pusup800_trk180_hmt_L1MU4_TE120', 'L1_MU4_TE120',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp3500_pusup800_trk180_hmt'],False]],
        ['mu4_mb_sp4100_pusup900_trk200_hmt_L1MU4_TE160', 'L1_MU4_TE160',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp4100_pusup900_trk200_hmt'],False]],
		['mu4_mb_sp4500_pusup1000_trk220_hmt_L1MU4_TE40.0ETA24', 'L1_MU4_TE40.0ETA24',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['mu4', 'mb_sp4500_pusup1000_trk220_hmt'],False]],
        
        # HMT + jets + pileup suppresion, j30 
        ['j30_mb_sp1200_trk60_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp1200_trk60_hmt'],False]],
		['j30_mb_sp1600_trk80_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp1600_trk80_hmt'],False]],        
		['j30_mb_sp2100_trk100_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp2100_trk100_hmt'],False]],
        ['j30_mb_sp2400_trk120_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp2400_trk120_hmt'],False]],
        ['j30_mb_sp2800_trk140_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp2800_trk140_hmt'],False]],
        ['j30_mb_sp3100_trk160_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp3100_trk160_hmt'],False]],
        ['j30_mb_sp3500_trk180_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp3500_trk180_hmt'],False]],
        ['j30_mb_sp4100_trk200_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp4100_trk200_hmt'],False]],
		['j30_mb_sp4500_trk220_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp4500_trk220_hmt'],False]],
        ['j30_mb_sp4800_trk240_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp4800_trk240_hmt'],False]],

		# HMT + jets + pileup suppresion, j30
		['j30_mb_sp1200_pusup200_trk60_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp1200_pusup200_trk60_hmt'],False]],
		['j30_mb_sp1600_pusup300_trk80_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp1600_pusup300_trk80_hmt'],False]],        
		['j30_mb_sp2100_pusup400_trk100_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp2100_pusup400_trk100_hmt'],False]],
        ['j30_mb_sp2400_pusup500_trk120_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp2400_pusup500_trk120_hmt'],False]],
        ['j30_mb_sp2800_pusup600_trk140_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp2800_pusup600_trk140_hmt'],False]],
        ['j30_mb_sp3100_pusup700_trk160_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp3100_pusup700_trk160_hmt'],False]],
        ['j30_mb_sp3500_pusup800_trk180_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp3500_pusup800_trk180_hmt'],False]],
        ['j30_mb_sp4100_pusup900_trk200_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp4100_pusup900_trk200_hmt'],False]],
		['j30_mb_sp4500_pusup1000_trk220_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp4500_pusup1000_trk220_hmt'],False]],
        ['j30_mb_sp4800_pusup1100_trk240_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30', 'mb_sp4800_pusup1100_trk240_hmt'],False]],
		
		#HMT +jets, j30 ion 
        ['j30_ion_mb_sp1200_trk60_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp1200_trk60_hmt'],False]],
		['j30_ion_mb_sp1600_trk80_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp1600_trk80_hmt'],False]],        
		['j30_ion_mb_sp2100_trk100_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp2100_trk100_hmt'],False]],
        ['j30_ion_mb_sp2400_trk120_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp2400_trk120_hmt'],False]],
        ['j30_ion_mb_sp2800_trk140_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp2800_trk140_hmt'],False]],
        ['j30_ion_mb_sp3100_trk160_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp3100_trk160_hmt'],False]],
        ['j30_ion_mb_sp3500_trk180_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp3500_trk180_hmt'],False]],
        ['j30_ion_mb_sp4100_trk200_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp4100_trk200_hmt'],False]],
		['j30_ion_mb_sp4500_trk220_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp4500_trk220_hmt'],False]],
        ['j30_ion_mb_sp4800_trk240_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp4800_trk240_hmt'],False]],

		# HMT + jets + pileup suppresion, j30 ion 
		['j30_ion_mb_sp1200_pusup200_trk60_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp1200_pusup200_trk60_hmt'],False]],
		['j30_ion_mb_sp1600_pusup300_trk80_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp1600_pusup300_trk80_hmt'],False]],        
		['j30_ion_mb_sp2100_pusup400_trk100_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp2100_pusup400_trk100_hmt'],False]],
        ['j30_ion_mb_sp2400_pusup500_trk120_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp2400_pusup500_trk120_hmt'],False]],
        ['j30_ion_mb_sp2800_pusup600_trk140_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp2800_pusup600_trk140_hmt'],False]],
        ['j30_ion_mb_sp3100_pusup700_trk160_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp3100_pusup700_trk160_hmt'],False]],
        ['j30_ion_mb_sp3500_pusup800_trk180_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp3500_pusup800_trk180_hmt'],False]],
        ['j30_ion_mb_sp4100_pusup900_trk200_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp4100_pusup900_trk200_hmt'],False]],
		['j30_ion_mb_sp4500_pusup1000_trk220_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp4500_pusup1000_trk220_hmt'],False]],
        ['j30_ion_mb_sp4800_pusup1100_trk240_hmt', 'L1_J5',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j30_ion', 'mb_sp4800_pusup1100_trk240_hmt'],False]],

        
        # HMT + jets + pileup suppresion, j40 
        ['j40_mb_sp1200_trk60_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp1200_trk60_hmt'],False]],
		['j40_mb_sp1600_trk80_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp1600_trk80_hmt'],False]],        
		['j40_mb_sp2100_trk100_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp2100_trk100_hmt'],False]],
        ['j40_mb_sp2400_trk120_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp2400_trk120_hmt'],False]],
        ['j40_mb_sp2800_trk140_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp2800_trk140_hmt'],False]],
        ['j40_mb_sp3100_trk160_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp3100_trk160_hmt'],False]],
        ['j40_mb_sp3500_trk180_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp3500_trk180_hmt'],False]],
        ['j40_mb_sp4100_trk200_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp4100_trk200_hmt'],False]],
		['j40_mb_sp4500_trk220_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp4500_trk220_hmt'],False]],
        ['j40_mb_sp4800_trk240_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp4800_trk240_hmt'],False]],

		# HMT + jets + pileup suppresion, j40
		['j40_mb_sp1200_pusup200_trk60_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp1200_pusup200_trk60_hmt'],False]],
		['j40_mb_sp1600_pusup300_trk80_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp1600_pusup300_trk80_hmt'],False]],        
		['j40_mb_sp2100_pusup400_trk100_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp2100_pusup400_trk100_hmt'],False]],
        ['j40_mb_sp2400_pusup500_trk120_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp2400_pusup500_trk120_hmt'],False]],
        ['j40_mb_sp2800_pusup600_trk140_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp2800_pusup600_trk140_hmt'],False]],
        ['j40_mb_sp3100_pusup700_trk160_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp3100_pusup700_trk160_hmt'],False]],
        ['j40_mb_sp3500_pusup800_trk180_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp3500_pusup800_trk180_hmt'],False]],
        ['j40_mb_sp4100_pusup900_trk200_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp4100_pusup900_trk200_hmt'],False]],
		['j40_mb_sp4500_pusup1000_trk220_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp4500_pusup1000_trk220_hmt'],False]],
        ['j40_mb_sp4800_pusup1100_trk240_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40', 'mb_sp4800_pusup1100_trk240_hmt'],False]],
		
		#HMT +jets, j40 ion 
        ['j40_ion_mb_sp1200_trk60_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp1200_trk60_hmt'],False]],
		['j40_ion_mb_sp1600_trk80_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp1600_trk80_hmt'],False]],        
		['j40_ion_mb_sp2100_trk100_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp2100_trk100_hmt'],False]],
        ['j40_ion_mb_sp2400_trk120_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp2400_trk120_hmt'],False]],
        ['j40_ion_mb_sp2800_trk140_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp2800_trk140_hmt'],False]],
        ['j40_ion_mb_sp3100_trk160_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp3100_trk160_hmt'],False]],
        ['j40_ion_mb_sp3500_trk180_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp3500_trk180_hmt'],False]],
        ['j40_ion_mb_sp4100_trk200_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp4100_trk200_hmt'],False]],
		['j40_ion_mb_sp4500_trk220_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp4500_trk220_hmt'],False]],
        ['j40_ion_mb_sp4800_trk240_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp4800_trk240_hmt'],False]],

		# HMT + jets + pileup suppresion, j40 ion 
		['j40_ion_mb_sp1200_pusup200_trk60_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp1200_pusup200_trk60_hmt'],False]],
		['j40_ion_mb_sp1600_pusup300_trk80_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp1600_pusup300_trk80_hmt'],False]],        
		['j40_ion_mb_sp2100_pusup400_trk100_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp2100_pusup400_trk100_hmt'],False]],
        ['j40_ion_mb_sp2400_pusup500_trk120_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp2400_pusup500_trk120_hmt'],False]],
        ['j40_ion_mb_sp2800_pusup600_trk140_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp2800_pusup600_trk140_hmt'],False]],
        ['j40_ion_mb_sp3100_pusup700_trk160_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp3100_pusup700_trk160_hmt'],False]],
        ['j40_ion_mb_sp3500_pusup800_trk180_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp3500_pusup800_trk180_hmt'],False]],
        ['j40_ion_mb_sp4100_pusup900_trk200_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp4100_pusup900_trk200_hmt'],False]],
		['j40_ion_mb_sp4500_pusup1000_trk220_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp4500_pusup1000_trk220_hmt'],False]],
        ['j40_ion_mb_sp4800_pusup1100_trk240_hmt', 'L1_J10',  ['', ''], [MinBiasStream],["BW:Muon", "RATE:SingleMuon"], 1, ['serial', -1, ['j40_ion', 'mb_sp4800_pusup1100_trk240_hmt'],False]],

		
        # EventShape + HMT
        ['hi_v2A_th05p_mb_sp2100_trk100_hmt_L1TE50', 'L1_TE50',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp2100_trk100_hmt_L1TE50'],False]],
        ['hi_v2A_th05p_mb_sp2100_trk100_hmt_L1TE70', 'L1_TE70',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp2100_trk100_hmt_L1TE70'],False]],
        ['hi_v2A_th05p_mb_sp2800_trk140_hmt_L1TE70', 'L1_TE70',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp2800_trk140_hmt_L1TE70'],False]],
        ['hi_v2A_th05p_mb_sp2400_trk120_hmt_L1TE90', 'L1_TE90',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp2400_trk120_hmt_L1TE90'],False]],
        ['hi_v2A_th05p_mb_sp3100_trk160_hmt_L1TE90', 'L1_TE90',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp3100_trk160_hmt_L1TE90'],False]],
        ['hi_v2A_th05p_mb_sp3100_trk160_hmt_L1TE120', 'L1_TE90',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp3100_trk160_hmt_L1TE120'],False]],
        ['hi_v2A_th05p_mb_sp4100_trk200_hmt_L1TE120', 'L1_TE120',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp4100_trk200_hmt_L1TE120'],False]],
        ['hi_v2A_th05p_mb_sp4100_trk200_hmt_L1TE160', 'L1_TE160',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp4100_trk200_hmt_L1TE160'],False]],
        ['hi_v2A_th05p_mb_sp2100_trk100_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp2100_trk100_hmt_L1TE10.0ETA24'],False]],
        ['hi_v2A_th05p_mb_sp2100_trk100_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp2100_trk100_hmt_L1TE20.0ETA24'],False]],
        ['hi_v2A_th05p_mb_sp2400_trk120_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp2400_trk120_hmt_L1TE30.0ETA24'],False]],
        ['hi_v2A_th05p_mb_sp3100_trk160_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp3100_trk160_hmt_L1TE30.0ETA24'],False]],
        ['hi_v2C_th05p_mb_sp2100_trk100_hmt_L1TE50', 'L1_TE50',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp2100_trk100_hmt_L1TE50'],False]],
        ['hi_v2C_th05p_mb_sp2100_trk100_hmt_L1TE70', 'L1_TE70',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp2100_trk100_hmt_L1TE70'],False]],
        ['hi_v2C_th05p_mb_sp2800_trk140_hmt_L1TE70', 'L1_TE70',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp2800_trk140_hmt_L1TE70'],False]],
        ['hi_v2C_th05p_mb_sp2400_trk120_hmt_L1TE90', 'L1_TE90',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp2400_trk120_hmt_L1TE90'],False]],
        ['hi_v2C_th05p_mb_sp3100_trk160_hmt_L1TE90', 'L1_TE90',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp3100_trk160_hmt_L1TE90'],False]],
        ['hi_v2C_th05p_mb_sp3100_trk160_hmt_L1TE120', 'L1_TE120',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp3100_trk160_hmt_L1TE120'],False]],
        ['hi_v2C_th05p_mb_sp4100_trk200_hmt_L1TE120', 'L1_TE120',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp4100_trk200_hmt_L1TE120'],False]],
        ['hi_v2C_th05p_mb_sp4100_trk200_hmt_L1TE160', 'L1_TE160',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp4100_trk200_hmt_L1TE160'],False]],
        ['hi_v2C_th05p_mb_sp2100_trk100_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp2100_trk100_hmt_L1TE10.0ETA24'],False]],
        ['hi_v2C_th05p_mb_sp2100_trk100_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp2100_trk100_hmt_L1TE20.0ETA24'],False]],
        ['hi_v2C_th05p_mb_sp2400_trk120_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp2400_trk120_hmt_L1TE30.0ETA24'],False]],
        ['hi_v2C_th05p_mb_sp3100_trk160_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp3100_trk160_hmt_L1TE30.0ETA24'],False]],
        ['hi_v2A_th05p_mb_sp2800_trk140_hmt_L1TE50', 'L1_TE50',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp2800_trk140_hmt_L1TE50'],False]],
        ['hi_v2A_th05p_mb_sp3500_trk180_hmt_L1TE70', 'L1_TE70',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp3500_trk180_hmt_L1TE70'],False]],
        ['hi_v2A_th05p_mb_sp4100_trk200_hmt_L1TE90', 'L1_TE90',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp4100_trk200_hmt_L1TE90'],False]],
        ['hi_v2A_th05p_mb_sp4800_trk240_hmt_L1TE120', 'L1_TE120',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp4800_trk240_hmt_L1TE120'],False]],
        ['hi_v2A_th05p_mb_sp5000_trk260_hmt_L1TE160', 'L1_TE160',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp5000_trk260_hmt_L1TE160'],False]],
        ['hi_v2A_th05p_mb_sp2400_trk120_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp2400_trk120_hmt_L1TE10.0ETA24'],False]],
        ['hi_v2A_th05p_mb_sp3100_trk160_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp3100_trk160_hmt_L1TE20.0ETA24'],False]],
        ['hi_v2A_th05p_mb_sp4500_trk220_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp4500_trk220_hmt_L1TE30.0ETA24'],False]],
        ['hi_v2C_th05p_mb_sp2800_trk140_hmt_L1TE50', 'L1_TE50',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp2800_trk140_hmt_L1TE50'],False]],
        ['hi_v2C_th05p_mb_sp3500_trk180_hmt_L1TE70', 'L1_TE70',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp3500_trk180_hmt_L1TE70'],False]],
        ['hi_v2C_th05p_mb_sp4100_trk200_hmt_L1TE90', 'L1_TE90',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp4100_trk200_hmt_L1TE90'],False]],
        ['hi_v2C_th05p_mb_sp4800_trk240_hmt_L1TE120', 'L1_TE120',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp4800_trk240_hmt_L1TE120'],False]],
        ['hi_v2C_th05p_mb_sp5000_trk260_hmt_L1TE160', 'L1_TE160',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp5000_trk260_hmt_L1TE160'],False]],
        ['hi_v2C_th05p_mb_sp2400_trk120_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp2400_trk120_hmt_L1TE10.0ETA24'],False]],
        ['hi_v2C_th05p_mb_sp3100_trk160_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp3100_trk160_hmt_L1TE20.0ETA24'],False]],
        ['hi_v2C_th05p_mb_sp4500_trk220_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp4500_trk220_hmt_L1TE30.0ETA24'],False]],

        # with pileup suppression
        ['hi_v2A_th05p_mb_sp2100_pusup400_trk100_hmt_L1TE50', 'L1_TE50',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp2100_pusup400_trk100_hmt_L1TE50'],False]],
        ['hi_v2A_th05p_mb_sp2100_pusup400_trk100_hmt_L1TE70', 'L1_TE70',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp2100_pusup400_trk100_hmt_L1TE70'],False]],
        ['hi_v2A_th05p_mb_sp2800_pusup600_trk140_hmt_L1TE70', 'L1_TE70',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp2800_pusup600_trk140_hmt_L1TE70'],False]],
        ['hi_v2A_th05p_mb_sp2400_pusup500_trk120_hmt_L1TE90', 'L1_TE90',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp2400_pusup500_trk120_hmt_L1TE90'],False]],
        ['hi_v2A_th05p_mb_sp3100_pusup700_trk160_hmt_L1TE90', 'L1_TE90',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp3100_pusup700_trk160_hmt_L1TE90'],False]],
        ['hi_v2A_th05p_mb_sp3100_pusup700_trk160_hmt_L1TE120', 'L1_TE120',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp3100_pusup700_trk160_hmt_L1TE120'],False]],
        ['hi_v2A_th05p_mb_sp4100_pusup900_trk200_hmt_L1TE120', 'L1_TE120',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp4100_pusup900_trk200_hmt_L1TE120'],False]],
        ['hi_v2A_th05p_mb_sp4100_pusup900_trk200_hmt_L1TE160', 'L1_TE160',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp4100_pusup900_trk200_hmt_L1TE160'],False]],
        ['hi_v2A_th05p_mb_sp2100_pusup400_trk100_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp2100_pusup400_trk100_hmt_L1TE10.0ETA24'],False]],
        ['hi_v2A_th05p_mb_sp2100_pusup400_trk100_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp2100_pusup400_trk100_hmt_L1TE20.0ETA24'],False]],
        ['hi_v2A_th05p_mb_sp2400_pusup500_trk120_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp2400_pusup500_trk120_hmt_L1TE30.0ETA24'],False]],
        ['hi_v2A_th05p_mb_sp3100_pusup700_trk160_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp3100_pusup700_trk160_hmt_L1TE30.0ETA24'],False]],
        ['hi_v2C_th05p_mb_sp2100_pusup400_trk100_hmt_L1TE50', 'L1_TE50',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp2100_pusup400_trk100_hmt_L1TE50'],False]],
        ['hi_v2C_th05p_mb_sp2100_pusup400_trk100_hmt_L1TE70', 'L1_TE70',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp2100_pusup400_trk100_hmt_L1TE70'],False]],
        ['hi_v2C_th05p_mb_sp2800_pusup600_trk140_hmt_L1TE70', 'L1_TE70',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp2800_pusup600_trk140_hmt_L1TE70'],False]],
        ['hi_v2C_th05p_mb_sp2400_pusup500_trk120_hmt_L1TE90', 'L1_TE90',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp2400_pusup500_trk120_hmt_L1TE90'],False]],
        ['hi_v2C_th05p_mb_sp3100_pusup700_trk160_hmt_L1TE90', 'L1_TE90',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp3100_pusup700_trk160_hmt_L1TE90'],False]],
        ['hi_v2C_th05p_mb_sp3100_pusup700_trk160_hmt_L1TE120', 'L1_TE120',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp3100_pusup700_trk160_hmt_L1TE120'],False]],
        ['hi_v2C_th05p_mb_sp4100_pusup900_trk200_hmt_L1TE120', 'L1_TE120',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp4100_pusup900_trk200_hmt_L1TE120'],False]],
        ['hi_v2C_th05p_mb_sp4100_pusup900_trk200_hmt_L1TE160', 'L1_TE160',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp4100_pusup900_trk200_hmt_L1TE160'],False]],
        ['hi_v2C_th05p_mb_sp2100_pusup400_trk100_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp2100_pusup400_trk100_hmt_L1TE10.0ETA24'],False]],
        ['hi_v2C_th05p_mb_sp2100_pusup400_trk100_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp2100_pusup400_trk100_hmt_L1TE20.0ETA24'],False]],
        ['hi_v2C_th05p_mb_sp2400_pusup500_trk120_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp2400_pusup500_trk120_hmt_L1TE30.0ETA24'],False]],
        ['hi_v2C_th05p_mb_sp3100_pusup700_trk160_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp3100_pusup700_trk160_hmt_L1TE30.0ETA24'],False]],
        ['hi_v2A_th05p_mb_sp2800_pusup600_trk140_hmt_L1TE50', 'L1_TE50',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp2800_pusup600_trk140_hmt_L1TE50'],False]],
        ['hi_v2A_th05p_mb_sp3500_pusup800_trk180_hmt_L1TE70', 'L1_TE70',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp3500_pusup800_trk180_hmt_L1TE70'],False]],
        ['hi_v2A_th05p_mb_sp4100_pusup900_trk200_hmt_L1TE90', 'L1_TE90',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp4100_pusup900_trk200_hmt_L1TE90'],False]],
        ['hi_v2A_th05p_mb_sp4800_pusup1100_trk240_hmt_L1TE120', 'L1_TE120',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp4800_pusup1100_trk240_hmt_L1TE120'],False]],
        ['hi_v2A_th05p_mb_sp5000_pusup1200_trk260_hmt_L1TE160', 'L1_TE160',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp5000_pusup1200_trk260_hmt_L1TE160'],False]],
        ['hi_v2A_th05p_mb_sp2400_pusup500_trk120_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp2400_pusup500_trk120_hmt_L1TE10.0ETA24'],False]],
        ['hi_v2A_th05p_mb_sp3100_pusup700_trk160_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp3100_pusup700_trk160_hmt_L1TE20.0ETA24'],False]],
        ['hi_v2A_th05p_mb_sp4500_pusup1000_trk220_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2A_th05p', 'mb_sp4500_pusup1000_trk220_hmt_L1TE30.0ETA24'],False]],
        ['hi_v2C_th05p_mb_sp2800_pusup600_trk140_hmt_L1TE50', 'L1_TE50',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp2800_pusup600_trk140_hmt_L1TE50'],False]],
        ['hi_v2C_th05p_mb_sp3500_pusup800_trk180_hmt_L1TE70', 'L1_TE70',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp3500_pusup800_trk180_hmt_L1TE70'],False]],
        ['hi_v2C_th05p_mb_sp4100_pusup900_trk200_hmt_L1TE90', 'L1_TE90',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp4100_pusup900_trk200_hmt_L1TE90'],False]],
        ['hi_v2C_th05p_mb_sp4800_pusup1100_trk240_hmt_L1TE120', 'L1_TE120',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp4800_pusup1100_trk240_hmt_L1TE120'],False]],
        ['hi_v2C_th05p_mb_sp5000_pusup1200_trk260_hmt_L1TE160', 'L1_TE160',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp5000_pusup1200_trk260_hmt_L1TE160'],False]],
        ['hi_v2C_th05p_mb_sp2400_pusup500_trk120_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp2400_pusup500_trk120_hmt_L1TE10.0ETA24'],False]],
        ['hi_v2C_th05p_mb_sp3100_pusup700_trk160_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp3100_pusup700_trk160_hmt_L1TE20.0ETA24'],False]],
        ['hi_v2C_th05p_mb_sp4500_pusup1000_trk220_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24',  ['', ''], [MinBiasStream],["BW:EventShape", "RATE:HMT"], 1, ['serial', -1, ['hi_v2C_th05p', 'mb_sp4500_pusup1000_trk220_hmt_L1TE30.0ETA24'],False]],
        


        # mu +jet
	['mu4_j20_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream, 'express'], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j20_ion'] ]],
	['mu4_j25_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j25_ion'] ]],
	['mu4_j30_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j30_ion'] ]],
	['mu4_j40_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j40_ion'] ]],
	['mu4_j50_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j50_ion'] ]],
	['mu4_j60_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream, 'express'], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j60_ion'] ]],

	['mu6_j20_ion_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j20_ion'] ]],
	['mu6_j25_ion_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j25_ion'] ]],
	['mu6_j30_ion_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream, 'express'], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j30_ion'] ]],

	['mu4_j20_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream, 'express'], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j20'] ]],
	['mu4_j25_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j25'] ]],
	['mu4_j30_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j30'] ]],
	['mu4_j40_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j40'] ]],
	['mu4_j50_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j50'] ]],
	['mu4_j60_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream, 'express'], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j60'] ]],

	['mu6_j20_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j20'] ]],
	['mu6_j25_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j25'] ]],
	['mu6_j30_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream, 'express'], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j30'] ]],



	#supporting triggers
	['mu4_j20_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j20_ion'] ]],
	['mu4_j25_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j25_ion'] ]],
	['mu4_j30_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j30_ion'] ]],
	['mu4_j40_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j40_ion'] ]],
	['mu4_j50_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j50_ion'] ]],
	['mu4_j60_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j60_ion'] ]],

	['mu6_j20_ion', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j20_ion'] ]],
	['mu6_j25_ion', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j25_ion'] ]],
	['mu6_j30_ion', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j30_ion'] ]],
    
    ['mu4_j20', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j20'] ]],
	['mu4_j25', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j25'] ]],
	['mu4_j30', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j30'] ]],
	['mu4_j40', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j40'] ]],
	['mu4_j50', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j50'] ]],
	['mu4_j60', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j60'] ]],

	['mu6_j20', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j20'] ]],
	['mu6_j25', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j25'] ]],
	['mu6_j30', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j30'] ]],
    
    
        ]
   
    TriggerFlags.HeavyIonSlice.signatures = [
	#event shape triggers
        # ['hi_v2A_th0p_L1TE50',     'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        # ['hi_v2A_th005p_L1TE50',   'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        # ['hi_v2A_th01p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        # ['hi_v2A_th025p_L1TE50',   'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        # ['hi_v2A_th05p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        # ['hi_v2A_th10p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        # ['hi_v2A_th15p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        # ['hi_v2A_th20p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],

        # ['hi_v2C_th0p_L1TE50',     'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        # ['hi_v2C_th005p_L1TE50',   'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        # ['hi_v2C_th01p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        # ['hi_v2C_th025p_L1TE50',   'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        # ['hi_v2C_th05p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        # ['hi_v2C_th10p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        # ['hi_v2C_th15p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        # ['hi_v2C_th20p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],

        ['hi_upc_FgapA_L1RD0',    'L1_RD0_FILLED',   [], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_FgapC_L1RD0',    'L1_RD0_FILLED',   [], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_FgapAC_L1RD0',   'L1_RD0_FILLED',   [], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1],

        ['hi_upc_FgapA_L1MBTS_1',    'L1_MBTS_1',   [], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_FgapC_L1MBTS_1',    'L1_MBTS_1',   [], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_FgapAC_L1MBTS_1',   'L1_MBTS_1',   [], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1],

        ['hi_upc_L2FgapA_L1MBTS_1',    'L1_MBTS_1',   [], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_L2FgapC_L1MBTS_1',    'L1_MBTS_1',   [], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_L2FgapAC_L1MBTS_1',   'L1_MBTS_1',   [], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1],

        ['hi_upc_EFFgapA_L1MBTS_1',    'L1_MBTS_1',   [], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_EFFgapC_L1MBTS_1',    'L1_MBTS_1',   [], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_EFFgapAC_L1MBTS_1',   'L1_MBTS_1',   [], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1],

        ['hi_upc_FgapA_L1ZDC',    'L1_ZDC',   [], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_FgapC_L1ZDC',    'L1_ZDC',   [], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_FgapAC_L1ZDC',    'L1_ZDC',   [], [UPCStream],["BW:UPC", "RATE:UPCInclusive"], 1],



        #['hi_v2_th5_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:UPC", "RATE:EventShape"], 1],
        #['hi_v3_th5_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        #['hi_v2_th10_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        #['hi_v3_th10_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        #['hi_v2_th13_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        #['hi_v3_th13_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        #['hi_v2_th14_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        #['hi_v3_th14_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        #['hi_v2_th15_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        #['hi_v3_th15_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        #['hi_v2_th16_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        #['hi_v3_th16_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        #['hi_v3_th13_veto2_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        #['hi_v2_th13_veto3_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        #['hi_v23_th14_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        #['hi_v23_th15_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
    
	#ultra-peripheral

        ]
    
    TriggerFlags.MinBiasSlice.signatures = [
        ['mb_mbts_L1MBTS_1', 'L1_MBTS_1', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_2',    'L1_MBTS_2',   [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_1_1',  'L1_MBTS_1_1', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1], 
        ['mb_mbts_L1MBTS_2_2',  'L1_MBTS_2_2', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_3_3',  'L1_MBTS_3_3', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_4_4',  'L1_MBTS_4_4', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],

		#sptrk
        ['mb_sp',  'L1_RD0_FILLED', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk',  'L1_RD0_FILLED', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp_L1MBTS_1',  'L1_MBTS_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_L1MBTS_1',  'L1_MBTS_1', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_L1MBTS_2',  'L1_MBTS_2', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_L1MBTS_1_1',  'L1_MBTS_1_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_L1ZDC_A',  'L1_ZDC_A', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_L1ZDC_C',  'L1_ZDC_C', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],

        # UPC. Loose requires 2 <= N <= 4 tracks, tight requires 2 <= N <= 5 tracks
        ['mb_sptrk_exclusiveloose_L1VTE10_VZDC_A',      'L1_VTE10_VZDC_A',     [], [UPCStream],["BW:UPC", "RATE:MinBias"], 1],
        ['mb_sptrk_exclusiveloose_L1VTE10_VZDC_C',      'L1_VTE10_VZDC_C',     [], [UPCStream],["BW:UPC", "RATE:MinBias"], 1],
        ['mb_sptrk_exclusiveloose_L1RD0_FILLED',  'L1_RD0_FILLED', [], [UPCStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_exclusiveloose_L1ZDC',         'L1_ZDC',        [], [UPCStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_exclusiveloose_L1MU4_VTE10',         'L1_MU4_VTE10',        [], [UPCStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_exclusivetight_L1VTE10_VZDC_A',      'L1_VTE10_VZDC_A',     [], [UPCStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_exclusivetight_L1VTE10_VZDC_C',      'L1_VTE10_VZDC_C',     [], [UPCStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_exclusivetight_L1RD0_FILLED',  'L1_RD0_FILLED', [], [UPCStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_exclusivetight_L1ZDC',         'L1_ZDC',        [], [UPCStream],["BW:MinBias", "RATE:MinBias"], 1],


        ##['mb_sptrk_ion_L1ZDC_A_C_VTE50',  'L1_ZDC_A_C_VTE50', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        ##['mb_sptrk_ion_L1MBTS_1_1_VTE50', 'L1_MBTS_1_1_VTE50', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        ##['mb_idperf_ion_L1MBTS_1', 'L1_MBTS_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ##['mb_idperf_ion_L1MBTS_1_1', 'L1_MBTS_1_1', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        
        ['mb_perf_L1RD1_FILLED',    'L1_RD1_FILLED',   [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_perf_L1RD1_EMPTY',    'L1_RD1_EMPTY',   [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_perf_L1MBTS_2',             'L1_MBTS_2', [], [MinBiasStream, 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
        #        ['mb_idperf_ion_L1MBTS_1',    'L1_MBTS_1',   [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        #        ['mb_idperf_ion_L1MBTS_1_1',    'L1_MBTS_1_1',   [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],

		#overlay
        ['mb_sp_L1RD0_FILLED_OVERLAY',  'L1_RD0_FILLED_OVERLAY', [], ['MinBiasOverlay'],["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
        ['mb_sptrk_L1RD0_FILLED_OVERLAY',  'L1_RD0_FILLED_OVERLAY', [], ['MinBiasOverlay'],["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
        ['mb_sp_L1MBTS_1_OVERLAY',  'L1_MBTS_1_OVERLAY', [], ['MinBiasOverlay'],["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
        ['mb_sptrk_L1MBTS_1_OVERLAY',  'L1_MBTS_1_OVERLAY', [], ['MinBiasOverlay'],["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],


        #Zdc
	['mb_lg_zdcperf_L1All',  '', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_lg_zdcperf_L1ZDC_A_C',  'L1_ZDC_A_C', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_hg_zdcperf_L1All',  '', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_hg_zdcperf_L1ZDC_A_C',  'L1_ZDC_A_C', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        
        #HMT
        ['mb_sp2100_trk100_hmt_L1TE50', 'L1_TE50', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2100_pusup400_trk100_hmt_L1TE50', 'L1_TE50', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2400_trk120_hmt_L1TE50', 'L1_TE50', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2400_pusup500_trk120_hmt_L1TE50', 'L1_TE50', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2800_trk140_hmt_L1TE50', 'L1_TE50', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2800_pusup600_trk140_hmt_L1TE50', 'L1_TE50', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3100_trk160_hmt_L1TE50', 'L1_TE50', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3100_pusup700_trk160_hmt_L1TE50', 'L1_TE50', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3500_trk180_hmt_L1TE50', 'L1_TE50', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3500_pusup800_trk180_hmt_L1TE50', 'L1_TE50', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2800_trk140_hmt_L1TE70', 'L1_TE70', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2800_pusup600_trk140_hmt_L1TE70', 'L1_TE70', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3100_trk160_hmt_L1TE70', 'L1_TE70', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3100_pusup700_trk160_hmt_L1TE70', 'L1_TE70', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3500_trk180_hmt_L1TE70', 'L1_TE70', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3500_pusup800_trk180_hmt_L1TE70', 'L1_TE70', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4100_trk200_hmt_L1TE70', 'L1_TE70', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4100_pusup900_trk200_hmt_L1TE70', 'L1_TE70', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4500_trk220_hmt_L1TE70', 'L1_TE70', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4500_pusup1000_trk220_hmt_L1TE70', 'L1_TE70', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3100_trk160_hmt_L1TE90', 'L1_TE90', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3100_pusup700_trk160_hmt_L1TE90', 'L1_TE90', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3500_trk180_hmt_L1TE90', 'L1_TE90', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3500_pusup800_trk180_hmt_L1TE90', 'L1_TE90', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4100_trk200_hmt_L1TE90', 'L1_TE90', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4100_pusup900_trk200_hmt_L1TE90', 'L1_TE90', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4500_trk220_hmt_L1TE90', 'L1_TE90', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4500_pusup1000_trk220_hmt_L1TE90', 'L1_TE90', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4800_trk240_hmt_L1TE90', 'L1_TE90', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4800_pusup1100_trk240_hmt_L1TE90', 'L1_TE90', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4100_trk200_hmt_L1TE120', 'L1_TE120', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4100_pusup900_trk200_hmt_L1TE120', 'L1_TE120', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4500_trk220_hmt_L1TE120', 'L1_TE120', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4500_pusup1000_trk220_hmt_L1TE120', 'L1_TE120', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4800_trk240_hmt_L1TE120', 'L1_TE120', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4800_pusup1100_trk240_hmt_L1TE120', 'L1_TE120', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5000_trk260_hmt_L1TE120', 'L1_TE120', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5000_pusup1200_trk260_hmt_L1TE120', 'L1_TE120', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5200_trk280_hmt_L1TE120', 'L1_TE120', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5200_pusup1300_trk280_hmt_L1TE120', 'L1_TE120', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4500_trk220_hmt_L1TE160', 'L1_TE160', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4500_pusup1000_trk220_hmt_L1TE160', 'L1_TE160', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4800_trk240_hmt_L1TE160', 'L1_TE160', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4800_pusup1100_trk240_hmt_L1TE160', 'L1_TE160', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5000_trk260_hmt_L1TE160', 'L1_TE160', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5000_pusup1200_trk260_hmt_L1TE160', 'L1_TE160', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5200_trk280_hmt_L1TE160', 'L1_TE160', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5200_pusup1300_trk280_hmt_L1TE160', 'L1_TE160', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		
		['mb_sp2100_trk100_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2100_pusup400_trk100_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2400_trk120_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2400_pusup500_trk120_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2800_trk140_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2800_pusup600_trk140_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3100_trk160_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3100_pusup700_trk160_hmt_L1TE10.0ETA24', 'L1_TE10.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2100_trk100_hmt_L1TE15.0ETA24', 'L1_TE15.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2100_pusup400_trk100_hmt_L1TE15.0ETA24', 'L1_TE15.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2400_trk120_hmt_L1TE15.0ETA24', 'L1_TE15.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2400_pusup500_trk120_hmt_L1TE15.0ETA24', 'L1_TE15.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2800_trk140_hmt_L1TE15.0ETA24', 'L1_TE15.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2800_pusup600_trk140_hmt_L1TE15.0ETA24', 'L1_TE15.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3100_trk160_hmt_L1TE15.0ETA24', 'L1_TE15.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3100_pusup700_trk160_hmt_L1TE15.0ETA24', 'L1_TE15.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3500_trk180_hmt_L1TE15.0ETA24', 'L1_TE15.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3500_pusup800_trk180_hmt_L1TE15.0ETA24', 'L1_TE15.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2400_trk120_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2400_pusup500_trk120_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2800_trk140_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2800_pusup600_trk140_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3100_trk160_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3100_pusup700_trk160_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3500_trk180_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3500_pusup800_trk180_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4100_trk200_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4100_pusup900_trk200_hmt_L1TE20.0ETA24', 'L1_TE20.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2800_trk140_hmt_L1TE25.0ETA24', 'L1_TE25.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp2800_pusup600_trk140_hmt_L1TE25.0ETA24', 'L1_TE25.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3100_trk160_hmt_L1TE25.0ETA24', 'L1_TE25.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3100_pusup700_trk160_hmt_L1TE25.0ETA24', 'L1_TE25.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3500_trk180_hmt_L1TE25.0ETA24', 'L1_TE25.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3500_pusup800_trk180_hmt_L1TE25.0ETA24', 'L1_TE25.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4100_trk200_hmt_L1TE25.0ETA24', 'L1_TE25.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4100_pusup900_trk200_hmt_L1TE25.0ETA24', 'L1_TE25.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4500_trk220_hmt_L1TE25.0ETA24', 'L1_TE25.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4500_pusup1000_trk220_hmt_L1TE25.0ETA24', 'L1_TE25.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3500_trk180_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp3500_pusup800_trk180_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4100_trk200_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4100_pusup900_trk200_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4500_trk220_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4500_pusup1000_trk220_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4800_trk240_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4800_pusup1100_trk240_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5000_trk260_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5000_pusup1200_trk260_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4100_trk200_hmt_L1TE35.0ETA24', 'L1_TE35.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4100_pusup900_trk200_hmt_L1TE35.0ETA24', 'L1_TE35.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4500_trk220_hmt_L1TE35.0ETA24', 'L1_TE35.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4500_pusup1000_trk220_hmt_L1TE35.0ETA24', 'L1_TE35.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4800_trk240_hmt_L1TE35.0ETA24', 'L1_TE35.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4800_pusup1100_trk240_hmt_L1TE35.0ETA24', 'L1_TE35.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5000_trk260_hmt_L1TE35.0ETA24', 'L1_TE35.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5000_pusup1200_trk260_hmt_L1TE35.0ETA24', 'L1_TE35.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5200_trk280_hmt_L1TE35.0ETA24', 'L1_TE35.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5200_pusup1300_trk280_hmt_L1TE35.0ETA24', 'L1_TE35.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4500_trk220_hmt_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4500_pusup1000_trk220_hmt_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4800_trk240_hmt_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4800_pusup1100_trk240_hmt_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5000_trk260_hmt_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5000_pusup1200_trk260_hmt_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5200_trk280_hmt_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5200_pusup1300_trk280_hmt_L1TE40.0ETA24', 'L1_TE40.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4500_trk220_hmt_L1TE45.0ETA24', 'L1_TE45.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4500_pusup1000_trk220_hmt_L1TE45.0ETA24', 'L1_TE45.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4800_trk240_hmt_L1TE45.0ETA24', 'L1_TE45.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp4800_pusup1100_trk240_hmt_L1TE45.0ETA24', 'L1_TE45.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5000_trk260_hmt_L1TE45.0ETA24', 'L1_TE45.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5000_pusup1200_trk260_hmt_L1TE45.0ETA24', 'L1_TE45.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5200_trk280_hmt_L1TE45.0ETA24', 'L1_TE45.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
		['mb_sp5200_pusup1300_trk280_hmt_L1TE45.0ETA24', 'L1_TE45.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        
		
        #performance
        
        ['mb_sp100_trk10_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp100_trk20_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp100_trk30_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp100_trk40_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp100_trk50_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp100_trk60_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp100_trk70_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp100_trk80_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp100_trk90_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp100_trk100_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp100_trk110_hmt_L1MBTS_1_1', 'L1_MBTS_1_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        
        ['mb_sp1800_hmtperf_L1TE5', 'L1_TE5', [], ['MinBias', 'express'], ["BW:MinBias", "RATE:MinBias"], -1],
        ]

    TriggerFlags.CalibSlice.signatures   = [

        ['lhcfpeb',       'L1_LHCF',       [], ['IDFwd'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lhcfpeb_L1LHCF_UNPAIRED_ISO',       'L1_LHCF_UNPAIRED_ISO',       [], ['IDFwd'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lhcfpeb_L1LHCF_EMPTY',       'L1_LHCF_EMPTY',       [], ['IDFwd'], ["RATE:Calibration", "BW:Detector"], -1],

        ['lumipeb_L1RD0_EMPTY',          'L1_RD0_EMPTY',        [], ['PixelNoise'], ["RATE:Calibration", "RATE:PixelCalibration", "BW:Detector"], -1],

        ['lumipeb_L1RD0_FILLED',            'L1_RD0_FILLED',            [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        ['lumipeb_L1RD0_UNPAIRED_ISO',      'L1_RD0_UNPAIRED_ISO',      [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        ['lumipeb_L1MBTS_1',                'L1_MBTS_1',                [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1], 
        ['lumipeb_L1MBTS_1_UNPAIRED_ISO',   'L1_MBTS_1_UNPAIRED_ISO',   [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        ['lumipeb_L1MBTS_2',                'L1_MBTS_2',                [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1], 
        ['lumipeb_L1MBTS_2_UNPAIRED_ISO',   'L1_MBTS_2_UNPAIRED_ISO',   [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        
        # triggers for VdM scan
        ['lumipeb_vdm_L1MBTS_2_BGRP11',       'L1_MBTS_2_BGRP11',       [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2_BGRP9',        'L1_MBTS_2_BGRP9',        [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1RD0_BGRP9',           'L1_RD0_BGRP9',           [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_1_1_BGRP9',           'L1_MBTS_1_1_BGRP9',           [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1ZDC_A_BGRP9',           'L1_ZDC_A_BGRP9',           [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1ZDC_C_BGRP9',           'L1_ZDC_C_BGRP9',           [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_1_UNPAIRED_ISO', 'L1_MBTS_1_UNPAIRED_ISO', [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2_UNPAIRED_ISO', 'L1_MBTS_2_UNPAIRED_ISO', [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1RD0_UNPAIRED_ISO',    'L1_RD0_UNPAIRED_ISO',    [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        
        ['lumipeb_vdm_L1MBTS_1', 'L1_MBTS_1', [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2', 'L1_MBTS_2', [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1RD0_FILLED',         'L1_RD0_FILLED', [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        

        ['lumipeb_L1RD0_ABORTGAPNOTCALIB',    'L1_RD0_ABORTGAPNOTCALIB',    [], ['PixelBeam'], ["RATE:Calibration", "BW:Detector"], -1],
		
        ['larnoiseburst_loose_L1All',             'L1_All',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1All',             'L1_All',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1XE35',            'L1_XE35',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1XE50',            'L1_XE50',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J50',             'L1_J50',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J75',             'L1_J75',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J40_XE50',            'L1_J40_XE50',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J40_XE60',            'L1_J40_XE60',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        
        ['larnoiseburst_L1XE80', 'L1_XE80', [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J100', 'L1_J100', [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],

        #Special re-run chains
        ['larnoiseburst_rerun',                  '',             [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_loose_rerun',            '',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        

        ['l1calocalib',             'L1_RD0_FILLED',     [], ['L1CaloCalib'], ["RATE:Calibration", "RATE:L1CaloCalib", "BW:Detector"], -1],
        
        
        ['larcalib_L1EM3_EMPTY',    'L1_EM7_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1EM7_EMPTY',    'L1_EM7_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1TAU8_EMPTY',   'L1_TAU8_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1J12_EMPTY',    'L1_J12_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1J3031ETA49_EMPTY',  'L1_J30.31ETA49_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],

        #['tilelarcalib_L1EM3_EMPTY',     'L1_EM3_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        #['tilelarcalib_L1TAU8_EMPTY',    'L1_TAU8_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        #['tilelarcalib_L1J12_EMPTY',     'L1_J12_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        #['tilelarcalib_L1J3031ETA49_EMPTY',   'L1_J30.31ETA49_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        
        #
        ['ibllumi_L1RD0_ABORTGAPNOTCALIB',    'L1_RD0_ABORTGAPNOTCALIB', [], ['IBLLumi'], [ 'RATE:IBLLumi','BW:Detector'], -1],
        ['ibllumi_L1RD0_FILLED',    'L1_RD0_FILLED', [], ['IBLLumi'], [ 'RATE:IBLLumi','BW:Detector'], -1],
        ['ibllumi_L1RD0_UNPAIRED_ISO',    'L1_RD0_UNPAIRED_ISO', [], ['IBLLumi'], [ 'RATE:IBLLumi','BW:Detector'], -1],

        #
        # ['idcalib_trk9_central',    'L1_TAU', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        # ['idcalib_trk9_fwd',        'L1_TAU', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        # ['idcalib_trk16_central',   'L1_TAU', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        # ['idcalib_trk16_fwd',       'L1_TAU', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        # ['idcalib_trk29_central',   'L1_TAU', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        # ['idcalib_trk29_fwd',       'L1_TAU', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk9_central_bs',    'L1_BS', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk9_fwd_bs',    'L1_BS', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],

        ['conej40_larpebj',  'L1_J12',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['conej165_larpebj', 'L1_J75',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1], 
        ['conej75_320eta490_larpebj',  'L1_J30.31ETA49',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['conej140_320eta490_larpebj', 'L1_J75.31ETA49',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
    
        #***Available chains copied from HI_v3 menu not present in pp_v6
        
        ['lumipeb_L1MBTS_1_1',    'L1_MBTS_1_1',    [], ['PixelBeam'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_L1MBTS_1_1_UNPAIRED_ISO',    'L1_MBTS_1_1_UNPAIRED_ISO',    [], ['PixelBeam'], ["RATE:Calibration", "BW:Detector"], -1],

        # triggers for VdM scan
        ['lumipeb_vdm_L1MBTS_1_1_UNPAIRED_ISO', 'L1_MBTS_1_1_UNPAIRED_ISO', [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1ZDC_A_C_UNPAIRED_ISO',  'L1_ZDC_A_C_UNPAIRED_ISO',  [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_1_1',              'L1_MBTS_1_1',              [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1ZDC_A_C',               'L1_ZDC_A_C',               [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1ZDC_A',               'L1_ZDC_A',               [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1ZDC_C',               'L1_ZDC_C',               [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
		['lumipeb_L1ZDC_A_UNPAIRED_ISO','L1_ZDC_A_UNPAIRED_ISO', [], ['VdM'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['lumipeb_L1ZDC_C_UNPAIRED_ISO','L1_ZDC_C_UNPAIRED_ISO', [], ['VdM'], ["BW:MinBias", "RATE:MinBias"], 1],

		
        # larpeb
        ['g8_loose_larpeb',                      'L1_EM7',     [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g12_loose_larpeb',                     'L1_EM10',     [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_loose_larpeb_L1EM15',              'L1_EM15', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g40_loose_larpeb',                     'L1_EM20VHI', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g60_loose_larpeb',                     'L1_EM20VHI', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g80_loose_larpeb',                     'L1_EM20VHI', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1], 
        
        
        ]
    
    TriggerFlags.CosmicSlice.signatures  = [        
        ['tilecalib_laser',   'L1_CALREQ2', [], ['Tile'], ["RATE:Calibration", "RATE:Cosmic_TileCalibration", "BW:Detector"], -1],
        #['pixel_noise',        'L1_RD0_EMPTY', [], ['PixelNoise'], ["RATE:Calibration", "RATE:PixelCalibration", "BW:Detector"], -1],
        #['pixel_beam',         'L1_RD0_FILLED', [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        ['sct_noise',          'L1_RD0_EMPTY', [], ['SCTNoise'], ["RATE:Calibration", "RATE:SCTCalibration", "BW:Detector"], -1],
        
        # CosmicCalo
        ['larps_L1EM3_EMPTY',  'L1_EM3_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1EM7_EMPTY',  'L1_EM7_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1EM7_FIRSTEMPTY',  'L1_EM7_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J12_EMPTY',  'L1_J12_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J30_EMPTY',  'L1_J30_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
	
        ['larps_L1J3031ETA49_EMPTY', 'L1_J30.31ETA49_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        #JetTauEtmiss 
        ['larps_L1J12_FIRSTEMPTY', 'L1_J12_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J30_FIRSTEMPTY', 'L1_J30_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1TAU8_EMPTY', 'L1_TAU8_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],

        ['larhec_L1J12_FIRSTEMPTY', 'L1_J12_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larhec_L1EM7_FIRSTEMPTY', 'L1_EM7_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larhec_L1TAU8_FIRSTEMPTY', 'L1_TAU8_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
#        ['larhec_L1RD0_FIRSTEMPTY', 'L1_RD0_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larhec_L1J30_FIRSTEMPTY', 'L1_J30_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        #CosmicID
        ['id_cosmicid_L1MU4_EMPTY',         'L1_MU4_EMPTY', [],  ['HLT_IDCosmic',  'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],

        #['id_cosmicid',               '', [], ['HLT_IDCosmic',  'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],
        #['id_cosmicid_trtxk',         '', [], ['HLT_IDCosmic'],            ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],
        #['id_cosmicid_trtxk_central', '', [], ['HLT_IDCosmic'],            ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],

        # ['id_cosmicid_ds',            '', [], ['DataScouting_04_IDCosmic'], ['RATE:CosmicSlice', 'BW:Detector'], -1],    
        ]
    TriggerFlags.StreamingSlice.signatures = [
	#TE full calo
	['noalg_mb_L1TE5',              'L1_TE5', [], [MinBiasStream, 'express'], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE10',      'L1_TE10', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE20',      'L1_TE20', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE50',      'L1_TE50', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE70',      'L1_TE70', [], [MinBiasStream, 'express'], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE90',      'L1_TE90', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE120',      'L1_TE120', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE160',      'L1_TE160', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    
    
    #TE restricted calo
    ['noalg_mb_L1TE10.0ETA24',      'L1_TE10.0ETA24', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE15.0ETA24',      'L1_TE15.0ETA24', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE20.0ETA24',      'L1_TE20.0ETA24', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE25.0ETA24',      'L1_TE25.0ETA24', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE30.0ETA24',      'L1_TE30.0ETA24', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE35.0ETA24',      'L1_TE35.0ETA24', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE40.0ETA24',      'L1_TE40.0ETA24', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE45.0ETA24',      'L1_TE45.0ETA24', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],


	#min bias
        ['noalg_mb_L1MBTS_1',    'L1_MBTS_1', [], [MinBiasStream, 'express'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1MBTS_2',    'L1_MBTS_2', [], [MinBiasStream, 'express'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1MBTS_1_1',  'L1_MBTS_1_1', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1MBTS_2_2',  'L1_MBTS_2_2', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],

	['noalg_mb_L1ZDC_A_C',   'L1_ZDC_A_C', [], [MinBiasStream, 'express'], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1ZDC_A',     'L1_ZDC_A', [], [MinBiasStream, 'express'], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1ZDC_C',     'L1_ZDC_C', [], [MinBiasStream, 'express'], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1ZDC',       'L1_ZDC', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],

	['noalg_mb_L1ZDC_AND',   'L1_ZDC_AND', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
        ['zdcpeb_L1ZDC_A',      'L1_ZDC_A', [], ['zdcCalib'], ["BW:Detector", "RATE:Calibration"], -1],
        ['zdcpeb_L1ZDC_C',      'L1_ZDC_C', [], ['zdcCalib'], ["BW:Detector", "RATE:Calibration"], -1],
        ['zdcpeb_L1ZDC_AND',    'L1_ZDC_AND', [], ['zdcCalib'], ["BW:Detector", "RATE:Calibration"], -1],
        ['zdcpeb_L1ZDC_A_C',    'L1_ZDC_A_C', [], ['zdcCalib'], ["BW:Detector", "RATE:Calibration"], -1],
        ['zdcpeb_L1ZDC',    'L1_ZDC', [], ['zdcCalib'], ["BW:Detector", "RATE:Calibration"], -1],
        ['zdcpeb_L1ZDC_A_EMPTY',      'L1_ZDC_A_EMPTY', [], ['zdcCalib'], ["BW:Detector", "RATE:Calibration"], -1],
        ['zdcpeb_L1ZDC_C_EMPTY',      'L1_ZDC_C_EMPTY', [], ['zdcCalib'], ["BW:Detector", "RATE:Calibration"], -1],
        
        ['zdcpeb_L1ZDC_EMPTY',              'L1_ZDC_EMPTY', [], ['zdcCalib'], ["BW:MinBias", "RATE:MinBias"], 1],
		['zdcpeb_L1ZDC_UNPAIRED_ISO',       'L1_ZDC_UNPAIRED_ISO', [], ['zdcCalib'], ["BW:MinBias", "RATE:MinBias"], 1],
		['zdcpeb_L1ZDC_UNPAIRED_NONISO',       'L1_ZDC_UNPAIRED_NONISO', [], ['zdcCalib'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['zdcpeb_L1ZDC_A_UNPAIRED_ISO','L1_ZDC_A_UNPAIRED_ISO', [], ['zdcCalib'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['zdcpeb_L1ZDC_C_UNPAIRED_ISO','L1_ZDC_C_UNPAIRED_ISO', [], ['zdcCalib'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['zdcpeb_L1ZDC_A_UNPAIRED_NONISO','L1_ZDC_A_UNPAIRED_NONISO', [], ['zdcCalib'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['zdcpeb_L1ZDC_C_UNPAIRED_NONISO','L1_ZDC_C_UNPAIRED_NONISO', [], ['zdcCalib'], ["BW:MinBias", "RATE:MinBias"], 1],

	['noalg_mb_L1RD0_FILLED',      'L1_RD0_FILLED', [], [MinBiasStream, 'express'], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1RD0_EMPTY',       'L1_RD0_EMPTY', [], [MinBiasStream, 'express'], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1RD0_UNPAIRED_ISO','L1_RD0_UNPAIRED_ISO', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['mb_sptrk_L1RD0_EMPTY',       'L1_RD0_EMPTY', [], [MinBiasStream, 'express'], ["BW:MinBias", "RATE:MinBias"], 1],
	['mb_sptrk_L1RD0_UNPAIRED_ISO','L1_RD0_UNPAIRED_ISO', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
 

	['noalg_L1LHCF_EMPTY',              'L1_LHCF_EMPTY', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_L1LHCF_UNPAIRED_ISO',       'L1_LHCF_UNPAIRED_ISO', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],

	['noalg_L1ZDC_EMPTY',              'L1_ZDC_EMPTY', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_L1ZDC_UNPAIRED_ISO',       'L1_ZDC_UNPAIRED_ISO', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_L1ZDC_UNPAIRED_NONISO',       'L1_ZDC_UNPAIRED_NONISO', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	
	['noalg_L1ZDC_A_UNPAIRED_ISO','L1_ZDC_A_UNPAIRED_ISO', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_L1ZDC_C_UNPAIRED_ISO','L1_ZDC_C_UNPAIRED_ISO', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_L1ZDC_A_UNPAIRED_NONISO','L1_ZDC_A_UNPAIRED_NONISO', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_L1ZDC_C_UNPAIRED_NONISO','L1_ZDC_C_UNPAIRED_NONISO', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	
	['noalg_L1ZDC_A_EMPTY',      'L1_ZDC_A_EMPTY', [], [MinBiasStream], ["BW:Detector", "RATE:Calibration"], -1],
    ['noalg_L1ZDC_C_EMPTY',      'L1_ZDC_C_EMPTY', [], [MinBiasStream], ["BW:Detector", "RATE:Calibration"], -1],

	['noalg_mb_L1LUCID',              'L1_LUCID', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1LUCID_EMPTY',        'L1_LUCID_EMPTY', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1LUCID_UNPAIRED_ISO', 'L1_LUCID_UNPAIRED_ISO', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],

	['noalg_mb_L1LHCF',             'L1_LHCF', [], [MinBiasStream, 'express'], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1LHCF_EMPTY',       'L1_LHCF_EMPTY', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1LHCF_UNPAIRED_ISO',    'L1_LHCF_UNPAIRED_ISO', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],

        # they are out for now ???
        # ['lhcfpeb_L1LHCF_ALFA_ANY_A',                    'L1_LHCF_ALFA_ANY_A',                 [], ['IDFwd'], [ 'RATE:ALFA','BW:Detector'], -1],
        # ['lhcfpeb_L1LHCF_ALFA_ANY_C',                    'L1_LHCF_ALFA_ANY_C',                 [], ['IDFwd'], [ 'RATE:ALFA','BW:Detector'], -1],
        # ['lhcfpeb_L1LHCF_ALFA_ANY_A_UNPAIRED_ISO',       'L1_LHCF_ALFA_ANY_A_UNPAIRED_ISO',    [], ['IDFwd'], [ 'RATE:ALFA','BW:Detector'], -1],
        # ['lhcfpeb_L1LHCF_ALFA_ANY_C_UNPAIRED_ISO',       'L1_LHCF_ALFA_ANY_C_UNPAIRED_ISO',    [], ['IDFwd'], [ 'RATE:ALFA','BW:Detector'], -1],

        # ['alfacalib_L1LHCF_ALFA_ANY_A',                    'L1_LHCF_ALFA_ANY_A',                 [], ['ALFACalib'], [ 'RATE:ALFA','BW:Detector'], -1],
        # ['alfacalib_L1LHCF_ALFA_ANY_C',                    'L1_LHCF_ALFA_ANY_C',                 [], ['ALFACalib'], [ 'RATE:ALFA','BW:Detector'], -1],
        # ['alfacalib_L1LHCF_ALFA_ANY_A_UNPAIRED_ISO',       'L1_LHCF_ALFA_ANY_A_UNPAIRED_ISO',    [], ['ALFACalib'], [ 'RATE:ALFA','BW:Detector'], -1],
        # ['alfacalib_L1LHCF_ALFA_ANY_C_UNPAIRED_ISO',       'L1_LHCF_ALFA_ANY_C_UNPAIRED_ISO',    [], ['ALFACalib'], [ 'RATE:ALFA','BW:Detector'], -1],


	#UPC+photoproduction

	#beam spot
	##['noalg_L1J10_VTE100',   'L1_J10_VTE100', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	##['noalg_L1J10_VTE200',   'L1_J10_VTE200', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	##['noalg_L1MU0_VTE50',   'L1_MU0_VTE50', [], [UPCStream], ["BW:UPC", "RATE:UPC"], 1],

	#muon
	['noalg_L1MU4',    'L1_MU4', [],   [HardProbesStream, 'express'], ["BW:SingleMuon", "RATE:Muon"], 1],
	['noalg_L1MU6',    'L1_MU6', [],  [HardProbesStream, 'express'], ["BW:SingleMuon", "RATE:Muon"], 1],	
	['noalg_L1MU10',   'L1_MU10', [],  [HardProbesStream, 'express'], ["BW:SingleMuon", "RATE:Muon"], 1],
	['noalg_L1MU15',   'L1_MU15', [], [HardProbesStream], ["BW:SingleMuon", "RATE:Muon"], 1],
	['noalg_L1MU20',   'L1_MU20', [], [HardProbesStream], ["BW:SingleMuon", "RATE:Muon"], 1],


	#jets
	['noalg_L1J10',   'L1_J10', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	['noalg_L1J15',   'L1_J15', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
        ['noalg_L1J20',   'L1_J20', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
        ['noalg_L1J30',   'L1_J30', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	##['noalg_L1J35',   'L1_J35', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	['noalg_L1J50',   'L1_J50', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	##['noalg_L1J75',   'L1_J75', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	##['noalg_L1J175',   'L1_J175', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],

	#egamma
	['noalg_L1EM3',    'L1_EM3',  [], [HardProbesStream], ["BW:SingleElectron", "RATE:Egamma"], 1],
	['noalg_L1EM7',    'L1_EM7',  [], [HardProbesStream], ["BW:SingleElectron", "RATE:Egamma"], 1],
        ##['noalg_L1EM8',    'L1_EM8',  [], [HardProbesStream], ["BW:SingleElectron", "RATE:Egamma"], 1],
	['noalg_L1EM10',   'L1_EM10', [], [HardProbesStream, 'express'], ["BW:SingleElectron", "RATE:Egamma"], 1],
	['noalg_L1EM12',   'L1_EM12', [], [HardProbesStream, 'express'], ["BW:SingleElectron", "RATE:Egamma"], 1],
        ['noalg_L1EM14',   'L1_EM14', [], [HardProbesStream, 'express'], ["BW:SingleElectron", "RATE:Egamma"], 1],
	['noalg_L1EM16',   'L1_EM16', [], [HardProbesStream], ["BW:SingleElectron", "RATE:Egamma"], 1],
	['noalg_L1EM20',   'L1_EM20', [], [HardProbesStream], ["BW:SingleElectron", "RATE:Egamma"], 1],
	##['noalg_L1EM18',   'L1_EM18', [], [HardProbesStream], ["BW:SingleElectron", "RATE:Egamma"], 1],
	##['noalg_L1EM20',   'L1_EM20', [], [HardProbesStream], ["BW:SingleElectron", "RATE:Egamma"], 1],

        ['noalg_L1MBTS_2_C_VZDC_A',    'L1_MBTS_2_C_VZDC_A',  [], [UPCStream], ["BW:UPC", "RATE:MinBias"], 1],
        ['noalg_L1MBTS_2_A_VZDC_C',    'L1_MBTS_2_A_VZDC_C',  [], [UPCStream], ["BW:UPC", "RATE:MinBias"], 1],
        ['noalg_L1ZDC_C_VZDC_A',    'L1_ZDC_C_VZDC_A',  [], [UPCStream], ["BW:UPC", "RATE:MinBias"], 1],
        ['noalg_L1ZDC_A_VZDC_C',    'L1_ZDC_A_VZDC_C',  [], [UPCStream], ["BW:UPC", "RATE:MinBias"], 1],
        ['noalg_L1EM3_VZDC_A',    'L1_EM3_VZDC_A',  [], [UPCStream], ["BW:UPC", "RATE:Egamma"], 1],
        ['noalg_L1EM3_VZDC_C',    'L1_EM3_VZDC_C',  [], [UPCStream], ["BW:UPC", "RATE:Egamma"], 1],
        ['noalg_L12EM3_VZDC_A',    'L1_2EM3_VZDC_A',  [], [UPCStream], ["BW:UPC", "RATE:Egamma"], 1],
        ['noalg_L12EM3_VZDC_C',    'L1_2EM3_VZDC_C',  [], [UPCStream], ["BW:UPC", "RATE:Egamma"], 1],
        ['noalg_L1EM5_VZDC_A',    'L1_EM5_VZDC_A',  [], [UPCStream], ["BW:UPC", "RATE:Egamma"], 1],
        ['noalg_L1EM5_VZDC_C',    'L1_EM5_VZDC_C',  [], [UPCStream], ["BW:UPC", "RATE:Egamma"], 1],
        ['noalg_L1J5_VZDC_A',    'L1_J5_VZDC_A',  [], [UPCStream], ["BW:UPC", "RATE:Jets"], 1],
        ['noalg_L1J5_VZDC_C',    'L1_J5_VZDC_C',  [], [UPCStream], ["BW:UPC", "RATE:Jets"], 1],
        ['noalg_L1J10_VZDC_A',    'L1_J5_VZDC_A',  [], [UPCStream], ["BW:UPC", "RATE:Jets"], 1],
        ['noalg_L1J10_VZDC_C',    'L1_J5_VZDC_C',  [], [UPCStream], ["BW:UPC", "RATE:Jets"], 1],
        ['noalg_L1MU4_VZDC_A',    'L1_MU4_VZDC_A',  [], [UPCStream], ["BW:UPC", "RATE:Muons"], 1],
        ['noalg_L1MU4_VZDC_C',    'L1_MU4_VZDC_C',  [], [UPCStream], ["BW:UPC", "RATE:Muons"], 1],
        ['noalg_L1MU6_VZDC_A',    'L1_MU6_VZDC_A',  [], [UPCStream], ["BW:UPC", "RATE:Muons"], 1],
        ['noalg_L1MU6_VZDC_C',    'L1_MU6_VZDC_C',  [], [UPCStream], ["BW:UPC", "RATE:Muons"], 1],
        ['noalg_L1TE5_VZDC_A',    'L1_TE5_VZDC_A',  [], [UPCStream], ["BW:UPC", "RATE:Jets"], 1],
        ['noalg_L1TE5_VZDC_C',    'L1_TE5_VZDC_C',  [], [UPCStream], ["BW:UPC", "RATE:Jets"], 1],
        
        # empty
        ['mb_mbts_L1MBTS_1_EMPTY',       'L1_MBTS_1_EMPTY', [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['mb_mbts_L1MBTS_2_EMPTY',       'L1_MBTS_2_EMPTY', [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['mb_mbts_L1MBTS_1_1_EMPTY',     'L1_MBTS_1_1_EMPTY', [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],

        ['mb_mbts_L1MBTS_1_UNPAIRED_ISO',             'L1_MBTS_1_UNPAIRED_ISO', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_mbts_L1MBTS_2_UNPAIRED_ISO',             'L1_MBTS_2_UNPAIRED_ISO', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_mbts_L1MBTS_1_1_UNPAIRED_ISO',           'L1_MBTS_1_1_UNPAIRED_ISO', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],

        ['noalg_mb_L1MBTS_1_EMPTY',       'L1_MBTS_1_EMPTY',        [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_2_EMPTY',       'L1_MBTS_2_EMPTY',        [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_1_1_EMPTY',       'L1_MBTS_1_1_EMPTY',        [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],

        ['noalg_mb_L1MBTS_1_UNPAIRED_ISO','L1_MBTS_1_UNPAIRED_ISO',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_2_UNPAIRED_ISO',  'L1_MBTS_2_UNPAIRED_ISO',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_1_1_UNPAIRED_ISO','L1_MBTS_1_1_UNPAIRED_ISO',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],

        #['noalg_L1',    'L1_',  [], [UPCStream], ["BW:UPC", "RATE:Jets"], 1],

	#data min-bias overlay
	['noalg_L1ZDC_A_C_OVERLAY',   'L1_ZDC_A_C_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
	['noalg_L1ZDC_A_OVERLAY',   'L1_ZDC_A_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
	['noalg_L1ZDC_C_OVERLAY',   'L1_ZDC_C_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
	['noalg_L1TE5_OVERLAY',   'L1_TE5_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
	['noalg_L1TE10_OVERLAY',   'L1_TE10_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
	['noalg_L1TE20_OVERLAY',   'L1_TE20_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
	['noalg_L1TE50_OVERLAY',   'L1_TE50_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
	
	#TE restricted calo in |eta|<4.9 for overlay   

	#data zero bias overlay
	['noalg_zb_L1ZB',   'L1_ZB', [], ['ZeroBias'], ["BW:ZeroBias", "RATE:ZeroBias"], 1],

        ##['noalg_L1TGC_BURST',   'L1_TGC_BURST', [], ['TgcNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],

        #idmon streamer
        ['noalg_idmon_L1RD0_EMPTY',          'L1_RD0_EMPTY',        [], ['IDMonitoring','express'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['noalg_idmon_L1RD0_FILLED',         'L1_RD0_FILLED',        [], ['IDMonitoring'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['noalg_idmon_L1RD0_UNPAIRED_ISO',   'L1_RD0_UNPAIRED_ISO', [], ['IDMonitoring'], ["RATE:Monitoring", "BW:Detector"], -1],

        # Cosmic calo streamer
        ['noalg_cosmiccalo_L1EM3_EMPTY',         'L1_EM3_EMPTY',        [], ['CosmicCalo'], ["BW:MinBias", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1EM7_EMPTY',         'L1_EM7_EMPTY',        [], ['CosmicCalo'], ["BW:MinBias", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1RD1_EMPTY',         'L1_RD1_EMPTY',        [], ['CosmicCalo', 'express'], ["RATE:Calibration", "BW:Detector"], -1],
        ['noalg_cosmiccalo_L1J3031ETA49_EMPTY',  'L1_J30.31ETA49_EMPTY',[], ['CosmicCalo'], ["BW:Jet", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1J12_EMPTY',         'L1_J12_EMPTY',        [], ['CosmicCalo', 'express'], ["BW:Jet", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1J30_EMPTY',         'L1_J30_EMPTY',        [], ['CosmicCalo', 'express'], ["BW:Jet", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1TAU8_EMPTY',         'L1_TAU8_EMPTY',        [], ['CosmicCalo', 'express'], ["BW:Jet", "RATE:Cosmic_Calo"], -1],

        #background streamers
        ['noalg_bkg_L1Bkg',               'L1_Bkg',               [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1],  
        ##['noalg_bkg_L1J12_BGRP12', 'L1_J12_BGRP12', [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1],
        ##['noalg_bkg_L1J30.31ETA49_BGRP12', 'L1_J30.31ETA49_BGRP12', [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_L1Calo', 'L1_Calo', [], ['L1Calo'], ["RATE:SeededStreamers", "BW:Other"], -1], 

        #DCM level monitoring test chain
        ['noalg_dcmmon_L1RD0_EMPTY',   'L1_RD0_EMPTY',        [], ['monitoring_random'], ["RATE:DISCARD","BW:DISCARD"], -1],
        
	#standby
	['noalg_standby_L1RD0_FILLED', 'L1_RD0_FILLED', [], ['Standby'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_standby_L1RD0_EMPTY', 'L1_RD0_EMPTY', [], ['Standby'], ["RATE:CoMinBias", "BW:MinBias"], -1],
	['noalg_L1Standby',              'L1_Standby',          [], ['Standby'],    ["RATE:SeededStreamers", "BW:Other"], -1],

        


 #       ['noalg_L1Topo',                           'L1_Topo',             [], ['L1Calo'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        #['noalg_larcells_L1LAR-EM',                'L1_LAR-EM',           [], ['LArCells'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        #['noalg_larcells_L1LAR-J',                 'L1_LAR-J',            [], ['LArCells'],     ["RATE:SeededStreamers", "BW:Other"], -1],

        # HI EB PbPb
        ##['noalg_eb_L1ZDC_A_C_VTE50', 'L1_ZDC_A_C_VTE50', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ##['noalg_eb_L1TE5_VTE200', 'L1_TE5_VTE200', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ##['noalg_eb_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ##['noalg_eb_L1MU0_VTE50', 'L1_MU0_VTE50', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ##['noalg_eb_L1EM12', 'L1_EM12', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ##['noalg_eb_L1MU4', 'L1_MU4', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ##['noalg_eb_L1TE50', 'L1_TE50', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],

        # HI EB pPb
        ['noalg_eb_L1ZDC_A',           'L1_ZDC_A',           [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1ZDC_C',           'L1_ZDC_C',           [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1MBTS_2',          'L1_MBTS_2',          [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1TE5',             'L1_TE5',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1TE50',            'L1_TE50',            [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1TE120',           'L1_TE120',           [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1TE160',           'L1_TE160',           [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1TE10.0ETA24',     'L1_TE10.0ETA24',     [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1TE30.0ETA24',     'L1_TE30.0ETA24',     [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1MU4',             'L1_MU4',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1MU10',            'L1_MU10',            [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1EM5',             'L1_EM5',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1EM10',            'L1_EM10',            [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1J10',             'L1_J10',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1J30',             'L1_J30',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1J5_VZDC_C',       'L1_J5_VZDC_C',       [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1J5_VZDC_A',       'L1_J5_VZDC_A',       [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1MU4_VZDC_A',      'L1_MU4_VZDC_A',      [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1MU4_VZDC_C',      'L1_MU4_VZDC_C',      [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1TE5_VZDC_A',      'L1_TE5_VZDC_A',      [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1TE5_VZDC_C',      'L1_TE5_VZDC_C',      [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1MBTS_2_A_VZDC_C', 'L1_MBTS_2_A_VZDC_C', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1MBTS_2_C_VZDC_A', 'L1_MBTS_2_C_VZDC_A', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1ZDC_A_VZDC_C',    'L1_ZDC_A_VZDC_C',    [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1ZDC_C_VZDC_A',    'L1_ZDC_C_VZDC_A',    [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1EM3_VZDC_C',      'L1_EM3_VZDC_C',      [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1EM3_VZDC_A',      'L1_EM3_VZDC_A',      [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1RD1_FILLED',      'L1_RD1_FILLED',      [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        
        #L1Topo
        ['noalg_l1topo_L12EM3',   'L1_2EM3',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L12MU4',   'L1_2MU4',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L12MU6',   'L1_2MU6',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L14J15',   'L1_4J15',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        #['noalg_l1topo_L14J20.0ETA49',   'L1_4J20.0ETA49',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        #['noalg_l1topo_L1EM12_2EM3',   'L1_EM12_2EM3',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        #['noalg_l1topo_L1EM15I_MU4',   'L1_EM15I_MU4',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L1EM3',   'L1_EM3',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L1EM15',   'L1_EM15',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        #['noalg_l1topo_L1EM7_2EM3',   'L1_EM7_2EM3',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        #['noalg_l1topo_L1EM8I_MU10',   'L1_EM8I_MU10',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L1EM7_MU10', 'L1_EM7_MU10', [], ['L1Topo'], ["RATE:SeededStreamers", "BW:Other"], -1],                                                                                                                              
        ['noalg_l1topo_L1EM7',   'L1_EM7',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L1J100',   'L1_J100',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L1J15',   'L1_J15',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L1J20',   'L1_J20',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        #['noalg_l1topo_L1J25',   'L1_J25',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        #['noalg_l1calo_L1J400', 'L1_J400', [], ['L1Calo'], ["RATE:SeededStreamers", "BW:Other"], -1],
        #['noalg_l1topo_L1J30.0ETA49_2J20.0ETA49',   'L1_J30.0ETA49_2J20.0ETA49',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L1J40_XE50',   'L1_J40_XE50',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        #['noalg_l1topo_L1MU10_2MU6',   'L1_MU10_2MU6',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L1MU10_TAU12IM',   'L1_MU10_TAU12IM',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L1MU4',   'L1_MU4',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
		#ATR-13743         ['noalg_l1topo_L1MU4_J30',   'L1_MU4_J30',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        #['noalg_l1topo_L1MU6_2MU4',   'L1_MU6_2MU4',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        #['noalg_l1topo_L1MU6_J20',   'L1_MU6_J20',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L1TAU12IM',   'L1_TAU12IM',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L1TAU20IM_2TAU12IM',   'L1_TAU20IM_2TAU12IM',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L1TAU30',   'L1_TAU30',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L1TAU40',   'L1_TAU40',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L1XE35',   'L1_XE35',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        #['noalg_l1topo_L1XE45',   'L1_XE45',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        #['noalg_l1topo_L1XE55',   'L1_XE55',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        #['noalg_l1topo_L1XE60',   'L1_XE60',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        #['noalg_l1topo_L1XE70',   'L1_XE70',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L1MU4_J12', 'L1_MU4_J12', [], ['L1Topo'], ["RATE:SeededStreamers", "BW:Other"], -1],
        
                ["noalg_l1topo_L12MU4-B", "L1_2MU4-B", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L12MU4-BO", "L1_2MU4-BO", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L12MU4_J20_XE30_DPHI-J20s2XE30", "L1_2MU4_J20_XE30_DPHI-J20s2XE30", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L12MU6-B", "L1_2MU6-B", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L12MU6-BO", "L1_2MU6-BO", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L130M-EM20ITAU12", "L1_30M-EM20ITAU12", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L13J15_BTAG-MU4J15", "L1_3J15_BTAG-MU4J15", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L13J15_BTAG-MU4J30", "L1_3J15_BTAG-MU4J30", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L13J15_BTAG-MU6J25", "L1_3J15_BTAG-MU6J25", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L13J20_4J20.0ETA49_MJJ-400", "L1_3J20_4J20.0ETA49_MJJ-400", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L13J20_4J20.0ETA49_MJJ-700", "L1_3J20_4J20.0ETA49_MJJ-700", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L13J20_4J20.0ETA49_MJJ-800", "L1_3J20_4J20.0ETA49_MJJ-800", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L13J20_4J20.0ETA49_MJJ-900", "L1_3J20_4J20.0ETA49_MJJ-900", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L13J20_BTAG-MU4J20", "L1_3J20_BTAG-MU4J20", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1BOX-TAU20ITAU12I", "L1_BOX-TAU20ITAU12I", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4", "L1_BPH-2M8-2MU4-B_BPH-0DR15-2MU4", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", "L1_BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1BPH-2M9-2MU6_BPH-2DR15-2MU6", "L1_BPH-2M9-2MU6_BPH-2DR15-2MU6", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1BPH-7M15-2MU4_BPH-0DR24-2MU4", "L1_BPH-7M15-2MU4_BPH-0DR24-2MU4", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-B", "L1_BPH-7M15-2MU4_BPH-0DR24-2MU4-B", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO", "L1_BPH-7M15-2MU4_BPH-0DR24-2MU4-BO", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1BPH-8M15-2MU6", "L1_BPH-8M15-2MU6", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1BPH-8M15-2MU6_BPH-0DR22-2MU6", "L1_BPH-8M15-2MU6_BPH-0DR22-2MU6", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1BPH-8M15-MU6MU4", "L1_BPH-8M15-MU6MU4", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4", "L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1BTAG-MU4J15", "L1_BTAG-MU4J15", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1BTAG-MU4J30", "L1_BTAG-MU4J30", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1BTAG-MU6J20", "L1_BTAG-MU6J20", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1BTAG-MU6J25", "L1_BTAG-MU6J25", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1DPHI-AJ20s2XE50", "L1_DPHI-AJ20s2XE50", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1DPHI-CJ20XE50", "L1_DPHI-CJ20XE50", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1DPHI-J20XE50", "L1_DPHI-J20XE50", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1DPHI-J20s2XE50", "L1_DPHI-J20s2XE50", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1DR-EM15TAU12I", "L1_DR-EM15TAU12I", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1DR-EM15TAU12I-J25", "L1_DR-EM15TAU12I-J25", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1DR-MU10TAU12I", "L1_DR-MU10TAU12I", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1DR-MU10TAU12I_TAU12I-J25", "L1_DR-MU10TAU12I_TAU12I-J25", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1DR-TAU20ITAU12I", "L1_DR-TAU20ITAU12I", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1DR-TAU20ITAU12I-J25", "L1_DR-TAU20ITAU12I-J25", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1DR25-TAU20ITAU12I", "L1_DR25-TAU20ITAU12I", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1DR25-TAU20ITAU12I-J25", "L1_DR25-TAU20ITAU12I-J25", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1DY-BOX-2MU6", "L1_DY-BOX-2MU6", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1DY-DR-2MU4", "L1_DY-DR-2MU4", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1EM15TAU12I-J25", "L1_EM15TAU12I-J25", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1EM15_W-MT35", "L1_EM15_W-MT35", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #["noalg_l1topo_L1EM18VH_MJJ-300", "L1_EM18VH_MJJ-300", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1HT150-J20.ETA31", "L1_HT150-J20.ETA31", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1HT150-J20s5.ETA31", "L1_HT150-J20s5.ETA31", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1HT150-JJ15.ETA49", "L1_HT150-JJ15.ETA49", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1HT190-J15.ETA21", "L1_HT190-J15.ETA21", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1HT190-J15s5.ETA21", "L1_HT190-J15s5.ETA21", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1HT150-J20s5.ETA31_MJJ-400-CF", "L1_HT150-J20s5.ETA31_MJJ-400-CF", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1J25_2J20_3J12_BOX-TAU20ITAU12I", "L1_J25_2J20_3J12_BOX-TAU20ITAU12I", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1J30_2J20_4J20.0ETA49_MJJ-400", "L1_J30_2J20_4J20.0ETA49_MJJ-400", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1J30_2J20_4J20.0ETA49_MJJ-700", "L1_J30_2J20_4J20.0ETA49_MJJ-700", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1J30_2J20_4J20.0ETA49_MJJ-800", "L1_J30_2J20_4J20.0ETA49_MJJ-800", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1J30_2J20_4J20.0ETA49_MJJ-900", "L1_J30_2J20_4J20.0ETA49_MJJ-900", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1J4-MATCH", "L1_J4-MATCH", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1J40_DPHI-CJ20XE50", "L1_J40_DPHI-CJ20XE50", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1J40_DPHI-J20XE50", "L1_J40_DPHI-J20XE50", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1J40_DPHI-J20s2XE50", "L1_J40_DPHI-J20s2XE50", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1J40_XE50_DPHI-J20s2XE50", "L1_J40_XE50_DPHI-J20s2XE50", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1JPSI-1M5", "L1_JPSI-1M5", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1JPSI-1M5-EM12", "L1_JPSI-1M5-EM12", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1JPSI-1M5-EM7", "L1_JPSI-1M5-EM7", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1KF-XE40", "L1_KF-XE40", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1KF-XE50", "L1_KF-XE50", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1KF-XE55", "L1_KF-XE55", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1KF-XE60", "L1_KF-XE60", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1KF-XE65", "L1_KF-XE65", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1KF-XE75", "L1_KF-XE75", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1LAR-EM", "L1_LAR-EM", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1LAR-J", "L1_LAR-J", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1LFV-EM15I", "L1_LFV-EM15I", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1LFV-EM8I", "L1_LFV-EM8I", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1LFV-MU", "L1_LFV-MU", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1LFV-MU6", "L1_LFV-MU6", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1LLP-NOMATCH", "L1_LLP-NOMATCH", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1LLP-RO", "L1_LLP-RO", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1MJJ-100", "L1_MJJ-100", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1MJJ-400", "L1_MJJ-400", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1MJJ-400-CF", "L1_MJJ-400-CF", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1MJJ-700", "L1_MJJ-700", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1MJJ-800", "L1_MJJ-800", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1MJJ-900", "L1_MJJ-900", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1MU10_TAU12I-J25", "L1_MU10_TAU12I-J25", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1MU4_J20_XE30_DPHI-J20s2XE30", "L1_MU4_J20_XE30_DPHI-J20s2XE30", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1MU6MU4-BO", "L1_MU6MU4-BO", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1MU6_2MU4-B", "L1_MU6_2MU4-B", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1MU6_MJJ-200", "L1_MU6_MJJ-200", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1MU6_MJJ-300", "L1_MU6_MJJ-300", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1MU6_MJJ-400", "L1_MU6_MJJ-400", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1MU6_MJJ-500", "L1_MU6_MJJ-500", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1TAU20ITAU12I-J25", "L1_TAU20ITAU12I-J25", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1W-05RO-XEHT-0", "L1_W-05RO-XEHT-0", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1W-250RO2-XEHT-0", "L1_W-250RO2-XEHT-0", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1W-90RO2-XEHT-0", "L1_W-90RO2-XEHT-0", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1W-HT20-JJ15.ETA49", "L1_W-HT20-JJ15.ETA49", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1W-NOMATCH", "L1_W-NOMATCH", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1XE35_MJJ-200", "L1_XE35_MJJ-200", [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],

        ]

    TriggerFlags.BeamspotSlice.signatures = [
        
        # taking this out all together, not needed and even with PS=-1 makes histograms
        ['beamspot_trkFS_trkfast_L14J10', 'L1_4J10', [], ["BeamSpot"], ['RATE:BeamSpot', 'BW:BeamSpot'], -1],

        ['beamspot_activeTE_trkfast_peb_L13J10', 'L1_3J10', [], ["BeamSpot"], ['RATE:BeamSpot', 'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_peb_L13J10', 'L1_3J10', [], ["BeamSpot"], ['RATE:BeamSpot', 'BW:BeamSpot'], -1],
        ['beamspot_trkFS_trkfast_L13J10', 'L1_3J10', [], ["BeamSpot"], ['RATE:BeamSpot', 'BW:BeamSpot'], -1],
        
        ['beamspot_trkFS_trkfast', 'L1_4J15', [], ["BeamSpot"], ['RATE:BeamSpot', 'BW:BeamSpot'], -1],

        ['beamspot_activeTE_trkfast_peb_L13J15', 'L1_3J15', [], ["BeamSpot"], ['RATE:BeamSpot', 'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_peb_L13J15', 'L1_3J15', [], ["BeamSpot"], ['RATE:BeamSpot', 'BW:BeamSpot'], -1],
        ['beamspot_trkFS_trkfast_L13J15', 'L1_3J15', [], ["BeamSpot"], ['RATE:BeamSpot', 'BW:BeamSpot'], -1],

        ['beamspot_activeTE_trkfast_peb_L1J15', 'L1_J15', [], ["BeamSpot"], ['RATE:BeamSpot', 'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_peb_L1J15', 'L1_J15', [], ["BeamSpot"], ['RATE:BeamSpot', 'BW:BeamSpot'], -1],
        ['beamspot_trkFS_trkfast_L1J15', 'L1_J15', [], ["BeamSpot"], ['RATE:BeamSpot', 'BW:BeamSpot'], -1],

	]

    TriggerFlags.MonitorSlice.signatures = [
        ['l1calooverflow','', [], ['L1Calo'], ['RATE:Monitoring','BW:Other'], -1],
        
        ## # enhancedbias
        
        ['mistimemonl1bccorr', '', [], ['Mistimed'], ["RATE:Monitoring","BW:Other"], -1],
        ['mistimemonl1bccorrnomu', '', [], ['Mistimed'], ["RATE:Monitoring","BW:Other"], -1],
        ['mistimemoncaltimenomu', '', [], ['Mistimed'], ["RATE:Monitoring","BW:Other"], -1],
        ['mistimemoncaltime', '', [], ['Mistimed'], ["RATE:Monitoring","BW:Other"], -1],
        ['mistimemonj400', '', [], ['Mistimed'], ["RATE:Monitoring","BW:Other"], -1],
        
        ['timeburner', '', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1],
        ['robrequest', '', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1],
        ['robrequest_L1RD0_EMPTY', 'L1_RD0_EMPTY', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1], 
        
        ['cscmon_L1All', 'L1_All',         [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1],

        ['costmonitor', '', [], ['CostMonitoring'], ['RATE:Monitoring','BW:Other'],1],
        
        ]
    #TriggerFlags.GenericSlice.signatures = []
    
class Prescales:
    #   Item name             | Prescale
    #----------------------------------------------------------
    L1Prescales = {

        }
    HLTPrescales = {
        }


# StreamConfig = {
#     'HardProbes':      [ 'egamma',
#                      'comb__ej', 'comb__emet', 'comb__ejmet', 'comb__eg',
#                      'comb__gmet', 
#                      'comb__emu', 'comb__gmu',
#                      'comb_tauej', 
#                      ], 
#     'MinBias':     [ 'minbias' ], 
#     'Background':  [ 'background' ], 

#     'HardProbes':       [ 'muons', 'bphysics',
#                      'comb__muj', 'comb__mumet', 'comb__mujmet',
#                      #'comb__emu', 'comb__gmu',
#                      'comb_taumuj','comb__mue', 'comb__mug',
#                      'comb__multimu',
#                      ], 
#     'HardProbes':   [ 'jets', 'tauEtmiss',
#                         'comb__tauj', 'comb__taumet', 'comb__jmet',
#                         'comb__taujmet', 'comb__taub', 'comb__taue', 
#                         'comb__taue', 'comb__taumu', 'comb__fjj', 
#                         'comb__tauej', 'comb__taumuj', ],
#     # New delayed streams for Jet triggers
#     'HadDelayed':  [ 'haddelayed' ],
#     'JetCalibDelayed':  [ 'jetcalibdelayed' ],
#     'Standby': [ 'standby' ], 
#     'calibration': [ 'calibration' ], 
#     'express':     [ 'express' ],

#     }


#  LocalWords:  sptrk L1RD0

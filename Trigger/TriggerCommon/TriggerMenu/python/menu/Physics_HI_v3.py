# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------------------------------------------------------------------#
def setupMenu():
    
    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenu.menu.Physics_HI_v3.py' )

    from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1 as tcl1
    
    if tcl1.current:
        log.info("L1 items: %s " % tcl1.current.menu.items.itemNames())
    else:
        log.info("ERROR L1 menu has not yet been defined")

    HardProbesStream="HardProbes"
    MinBiasStream="MinBias"
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
	['mu4_idperf_L1MU0',    'L1_MU0', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
        ['mu4_idperf',          'L1_MU4', [], [HardProbesStream, 'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu6_idperf_L1MU0',    'L1_MU0', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu6_idperf',          'L1_MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	#['mu8_idperf',          'L1_MU6', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1], #no configuration
	['mu10_idperf',         'L1_MU6', [], [HardProbesStream, 'express'], ['RATE:SingleMuon','BW:Muon'], -1],

	#RoI muons
	['mu4_L1MU0',           'L1_MU0', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu4',                 'L1_MU4', [], [HardProbesStream,  'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu6_L1MU0',           'L1_MU0', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu6',                 'L1_MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu8',                 'L1_MU6', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu10',                'L1_MU6', [], [HardProbesStream,  'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu14',                'L1_MU11', [], [HardProbesStream, 'express'], ['RATE:SingleMuon','BW:Muon'], -1],

	#msonly
	['mu15_msonly',            'L1_MU11',           [], [HardProbesStream, 'express'], ['RATE:SingleMuon', 'BW:Muon'], -1],
	['mu20_msonly',            'L1_MU15',           [], [HardProbesStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

	#di-muons RoI based
	['2mu4',                 'L1_2MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['2mu6',                 'L1_2MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['2mu4_msonly',          'L1_2MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
        ['2mu6_msonly',          'L1_2MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],

	['2mu4_L1MU0',                 'L1_2MU0', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
        ['2mu6_L1MU0',                 'L1_2MU0', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
        ['2mu4_msonly_L1MU0',          'L1_2MU0', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
        ['2mu6_msonly_L1MU0',          'L1_2MU0', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],

	#Full scan
	['mu4_mu2noL1',         'L1_MU4', ['L1_MU4',''], [HardProbesStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial',-1,["mu4","mu2noL1"]]],
        ['mu4_mu4noL1',         'L1_MU4', ['L1_MU4',''], [HardProbesStream, 'express'], ['RATE:MultiMuon','BW:Muon'], -1, ['serial',-1,["mu4","mu4noL1"]]],
        ['mu6_mu4noL1',         'L1_MU4', ['L1_MU4',''], [HardProbesStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial',-1,["mu6","mu4noL1"]]],
        
        #Full-scan supporting
	['mu2noL1',         'L1_MU4', ['L1_MU4',''], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1, ],
        ['mu4noL1',         'L1_MU4', ['L1_MU4',''], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1, ],
        ['mu6noL1',         'L1_MU4', ['L1_MU4',''], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1, ],

        # muon calibration
        ['mu0_muoncalib',             'L1_MU4',      [],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
        ['mu0_muoncalib_L1MU15',      'L1_MU15',     [],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
        ['mu0_muoncalib_L1MU4_EMPTY', 'L1_MU4_EMPTY',[],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
        
        
        ]

    TriggerFlags.JetSlice.signatures = [
	#performance
	['j0_perf_L1ZDC_A_C',     'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j0_perf_L1RD0_FILLED',     'L1_RD0_FILLED',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],

	#physics
	#R=0.4 jets
	['j20_ion_L1TE20',         'L1_TE20',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j25_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_ion_L1TE20',         'L1_TE20',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],   
	['j40_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j50_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_ion_L1TE50',         'L1_TE50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],   
	['j40_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j50_ion_L1TE50',         'L1_TE50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j60_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j75_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j100_ion_L1TE50',         'L1_TE50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j150_ion_L1TE50',         'L1_TE50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j50_ion_L1J10',         'L1_J10',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j60_ion_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j75_ion_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j100_ion_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j150_ion_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],	
	['j30_ion_L1ZDC_A_C',         'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_ion_L1ZDC_A_C',         'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j50_ion_L1ZDC_A_C',         'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j60_ion_L1ZDC_A_C',         'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j75_ion_L1ZDC_A_C',         'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	#R=0.3 jets
	#['j20_a3_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j25_a3_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j30_a3_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],   
	#['j40_a3_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j50_a3_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j30_a3_ion_L1TE50',         'L1_TE50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],   
	#['j40_a3_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j50_a3_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j60_a3_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j75_a3_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j100_a3_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j150_a3_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j50_a3_ion_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j60_a3_ion_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j75_a3_ion_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j100_a3_ion_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j150_a3_ion_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j30_a3_ion_L1ZDC_A_C',         'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j40_a3_ion_L1ZDC_A_C',         'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j50_a3_ion_L1ZDC_A_C',         'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j60_a3_ion_L1ZDC_A_C',         'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j75_a3_ion_L1ZDC_A_C',         'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],

	#forward jets ATR-12623
	['j25_320eta490_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_320eta490_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_320eta490_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_320eta490_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_320eta490_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j40_320eta490_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j50_320eta490_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j50_320eta490_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],

	#['j25_a3_320eta490_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        #['j25_a3_320eta490_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        #['j30_a3_320eta490_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        #['j30_a3_320eta490_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        #['j40_a3_320eta490_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        #['j40_a3_320eta490_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        #['j50_a3_320eta490_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        #['j50_a3_320eta490_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],

	#multijets ATR-12737
	['j60_ion_2j30_ion_L1TE50',    'L1_TE50', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j75_ion_2j30_ion_L1TE50',    'L1_TE50', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j75_ion_2j40_ion_L1TE50',    'L1_TE50', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j100_ion_2j30_ion_L1TE50',   'L1_TE50', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j100_ion_2j40_ion_L1TE50',   'L1_TE50', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j100_ion_2j50_ion_L1TE50',   'L1_TE50', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],

	['j60_ion_2j30_ion_L1J10',     'L1_J10', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j75_ion_2j30_ion_L1J10',     'L1_J10', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j75_ion_2j40_ion_L1J10',     'L1_J10', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j100_ion_2j30_ion_L1J10',    'L1_J10', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j100_ion_2j40_ion_L1J10',    'L1_J10', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j100_ion_2j50_ion_L1J10',    'L1_J10', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        #photoproduction ATR-12923
        ['j5_ion_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
        ['j10_ion_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j15_ion_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j20_ion_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],

        ['j5_ion_L1ZDC_XOR_TE5_VTE200',                 'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
        ['j10_ion_L1ZDC_XOR_TE5_VTE200',                 'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j15_ion_L1ZDC_XOR_TE5_VTE200',                 'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j20_ion_L1ZDC_XOR_TE5_VTE200',                 'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],

        ['j5_ion_L1ZDC_A_C_TE5_VTE200',                  'L1_ZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
        ['j10_ion_L1ZDC_A_C_TE5_VTE200',                 'L1_ZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j15_ion_L1ZDC_A_C_TE5_VTE200',                 'L1_ZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j20_ion_L1ZDC_A_C_TE5_VTE200',                 'L1_ZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
        
        #forward
        ['j10_320eta490_ion_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
        ['j10_320eta490_ion_L1ZDC_XOR_TE5_VTE200',         'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
        ['j10_320eta490_ion_L1ZDC_A_C_TE5_VTE200',         'L1_ZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
        ['j15_320eta490_ion_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
        ['j15_320eta490_ion_L1ZDC_XOR_TE5_VTE200',         'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
        ['j15_320eta490_ion_L1ZDC_A_C_TE5_VTE200',         'L1_ZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
        ['j20_320eta490_ion_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
        ['j20_320eta490_ion_L1ZDC_XOR_TE5_VTE200',         'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
        ['j20_320eta490_ion_L1ZDC_A_C_TE5_VTE200',         'L1_ZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],

	#photoproduction ATR-12470
	['2j10_ion_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['2j15_ion_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['2j20_ion_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],

	['2j10_ion_L1ZDC_XOR_TE5_VTE200',         'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['2j15_ion_L1ZDC_XOR_TE5_VTE200',         'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['2j20_ion_L1ZDC_XOR_TE5_VTE200',         'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],

	['2j10_ion_L1ZDC_A_C_TE5_VTE200',         'L1_ZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['2j15_ion_L1ZDC_A_C_TE5_VTE200',         'L1_ZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['2j20_ion_L1ZDC_A_C_TE5_VTE200',         'L1_ZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],

	['j10_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_VTE50',  'L1_ZDC_A_C_VTE50'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','j10_ion']]],
        ['j15_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_VTE50',  'L1_ZDC_A_C_VTE50'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','j15_ion']]],
        ['j20_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_VTE50',  'L1_ZDC_A_C_VTE50'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','j20_ion']]],
        
        ['j10_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_TE5_VTE200',  'L1_ZDC_A_C_TE5_VTE200'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','j10_ion']]],
        ['j15_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_TE5_VTE200',  'L1_ZDC_A_C_TE5_VTE200'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','j15_ion']]],
        ['j20_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_TE5_VTE200',  'L1_ZDC_A_C_TE5_VTE200'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','j20_ion']]],
        
        ['j10_ion_mb_mbts_vetombts1side2in_L1ZDC_XOR_TE5_VTE200',  'L1_ZDC_XOR_TE5_VTE200'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','j10_ion']]],
        ['j15_ion_mb_mbts_vetombts1side2in_L1ZDC_XOR_TE5_VTE200',  'L1_ZDC_XOR_TE5_VTE200'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','j15_ion']]],
        ['j20_ion_mb_mbts_vetombts1side2in_L1ZDC_XOR_TE5_VTE200',  'L1_ZDC_XOR_TE5_VTE200'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','j20_ion']]],

	['2j10_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_VTE50',  'L1_ZDC_A_C_VTE50'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','2j10_ion']]],
	['2j15_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_VTE50',  'L1_ZDC_A_C_VTE50'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','2j15_ion']]],
        ['2j20_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_VTE50',  'L1_ZDC_A_C_VTE50'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','2j20_ion']]],

	['2j10_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_TE5_VTE200',  'L1_ZDC_A_C_TE5_VTE200'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','2j10_ion']]],
        ['2j15_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_TE5_VTE200',  'L1_ZDC_A_C_TE5_VTE200'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','2j15_ion']]],
        ['2j20_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_TE5_VTE200',  'L1_ZDC_A_C_TE5_VTE200'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','2j20_ion']]],

	['2j10_ion_mb_mbts_vetombts1side2in_L1ZDC_XOR_TE5_VTE200',  'L1_ZDC_XOR_TE5_VTE200'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','2j10_ion']]],
        ['2j15_ion_mb_mbts_vetombts1side2in_L1ZDC_XOR_TE5_VTE200',  'L1_ZDC_XOR_TE5_VTE200'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','2j15_ion']]],
        ['2j20_ion_mb_mbts_vetombts1side2in_L1ZDC_XOR_TE5_VTE200',  'L1_ZDC_XOR_TE5_VTE200'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','2j20_ion']]],
        ]

    TriggerFlags.BjetSlice.signatures = [
        ]

    TriggerFlags.METSlice.signatures = [
        ]

    TriggerFlags.TauSlice.signatures = [
        ]

    TriggerFlags.EgammaSlice.signatures = [

	['e10_etcut',                  'L1_EM7',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e12_etcut',                  'L1_EM8',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_etcut',                  'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_etcut_L1EM12',           'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['g12_etcut',                  'L1_EM10', [], [HardProbesStream], ['RATE:SinglPhoton', 'BW:Egamma'], -1],
       	['g15_etcut',                  'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g18_etcut',                  'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g20_etcut',                  'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

	#Run 1 PID
	['e15_loose',                  'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_medium',                 'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_lhloose',                'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_lhmedium',               'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['e15_loose_L1EM12',           'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_medium_L1EM12',          'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_lhloose_L1EM12',         'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_lhmedium_L1EM12',        'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	
	#heavy ion instances	
	['e10_etcut_ion',              'L1_EM7', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e12_etcut_ion',              'L1_EM8', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e12_etcut_ion_L1EM10',       'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_etcut_ion',              'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_etcut_ion_L1EM12',       'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['g12_etcut_ion',              'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g15_etcut_ion',              'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g18_etcut_ion',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g20_etcut_ion',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

        ['e15_loose_ion',              'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_medium_ion',             'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_lhloose_ion',            'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_lhmedium_ion',           'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

        ['e15_loose_ion_L1EM12',       'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_medium_ion_L1EM12',      'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_lhloose_ion_L1EM12',     'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_lhmedium_ion_L1EM12',    'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
 
	#backup for e15
        ['e18_loose_ion',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e18_medium_ion',             'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e18_lhloose_ion',            'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e18_lhmedium_ion',           'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	#backup for e15
	['e20_etcut_ion',              'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
	['e20_loose_ion',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e20_lhloose_ion',            'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e20_etcut',                  'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e20_loose',                  'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e20_lhloose',                'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	#idperf
	['e15_loose_ion_idperf',       'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
	['e15_medium_ion_idperf',       'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
	['e15_loose_ion_idperf_L1EM12', 'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
	['e15_medium_ion_idperf_L1EM12', 'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],

	#di-electrons , timeouts ATR-10226
	['2e10_loose_ion',               'L1_2EM7', [], [HardProbesStream, 'express'], ['RATE:MultiElectron', 'BW:Egamma'], -1],
	#['2e10_lhloose_ion',            'L1_2EM5', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['g15_loose',                  'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g15_medium',                 'L1_EM10', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g20_loose',                  'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g20_medium',                 'L1_EM12', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

	['g15_loose_ion',              'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g15_medium_ion',             'L1_EM10', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g20_loose_ion',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g20_medium_ion',             'L1_EM12', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

	#di-photon triggers
	['2g15_loose',                  'L1_2EM10', [], [HardProbesStream], ['RATE:MultiPhoton', 'BW:Egamma'], -1],
	['2g15_loose_ion',              'L1_2EM10', [], [HardProbesStream], ['RATE:MultiPhoton', 'BW:Egamma'], -1],

	# Monopole triggers ATR-12467
        ['g0_hiptrt_L1EM14',                    'L1_EM14', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g0_hiptrt_L1EM16',                    'L1_EM16', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

        ]

    TriggerFlags.BphysicsSlice.signatures = [
	['2mu4_bDimu',                    'L1_2MU4', [], [HardProbesStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1], 
        ['2mu4_bDimu_novtx_noos',         'L1_2MU4', [], [HardProbesStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],
	['mu4_mu4_idperf_bJpsimumu_noid',  'L1_2MU4', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring','BW:Bphys', 'BW:ID'], -1],
	['mu13_mu13_idperf_Zmumu',     'L1_2MU6', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring','BW:Bphys', 'BW:ID'], -1],
        ]

    TriggerFlags.CombinedSlice.signatures = [
	#['mu4_j20_a3_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j20_a3_ion'] ]],
	['mu4_j20_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream, 'express'], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j20_ion'] ]],

	#['mu4_j25_a3_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j25_a3_ion'] ]],
	['mu4_j25_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j25_ion'] ]],

	#['mu4_j30_a3_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j30_a3_ion'] ]],
	['mu4_j30_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j30_ion'] ]],

	#['mu4_j40_a3_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j40_a3_ion'] ]],
	['mu4_j40_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j40_ion'] ]],
	
	#['mu4_j50_a3_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j50_a3_ion'] ]],
	['mu4_j50_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j50_ion'] ]],

	#['mu4_j60_a3_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j60_a3_ion'] ]],
	['mu4_j60_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream, 'express'], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j60_ion'] ]],


	#['mu6_j20_a3_ion_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j20_a3_ion'] ]],
	['mu6_j20_ion_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j20_ion'] ]],

	#['mu6_j25_a3_ion_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j25_a3_ion'] ]],
	['mu6_j25_ion_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j25_ion'] ]],

	#['mu6_j30_a3_ion_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j30_a3_ion'] ]],
	['mu6_j30_ion_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream, 'express'], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j30_ion'] ]],

	#supporting triggers
	#['mu4_j20_a3_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j20_a3_ion'] ]],
	['mu4_j20_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j20_ion'] ]],

	#['mu4_j25_a3_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j25_a3_ion'] ]],
	['mu4_j25_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j25_ion'] ]],
    
	#['mu4_j30_a3_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j30_a3_ion'] ]],
	['mu4_j30_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j30_ion'] ]],

	#['mu4_j40_a3_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j40_a3_ion'] ]],
	['mu4_j40_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j40_ion'] ]],

	#['mu4_j50_a3_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j50_a3_ion'] ]],
	['mu4_j50_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j50_ion'] ]],

	#['mu4_j60_a3_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j60_a3_ion'] ]],
	['mu4_j60_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j60_ion'] ]],

        

	#['mu6_j20_a3_ion', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j20_a3_ion'] ]],
	['mu6_j20_ion', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j20_ion'] ]],

	#['mu6_j25_a3_ion', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j25_a3_ion'] ]],
	['mu6_j25_ion', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j25_ion'] ]],

	#['mu6_j30_a3_ion', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j30_a3_ion'] ]],
	['mu6_j30_ion', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j30_ion'] ]],
        ]
   
    TriggerFlags.HeavyIonSlice.signatures = [
	#event shape triggers
        ['hi_v2_th5_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3_th5_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2_th10_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3_th10_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2_th13_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3_th13_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2_th14_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3_th14_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2_th15_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3_th15_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2_th16_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3_th16_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3_th13_veto2_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2_th13_veto3_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v23_th14_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v23_th15_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],

	#ultra-central triggers
	#physics
        ['hi_th1_ucc_L1TE10000',    'L1_TE10000',   [], [MinBiasStream, 'express'],["BW:UltraCentral", "RATE:UltraCentral"], 1],
	['hi_th2_ucc_L1TE10000',    'L1_TE10000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
	['hi_th3_ucc_L1TE10000',    'L1_TE10000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],

	['hi_th1_ucc_L1TE12000',    'L1_TE12000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        ['hi_th2_ucc_L1TE12000',    'L1_TE12000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        ['hi_th3_ucc_L1TE12000',    'L1_TE12000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],

	['hi_th1_ucc_L1TE14000',    'L1_TE14000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        ['hi_th2_ucc_L1TE14000',    'L1_TE14000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        ['hi_th3_ucc_L1TE14000',    'L1_TE14000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],

	#backup (ATR-12547)
	['hi_th1_zdcpu_ucc_L1TE10000',    'L1_TE10000',   [], [MinBiasStream, 'express'],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        ['hi_th2_zdcpu_ucc_L1TE10000',    'L1_TE10000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        ['hi_th3_zdcpu_ucc_L1TE10000',    'L1_TE10000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],

        ['hi_th1_zdcpu_ucc_L1TE12000',    'L1_TE12000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        ['hi_th2_zdcpu_ucc_L1TE12000',    'L1_TE12000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        ['hi_th3_zdcpu_ucc_L1TE12000',    'L1_TE12000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
    
        ['hi_th1_zdcpu_ucc_L1TE14000',    'L1_TE14000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        ['hi_th2_zdcpu_ucc_L1TE14000',    'L1_TE14000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        ['hi_th3_zdcpu_ucc_L1TE14000',    'L1_TE14000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],

	#performance/calibration
	['hi_perf_ucc_L1TE10000',    'L1_TE10000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
	['hi_perf_ucc_L1ZDC_A_C',    'L1_ZDC_A_C',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        ['hi_perf_ucc_L1All',    '',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        ['hi_perfzdc_ucc_L1ZDC_A_C',    'L1_ZDC_A_C',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        ['hi_perfzdc_ucc_L1All',    '',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        
	#ultra-peripheral
        ['hi_loose_upc_L1ZDC_A_C_VTE50',     'L1_ZDC_A_C_VTE50',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	['hi_medium_upc_L1ZDC_A_C_VTE50',    'L1_ZDC_A_C_VTE50',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	['hi_tight_upc_L1ZDC_A_C_VTE50',     'L1_ZDC_A_C_VTE50',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],

	# ATR-12727
	['hi_gg_upc_L1ZDC_A_C_VTE50',     'L1_ZDC_A_C_VTE50',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	['hi_gg_upc_L1TE5_VTE200',        'L1_TE5_VTE200',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	['hi_gg_upc_L1All',               '',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],

	# validation trigger
	['hi_loose_upc_L1All',     '',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
        ]
    
    TriggerFlags.MinBiasSlice.signatures = [
        ['mb_mbts_L1MBTS_2',    'L1_MBTS_2',   [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_1_1',  'L1_MBTS_1_1', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1], 
        ['mb_mbts_L1MBTS_2_2',  'L1_MBTS_2_2', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_3_3',  'L1_MBTS_3_3', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_4_4',  'L1_MBTS_4_4', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],

	#sptrk
        ['mb_sp',  'L1_RD0_FILLED', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_ion_L1ZDC_A_C_VTE50',  'L1_ZDC_A_C_VTE50', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_ion_L1MBTS_1_1_VTE50', 'L1_MBTS_1_1_VTE50', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_idperf_ion_L1MBTS_1', 'L1_MBTS_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_idperf_ion_L1MBTS_1_1', 'L1_MBTS_1_1', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        
        ['mb_perf_L1RD1_FILLED',    'L1_RD1_FILLED',   [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_perf_L1RD1_EMPTY',    'L1_RD1_EMPTY',   [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],

	#supporting ATR-12470, ATR-12923
	['mb_sptrk_ion_L1ZDC_A_C_TE5_VTE200',  'L1_ZDC_A_C_TE5_VTE200', [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	['mb_sptrk_ion_L1ZDC_XOR_TE5_VTE200',  'L1_ZDC_XOR_TE5_VTE200', [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	['mb_sptrk_ion_L1ZDC_VTE200',  'L1_ZDC_VTE200', [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],

	#overlay
	['mb_sptrk_ion_L1ZDC_A_C_VTE50_OVERLAY',  'L1_ZDC_A_C_VTE50_OVERLAY', [], ['MinBiasOverlay'],["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],

        #Zdc
	['mb_lg_zdcperf_L1All',  '', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_lg_zdcperf_L1ZDC_A_C',  'L1_ZDC_A_C', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_hg_zdcperf_L1All',  '', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_hg_zdcperf_L1ZDC_A_C',  'L1_ZDC_A_C', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],

	#UPC ATR-10574
	['mb_sptrk_vetombts2in_L1MU0_VTE50',                 'L1_MU0_VTE50',   [], [UPCStream, 'express'], ["BW:UPC", "RATE:UPC"], -1],
	['mb_sptrk_vetombts2in_L1ZDC_A_C_VTE50',             'L1_ZDC_A_C_VTE50',   [], [UPCStream], ["BW:UPC", "RATE:UPC"], -1],

	#photo-production ATR-12470
	['mb_sptrk_vetombts1side2in_L1ZDC_XOR_TE5_VTE200',        'L1_ZDC_XOR_TE5_VTE200',   [], [UPCStream], ["BW:UPC", "RATE:UPC"], -1],
	['mb_sptrk_vetombts1side2in_L1ZDC_XOR_TRT_VTE200',        'L1_ZDC_XOR_TRT_VTE200',   [], [UPCStream], ["BW:UPC", "RATE:UPC"], -1],

        ]

    TriggerFlags.CalibSlice.signatures   = [

	['idcalib_trk9_central_L1J10_VTE100',    'L1_J10_VTE100', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk9_fwd_L1J10_VTE100',        'L1_J10_VTE100', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
	['idcalib_trk9_central_L1J10_VTE200',    'L1_J10_VTE200', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk9_fwd_L1J10_VTE200',        'L1_J10_VTE200', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],

        ['idcalib_trk9_central',    'L1_TE5_VTE200', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk9_fwd',        'L1_TE5_VTE200', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk16_central',   'L1_TE5_VTE200', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk16_fwd',       'L1_TE5_VTE200', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk29_central',   'L1_TE5_VTE200', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk29_fwd',       'L1_TE5_VTE200', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],

        ['lumipeb_L1RD0_EMPTY',        'L1_RD0_EMPTY', [], ['PixelNoise'], ["RATE:Calibration", "RATE:PixelCalibration", "BW:Detector"], -1],
        ['lumipeb_L1RD0_FILLED',         'L1_RD0_FILLED', [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        ['lumipeb_L1RD0_UNPAIRED_ISO',    'L1_RD0_UNPAIRED_ISO',    [], ['PixelBeam'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_L1RD0_ABORTGAPNOTCALIB',    'L1_RD0_ABORTGAPNOTCALIB',    [], ['PixelBeam'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_L1MBTS_1_1',    'L1_MBTS_1_1',    [], ['PixelBeam'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_L1MBTS_1_1_UNPAIRED_ISO',    'L1_MBTS_1_1_UNPAIRED_ISO',    [], ['PixelBeam'], ["RATE:Calibration", "BW:Detector"], -1],


        # triggers for VdM scan
        ['lumipeb_vdm_L1MBTS_2_BGRP9',          'L1_MBTS_2_BGRP9',          [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2_BGRP11',         'L1_MBTS_2_BGRP11',         [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2_UNPAIRED_ISO',   'L1_MBTS_2_UNPAIRED_ISO',   [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_1_1_BGRP9',        'L1_MBTS_1_1_BGRP9',        [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_1_1_BGRP11',       'L1_MBTS_1_1_BGRP11',       [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_1_1_UNPAIRED_ISO', 'L1_MBTS_1_1_UNPAIRED_ISO', [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1ZDC_A_C_BGRP9',         'L1_ZDC_A_C_BGRP9',         [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1ZDC_A_C_BGRP11',        'L1_ZDC_A_C_BGRP11',        [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1ZDC_A_C_UNPAIRED_ISO',  'L1_ZDC_A_C_UNPAIRED_ISO',  [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1RD0_BGRP9',             'L1_RD0_BGRP9',             [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2',                'L1_MBTS_2',                [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_1_1',              'L1_MBTS_1_1',              [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1ZDC_A_C',               'L1_ZDC_A_C',               [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],

        
        # Monitoritems for saturated towers
        ['satu20em_l1satmon_L1J100_FIRSTEMPTY',  'L1_J100_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['satu20em_l1satmon_L1J100.31ETA49_FIRSTEMPTY',  'L1_J100.31ETA49_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['satu20em_l1satmon_L1EM20VH_FIRSTEMPTY',  'L1_EM20VH_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        #['satu20em_l1satmon_L1EM20VHI_FIRSTEMPTY',  'L1_EM20VHI_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1], 
        # the one above was replaced with a em22vhi_firstempty seeded one:
        #['satu20em_l1satmon_L1EM22VHI_FIRSTEMPTY',  'L1_EM22VHI_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1], # for HI, L1EM20VH one is OK?
        ['satu20em_l1satmon_L1J100',  'L1_J100',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['satu20em_l1satmon_L1J100.31ETA49',  'L1_J100.31ETA49',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],                 
        # larnoiseburst
        ['larnoiseburst_L1XE35',            'L1_XE35',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1XE50',            'L1_XE50',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        #['larnoiseburst_L1XE80', 'L1_XE80', [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1], # for HI, XE50 is OK?
        ['larnoiseburst_L1J50',             'L1_J50',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J75',             'L1_J75',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J100', 'L1_J100', [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J40_XE50',            'L1_J40_XE50',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1], 
        ['larnoiseburst_L1J75_XE40',            'L1_J75_XE40',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1], 

        # larpeb
        ['g8_loose_larpeb',                      'L1_EM7',     [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g12_loose_larpeb',                     'L1_EM10',     [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_loose_larpeb_L1EM15',              'L1_EM15', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g40_loose_larpeb',                     'L1_EM20', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g60_loose_larpeb',                     'L1_EM20', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g80_loose_larpeb',                     'L1_EM20', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['conej40_larpebj',  'L1_J12',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['conej165_larpebj', 'L1_J75',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['conej75_320eta490_larpebj',  'L1_J30.31ETA49',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['conej140_320eta490_larpebj', 'L1_J75.31ETA49',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        
        # larcalib
        ['larcalib_L1EM7_EMPTY',    'L1_EM7_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1TAU12_EMPTY',   'L1_TAU12_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1J12_EMPTY',    'L1_J12_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1J3031ETA49_EMPTY',  'L1_J30.31ETA49_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        
        #Special re-run chains
        ['larnoiseburst_rerun',                  '',             [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_loose_rerun',            '',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],          
        
        ]
    
    TriggerFlags.CosmicSlice.signatures  = [        
        ['tilecalib_laser',   'L1_CALREQ2', [], ['Tile'], ["RATE:Calibration", "RATE:Cosmic_TileCalibration", "BW:Detector"], -1], 
        ['sct_noise',          'L1_RD0_EMPTY', [], ['SCTNoise'], ["RATE:Calibration", "RATE:SCTCalibration", "BW:Detector"], -1],
        # CosmicID
        ['id_cosmicid_L1MU4_EMPTY',         'L1_MU4_EMPTY', [],  ['HLT_IDCosmic', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],
        ['id_cosmicid_L1MU11_EMPTY',        'L1_MU11_EMPTY', [], ['HLT_IDCosmic', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],

        # CosmicCalo
        ['larps_L1EM7_EMPTY',  'L1_EM7_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J12_EMPTY',  'L1_J12_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J30_EMPTY',  'L1_J30_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1TAU12_EMPTY', 'L1_TAU12_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],        
        ['larps_L1J3031ETA49_EMPTY', 'L1_J30.31ETA49_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J12_FIRSTEMPTY', 'L1_J12_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J30_FIRSTEMPTY', 'L1_J30_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1], 
        ['larps_L1EM7_FIRSTEMPTY',  'L1_EM7_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1], 
        
        ['larhec_L1J12_FIRSTEMPTY', 'L1_J12_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larhec_L1J30_FIRSTEMPTY', 'L1_J30_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larhec_L1EM7_FIRSTEMPTY', 'L1_EM7_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larhec_L1TAU12_FIRSTEMPTY', 'L1_TAU12_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],


        
        
        ]
    TriggerFlags.StreamingSlice.signatures = [
	#TE full calo
	['noalg_mb_L1TE5',      'L1_TE5', [], [MinBiasStream, 'express'], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE20',      'L1_TE20', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1TE50',      'L1_TE50', [], [MinBiasStream, 'express'], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE100',      'L1_TE100', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE200',      'L1_TE200', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE10000',      'L1_TE10000', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE12000',      'L1_TE12000', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE14000',      'L1_TE14000', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],

	#TE restricted calo
        ['noalg_mb_L1TE500.0ETA49',      'L1_TE500.0ETA49', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE1500.0ETA49',      'L1_TE1500.0ETA49', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE3000.0ETA49',      'L1_TE3000.0ETA49', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE3500.0ETA49',      'L1_TE3500.0ETA49', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE5000.0ETA49',      'L1_TE5000.0ETA49', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE6500.0ETA49',      'L1_TE6500.0ETA49', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE8000.0ETA49',      'L1_TE8000.0ETA49', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE9000.0ETA49',      'L1_TE9000.0ETA49', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],

	#min bias
        ['noalg_mb_L1MBTS_2',    'L1_MBTS_2', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1MBTS_1_1',  'L1_MBTS_1_1', [], [MinBiasStream, 'express'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1MBTS_2_2',  'L1_MBTS_2_2', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1ZDC_A_C',   'L1_ZDC_A_C', [], [MinBiasStream, 'express'], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1ZDC_A',     'L1_ZDC_A', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1ZDC_C',     'L1_ZDC_C', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1ZDC_AND',   'L1_ZDC_AND', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
        ['zdcpeb_L1ZDC_A',      'L1_ZDC_A', [], ['zdcCalib'], ["BW:Detector", "RATE:Calibration"], -1],
        ['zdcpeb_L1ZDC_C',      'L1_ZDC_C', [], ['zdcCalib'], ["BW:Detector", "RATE:Calibration"], -1],
        ['zdcpeb_L1ZDC_AND',    'L1_ZDC_AND', [], ['zdcCalib'], ["BW:Detector", "RATE:Calibration"], -1],
        ['zdcpeb_L1ZDC_A_C',    'L1_ZDC_A_C', [], ['zdcCalib'], ["BW:Detector", "RATE:Calibration"], -1],
	['noalg_mb_L1RD0_FILLED',      'L1_RD0_FILLED', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1RD0_EMPTY',       'L1_RD0_EMPTY', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1RD0_UNPAIRED_ISO','L1_RD0_UNPAIRED_ISO', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1LUCID',              'L1_LUCID', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1LUCID_EMPTY',        'L1_LUCID_EMPTY', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1LUCID_UNPAIRED_ISO', 'L1_LUCID_UNPAIRED_ISO', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],

	#UPC+photoproduction
	['noalg_L1ZDC_A_C_VTE50',   'L1_ZDC_A_C_VTE50', [], [UPCStream], ["BW:UPC", "RATE:UPC"], 1],
	['noalg_L1ZDC_XOR',         'L1_ZDC_XOR', [], [UPCStream], ["BW:UPC", "RATE:UPC"], 1],
	['noalg_L1TE5_VTE200',      'L1_TE5_VTE200', [], [UPCStream], ["BW:UPC", "RATE:UPC"], 1],
	['noalg_L1ZDC_A_C_TE5_VTE200',   'L1_ZDC_A_C_TE5_VTE200', [], [UPCStream], ["BW:UPC", "RATE:UPC"], 1],
	['noalg_L1ZDC_XOR_TE5_VTE200',   'L1_ZDC_XOR_TE5_VTE200', [], [UPCStream], ["BW:UPC", "RATE:UPC"], 1],
	['noalg_L1ZDC_XOR_TRT_VTE200',   'L1_ZDC_XOR_TRT_VTE200', [], [UPCStream], ["BW:UPC", "RATE:UPC"], 1],

	#beam spot
	['noalg_L1J10_VTE100',   'L1_J10_VTE100', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	['noalg_L1J10_VTE200',   'L1_J10_VTE200', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	['noalg_L1MU0_VTE50',   'L1_MU0_VTE50', [], [UPCStream], ["BW:UPC", "RATE:UPC"], 1],

	#muon
	['noalg_L1MU4',    'L1_MU4', [],   [HardProbesStream, 'express'], ["BW:SingleMuon", "RATE:Muon"], 1],
	['noalg_L1MU6',    'L1_MU6', [],  [HardProbesStream, 'express'], ["BW:SingleMuon", "RATE:Muon"], 1],	
	['noalg_L1MU11',   'L1_MU4', [],  [HardProbesStream, 'express'], ["BW:SingleMuon", "RATE:Muon"], 1],
	['noalg_L1MU15',   'L1_MU15', [], [HardProbesStream], ["BW:SingleMuon", "RATE:Muon"], 1],
	['noalg_L1MU20',   'L1_MU20', [], [HardProbesStream], ["BW:SingleMuon", "RATE:Muon"], 1],


	#jets
	['noalg_L1J10',   'L1_J10', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	['noalg_L1J15',   'L1_J15', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
        ['noalg_L1J20',   'L1_J20', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
        ['noalg_L1J30',   'L1_J30', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	['noalg_L1J35',   'L1_J35', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	['noalg_L1J50',   'L1_J50', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	['noalg_L1J75',   'L1_J75', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	['noalg_L1J175',   'L1_J175', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],

	#egamma
	['noalg_L1EM7',    'L1_EM7',  [], [HardProbesStream], ["BW:SingleElectron", "RATE:Egamma"], 1],
        ['noalg_L1EM8',    'L1_EM8',  [], [HardProbesStream], ["BW:SingleElectron", "RATE:Egamma"], 1],
	['noalg_L1EM10',   'L1_EM10', [], [HardProbesStream, 'express'], ["BW:SingleElectron", "RATE:Egamma"], 1],
	['noalg_L1EM12',   'L1_EM12', [], [HardProbesStream, 'express'], ["BW:SingleElectron", "RATE:Egamma"], 1],
        ['noalg_L1EM14',   'L1_EM14', [], [HardProbesStream, 'express'], ["BW:SingleElectron", "RATE:Egamma"], 1],
	['noalg_L1EM16',   'L1_EM16', [], [HardProbesStream], ["BW:SingleElectron", "RATE:Egamma"], 1],
	['noalg_L1EM18',   'L1_EM18', [], [HardProbesStream], ["BW:SingleElectron", "RATE:Egamma"], 1],
	['noalg_L1EM20',   'L1_EM20', [], [HardProbesStream], ["BW:SingleElectron", "RATE:Egamma"], 1],

	#data min-bias overlay
	['noalg_L1ZDC_A_C_OVERLAY',   'L1_ZDC_A_C_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
	['noalg_L1TE50_OVERLAY',   'L1_TE50_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
	
	#TE restricted calo in |eta|<4.9   
        ['noalg_mb_L1TE500.0ETA49_OVERLAY',       'L1_TE500.0ETA49_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
        ['noalg_mb_L1TE1500.0ETA49_OVERLAY',      'L1_TE1500.0ETA49_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
        ['noalg_mb_L1TE3000.0ETA49_OVERLAY',      'L1_TE3000.0ETA49_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
        ['noalg_mb_L1TE3500.0ETA49_OVERLAY',      'L1_TE3500.0ETA49_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
        ['noalg_mb_L1TE5000.0ETA49_OVERLAY',      'L1_TE5000.0ETA49_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
        ['noalg_mb_L1TE6500.0ETA49_OVERLAY',      'L1_TE6500.0ETA49_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
        ['noalg_mb_L1TE8000.0ETA49_OVERLAY',      'L1_TE8000.0ETA49_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
        ['noalg_mb_L1TE9000.0ETA49_OVERLAY',      'L1_TE9000.0ETA49_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],

	#data zero bias overlay
	['noalg_zb_L1ZB',   'L1_ZB', [], ['ZeroBias'], ["BW:ZeroBias", "RATE:ZeroBias"], 1],

        ['noalg_L1TGC_BURST',   'L1_TGC_BURST', [], ['TgcNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],

        #idmon streamer
        ['noalg_idmon_L1RD0_EMPTY',          'L1_RD0_EMPTY',        [], ['IDMonitoring','express'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['noalg_idmon_L1RD0_FILLED',         'L1_RD0_FILLED',        [], ['IDMonitoring'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['noalg_idmon_L1RD0_UNPAIRED_ISO',   'L1_RD0_UNPAIRED_ISO', [], ['IDMonitoring'], ["RATE:Monitoring", "BW:Detector"], -1],

        # Cosmic calo streamer
        ['noalg_cosmiccalo_L1EM7_EMPTY',         'L1_EM7_EMPTY',        [], ['CosmicCalo'], ["BW:MinBias", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1RD1_EMPTY',         'L1_RD1_EMPTY',        [], ['CosmicCalo', 'express'], ["RATE:Calibration", "BW:Detector"], -1],
        ['noalg_cosmiccalo_L1J3031ETA49_EMPTY',  'L1_J30.31ETA49_EMPTY',[], ['CosmicCalo'], ["BW:Jet", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1J12_EMPTY',         'L1_J12_EMPTY',        [], ['CosmicCalo', 'express'], ["BW:Jet", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1J30_EMPTY',         'L1_J30_EMPTY',        [], ['CosmicCalo', 'express'], ["BW:Jet", "RATE:Cosmic_Calo"], -1],

        #background streamers
        ['noalg_bkg_L1Bkg',               'L1_Bkg',               [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1],  
        ['noalg_bkg_L1J12_BGRP12', 'L1_J12_BGRP12', [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_bkg_L1J30.31ETA49_BGRP12', 'L1_J30.31ETA49_BGRP12', [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_L1Calo', 'L1_Calo', [], ['L1Calo'], ["RATE:SeededStreamers", "BW:Other"], -1], 

        #DCM level monitoring test chain
        ['noalg_dcmmon_L1RD0_EMPTY',   'L1_RD0_EMPTY',        [], ['monitoring_random'], ["RATE:DISCARD","BW:DISCARD"], -1],
        
	#standby
	['noalg_standby_L1RD0_FILLED', 'L1_RD0_FILLED', [], ['Standby'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_standby_L1RD0_EMPTY', 'L1_RD0_EMPTY', [], ['Standby'], ["RATE:CoMinBias", "BW:MinBias"], -1],
	['noalg_L1Standby',              'L1_Standby',          [], ['Standby'],    ["RATE:SeededStreamers", "BW:Other"], -1],

 #       ['noalg_L1Topo',                           'L1_Topo',             [], ['L1Calo'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
#        ['noalg_larcells_L1LAR-EM',                'L1_LAR-EM',           [], ['LArCells'],     ["RATE:SeededStreamers", "BW:Other"], -1],
#        ['noalg_larcells_L1LAR-J',                 'L1_LAR-J',            [], ['LArCells'],     ["RATE:SeededStreamers", "BW:Other"], -1],

        # HI EB
        ['noalg_eb_L1ZDC_A_C_VTE50', 'L1_ZDC_A_C_VTE50', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1TE5_VTE200', 'L1_TE5_VTE200', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1ZDC_XOR_TE5_VTE200', 'L1_ZDC_XOR_TE5_VTE200', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1MU0_VTE50', 'L1_MU0_VTE50', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1EM12', 'L1_EM12', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1MU4', 'L1_MU4', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1TE50', 'L1_TE50', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1TE12000', 'L1_TE12000', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],

        ]

    TriggerFlags.BeamspotSlice.signatures = [
        ['beamspot_activeTE_trkfast_peb',           'L1_J10_VTE100', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_pebTRT',           'L1_J10_VTE100', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_trkFS_trkfast',                  'L1_J10_VTE100', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

	['beamspot_activeTE_trkfast_peb_L1J10_VTE200',           'L1_J10_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_pebTRT_L1J10_VTE200',           'L1_J10_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_trkFS_trkfast_L1J10_VTE200',                  'L1_J10_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

	['beamspot_activeTE_trkfast_peb_L1TE5_VTE200',           'L1_TE5_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_pebTRT_L1TE5_VTE200',           'L1_TE5_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_trkFS_trkfast_L1TE5_VTE200',                  'L1_TE5_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

	]

    TriggerFlags.MonitorSlice.signatures = [
        ['l1calooverflow','', [], ['L1Calo'], ['RATE:Monitoring','BW:Other'], -1],
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


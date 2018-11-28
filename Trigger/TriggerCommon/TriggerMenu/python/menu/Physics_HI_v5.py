# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------------------------------------------------------------------#
def setupMenu():
    
    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenu.menu.Physics_HI_v5.py' )

    from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1 as tcl1
    
    if tcl1.current:
        log.info("L1 items: %s " % tcl1.current.menu.items.itemNames())
    else:
        log.info("ERROR L1 menu has not yet been defined")

    HardProbesStream="HardProbes"
    MinBiasStream="MinBias"
    UPCStream="UPC"
    UCCStream="UCC"
    PCStream="PC"
    CCStream="CC"
    PCpebStream="PCPEB"
    CCpebStream="CCPEB"

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
    ['mu4_idperf',          'L1_MU4', [], [HardProbesStream, 'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu6_idperf',          'L1_MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu10_idperf',         'L1_MU6', [], [HardProbesStream, 'express'], ['RATE:SingleMuon','BW:Muon'], -1],

	#RoI muons
	['mu3',                 'L1_MU4', [], [HardProbesStream, 'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu4',                 'L1_MU4', [], [HardProbesStream,  'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu6',                 'L1_MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu8',                 'L1_MU6', [], [HardProbesStream, 'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu10',                'L1_MU6', [], [HardProbesStream,  'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu14',                'L1_MU11', [], [HardProbesStream, 'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu20',                'L1_MU11', [], [HardProbesStream, 'express'], ['RATE:SingleMuon','BW:Muon'], -1],

	#msonly
	['mu15_msonly',            'L1_MU11',           [], [HardProbesStream, 'express'], ['RATE:SingleMuon', 'BW:Muon'], -1],
	['mu20_msonly',            'L1_MU11',           [], [HardProbesStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

	#di-muons RoI based
	['2mu3',                 'L1_2MU4', [], [HardProbesStream, 'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['2mu4',                 'L1_2MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['2mu6',                 'L1_2MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['2mu14',                 'L1_2MU10', [], [HardProbesStream, 'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['2mu4_msonly',          'L1_2MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
    ['2mu6_msonly',          'L1_2MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],

	#Full scan
	['mu4_mu2noL1',         'L1_MU4', ['L1_MU4',''], [HardProbesStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial',-1,["mu4","mu2noL1"]]],
    ['mu4_mu4noL1',         'L1_MU4', ['L1_MU4',''], [HardProbesStream, 'express'], ['RATE:MultiMuon','BW:Muon'], -1, ['serial',-1,["mu4","mu4noL1"]]],
    ['mu6_mu4noL1',         'L1_MU4', ['L1_MU4',''], [HardProbesStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial',-1,["mu6","mu4noL1"]]],
        
        #Full-scan supporting
	['mu2noL1',         'L1_MU4', ['L1_MU4',''], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1, ],
    ['mu4noL1',         'L1_MU4', ['L1_MU4',''], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1, ],
    ['mu6noL1',         'L1_MU4', ['L1_MU4',''], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1, ],

    # muon calibration
    ['mu0_muoncalib',             'L1_MU6',      [],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
    #['mu0_muoncalib_L1MU15',      'L1_MU15',     [],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
    #['mu0_muoncalib_L1MU4_EMPTY', 'L1_MU4_EMPTY',[],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
    
    #UPC muons
    ['mu4_L1MU4_VZDC_AORC_VTE200',                 'L1_MU4_VZDC_AORC_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
    ['mu4_L1MU4_VTE50',                 'L1_MU4_VTE50',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
    ['mu3_mu3noL1_L1MU4_VTE50',         'L1_MU4_VTE50', ['L1_MU4',''], [HardProbesStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial',-1,["mu3","mu3noL1"]]],
    ['2mu3_L12MU4_VTE50',                 'L1_2MU4_VTE50',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
    ['2mu4_L12MU4_VTE50',                 'L1_2MU4_VTE50',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
    ['mu4_mu4noL1_L1MU4_VTE50',                 'L1_MU4_VTE50',['L1_MU4',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1, ['serial',-1,["mu4","mu4noL1"]]],
    ['mu6_L1MU4_VTE50',                 'L1_MU4_VTE50',[], [UPCStream, 'express'], ['RATE:UPC', 'BW:UPC'], -1],
    ['mu8_L1MU6_VTE50',                 'L1_MU6_VTE50',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],    
        
        ]

    TriggerFlags.JetSlice.signatures = [
	#performance
	#['j0_perf_L1ZDC_A_C',     'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j0_perf_L1RD0_FILLED',     'L1_RD0_FILLED',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],

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
	['j75_ion_L1TE50',         'L1_TE50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j85_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j100_ion_L1TE50',         'L1_TE50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j150_ion_L1TE50',         'L1_TE50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j50_ion_L1J12',         'L1_J12',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j60_ion_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j60_ion_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j75_ion_L1J20',         'L1_J20',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j75_ion_L1J30',         'L1_J30',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j85_ion_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j85_ion_L1J30',         'L1_J30',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j100_ion_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j100_ion_L1J30',         'L1_J30',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j110_ion_L1J30',         'L1_J30',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j110_ion_L1J50',         'L1_J50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j120_ion_L1J30',         'L1_J30',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j120_ion_L1J50',         'L1_J50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j130_ion_L1J30',         'L1_J30',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j150_ion_L1J30',         'L1_J30',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j150_ion_L1J50',         'L1_J50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j180_ion_L1J50',         'L1_J50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j200_ion_L1J50',         'L1_J50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],		
	['j30_ion_L1ZDC_A_C',         'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_ion_L1ZDC_A_C',         'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j50_ion_L1ZDC_A_C',         'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j60_ion_L1ZDC_A_C',         'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j75_ion_L1ZDC_A_C',         'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	#Large R jets
	['j120_a10_ion_L1J30',         'L1_J30',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j120_a10_ion_L1J50',         'L1_J50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j150_a10_ion_L1J50',         'L1_J50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j180_a10_ion_L1J50',         'L1_J50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j200_a10_ion_L1J50',         'L1_J50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],		
	

	#forward jets ATR-12623
	['j25_320eta490_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_320eta490_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_320eta490_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
    ['j30_320eta490_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_320eta490_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
    ['j40_320eta490_ion_L1TE50',         'L1_TE50',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j50_320eta490_ion_L1TE20',         'L1_TE20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
    ['j50_320eta490_ion_L1TE50',         'L1_TE50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
    
    ['j50_320eta490_ion',         'L1_J15.31ETA49',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
    ['j60_320eta490_ion',         'L1_J20.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
    ['j70_320eta490_ion',         'L1_J20.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
    ['j100_320eta490_ion',         'L1_J30.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
    ['j170_320eta490_ion',         'L1_J50.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],


	#multijets
	['j60_ion_2j30_ion_L1TE50',    'L1_TE50', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j75_ion_2j30_ion_L1TE50',    'L1_TE50', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j75_ion_2j40_ion_L1TE50',    'L1_TE50', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j100_ion_2j30_ion_L1TE50',   'L1_TE50', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j100_ion_2j40_ion_L1TE50',   'L1_TE50', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j100_ion_2j50_ion_L1TE50',   'L1_TE50', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],

	['j60_ion_2j30_ion_L1J12',     'L1_J12', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j75_ion_2j30_ion_L1J12',     'L1_J12', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j75_ion_2j40_ion_L1J12',     'L1_J12', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j100_ion_2j30_ion_L1J12',    'L1_J12', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j100_ion_2j40_ion_L1J12',    'L1_J12', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j100_ion_2j50_ion_L1J12',    'L1_J12', [], [HardProbesStream], ['RATE:MultiJet',  'BW:Jet'], -1],

    #photoproduction
    #['j5_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
    ['j10_rcu4_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
    ['j10_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j15_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j20_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
		
    #['j5_L1ZDC_XOR_TE5_VTE200',                 'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
    ['j10_rcu4_L1ZDC_XOR_TE5_VTE200',                 'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream, 'express'], ['RATE:UPC', 'BW:UPC'], -1],
    ['j10_L1ZDC_XOR_TE5_VTE200',                 'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream, 'express'], ['RATE:UPC', 'BW:UPC'], -1],
	['j15_L1ZDC_XOR_TE5_VTE200',                 'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j20_L1ZDC_XOR_TE5_VTE200',                 'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j30_L1ZDC_XOR_TE20_VTE200',                 'L1_ZDC_XOR_TE20_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j40_L1ZDC_XOR_TE20_VTE200',                 'L1_ZDC_XOR_TE20_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	
	['j10_rcu4_L1VZDC_A_C_TE5_VTE200',                 'L1_VZDC_A_C_TE5_VTE200',[], [UPCStream,'express'], ['RATE:UPC', 'BW:UPC'], -1],
	['j10_L1VZDC_A_C_TE5_VTE200',                 'L1_VZDC_A_C_TE5_VTE200',[], [UPCStream,'express'], ['RATE:UPC', 'BW:UPC'], -1],
	['j15_L1VZDC_A_C_TE5_VTE200',                 'L1_VZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j20_L1VZDC_A_C_TE5_VTE200',                 'L1_VZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j30_L1VZDC_A_C_TE20_VTE200',                 'L1_VZDC_A_C_TE20_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	
	#supporting
	['j10_rcu4_L1ZDC_XOR_VTE200',                 'L1_ZDC_XOR_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j10_L1ZDC_XOR_VTE200',                 'L1_ZDC_XOR_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	
	#eta inclusive versions:
	['j10_rcu4_0eta490_L1ZDC_XOR_TE5_VTE200',                 'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream, 'express'], ['RATE:UPC', 'BW:UPC'], -1],
	['j10_0eta490_L1ZDC_XOR_TE5_VTE200',                 'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream, 'express'], ['RATE:UPC', 'BW:UPC'], -1],
	['j15_0eta490_L1ZDC_XOR_TE5_VTE200',                 'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j20_0eta490_L1ZDC_XOR_TE5_VTE200',                 'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j30_0eta490_L1ZDC_XOR_TE20_VTE200',                 'L1_ZDC_XOR_TE20_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j40_0eta490_L1ZDC_XOR_TE20_VTE200',                 'L1_ZDC_XOR_TE20_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	
	['j10_rcu4_0eta490_L1VZDC_A_C_TE5_VTE200',                 'L1_VZDC_A_C_TE5_VTE200',[], [UPCStream,'express'], ['RATE:UPC', 'BW:UPC'], -1],
	['j10_0eta490_L1VZDC_A_C_TE5_VTE200',                 'L1_VZDC_A_C_TE5_VTE200',[], [UPCStream,'express'], ['RATE:UPC', 'BW:UPC'], -1],
	['j15_0eta490_L1VZDC_A_C_TE5_VTE200',                 'L1_VZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j20_0eta490_L1VZDC_A_C_TE5_VTE200',                 'L1_VZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j30_0eta490_L1VZDC_A_C_TE20_VTE200',                 'L1_VZDC_A_C_TE20_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	
	['j10_rcu4_0eta490_L1ZDC_XOR_VTE200',                 'L1_ZDC_XOR_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j10_0eta490_L1ZDC_XOR_VTE200',                 'L1_ZDC_XOR_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	
	
    #single-jet large-R UPC trigger chains
    ['j10_a10_lcw_subjes_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j15_a10_lcw_subjes_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j20_a10_lcw_subjes_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
    
	['j10_a10_lcw_subjes_L1ZDC_XOR_TE5_VTE200',                 'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j15_a10_lcw_subjes_L1ZDC_XOR_TE5_VTE200',                 'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j20_a10_lcw_subjes_L1ZDC_XOR_TE5_VTE200',                 'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
    
    ['j10_a10_lcw_subjes_L1VZDC_A_C_TE5_VTE200',                 'L1_VZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j15_a10_lcw_subjes_L1VZDC_A_C_TE5_VTE200',                 'L1_VZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j20_a10_lcw_subjes_L1VZDC_A_C_TE5_VTE200',                 'L1_VZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	['j30_a10_lcw_subjes_L1VZDC_A_C_TE20_VTE200',                 'L1_VZDC_A_C_TE20_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
        
    #forward - to be added back if needed
    #['j10_320eta490_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
    #['j10_320eta490_L1ZDC_XOR_TE5_VTE200',         'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
    #['j15_320eta490_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
    #['j15_320eta490_L1ZDC_XOR_TE5_VTE200',         'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
    #['j20_320eta490_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
    #['j20_320eta490_L1ZDC_XOR_TE5_VTE200',         'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
    
	#photoproduction: di-jets - to be added back if needed
	#['2j10_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	#['2j15_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	#['2j20_L1TE5_VTE200',                 'L1_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],

	#['2j10_L1ZDC_XOR_TE5_VTE200',         'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	#['2j15_L1ZDC_XOR_TE5_VTE200',         'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	#['2j20_L1ZDC_XOR_TE5_VTE200',         'L1_ZDC_XOR_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],

	#['2j10_L1ZDC_A_C_TE5_VTE200',         'L1_ZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	#['2j15_L1ZDC_A_C_TE5_VTE200',         'L1_ZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
	#['2j20_L1ZDC_A_C_TE5_VTE200',         'L1_ZDC_A_C_TE5_VTE200',[], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1],
		
	#Jet triggers with MBTS vetos - to be added back if needed     
    #['j10_mb_mbts_vetombts1side2in_L1ZDC_XOR_TE5_VTE200',  'L1_ZDC_XOR_TE5_VTE200'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','j10']]],
    #['j15_mb_mbts_vetombts1side2in_L1ZDC_XOR_TE5_VTE200',  'L1_ZDC_XOR_TE5_VTE200'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','j15']]],
    #['j20_mb_mbts_vetombts1side2in_L1ZDC_XOR_TE5_VTE200',  'L1_ZDC_XOR_TE5_VTE200'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','j20']]],
	
	#['2j10_mb_mbts_vetombts1side2in_L1ZDC_XOR_TE5_VTE200',  'L1_ZDC_XOR_TE5_VTE200'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','2j10']]],
    #['2j15_mb_mbts_vetombts1side2in_L1ZDC_XOR_TE5_VTE200',  'L1_ZDC_XOR_TE5_VTE200'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','2j15']]],
    #['2j20_mb_mbts_vetombts1side2in_L1ZDC_XOR_TE5_VTE200',  'L1_ZDC_XOR_TE5_VTE200'  ,['',''], [UPCStream], ['RATE:UPC', 'BW:UPC'], -1,['serial',-1,[ 'mb_mbts_vetombts1side2in','2j20']]],
        ]

    TriggerFlags.BjetSlice.signatures = [
        ]

    TriggerFlags.METSlice.signatures = [
        ]

    TriggerFlags.TauSlice.signatures = [
       ]

    TriggerFlags.EgammaSlice.signatures = [
    
    ['g18_etcut',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
    
	#heavy ion instances	
	['e13_etcut_ion',       'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
    ['e15_etcut_ion',       'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
    ['e18_etcut_ion',              'L1_EM14', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
    ['e20_etcut_ion',              'L1_EM16', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
    ['e30_etcut_ion',              'L1_EM22', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
	['e50_etcut_ion',              'L1_EM22', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	
	#['g12_etcut_ion',              'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
    ['g13_etcut_ion',              'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g18_etcut_ion',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g28_etcut_ion',              'L1_EM16', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

    ['e15_loose_ion',       'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
    ['e15_medium_ion',      'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
    ['e15_lhloose_ion_L1EM12',     'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
    ['e15_lhmedium_ion_L1EM12',    'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
 
	#backup for e15
    ['e18_loose_ion',              'L1_EM14', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
    ['e18_medium_ion',             'L1_EM14', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
    ['e18_lhloose_ion',            'L1_EM14', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
    ['e18_lhmedium_ion',           'L1_EM14', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	#backup for e15
	
	['e20_loose_ion',              'L1_EM16', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e20_medium_ion',              'L1_EM16', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
    ['e20_lhloose_ion',            'L1_EM16', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
    ['e20_lhmedium_ion',            'L1_EM16', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	
	#monitoring
	['2e20_loose_ion',            'L1_2EM16', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	
	#idperf
	['e15_loose_ion_idperf',       'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
	['e18_loose_ion_idperf',       'L1_EM14', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
	['e20_loose_ion_idperf',       'L1_EM16', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
	
	
	['g15_loose_ion',              'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
    #['g15_medium_ion',             'L1_EM10', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
    ['g20_loose',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
    ['g20_loose_ion',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
    ['g30_loose_ion',              'L1_EM16', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
    #['g20_medium_ion',             'L1_EM12', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
    ['g50_loose_ion',              'L1_EM22', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
    
	#di-photon triggers
	['2g15_loose',                  'L1_2EM10', [], [HardProbesStream], ['RATE:MultiPhoton', 'BW:Egamma'], -1],
	['2g15_loose_ion',              'L1_2EM10', [], [HardProbesStream], ['RATE:MultiPhoton', 'BW:Egamma'], -1],

	# Monopole triggers ATR-12467
    #['g0_hiptrt_L1EM14',                    'L1_EM14', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	#['g0_hiptrt_L1EM16',                    'L1_EM16', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	
	#UPC electrons
    ['e12_lhloose_nod0_L1EM7_VTE200',     'L1_EM7_VTE200',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
    ['e12_lhloose_L1EM7_VTE200',     'L1_EM7_VTE200',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
    ['e12_loose_L1EM7_VTE200',     'L1_EM7_VTE200',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
    ['e12_medium_L1EM7_VTE200',     'L1_EM7_VTE200',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
    ['e12_loose_ion_L1EM7_VTE200',     'L1_EM7_VTE200',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
    ['e12_medium_ion_L1EM7_VTE200',     'L1_EM7_VTE200',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
    
    #UPC photons
    ['g12_loose_L1EM7_VTE200',     'L1_EM7_VTE200',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
    ['g12_medium_L1EM7_VTE200',     'L1_EM7_VTE200',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],

        ]

    TriggerFlags.BphysicsSlice.signatures = [
	['2mu4_bDimu',                    'L1_2MU4', [], [HardProbesStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1], 
    ['2mu4_bDimu_novtx_noos',         'L1_2MU4', [], [HardProbesStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],
	['mu4_mu4_idperf_bJpsimumu_noid',  'L1_2MU4', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring','BW:Bphys', 'BW:ID'], -1],
	['mu13_mu13_idperf_Zmumu',     'L1_2MU6', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring','BW:Bphys', 'BW:ID'], -1],
        ]

    TriggerFlags.CombinedSlice.signatures = [
	['mu4_j30_a2_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j30_a2_ion'] ]],
	['mu4_j40_a2_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j40_a2_ion'] ]],
	['mu4_j30_a3_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j30_a3_ion'] ]],
	['mu4_j40_a3_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream, 'express'], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j40_a3_ion'] ]],
	['mu4_j30_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j30_ion'] ]],
	['mu4_j40_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j40_ion'] ]],
    ['mu4_j50_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j50_ion'] ]],
    ['mu4_j50_ion_dr05_L1MU4_J12', 'L1_MU4_J12',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j50_ion'] ]],
	['mu4_j60_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream, 'express'], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j60_ion'] ]],
	['mu4_j60_ion_dr05_L1MU4_J15', 'L1_MU4_J15',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j60_ion'] ]],
	['mu6_j20_ion_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j20_ion'] ]],
	['mu6_j25_ion_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j25_ion'] ]],
	['mu6_j30_ion_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream, 'express'], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j30_ion'] ]],
	['mu6_j40_ion_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j40_ion'] ]],
	['mu6_j50_ion_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j50_ion'] ]],

	#supporting triggers
	['mu4_j30_a2_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j30_a2_ion'] ]],
    ['mu4_j40_a2_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j40_a2_ion'] ]],
	['mu4_j30_a3_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j30_a3_ion'] ]],
    ['mu4_j40_a3_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j40_a3_ion'] ]],
	['mu4_j30_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j30_ion'] ]],
    ['mu4_j40_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j40_ion'] ]],
	['mu4_j50_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j50_ion'] ]],
	['mu4_j50_ion_L1MU4_J12', 'L1_MU4_J12',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j50_ion'] ]],
	['mu4_j60_ion', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j60_ion'] ]],
	['mu4_j60_ion_L1MU4_J15', 'L1_MU4_J15',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j60_ion'] ]],
    ['mu6_j20_ion', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j20_ion'] ]],
    ['mu6_j25_ion', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j25_ion'] ]],
    ['mu6_j30_ion', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j30_ion'] ]],
    ['mu6_j40_ion', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j40_ion'] ]],
    ['mu6_j50_ion', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j50_ion'] ]],
            
     #UPC:Supporting
    ['hi_upc_FgapAC3_mb_sptrk_exclusiveloose2_L12TAU1_VTE50',        'L1_2TAU1_VTE50',   ['', ''], [UPCStream, 'express'],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapAC3','mb_sptrk_exclusiveloose2'],False]],
    ['hi_upc_FgapAC3_mb_sptrk_exclusiveloose2_L12TAU2_VTE200',        'L1_2TAU2_VTE200',   ['', ''], [UPCStream, 'express'],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapAC3','mb_sptrk_exclusiveloose2'],False]],
    ['hi_upc_FgapAC3_mb_sptrk_exclusiveloose1_L1ZDC_XOR_VTE50',        'L1_ZDC_XOR_VTE50',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapAC3','mb_sptrk_exclusiveloose1'],False]],
    ['hi_upc_FgapAC3_mb_sptrk_exclusiveloose2_L1ZDC_XOR_VTE50',        'L1_ZDC_XOR_VTE50',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapAC3','mb_sptrk_exclusiveloose2'],False]],
    ['hi_upc_FgapAC3_mb_sptrk_exclusiveloose1_L1MU4_VTE50',        'L1_MU4_VTE50',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapAC3','mb_sptrk_exclusiveloose1'],False]],
    ['hi_upc_FgapAC3_mb_sptrk_exclusiveloose2_L1MU4_VTE50',        'L1_MU4_VTE50',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapAC3','mb_sptrk_exclusiveloose2'],False]],
    ['hi_upc_FgapAC3_mb_sptrk_exclusiveloose2_L1ZDC_A_C_VTE50',        'L1_ZDC_A_C_VTE50',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapAC3','mb_sptrk_exclusiveloose2'],False]],
 	['hi_upc_FgapAC3_mb_sptrk_exclusiveloose2_L1VZDC_A_C_VTE50',        'L1_VZDC_A_C_VTE50',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapAC3','mb_sptrk_exclusiveloose2'],False]],
 	['mu4_hi_upc_FgapAC3_L1MU4_VTE50',        'L1_MU4_VTE50',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['mu4','hi_upc_FgapAC3'],False]],
 	['mu3_hi_upc_FgapAC3_L1MU4_VTE50',        'L1_MU4_VTE50',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['mu3','hi_upc_FgapAC3'],False]],
 	
 	
 	#UPC HMT
 	['mb_sp50_trk15_hmt_hi_upc_FgapC5_L1MBTS_1_ZDC_A_VZDC_C_VTE200',        'L1_MBTS_1_ZDC_A_VZDC_C_VTE200',   ['', ''], [UPCStream, 'express'],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapC5','mb_sp50_trk15_hmt'],False]],
    ['mb_sp50_trk15_hmt_hi_upc_FgapA5_L1MBTS_1_ZDC_C_VZDC_A_VTE200',        'L1_MBTS_1_ZDC_C_VZDC_A_VTE200',   ['', ''], [UPCStream, 'express'],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapA5','mb_sp50_trk15_hmt'],False]],
    ['mb_sp50_trk15_hmt_hi_upc_FgapC5_L1ZDC_A_VZDC_C_VTE200',        'L1_ZDC_A_VZDC_C_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapC5','mb_sp50_trk15_hmt'],False]],
    ['mb_sp50_trk15_hmt_hi_upc_FgapA5_L1ZDC_C_VZDC_A_VTE200',        'L1_ZDC_C_VZDC_A_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapA5','mb_sp50_trk15_hmt'],False]],
    ['mb_sp400_trk25_hmt_hi_upc_FgapC5_L1TE4_ZDC_A_VZDC_C_VTE200',        'L1_TE4_ZDC_A_VZDC_C_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapC5','mb_sp400_trk25_hmt'],False]],
    ['mb_sp400_trk25_hmt_hi_upc_FgapA5_L1TE4_ZDC_C_VZDC_A_VTE200',        'L1_TE4_ZDC_C_VZDC_A_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapA5','mb_sp400_trk25_hmt'],False]],
    ['mb_sp400_trk25_hmt_hi_upc_FgapC5_L1TE3.0ETA49_ZDC_A_VZDC_C_VTE200',        'L1_TE3.0ETA49_ZDC_A_VZDC_C_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapC5','mb_sp400_trk25_hmt'],False]],
    ['mb_sp400_trk25_hmt_hi_upc_FgapA5_L1TE3.0ETA49_ZDC_C_VZDC_A_VTE200',        'L1_TE3.0ETA49_ZDC_C_VZDC_A_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapA5','mb_sp400_trk25_hmt'],False]],
    ['mb_sp700_trk35_hmt_hi_upc_FgapC5_L1TE5_ZDC_A_VZDC_C_VTE200',        'L1_TE5_ZDC_A_VZDC_C_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapC5','mb_sp700_trk35_hmt'],False]],
    ['mb_sp700_trk35_hmt_hi_upc_FgapA5_L1TE5_ZDC_C_VZDC_A_VTE200',        'L1_TE5_ZDC_C_VZDC_A_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapA5','mb_sp700_trk35_hmt'],False]],
    ['mb_sp700_trk35_hmt_hi_upc_FgapC5_L1TE7.0ETA49_ZDC_A_VZDC_C_VTE200',        'L1_TE7.0ETA49_ZDC_A_VZDC_C_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapC5','mb_sp700_trk35_hmt'],False]],
    ['mb_sp700_trk35_hmt_hi_upc_FgapA5_L1TE7.0ETA49_ZDC_C_VZDC_A_VTE200',        'L1_TE7.0ETA49_ZDC_C_VZDC_A_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapA5','mb_sp700_trk35_hmt'],False]],

    ['mb_sp50_trk15_hmt_hi_upc_FgapC10_L1MBTS_1_ZDC_A_VZDC_C_VTE200',        'L1_MBTS_1_ZDC_A_VZDC_C_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapC10','mb_sp50_trk15_hmt'],False]],
    ['mb_sp50_trk15_hmt_hi_upc_FgapA10_L1MBTS_1_ZDC_C_VZDC_A_VTE200',        'L1_MBTS_1_ZDC_C_VZDC_A_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapA10','mb_sp50_trk15_hmt'],False]],
    ['mb_sp50_trk15_hmt_hi_upc_FgapC10_L1ZDC_A_VZDC_C_VTE200',        'L1_ZDC_A_VZDC_C_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapC10','mb_sp50_trk15_hmt'],False]],
    ['mb_sp50_trk15_hmt_hi_upc_FgapA10_L1ZDC_C_VZDC_A_VTE200',        'L1_ZDC_C_VZDC_A_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapA10','mb_sp50_trk15_hmt'],False]],
    ['mb_sp400_trk25_hmt_hi_upc_FgapC10_L1TE4_ZDC_A_VZDC_C_VTE200',        'L1_TE4_ZDC_A_VZDC_C_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapC10','mb_sp400_trk25_hmt'],False]],
    ['mb_sp400_trk25_hmt_hi_upc_FgapA10_L1TE4_ZDC_C_VZDC_A_VTE200',        'L1_TE4_ZDC_C_VZDC_A_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapA10','mb_sp400_trk25_hmt'],False]],
    ['mb_sp400_trk25_hmt_hi_upc_FgapC10_L1TE3.0ETA49_ZDC_A_VZDC_C_VTE200',        'L1_TE3.0ETA49_ZDC_A_VZDC_C_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapC10','mb_sp400_trk25_hmt'],False]],
    ['mb_sp400_trk25_hmt_hi_upc_FgapA10_L1TE3.0ETA49_ZDC_C_VZDC_A_VTE200',        'L1_TE3.0ETA49_ZDC_C_VZDC_A_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapA10','mb_sp400_trk25_hmt'],False]],
    ['mb_sp700_trk35_hmt_hi_upc_FgapC10_L1TE5_ZDC_A_VZDC_C_VTE200',        'L1_TE5_ZDC_A_VZDC_C_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapC10','mb_sp700_trk35_hmt'],False]],
    ['mb_sp700_trk35_hmt_hi_upc_FgapA10_L1TE5_ZDC_C_VZDC_A_VTE200',        'L1_TE5_ZDC_C_VZDC_A_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapA10','mb_sp700_trk35_hmt'],False]], 
    ['mb_sp700_trk35_hmt_hi_upc_FgapC10_L1TE7.0ETA49_ZDC_A_VZDC_C_VTE200',        'L1_TE7.0ETA49_ZDC_A_VZDC_C_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapC10','mb_sp700_trk35_hmt'],False]],
    ['mb_sp700_trk35_hmt_hi_upc_FgapA10_L1TE7.0ETA49_ZDC_C_VZDC_A_VTE200',        'L1_TE7.0ETA49_ZDC_C_VZDC_A_VTE200',   ['', ''], [UPCStream],["BW:UPC", "RATE:UPC"], 1, ['serial', -1, ['hi_upc_FgapA10','mb_sp700_trk35_hmt'],False]],

        ]
      
    TriggerFlags.HeavyIonSlice.signatures = [
	#event shape triggers
        ['hi_v2A_th0p_L1TE600.0ETA49',     'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2A_th001p_L1TE600.0ETA49',   'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2A_th01p_L1TE600.0ETA49',    'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2A_th025p_L1TE600.0ETA49',   'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2A_th05p_L1TE600.0ETA49',    'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2A_th10p_L1TE600.0ETA49',    'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],

        ['hi_v2C_th0p_L1TE600.0ETA49',     'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2C_th001p_L1TE600.0ETA49',   'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2C_th01p_L1TE600.0ETA49',    'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2C_th025p_L1TE600.0ETA49',   'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2C_th05p_L1TE600.0ETA49',    'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2C_th10p_L1TE600.0ETA49',    'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],


        ['hi_v3A_th0p_L1TE600.0ETA49',     'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3A_th001p_L1TE600.0ETA49',   'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3A_th01p_L1TE600.0ETA49',    'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3A_th025p_L1TE600.0ETA49',   'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3A_th05p_L1TE600.0ETA49',    'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3A_th10p_L1TE600.0ETA49',    'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],

        ['hi_v3C_th0p_L1TE600.0ETA49',     'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3C_th001p_L1TE600.0ETA49',   'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3C_th01p_L1TE600.0ETA49',    'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3C_th025p_L1TE600.0ETA49',   'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3C_th05p_L1TE600.0ETA49',    'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3C_th10p_L1TE600.0ETA49',    'L1_TE600.0ETA49',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],


	#ultra-central triggers
	#physics
        ['hi_th1_ucc_L1TE10000',    'L1_TE10000',   [], [UCCStream, 'express'],["BW:UltraCentral", "RATE:UltraCentral"], 1],
	    ['hi_th2_ucc_L1TE10000',    'L1_TE10000',   [], [UCCStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
	    ['hi_th3_ucc_L1TE10000',    'L1_TE10000',   [], [UCCStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],

	    ['hi_th1_ucc_L1TE12000',    'L1_TE12000',   [], [UCCStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        ['hi_th2_ucc_L1TE12000',    'L1_TE12000',   [], [UCCStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        ['hi_th3_ucc_L1TE12000',    'L1_TE12000',   [], [UCCStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],

	  
	#With zdc pileup rejection
	    #['hi_th1_zdcpu_ucc_L1TE10000',    'L1_TE10000',   [], [UCCStream, 'express'],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        #['hi_th2_zdcpu_ucc_L1TE10000',    'L1_TE10000',   [], [UCCStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        #['hi_th3_zdcpu_ucc_L1TE10000',    'L1_TE10000',   [], [UCCStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],

        #['hi_th1_zdcpu_ucc_L1TE12000',    'L1_TE12000',   [], [UCCStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        #['hi_th2_zdcpu_ucc_L1TE12000',    'L1_TE12000',   [], [UCCStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        #['hi_th3_zdcpu_ucc_L1TE12000',    'L1_TE12000',   [], [UCCStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
    
	#performance/calibration
	    ['hi_perf_ucc_L1TE10000',    'L1_TE10000',   [], [UCCStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
	    ['hi_perf_ucc_L1ZDC_A_C',    'L1_ZDC_A_C',   [], [UCCStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        ['hi_perf_ucc_L1All',    '',   [], [UCCStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        #['hi_perfzdc_ucc_L1ZDC_A_C',    'L1_ZDC_A_C',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        #['hi_perfzdc_ucc_L1All',    '',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        
	#ultra-peripheral
        #['hi_loose_upc_L1ZDC_A_C_VTE50',     'L1_ZDC_A_C_VTE50',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	    #['hi_medium_upc_L1ZDC_A_C_VTE50',    'L1_ZDC_A_C_VTE50',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	    #['hi_tight_upc_L1ZDC_A_C_VTE50',     'L1_ZDC_A_C_VTE50',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],

	#UPC:LbL
 	    ['hi_upc_FgapAC3_hi_gg_upc_L1TE5_VTE200',        'L1_TE5_VTE200',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc"]]],
 	    ['hi_upc_FgapAC3_hi_gg_upc_L1TE4_VTE200',        'L1_TE4_VTE200',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc"]]],
 	    ['hi_upc_FgapAC3_hi_gg_upc_L1TE5_VTE100',        'L1_TE5_VTE100',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc"]]],
 	    ['hi_upc_FgapAC3_hi_gg_upc_L1TE4_VTE100',        'L1_TE4_VTE100',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc"]]],
 	    ['hi_upc_FgapAC3_hi_gg_upc_L1TE5_VTE50',        'L1_TE5_VTE50',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc"]]],
 	    ['hi_upc_FgapAC3_hi_gg_upc_L1TE4_VTE50',        'L1_TE4_VTE50',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc"]]],
 	    ['hi_upc_FgapAC3_hi_gg_upc_L1TE50_VTE200',        'L1_TE50_VTE200',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc_noiseSup"]]],
 	    
 	    #nois suppresion ATR-18764: noise supression chains
 	    ['hi_upc_FgapAC3_hi_gg_upc_noiseSup_L1TE4_VTE200',        'L1_TE4_VTE200',   [], [UPCStream, 'express'],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc_noiseSup"]]],
 	    ['hi_upc_FgapAC3_hi_gg_upc_noiseSup_L1TE4_VTE50',        'L1_TE4_VTE50',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc_noiseSup"]]],
        ['hi_upc_FgapAC3_hi_gg_upc_noiseSup_L1TE5_VTE200',        'L1_TE5_VTE200',   [], [UPCStream, 'express'],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc_noiseSup"]]],
        ['hi_upc_FgapAC3_hi_gg_upc_noiseSup_L1TE5_VTE50',        'L1_TE5_VTE50',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc_noiseSup"]]],
        ['hi_upc_FgapAC3_hi_gg_upc_noiseSup_L12TAU1_VTE50',        'L1_2TAU1_VTE50',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc_noiseSup"]]],
        ['hi_upc_FgapAC3_hi_gg_upc_noiseSup_L1TE4_VTE200_EMPTY',        'L1_TE4_VTE200_EMPTY',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc_noiseSup"]]],
        ['hi_upc_FgapAC3_hi_gg_upc_noiseSup_L1TE5_VTE200_EMPTY',        'L1_TE5_VTE200_EMPTY',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc_noiseSup"]]],
        ['hi_upc_FgapAC3_hi_gg_upc_noiseSup_L1TAU1_TE4_VTE200',        'L1_TAU1_TE4_VTE200',   [], [UPCStream, 'express'],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc_noiseSup"]]],
        ['hi_upc_FgapAC3_hi_gg_upc_noiseSup_L1TAU1_TE5_VTE200',        'L1_TAU1_TE5_VTE200',   [], [UPCStream, 'express'],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc_noiseSup"]]],
 	    ['hi_upc_FgapAC3_hi_gg_upc_L12TAU2_VTE200',        'L1_2TAU2_VTE200',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc"]]],
 	    ['hi_upc_FgapAC3_hi_gg_upc_L12TAU3_VTE200',        'L1_2TAU3_VTE200',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc"]]],
 	    ['hi_upc_FgapAC3_hi_gg_upc_L12TAU2_VTE100',        'L1_2TAU2_VTE100',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc"]]],
 	    ['hi_upc_FgapAC3_hi_gg_upc_L12TAU3_VTE100',        'L1_2TAU3_VTE100',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc"]]],
 	    ['hi_upc_FgapAC3_hi_gg_upc_L12TAU2_VTE50',        'L1_2TAU2_VTE50',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc"]]],
 	    ['hi_upc_FgapAC3_hi_gg_upc_L12TAU3_VTE50',        'L1_2TAU3_VTE50',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc"]]],
 	    ['hi_upc_FgapAC3_hi_gg_upc_L12TAU1_VTE50',        'L1_2TAU1_VTE50',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc"]]],
 	    
 	    
 	    ['hi_upc_FgapAC3_hi_gg_upc_L1TAU1_TE4_VTE200',        'L1_TAU1_TE4_VTE200',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc"]]],
 	    ['hi_upc_FgapAC3_hi_gg_upc_L1TAU1_TE5_VTE200',        'L1_TAU1_TE5_VTE200',   [], [UPCStream],["BW:UPC", "RATE:UPC"], -1, ['serial',-1,["hi_upc_FgapAC3","hi_gg_upc"]]],
 	    
 	    ['hi_gg_upc_L1TE5_VTE200',        'L1_TE5_VTE200',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
 	    ['hi_gg_upc_L1TE4_VTE200',        'L1_TE4_VTE200',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
 	    ['hi_gg_upc_L1TE5_VTE100',        'L1_TE5_VTE100',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
 	    ['hi_gg_upc_L1TE4_VTE100',        'L1_TE4_VTE100',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
 	    ['hi_gg_upc_L1TE5_VTE50',        'L1_TE5_VTE50',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
 	    ['hi_gg_upc_L1TE4_VTE50',        'L1_TE4_VTE50',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
 	    
 	    #PCpeb
 	    ['hi_hipeb_L1TE50_VTE600',      'L1_TE50_VTE600.0ETA49_PEB', [], [PCpebStream], ["BW:PCpeb", "RATE:PCpeb"], 1],
		#CCpeb
		['hi_hipeb_L1TE600',      'L1_TE600.0ETA49_PEB', [], [CCpebStream], ["BW:CCpeb", "RATE:CCpeb"], 1],
 	    	    
	    #['hi_gg_upc_L1All',               '',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	# validation trigger
	   #['hi_loose_upc_L1All',     '',   [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
        ]
    
    TriggerFlags.MinBiasSlice.signatures = [
        ['mb_mbts_L1MBTS_2',    'L1_MBTS_2',   [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_1_1',  'L1_MBTS_1_1', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1], 
        ['mb_mbts_L1MBTS_2_2',  'L1_MBTS_2_2', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_3_3',  'L1_MBTS_3_3', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_4_4',  'L1_MBTS_4_4', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],

	#sptrk
        ['mb_sp_vetospmbts2in_L1TE5_VTE200',  'L1_TE5_VTE200', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp',  'L1_RD0_FILLED', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk',  'L1_ZDC_A_C_VTE50', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_L1MBTS_1_1_VTE50', 'L1_MBTS_1_1_VTE50', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        #['mb_idperf_ion_L1MBTS_1', 'L1_MBTS_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        #['mb_idperf_ion_L1MBTS_1_1', 'L1_MBTS_1_1', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        
        ['mb_perf_L1RD1_FILLED',    'L1_RD1_FILLED',   [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_perf_L1RD1_EMPTY',    'L1_RD1_EMPTY',   [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],

	#PCpeb
	['mb_sptrk_hipeb_L1ZDC_A_C_VTE50',  'L1_ZDC_A_C_VTE50_PEB', [], [PCpebStream],["BW:PCpeb", "RATE:PCpeb"], 1],
	#PC
	['mb_sptrk_L1ZDC_A_C_VTE50',  'L1_ZDC_A_C_VTE50', [], [PCStream, 'express'],["BW:PC", "RATE:PC"], 1],
	
	
	#MB UPC:
	['mb_sptrk_L1ZDC_XOR_VTE200',  'L1_ZDC_XOR_VTE200', [], [UPCStream, 'express'],["BW:UPC", "RATE:UPC"], 1],
	['mb_sptrk_L1ZDC_XOR_TE4_VTE200',  'L1_ZDC_XOR_TE4_VTE200', [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	['mb_sptrk_L1ZDC_XOR_TE3.0ETA49_VTE200',  'L1_ZDC_XOR_TE3.0ETA49_VTE200', [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	['mb_sptrk_L1ZDC_XOR_VTE200_MBTS_1',  'L1_ZDC_XOR_VTE200_MBTS_1', [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	
	['mb_sptrk_exclusiveloose2_L12TAU1_VTE50',  'L1_2TAU1_VTE50', [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	['mb_sptrk_exclusiveloose2_L12TAU2_VTE50',  'L1_2TAU2_VTE50', [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	['mb_sptrk_exclusiveloose2_L12TAU2_VTE100',  'L1_2TAU2_VTE100', [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	['mb_sptrk_exclusiveloose2_L12TAU2_VTE200',  'L1_2TAU2_VTE200', [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	['mb_sptrk_vetombts2in_exclusiveloose2_L12TAU1_VTE50',  'L1_2TAU1_VTE50', [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	['mb_sptrk_vetombts2in_exclusiveloose2_L12TAU2_VTE50',  'L1_2TAU2_VTE50', [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	['mb_sptrk_vetombts2in_exclusiveloose2_L12TAU2_VTE100',  'L1_2TAU2_VTE100', [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	['mb_sptrk_vetombts2in_exclusiveloose2_L12TAU2_VTE200',  'L1_2TAU2_VTE200', [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	#['mb_sptrk_ion_trk15_L1ZDC_XOR_VTE200_MBTS_1',  'L1_ZDC_XOR_VTE200_MBTS_1', [], [UPCStream],["BW:UPC", "RATE:UPC"], 1],
	
	#UPC: Other primary 
	['mb_sptrk_exclusiveloose_vetosp1500_L1VTE20',        'L1_VTE20',   [], [UPCStream], ["BW:UPC", "RATE:UPC"], -1],
	['mb_sptrk_exclusivetight_vetosp1500_L1VTE20',        'L1_VTE20',   [], [UPCStream, 'express'], ["BW:UPC", "RATE:UPC"], -1],
	['mb_sptrk_exclusiveloose_vetombts8_vetosp1500_L1VTE20',        'L1_VTE20',   [], [UPCStream], ["BW:UPC", "RATE:UPC"], -1],
	['mb_sptrk_exclusivetight_vetombts8_vetosp1500_L1VTE20',        'L1_VTE20',   [], [UPCStream, 'express'], ["BW:UPC", "RATE:UPC"], -1],
		
	#['mb_sptrk_exclusiveloose_L1VTE20',        'L1_VTE20',   [], [UPCStream], ["BW:UPC", "RATE:UPC"], -1],
	#['mb_sptrk_exclusivetight_L1VTE20',        'L1_VTE20',   [], [UPCStream], ["BW:UPC", "RATE:UPC"], -1],
	
	#['mb_sptrk_ptX_L1TE5_VZDC_AORC_VTE200',        'L1_TE5_VZDC_AORC_VTE200',   [], [UPCStream], ["BW:UPC", "RATE:UPC"], -1],
       
	#UPC:Supporting
	['mb_sp_L1VTE50',  'L1_VTE50', [], [UPCStream], ["BW:UPC", "RATE:UPC"], -1],
	['mb_sptrk_L1ZDC_XOR_TE5_VTE200',        'L1_ZDC_XOR_TE5_VTE200',   [], [UPCStream], ["BW:UPC", "RATE:UPC"], -1],
	
	#overlay
	['mb_sptrk_L1ZDC_A_C_VTE50_OVERLAY',  'L1_ZDC_A_C_VTE50_OVERLAY', [], ['MinBiasOverlay'],["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],

        ]

    TriggerFlags.CalibSlice.signatures   = [

	['idcalib_trk9_central_L1J12_VTE100',    'L1_J12_VTE100', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk9_fwd_L1J12_VTE100',        'L1_J12_VTE100', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
	['idcalib_trk9_central_L1J12_VTE200',    'L1_J12_VTE200', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk9_fwd_L1J12_VTE200',        'L1_J12_VTE200', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],

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

        ['lumipeb_vdm_L1LUCID_BGRP7',          'L1_LUCID_BGRP7',          [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_1_1_BGRP9_LUCID_BGRP7',          'L1_MBTS_1_1_BGRP9,L1_LUCID_BGRP7',          [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2_BGRP9_LUCID_BGRP7',          'L1_MBTS_2_BGRP9,L1_LUCID_BGRP7',          [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],

        # Monitoritems for saturated towers
        ['satu20em_l1satmon_L1J100_FIRSTEMPTY',  'L1_J100_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        #['satu20em_l1satmon_L1J100.31ETA49_FIRSTEMPTY',  'L1_J100.31ETA49_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['satu20em_l1satmon_L1EM20VH_FIRSTEMPTY',  'L1_EM20VH_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        #['satu20em_l1satmon_L1EM20VHI_FIRSTEMPTY',  'L1_EM20VHI_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1], 
        # the one above was replaced with a em22vhi_firstempty seeded one:
        #['satu20em_l1satmon_L1EM22VHI_FIRSTEMPTY',  'L1_EM22VHI_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1], # for HI, L1EM20VH one is OK?
        ['satu20em_l1satmon_L1J100',  'L1_J100',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        #['satu20em_l1satmon_L1J100.31ETA49',  'L1_J100.31ETA49',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],                 
        # larnoiseburst
        #['larnoiseburst_L1XE50',            'L1_XE50',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1XE60', 'L1_XE60', [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1XE70', 'L1_XE70', [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1XE80', 'L1_XE80', [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J75',             'L1_J75',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J100', 'L1_J100', [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J40_XE60',            'L1_J40_XE60',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1], 
        
        # larpeb
        ['g8_loose_larpeb',                      'L1_EM7',     [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g12_loose_larpeb',                     'L1_EM10',     [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_loose_larpeb_L1EM15',              'L1_EM15', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g40_loose_larpeb',                     'L1_EM20', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g60_loose_larpeb',                     'L1_EM20', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g80_loose_larpeb',                     'L1_EM20', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['conej40_larpebj',  'L1_J20',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['conej165_larpebj', 'L1_J100',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
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
	#['noalg_mb_L1TE14000',      'L1_TE14000', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    
	#TE restricted calo
    ['noalg_mb_L1TE3.0ETA49',      'L1_TE3.0ETA49', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE7.0ETA49',      'L1_TE7.0ETA49', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE600.0ETA49',      'L1_TE600.0ETA49', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE1500.0ETA49',      'L1_TE1500.0ETA49', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE3000.0ETA49',      'L1_TE3000.0ETA49', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE3500.0ETA49',      'L1_TE3500.0ETA49', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	#['noalg_mb_L1TE5000.0ETA49',      'L1_TE5000.0ETA49', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE6500.0ETA49',      'L1_TE6500.0ETA49', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE8000.0ETA49',      'L1_TE8000.0ETA49', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	#['noalg_mb_L1TE9000.0ETA49',      'L1_TE9000.0ETA49', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    
    #PC
 	['noalg_pc_L1TE50_VTE600.0ETA49',      'L1_TE50_VTE600.0ETA49', [], [PCStream, 'express'], ["BW:PC", "RATE:PC"], 1],
	#CC
	['noalg_cc_L1TE600.0ETA49',      'L1_TE600.0ETA49', [], [CCStream, 'express'], ["BW:CC", "RATE:CC"], 1],
    
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

	#UPC streamers
	['noalg_L1ZDC_A_C_VTE50',   'L1_ZDC_A_C_VTE50', [], [UPCStream], ["BW:UPC", "RATE:UPC"], 1],
	['noalg_L1ZDC_XOR',         'L1_ZDC_XOR', [], [UPCStream], ["BW:UPC", "RATE:UPC"], 1],
	['noalg_L1TE5_VTE200',      'L1_TE5_VTE200', [], [UPCStream], ["BW:UPC", "RATE:UPC"], 1],
	['noalg_L1ZDC_A_C_TE5_VTE200',   'L1_ZDC_A_C_TE5_VTE200', [], [UPCStream], ["BW:UPC", "RATE:UPC"], 1],
	['noalg_L1ZDC_XOR_TE5_VTE200',   'L1_ZDC_XOR_TE5_VTE200', [], [UPCStream], ["BW:UPC", "RATE:UPC"], 1],

	#beam spot
	['noalg_L1J12_VTE100',   'L1_J12_VTE100', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	['noalg_L1J12_VTE200',   'L1_J12_VTE200', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	#['noalg_L1MU0_VTE50',   'L1_MU0_VTE50', [], [UPCStream], ["BW:UPC", "RATE:UPC"], 1],

	#muon
	['noalg_L1MU4',    'L1_MU4', [],   [HardProbesStream, 'express'], ["BW:SingleMuon", "RATE:Muon"], 1],
	['noalg_L1MU6',    'L1_MU6', [],  [HardProbesStream, 'express'], ["BW:SingleMuon", "RATE:Muon"], 1],	
	['noalg_L1MU11',   'L1_MU11', [],  [HardProbesStream, 'express'], ["BW:SingleMuon", "RATE:Muon"], 1],
	#['noalg_L1MU15',   'L1_MU15', [], [HardProbesStream], ["BW:SingleMuon", "RATE:Muon"], 1],
	['noalg_L1MU20',   'L1_MU20', [], [HardProbesStream], ["BW:SingleMuon", "RATE:Muon"], 1],


	#jets
	['noalg_L1J12',   'L1_J12', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	['noalg_L1J15',   'L1_J15', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
    ['noalg_L1J20',   'L1_J20', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
    ['noalg_L1J30',   'L1_J30', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	#['noalg_L1J35',   'L1_J35', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	['noalg_L1J50',   'L1_J50', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	['noalg_L1J75',   'L1_J75', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	#['noalg_L1J175',   'L1_J175', [], [HardProbesStream], ["BW:Jet", "RATE:Jets"], 1],
	['noalg_L1J400',   'L1_J400', [], [HardProbesStream, 'express'], ["BW:Jet", "RATE:Jets"], 1],

	#egamma
	#['noalg_L1EM7',    'L1_EM7',  [], [HardProbesStream], ["BW:SingleElectron", "RATE:Egamma"], 1],
    #['noalg_L1EM8',    'L1_EM8',  [], [HardProbesStream], ["BW:SingleElectron", "RATE:Egamma"], 1],
	['noalg_L1EM10',   'L1_EM10', [], [HardProbesStream, 'express'], ["BW:SingleElectron", "RATE:Egamma"], 1],
	['noalg_L1EM12',   'L1_EM12', [], [HardProbesStream, 'express'], ["BW:SingleElectron", "RATE:Egamma"], 1],
    ['noalg_L1EM14',   'L1_EM14', [], [HardProbesStream, 'express'], ["BW:SingleElectron", "RATE:Egamma"], 1],
	['noalg_L1EM16',   'L1_EM16', [], [HardProbesStream], ["BW:SingleElectron", "RATE:Egamma"], 1],
	['noalg_L1EM18',   'L1_EM18', [], [HardProbesStream], ["BW:SingleElectron", "RATE:Egamma"], 1],
	['noalg_L1EM20',   'L1_EM20', [], [HardProbesStream], ["BW:SingleElectron", "RATE:Egamma"], 1],
	['noalg_L1EM22',   'L1_EM22', [], [HardProbesStream, 'express'], ["BW:SingleElectron", "RATE:Egamma"], 1],

	#data min-bias overlay
	['noalg_L1ZDC_A_C_OVERLAY',   'L1_ZDC_A_C_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
	['noalg_L1TE50_OVERLAY',   'L1_TE50_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
	
	#TE restricted calo in |eta|<4.9   
        ['noalg_mb_L1TE600.0ETA49_OVERLAY',       'L1_TE600.0ETA49_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
        ['noalg_mb_L1TE1500.0ETA49_OVERLAY',      'L1_TE1500.0ETA49_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
        ['noalg_mb_L1TE3000.0ETA49_OVERLAY',      'L1_TE3000.0ETA49_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
        ['noalg_mb_L1TE3500.0ETA49_OVERLAY',      'L1_TE3500.0ETA49_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
        #['noalg_mb_L1TE5000.0ETA49_OVERLAY',      'L1_TE5000.0ETA49_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
        ['noalg_mb_L1TE6500.0ETA49_OVERLAY',      'L1_TE6500.0ETA49_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
        ['noalg_mb_L1TE8000.0ETA49_OVERLAY',      'L1_TE8000.0ETA49_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
        #['noalg_mb_L1TE9000.0ETA49_OVERLAY',      'L1_TE9000.0ETA49_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],

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
        ['noalg_bkg_L1MU4_UNPAIRED_ISO',  'L1_MU4_UNPAIRED_ISO',  [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1],  

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
        #['noalg_eb_L1MU0_VTE50', 'L1_MU0_VTE50', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1EM12', 'L1_EM12', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1EM14', 'L1_EM14', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1EM16', 'L1_EM16', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1EM22', 'L1_EM22', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L12EM22', 'L1_2EM22', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1MU4', 'L1_MU4', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1TE50', 'L1_TE50', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1TE12000', 'L1_TE12000', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1TE4_VTE200', 'L1_TE4_VTE200', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1TE50_VTE200', 'L1_TE50_VTE200', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1TAU1_TE4_VTE200', 'L1_TAU1_TE4_VTE200', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1TAU1_TE5_VTE200', 'L1_TAU1_TE5_VTE200', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L12TAU1_VTE50', 'L1_2TAU1_VTE50', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L12TAU2_VTE200', 'L1_2TAU2_VTE200', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1ZDC_XOR_VTE50', 'L1_ZDC_XOR_VTE50', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1ZDC_XOR_VTE200', 'L1_ZDC_XOR_VTE200', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1VZDC_A_C_VTE50', 'L1_VZDC_A_C_VTE50', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1VZDC_A_C_TE5_VTE200', 'L1_VZDC_A_C_VTE50', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1MU4_VTE50', 'L1_MU4_VTE50', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1RD1_FILLED', 'L1_RD1_FILLED', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        
        #LAr demonostartors
        ['noalg_larcells_L1LAR-EM',                'L1_LAR-EM',           [], ['LArCells'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_larcells_L1LAR-J',                 'L1_LAR-J',            [], ['LArCells'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_larcells_L1LAR-ZEE_LAR-EM',        'L1_LAR-ZEE_LAR-EM',   [], ['LArCells'],     ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_larcells_L1LAR-ZEE',               'L1_LAR-ZEE',          [], ['LArCells'],     ["RATE:SeededStreamers", "BW:Other"], -1],

        
        ["noalg_l1topo_L1LAR-EM",          			"L1_LAR-EM",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
	    ["noalg_l1topo_L1LAR-J",          			"L1_LAR-J",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],

        #ATR-18734 b-jet+HT
        ["noalg_l1topo_L1HT190-J15s5.ETA21",                  "L1_HT190-J15s5.ETA21",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #ATR-18734 B->K*ee
        ["noalg_l1topo_L1BPH-0M9-EM7-EM5_2MU4",          "L1_BPH-0M9-EM7-EM5_2MU4",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1BPH-0M9-EM7-EM5_MU6",          "L1_BPH-0M9-EM7-EM5_MU6",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1BPH-0DR3-EM7J15_2MU4",          "L1_BPH-0DR3-EM7J15_2MU4",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1BPH-0DR3-EM7J15_MU6",          "L1_BPH-0DR3-EM7J15_MU6",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #ATR-18734 jpsi->ee T&P
        ["noalg_l1topo_L1JPSI-1M5-EM7",                 "L1_JPSI-1M5-EM7",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1JPSI-1M5-EM12",                 "L1_JPSI-1M5-EM12",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #ATR-18734 VBF inclusive
        ["noalg_l1topo_L1MJJ-500-NFF",                  "L1_MJJ-500-NFF",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1      ],
        ["noalg_l1topo_L1MJJ-700",                  "L1_MJJ-700",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1      ],
        #ATR-18734 VBF H->bb + photon
        #['noalg_l1topo_L1EM18VHI_MJJ-300',   'L1_EM18VHI_MJJ-300',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L1EM18VH_MJJ-300',   'L1_EM18VH_MJJ-300',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        #ATR-18734
        #["noalg_l1topo_L1HT150-J20s5.ETA31",                  "L1_HT150-J20s5.ETA31",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1HT150-J20s5.ETA31_MJJ-400-CF",                    "L1_HT150-J20s5.ETA31_MJJ-400-CF",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],

        #ATR-18734 g + tau for excl Higgs decays
        ["noalg_l1topo_L130M-EM20ITAU12",                     "L1_30M-EM20ITAU12",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #ATR-18734 Exotics LLP cal-ratio
        ["noalg_l1topo_L1LLP-RO",                 "L1_LLP-RO",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1LLP-NOMATCH",                  "L1_LLP-NOMATCH",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #ATR-18734 SUSY Higgsino
        ['noalg_l1topo_L12MU4_J20_XE30_DPHI-J20s2XE30',   'L1_2MU4_J20_XE30_DPHI-J20s2XE30',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L1MU4_J30_XE40_DPHI-J20s2XE30',   'L1_MU4_J30_XE40_DPHI-J20s2XE30',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_l1topo_L1MU4_J50_XE50_DPHI-J20s2XE30',   'L1_MU4_J50_XE50_DPHI-J20s2XE30',    [], ['L1Topo'],    ["RATE:SeededStreamers", "BW:Other"], -1],
        ["noalg_l1topo_L1J40_XE50_DPHI-J20s2XE50",                 "L1_J40_XE50_DPHI-J20s2XE50",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #ATR-18734 Axion search
        ["noalg_l1topo_L1DPHI-2EM3",                    "L1_DPHI-2EM3",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #ATR-18734 LAr
        #["noalg_l1topo_L1LAR-J",                        "L1_LAR-J",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #["noalg_l1topo_L1LAR-EM",                       "L1_LAR-EM",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_l1topo_L1LAR-ZEE',                              'L1_LAR-ZEE',          [], ['L1Topo'],     ["RATE:L1TopoStreamers", "BW:Other"], -1],     
        ['noalg_l1topo_L1LAR-ZEE_LAR-EM',                       'L1_LAR-ZEE_LAR-EM',   [], ['L1Topo'],     ["RATE:L1TopoStreamers", "BW:Other"], -1],

        #ATR-18734 Tau
        ["noalg_l1topo_L1DR-EM15TAU12I-J25",                  "L1_DR-EM15TAU12I-J25",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1DR-TAU20ITAU12I-J25",                "L1_DR-TAU20ITAU12I-J25",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ["noalg_l1topo_L1TAU60_DR-TAU20ITAU12I",                    "L1_TAU60_DR-TAU20ITAU12I",          [], ["L1Topo"], ["RATE:L1TopoStream     ers", "BW:Other"], -1 ],
        #ATR-18734 Jets
        ["noalg_l1topo_L1SC111-CJ15",                   "L1_SC111-CJ15",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        #ATR-18734 TLA
        ["noalg_l1topo_L1J50_DETA20-J50J",                    "L1_J50_DETA20-J50J",          [], ["L1Topo"], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],

        ]

    if TriggerFlags.doFTK():
        TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + [
            ['noalg_ftk_L1TE4_VTE200',   'L1_TE4_VTE200', [], ['FTK'], ["RATE:SeededStreamers", "BW:Detector"], -1 ],
            ['noalg_ftk_L1TE5_VTE200',   'L1_TE5_VTE200', [], ['FTK'], ["RATE:SeededStreamers", "BW:Detector"], -1 ],
            ['noalg_ftk_L1TE50_VTE200',   'L1_TE50_VTE200', [], ['FTK'], ["RATE:SeededStreamers", "BW:Detector"], -1 ],
            ['noalg_ftk_L1TE50_VTE600.0ETA49_PEBX',   'L1_TE50_VTE600.0ETA49_PEB', [], ['FTK'], ["RATE:SeededStreamers", "BW:Detector"], -1 ],

            ]


    TriggerFlags.BeamspotSlice.signatures = [
        ['beamspot_activeTE_trkfast_peb',           'L1_J12_VTE100', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_pebTRT',           'L1_J12_VTE100', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_trkFS_trkfast',                  'L1_J12_VTE100', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

	['beamspot_activeTE_trkfast_peb_L1J12_VTE200',           'L1_J12_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_pebTRT_L1J12_VTE200',           'L1_J12_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_trkFS_trkfast_L1J12_VTE200',                  'L1_J12_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

	['beamspot_activeTE_trkfast_peb_L1TE5_VTE200',           'L1_TE5_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_pebTRT_L1TE5_VTE200',           'L1_TE5_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_trkFS_trkfast_L1TE5_VTE200',                  'L1_TE5_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

	]

    TriggerFlags.MonitorSlice.signatures = [
        ['mistimemonl1bccorr', '', [], ['Mistimed'], ["RATE:Monitoring","BW:Other"], -1],
        ['mistimemonl1bccorrnomu', '', [], ['Mistimed'], ["RATE:Monitoring","BW:Other"], -1],
        ['mistimemoncaltimenomu', '', [], ['Mistimed'], ["RATE:Monitoring","BW:Other"], -1],
        ['mistimemoncaltime', '', [], ['Mistimed'], ["RATE:Monitoring","BW:Other"], -1],
        ['mistimemonj400', '', [], ['Mistimed'], ["RATE:Monitoring","BW:Other"], -1],
        
        ['l1calooverflow','', [], ['L1Calo'], ['RATE:Monitoring','BW:Other'], -1],
        ['l1topodebug', '', [], ['L1TopoMismatches'], ['RATE:Monitoring','BW:Other'], -1],        
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

######################################################
# TIGHT physics prescales
######################################################
from copy import deepcopy
# enable if desired: # setting all L1 prescales to 1
#Prescales.L1Prescales = dict([(ctpid,1) for ctpid in Prescales.L1Prescales]) 
 
Prescales.L1Prescales_tight_physics_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_tight_physics_prescale = deepcopy(Prescales.HLTPrescales)

chain_list=[
    'timeburner','mb_sp',
    ]
Prescales.HLTPrescales_tight_physics_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0,-1] ])))
######################################################

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


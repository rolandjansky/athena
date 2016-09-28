# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------------------------------------------------------------------#
def setupMenu():
    
    from TriggerMenu.menu.TriggerPythonConfig import TriggerPythonConfig
    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenu.menu.Physics_HI_v4.py' )

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
    
    TriggerFlags.MuonSlice.signatures = [
	#ID tracking performance
    ['mu4_idperf',          'L1_MU4', [], [HardProbesStream, 'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu6_idperf',          'L1_MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu8_idperf',          'L1_MU6', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1], #no configuration
	['mu10_idperf',         'L1_MU10', [], [HardProbesStream, 'express'], ['RATE:SingleMuon','BW:Muon'], -1],

	#RoI muons
	['mu4',                 'L1_MU4', [], [HardProbesStream,  'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu4_L1MU4_VTE10',     'L1_MU4_VTE10', [], [HardProbesStream,  'express'], ['RATE:UPCMuon','BW:Muon'], -1],
	['mu4_iloose',                 'L1_MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu6',                 'L1_MU6', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu6_iloose',                 'L1_MU6', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],	
	['mu8',                 'L1_MU6', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu10',                'L1_MU10', [], [HardProbesStream,  'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu10_iloose',                'L1_MU10', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu11',                'L1_MU10', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu14',                'L1_MU10', [], [HardProbesStream,  'express'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu14_iloose',                'L1_MU10', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu15',                'L1_MU15', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu15_L1MU10',                'L1_MU10', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu18',                'L1_MU15', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu20',                'L1_MU20', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu20_iloose_L1MU15',                'L1_MU15', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu20_L1MU15',                'L1_MU15', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],

	#msonly
	['mu4_msonly',            'L1_MU4',           [], [HardProbesStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
	['mu6_msonly',            'L1_MU6',           [], [HardProbesStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
	['mu10_msonly',            'L1_MU10',           [], [HardProbesStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
	['mu14_msonly',            'L1_MU10',           [], [HardProbesStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
	['mu15_msonly',            'L1_MU15',           [], [HardProbesStream,  'express'], ['RATE:SingleMuon', 'BW:Muon'], -1],
	['mu20_msonly',            'L1_MU20',           [], [HardProbesStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
	

	#di-muons RoI based
	['2mu4',                 'L1_2MU4', [], [HardProbesStream], ['RATE:MultiMuon','BW:Muon'], -1],
	['2mu6',                 'L1_2MU6', [], [HardProbesStream], ['RATE:MultiMuon','BW:Muon'], -1],
	['2mu4_msonly',          'L1_2MU4', [], [HardProbesStream], ['RATE:MultiMuon','BW:Muon'], -1],
    ['2mu6_msonly',          'L1_2MU4', [], [HardProbesStream], ['RATE:MultiMuon','BW:Muon'], -1],

	
	
    # muon calibration
    ['mu0_muoncalib',             'L1_MU4',      [],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
    ['mu0_muoncalib_L1MU15',      'L1_MU15',     [],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
    ['mu0_muoncalib_L1MU4_EMPTY', 'L1_MU4_EMPTY',[],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
    
        
        ]

    TriggerFlags.JetSlice.signatures = [
	#performance
	['j0_perf_L1ZDC_A_C',     'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j0_perf_L1RD0_FILLED',     'L1_RD0_FILLED',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],


	['j10_L1RD0_FILLED',     'L1_RD0_FILLED',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_L1MBTS_1',         'L1_MBTS_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_L1TE5',            'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_L1J5',             'L1_J5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_L1J10',            'L1_J10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_L1ZDC_A',          'L1_ZDC_A',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_L1ZDC_C',          'L1_ZDC_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j10_L1ZDC',            'L1_ZDC',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],

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
	['j30_ion_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_ion_0eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],   
	['j40_ion_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_ion_0eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_ion_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j50_ion_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j50_ion_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j60_ion_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j60_ion_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j75_ion_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j85_ion_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j100_ion_L1J20',         'L1_J20',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j110_ion_L1J3',         'L1_J30',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j150_ion_L1J30',         'L1_J30',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j175_ion_L1J30',         'L1_J50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	#R=0.4 jets pp jets
	#['j20',         'L1_RD0_FILLED',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j20_0eta490',         'L1_RD0_FILLED',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_0eta490_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j20_ion_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	#['j20_0eta490_ion_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j20_0eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j30_0eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],   
	['j40_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_0eta490_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j40_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j50_L1J10',         'L1_J10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j50_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j60_L1J15',         'L1_J15',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j60',         'L1_J20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j75_L1J20',         'L1_J20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j85',         'L1_J20',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j100_L1J20',         'L1_J20',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j110',         'L1_J30',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j150_L1J30',         'L1_J30',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j175',         'L1_J50',[], [HardProbesStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
	
	
	#forward jets ATR-12623
	['j10_320eta490_ion_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_320eta490_ion_L1MBTS_1_1',         'L1_MBTS_1_1',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j15_320eta490_ion_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_320eta490_ion_L1TE5',         'L1_TE5',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_320eta490_ion_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_320eta490_ion_L1TE10',         'L1_TE10',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j45_320eta490_ion',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j55_320eta490_ion',         'L1_J15.31ETA49',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ]

    TriggerFlags.BjetSlice.signatures = [
        ]

    TriggerFlags.METSlice.signatures = [
        ]

    TriggerFlags.TauSlice.signatures = [
        ]

    TriggerFlags.EgammaSlice.signatures = [

	['e5_etcut',                  'L1_EM3',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e5_loose',                  'L1_EM3',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e5_lhloose',                  'L1_EM3',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],


	['2e5_etcut',                  'L1_2EM3',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
	['2e5_loose',                  'L1_2EM3',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
	['2e5_lhloose',                  'L1_2EM3',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],

	['e7_etcut',                  'L1_EM5',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e7_loose',                  'L1_EM5',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e7_lhloose',                'L1_EM5',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['2e7_etcut',                  'L1_2EM5',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
	['2e7_loose',                  'L1_2EM5',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
	['2e7_lhloose',                'L1_2EM5',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],


	['e10_etcut',                  'L1_EM7',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e10_loose',                  'L1_EM7',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e10_lhloose',                'L1_EM7',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['2e10_etcut',                  'L1_2EM7',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
	['2e10_loose',                  'L1_2EM7',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],
	['2e10_lhloose',                'L1_2EM7',  [], [HardProbesStream], ['RATE:DiElectron', 'BW:Egamma'], -1],

	['e12_etcut',                  'L1_EM8',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e12_loose',                  'L1_EM8',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e12_lhloose',                'L1_EM8',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],


	['e15_etcut',                  'L1_EM10',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_loose',                  'L1_EM10',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_lhloose',                'L1_EM10',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

        ['e15_etcut_L1EM12',           'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e17_etcut',                  'L1_EM12',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e17_loose',                  'L1_EM12',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e17_lhloose',                'L1_EM12',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],


	['e18_etcut',                  'L1_EM14',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e18_loose',                  'L1_EM14',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e18_lhloose',                'L1_EM14',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['e20_etcut',                  'L1_EM14',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e20_loose',                  'L1_EM14',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e20_lhloose',                'L1_EM14',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['e20_etcut_L1EM16',                  'L1_EM16',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e20_loose_L1EM16',                  'L1_EM16',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e20_lhloose_L1EM16',                'L1_EM16',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['e22_etcut',                  'L1_EM16',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e22_loose',                  'L1_EM16',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e22_lhloose',                'L1_EM16',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['e24_etcut',                  'L1_EM20',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e24_loose',                  'L1_EM20',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e24_lhloose',                'L1_EM20',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],


        

	##['e12_etcut',                  'L1_EM8',  [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],


	['2g10_loose',             'L1_2EM3', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['2g10_loose_L1EM5',       'L1_2EM5', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

	['g15_loose',              'L1_EM7', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g13_etcut',              'L1_EM7', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

	['g20_loose',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g18_etcut',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

	['g25_loose',              'L1_EM20', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g28_etcut',              'L1_EM20', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

	['g35_loose',              'L1_EM20', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
	['g33_etcut',              'L1_EM20', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

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
	['e10_etcut_ion',              'L1_EM7', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	##['e12_etcut_ion',              'L1_EM8', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e12_etcut_ion_L1EM10',       'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_etcut_ion',              'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_etcut_ion_L1EM12',       'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
#	['g12_etcut_ion',              'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
#        ['g15_etcut_ion',              'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
#	['g18_etcut_ion',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
#	['g20_etcut_ion',              'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

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
	['e20_etcut_L1EM12',                  'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e20_loose_L1EM12',                  'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e20_lhloose_L1EM12',                'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	#idperf
	['e15_loose_ion_idperf',       'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
	['e15_medium_ion_idperf',       'L1_EM10', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
	['e15_loose_ion_idperf_L1EM12', 'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
	['e15_medium_ion_idperf_L1EM12', 'L1_EM12', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],

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
	##['2g15_loose',                  'L1_2EM10', [], [HardProbesStream], ['RATE:MultiPhoton', 'BW:Egamma'], -1],
	##['2g15_loose_ion',              'L1_2EM10', [], [HardProbesStream], ['RATE:MultiPhoton', 'BW:Egamma'], -1],

	# Monopole triggers ATR-12467
#        ['g0_hiptrt_L1EM14',                    'L1_EM14', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
#	['g0_hiptrt_L1EM16',                    'L1_EM16', [], [HardProbesStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

        ]

    TriggerFlags.BphysicsSlice.signatures = [
	['2mu4_bDimu',                    'L1_2MU4', [], [HardProbesStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],
	['2mu4_bJpsimumu',                    'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
	['2mu4_bUpsimumu',                    'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
	['2mu4_bBmumu',                    'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1], 
	['2mu4_bBmumuxv2',                    'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
	['2mu4_bBmumux_BcmumuDsloose',                    'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1], 
	['2mu4_bBmumux_BpmumuKp',                    'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
	['2mu6_bDimu',                    'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1], 
	['2mu6_bJpsimumu',                'L1_2MU6', [], [HardProbesStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],
	['2mu6_bUpsimumu',                    'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
	['2mu6_bBmumu',                    'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1], 
	['2mu6_bBmumuxv2',                    'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
	['2mu6_bBmumux_BcmumuDsloose',                    'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1], 
	['2mu6_bBmumux_BpmumuKp',                    'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
	    
    ['mu4_mu4_idperf_bJpsimumu_noid',  'L1_2MU4', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring','BW:Bphys', 'BW:ID'], -1],
    ['mu13_mu13_idperf_Zmumu',     'L1_2MU6', [], [HardProbesStream, 'express'], ['RATE:IDMonitoring','BW:Bphys', 'BW:ID'], -1],
        
    ['2mu4_bDimu_novtx_noos',         'L1_2MU4', [], [HardProbesStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],
    ['2mu4_bDimu_noinvm_novtx_ss',         'L1_2MU4', [], [HardProbesStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],
    ['2mu6_bDimu_noinvm_novtx_ss',         'L1_2MU6', [], [HardProbesStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],
    ['2mu6_bDimu_novtx_noos',         'L1_2MU6', [], [HardProbesStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1],
    
    ['2mu4_nomucomb',                    'L1_2MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['2mu6_nomucomb',                    'L1_2MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['mu4_nomucomb',                    'L1_MU4', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['mu6_nomucomb',                    'L1_MU6', [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    
    ['mu4_bJpsi_Trkloose',            'L1_MU4',  [], [HardProbesStream], ['RATE:Bphysics','BW:Bphys'], -1],
    ['mu6_bJpsi_Trkloose',            'L1_MU6',  [], [HardProbesStream,'express'], ['RATE:Bphysics','BW:Bphys'], -1],
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
        ['hi_upc_FgapA_mb_sptrk_L1RD0_FILLED',    'L1_RD0_FILLED',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapA', 'mb_sptrk'],False]],
        ['hi_upc_FgapC_mb_sptrk_L1RD0_FILLED',    'L1_RD0_FILLED',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapC', 'mb_sptrk'],False]],
        ['hi_upc_FgapAC_mb_sptrk_L1RD0_FILLED',    'L1_RD0_FILLED',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mb_sptrk'],False]],

        ['hi_upc_FgapA_mb_sptrk_L1MBTS_1',    'L1_MBTS_1',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapA', 'mb_sptrk'],False]],
        ['hi_upc_FgapC_mb_sptrk_L1MBTS_1',    'L1_MBTS_1',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapC', 'mb_sptrk'],False]],
        ['hi_upc_FgapAC_mb_sptrk_L1MBTS_1',    'L1_MBTS_1',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mb_sptrk'],False]],


        ['hi_upc_FgapA_mb_sptrk_L1ZDC',    'L1_ZDC',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapA', 'mb_sptrk_L1ZDC'],False]],
        ['hi_upc_FgapC_mb_sptrk_L1ZDC',    'L1_ZDC',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapC', 'mb_sptrk_L1ZDC'],False]],
        ['hi_upc_FgapAC_mb_sptrk_L1ZDC',    'L1_ZDC',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mb_sptrk_L1ZDC'],False]],

        # UPC jets
        ['hi_upc_FgapA_j10_L1RD0_FILLED',    'L1_RD0_FILLED',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j10'],False]],
        ['hi_upc_FgapA_j10_L1MBTS_1',        'L1_MBTS_1',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j10'],False]],


        ['hi_upc_FgapA_j10_L1TE5',           'L1_TE5',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j10_L1TE5'],False]],
        ['hi_upc_FgapA_j10_L1J5',            'L1_J5',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j10_L1J5'],False]],
        ['hi_upc_FgapA_j10_L1J10',           'L1_J10',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j10_L1J10'],False]],
        ['hi_upc_FgapA_j10_L1ZDC_A',         'L1_ZDC_A',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j10'],False]],
        ['hi_upc_FgapA_j10_L1ZDC_C',         'L1_ZDC_C',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j10'],False]],
        ['hi_upc_FgapA_j10_L1ZDC',           'L1_ZDC',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapA', 'j10_L1ZDC'],False]],

        ['hi_upc_L2FgapA_j10_L1RD0_FILLED',    'L1_RD0_FILLED',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_L2FgapA', 'j10'],False]],
        ['hi_upc_L2FgapA_j10_L1MBTS_1',        'L1_MBTS_1',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_L2FgapA', 'j10'],False]],
        ['hi_upc_EFFgapA_j10_L1RD0_FILLED',    'L1_RD0_FILLED',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_EFFgapA', 'j10'],False]],
        ['hi_upc_EFFgapA_j10_L1MBTS_1',        'L1_MBTS_1',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_EFFgapA', 'j10'],False]],


        ['hi_upc_FgapC_j10_L1RD0_FILLED',    'L1_RD0_FILLED',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j10'],False]],
        ['hi_upc_FgapC_j10_L1MBTS_1',        'L1_MBTS_1',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j10'],False]],

        ['hi_upc_FgapC_j10_L1TE5',           'L1_TE5',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j10_L1TE5'],False]],
        ['hi_upc_FgapC_j10_L1J5',            'L1_J5',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j10_L1J5'],False]],
        ['hi_upc_FgapC_j10_L1J10',           'L1_J10',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j10_L1J10'],False]],
        ['hi_upc_FgapC_j10_L1ZDC_A',         'L1_ZDC_A',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j10'],False]],
        ['hi_upc_FgapC_j10_L1ZDC_C',         'L1_ZDC_C',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j10'],False]],
        ['hi_upc_FgapC_j10_L1ZDC',           'L1_ZDC',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_FgapC', 'j10_L1ZDC'],False]],

        ['hi_upc_L2FgapC_j10_L1RD0_FILLED',    'L1_RD0_FILLED',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_L2FgapC', 'j10'],False]],
        ['hi_upc_L2FgapC_j10_L1MBTS_1',        'L1_MBTS_1',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_L2FgapC', 'j10'],False]],
        ['hi_upc_EFFgapC_j10_L1RD0_FILLED',    'L1_RD0_FILLED',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_EFFgapC', 'j10'],False]],
        ['hi_upc_EFFgapC_j10_L1MBTS_1',        'L1_MBTS_1',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCJets"], 1, ['serial', -1, ['hi_upc_EFFgapC', 'j10'],False]],

        # UPC muons
        ['hi_upc_FgapAC_mu4',    'L1_MU4',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCMuons"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mu4'],False],],
        ['hi_upc_FgapAC_mu6',    'L1_MU6',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCMuons"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mu6'],False],],
        # do not knwo how to configure chain wiht 1 mu4 from L1 and another found by full scan
        #['hi_upc_FgapAC_mu4_mu4noL1',    'L1_MU4',   ['L1_MU4', ''], [MinBiasStream],["BW:UPC", "RATE:UPCMuons"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mu4_mu4noL1_L1MU4'],False]],
        ['hi_upc_FgapAC_mu4_L1MU4_VTE10',    'L1_MU4_VTE10',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCMuons"], 1, ['serial', -1, ['hi_upc_FgapAC', 'mu4'],False]],

        # UPC electrons
        ['hi_upc_FgapAC_e5_etcut',    'L1_EM3',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCMuons"], 1, ['serial', -1, ['hi_upc_FgapAC', 'e5_etcut'],False]],
        ['hi_upc_FgapAC_e5_lhloose',    'L1_EM3',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCMuons"], 1, ['serial', -1, ['hi_upc_FgapAC', 'e5_lhloose'],False]],

        ['hi_upc_FgapAC_2e5_etcut',    'L1_2EM3',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCMuons"], 1, ['serial', -1, ['hi_upc_FgapAC', '2e5_etcut'],False]],
        ['hi_upc_FgapAC_2e5_lhloose',    'L1_2EM3',   ['', ''], [MinBiasStream],["BW:UPC", "RATE:UPCMuons"], 1, ['serial', -1, ['hi_upc_FgapAC', '2e5_lhloose'],False]],

        # not sure what two track triggers are, will check with Peter


	['mu4_j20_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream, 'express'], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j20_ion'] ]],
	['mu4_j25_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j25_ion'] ]],
	['mu4_j30_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j30_ion'] ]],
	['mu4_j40_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j40_ion'] ]],
	['mu4_j50_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j50_ion'] ]],
	['mu4_j60_ion_dr05', 'L1_MU4',     ['L1_MU4', ''], [HardProbesStream, 'express'], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j60_ion'] ]],

	['mu6_j20_ion_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j20_ion'] ]],
	['mu6_j25_ion_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j25_ion'] ]],
	['mu6_j30_ion_dr05', 'L1_MU6',     ['L1_MU6', ''], [HardProbesStream, 'express'], ['RATE:MuonBjet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j30_ion'] ]],

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
        ]
   
    TriggerFlags.HeavyIonSlice.signatures = [
	#event shape triggers
        ['hi_v2A_th0p_L1TE50',     'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2A_th005p_L1TE50',   'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2A_th01p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2A_th025p_L1TE50',   'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2A_th05p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2A_th10p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2A_th15p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2A_th20p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],

        ['hi_v2C_th0p_L1TE50',     'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2C_th005p_L1TE50',   'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2C_th01p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2C_th025p_L1TE50',   'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2C_th05p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2C_th10p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2C_th15p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2C_th20p_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],

        ['hi_upc_FgapA_L1RD0',    'L1_RD0_FILLED',   [], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_FgapC_L1RD0',    'L1_RD0_FILLED',   [], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_FgapAC_L1RD0',    'L1_RD0_FILLED',   [], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1],



        ['hi_upc_FgapA_L1MBTS_1',    'L1_MBTS_1',   [], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_FgapC_L1MBTS_1',    'L1_MBTS_1',   [], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_FgapAC_L1MBTS_1',    'L1_MBTS_1',   [], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1],

        ['hi_upc_L2FgapA_L1MBTS_1',    'L1_MBTS_1',   [], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_L2FgapC_L1MBTS_1',    'L1_MBTS_1',   [], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_L2FgapAC_L1MBTS_1',    'L1_MBTS_1',   [], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1],

        ['hi_upc_EFFgapA_L1MBTS_1',    'L1_MBTS_1',   [], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_EFFgapC_L1MBTS_1',    'L1_MBTS_1',   [], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_EFFgapAC_L1MBTS_1',    'L1_MBTS_1',   [], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1],



        ['hi_upc_FgapA_L1ZDC',    'L1_ZDC',   [], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_FgapC_L1ZDC',    'L1_ZDC',   [], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1],
        ['hi_upc_FgapAC_L1ZDC',    'L1_ZDC',   [], [MinBiasStream],["BW:UPC", "RATE:UPCInclusive"], 1],



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
        ['mb_mbts_L1MBTS_1', 'L1_MBTS_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_2',    'L1_MBTS_2',   [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_1_1',  'L1_MBTS_1_1', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1], 
        ['mb_mbts_L1MBTS_2_2',  'L1_MBTS_2_2', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_3_3',  'L1_MBTS_3_3', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_4_4',  'L1_MBTS_4_4', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],

	#sptrk
        ['mb_sp',  'L1_RD0_FILLED', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk',  'L1_RD0_FILLED', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp_L1MBTS_1',  'L1_MBTS_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_L1MBTS_1',  'L1_MBTS_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],



        ##['mb_sptrk_ion_L1ZDC_A_C_VTE50',  'L1_ZDC_A_C_VTE50', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        ##['mb_sptrk_ion_L1MBTS_1_1_VTE50', 'L1_MBTS_1_1_VTE50', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        ##['mb_idperf_ion_L1MBTS_1', 'L1_MBTS_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ##['mb_idperf_ion_L1MBTS_1_1', 'L1_MBTS_1_1', [], [MinBiasStream, 'express'],["BW:MinBias", "RATE:MinBias"], 1],
        
        ['mb_perf_L1RD1_FILLED',    'L1_RD1_FILLED',   [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_perf_L1RD1_EMPTY',    'L1_RD1_EMPTY',   [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
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
        #version with pileup suppresion
        ['mb_sp2400_pusup500_trk120_hmt_L1TE70.0ETA24', 'L1_TE70.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp2400_pusup500_trk120_hmt_L1TE140', 'L1_TE140', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp2800_pusup600_trk140_hmt_L1TE70.0ETA24', 'L1_TE70.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp2800_pusup600_trk140_hmt_L1TE140', 'L1_TE140', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp3100_pusup700_trk160_hmt_L1TE70.0ETA24', 'L1_TE70.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp3100_pusup700_trk160_hmt_L1TE140', 'L1_TE140', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp3500_pusup800_trk180_hmt_L1TE110.0ETA24', 'L1_TE110.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp3500_pusup800_trk180_hmt_L1TE200', 'L1_TE200', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp4100_pusup900_trk200_hmt_L1TE110.0ETA24', 'L1_TE110.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp4100_pusup900_trk200_hmt_L1TE200', 'L1_TE200', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp4500_pusup1000_trk220_hmt_L1TE150.0ETA24', 'L1_TE150.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp4500_pusup1000_trk220_hmt_L1TE280', 'L1_TE280', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp4800_pusup1100_trk240_hmt_L1TE150.0ETA24', 'L1_TE150.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp4800_pusup1100_trk240_hmt_L1TE280', 'L1_TE280', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp5000_pusup1200_trk260_hmt_L1TE180.0ETA24', 'L1_TE180.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp5000_pusup1200_trk260_hmt_L1TE360', 'L1_TE360', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp5200_pusup1300_trk280_hmt_L1TE180.0ETA24', 'L1_TE180.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp5200_pusup1300_trk280_hmt_L1TE360', 'L1_TE360', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        
        ['mb_sp2400_trk120_hmt_L1TE70.0ETA24', 'L1_TE70.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp2400_trk120_hmt_L1TE140', 'L1_TE140', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp2800_trk140_hmt_L1TE70.0ETA24', 'L1_TE70.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp2800_trk140_hmt_L1TE140', 'L1_TE140', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp3100_trk160_hmt_L1TE70.0ETA24', 'L1_TE70.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp3100_trk160_hmt_L1TE140', 'L1_TE140', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp3500_trk180_hmt_L1TE110.0ETA24', 'L1_TE110.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp3500_trk180_hmt_L1TE200', 'L1_TE200', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp4100_trk200_hmt_L1TE110.0ETA24', 'L1_TE110.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp4100_trk200_hmt_L1TE200', 'L1_TE200', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp4500_trk220_hmt_L1TE150.0ETA24', 'L1_TE150.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp4500_trk220_hmt_L1TE280', 'L1_TE280', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp4800_trk240_hmt_L1TE150.0ETA24', 'L1_TE150.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp4800_trk240_hmt_L1TE280', 'L1_TE280', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp5000_trk260_hmt_L1TE180.0ETA24', 'L1_TE180.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp5000_trk260_hmt_L1TE360', 'L1_TE360', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp5200_trk280_hmt_L1TE180.0ETA24', 'L1_TE180.0ETA24', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sp5200_trk280_hmt_L1TE360', 'L1_TE360', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],

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
        ]

    TriggerFlags.CalibSlice.signatures   = [

	##['idcalib_trk9_central_L1J10_VTE100',    'L1_J10_VTE100', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ##['idcalib_trk9_fwd_L1J10_VTE100',        'L1_J10_VTE100', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
	##['idcalib_trk9_central_L1J10_VTE200',    'L1_J10_VTE200', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ##['idcalib_trk9_fwd_L1J10_VTE200',        'L1_J10_VTE200', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],

        ##['idcalib_trk9_central',    'L1_TE5_VTE200', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ##['idcalib_trk9_fwd',        'L1_TE5_VTE200', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ##['idcalib_trk16_central',   'L1_TE5_VTE200', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ##['idcalib_trk16_fwd',       'L1_TE5_VTE200', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ##['idcalib_trk29_central',   'L1_TE5_VTE200', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ##['idcalib_trk29_fwd',       'L1_TE5_VTE200', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],

        ['lumipeb_L1RD0_EMPTY',        'L1_RD0_EMPTY', [], ['PixelNoise'], ["RATE:Calibration", "RATE:PixelCalibration", "BW:Detector"], -1],
        ['lumipeb_L1RD0_FILLED',         'L1_RD0_FILLED', [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        ['lumipeb_L1RD0_UNPAIRED_ISO',    'L1_RD0_UNPAIRED_ISO',    [], ['PixelBeam'], ["RATE:Calibration", "BW:Detector"], -1],
        #['lumipeb_L1RD0_ABORTGAPNOTCALIB',    'L1_RD0_ABORTGAPNOTCALIB',    [], ['PixelBeam'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_L1MBTS_1_1',    'L1_MBTS_1_1',    [], ['PixelBeam'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_L1MBTS_1_1_UNPAIRED_ISO',    'L1_MBTS_1_1_UNPAIRED_ISO',    [], ['PixelBeam'], ["RATE:Calibration", "BW:Detector"], -1],


        # triggers for VdM scan
        ##['lumipeb_vdm_L1MBTS_2_BGRP9',          'L1_MBTS_2_BGRP9',          [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ##['lumipeb_vdm_L1MBTS_2_BGRP11',         'L1_MBTS_2_BGRP11',         [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2_UNPAIRED_ISO',   'L1_MBTS_2_UNPAIRED_ISO',   [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ##['lumipeb_vdm_L1MBTS_1_1_BGRP9',        'L1_MBTS_1_1_BGRP9',        [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ##['lumipeb_vdm_L1MBTS_1_1_BGRP11',       'L1_MBTS_1_1_BGRP11',       [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_1_1_UNPAIRED_ISO', 'L1_MBTS_1_1_UNPAIRED_ISO', [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ##['lumipeb_vdm_L1ZDC_A_C_BGRP9',         'L1_ZDC_A_C_BGRP9',         [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ##['lumipeb_vdm_L1ZDC_A_C_BGRP11',        'L1_ZDC_A_C_BGRP11',        [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1ZDC_A_C_UNPAIRED_ISO',  'L1_ZDC_A_C_UNPAIRED_ISO',  [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ##['lumipeb_vdm_L1RD0_BGRP9',             'L1_RD0_BGRP9',             [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_2',                'L1_MBTS_2',                [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1MBTS_1_1',              'L1_MBTS_1_1',              [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_vdm_L1ZDC_A_C',               'L1_ZDC_A_C',               [], ['VdM'], ["RATE:Calibration", "BW:Detector"], -1],

        
        # Monitoritems for saturated towers
        ##['satu20em_l1satmon_L1J100_FIRSTEMPTY',  'L1_J100_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        ##['satu20em_l1satmon_L1J100.31ETA49_FIRSTEMPTY',  'L1_J100.31ETA49_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        ##['satu20em_l1satmon_L1EM20VH_FIRSTEMPTY',  'L1_EM20VH_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        #['satu20em_l1satmon_L1EM20VHI_FIRSTEMPTY',  'L1_EM20VHI_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1], 
        # the one above was replaced with a em22vhi_firstempty seeded one:
        #['satu20em_l1satmon_L1EM22VHI_FIRSTEMPTY',  'L1_EM22VHI_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1], # for HI, L1EM20VH one is OK?
        ##['satu20em_l1satmon_L1J100',  'L1_J100',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        ##['satu20em_l1satmon_L1J100.31ETA49',  'L1_J100.31ETA49',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],                 
        # larnoiseburst
        ##['larnoiseburst_L1XE35',            'L1_XE35',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ##['larnoiseburst_L1XE50',            'L1_XE50',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        #['larnoiseburst_L1XE80', 'L1_XE80', [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1], # for HI, XE50 is OK?
        ['larnoiseburst_L1J50',             'L1_J50',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ##['larnoiseburst_L1J75',             'L1_J75',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ##['larnoiseburst_L1J100', 'L1_J100', [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ##['larnoiseburst_L1J40_XE50',            'L1_J40_XE50',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1], 
        ##['larnoiseburst_L1J75_XE40',            'L1_J75_XE40',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1], 

        # larpeb
        ['g8_loose_larpeb',                      'L1_EM7',     [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g12_loose_larpeb',                     'L1_EM10',     [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ##['g20_loose_larpeb_L1EM15',              'L1_EM15', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ##['g40_loose_larpeb',                     'L1_EM20', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ##['g60_loose_larpeb',                     'L1_EM20', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ##['g80_loose_larpeb',                     'L1_EM20', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ##['conej40_larpebj',  'L1_J12',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        ##['conej165_larpebj', 'L1_J75',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        ##['conej75_320eta490_larpebj',  'L1_J30.31ETA49',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        ##['conej140_320eta490_larpebj', 'L1_J75.31ETA49',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        
        # larcalib
        ##['larcalib_L1EM7_EMPTY',    'L1_EM7_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ##['larcalib_L1TAU12_EMPTY',   'L1_TAU12_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1], ##no ctpid
        ##['larcalib_L1J12_EMPTY',    'L1_J12_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ##['larcalib_L1J3031ETA49_EMPTY',  'L1_J30.31ETA49_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        
        #Special re-run chains
        ['larnoiseburst_rerun',                  '',             [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_loose_rerun',            '',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],          
        
        ]
    
    TriggerFlags.CosmicSlice.signatures  = [        
        
        ]
    TriggerFlags.StreamingSlice.signatures = [
	#TE full calo
	['noalg_mb_L1TE5',              'L1_TE5', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE10',      'L1_TE10', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE20',      'L1_TE20', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE50',      'L1_TE50', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE140',      'L1_TE140', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE200',      'L1_TE200', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE280',      'L1_TE280', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE360',      'L1_TE360', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    
    
    #TE restricted calo
    ['noalg_mb_L1TE70.0ETA24',      'L1_TE70.0ETA24', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE110.0ETA24',      'L1_TE110.0ETA24', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE150.0ETA24',      'L1_TE150.0ETA24', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
    ['noalg_mb_L1TE180.0ETA24',      'L1_TE180.0ETA24', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],


	#min bias
        ['noalg_mb_L1MBTS_1',    'L1_MBTS_1', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1MBTS_2',    'L1_MBTS_2', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1MBTS_1_1',  'L1_MBTS_1_1', [], [MinBiasStream, 'express'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1MBTS_2_2',  'L1_MBTS_2_2', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],

	['noalg_mb_L1ZDC_A_C',   'L1_ZDC_A_C', [], [MinBiasStream, 'express'], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1ZDC_A',     'L1_ZDC_A', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1ZDC_C',     'L1_ZDC_C', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1ZDC',       'L1_ZDC', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],

	['noalg_mb_L1ZDC_AND',   'L1_ZDC_AND', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
        ['zdcpeb_L1ZDC_A',      'L1_ZDC_A', [], ['zdcCalib'], ["BW:Detector", "RATE:Calibration"], -1],
        ['zdcpeb_L1ZDC_C',      'L1_ZDC_C', [], ['zdcCalib'], ["BW:Detector", "RATE:Calibration"], -1],
        ['zdcpeb_L1ZDC_AND',    'L1_ZDC_AND', [], ['zdcCalib'], ["BW:Detector", "RATE:Calibration"], -1],
        ['zdcpeb_L1ZDC_A_C',    'L1_ZDC_A_C', [], ['zdcCalib'], ["BW:Detector", "RATE:Calibration"], -1],
        ['zdcpeb_L1ZDC',    'L1_ZDC', [], ['zdcCalib'], ["BW:Detector", "RATE:Calibration"], -1],

	['noalg_mb_L1RD0_FILLED',      'L1_RD0_FILLED', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1RD0_EMPTY',       'L1_RD0_EMPTY', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1RD0_UNPAIRED_ISO','L1_RD0_UNPAIRED_ISO', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],

	['noalg_mb_L1LUCID',              'L1_LUCID', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1LUCID_EMPTY',        'L1_LUCID_EMPTY', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1LUCID_UNPAIRED_ISO', 'L1_LUCID_UNPAIRED_ISO', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],

	['noalg_mb_L1LHCF',             'L1_LHCF', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1LHCF_EMPTY',       'L1_LHCF_EMPTY', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1LHCF_UNPAIRED_ISO',    'L1_LHCF_UNPAIRED_ISO', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],

        ['lhcfpeb_L1LHCF_UNPAIRED_ISO',       'L1_LHCF_UNPAIRED_ISO',       [], ['IDFwd'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lhcfpeb_L1LHCF_EMPTY',       'L1_LHCF_EMPTY',       [], ['IDFwd'], ["RATE:Calibration", "BW:Detector"], -1],

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

	#data min-bias overlay
	['noalg_L1ZDC_A_C_OVERLAY',   'L1_ZDC_A_C_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
	##['noalg_L1TE50_OVERLAY',   'L1_TE50_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
	
	#TE restricted calo in |eta|<4.9 for overlay   

	#data zero bias overlay
	['noalg_zb_L1ZB',   'L1_ZB', [], ['ZeroBias'], ["BW:ZeroBias", "RATE:ZeroBias"], 1],

        ##['noalg_L1TGC_BURST',   'L1_TGC_BURST', [], ['TgcNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],

        #idmon streamer
        ['noalg_idmon_L1RD0_EMPTY',          'L1_RD0_EMPTY',        [], ['IDMonitoring','express'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['noalg_idmon_L1RD0_FILLED',         'L1_RD0_FILLED',        [], ['IDMonitoring'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['noalg_idmon_L1RD0_UNPAIRED_ISO',   'L1_RD0_UNPAIRED_ISO', [], ['IDMonitoring'], ["RATE:Monitoring", "BW:Detector"], -1],

        # Cosmic calo streamer
        ##['noalg_cosmiccalo_L1EM7_EMPTY',         'L1_EM7_EMPTY',        [], ['CosmicCalo'], ["BW:MinBias", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1RD1_EMPTY',         'L1_RD1_EMPTY',        [], ['CosmicCalo', 'express'], ["RATE:Calibration", "BW:Detector"], -1],
        ##['noalg_cosmiccalo_L1J3031ETA49_EMPTY',  'L1_J30.31ETA49_EMPTY',[], ['CosmicCalo'], ["BW:Jet", "RATE:Cosmic_Calo"], -1],
        ##['noalg_cosmiccalo_L1J12_EMPTY',         'L1_J12_EMPTY',        [], ['CosmicCalo', 'express'], ["BW:Jet", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1J30_EMPTY',         'L1_J30_EMPTY',        [], ['CosmicCalo', 'express'], ["BW:Jet", "RATE:Cosmic_Calo"], -1],

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
#        ['noalg_larcells_L1LAR-EM',                'L1_LAR-EM',           [], ['LArCells'],     ["RATE:SeededStreamers", "BW:Other"], -1],
#        ['noalg_larcells_L1LAR-J',                 'L1_LAR-J',            [], ['LArCells'],     ["RATE:SeededStreamers", "BW:Other"], -1],

        # HI EB
        ##['noalg_eb_L1ZDC_A_C_VTE50', 'L1_ZDC_A_C_VTE50', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ##['noalg_eb_L1TE5_VTE200', 'L1_TE5_VTE200', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1MBTS_1_1', 'L1_MBTS_1_1', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ##['noalg_eb_L1MU0_VTE50', 'L1_MU0_VTE50', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1EM12', 'L1_EM12', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1MU4', 'L1_MU4', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1TE50', 'L1_TE50', [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],

        ]

    TriggerFlags.BeamspotSlice.signatures = [
        ##['beamspot_activeTE_trkfast_peb',           'L1_J10_VTE100', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ##['beamspot_allTE_trkfast_pebTRT',           'L1_J10_VTE100', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ##['beamspot_trkFS_trkfast',                  'L1_J10_VTE100', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

	##['beamspot_activeTE_trkfast_peb_L1J10_VTE200',           'L1_J10_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ##['beamspot_allTE_trkfast_pebTRT_L1J10_VTE200',           'L1_J10_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ##['beamspot_trkFS_trkfast_L1J10_VTE200',                  'L1_J10_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

	##['beamspot_activeTE_trkfast_peb_L1TE5_VTE200',           'L1_TE5_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ##['beamspot_allTE_trkfast_pebTRT_L1TE5_VTE200',           'L1_TE5_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ##['beamspot_trkFS_trkfast_L1TE5_VTE200',                  'L1_TE5_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

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


#  LocalWords:  sptrk L1RD0

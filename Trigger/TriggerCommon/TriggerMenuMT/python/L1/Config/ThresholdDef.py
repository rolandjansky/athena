# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from ..Base.Thresholds import MuonThreshold, eEMThreshold, eEMVarThreshold, jEMThreshold, eTauThreshold, jTauThreshold, cTauThreshold, jJetThreshold, jLJetThreshold, gJetThreshold, gLJetThreshold, XEThreshold, TEThreshold, MBTSThreshold, MBTSSIThreshold, NimThreshold, NSWMonThreshold
from .L1CaloThresholdMapping import get_threshold_cut

# Max thresholds for SPARE triggers, corresponding to maximum value in L1Topo
# The pass requirement is et >= cut, so these ensure no events can pass
thrVal_SPARE = 819.1      # == 2^13-1 * 100 MeV
thrVal_SPAREXE = 104857.5 # == 2^20-1 * 100 MeV

class ThresholdDef:

    alreadyExecuted = False

    eEMVar = {
        1 : {
            "eta_bin_boundaries": [0, 0.7, 0.8, 1.1, 1.3, 1.4, 1.5, 1.8, 2.5], # 8 bins => 9 boundaries
            "shift": [ 1, 1, -1, -2,  -2, -3, -1, 0]
        }
    }

    jJVar = {
        0 : {
            # boundaries fixed in L1Topo FW, do not change!
            "eta_bin_boundaries": [0, 0.8, 1.6, 4.9], # 3 bins => 4 boundaries
            "shift": [ 0, 0, 0]
        },
        1 : {
            # boundaries fixed in L1Topo FW, do not change!
            "eta_bin_boundaries": [0, 0.8, 1.6, 4.9], # 3 bins => 4 boundaries
            "shift": [ 1, -1, 0]
        }
    }

    @staticmethod
    def addVaryingThrValues(thr, pt, shift_set):
        eta_bin_boundaries = ThresholdDef.eEMVar[shift_set]["eta_bin_boundaries"]
        shift = ThresholdDef.eEMVar[shift_set]["shift"]
        thr.addThrValue(pt+shift[0], priority=1)
        for idx,sh in reversed(list(enumerate(shift))):
            eta_min = int(10 * eta_bin_boundaries[idx])
            eta_max = int(10 * eta_bin_boundaries[idx+1])
            thr.addThrValue( pt + sh, -eta_max, -eta_min, priority=2)
        for idx,sh in enumerate(shift):
            eta_min = int(10 * eta_bin_boundaries[idx])
            eta_max = int(10 * eta_bin_boundaries[idx+1])
            thr.addThrValue( pt + sh, eta_min, eta_max, priority=2)
        return thr

    def addJetVaryingThrValues(thr, pt, shift_set, rangemin, rangemax):
        eta_bin_boundaries = ThresholdDef.jJVar[shift_set]["eta_bin_boundaries"]
        shift = ThresholdDef.jJVar[shift_set]["shift"]
        for idx,sh in reversed(list(enumerate(shift))):
            eta_min = int(10 * eta_bin_boundaries[idx])
            eta_max = int(10 * eta_bin_boundaries[idx+1])
            if -eta_min < -rangemax or -eta_max > -rangemin:
                continue
            if -eta_min > - rangemin:
                eta_min = rangemin
            if -eta_max < - rangemax:
                eta_max = rangemax
            thr.addThrValue( pt + sh, -eta_max, -eta_min, priority=1)
        for idx,sh in enumerate(shift):
            eta_min = int(10 * eta_bin_boundaries[idx])
            eta_max = int(10 * eta_bin_boundaries[idx+1])
            if eta_min > rangemax or eta_max < rangemin:
                continue
            if eta_max > rangemax:
                eta_max = rangemax
            if eta_min < rangemin:
                eta_min = rangemin
            thr.addThrValue( pt + sh, eta_min, eta_max, priority=1)
        return thr


    @staticmethod
    def registerThresholds(tc, menuName):

        if ThresholdDef.alreadyExecuted:
            raise RuntimeError("Calling ThresholdDef.registerThresholds twice")
        ThresholdDef.alreadyExecuted = True
 
        # MU (ATR-23227)
        # Primary and emergency:
        MuonThreshold( "MU3V"     ).setThrValue( thr=3, ba=4 )                     # similar to Run-2 MU4 efficiency
        MuonThreshold( "MU3VF"    ).setThrValue( thr=3, ba=4 ).setTGCFlags("F")    # similar to Run-2 MU4 rate
        MuonThreshold( "MU3VC"    ).setThrValue( thr=3, ba=4 ).setTGCFlags("C")    # to be checked
        MuonThreshold( "MU5VF"    ).setThrValue( thr=5, ba=6 ).setTGCFlags("F")    # similar to Run-2 MU6
        MuonThreshold( "MU8F"     ).setThrValue( thr=8 ).setTGCFlags("F")          # similar to Run-2 MU10
        MuonThreshold( "MU8FC"    ).setThrValue( thr=8 ).setTGCFlags("F & C")      # backup for MU8F
        MuonThreshold( "MU9VF"    ).setThrValue( thr=9,ba=8 ).setTGCFlags("F")     # backup for MU8F
        MuonThreshold( "MU9VFC"   ).setThrValue( thr=9,ba=8 ).setTGCFlags("F & C") # backup for MU8F
        MuonThreshold( "MU8VF"    ).setThrValue( thr=8, ba=10 ).setTGCFlags("F")   # similar to Run-2 MU11
        MuonThreshold( "MU8VFC"   ).setThrValue( thr=8, ba=10 ).setTGCFlags("F & C")   # backup for MU8VF
        MuonThreshold( "MU14FCH"  ).setThrValue( thr=14 ).setTGCFlags("F & C & H") # similar to Run-2 MU20
        MuonThreshold( "MU14FCHR" ).setThrValue( thr=14 ).setTGCFlags("F & C & H").setExclusionList("rpcFeet") # similar to Run-2 MU21
        MuonThreshold( "MU15VFCH" ).setThrValue( thr=15, ba=14 ).setTGCFlags("F & C & H") # similar to Run-2 MU20, bit lower rate than MU14
        MuonThreshold( "MU15VFCHR").setThrValue( thr=15, ba=14 ).setTGCFlags("F & C & H").setExclusionList("rpcFeet") # emergency        
        MuonThreshold( "MU18VFCH" ).setThrValue( thr=18, ba=14 ).setTGCFlags("F & C & H") # emergency 
        # Close-by barrel muon:
        MuonThreshold( "MU10BOM"  ).setThrValue( thr=10 ).setRPCFlags("M").setRegion("BA") # multiple close-by muons, barrel-only
        MuonThreshold( "MU12BOM"  ).setThrValue( thr=12 ).setRPCFlags("M").setRegion("BA") # multiple close-by muons, barel-only, emergency
        # Late muon:
        MuonThreshold( "MU8FH"    ).setThrValue( thr=8 ).setTGCFlags("F & H") # for late muon 
        # Alignment:
        MuonThreshold( "MU20VFC"  ).setThrValue( thr=20, ba=14 ).setTGCFlags("F & C") # alignment with toroid off
        # Commissioning:
        MuonThreshold( "MU12FCH"  ).setThrValue( thr=12 ).setTGCFlags("F & C & H")             # commissioning
        MuonThreshold( "MU4BOM"   ).setThrValue( thr=4  ).setRPCFlags("M").setRegion("BA")     # multiple close-by muons, barrel-only, commissioning
        MuonThreshold( "MU4BO"    ).setThrValue( thr=4  ).setRegion("BA")                      # barrel-only, commissioning
        MuonThreshold( "MU10BO"   ).setThrValue( thr=10 ).setRegion("BA")                      # barrel-only, commissioning
        MuonThreshold( "MU14EOF"  ).setThrValue( thr=14 ).setTGCFlags("F").setRegion("EC,FW")  # forward muon, commissioning
        MuonThreshold( "MU8EOF"   ).setThrValue( thr=8  ).setTGCFlags("F").setRegion("EC,FW")  # forward muon, commissioning
        MuonThreshold( "MU3EOF"   ).setThrValue( thr=3, ba=4 ).setTGCFlags("F").setRegion("EC,FW")  # forward muon, commissioning

        NSWMonThreshold('NSWMon')

        # eEM
        eEM_cuts = [5, 7, 9, 15, 18, 26]
        for thrV in eEM_cuts:
            eEMThreshold('eEM%i' %thrV, 'eEM').addThrValue(get_threshold_cut('eEM',thrV))

        # eEM SPARES
        for thrV in range(1,9):
            eEMThreshold('eEMSPARE%i' % thrV, 'eEM').addThrValue(thrVal_SPARE)

        # L section (used to be VH in Run2)
        eEM_cuts = [10, 12, 18, 24, 26]
        for thrV in eEM_cuts:
            eEMThreshold('eEM%iL' % thrV, 'eEM').addThrValue(get_threshold_cut('eEM',thrV)).setIsolation( reta = "Loose", wstot = "Loose", rhad = "Loose" )

        # M section (used to be VHI in Run2)
        eEM_cuts = [10, 18, 22, 26, 28]
        for thrV in eEM_cuts:
            eEMThreshold('eEM%iM' % thrV, 'eEM').addThrValue(get_threshold_cut('eEM',thrV)).setIsolation( reta = "Medium", wstot = "Medium", rhad = "Medium" )

        # T section (used to be VHIM in Run2)
        eEM_cuts = [26]
        for thrV in eEM_cuts:
            eEMThreshold('eEM%iT' % thrV, 'eEM').addThrValue(get_threshold_cut('eEM',thrV)).setIsolation( reta = "Tight", wstot = "Tight", rhad = "Tight" )

        # eEM with eta-dependent Et cuts
        eEM_cuts = [24]
        for thrV in eEM_cuts:
            ThresholdDef.addVaryingThrValues( pt= get_threshold_cut('eEM',thrV), shift_set = 1,
                                              thr=eEMVarThreshold( 'eEM%iVM' % thrV, 'eEM').setIsolation( reta = "Medium", wstot = "Medium", rhad = "Medium" ) )

        # jEM
        jEM_cuts = [20]
        for thrV in jEM_cuts:
            jEMThreshold('jEM%i' % thrV, 'jEM').addThrValue(get_threshold_cut('jEM',thrV))

        jEM_cuts = [20] # TODO: name->cut dictionary (needed as long as jEM are not calibrated)
        for thrV in jEM_cuts:
            jEMThreshold('jEM%iM' % thrV, 'jEM').addThrValue(get_threshold_cut('jEM',thrV)).setIsolation( iso = "Medium", frac = "Medium", frac2 = "Medium" )

        # jEM SPARES
        for thrV in range(1,2):
            jEMThreshold('jEMSPARE%i' % thrV, 'jEM').addThrValue(thrVal_SPARE)

        # eTAU
        eTAU_cuts = [12, 20, 30, 35, 60, 80, 140]
        for thrV in eTAU_cuts:
            eTauThreshold('eTAU%i' % thrV, 'eTAU').setEt(get_threshold_cut('eTAU', thrV))

        eTAU_cuts = [20]
        for thrV in eTAU_cuts:
            eTauThreshold('eTAU%iL' % thrV, 'eTAU').setEt(get_threshold_cut('eTAU', thrV)).setIsolation( rCore = "Loose" )
        eTAU_cuts = [20]
        for thrV in eTAU_cuts:
            eTauThreshold('eTAU%iM' % thrV, 'eTAU').setEt(get_threshold_cut('eTAU', thrV)).setIsolation( rCore = "Medium" )
        eTAU_cuts = [40]
        for thrV in eTAU_cuts:
            eTauThreshold('eTAU%iHM' % thrV, 'eTAU').setEt(get_threshold_cut('eTAU', thrV)).setIsolation( rHad = "Medium" )

        # eTAU SPARES
        for thrV in range(1,11):
            eTauThreshold('eTAUSPARE%i' % thrV, 'eTAU').setEt(thrVal_SPARE)

        # cTAU
        cTAU_cuts = [20, 30, 35]
        for thrV in cTAU_cuts:
            cTauThreshold('cTAU%iM' % thrV, 'cTAU').setEt(get_threshold_cut('cTAU', thrV)).setIsolation( isolation = "Medium" )

        # cTAU SPARES
        for thrV in range(1,3):
            cTauThreshold('cTAUSPARE%i' % thrV, 'cTAU').setEt(thrVal_SPARE)

        # jTAU
        jTAU_cuts = [20, 30]
        for thrV in jTAU_cuts:
            jTauThreshold('jTAU%i' % thrV, 'jTAU').setEt(get_threshold_cut('jTAU', thrV))
        jTAU_cuts = [30]
        for thrV in jTAU_cuts:
            jTauThreshold('jTAU%iM' % thrV, 'jTAU').setEt(get_threshold_cut('jTAU', thrV)).setIsolation( isolation = "Medium" )

        # jTAU SPARES
        for thrV in range(1,2):
            jTauThreshold('jTAUSPARE%i' % thrV, 'jTAU').setEt(thrVal_SPARE)

        # jJET (default eta range)
        # For correspondence to Run 2, see https://twiki.cern.ch/twiki/bin/viewauth/Atlas/TriggerNamingRun3#New_naming_for_Calo_items
        jJ_cuts = [20, 30, 40, 50, 55, 60, 80, 90, 125, 140, 160, 180, 500]
        for thrV in jJ_cuts:
            ThresholdDef.addJetVaryingThrValues( jJetThreshold('jJ%i' % thrV, 'jJ'), pt=get_threshold_cut('jJ', thrV), shift_set=0, rangemin=0, rangemax=31 )

        # jJET central
        jJ_cuts = [(30,25), (40,25), (55,23), (70,23), (80,25), (85,21)]
        for thrV, etamax in jJ_cuts:
            ThresholdDef.addJetVaryingThrValues( jJetThreshold( 'jJ%ip0ETA%i'  % (thrV, etamax), 'jJ'), pt=get_threshold_cut('jJ', thrV), shift_set=0, rangemin=0, rangemax=etamax )

        # jJET central, variable eta (EXAMPLE)
        # ThresholdDef.addJetVaryingThrValues( jJetThreshold('jJ12p0ETA25V', 'jJ'), pt=12, shift_set=1, rangemin=0, rangemax=25 )

        # jJET forward jet
        jJ_cuts = [40, 50, 60, 90, 125]
        for thrV in jJ_cuts:
            ThresholdDef.addJetVaryingThrValues( jJetThreshold('jJ%ip31ETA49' % thrV, 'jJ'), pt=get_threshold_cut('jJ', thrV), shift_set=0, rangemin=31, rangemax=49 )

        # jJET SPARES
        for thrV in range(1,7):
            jJetThreshold('jJSPARE%i' % thrV, 'jJ').addThrValue(thrVal_SPARE)

        # jLJET (default range)
        for thrV in [60, 80, 100, 120, 140, 160, 180, 200]:
            ThresholdDef.addJetVaryingThrValues( jLJetThreshold('jLJ%i' % thrV, 'jLJ'), pt=get_threshold_cut('jLJ', thrV), shift_set=0, rangemin=0, rangemax=31 )

        # jLJET SPARES
        for thrV in range(1,5):
            jLJetThreshold('jLJSPARE%i' % thrV, 'jLJ').addThrValue(thrVal_SPARE)

        # gJET (default range)
        for thrV in [20, 30, 40, 50, 100, 160]:
            gJetThreshold('gJ%i' % thrV, 'gJ').setEt(get_threshold_cut('gJ', thrV))

        # gLJET (default range)
        for thrV in [80, 100, 140, 160]:
            gLJetThreshold('gLJ%i' % thrV, 'gLJ').setEt(get_threshold_cut('gLJ',  thrV))

        # gLJET SPARES
        for thrV in range(1,5):
            gLJetThreshold('gLJSPARE%i' % thrV, 'gLJ').setEt(thrVal_SPARE)

        # gXE
        gXE_cuts = [70, 100]
        for thrV in gXE_cuts:
            XEThreshold('gXENC%i' % thrV, 'gXE').setXE(get_threshold_cut('gXENC', thrV))

        gXE_cuts = [70, 100]
        for thrV in gXE_cuts:
            XEThreshold('gXERHO%i' % thrV, 'gXE').setXE(get_threshold_cut('gXERHO', thrV))

        gXE_cuts = [70, 80, 100]
        for thrV in gXE_cuts:
            XEThreshold('gXEJWOJ%i' % thrV, 'gXE').setXE(get_threshold_cut('gXEJWOJ', thrV))

        # gMHT
        for thrV in [500]:
            XEThreshold('gMHT%i' % thrV, 'gXE').setXE(thrV)
        
        # gTE
        for thrV in [200]:
            TEThreshold('gTE%i' % thrV, 'gTE').setTE(thrV)

        # jXE
        jXE_cuts = [70, 80, 100, 110, 500]
        for thrV in jXE_cuts:
            XEThreshold('jXE%i' % thrV, 'jXE').setXE(get_threshold_cut('jXE', thrV))

        # ENERGY SPARES
        for thrV in range(1,30):
            XEThreshold('jXESPARE%i' % thrV, 'jXE').setXE(thrVal_SPAREXE)

        jXE_cuts = [100]
        for thrV in jXE_cuts:
            XEThreshold('jXEC%i' % thrV, 'jXE').setXE(get_threshold_cut('jXE', thrV))

        # ATR-24037
        jXE_cuts = [100]
        for thrV in jXE_cuts:
            XEThreshold('jXEPerf%i' % thrV, 'jXE').setXE(get_threshold_cut('jXE', thrV))

        # jTE
        for thrV in [200,]:
            TEThreshold('jTE%i' % thrV, 'jTE').setTE(thrV)

        for thrV in [200,]:
            TEThreshold('jTEC%i' % thrV, 'jTE').setTE(thrV)

        for thrV in [100,]:
            TEThreshold('jTEFWD%i' % thrV, 'jTE').setTE(thrV)

        for thrV in [100,]:
            TEThreshold('jTEFWDA%i' % thrV, 'jTE').setTE(thrV)

        for thrV in [100,]:
            TEThreshold('jTEFWDC%i' % thrV, 'jTE').setTE(thrV)

        # CALREQ
            
        for i in range(3):
            NimThreshold('CAL%i' % i, 'CALREQ', mapping=i)

        ## MBTS

        # MBTS naming scheme defined in
        # https://docs.google.com/spreadsheets/d/1R0s8Lw-0nPSjqe9YTuZBCeAdedn_Ve4Ax6bbMe_4bSk/edit#gid=1818891632

        # run 1
        thresholdA=[ 32.04, 26.98, 35.00, 33.54, 32.08, 36.46, 30.63, 32.08, 33.54, 30.63, 29.17, 33.54, 32.08, 32.08, 30.63, 26.25]
        thresholdC=[ 55.42, 31.98, 32.81, 49.48, 98.44, 32.11, 32.62, 29.90, 24.06, 25.81, 25.52, 35.00, 27.71, 36.46, 26.25, 30.63]

        # run 2 above MBTS_A08 only the even numbers are used
        thresholdA=[ 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0]
        thresholdC=[ 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0]

        for i, (vA, vC) in enumerate(zip(thresholdA, thresholdC)):
            MBTSSIThreshold('MBTS_A%i' % i).setVoltage(vA)
            MBTSSIThreshold('MBTS_C%i' % i).setVoltage(vC)

        thr_mbtsA = MBTSThreshold('MBTS_A', mapping=0)
        thr_mbtsC = MBTSThreshold('MBTS_C', mapping=1)
        for x in range(16):
            if tc.thresholdExists('MBTS_A%i' % x):
                thr_mbtsA.addSector( tc.getDefinedThreshold('MBTS_A%i' % x) )
            if tc.thresholdExists('MBTS_C%i' % x):
                thr_mbtsC.addSector( tc.getDefinedThreshold('MBTS_C%i' % x) )



        ## ZDC
        
        NimThreshold('ZDC_0',   'ZDC')
        NimThreshold('ZDC_1',   'ZDC')
        NimThreshold('ZDC_2',   'ZDC')


        ## BCM

        NimThreshold('BCM_AtoC', 'BCM')
        NimThreshold('BCM_CtoA', 'BCM')
        NimThreshold('BCM_Wide', 'BCM')
        NimThreshold('BCM_Comb', 'BCMCMB')
        NimThreshold('BCM_MCA',  'BCM')
        NimThreshold('BCM_MCC',  'BCM')
        NimThreshold('BCM6',    'BCM')
        NimThreshold('BCM7',    'BCM')
        NimThreshold('BCM8',    'BCM')

        ## BMA
        # Phase-II lumi demonstrator system
        # DO NOT USE FOR CTP OUTPUTS
        NimThreshold('BMA0',    'NIM') #LUCID?
        NimThreshold('BMA1',    'NIM') #LUCID?

        ## LUCID

        NimThreshold('LUCID_A', 'LUCID')
        NimThreshold('LUCID_C', 'LUCID')
        NimThreshold('LUCID_Coinc_AC', 'LUCID')
        NimThreshold('LUCID_COMM', 'LUCID')
        NimThreshold('LUCID_05', 'LUCID')
        NimThreshold('LUCID_06', 'LUCID')

        ## AFP (ATR-23476)
        NimThreshold('AFP_NSA', 'NIM', mapping=2)        
        NimThreshold('AFP_FSA', 'NIM', mapping=3)        
        NimThreshold('AFP_FSA_TOF_T0', 'NIM', mapping=4)        
        NimThreshold('AFP_FSA_TOF_T1', 'NIM', mapping=5)  
        NimThreshold('AFP_FSA_TOF_T2', 'NIM', mapping=6)        
        NimThreshold('AFP_FSA_TOF_T3', 'NIM', mapping=7)        
        NimThreshold('AFP_NSC', 'NIM', mapping=15)        
        NimThreshold('AFP_FSC', 'NIM', mapping=16)        
        NimThreshold('AFP_FSC_TOF_T0', 'NIM', mapping=17)     
        NimThreshold('AFP_FSC_TOF_T1', 'NIM', mapping=18)
        NimThreshold('AFP_FSC_TOF_T2', 'NIM', mapping=19)     
        NimThreshold('AFP_FSC_TOF_T3', 'NIM', mapping=20)


        ## BPTX
        NimThreshold('BPTX0', 'BPTX')
        NimThreshold('BPTX1', 'BPTX')


        ## Other NIMs
        NimThreshold('NIML1A',  'NIM', mapping=0)
        NimThreshold('NIMLHCF', 'NIM', mapping=1)
        NimThreshold('NIMTGC',  'NIM', mapping=12)
        NimThreshold('NIMRPC',  'NIM', mapping=13)
        NimThreshold('NIMTRT',  'NIM', mapping=14)

        # ALFA
        LUT1offset =  2 # this is needed because the first 4 direct inputs are in a LUT with 8 PITs so the OR with the next inputs would not work
        LUT2offset =  8
        LUT3offset = 14
        LUT4offset = 20
        LUT5offset = 26
        for i, alfa in enumerate( ['B7R1L', 'B7R1U', 'A7R1L', 'A7R1U', 'A7L1L', 'A7L1U', 'B7L1L', 'B7L1U'] ):
            phaseOffset = 32 * (i%2)
            NimThreshold('ALFA_%s'    % alfa, 'ALFA', mapping = LUT1offset + i/2 + phaseOffset )
            NimThreshold('ALFA2_%s'   % alfa, 'ALFA', mapping = LUT2offset + i/2 + phaseOffset )
            NimThreshold('ALFA3_%s'   % alfa, 'ALFA', mapping = LUT3offset + i/2 + phaseOffset )
            NimThreshold('ALFA4_%s'   % alfa, 'ALFA', mapping = LUT4offset + i/2 + phaseOffset )
            NimThreshold('ALFA_%s_OD' % alfa, 'ALFA', mapping = LUT5offset + i/2 + phaseOffset )

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from ..Base.Thresholds import MuonThreshold, eEMThreshold, eEMVarThreshold, jEMThreshold, eTauThreshold, jTauThreshold, cTauThreshold, jJetThreshold, jLJetThreshold, gJetThreshold, gLJetThreshold, XEThreshold, TEThreshold, MBTSThreshold, MBTSSIThreshold, NimThreshold

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

        # eEM
        eEM_cuts = {5:3, 7:5, 9:7, 15:12, 18:15, 26:22} # TODO: name->cut dictionary (needed as long as eEM are not calibrated) 
        for name, cut in eEM_cuts.items():
            eEMThreshold('eEM%i' %name, 'eEM').addThrValue(cut)

        # eEM SPARES
        for thrV in range(1,9):
            eEMThreshold('eEMSPARE%i' % thrV, 'eEM').addThrValue(8191)

        # L section (used to be VH in Run2)
        eEM_cuts = {10:8, 12:10, 18:15, 24:20, 26:22} # TODO: name->cut dictionary (needed as long as eEM are not calibrated)
        for name, cut in eEM_cuts.items():
            eEMThreshold('eEM%iL' % name, 'eEM').addThrValue(cut).setIsolation( reta = "Loose", wstot = "Loose", rhad = "Loose" )

        # M section (used to be VHI in Run2)
        eEM_cuts = {10:8, 18:15, 22:18, 26:22, 28:24} # TODO: name->cut dictionary (needed as long as eEM are not calibrated)
        for name, cut in eEM_cuts.items():
            eEMThreshold('eEM%iM' % name, 'eEM').addThrValue(cut).setIsolation( reta = "Medium", wstot = "Medium", rhad = "Medium" )

        # T section (used to be VHIM in Run2)
        eEM_cuts = {26:22} # TODO: name->cut dictionary (needed as long as eEM are not calibrated)
        for name, cut in eEM_cuts.items():
            eEMThreshold('eEM%iT' % name, 'eEM').addThrValue(cut).setIsolation( reta = "Tight", wstot = "Tight", rhad = "Tight" )

        # eEM with eta-dependent Et cuts
        eEM_cuts = {24:20} # TODO: name->cut dictionary (needed as long as eEM are not calibrated)
        for name, cut in eEM_cuts.items():
            ThresholdDef.addVaryingThrValues( eEMVarThreshold( 'eEM%iVM' % name, 'eEM').setIsolation( reta = "Medium", wstot = "Medium", rhad = "Medium" ), pt= cut, shift_set = 1 )

        # jEM
        jEM_cuts = {20:15} # TODO: name->cut dictionary (needed as long as jEM are not calibrated)
        for name, cut in jEM_cuts.items():
            jEMThreshold('jEM%i' % name, 'jEM').addThrValue(cut)

        jEM_cuts = {20:15} # TODO: name->cut dictionary (needed as long as jEM are not calibrated)
        for name, cut in jEM_cuts.items():
            jEMThreshold('jEM%iM' % name, 'jEM').addThrValue(cut).setIsolation( iso = "Medium", frac = "Medium", frac2 = "Medium" )

        # jEM SPARES
        for thrV in range(1,2):
            jEMThreshold('jEMSPARE%i' % thrV, 'jEM').addThrValue(8191)

        # eTAU
        eTAU_cuts = {12:8, 20:12, 30:20, 35:25, 60:40, 80:60, 140:100} # TODO: name->cut dictionary (needed as long as eTAU are not calibrated)
        for name, cut in eTAU_cuts.items():
            eTauThreshold('eTAU%i' % name, 'eTAU').setEt(cut)

        eTAU_cuts = {20:12} # TODO: name->cut dictionary (needed as long as eTAU are not calibrated)
        for name, cut in eTAU_cuts.items():
            eTauThreshold('eTAU%iL' % name, 'eTAU').setEt(cut).setIsolation( rCore = "Loose" )
        eTAU_cuts = {20:12} # TODO: name->cut dictionary (needed as long as eTAU are not calibrated)
        for name, cut in eTAU_cuts.items():
            eTauThreshold('eTAU%iM' % name, 'eTAU').setEt(cut).setIsolation( rCore = "Medium" )
        eTAU_cuts = {40:30} # TODO: name->cut dictionary (needed as long as eTAU are not calibrated)
        for name, cut in eTAU_cuts.items():
            eTauThreshold('eTAU%iHM' % name, 'eTAU').setEt(cut).setIsolation( rHad = "HadMedium" )

        # eTAU SPARES
        for thrV in range(1,11):
            eTauThreshold('eTAUSPARE%i' % thrV, 'eTAU').setEt(8191)  

        # cTAU
        cTAU_cuts = {20:12, 30:20, 35:25} # TODO: name->cut dictionary (needed as long as eTAU are not calibrated)
        for name, cut in cTAU_cuts.items():
            cTauThreshold('cTAU%iM' % name, 'cTAU').setEt(cut).setIsolation( isolation = "Medium" )

        # cTAU SPARES
        for thrV in range(1,3):
            cTauThreshold('cTAUSPARE%i' % thrV, 'cTAU').setEt(8191)

        # jTAU
        jTAU_cuts = {20:12, 30:20} # TODO: name->cut dictionary (needed as long as jTAU are not calibrated)
        for name, cut in jTAU_cuts.items():
            jTauThreshold('jTAU%i' % name, 'jTAU').setEt(cut)
        jTAU_cuts = {30:20} # TODO: name->cut dictionary (needed as long as jTAU are not calibrated)
        for name, cut in jTAU_cuts.items():
            jTauThreshold('jTAU%iM' % name, 'jTAU').setEt(cut).setIsolation( isolation = "Medium" )

        # jTAU SPARES
        for thrV in range(1,2):
            jTauThreshold('jTAUSPARE%i' % thrV, 'jTAU').setEt(8191)

        # jJET (default range)
        jJ_cuts = {20:5, 30:12, 40:15, 50:20, 55:25, 60:30, 80:40, 90:50, 125:75, 140:85, 160:100, 180:120, 500:400} # TODO: name->cut dictionary (needed as long as jJ are not calibrated)
        for name, cut in jJ_cuts.items():
            ThresholdDef.addJetVaryingThrValues( jJetThreshold('jJ%i' % name, 'jJ'), pt=cut, shift_set=0, rangemin=0, rangemax=31 )

        # jJET central
        jJ_cuts = {30:(12,25), 40:(15,25), 55:(25,23), 70:(35,23), 80:(40,25), 85:(45,21)}
        for name, (thrV, etamax) in jJ_cuts.items():
            ThresholdDef.addJetVaryingThrValues( jJetThreshold( 'jJ%ip0ETA%i'  % (name, etamax), 'jJ'), pt=thrV, shift_set=0, rangemin=0, rangemax=etamax )

        # jJET central, variable eta (EXAMPLE)
        # ThresholdDef.addJetVaryingThrValues( jJetThreshold('jJ12p0ETA25V', 'jJ'), pt=12, shift_set=1, rangemin=0, rangemax=25 )

        # jJET forward jet
        jJ_cuts = {40:15, 50:20, 60:30, 90:50, 125:75} # TODO: name->cut dictionary (needed as long as jJ are not calibrated)
        for name, cut in jJ_cuts.items():
            ThresholdDef.addJetVaryingThrValues( jJetThreshold('jJ%ip31ETA49' % name, 'jJ'), pt=cut, shift_set=0, rangemin=31, rangemax=49 )

        # jJET SPARES
        for thrV in range(1,7):
            jJetThreshold('jJSPARE%i' % thrV, 'jJ').addThrValue(8191)

        # jLJET (default range)
        for thrV in [80, 100, 140, 160]:
            ThresholdDef.addJetVaryingThrValues( jLJetThreshold('jLJ%i' % thrV, 'jLJ'), pt=thrV, shift_set=0, rangemin=0, rangemax=31 )

        # jLJET SPARES
        for thrV in range(1,5):
            jLJetThreshold('jLJSPARE%i' % thrV, 'jLJ').addThrValue(8191)

        # gJET (default range)
        for thrV in [20, 30, 40, 50, 100, 160]:
            gJetThreshold('gJ%i' % thrV, 'gJ').setEt(thrV)

        # gLJET (default range)
        for thrV in [80, 100, 140, 160]:
            gLJetThreshold('gLJ%i' % thrV, 'gLJ').setEt(thrV)

        # gLJET SPARES
        for thrV in range(1,5):
            gLJetThreshold('gLJSPARE%i' % thrV, 'gLJ').setEt(8191)

        # gXE
        gXE_cuts = {70:30, 100:50} # TODO: name->cut dictionary (needed as long as gXE are not calibrated)
        for name, cut in gXE_cuts.items():
            XEThreshold('gXENC%i' % name, 'gXE').setXE(cut)

        gXE_cuts = {70:30, 100:50} # TODO: name->cut dictionary (needed as long as gXE are not calibrated)
        for name, cut in gXE_cuts.items():
            XEThreshold('gXERHO%i' % name, 'gXE').setXE(cut)

        gXE_cuts = {70:30, 80:40, 100:50} # TODO: name->cut dictionary (needed as long as gXE are not calibrated)
        for name, cut in gXE_cuts.items():
            XEThreshold('gXEJWOJ%i' % name, 'gXE').setXE(cut)

        # gMHT
        for thrV in [500]:
            XEThreshold('gMHT%i' % thrV, 'gXE').setXE(thrV)
        
        # gTE
        for thrV in [200]:
            TEThreshold('gTE%i' % thrV, 'gTE').setTE(thrV)

        # jXE
        jXE_cuts = {70:35, 80:40, 100:50, 110:55, 500:300} # TODO: name->cut dictionary (needed as long as jXE are not calibrated)
        for name, cut in jXE_cuts.items():
            XEThreshold('jXE%i' % name, 'jXE').setXE(cut)

        # ENERGY SPARES
        for thrV in range(1,30):
            XEThreshold('jXESPARE%i' % thrV, 'jXE').setXE(1048575)

        jXE_cuts = {100:50} # TODO: name->cut dictionary (needed as long as jXE are not calibrated)
        for name, cut in jXE_cuts.items():
            XEThreshold('jXEC%i' % name, 'jXE').setXE(cut)

        # ATR-24037
        jXE_cuts = {100:50} # TODO: name->cut dictionary (needed as long as jXE are not calibrated)
        for name, cut in jXE_cuts.items():
            XEThreshold('jXEPerf%i' % name, 'jXE').setXE(cut)

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
        NimThreshold('BCM_X',    'BCM')

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

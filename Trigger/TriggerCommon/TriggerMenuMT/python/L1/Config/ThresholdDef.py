# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from ..Base.Thresholds import MuonThreshold, EMThreshold, eTauThreshold, jTauThreshold, cTauThreshold, JetThreshold, XEThreshold, MBTSThreshold, MBTSSIThreshold, NimThreshold

class ThresholdDef:

    alreadyExecuted = False

    eEMVar = {
        1 : {
            "eta_bin_boundaries": [0, 0.7, 0.8, 1.1, 1.3, 1.4, 1.5, 1.8, 2.5], # 8 bins => 9 boundaries
            "shift": [ 1, 1, -1, -2,  -2, -3, -1, 0]
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
        MuonThreshold( "MU20FC"   ).setThrValue( thr=20, ba=14 ).setTGCFlags("F & C") # alignment with toroid off
        # Commissioning:
        MuonThreshold( "MU12FCH"  ).setThrValue( thr=12 ).setTGCFlags("F & C & H")             # commissioning
        MuonThreshold( "MU4BOM"   ).setThrValue( thr=4  ).setRPCFlags("M").setRegion("BA")     # multiple close-by muons, barrel-only, commissioning
        MuonThreshold( "MU4BO"    ).setThrValue( thr=4  ).setRegion("BA")                      # barrel-only, commissioning
        MuonThreshold( "MU14EOF"  ).setThrValue( thr=14 ).setTGCFlags("F").setRegion("EC,FW")  # forward muon, commissioning
        MuonThreshold( "MU8EOF"   ).setThrValue( thr=8  ).setTGCFlags("F").setRegion("EC,FW")  # forward muon, commissioning
        MuonThreshold( "MU3EOF"   ).setThrValue( thr=3, ba=4 ).setTGCFlags("F").setRegion("EC,FW")  # forward muon, commissioning

        # backward compatible threshold names (definition like above)
        MuonThreshold( "MU4" ).setThrValue( thr=3, ba=4 )                     # similar to Run-2 MU4 efficiency
        MuonThreshold( "MU6" ).setThrValue( thr=5, ba=6 ).setTGCFlags("F")    # similar to Run-2 MU6
        MuonThreshold( "MU10").setThrValue( thr=8 ).setTGCFlags("F")          # similar to Run-2 MU10
        MuonThreshold( "MU11").setThrValue( thr=8, ba=10 ).setTGCFlags("F")   # similar to Run-2 MU11
        MuonThreshold( "MU15").setThrValue( thr=12).setTGCFlags("F & c & H")  # guess
        MuonThreshold( "MU20").setThrValue( thr=14 ).setTGCFlags("F & C & H") # similar to Run-2 MU20
        MuonThreshold( "MU21" ).setThrValue( thr=14 ).setTGCFlags("F & C & H").setExclusionList("rpcFeet") # similar to Run-2 MU21

        # special threshold for magnet-off menu
        MuonThreshold( "MU0").setThrValue( thr=0 )



        # EM 
        for thrV in [3, 5, 8, 15, 20, 22]:
            #EMThreshold('eEM%i' % thrV, 'eEM').addThrValue(thrV)
            ThresholdDef.addVaryingThrValues( EMThreshold(  'eEM%i'% thrV,  'eEM'), pt=thrV,  shift_set = 1 )

        # L section (used to be VH in Run2)
        ThresholdDef.addVaryingThrValues( EMThreshold(  'eEM8L',  'eEM').setIsolation( reta = "Loose", wstot = "Loose", rhad = "Loose" ),    pt=8,  shift_set = 1 ) 
        ThresholdDef.addVaryingThrValues( EMThreshold( 'eEM10L',  'eEM').setIsolation( reta = "Loose", wstot = "Loose", rhad = "Loose" ),    pt=10, shift_set = 1 )
        ThresholdDef.addVaryingThrValues( EMThreshold( 'eEM15L',  'eEM').setIsolation( reta = "Loose", wstot = "Loose", rhad = "Loose" ),    pt=15, shift_set = 1 )
        ThresholdDef.addVaryingThrValues( EMThreshold( 'eEM20L',  'eEM').setIsolation( reta = "Loose", wstot = "Loose", rhad = "Loose" ),    pt=20, shift_set = 1 )
        ThresholdDef.addVaryingThrValues( EMThreshold( 'eEM22L',  'eEM').setIsolation( reta = "Loose", wstot = "Loose", rhad = "Loose" ),    pt=22, shift_set = 1 )

        # M section (used to be VHI in Run2)
        ThresholdDef.addVaryingThrValues( EMThreshold(  'eEM8M',  'eEM').setIsolation( reta = "Medium", wstot = "Medium", rhad = "Medium" ), pt=8,  shift_set = 1 )
        ThresholdDef.addVaryingThrValues( EMThreshold( 'eEM15M',  'eEM').setIsolation( reta = "Medium", wstot = "Medium", rhad = "Medium" ), pt=15, shift_set = 1 )
        ThresholdDef.addVaryingThrValues( EMThreshold( 'eEM20M',  'eEM').setIsolation( reta = "Medium", wstot = "Medium", rhad = "Medium" ), pt=20, shift_set = 1 )
        ThresholdDef.addVaryingThrValues( EMThreshold( 'eEM22M',  'eEM').setIsolation( reta = "Medium", wstot = "Medium", rhad = "Medium" ), pt=22, shift_set = 1 )

        # T section (used to be VHIM in Run2)
        ThresholdDef.addVaryingThrValues( EMThreshold( 'eEM22T',  'eEM').setIsolation( reta = "Tight", wstot = "Tight", rhad = "Tight" ),    pt=22, shift_set = 1 )

        # eTAU
        for et in [8, 12, 20, 25, 40, 60, 100]:
            eTauThreshold('eTAU%i' % et, 'eTAU').setEt(et)

        for et in [12]:
            eTauThreshold('eTAU%iL' % et, 'eTAU').setEt(et).setIsolation( isoConeRel = "Loose" )
        for et in [12]:
            eTauThreshold('eTAU%iM' % et, 'eTAU').setEt(et).setIsolation( isoConeRel = "Medium" )
        for et in [30]:
            eTauThreshold('eTAU%iH' % et, 'eTAU').setEt(et).setIsolation( fEM = "Had" )
  
        # cTAU
        for et in [12, 20, 25]:
            cTauThreshold('cTAU%iM' % et, 'cTAU').setEt(et).setIsolation( isolation = "Medium" )

        # jTAU
        for et in [12]:
            jTauThreshold('jTAU%i' % et, 'jTAU').setEt(et)
        for et in [12]:
            jTauThreshold('jTAU%iM' % et, 'jTAU').setEt(et).setIsolation( isolation = "Medium" )

        # JET
        for thrV in [12, 15, 20, 25, 30, 40, 50, 75, 85, 100, 120, 400]:
            JetThreshold('jJ%i' % thrV, 'jJ').setPt(thrV).addRange(etamin=-31, etamax=31) # jets are between -31 and 31 -ATR-11526

        # Central jet
        for (thrV, etamax) in [(12,25), (15,25), (25,23), (35,23), (40,25), (45,20)]:
            JetThreshold('jJ%ip0ETA%i'  % (thrV, etamax), 'jJ').setPt(thrV).addRange(etamin = -etamax,  etamax = etamax)  

        # Standard forward jet
        for thrV in [15, 20, 30, 50, 75]:
            JetThreshold('jJ%ip31ETA49' % thrV, 'jJ').setPt(thrV).addRange(etamin=31, etamax=49).addRange(etamin=-49, etamax=-31)

        # XE
        for thrV in [20, 50]:
            XEThreshold('gXEPUFIT%i' % thrV, 'gXE').setXE(thrV)

        for thrV in [20, 30, 35, 40, 45, 50]:
            XEThreshold('gXERHO%i' % thrV, 'gXE').setXE(thrV)

        for thrV in [50]:
            XEThreshold('gXE%i' % thrV, 'gXE').setXE(thrV)

        for thrV in [50]:
            XEThreshold('jXE%i' % thrV, 'jXE').setXE(thrV)




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
        
        NimThreshold('ZDC_A',   'ZDC')
        NimThreshold('ZDC_C',   'ZDC')
        NimThreshold('ZDC_AND', 'ZDC')


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

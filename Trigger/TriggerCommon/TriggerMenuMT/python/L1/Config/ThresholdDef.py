# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.Logging import logging
log = logging.getLogger('Menu.L1.Config.ThresholdDef')

from ..Base.Thresholds import Threshold, MuonThreshold, EMThreshold, TauThreshold, JetThreshold, XEThreshold, MBTSThreshold, MBTSSIThreshold, NimThreshold, ThresholdValue

class ThresholdDef:

    alreadyExecuted = False

    @staticmethod
    def registerThresholds(tc):

        if ThresholdDef.alreadyExecuted: 
            raise RuntimeError("Calling ThresholdDef.registerThresholds twice")
            return # just for safety
        ThresholdDef.alreadyExecuted = True
        
        # MU
        ThresholdValue.setDefaults('MU', {})

        MuonThreshold( 'MU0'   ).setThrValue( thr = 4 )
        MuonThreshold( 'MU4'   ).setThrValue( ba=4,  ec=4,  fw=4 )
        MuonThreshold( 'MU5'   ).setThrValue( ba=4,  ec=4,  fw=6  )
        MuonThreshold( 'MU6'   ).setThrValue( thr=6  )
        MuonThreshold( 'MU6M'  ).setThrValue( thr=6,  fw=8  ).setTGCFlags("F & C | F & H | C & H")
        MuonThreshold( 'MU10'  ).setThrValue( thr=8  )
        MuonThreshold( 'MU11'  ).setThrValue( thr=10 )
        MuonThreshold( 'MU15'  ).setThrValue( thr=14, ba=15 )
        MuonThreshold( 'MU20'  ).setThrValue( thr=20 )
        MuonThreshold( 'MU21'  ).setThrValue( thr=20 ).setExclusionList("rpcFeet")
        MuonThreshold( 'MU20BA').setThrValue( thr=20 ).setRegion("BA")
        MuonThreshold( 'MU20NF').setThrValue( thr=20 ).setExclusionList("rpcFeet")
        MuonThreshold( 'MU22'  ).setThrValue( thr=20 )
        MuonThreshold( 'MU24'  ).setThrValue( thr=20 )
        MuonThreshold( 'MU26'  ).setThrValue( thr=20 )


        # EM 
        for thrV in [3, 7, 8, 10, 15, 20, 22]:
            EMThreshold('eEM%i' % thrV, 'eEM').addThrValue(thrV)

        # VH section

        ThresholdValue.setDefaults('EM', {'isobits' : '00001', 'use_relIso' : True })

        EMThreshold( 'eEM8VH', 'eEM').addThrValue(9, priority=1)\
            .addThrValue(9, -8, 0, priority=2).addThrValue(9, 0, 8, priority=2)\
            .addThrValue(7, -11, -8, priority=2).addThrValue(7, 8, 11, priority=2)\
            .addThrValue(6, -14, -11, priority=2).addThrValue(6, 11, 14, priority=2)\
            .addThrValue(5, -15, -14, priority=2).addThrValue(5, 14, 15, priority=2)\
            .addThrValue(7, -18, -15, priority=2).addThrValue(7, 15, 18, priority=2)\
            .addThrValue(8, -25, -18, priority=2).addThrValue(8, 18, 25, priority=2)
        
        EMThreshold( 'eEM10VH', 'eEM' ).addThrValue(11, priority=1)\
            .addThrValue(11, -8, 0, priority=2).addThrValue(11, 0, 8, priority=2)\
            .addThrValue(9, -11, -8, priority=2).addThrValue(9, 8, 11, priority=2)\
            .addThrValue(8, -14, -11, priority=2).addThrValue(8, 11, 14, priority=2)\
            .addThrValue(7, -15, -14, priority=2).addThrValue(7, 14, 15, priority=2)\
            .addThrValue(9, -18, -15, priority=2).addThrValue(9, 15, 18, priority=2)\
            .addThrValue(10, -25, -18, priority=2).addThrValue(10, 18, 25, priority=2)

        EMThreshold( 'eEM15VH', 'eEM').addThrValue(17, priority=1)\
            .addThrValue(17, -7, 0, priority=2).addThrValue(17, 0, 7, priority=2)\
            .addThrValue(16, -9, -7, priority=2).addThrValue(16, 7, 9, priority=2)\
            .addThrValue(15, -12, -9, priority=2).addThrValue(15, 9, 12, priority=2)\
            .addThrValue(14, -14, -12, priority=2).addThrValue(14, 12, 14, priority=2)\
            .addThrValue(13, -15, -14, priority=2).addThrValue(13, 14, 15, priority=2)\
            .addThrValue(15, -17, -15, priority=2).addThrValue(15, 15, 17, priority=2)\
            .addThrValue(16, -25, -17, priority=2).addThrValue(16, 17, 25, priority=2)  
      
        EMThreshold( 'eEM20VH', 'eEM').addThrValue(22, priority=1)\
            .addThrValue(22, -7, 0, priority=2).addThrValue(22, 0, 7, priority=2)\
            .addThrValue(21, -8, -7, priority=2).addThrValue(21, 7, 8, priority=2)\
            .addThrValue(20, -11, -8, priority=2).addThrValue(20, 8, 11, priority=2)\
            .addThrValue(19, -13, -11, priority=2).addThrValue(19, 11, 13, priority=2)\
            .addThrValue(18, -14, -13, priority=2).addThrValue(18, 13, 14, priority=2)\
            .addThrValue(17, -15, -14, priority=2).addThrValue(17, 14, 15, priority=2)\
            .addThrValue(19, -17, -15, priority=2).addThrValue(19, 15, 17, priority=2)\
            .addThrValue(21, -25, -17, priority=2).addThrValue(21, 17, 25, priority=2)       

        # (V)HI section

        ThresholdValue.setDefaults('EM', {'isobits' : '00100', 'use_relIso' : True })

        EMThreshold( 'eEM15VHI', 'eEM').addThrValue(17, priority=1)\
            .addThrValue(17, -7, 0, priority=2).addThrValue(17, 0, 7, priority=2)\
            .addThrValue(16, -9, -7, priority=2).addThrValue(16, 7, 9, priority=2)\
            .addThrValue(15, -12, -9, priority=2).addThrValue(15, 9, 12, priority=2)\
            .addThrValue(14, -14, -12, priority=2).addThrValue(14, 12, 14, priority=2)\
            .addThrValue(13, -15, -14, priority=2).addThrValue(13, 14, 15, priority=2)\
            .addThrValue(15, -17, -15, priority=2).addThrValue(15, 15, 17, priority=2)\
            .addThrValue(16, -25, -17, priority=2).addThrValue(16, 17, 25, priority=2)

        EMThreshold( 'eEM18VHI', 'eEM').addThrValue(20, priority=1)\
            .addThrValue(20, -7, 0, priority=2).addThrValue(20, 0, 7, priority=2)\
            .addThrValue(19, -8, -7, priority=2).addThrValue(19, 7, 8, priority=2)\
            .addThrValue(18, -11, -8, priority=2).addThrValue(18, 8, 11, priority=2)\
            .addThrValue(17, -13, -11, priority=2).addThrValue(17, 11, 13, priority=2)\
            .addThrValue(16, -14, -13, priority=2).addThrValue(16, 13, 14, priority=2)\
            .addThrValue(15, -15, -14, priority=2).addThrValue(15, 14, 15, priority=2)\
            .addThrValue(17, -17, -15, priority=2).addThrValue(17, 15, 17, priority=2)\
            .addThrValue(19, -25, -17, priority=2).addThrValue(19, 17, 25, priority=2)

        EMThreshold( 'eEM20VHI', 'eEM').addThrValue(22, priority=1)\
            .addThrValue(22, -7, 0, priority=2).addThrValue(22, 0, 7, priority=2)\
            .addThrValue(21, -8, -7, priority=2).addThrValue(21, 7, 8, priority=2)\
            .addThrValue(20, -11, -8, priority=2).addThrValue(20, 8, 11, priority=2)\
            .addThrValue(19, -13, -11, priority=2).addThrValue(19, 11, 13, priority=2)\
            .addThrValue(18, -14, -13, priority=2).addThrValue(18, 13, 14, priority=2)\
            .addThrValue(17, -15, -14, priority=2).addThrValue(17, 14, 15, priority=2)\
            .addThrValue(19, -17, -15, priority=2).addThrValue(19, 15, 17, priority=2)\
            .addThrValue(21, -25, -17, priority=2).addThrValue(21, 17, 25, priority=2)

        EMThreshold( 'eEM22VHI', 'eEM').addThrValue(24, priority=1)\
            .addThrValue(24, -7, 0, priority=2).addThrValue(24, 0, 7, priority=2)\
            .addThrValue(23, -8, -7, priority=2).addThrValue(23, 7, 8, priority=2)\
            .addThrValue(22, -11, -8, priority=2).addThrValue(22, 8, 11, priority=2)\
            .addThrValue(21, -13, -11, priority=2).addThrValue(21, 11, 13, priority=2)\
            .addThrValue(20, -14, -13, priority=2).addThrValue(20, 13, 14, priority=2)\
            .addThrValue(19, -15, -14, priority=2).addThrValue(19, 14, 15, priority=2)\
            .addThrValue(21, -17, -15, priority=2).addThrValue(21, 15, 17, priority=2)\
            .addThrValue(23, -25, -17, priority=2).addThrValue(23, 17, 25, priority=2)
        


        # TAU
        for et in [12, 20, 40, 60, 100]:
            TauThreshold('eTAU%i' % et, 'eTAU').setEt(et)

        for et in [12,20, 25]:
            TauThreshold('eTAU%iIM' % et, 'eTAU').setEt(et)



        # JET
        for thrV in [12, 15, 20, 25, 30, 40, 50, 85, 100]:
            JetThreshold('jJ%i' % thrV, 'jJ').setPt(thrV).addRange(etamin=-31, etamax=31) # jets are between -31 and 31 -ATR-11526


        # Central jet
        for (thrV, etamax) in [(12,23), (15,25), (25,23), (35,23), (40,25)]:
            JetThreshold('jJ%i.0ETA%i'  % (thrV, etamax), 'jJ').setPt(thrV).addRange(etamin = -etamax,  etamax = etamax)  

        # Standard forward jet
        for thrV in [15, 20, 75]:
            JetThreshold('jJ%i.31ETA49' % thrV, 'jJ').setPt(thrV).addRange(etamin=31, etamax=49).addRange(etamin=-49, etamax=-31)

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
        for x in xrange(16):
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

        ## AFP

        if '_v6' in TriggerFlags.triggerMenuSetup():
            NimThreshold('AFP_NSC', 'NIM', mapping=2)
            NimThreshold('AFP_FSC', 'NIM', mapping=3)
            NimThreshold('AFP_NSA', 'NIM', mapping=4)
            NimThreshold('AFP_FSA', 'NIM', mapping=5)      
        else:
            NimThreshold('AFP_NSC', 'NIM', mapping=2)        
            NimThreshold('AFP_NSA', 'NIM', mapping=3)        
            NimThreshold('AFP_FSA_SIT', 'NIM', mapping=4)        
            NimThreshold('AFP_FSA_TOF', 'NIM', mapping=5)  
            NimThreshold('AFP_FSA_LOG', 'NIM', mapping=6)        
            NimThreshold('AFP_FSC_SIT', 'NIM', mapping=7)        
            NimThreshold('AFP_FSC_LOG', 'NIM', mapping=8)        
            NimThreshold('AFP_FSC_TOF', 'NIM', mapping=9)  


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
            Threshold('ALFA_%s'    % alfa, 'ALFA', mapping = LUT1offset + i/2 + phaseOffset, run = 3 )
            Threshold('ALFA2_%s'   % alfa, 'ALFA', mapping = LUT2offset + i/2 + phaseOffset, run = 3 )
            Threshold('ALFA3_%s'   % alfa, 'ALFA', mapping = LUT3offset + i/2 + phaseOffset, run = 3 )
            Threshold('ALFA4_%s'   % alfa, 'ALFA', mapping = LUT4offset + i/2 + phaseOffset, run = 3 )
            Threshold('ALFA_%s_OD' % alfa, 'ALFA', mapping = LUT5offset + i/2 + phaseOffset, run = 3 )

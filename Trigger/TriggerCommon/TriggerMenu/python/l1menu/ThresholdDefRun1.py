# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerMenu.l1.Lvl1Thresholds import ThresholdValue
from TriggerMenu.l1.Limits import CaloLimits as CL
ClusterOff   = CL.ClusterOff
IsolationOff = CL.IsolationOff
JetOff       = CL.JetOff
EtSumOff     = CL.EtSumOff
EtMissOff    = CL.EtMissOff
JetEtOff     = CL.JetEtOff

from TriggerJobOpts.TriggerFlags import TriggerFlags

class ThresholdDef:

    @staticmethod
    def registerThresholds(tm):
        
        # MUON
        ThresholdValue.setDefaults('MUON', {'etamin' : -49,'etamax' : 49, 'phimin' : 0,'phimax' : 64}) # keep this as an example how defaults for groups can be defined

        for thrV in [0, 4, 6, 10, 11, 15, 20]:
            tm.registerThr('MU%i' % thrV, 'MUON').addThrValue(thrV)

        # EM
        
        for thrV in [3, 4, 5, 6, 7, 10, 12, 14, 16, 30, 50]:
            tm.registerThr('EM%i' % thrV, 'EM').addThrValue(thrV)

        tm.registerThr( 'EM16V',  type='EM').addThrValue(18, priority=1)\
                        .addThrValue(17, -12,  -8, priority=2).addThrValue(17,   8,  12, priority=2)\
                        .addThrValue(16, -16, -12, priority=2).addThrValue(16,  12,  16, priority=2)\
                        .addThrValue(17, -20, -16, priority=2).addThrValue(17,  16,  20, priority=2)

        ThresholdValue.setDefaults('EM', {'had_veto' : 1})
        tm.registerThr( 'EM10VH', type='EM').addThrValue(11, priority=1)\
                        .addThrValue(10, -20, -8, priority=2).addThrValue(10, 8, 20, priority=2)
        tm.registerThr( 'EM14VH', type='EM').addThrValue(15, had_veto=1, priority=1)\
                        .addThrValue(14, -20, -8, priority=2).addThrValue(14, 8, 20, priority=2)
        tm.registerThr( 'EM16VH', type='EM').addThrValue(18, priority=1)\
                        .addThrValue(17, -12, -8, priority=2).addThrValue(17, 8, 12, priority=2)\
                        .addThrValue(16, -16, -12, priority=2).addThrValue(16, 12, 16, priority=2)\
                        .addThrValue(17, -20, -16, priority=2).addThrValue(17, 16, 20, priority=2)
        tm.registerThr( 'EM18VH', type='EM').addThrValue(20, had_veto=1, priority=1)\
                        .addThrValue(19, -12, -8, priority=2).addThrValue(19, 8, 12, priority=2)\
                        .addThrValue(18, -16, -12, priority=2).addThrValue(18, 12, 16, priority=2)\
                        .addThrValue(19, -20, -16, priority=2).addThrValue(19, 16, 20, priority=2)

        ##Add for DC14
        
        ##V section
        tm.registerThr( 'EM7V', type='EM').addThrValue(7)\
                        .addThrValue(7, -20, -8, priority=2).addThrValue(7, 8, 20, priority=2)
        
        tm.registerThr( 'EM15V', type='EM').addThrValue(17, priority=1)\
                        .addThrValue(16, -12, -8, priority=2).addThrValue(16, 8, 12, priority=2)\
                        .addThrValue(15, -16, -12, priority=2).addThrValue(15, 12, 16, priority=2)\
                        .addThrValue(16, -20, -16, priority=2).addThrValue(16, 16, 20, priority=2)

        tm.registerThr( 'EM20V', type='EM').addThrValue(22, priority=1)\
                        .addThrValue(21, -12, -8, priority=2).addThrValue(21, 8, 12, priority=2)\
                        .addThrValue(20, -16, -12, priority=2).addThrValue(20, 12, 16, priority=2)\
                        .addThrValue(21, -20, -16, priority=2).addThrValue(21, 16, 20, priority=2)

        ##VH section
        ThresholdValue.setDefaults('EM', {'had_veto' : 1, 'had_isolation' : 1, 'isolation' : 1})
        
        tm.registerThr( 'EM8VH', type='EM').addThrValue(9, priority=1)\
                        .addThrValue(8, -20, -8, priority=2).addThrValue(8, 8, 20, priority=2)

        tm.registerThr( 'EM15VH', type='EM').addThrValue(17, priority=1)\
                        .addThrValue(16, -12, -8, priority=2).addThrValue(16, 8, 12, priority=2)\
                        .addThrValue(15, -16, -12, priority=2).addThrValue(15, 12, 16, priority=2)\
                        .addThrValue(16, -20, -16, priority=2).addThrValue(16, 16, 20, priority=2)

        # VHI section
        ThresholdValue.setDefaults('EM', {'had_veto' : 1, 'had_isolation' : 1, 'em_isolation' : 2, 'isolation' : 2})

        tm.registerThr( 'EM24VHI', type='EM').addThrValue(26, priority=1)\
                        .addThrValue(25, -12, -8, priority=2).addThrValue(25, 8, 12, priority=2)\
                        .addThrValue(24, -16, -12, priority=2).addThrValue(24, 12, 16, priority=2)\
                        .addThrValue(25, -20, -16, priority=2).addThrValue(25, 16, 20, priority=2)

        ThresholdValue.setDefaults('EM', {})


        # TAU

        for thrV in [3, 6, 8, 11, 12, 15, 20, 40,60]:
            tm.registerThr('HA%i' % thrV, 'TAU').addThrValue(thrV)

        for (thrV, emIso) in [ (6,5), (11,4), (15,4) ]:
            tm.registerThr('HA%iI' % thrV, 'TAU').addThrValue(thrV, em_isolation = emIso)


        # JET

        tm.registerThr('J5',      'JET').addThrValue(5, window=4)
        tm.registerThr('J5_win6', 'JET').addThrValue(5, window=6)

        for thrV in [10, 12,15, 18, 20,25, 30, 40, 50, 75, 100, 350]:
            tm.registerThr('J%i' % thrV, 'JET').addThrValue(thrV)
            
        tm.registerThr('J15CF', 'JET').addThrValue(1023).addThrValue(15, -32, -29, priority=1).addThrValue(15, 27, 32, priority=1) # from Allan
        
        # JB and JF

        ThresholdValue.setDefaults('JB', {'etamin' : -49,'etamax' : 49, 'phimin' : 0,'phimax' : 64, 'window' : 8, 'priority': 1})
        ThresholdValue.setDefaults('JF', {'etamin' : -49,'etamax' : 49, 'phimin' : 0,'phimax' : 64, 'window' : 8, 'priority': 1})

        for thrV in [0, 5, 10, 15, 30, 50, 75, 100]:
            tm.registerThr('JF%i' % thrV, 'JF').addThrValue(JetOff).addThrValue( thrV, etamin=32, priority=2)
            tm.registerThr('JB%i' % thrV, 'JB').addThrValue(JetOff).addThrValue( thrV, etamax=-32, priority=2)


        ##For DC14
            
        # JB and JF (but for DC14 the software can actually apply cuts on eta)
        for (thrV, etamin) in [ (15,23),(20,32), (25,32), (100,32) ]:
            tm.registerThr('JF%i.%iETA49' % (thrV, etamin), 'JF').addThrValue(JetOff).addThrValue( thrV, etamin=etamin, priority=2)
            tm.registerThr('JB%i.%iETA49' % (thrV, etamin), 'JB').addThrValue(JetOff).addThrValue( thrV, etamax=-etamin, priority=2)

        # central trigger
        for (thrV, etamax) in [ (15,24) ]:
            tm.registerThr('J%i.ETA%i' % (thrV, etamax), 'JET').addThrValue(JetOff).addThrValue( thrV, etamin=-etamax,  etamax=etamax, priority=1)

        ##end of DC14

        # ZB
        tm.registerThr('ZB_EM15V' , 'ZB', seed='EM15V' , seed_ttype = 'EM',  seed_multi=1, bcdelay=3564)
        tm.registerThr('ZB_EM12' , 'ZB', seed='EM12' , seed_ttype = 'EM',  seed_multi=1, bcdelay=3564)
        tm.registerThr('ZB_J10'  , 'ZB', seed='J10'  ,  seed_ttype = 'JET', seed_multi=1, bcdelay=3564)


        # JE

        ThresholdValue.setDefaults('JE', {'etamin' : -49,'etamax' : 49, 'phimin' : 0,'phimax' : 64,
                                                       'window' : 8, 'priority': 0})

        for thrV in [100, 140, 200, 300, 350, 500]:
            tm.registerThr('JE%i' % thrV, 'JE').addThrValue(thrV)



        # TE

        ThresholdValue.setDefaults('TE', {'etamin' : -49,'etamax' : 49, 'phimin' : 0,'phimax' : 64, 'priority': 0})

        for thrV in [0, 10, 20, 30, 35, 40, 50, 65, 90, 100]:
            if 'HI_v2' in TriggerFlags.triggerMenuSetup(): # pPb thresholds
                tm.registerThr('TE%i' % thrV, 'TE').addThrValue(0x3fff).addThrValue(  thrV, etamax=-32, priority=1).addThrValue(  thrV, etamin=32, priority=1)
            else:
                tm.registerThr('TE%i' % thrV, 'TE').addThrValue(thrV)

        for thrV in [300, 500, 700, 800, 1000, 1200, 1400]:
            tm.registerThr('TE%i' % thrV, 'TE').addThrValue(thrV)


        # XE

        ThresholdValue.setDefaults('XE', {'etamin' : -49,'etamax' : 49, 'phimin' : 0,'phimax' : 64, 'priority': 0})

        for thrV in [10, 15, 20, 25, 30, 35, 40,45, 50, 60, 70, 80, 90]:
            tm.registerThr('XE%i' % thrV, 'XE').addThrValue(thrV)

        # XS

        for thrV in [25, 30, 35, 40, 45, 50, 55, 60, 65]:
            tm.registerThr('XS%i' % thrV, 'XS').addThrValue(thrV)


        # CALREQ (needs mapping defined)

        for i in range(3):
            tm.registerThr('NIM%i' % (i+28), 'CALREQ', mapping=i).addThrValue(40)



        ## MBTS
        thresholdA=[ 32.04, 26.98, 35.00, 33.54, 32.08, 36.46, 30.63, 32.08, 33.54, 30.63, 29.17, 33.54, 32.08, 32.08, 30.63, 26.25]
        thresholdC=[ 55.42, 31.98, 32.81, 49.48, 98.44, 32.11, 32.62, 29.90, 24.06, 25.81, 25.52, 35, 27.71, 36.46, 26.25, 30.63]

        for i, (vA, vC) in enumerate(zip(thresholdA, thresholdC)):
            tm.registerThr('MBTS_A%i' % i, 'MBTSSI').addThrValue(thresholdA[i])
            tm.registerThr('MBTS_C%i' % i, 'MBTSSI').addThrValue(thresholdC[i])


        thr_mbtsA = tm.registerThr('MBTS_A', 'MBTS', mapping=0)
        thr_mbtsC = tm.registerThr('MBTS_C', 'MBTS', mapping=1)
        for x in xrange(16):
            thr_mbtsA.thresholdValues += [ tm.getRegisteredThreshold('MBTS_A%i' % x).thresholdValues[0] ]
            thr_mbtsC.thresholdValues += [ tm.getRegisteredThreshold('MBTS_C%i' % x).thresholdValues[0] ]



        ## ZDC
        
        tm.registerThr('ZDC_A',   'ZDC').addThrValue(250)
        tm.registerThr('ZDC_C',   'ZDC').addThrValue(250)
        tm.registerThr('ZDC_AND', 'ZDC').addThrValue(250)

        ## BCM

        tm.registerThr('BCM_AtoC', 'BCM').addThrValue(40)
        tm.registerThr('BCM_CtoA', 'BCM').addThrValue(40)
        tm.registerThr('BCM_Wide', 'BCM').addThrValue(40)
        tm.registerThr('BCM_Comb', 'BCMCMB').addThrValue(40)


        tm.registerThr('LUCID_A', 'LUCID').addThrValue(650)
        tm.registerThr('LUCID_C', 'LUCID').addThrValue(650)


        ## BPTX

        tm.registerThr('BPTX0', 'NIM', mapping=0 ).addThrValue(40)
        tm.registerThr('BPTX1', 'NIM', mapping=1 ).addThrValue(40)


        ## Other NIMs

        for i in [5,6,7,8,14,25,36]:
            tm.registerThr('NIMDIR%i' % i, 'NIM', mapping=i).addThrValue(40)
        tm.registerThr('NIMDIR26', 'NIM', mapping=35).addThrValue(40)


        # ALFA NIMS
        
        for i, alfa in enumerate( ['B7R1L', 'B7R1U', 'A7R1L', 'A7R1U', 'A7L1L', 'A7L1U', 'B7L1L', 'B7L1U', 'ALFA_LOG'] ):
            tm.registerThr('NIM_%s' % alfa,    'NIM', mapping=14+i).addThrValue(40) 
            tm.registerThr('NIM_%s_OD' % alfa, 'NIM', mapping=26+i).addThrValue(40) 

        # Muon NIMS for Barrel and Endcap
        
        tm.registerThr('NIM_MUB', 'NIM', mapping=23).addThrValue(40)
        tm.registerThr('NIM_MUE', 'NIM', mapping=24).addThrValue(40)


        # DIRECT INPUTS
        
        tm.registerThr('TOPO_DPHI', 'TOPO', mapping=0)
        tm.registerThr('TOPO_HT',   'TOPO', mapping=1)
        tm.registerThr('TOPO_JPSI', 'TOPO', mapping=31)
        tm.registerThr('TOPO_0',    'TOPO', mapping=32)
        tm.registerThr('TOPO_1',    'TOPO', mapping=63)
        tm.registerThr('TOPO_2',    'TOPO', mapping=64)
        tm.registerThr('TOPO_3',    'TOPO', mapping=91)
        tm.registerThr('TOPO_4',    'TOPO', mapping=127)
        tm.registerThr('TOPO_5',    'TOPO', mapping=128)
        tm.registerThr('TOPO_6',    'TOPO', mapping=191)
        

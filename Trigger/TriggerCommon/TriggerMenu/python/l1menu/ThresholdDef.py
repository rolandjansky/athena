# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerMenu.l1.Lvl1Thresholds import LVL1Threshold, ThresholdValue
from TriggerMenu.l1.Limits import CaloLimits as CL
from TriggerJobOpts.TriggerFlags import TriggerFlags

ClusterOff   = CL.ClusterOff
IsolationOff = CL.IsolationOff
JetOff       = CL.JetOff
EtSumOff     = CL.EtSumOff
EtMissOff    = CL.EtMissOff
JetEtOff     = CL.JetEtOff


class ThresholdDef:

    @staticmethod
    def registerThresholds(tc):
        
        # MUON
        ThresholdValue.setDefaults('MUON', {'etamin' : -49,'etamax' : 49, 'phimin' : 0,'phimax' : 64}) # keep this as an example how defaults for groups can be defined

        for thrV in [0, 4, 6, 10, 11, 15, 20]:
            tc.registerThr('MU%i' % thrV, 'MUON').addThrValue(thrV)



        # EM all have hadronic veto
        ThresholdValue.setDefaults('EM', {'had_veto' : 1})
        
        for thrV in [3, 4, 5, 6, 7, 8, 10, 12, 14, 15, 16, 30, 50]:
            tc.registerThr('EM%i' % thrV, 'EM').addThrValue(thrV)


        # variable thresholds (V)
        # hadronic isolation (H) had_isolation=1GeV
        # em isolation (I)

        # V section
        tc.registerThr( 'EM7V', type='EM').addThrValue(7)\
                        .addThrValue(7, -20, -8, priority=2).addThrValue(7, 8, 20, priority=2)
        
        tc.registerThr( 'EM15V', type='EM').addThrValue(17, priority=1)\
                        .addThrValue(16, -12, -8, priority=2).addThrValue(16, 8, 12, priority=2)\
                        .addThrValue(15, -16, -12, priority=2).addThrValue(15, 12, 16, priority=2)\
                        .addThrValue(16, -20, -16, priority=2).addThrValue(16, 16, 20, priority=2)

        tc.registerThr( 'EM20V', type='EM').addThrValue(22, priority=1)\
                        .addThrValue(21, -12, -8, priority=2).addThrValue(21, 8, 12, priority=2)\
                        .addThrValue(20, -16, -12, priority=2).addThrValue(20, 12, 16, priority=2)\
                        .addThrValue(21, -20, -16, priority=2).addThrValue(21, 16, 20, priority=2)


        # VH section
        ThresholdValue.setDefaults('EM', {'had_veto' : 1, 'had_isolation' : 1, 'isolation' : 1})

        tc.registerThr( 'EM7VH', type='EM').addThrValue(8, priority=1)\
                        .addThrValue(7, -20, -8, priority=2).addThrValue(7, 8, 20, priority=2)

        tc.registerThr( 'EM8VH', type='EM').addThrValue(9, priority=1)\
                        .addThrValue(8, -20, -8, priority=2).addThrValue(8, 8, 20, priority=2)

        tc.registerThr( 'EM15VH', type='EM').addThrValue(17, priority=1)\
                        .addThrValue(16, -12, -8, priority=2).addThrValue(16, 8, 12, priority=2)\
                        .addThrValue(15, -16, -12, priority=2).addThrValue(15, 12, 16, priority=2)\
                        .addThrValue(16, -20, -16, priority=2).addThrValue(16, 16, 20, priority=2)

        tc.registerThr( 'EM18VH', type='EM').addThrValue(20, priority=1)\
                        .addThrValue(19, -12, -8, priority=2).addThrValue(19, 8, 12, priority=2)\
                        .addThrValue(18, -16, -12, priority=2).addThrValue(18, 12, 16, priority=2)\
                        .addThrValue(19, -20, -16, priority=2).addThrValue(19, 16, 20, priority=2)

        tc.registerThr( 'EM20VH', type='EM').addThrValue(22, priority=1)\
                        .addThrValue(21, -12, -8, priority=2).addThrValue(21, 8, 12, priority=2)\
                        .addThrValue(20, -16, -12, priority=2).addThrValue(20, 12, 16, priority=2)\
                        .addThrValue(21, -20, -16, priority=2).addThrValue(21, 16, 20, priority=2)

        tc.registerThr( 'EM24VH', type='EM').addThrValue(26, priority=1)\
                        .addThrValue(25, -12, -8, priority=2).addThrValue(25, 8, 12, priority=2)\
                        .addThrValue(24, -16, -12, priority=2).addThrValue(24, 12, 16, priority=2)\
                        .addThrValue(25, -20, -16, priority=2).addThrValue(25, 16, 20, priority=2)

        tc.registerThr( 'EM26VH', type='EM').addThrValue(28, priority=1)\
                        .addThrValue(27, -12, -8, priority=2).addThrValue(27, 8, 12, priority=2)\
                        .addThrValue(26, -16, -12, priority=2).addThrValue(26, 12, 16, priority=2)\
                        .addThrValue(27, -20, -16, priority=2).addThrValue(27, 16, 20, priority=2)


        # VHI section
        ThresholdValue.setDefaults('EM', {'had_veto' : 1, 'had_isolation' : 1, 'em_isolation' : 2, 'isolation' : 2})

        tc.registerThr( 'EM15VHI', type='EM').addThrValue(17, priority=1)\
                        .addThrValue(16, -12, -8, priority=2).addThrValue(16, 8, 12, priority=2)\
                        .addThrValue(15, -16, -12, priority=2).addThrValue(15, 12, 16, priority=2)\
                        .addThrValue(16, -20, -16, priority=2).addThrValue(16, 16, 20, priority=2)

        tc.registerThr( 'EM18VHI', type='EM').addThrValue(20, priority=1)\
                        .addThrValue(19, -12, -8, priority=2).addThrValue(19, 8, 12, priority=2)\
                        .addThrValue(18, -16, -12, priority=2).addThrValue(18, 12, 16, priority=2)\
                        .addThrValue(19, -20, -16, priority=2).addThrValue(19, 16, 20, priority=2)

        tc.registerThr( 'EM20VHI', type='EM').addThrValue(22, priority=1)\
                        .addThrValue(21, -12, -8, priority=2).addThrValue(21, 8, 12, priority=2)\
                        .addThrValue(20, -16, -12, priority=2).addThrValue(20, 12, 16, priority=2)\
                        .addThrValue(21, -20, -16, priority=2).addThrValue(21, 16, 20, priority=2)

        tc.registerThr( 'EM22VHI', type='EM').addThrValue(24, priority=1)\
                        .addThrValue(23, -12, -8, priority=2).addThrValue(23, 8, 12, priority=2)\
                        .addThrValue(22, -16, -12, priority=2).addThrValue(22, 12, 16, priority=2)\
                        .addThrValue(23, -20, -16, priority=2).addThrValue(23, 16, 20, priority=2)

        tc.registerThr( 'EM24VHI', type='EM').addThrValue(26, priority=1)\
                        .addThrValue(25, -12, -8, priority=2).addThrValue(25, 8, 12, priority=2)\
                        .addThrValue(24, -16, -12, priority=2).addThrValue(24, 12, 16, priority=2)\
                        .addThrValue(25, -20, -16, priority=2).addThrValue(25, 16, 20, priority=2)

        tc.registerThr( 'EM26VHI', type='EM').addThrValue(28, priority=1)\
                        .addThrValue(27, -12, -8, priority=2).addThrValue(27, 8, 12, priority=2)\
                        .addThrValue(26, -16, -12, priority=2).addThrValue(26, 12, 16, priority=2)\
                        .addThrValue(27, -20, -16, priority=2).addThrValue(27, 16, 20, priority=2)


        # new isolation
        ThresholdValue.setDefaults('EM', {'em_isobits' : '00010', 'had_isobits' : '00001' })

        tc.registerThr( 'EM18VHIrel', type='EM').addThrValue(20, priority=1)\
                        .addThrValue(19, -12, -8, priority=2).addThrValue(19, 8, 12, priority=2)\
                        .addThrValue(18, -16, -12, priority=2).addThrValue(18, 12, 16, priority=2)\
                        .addThrValue(19, -20, -16, priority=2).addThrValue(19, 16, 20, priority=2)


        ThresholdValue.setDefaults('EM', {})


        # TAU

        for thrV in [6, 8, 12, 15, 20, 30, 40, 60]:
            tc.registerThr('HA%i' % thrV, 'TAU').addThrValue(thrV)

        for (thrV, emIso) in [ (12,4), (20,4) ]: # isolation thresholds
            tc.registerThr('HA%iI' % thrV, 'TAU').addThrValue(thrV, em_isolation = emIso)

        for (thrV, hadv) in [ (60,1) ]: # veto thresholds
            tc.registerThr('HA%iV' % thrV, 'TAU').addThrValue(thrV, had_veto = emIso)


        # JET

        ThresholdValue.setDefaults('JET', {'window' : 8})

        for thrV in [10, 12, 15, 20, 25, 30, 40, 50, 70, 75, 100, 120, 175, 250, 400]:
            tc.registerThr('J%i' % thrV, 'JET').addThrValue(JetOff).addThrValue(thrV, etamin=-32, etamax=32, priority=1) # jets are between -32 and 32


        ThresholdValue.setDefaults('JET', {})


        
        # JB and JF

        ThresholdValue.setDefaults('JET', {'window' : 8})

        for thrV in [25]:
            tc.registerThr('JF%i' % thrV, 'JET').addThrValue(JetOff).addThrValue( thrV, etamin=32,  etamax=49, priority=1)
            tc.registerThr('JB%i' % thrV, 'JET').addThrValue(JetOff).addThrValue( thrV, etamin=-49, etamax=-32, priority=1)

        for thrV in [10, 15, 20, 30, 50, 75, 100]:
            tc.registerThr('FJ%i' % thrV, 'JET').addThrValue(JetOff).addThrValue( thrV, etamin=32,  etamax=49, priority=1).addThrValue( thrV, etamin=-49, etamax=-32, priority=1)


        # Central jet
        for (thrV, etamax) in [ (17,22), (25,22), (15,24) ]:
            tc.registerThr('J%i.ETA%i' % (thrV, etamax), 'JET').addThrValue(JetOff).addThrValue( thrV, etamin=-etamax,  etamax=etamax, priority=1)

        # Forward jet
        for (thrV, etamin) in [ (15,23) ]:
            tc.registerThr('FJ%i.%iETA' % (thrV, etamin), 'JET').addThrValue(JetOff).addThrValue( thrV, etamin=etamin, priority=1).addThrValue( thrV, etamax=-etamin, priority=1)


        ThresholdValue.setDefaults('JET', {})


        # ZB
        tc.registerThr('ZB_EM12', 'ZB', seed='EM12',seed_ttype = 'EM',  seed_multi=1, bcdelay=3564)
        tc.registerThr('ZB_J10',  'ZB', seed='J10', seed_ttype = 'JET', seed_multi=1, bcdelay=3564)
        tc.registerThr('ZB_J75',  'ZB', seed='J75', seed_ttype = 'JET', seed_multi=1, bcdelay=3564)

        # JE

        ThresholdValue.setDefaults('JE', {'etamin' : -49,'etamax' : 49, 'phimin' : 0,'phimax' : 64,
                                                       'window' : 8, 'priority': 0})

        for thrV in [100, 140, 200, 300, 350, 500]:
            tc.registerThr('JE%i' % thrV, 'JE').addThrValue(thrV)



        # TE
        for thrV in [5, 10, 20, 30, 40, 50, 2000, 4000]:
            tc.registerThr('TE%i' % thrV, 'TE').addThrValue(thrV)


        # XE
        for thrV in [35, 40, 45, 50, 60, 70, 80, 90]:
            tc.registerThr('XE%i' % thrV, 'XE').addThrValue(thrV)

        # XS
        for thrV in [25, 30, 35, 40, 45, 50, 55, 60, 65]:
            tc.registerThr('XS%i' % thrV, 'XS').addThrValue(thrV)


        # CALREQ (needs mapping defined)

        for i in range(3):
            tc.registerThr('NIM%i' % (i+28), 'CALREQ', mapping=i).addThrValue(40)



        ## MBTS

        # MBTS naming scheme defined in
        # https://docs.google.com/spreadsheets/d/1R0s8Lw-0nPSjqe9YTuZBCeAdedn_Ve4Ax6bbMe_4bSk/edit#gid=1818891632

        # run 1
        thresholdA=[ 32.04, 26.98, 35.00, 33.54, 32.08, 36.46, 30.63, 32.08, 33.54, 30.63, 29.17, 33.54, 32.08, 32.08, 30.63, 26.25]
        thresholdC=[ 55.42, 31.98, 32.81, 49.48, 98.44, 32.11, 32.62, 29.90, 24.06, 25.81, 25.52, 35.00, 27.71, 36.46, 26.25, 30.63]

        # run 2 above MBTS_A08 only the even numbers are used
        thresholdA=[ 32.04, 26.98, 35.00, 33.54, 32.08, 36.46, 30.63, 32.08, 33.54, 0, 29.17, 0, 32.08, 0, 30.63, 0]
        thresholdC=[ 55.42, 31.98, 32.81, 49.48, 98.44, 32.11, 32.62, 29.90, 24.06, 0, 25.52, 0, 27.71, 0, 26.25, 0]

        for i, (vA, vC) in enumerate(zip(thresholdA, thresholdC)):
            if(thresholdA[i]!=0):
                tc.registerThr('MBTS_A%i' % i, 'MBTSSI').addThrValue(thresholdA[i])
            if(thresholdC[i]!=0):
                tc.registerThr('MBTS_C%i' % i, 'MBTSSI').addThrValue(thresholdC[i])


        thr_mbtsA = tc.registerThr('MBTS_A', 'MBTS', mapping=0)
        thr_mbtsC = tc.registerThr('MBTS_C', 'MBTS', mapping=1)
        for x in xrange(16):
            if tc.getRegisteredThreshold('MBTS_A%i' % x):
                thr_mbtsA.thresholdValues += [ tc.getRegisteredThreshold('MBTS_A%i' % x).thresholdValues[0] ]
            if tc.getRegisteredThreshold('MBTS_C%i' % x):
                thr_mbtsC.thresholdValues += [ tc.getRegisteredThreshold('MBTS_C%i' % x).thresholdValues[0] ]



        ## ZDC
        
        tc.registerThr('ZDC_A',   'ZDC').addThrValue(250)
        tc.registerThr('ZDC_C',   'ZDC').addThrValue(250)
        tc.registerThr('ZDC_AND', 'ZDC').addThrValue(250)

        ## BCM

        tc.registerThr('BCM_AtoC', 'BCM').addThrValue(40)
        tc.registerThr('BCM_CtoA', 'BCM').addThrValue(40)
        tc.registerThr('BCM_Wide', 'BCM').addThrValue(40)
        tc.registerThr('BCM_Comb', 'BCMCMB').addThrValue(40)


        tc.registerThr('LUCID_A', 'LUCID').addThrValue(650)
        tc.registerThr('LUCID_C', 'LUCID').addThrValue(650)


        ## BPTX

        tc.registerThr('BPTX0', 'NIM', mapping=0 ).addThrValue(40)
        tc.registerThr('BPTX1', 'NIM', mapping=1 ).addThrValue(40)


        ## Other NIMs

        for i in [5,6,7,8,14,25,36]:
            tc.registerThr('NIMDIR%i' % i, 'NIM', mapping=i).addThrValue(40)
        tc.registerThr('NIMDIR26', 'NIM', mapping=35).addThrValue(40)


        # ALFA NIMS
        
        for i, alfa in enumerate( ['B7R1L', 'B7R1U', 'A7R1L', 'A7R1U', 'A7L1L', 'A7L1U', 'B7L1L', 'B7L1U', 'ALFA_LOG'] ):
            tc.registerThr('NIM_%s' % alfa,    'NIM', mapping=14+i).addThrValue(40) 
            tc.registerThr('NIM_%s_OD' % alfa, 'NIM', mapping=26+i).addThrValue(40) 

        # Muon NIMS for Barrel and Endcap
        
        tc.registerThr('NIM_MUB', 'NIM', mapping=23).addThrValue(40)
        tc.registerThr('NIM_MUE', 'NIM', mapping=24).addThrValue(40)


        # DIRECT INPUTS
        # all topo inputs are directly set from the L1Topo menu
        
        #tc.registerTopoThr('TOPO_JETDPHI', 'TOPO', mapping=0)
        #tc.registerThr('TOPO_HT',   'TOPO', mapping=1)
        #tc.registerThr('TOPO_JPSI', 'TOPO', mapping=31)
        #tc.registerThr('TOPO_0',    'TOPO', mapping=32)
        #tc.registerThr('TOPO_1',    'TOPO', mapping=63)
        #tc.registerThr('TOPO_2',    'TOPO', mapping=64)
        #tc.registerThr('TOPO_3',    'TOPO', mapping=91)
        #tc.registerThr('TOPO_4',    'TOPO', mapping=127)
        #tc.registerThr('TOPO_5',    'TOPO', mapping=128)
        #tc.registerThr('TOPO_6',    'TOPO', mapping=191)
        

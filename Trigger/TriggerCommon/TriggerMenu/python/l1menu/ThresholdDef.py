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


        # EM 
        ThresholdValue.setDefaults('EM',{'isobits' : '00000', 'use_relIso' : True })

        for thrV in [3, 4, 5, 6, 7, 8, 10, 12, 14, 15, 16, 30, 50]:
            tc.registerThr('EM%i' % thrV, 'EM').addThrValue(thrV)


        # variable thresholds (V)
        # hadronic isolation (H) had_isolation=1GeV
        # em isolation (I)

        # V section
        
        tc.registerThr( 'EM50V', type='EM').addThrValue(52, priority=1)\
                        .addThrValue(51, -12, -8, priority=2).addThrValue(51, 8, 12, priority=2)\
                        .addThrValue(50, -16, -12, priority=2).addThrValue(50, 12, 16, priority=2)\
                        .addThrValue(51, -20, -16, priority=2).addThrValue(51, 16, 20, priority=2)

        # VH section

        ThresholdValue.setDefaults('EM', {'isobits' : '00001', 'use_relIso' : True })

        tc.registerThr( 'EM8VH', type='EM').addThrValue(9, priority=1)\
                        .addThrValue(8, -20, -8, priority=2).addThrValue(8, 8, 20, priority=2)

        tc.registerThr( 'EM10VH', type='EM').addThrValue(11, priority=1)\
                        .addThrValue(10, -20, -8, priority=2).addThrValue(10, 8, 20, priority=2)

        tc.registerThr( 'EM13VH', type='EM').addThrValue(14, priority=1)\
                        .addThrValue(13, -20, -8, priority=2).addThrValue(13, 8, 20, priority=2)

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


        # I section

        ThresholdValue.setDefaults('EM', {'isobits' : '00010', 'use_relIso' : True })

        for thrV in [8, 15]:
            tc.registerThr('EM%iI' % thrV, 'EM').addThrValue(thrV)



        # (V)HI section
        ThresholdValue.setDefaults('EM', {'isobits' : '00100', 'use_relIso' : True })

        for thrV in [15]:
            tc.registerThr('EM%iHI' % thrV, 'EM').addThrValue(thrV)


        tc.registerThr( 'EM20VHI', type='EM').addThrValue(22, priority=1)\
                        .addThrValue(21, -12, -8, priority=2).addThrValue(21, 8, 12, priority=2)\
                        .addThrValue(20, -16, -12, priority=2).addThrValue(20, 12, 16, priority=2)\
                        .addThrValue(21, -20, -16, priority=2).addThrValue(21, 16, 20, priority=2)



        # (V)HI section

        ThresholdValue.setDefaults('EM', {'isobits' : '01000', 'use_relIso' : True })

        tc.registerThr( 'EM22VHLIL', type='EM').addThrValue(24, priority=1)\
                        .addThrValue(23, -12, -8, priority=2).addThrValue(23, 8, 12, priority=2)\
                        .addThrValue(22, -16, -12, priority=2).addThrValue(22, 12, 16, priority=2)\
                        .addThrValue(23, -20, -16, priority=2).addThrValue(23, 16, 20, priority=2)

        tc.registerThr( 'EM3HLIL', type='EM').addThrValue(3, priority=1)\



        ThresholdValue.setDefaults('EM', {})


        # TAU

        ThresholdValue.setDefaults('TAU',{'isobits' : '00000', 'use_relIso' : True })

        for thrV in [6, 8, 12, 15, 20, 25, 30, 40, 60]:
            tc.registerThr('HA%i' % thrV, 'TAU').addThrValue(thrV)

#        ThresholdValue.setDefaults('TAU', {'isobits' : '00100', 'use_relIso' : True })
#        for thrV in [12,20]:
#            tc.registerThr('HA%iI' % thrV, 'TAU').addThrValue(thrV)
#
#        ThresholdValue.setDefaults('TAU', {'isobits' : '00001', 'use_relIso' : True })
#        for thrV in [12]:
#            tc.registerThr('HA%iIL' % thrV, 'TAU').addThrValue(thrV)

        ThresholdValue.setDefaults('TAU', {'isobits' : '00001', 'use_relIso' : True })
        for thrV in [12,20]:
            tc.registerThr('HA%iIL' % thrV, 'TAU').addThrValue(thrV)

        ThresholdValue.setDefaults('TAU', {'isobits' : '00010', 'use_relIso' : True })
        for thrV in [10,12,20]:
            tc.registerThr('HA%iIM' % thrV, 'TAU').addThrValue(thrV)

        ThresholdValue.setDefaults('TAU', {'isobits' : '00100', 'use_relIso' : True })
        for thrV in [12,20,25]:
            tc.registerThr('HA%iIT' % thrV, 'TAU').addThrValue(thrV)

        ThresholdValue.setDefaults('TAU', {'isobits' : '01000', 'use_relIso' : True })
        for thrV in [12,20]:
            tc.registerThr('HA%iI' % thrV, 'TAU').addThrValue(thrV)

        ThresholdValue.setDefaults('TAU', {})
            
            
        # JET

        ThresholdValue.setDefaults('JET', {'window' : 8})

        for thrV in [5,10, 12, 15, 20, 25, 30, 35, 40, 50, 60, 70, 75, 85, 100, 120, 150,175, 250, 400]:
            tc.registerThr('J%i' % thrV, 'JET').addThrValue(JetOff).addThrValue(thrV, etamin=-32, etamax=32, priority=1) # jets are between -32 and 32




        ThresholdValue.setDefaults('JET', {})
        
        # JB and JF

        ThresholdValue.setDefaults('JET', {'window' : 8})
        
        for thrV in [25]:
            tc.registerThr('JF%i' % thrV, 'JET').addThrValue(JetOff).addThrValue( thrV, etamin=32,  etamax=49, priority=1)
            tc.registerThr('JB%i' % thrV, 'JET').addThrValue(JetOff).addThrValue( thrV, etamin=-49, etamax=-32, priority=1)


        # Central jet
        for (thrV, etamax) in [(15,24), (17,22), (20,28), (25,23), ]:
            tc.registerThr('J%i.0ETA%i'  % (thrV, etamax), 'JET').addThrValue(JetOff).addThrValue( thrV, etamin = -etamax,  etamax = etamax, priority=1)  

        # Standard forward jet
        for thrV in [10, 15, 20, 25, 30, 35, 45, 50, 70, 75, 100]:
            tc.registerThr('J%i.32ETA49' % thrV, 'JET').addThrValue(JetOff).addThrValue( thrV, etamin=32,  etamax=49, priority=1).addThrValue( thrV, etamin=-49, etamax=-32, priority=1)

        # Custom Forward jet and VBF jets
        for (thrV, etamin, etamax) in [ (20,0,49), (30,0,49), (15,23,49), (15,24,49), (15,29,32), (20,29,32)   ]:
            tc.registerThr('J%i.%iETA%i' % (thrV, etamin,etamax), 'JET').addThrValue(JetOff).addThrValue( thrV, etamin  =  etamin, etamax=etamax, priority=1).addThrValue( thrV, etamin = -etamax, etamax = -etamin, priority=1)

        ##4x4 jet windows for forward electrons
        ThresholdValue.setDefaults('JET', {'window' : 4})
        
        for thrV in [15]:
            tc.registerThr('JJ%i.23ETA49' % thrV, 'JET').addThrValue(JetOff).addThrValue( thrV, etamin=23,  etamax=49, priority=1).addThrValue( thrV, etamin=-49, etamax=-23, priority=1)




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
        for thrV in [5, 10, 20, 30, 40, 50, 60, 2000, 4000]:
            tc.registerThr('TE%i' % thrV, 'TE').addThrValue(thrV)

        for (thrV, etamax) in [(30,24),]:
            tc.registerThr('TE%i.0ETA%i' % (thrV, etamax), 'TE').addThrValue(thrV).addThrValue( thrV, etamin = -etamax, etamax = etamax, priority=1)

        # XE
        for thrV in [35, 40, 45, 50, 55, 60, 70, 80]:
            tc.registerThr('XE%i' % thrV, 'XE').addThrValue(thrV)

        # XS
        for thrV in [25, 30, 35, 40, 45, 50, 55, 60, 65]:
            tc.registerThr('XS%i' % thrV, 'XS').addThrValue(thrV)


        # CALREQ

        for i in range(3):
            tc.registerThr('CAL%i' % i, 'CALREQ', mapping=i).addThrValue(40)


        ## MBTS

        # MBTS naming scheme defined in
        # https://docs.google.com/spreadsheets/d/1R0s8Lw-0nPSjqe9YTuZBCeAdedn_Ve4Ax6bbMe_4bSk/edit#gid=1818891632

        # run 1
        thresholdA=[ 32.04, 26.98, 35.00, 33.54, 32.08, 36.46, 30.63, 32.08, 33.54, 30.63, 29.17, 33.54, 32.08, 32.08, 30.63, 26.25]
        thresholdC=[ 55.42, 31.98, 32.81, 49.48, 98.44, 32.11, 32.62, 29.90, 24.06, 25.81, 25.52, 35.00, 27.71, 36.46, 26.25, 30.63]

        # run 2 above MBTS_A08 only the even numbers are used
        thresholdA=[ 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 0.0, 100.0, 0.0, 100.0, 0.0, 100.0, 0.0]
        thresholdC=[ 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 0.0, 100.0, 0.0, 100.0, 0.0, 100.0, 0.0]

        for i, (vA, vC) in enumerate(zip(thresholdA, thresholdC)):
            #if(thresholdA[i]!=0):
            tc.registerThr('MBTS_A%i' % i, 'MBTSSI').addThrValue(thresholdA[i])
            #if(thresholdC[i]!=0):
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

        tc.registerThr('NIML1A', 'NIM', mapping=0).addThrValue(40)
        tc.registerThr('NIMLHCF', 'NIM', mapping=1).addThrValue(40)
        tc.registerThr('NIMTGC', 'NIM', mapping=12).addThrValue(40)
        tc.registerThr('NIMRPC', 'NIM', mapping=13).addThrValue(40)
        tc.registerThr('NIMTRT', 'NIM', mapping=14).addThrValue(40)

        #for m,i in enumerate([5,6,7,8,14,25,36]):
        #for m,i in enumerate([5,25]):
        #    tc.registerThr('NIMDIR%i' % i, 'NIM', mapping=m+2).addThrValue(40)
        #tc.registerThr('NIMDIR26', 'NIM', mapping=35).addThrValue(40)


        # ALFA NIMS will not exist in run 2        
        #for i, alfa in enumerate( ['B7R1L', 'B7R1U', 'A7R1L', 'A7R1U', 'A7L1L', 'A7L1U', 'B7L1L', 'B7L1U', 'ALFA_LOG'] ):
        #    tc.registerThr('ALFA_%s'    % alfa, 'ALFA', mapping=i).addThrValue(40) 
        #    tc.registerThr('ALFA_%s_OD' % alfa, 'ALFA', mapping=33+i).addThrValue(40) 

        # Muon NIMS for Barrel and Endcap
        
        #tc.registerThr('NIM_MUB', 'NIM', mapping=23).addThrValue(40)
        #tc.registerThr('NIM_MUE', 'NIM', mapping=24).addThrValue(40)


        # DIRECT INPUTS
        # all topo inputs are directly set from the L1Topo menu
        

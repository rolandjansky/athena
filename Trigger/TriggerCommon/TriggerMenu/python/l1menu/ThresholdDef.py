# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerMenu.l1.Lvl1Thresholds import ThresholdValue
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

        for thrV in [0, 4, 6, 10, 11, 15, 20, 21]:
            tc.registerThr('MU%i' % thrV, 'MUON').addThrValue(thrV)


        # EM 
        ThresholdValue.setDefaults('EM',{'isobits' : '00000', 'use_relIso' : True })

        for thrV in [3, 4, 5, 6, 7, 8, 10, 12, 14, 15, 16, 18, 20, 22, 30, 50]:
            tc.registerThr('EM%i' % thrV, 'EM').addThrValue(thrV)


        # for beam splashes:
        #ThresholdValue.setDefaults('EM', {'etamin' : 16,'etamax' : 20, 'phimin' : 0,'phimax' : 64, 'isobits' : '00000', 'use_relIso' : True })
        tc.registerThr('EM20A', 'EM').addThrValue(255, priority=1)\
            .addThrValue(20,16,20,priority=2)
        
        #ThresholdValue.setDefaults('EM', {'etamin' : -20,'etamax' : -16, 'phimin' : 0,'phimax' : 64, 'isobits' : '00000', 'use_relIso' : True })
        tc.registerThr('EM20C', 'EM').addThrValue(255, priority=1)\
            .addThrValue(20,-20,-16,priority=2)



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
            .addThrValue(9, -8, 0, priority=2).addThrValue(9, 0, 8, priority=2)\
            .addThrValue(7, -11, -8, priority=2).addThrValue(7, 8, 11, priority=2)\
            .addThrValue(6, -14, -11, priority=2).addThrValue(6, 11, 14, priority=2)\
            .addThrValue(5, -15, -14, priority=2).addThrValue(5, 14, 15, priority=2)\
            .addThrValue(7, -18, -15, priority=2).addThrValue(7, 15, 18, priority=2)\
            .addThrValue(8, -25, -18, priority=2).addThrValue(8, 18, 25, priority=2)
        
        tc.registerThr( 'EM10VH', type='EM').addThrValue(11, priority=1)\
            .addThrValue(11, -8, 0, priority=2).addThrValue(11, 0, 8, priority=2)\
            .addThrValue(9, -11, -8, priority=2).addThrValue(9, 8, 11, priority=2)\
            .addThrValue(8, -14, -11, priority=2).addThrValue(8, 11, 14, priority=2)\
            .addThrValue(7, -15, -14, priority=2).addThrValue(7, 14, 15, priority=2)\
            .addThrValue(9, -18, -15, priority=2).addThrValue(9, 15, 18, priority=2)\
            .addThrValue(10, -25, -18, priority=2).addThrValue(10, 18, 25, priority=2)
        
        tc.registerThr( 'EM13VH', type='EM').addThrValue(15, priority=1)\
            .addThrValue(15, -7, 0, priority=2).addThrValue(15, 0, 7, priority=2)\
            .addThrValue(14, -9, -7, priority=2).addThrValue(14, 7, 9, priority=2)\
            .addThrValue(13, -12, -9, priority=2).addThrValue(13, 9, 12, priority=2)\
            .addThrValue(12, -14, -12, priority=2).addThrValue(12, 12, 14, priority=2)\
            .addThrValue(11, -15, -14, priority=2).addThrValue(11, 14, 15, priority=2)\
            .addThrValue(13, -17, -15, priority=2).addThrValue(13, 15, 17, priority=2)\
            .addThrValue(14, -25, -17, priority=2).addThrValue(14, 17, 25, priority=2)
        
        tc.registerThr( 'EM15VH', type='EM').addThrValue(17, priority=1)\
            .addThrValue(17, -7, 0, priority=2).addThrValue(17, 0, 7, priority=2)\
            .addThrValue(16, -9, -7, priority=2).addThrValue(16, 7, 9, priority=2)\
            .addThrValue(15, -12, -9, priority=2).addThrValue(15, 9, 12, priority=2)\
            .addThrValue(14, -14, -12, priority=2).addThrValue(14, 12, 14, priority=2)\
            .addThrValue(13, -15, -14, priority=2).addThrValue(13, 14, 15, priority=2)\
            .addThrValue(15, -17, -15, priority=2).addThrValue(15, 15, 17, priority=2)\
            .addThrValue(16, -25, -17, priority=2).addThrValue(16, 17, 25, priority=2)  
      
        tc.registerThr( 'EM18VH', type='EM').addThrValue(20, priority=1)\
            .addThrValue(20, -7, 0, priority=2).addThrValue(20, 0, 7, priority=2)\
            .addThrValue(19, -8, -7, priority=2).addThrValue(19, 7, 8, priority=2)\
            .addThrValue(18, -11, -8, priority=2).addThrValue(18, 8, 11, priority=2)\
            .addThrValue(17, -13, -11, priority=2).addThrValue(17, 11, 13, priority=2)\
            .addThrValue(16, -14, -13, priority=2).addThrValue(16, 13, 14, priority=2)\
            .addThrValue(15, -15, -14, priority=2).addThrValue(15, 14, 15, priority=2)\
            .addThrValue(17, -17, -15, priority=2).addThrValue(17, 15, 17, priority=2)\
            .addThrValue(19, -25, -17, priority=2).addThrValue(19, 17, 25, priority=2)
        
        tc.registerThr( 'EM20VH', type='EM').addThrValue(22, priority=1)\
            .addThrValue(22, -7, 0, priority=2).addThrValue(22, 0, 7, priority=2)\
            .addThrValue(21, -8, -7, priority=2).addThrValue(21, 7, 8, priority=2)\
            .addThrValue(20, -11, -8, priority=2).addThrValue(20, 8, 11, priority=2)\
            .addThrValue(19, -13, -11, priority=2).addThrValue(19, 11, 13, priority=2)\
            .addThrValue(18, -14, -13, priority=2).addThrValue(18, 13, 14, priority=2)\
            .addThrValue(17, -15, -14, priority=2).addThrValue(17, 14, 15, priority=2)\
            .addThrValue(19, -17, -15, priority=2).addThrValue(19, 15, 17, priority=2)\
            .addThrValue(21, -25, -17, priority=2).addThrValue(21, 17, 25, priority=2)       

        # I section

        ThresholdValue.setDefaults('EM', {'isobits' : '00010', 'use_relIso' : True })

        for thrV in [8]:
            tc.registerThr('EM%iI' % thrV, 'EM').addThrValue(thrV)



        # (V)HI section

        # new egamma v7 menu (ATR-16089)
        if not '_v6' in TriggerFlags.triggerMenuSetup() and not '_HI' in TriggerFlags.triggerMenuSetup():

           ThresholdValue.setDefaults('EM', {'isobits' : '00001', 'use_relIso' : True })
           tc.registerThr( 'EM22VH', type='EM').addThrValue(24, priority=1)\
            .addThrValue(24, -7, 0, priority=2).addThrValue(24, 0, 7, priority=2)\
            .addThrValue(23, -8, -7, priority=2).addThrValue(23, 7, 8, priority=2)\
            .addThrValue(22, -11, -8, priority=2).addThrValue(22, 8, 11, priority=2)\
            .addThrValue(21, -13, -11, priority=2).addThrValue(21, 11, 13, priority=2)\
            .addThrValue(20, -14, -13, priority=2).addThrValue(20, 13, 14, priority=2)\
            .addThrValue(19, -15, -14, priority=2).addThrValue(19, 14, 15, priority=2)\
            .addThrValue(21, -17, -15, priority=2).addThrValue(21, 15, 17, priority=2)\
            .addThrValue(23, -25, -17, priority=2).addThrValue(23, 17, 25, priority=2)

           ThresholdValue.setDefaults('EM', {'isobits' : '01000', 'use_relIso' : True })
           tc.registerThr( 'EM24VHIM', type='EM').addThrValue(24, priority=1)\
            .addThrValue(26, -7, 0, priority=2).addThrValue(26, 0, 7, priority=2)\
            .addThrValue(25, -8, -7, priority=2).addThrValue(25, 7, 8, priority=2)\
            .addThrValue(24, -11, -8, priority=2).addThrValue(24, 8, 11, priority=2)\
            .addThrValue(23, -13, -11, priority=2).addThrValue(23, 11, 13, priority=2)\
            .addThrValue(22, -14, -13, priority=2).addThrValue(22, 13, 14, priority=2)\
            .addThrValue(21, -15, -14, priority=2).addThrValue(21, 14, 15, priority=2)\
            .addThrValue(23, -17, -15, priority=2).addThrValue(23, 15, 17, priority=2)\
            .addThrValue(25, -25, -17, priority=2).addThrValue(25, 17, 25, priority=2)


        ThresholdValue.setDefaults('EM', {'isobits' : '00100', 'use_relIso' : True })

        if not '_v6' in TriggerFlags.triggerMenuSetup() and not '_HI' in TriggerFlags.triggerMenuSetup():
           tc.registerThr( 'EM15VHI', type='EM').addThrValue(17, priority=1)\
            .addThrValue(17, -7, 0, priority=2).addThrValue(17, 0, 7, priority=2)\
            .addThrValue(16, -9, -7, priority=2).addThrValue(16, 7, 9, priority=2)\
            .addThrValue(15, -12, -9, priority=2).addThrValue(15, 9, 12, priority=2)\
            .addThrValue(14, -14, -12, priority=2).addThrValue(14, 12, 14, priority=2)\
            .addThrValue(13, -15, -14, priority=2).addThrValue(13, 14, 15, priority=2)\
            .addThrValue(15, -17, -15, priority=2).addThrValue(15, 15, 17, priority=2)\
            .addThrValue(16, -25, -17, priority=2).addThrValue(16, 17, 25, priority=2)

        for thrV in [15]:
           tc.registerThr('EM%iHI' % thrV, 'EM').addThrValue(thrV)

        tc.registerThr( 'EM18VHI', type='EM').addThrValue(20, priority=1)\
            .addThrValue(20, -7, 0, priority=2).addThrValue(20, 0, 7, priority=2)\
            .addThrValue(19, -8, -7, priority=2).addThrValue(19, 7, 8, priority=2)\
            .addThrValue(18, -11, -8, priority=2).addThrValue(18, 8, 11, priority=2)\
            .addThrValue(17, -13, -11, priority=2).addThrValue(17, 11, 13, priority=2)\
            .addThrValue(16, -14, -13, priority=2).addThrValue(16, 13, 14, priority=2)\
            .addThrValue(15, -15, -14, priority=2).addThrValue(15, 14, 15, priority=2)\
            .addThrValue(17, -17, -15, priority=2).addThrValue(17, 15, 17, priority=2)\
            .addThrValue(19, -25, -17, priority=2).addThrValue(19, 17, 25, priority=2)

        tc.registerThr( 'EM20VHI', type='EM').addThrValue(22, priority=1)\
            .addThrValue(22, -7, 0, priority=2).addThrValue(22, 0, 7, priority=2)\
            .addThrValue(21, -8, -7, priority=2).addThrValue(21, 7, 8, priority=2)\
            .addThrValue(20, -11, -8, priority=2).addThrValue(20, 8, 11, priority=2)\
            .addThrValue(19, -13, -11, priority=2).addThrValue(19, 11, 13, priority=2)\
            .addThrValue(18, -14, -13, priority=2).addThrValue(18, 13, 14, priority=2)\
            .addThrValue(17, -15, -14, priority=2).addThrValue(17, 14, 15, priority=2)\
            .addThrValue(19, -17, -15, priority=2).addThrValue(19, 15, 17, priority=2)\
            .addThrValue(21, -25, -17, priority=2).addThrValue(21, 17, 25, priority=2)

        tc.registerThr( 'EM22VHI', type='EM').addThrValue(24, priority=1)\
            .addThrValue(24, -7, 0, priority=2).addThrValue(24, 0, 7, priority=2)\
            .addThrValue(23, -8, -7, priority=2).addThrValue(23, 7, 8, priority=2)\
            .addThrValue(22, -11, -8, priority=2).addThrValue(22, 8, 11, priority=2)\
            .addThrValue(21, -13, -11, priority=2).addThrValue(21, 11, 13, priority=2)\
            .addThrValue(20, -14, -13, priority=2).addThrValue(20, 13, 14, priority=2)\
            .addThrValue(19, -15, -14, priority=2).addThrValue(19, 14, 15, priority=2)\
            .addThrValue(21, -17, -15, priority=2).addThrValue(21, 15, 17, priority=2)\
            .addThrValue(23, -25, -17, priority=2).addThrValue(23, 17, 25, priority=2)
        
        tc.registerThr( 'EM24VHI', type='EM').addThrValue(24, priority=1)\
            .addThrValue(26, -7, 0, priority=2).addThrValue(26, 0, 7, priority=2)\
            .addThrValue(25, -8, -7, priority=2).addThrValue(25, 7, 8, priority=2)\
            .addThrValue(24, -11, -8, priority=2).addThrValue(24, 8, 11, priority=2)\
            .addThrValue(23, -13, -11, priority=2).addThrValue(23, 11, 13, priority=2)\
            .addThrValue(22, -14, -13, priority=2).addThrValue(22, 13, 14, priority=2)\
            .addThrValue(21, -15, -14, priority=2).addThrValue(21, 14, 15, priority=2)\
            .addThrValue(23, -17, -15, priority=2).addThrValue(23, 15, 17, priority=2)\
            .addThrValue(25, -25, -17, priority=2).addThrValue(25, 17, 25, priority=2)
       
        tc.registerThr( 'EM26VHI', type='EM').addThrValue(26, priority=1)\
            .addThrValue(28, -7, 0, priority=2).addThrValue(28, 0, 7, priority=2)\
            .addThrValue(27, -8, -7, priority=2).addThrValue(27, 7, 8, priority=2)\
            .addThrValue(26, -11, -8, priority=2).addThrValue(26, 8, 11, priority=2)\
            .addThrValue(25, -13, -11, priority=2).addThrValue(25, 11, 13, priority=2)\
            .addThrValue(24, -14, -13, priority=2).addThrValue(24, 13, 14, priority=2)\
            .addThrValue(23, -15, -14, priority=2).addThrValue(23, 14, 15, priority=2)\
            .addThrValue(25, -17, -15, priority=2).addThrValue(25, 15, 17, priority=2)\
            .addThrValue(27, -25, -17, priority=2).addThrValue(27, 17, 25, priority=2)


        tc.registerThr( 'EM3HI', type='EM').addThrValue(3, priority=1)\


        #ThresholdValue.setDefaults('EM', {'etamin' : 16,'etamax' : 20, 'phimin' : 0,'phimax' : 64, 'isobits' : '00000', 'use_relIso' : True })
        #tc.registerThr('EM20A', 'EM').addThrValue(20)
        #ThresholdValue.setDefaults('EM', {'etamin' : -20,'etamax' : -16, 'phimin' : 0,'phimax' : 64, 'isobits' : '00000', 'use_relIso' : True })
        #tc.registerThr('EM20C', 'EM').addThrValue(20)




        ThresholdValue.setDefaults('EM', {})


        # TAU

        ThresholdValue.setDefaults('TAU',{'isobits' : '00000', 'use_relIso' : True })

        for thrV in [1, 2, 3, 6, 8, 12, 15, 20, 25, 30, 35, 40, 50, 60,100]:
            tc.registerThr('HA%i' % thrV, 'TAU').addThrValue(thrV)

        # beam splashes     
        for thrV in [20]: 
            tc.registerThr('HA%iA' % thrV, 'TAU').addThrValue(255, priority=1).addThrValue( thrV, etamin = 12,  etamax = 16, priority=2) 
            tc.registerThr('HA%iC' % thrV, 'TAU').addThrValue(255, priority=1).addThrValue( thrV, etamin = -16,  etamax = -12, priority=2) 


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
        if '_v6' in TriggerFlags.triggerMenuSetup() or '_HI' in TriggerFlags.triggerMenuSetup():
            for thrV in [12,20]:
                tc.registerThr('HA%iIM' % thrV, 'TAU').addThrValue(thrV)
        else:
            for thrV in [12,20,25]:
                tc.registerThr('HA%iIM' % thrV, 'TAU').addThrValue(thrV)   

        ThresholdValue.setDefaults('TAU', {'isobits' : '00100', 'use_relIso' : True })
        if '_v6' in TriggerFlags.triggerMenuSetup() or '_HI' in TriggerFlags.triggerMenuSetup():
            for thrV in [12,20,25]:
                tc.registerThr('HA%iIT' % thrV, 'TAU').addThrValue(thrV)
        else:
            for thrV in [12,20]:
                tc.registerThr('HA%iIT' % thrV, 'TAU').addThrValue(thrV)
  
        ThresholdValue.setDefaults('TAU', {'isobits' : '01000', 'use_relIso' : True })
        for thrV in [12,20]:
            tc.registerThr('HA%iI' % thrV, 'TAU').addThrValue(thrV)

        ThresholdValue.setDefaults('TAU', {})
            
            
        # JET

        ThresholdValue.setDefaults('JET', {'window' : 8})

        for thrV in [5, 10, 12, 15, 20, 25, 30, 35, 40, 50, 60, 70, 75, 85, 100, 120, 150,175, 250, 400]:
            tc.registerThr('J%i' % thrV, 'JET').addThrValue(JetOff).addThrValue(thrV, etamin=-31, etamax=31, priority=1) # jets are between -31 and 31 -ATR-11526
        
        ThresholdValue.setDefaults('JET', {})

        
        # JB and JF

        ThresholdValue.setDefaults('JET', {'window' : 8})
        
        #for thrV in [25]:
        #    tc.registerThr('JF%i' % thrV, 'JET').addThrValue(JetOff).addThrValue( thrV, etamin=31,  etamax=49, priority=1)
        #    tc.registerThr('JB%i' % thrV, 'JET').addThrValue(JetOff).addThrValue( thrV, etamin=-49, etamax=-31, priority=1)

        # Beam Splashes
        for thrV in [75]:
            tc.registerThr('J%iA' % thrV, 'JET').addThrValue(JetOff).addThrValue( thrV, etamin = 15,  etamax = 23, priority=1)
            tc.registerThr('J%iC' % thrV, 'JET').addThrValue(JetOff).addThrValue( thrV, etamin = -23,  etamax = -15, priority=1)          
        

        # Central jet
        for (thrV, etamax) in [(12,23), (15,25), (17,22), (20,28), (25,23), (35,23), (20,49), (30,49), (40,25), (45,20)]:
            tc.registerThr('J%i.0ETA%i'  % (thrV, etamax), 'JET').addThrValue(JetOff).addThrValue( thrV, etamin = -etamax,  etamax = etamax, priority=1)  

        # Standard forward jet
        for thrV in [10, 15, 20, 25, 30, 35, 45, 50, 70, 75, 100]:
            tc.registerThr('J%i.31ETA49' % thrV, 'JET').addThrValue(JetOff).addThrValue( thrV, etamin=31,  etamax=49, priority=1).addThrValue( thrV, etamin=-49, etamax=-31, priority=1)

        # Custom Forward jet and VBF jets
        for (thrV, etamin, etamax) in [ (15,23,49), (15,24,49), (20,28,31) ]:
            tc.registerThr('J%i.%iETA%i' % (thrV, etamin,etamax), 'JET').addThrValue(JetOff).addThrValue( thrV, etamin  =  etamin, etamax=etamax, priority=1).addThrValue( thrV, etamin = -etamax, etamax = -etamin, priority=1)

        ##4x4 jet windows for forward electrons
        ThresholdValue.setDefaults('JET', {'window' : 4})
        
        for thrV in [15]:
            tc.registerThr('JJ%i.23ETA49' % thrV, 'JET').addThrValue(JetOff).addThrValue( thrV, etamin=23,  etamax=49, priority=1).addThrValue( thrV, etamin=-49, etamax=-23, priority=1)

        ThresholdValue.setDefaults('JET', {})


        # ZB
        tc.registerThr('ZB_EM12', 'ZB', seed='EM12',seed_ttype = 'EM',  seed_multi=1, bcdelay=3564)
        tc.registerThr('ZB_EM15', 'ZB', seed='EM15',seed_ttype = 'EM',  seed_multi=1, bcdelay=3564)
        tc.registerThr('ZB_J10',  'ZB', seed='J10', seed_ttype = 'JET', seed_multi=1, bcdelay=3564)
        # tc.registerThr('ZB_J20',  'ZB', seed='J20', seed_ttype = 'JET', seed_multi=1, bcdelay=3564)
        tc.registerThr('ZB_J75',  'ZB', seed='J75', seed_ttype = 'JET', seed_multi=1, bcdelay=3564)


        # JE

        ThresholdValue.setDefaults('JE', {'etamin' : -49,'etamax' : 49, 'phimin' : 0,'phimax' : 64,
                                                       'window' : 8, 'priority': 0})

        for thrV in [100, 140, 200, 300, 350, 500]:
            tc.registerThr('JE%i' % thrV, 'JE').addThrValue(thrV)

        
        # TE
        for thrV in [0, 2, 3, 4, 5, 10, 15, 20, 25, 30, 40, 45, 50, 55, 60, 65, 70, 90, 100, 120, 140, 160, 200, 280, 300, 360, 2000, 4000, 10000, 12000, 14000]:
            tc.registerThr('TE%i' % thrV, 'TE').addThrValue(thrV)

        # XE
        for thrV in [10, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 150, 300]:
            tc.registerThr('XE%i' % thrV, 'XE').addThrValue(thrV)

        # XS
        for thrV in [20, 25, 30, 35, 40, 45, 50, 55, 60, 65]:
            tc.registerThr('XS%i' % thrV, 'XS').addThrValue(thrV)

        # Restricted range TE |eta|<2.4
        etamax = 24
        for thrV in [0, 3, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 80, 110, 150, 180, 1000, 2000, 5000, 6500, 8000, 9000]:
            tc.registerThr('TE%i.0ETA%i' % (thrV, etamax), 'TE').addThrValue(EtSumOff).addThrValue( thrV, etamin = -etamax, etamax = etamax, priority=1)
        
        # Restricted range TE 2.4<|eta|<4.9
        etamin = 24
        etamax = 49
        for thrV in [0, 3, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 80, 110, 150, 180, 1000, 2000, 5000, 6500, 8000, 9000]:
            tc.registerThr('TE%i.%iETA%i' % (thrV, etamin, etamax), 'TE').addThrValue(EtSumOff).addThrValue( thrV, etamin  =  etamin, etamax=etamax, priority=1).addThrValue( thrV, etamin = -etamax, etamax = -etamin, priority=1)

        # RXE (restriced range ET miss)
        etamax = 24
        for thrV in [30, 35, 40, 45, 50, 55, 60, 70, 80]:
            tc.registerThr('XE%i.0ETA%i'    % (thrV, etamax), 'XE').addThrValue(EtMissOff).addThrValue( thrV, etamin = -etamax, etamax = etamax, priority=1) 

	 # Restricted range TE |eta|<4.9
        etamax = 49
        for thrV in [3, 7, 500, 600, 1500, 3000, 3500, 5000, 6500, 8000, 9000]:
            tc.registerThr('TE%i.0ETA%i' % (thrV, etamax), 'TE').addThrValue(EtSumOff).addThrValue( thrV, etamin = -etamax, etamax = etamax, priority=1)


        ##RESTRICTED ETA THRESHOLD FOR TE and XE HAVE TO USE THE SAME ETA RANGE. Only possible for thesholds 8-15
        #ThresholdValue.setDefaults('TE', {'etamin' : -24,'etamax' : 24, 'priority': 1})
        #
        #for  thrV in [(30),]:
        #    tc.registerThr('TE%i.0ETA24' % (thrV), 'TE').addThrValue(thrV)
            
            
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
        thresholdA=[ 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0]
        thresholdC=[ 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0]

        for i, (vA, vC) in enumerate(zip(thresholdA, thresholdC)):
            tc.registerThr('MBTS_A%i' % i, 'MBTSSI').addThrValue(vA)
            tc.registerThr('MBTS_C%i' % i, 'MBTSSI').addThrValue(vC)

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

        ## LUCID

        tc.registerThr('LUCID_A', 'LUCID').addThrValue(650)
        tc.registerThr('LUCID_C', 'LUCID').addThrValue(650)
        tc.registerThr('LUCID_Coinc_AC', 'LUCID').addThrValue(650)
        tc.registerThr('LUCID_COMM', 'LUCID').addThrValue(650)
        tc.registerThr('LUCID_05', 'LUCID').addThrValue(650)
        tc.registerThr('LUCID_06', 'LUCID').addThrValue(650)

        ## AFP

#        if not '_v6' in TriggerFlags.triggerMenuSetup(): # first 2017 config
#            tc.registerThr('AFP_C_SPECTOF', 'NIM', mapping=2).addThrValue(50)        
#            tc.registerThr('AFP_C', 'NIM', mapping=3).addThrValue(50)        
#            tc.registerThr('AFP_A_SPECTOF', 'NIM', mapping=4).addThrValue(50)        
#            tc.registerThr('AFP_A', 'NIM', mapping=5).addThrValue(50)  
        if not '_v6' in TriggerFlags.triggerMenuSetup():
            tc.registerThr('AFP_NSC', 'NIM', mapping=2).addThrValue(50)        
            tc.registerThr('AFP_NSA', 'NIM', mapping=3).addThrValue(50)        
            tc.registerThr('AFP_FSA_SIT', 'NIM', mapping=4).addThrValue(50)        
            tc.registerThr('AFP_FSA_TOF', 'NIM', mapping=5).addThrValue(50)  
            tc.registerThr('AFP_FSA_LOG', 'NIM', mapping=6).addThrValue(50)        
            tc.registerThr('AFP_FSC_SIT', 'NIM', mapping=7).addThrValue(50)        
            tc.registerThr('AFP_FSC_LOG', 'NIM', mapping=8).addThrValue(50)        
            tc.registerThr('AFP_FSC_TOF', 'NIM', mapping=9).addThrValue(50)  
        else:
            tc.registerThr('AFP_NSC', 'NIM', mapping=2).addThrValue(50)
            tc.registerThr('AFP_FSC', 'NIM', mapping=3).addThrValue(50)
            tc.registerThr('AFP_NSA', 'NIM', mapping=4).addThrValue(50)
            tc.registerThr('AFP_FSA', 'NIM', mapping=5).addThrValue(50)      

        # Diamond beam monitors
        #for i in range(8):
        #    tc.registerThr('DBM%i'    % i, 'DBM')

        ## BPTX

        tc.registerThr('BPTX0', 'BPTX').addThrValue(40)
        tc.registerThr('BPTX1', 'BPTX').addThrValue(40)


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


        # ALFA
        LUT1offset =  2 # this is needed because the first 4 direct inputs are in a LUT with 8 PITs so the OR with the next inputs would not work
        LUT2offset =  8
        LUT3offset = 14
        LUT4offset = 20
        LUT5offset = 26
        for i, alfa in enumerate( ['B7R1L', 'B7R1U', 'A7R1L', 'A7R1U', 'A7L1L', 'A7L1U', 'B7L1L', 'B7L1U'] ):
            phaseOffset = 32 * (i%2)
            tc.registerThr('ALFA_%s'    % alfa, 'ALFA', mapping = LUT1offset + i/2 + phaseOffset )
            tc.registerThr('ALFA2_%s'   % alfa, 'ALFA', mapping = LUT2offset + i/2 + phaseOffset )
            tc.registerThr('ALFA3_%s'   % alfa, 'ALFA', mapping = LUT3offset + i/2 + phaseOffset )
            tc.registerThr('ALFA4_%s'   % alfa, 'ALFA', mapping = LUT4offset + i/2 + phaseOffset )
            tc.registerThr('ALFA_%s_OD' % alfa, 'ALFA', mapping = LUT5offset + i/2 + phaseOffset )





        # DIRECT INPUTS
        # all topo inputs are directly set from the L1Topo menu
        

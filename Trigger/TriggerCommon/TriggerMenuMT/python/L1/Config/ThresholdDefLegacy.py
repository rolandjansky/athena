# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from ..Base.Thresholds import LegacyThreshold, ZeroBiasThreshold, ThresholdValue
from ..Base.Limits import CaloLimits as CL


class ThresholdDefLegacy:

    alreadyExecuted = False

    @staticmethod
    def registerThresholds(tc, menuName):

        # dear Menu Experts, please note that it is only necessary to
        # check the menu name if a certain threshold has different
        # definitions in different menus

        if ThresholdDefLegacy.alreadyExecuted:
            return
        ThresholdDefLegacy.alreadyExecuted = True
        
        # EM

        ThresholdValue.setDefaults('EM',{'isobits' : '00000', 'use_relIso' : True })

        for thrV in [3, 5, 7, 8, 10, 12, 14, 15, 16, 18, 20, 22]:
            LegacyThreshold('EM%i' % thrV, 'EM').addThrValue(thrV)

        # for beam splashes:
        LegacyThreshold('EM20A', 'EM').addThrValue(255, priority=1).addThrValue(20,16,20,priority=2)
        LegacyThreshold('EM20C', 'EM').addThrValue(255, priority=1).addThrValue(20,-20,-16,priority=2)

        # variable thresholds (V)
        # hadronic isolation (H) had_isolation=1GeV
        # em isolation (I)

        # I section

        ThresholdValue.setDefaults('EM', {'isobits' : '00010', 'use_relIso' : True })

        for thrV in [8]:
            LegacyThreshold('EM%iI' % thrV, 'EM').addThrValue(thrV)

        # VH section

        ThresholdValue.setDefaults('EM', {'isobits' : '00001', 'use_relIso' : True })

        LegacyThreshold( 'EM8VH', 'EM').addThrValue(9, priority=1)\
            .addThrValue(8, -25, -18, priority=2).addThrValue(7, -18, -15, priority=2)\
            .addThrValue(5, -15, -14, priority=2).addThrValue(6, -14, -11, priority=2)\
            .addThrValue(7, -11, -8, priority=2).addThrValue(9, -8, 0, priority=2)\
            .addThrValue(9, 0, 8, priority=2).addThrValue(7, 8, 11, priority=2)\
            .addThrValue(6, 11, 14, priority=2).addThrValue(5, 14, 15, priority=2)\
            .addThrValue(7, 15, 18, priority=2).addThrValue(8, 18, 25, priority=2)

        LegacyThreshold( 'EM10VH', 'EM').addThrValue(11, priority=1)\
            .addThrValue(10, -25, -18, priority=2).addThrValue(9, -18, -15, priority=2)\
            .addThrValue(7, -15, -14, priority=2).addThrValue(8, -14, -11, priority=2)\
            .addThrValue(9, -11, -8, priority=2).addThrValue(11, -8, 0, priority=2)\
            .addThrValue(11, 0, 8, priority=2).addThrValue(9, 8, 11, priority=2)\
            .addThrValue(8, 11, 14, priority=2).addThrValue(7, 14, 15, priority=2)\
            .addThrValue(9, 15, 18, priority=2).addThrValue(10, 18, 25, priority=2)

        LegacyThreshold( 'EM13VH', 'EM').addThrValue(15, priority=1)\
            .addThrValue(14, -25, -17, priority=2).addThrValue(13, -17, -15, priority=2)\
            .addThrValue(11, -15, -14, priority=2).addThrValue(12, -14, -12, priority=2)\
            .addThrValue(13, -12, -9, priority=2).addThrValue(14, -9, -7, priority=2)\
            .addThrValue(15, -7, 0, priority=2).addThrValue(15, 0, 7, priority=2)\
            .addThrValue(14, 7, 9, priority=2).addThrValue(13, 9, 12, priority=2)\
            .addThrValue(12, 12, 14, priority=2).addThrValue(11, 14, 15, priority=2)\
            .addThrValue(13, 15, 17, priority=2).addThrValue(14, 17, 25, priority=2)

        LegacyThreshold( 'EM15VH', 'EM').addThrValue(17, priority=1)\
            .addThrValue(16, -25, -17, priority=2).addThrValue(15, -17, -15, priority=2)\
            .addThrValue(13, -15, -14, priority=2).addThrValue(14, -14, -12, priority=2)\
            .addThrValue(15, -12, -9, priority=2).addThrValue(16, -9, -7, priority=2)\
            .addThrValue(17, -7, 0, priority=2).addThrValue(17, 0, 7, priority=2)\
            .addThrValue(16, 7, 9, priority=2).addThrValue(15, 9, 12, priority=2)\
            .addThrValue(14, 12, 14, priority=2).addThrValue(13, 14, 15, priority=2)\
            .addThrValue(15, 15, 17, priority=2).addThrValue(16, 17, 25, priority=2)

        LegacyThreshold( 'EM18VH', 'EM').addThrValue(20, priority=1)\
            .addThrValue(19, -25, -17, priority=2).addThrValue(17, -17, -15, priority=2)\
            .addThrValue(15, -15, -14, priority=2).addThrValue(16, -14, -13, priority=2)\
            .addThrValue(17, -13, -11, priority=2).addThrValue(18, -11, -8, priority=2)\
            .addThrValue(19, -8, -7, priority=2).addThrValue(20, -7, 0, priority=2)\
            .addThrValue(20, 0, 7, priority=2).addThrValue(19, 7, 8, priority=2)\
            .addThrValue(18, 8, 11, priority=2).addThrValue(17, 11, 13, priority=2)\
            .addThrValue(16, 13, 14, priority=2).addThrValue(15, 14, 15, priority=2)\
            .addThrValue(17, 15, 17, priority=2).addThrValue(19, 17, 25, priority=2)

        LegacyThreshold( 'EM20VH', 'EM').addThrValue(22, priority=1)\
            .addThrValue(21, -25, -17, priority=2).addThrValue(19, -17, -15, priority=2)\
            .addThrValue(17, -15, -14, priority=2).addThrValue(18, -14, -13, priority=2)\
            .addThrValue(19, -13, -11, priority=2).addThrValue(20, -11, -8, priority=2)\
            .addThrValue(21, -8, -7, priority=2).addThrValue(22, -7, 0, priority=2)\
            .addThrValue(22, 0, 7, priority=2).addThrValue(21, 7, 8, priority=2)\
            .addThrValue(20, 8, 11, priority=2).addThrValue(19, 11, 13, priority=2)\
            .addThrValue(18, 13, 14, priority=2).addThrValue(17, 14, 15, priority=2)\
            .addThrValue(19, 15, 17, priority=2).addThrValue(21, 17, 25, priority=2)

        LegacyThreshold( 'EM22VH', 'EM').addThrValue(24, priority=1)\
            .addThrValue(23, -25, -17, priority=2).addThrValue(21, -17, -15, priority=2)\
            .addThrValue(19, -15, -14, priority=2).addThrValue(20, -14, -13, priority=2)\
            .addThrValue(21, -13, -11, priority=2).addThrValue(22, -11, -8, priority=2)\
            .addThrValue(23, -8, -7, priority=2).addThrValue(24, -7, 0, priority=2)\
            .addThrValue(24, 0, 7, priority=2).addThrValue(23, 7, 8, priority=2)\
            .addThrValue(22, 8, 11, priority=2).addThrValue(21, 11, 13, priority=2)\
            .addThrValue(20, 13, 14, priority=2).addThrValue(19, 14, 15, priority=2)\
            .addThrValue(21, 15, 17, priority=2).addThrValue(23, 17, 25, priority=2)

        # (V)HI section

        ThresholdValue.setDefaults('EM', {'isobits' : '00100', 'use_relIso' : True })

        for thrV in [15]:
            LegacyThreshold('EM%iHI' % thrV, 'EM').addThrValue(thrV)

        LegacyThreshold( 'EM15VHI', 'EM').addThrValue(17, priority=1)\
            .addThrValue(16, -25, -17, priority=2).addThrValue(15, -17, -15, priority=2)\
            .addThrValue(13, -15, -14, priority=2).addThrValue(14, -14, -12, priority=2)\
            .addThrValue(15, -12, -9, priority=2).addThrValue(16, -9, -7, priority=2)\
            .addThrValue(17, -7, 0, priority=2).addThrValue(17, 0, 7, priority=2)\
            .addThrValue(16, 7, 9, priority=2).addThrValue(15, 9, 12, priority=2)\
            .addThrValue(14, 12, 14, priority=2).addThrValue(13, 14, 15, priority=2)\
            .addThrValue(15, 15, 17, priority=2).addThrValue(16, 17, 25, priority=2)

        LegacyThreshold( 'EM18VHI', 'EM').addThrValue(20, priority=1)\
            .addThrValue(19, -25, -17, priority=2).addThrValue(17, -17, -15, priority=2)\
            .addThrValue(15, -15, -14, priority=2).addThrValue(16, -14, -13, priority=2)\
            .addThrValue(17, -13, -11, priority=2).addThrValue(18, -11, -8, priority=2)\
            .addThrValue(19, -8, -7, priority=2).addThrValue(20, -7, 0, priority=2)\
            .addThrValue(20, 0, 7, priority=2).addThrValue(19, 7, 8, priority=2)\
            .addThrValue(18, 8, 11, priority=2).addThrValue(17, 11, 13, priority=2)\
            .addThrValue(16, 13, 14, priority=2).addThrValue(15, 14, 15, priority=2)\
            .addThrValue(17, 15, 17, priority=2).addThrValue(19, 17, 25, priority=2)

 
        LegacyThreshold( 'EM20VHI', 'EM').addThrValue(22, priority=1)\
            .addThrValue(21, -25, -17, priority=2).addThrValue(19, -17, -15, priority=2)\
            .addThrValue(17, -15, -14, priority=2).addThrValue(18, -14, -13, priority=2)\
            .addThrValue(19, -13, -11, priority=2).addThrValue(20, -11, -8, priority=2)\
            .addThrValue(21, -8, -7, priority=2).addThrValue(22, -7, 0, priority=2)\
            .addThrValue(22, 0, 7, priority=2).addThrValue(21, 7, 8, priority=2)\
            .addThrValue(20, 8, 11, priority=2).addThrValue(19, 11, 13, priority=2)\
            .addThrValue(18, 13, 14, priority=2).addThrValue(17, 14, 15, priority=2)\
            .addThrValue(19, 15, 17, priority=2).addThrValue(21, 17, 25, priority=2)

        LegacyThreshold( 'EM22VHI', 'EM').addThrValue(24, priority=1)\
            .addThrValue(23, -25, -17, priority=2).addThrValue(21, -17, -15, priority=2)\
            .addThrValue(19, -15, -14, priority=2).addThrValue(20, -14, -13, priority=2)\
            .addThrValue(21, -13, -11, priority=2).addThrValue(22, -11, -8, priority=2)\
            .addThrValue(23, -8, -7, priority=2).addThrValue(24, -7, 0, priority=2)\
            .addThrValue(24, 0, 7, priority=2).addThrValue(23, 7, 8, priority=2)\
            .addThrValue(22, 8, 11, priority=2).addThrValue(21, 11, 13, priority=2)\
            .addThrValue(20, 13, 14, priority=2).addThrValue(19, 14, 15, priority=2)\
            .addThrValue(21, 15, 17, priority=2).addThrValue(23, 17, 25, priority=2)

        LegacyThreshold( 'EM24VHI', 'EM').addThrValue(24, priority=1)\
            .addThrValue(25, -25, -17, priority=2).addThrValue(23, -17, -15, priority=2)\
            .addThrValue(21, -15, -14, priority=2).addThrValue(22, -14, -13, priority=2)\
            .addThrValue(23, -13, -11, priority=2).addThrValue(24, -11, -8, priority=2)\
            .addThrValue(25, -8, -7, priority=2).addThrValue(26, -7, 0, priority=2)\
            .addThrValue(26, 0, 7, priority=2).addThrValue(25, 7, 8, priority=2)\
            .addThrValue(24, 8, 11, priority=2).addThrValue(23, 11, 13, priority=2)\
            .addThrValue(22, 13, 14, priority=2).addThrValue(21, 14, 15, priority=2)\
            .addThrValue(23, 15, 17, priority=2).addThrValue(25, 17, 25, priority=2)

        LegacyThreshold( 'EM26VHI', 'EM').addThrValue(26, priority=1)\
            .addThrValue(27, -25, -17, priority=2).addThrValue(25, -17, -15, priority=2)\
            .addThrValue(23, -15, -14, priority=2).addThrValue(24, -14, -13, priority=2)\
            .addThrValue(25, -13, -11, priority=2).addThrValue(26, -11, -8, priority=2)\
            .addThrValue(27, -8, -7, priority=2).addThrValue(28, -7, 0, priority=2)\
            .addThrValue(28, 0, 7, priority=2).addThrValue(27, 7, 8, priority=2)\
            .addThrValue(26, 8, 11, priority=2).addThrValue(25, 11, 13, priority=2)\
            .addThrValue(24, 13, 14, priority=2).addThrValue(23, 14, 15, priority=2)\
            .addThrValue(25, 15, 17, priority=2).addThrValue(27, 17, 25, priority=2)

        LegacyThreshold( 'EM30VHI', 'EM').addThrValue(30, priority=1)\
            .addThrValue(31, -29, -17, priority=2).addThrValue(29, -17, -15, priority=2)\
            .addThrValue(27, -15, -14, priority=2).addThrValue(28, -14, -13, priority=2)\
            .addThrValue(29, -13, -11, priority=2).addThrValue(30, -11, -8, priority=2)\
            .addThrValue(31, -8, -7, priority=2).addThrValue(32, -7, 0, priority=2)\
            .addThrValue(32, 0, 7, priority=2).addThrValue(31, 7, 8, priority=2)\
            .addThrValue(30, 8, 11, priority=2).addThrValue(29, 11, 13, priority=2)\
            .addThrValue(28, 13, 14, priority=2).addThrValue(27, 14, 15, priority=2)\
            .addThrValue(29, 15, 17, priority=2).addThrValue(31, 17, 29, priority=2)


        # VHIM section

        ThresholdValue.setDefaults('EM', {'isobits' : '01000', 'use_relIso' : True })

        LegacyThreshold( 'EM24VHIM', 'EM').addThrValue(24, priority=1)\
            .addThrValue(25, -25, -17, priority=2).addThrValue(23, -17, -15, priority=2)\
            .addThrValue(21, -15, -14, priority=2).addThrValue(22, -14, -13, priority=2)\
            .addThrValue(23, -13, -11, priority=2).addThrValue(24, -11, -8, priority=2)\
            .addThrValue(25, -8, -7, priority=2).addThrValue(26, -7, 0, priority=2)\
            .addThrValue(26, 0, 7, priority=2).addThrValue(25, 7, 8, priority=2)\
            .addThrValue(24, 8, 11, priority=2).addThrValue(23, 11, 13, priority=2)\
            .addThrValue(22, 13, 14, priority=2).addThrValue(21, 14, 15, priority=2)\
            .addThrValue(23, 15, 17, priority=2).addThrValue(25, 17, 25, priority=2)

        ThresholdValue.setDefaults('EM', {'isobits' : '00100', 'use_relIso' : True })

        ThresholdValue.setDefaults('EM', {})






        # TAU

        ThresholdValue.setDefaults('TAU',{'isobits' : '00000', 'use_relIso' : True })

        for thrV in [1, 2, 3, 5, 6, 8, 12, 15, 20, 25, 30, 35, 40, 50, 60, 90, 100]:
            LegacyThreshold('HA%i' % thrV, 'TAU').addThrValue(thrV)

        # beam splashes     
        for thrV in [20]: 
            LegacyThreshold('HA%iA' % thrV, 'TAU').addThrValue(255, priority=1).addThrValue( thrV, etamin = 12,  etamax = 16, priority=2) 
            LegacyThreshold('HA%iC' % thrV, 'TAU').addThrValue(255, priority=1).addThrValue( thrV, etamin = -16,  etamax = -12, priority=2) 

        ThresholdValue.setDefaults('TAU', {'isobits' : '00001', 'use_relIso' : True })
        for thrV in [12,20]:
            LegacyThreshold('HA%iIL' % thrV, 'TAU').addThrValue(thrV)

        ThresholdValue.setDefaults('TAU', {'isobits' : '00010', 'use_relIso' : True })
        for thrV in [12,20,25]:
            LegacyThreshold('HA%iIM' % thrV, 'TAU').addThrValue(thrV)   
        
        ThresholdValue.setDefaults('TAU', {'isobits' : '00100', 'use_relIso' : True })
        for thrV in [12,20,25]:
            LegacyThreshold('HA%iIT' % thrV, 'TAU').addThrValue(thrV)
  
        ThresholdValue.setDefaults('TAU', {'isobits' : '01000', 'use_relIso' : True })
        for thrV in [12,20]:
            LegacyThreshold('HA%iI' % thrV, 'TAU').addThrValue(thrV)

        ThresholdValue.setDefaults('TAU', {})
            
            
        # JET

        ThresholdValue.setDefaults('JET', {'window' : 8})

        for thrV in [5, 10, 12, 15, 20, 25, 30, 35, 40, 50, 60, 70, 75, 85, 100, 120, 150,175, 250, 400]:
            LegacyThreshold('J%i' % thrV, 'JET').addThrValue(CL.JetOff).addThrValue(thrV, etamin=-31, etamax=31, priority=1) # jets are between -31 and 31 -ATR-11526
                
        ThresholdValue.setDefaults('JET', {})

        
        # JB and JF

        ThresholdValue.setDefaults('JET', {'window' : 8})
        
        # Beam Splashes
        for thrV in [75]:
            LegacyThreshold('J%iA' % thrV, 'JET').addThrValue(CL.JetOff).addThrValue( thrV, etamin = 15,  etamax = 23, priority=1)
            LegacyThreshold('J%iC' % thrV, 'JET').addThrValue(CL.JetOff).addThrValue( thrV, etamin = -23,  etamax = -15, priority=1)          
        

        # Central jet
        for (thrV, etamax) in [(12,23), (15,25), (17,22), (20,28), (25,23), (35,23), (20,49), (30,49), (40,25), (45,20)]:
            LegacyThreshold('J%i.0ETA%i'  % (thrV, etamax), 'JET').addThrValue(CL.JetOff).addThrValue( thrV, etamin = -etamax,  etamax = etamax, priority=1)  

        # Standard forward jet
        for thrV in [10, 15, 20, 25, 30, 35, 45, 50, 70, 75, 100]:
            LegacyThreshold('J%i.31ETA49' % thrV, 'JET').addThrValue(CL.JetOff)\
                                                        .addThrValue( thrV, etamin=31,  etamax=49, priority=1)\
                                                        .addThrValue( thrV, etamin=-49, etamax=-31, priority=1)

        # Custom Forward jet and VBF jets
        for (thrV, etamin, etamax) in [ (15,23,49), (15,24,49), (20,28,31) ]:
            LegacyThreshold('J%i.%iETA%i' % (thrV, etamin,etamax), 'JET').addThrValue(CL.JetOff)\
                                                                         .addThrValue( thrV, etamin  =  etamin, etamax=etamax, priority=1)\
                                                                         .addThrValue( thrV, etamin = -etamax, etamax = -etamin, priority=1)

        ##4x4 jet windows for forward electrons
        ThresholdValue.setDefaults('JET', {'window' : 4})
        
        for thrV in [15]:
            LegacyThreshold('JJ%i.23ETA49' % thrV, 'JET')\
              .addThrValue(CL.JetOff)\
              .addThrValue( thrV, etamin=23,  etamax=49, priority=1)\
              .addThrValue( thrV, etamin=-49, etamax=-23, priority=1)

        ThresholdValue.setDefaults('JET', {})


        # ZB
        ZeroBiasThreshold('ZB_EM12').setSeedThreshold( seed='EM12', seed_multi=1, bcdelay=3564 )
        ZeroBiasThreshold('ZB_EM15').setSeedThreshold( seed='EM15', seed_multi=1, bcdelay=3564 )
        ZeroBiasThreshold('ZB_J10' ).setSeedThreshold( seed='J10',  seed_multi=1, bcdelay=3564 )
        ZeroBiasThreshold('ZB_J75' ).setSeedThreshold( seed='J75',  seed_multi=1, bcdelay=3564 )


        # JE

        ThresholdValue.setDefaults('JE', {'etamin' : -49,'etamax' : 49, 'phimin' : 0,'phimax' : 64,
                                                       'window' : 8, 'priority': 0})

        for thrV in [100, 140, 200, 300, 350, 500]:
            LegacyThreshold('JE%i' % thrV, 'JE').addThrValue(thrV)

        
        # TE
        for thrV in [0, 2, 3, 4, 5, 10, 15, 20, 25, 30, 40, 45, 50, 55, 60, 65, 70, 90, 100, 120, 140, 160, 200, 280, 300, 360, 2000, 4000, 10000, 12000, 14000]:
            LegacyThreshold('TE%i' % thrV, 'TE').addThrValue(thrV)

        # XE
        for thrV in [10, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 150, 300]:
            LegacyThreshold('XE%i' % thrV, 'XE').addThrValue(thrV)

        # XS
        for thrV in [20, 25, 30, 35, 40, 45, 50, 55, 60, 65]:
            LegacyThreshold('XS%i' % thrV, 'XS').addThrValue(thrV)

        # Restricted range TE |eta|<2.4
        etamax = 24
        for thrV in [0, 3, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 80, 110, 150, 180, 1000, 2000, 5000, 6500, 8000, 9000]:
            LegacyThreshold('TE%i.0ETA%i' % (thrV, etamax), 'TE').addThrValue(CL.EtSumOff).addThrValue( thrV, etamin = -etamax, etamax = etamax, priority=1)
        
        # Restricted range TE 2.4<|eta|<4.9
        etamin = 24
        etamax = 49
        for thrV in [0, 3, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 80, 110, 150, 180, 1000, 2000, 5000, 6500, 8000, 9000]:
            LegacyThreshold('TE%i.%iETA%i' % (thrV, etamin, etamax), 'TE')\
              .addThrValue(CL.EtSumOff)\
              .addThrValue( thrV, etamin  =  etamin, etamax=etamax, priority=1)\
              .addThrValue( thrV, etamin = -etamax, etamax = -etamin, priority=1)

        # RXE (restriced range ET miss)
        etamax = 24
        for thrV in [30, 35, 40, 45, 50, 55, 60, 70, 80]:
            LegacyThreshold('XE%i.0ETA%i'    % (thrV, etamax), 'XE').addThrValue(CL.EtMissOff).addThrValue( thrV, etamin = -etamax, etamax = etamax, priority=1) 

        # Restricted range TE |eta|<4.9
        etamax = 49
        for thrV in [3, 7, 500, 600, 1500, 3000, 3500, 5000, 6500, 8000, 9000]:
            LegacyThreshold('TE%i.0ETA%i' % (thrV, etamax), 'TE').addThrValue(CL.EtSumOff).addThrValue( thrV, etamin = -etamax, etamax = etamax, priority=1)


            


        

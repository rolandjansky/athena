# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from ..Base.Thresholds import LegacyThreshold, ZeroBiasThreshold, ThresholdValue
from ..Base.Limits import CaloLimits as CL


class ThresholdDefLegacy:

    alreadyExecuted = False

    EMVarRun2 = {
        1 : {
            "eta_bin_boundaries": [0, 0.8, 1.1, 1.4, 1.5, 1.8, 2.5], # 6 bins
            "shift": [ 1, -1, -2, -3, -1, 0]
        },
        2 : {
            "eta_bin_boundaries": [0, 0.7, 0.9, 1.2, 1.4, 1.5, 1.7, 2.5], # 7 bins
            "shift": [ 2, 1, 0, -1, -2, 0, 1]
        },
        3 : {
            "eta_bin_boundaries": [0, 0.7, 0.8, 1.1, 1.3, 1.4, 1.5, 1.7, 2.5], # 8 bins
            "shift" : [ 2, 1, 0, -1, -2, -3, -1, 1]
        }
    }

    @staticmethod
    def addVaryingThrValuesRun2(thr, pt, shift_set):
        eta_bin_boundaries = ThresholdDefLegacy.EMVarRun2[shift_set]["eta_bin_boundaries"]
        shift = ThresholdDefLegacy.EMVarRun2[shift_set]["shift"]
        thr.addThrValue(pt+shift[0], priority=1)
        for idx,sh in reversed(list(enumerate(shift))[1:]):
            eta_min = int(10 * eta_bin_boundaries[idx])
            eta_max = int(10 * eta_bin_boundaries[idx+1])
            thr.addThrValue( pt + sh, -eta_max, -eta_min, priority=2)
        for idx,sh in list(enumerate(shift))[1:]:
            eta_min = int(10 * eta_bin_boundaries[idx])
            eta_max = int(10 * eta_bin_boundaries[idx+1])
            thr.addThrValue( pt + sh, eta_min, eta_max, priority=2)
        return thr

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
        ThresholdDefLegacy.addVaryingThrValuesRun2( LegacyThreshold( 'EM8VH',  'EM'),  8, shift_set = 1 )
        ThresholdDefLegacy.addVaryingThrValuesRun2( LegacyThreshold( 'EM10VH', 'EM'), 10, shift_set = 1 )
        ThresholdDefLegacy.addVaryingThrValuesRun2( LegacyThreshold( 'EM13VH', 'EM'), 13, shift_set = 2 )
        ThresholdDefLegacy.addVaryingThrValuesRun2( LegacyThreshold( 'EM15VH', 'EM'), 15, shift_set = 2 )
        ThresholdDefLegacy.addVaryingThrValuesRun2( LegacyThreshold( 'EM18VH', 'EM'), 18, shift_set = 3 )        
        ThresholdDefLegacy.addVaryingThrValuesRun2( LegacyThreshold( 'EM20VH', 'EM'), 20, shift_set = 3 )
        ThresholdDefLegacy.addVaryingThrValuesRun2( LegacyThreshold( 'EM22VH', 'EM'), 22, shift_set = 3 )

        # (V)HI section
        ThresholdValue.setDefaults('EM', {'isobits' : '00100', 'use_relIso' : True })
        for thrV in [15]:
            LegacyThreshold('EM%iHI' % thrV, 'EM').addThrValue(thrV)

        ThresholdDefLegacy.addVaryingThrValuesRun2( LegacyThreshold( 'EM15VHI', 'EM'), 15, shift_set = 2 )
        ThresholdDefLegacy.addVaryingThrValuesRun2( LegacyThreshold( 'EM18VHI', 'EM'), 18, shift_set = 3 )
        ThresholdDefLegacy.addVaryingThrValuesRun2( LegacyThreshold( 'EM20VHI', 'EM'), 20, shift_set = 3 )
        ThresholdDefLegacy.addVaryingThrValuesRun2( LegacyThreshold( 'EM22VHI', 'EM'), 22, shift_set = 3 )
        ThresholdDefLegacy.addVaryingThrValuesRun2( LegacyThreshold( 'EM24VHI', 'EM'), 24, shift_set = 3 )
        ThresholdDefLegacy.addVaryingThrValuesRun2( LegacyThreshold( 'EM26VHI', 'EM'), 26, shift_set = 3 ) 
        ThresholdDefLegacy.addVaryingThrValuesRun2( LegacyThreshold( 'EM30VHI', 'EM'), 30, shift_set = 3 )

        # VHIM section
        ThresholdValue.setDefaults('EM', {'isobits' : '01000', 'use_relIso' : True })
        ThresholdDefLegacy.addVaryingThrValuesRun2( LegacyThreshold( 'EM24VHIM', 'EM'), 24, shift_set = 3 )

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

        # Standart jet (-31<=eta<=31)
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
        for (thrV, etamax) in [(12,25), (12,28), (15,25), (17,22), (20,28), (25,23), (35,23), (20,49), (30,49), (40,25), (45,21)]:
            LegacyThreshold('J%ip0ETA%i'  % (thrV, etamax), 'JET').addThrValue(CL.JetOff).addThrValue( thrV, etamin = -etamax,  etamax = etamax, priority=1)  

        # Standard forward jet
        for thrV in [10, 15, 20, 25, 30, 35, 45, 50, 70, 75, 100]:
            LegacyThreshold('J%ip31ETA49' % thrV, 'JET').addThrValue(CL.JetOff)\
                                                        .addThrValue( thrV, etamin=31,  etamax=49, priority=1)\
                                                        .addThrValue( thrV, etamin=-49, etamax=-31, priority=1)

        # Custom Forward jet and VBF jets
        for (thrV, etamin, etamax) in [ (15,23,49), (15,24,49), (20,28,31) ]:
            LegacyThreshold('J%ip%iETA%i' % (thrV, etamin,etamax), 'JET').addThrValue(CL.JetOff)\
                                                                         .addThrValue( thrV, etamin  =  etamin, etamax=etamax, priority=1)\
                                                                         .addThrValue( thrV, etamin = -etamax, etamax = -etamin, priority=1)

        ##4x4 jet windows for forward electrons
        ThresholdValue.setDefaults('JET', {'window' : 4})
        
        for thrV in [15]:
            LegacyThreshold('JJ%ip23ETA49' % thrV, 'JET')\
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
            LegacyThreshold('TE%ip0ETA%i' % (thrV, etamax), 'TE').addThrValue(CL.EtSumOff).addThrValue( thrV, etamin = -etamax, etamax = etamax, priority=1)
        
        # Restricted range TE 2.4<|eta|<4.9
        etamin = 24
        etamax = 49
        for thrV in [0, 3, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 80, 110, 150, 180, 1000, 2000, 5000, 6500, 8000, 9000]:
            LegacyThreshold('TE%ip%iETA%i' % (thrV, etamin, etamax), 'TE')\
              .addThrValue( CL.EtSumOff)\
              .addThrValue( thrV, etamin  =  etamin, etamax=etamax, priority=1)\
              .addThrValue( thrV, etamin = -etamax, etamax = -etamin, priority=1)

        # RXE (restriced range ET miss)
        etamax = 24
        for thrV in [30, 35, 40, 45, 50, 55, 60, 70, 80]:
            LegacyThreshold('XE%ip0ETA%i'    % (thrV, etamax), 'XE').addThrValue(CL.EtMissOff).addThrValue( thrV, etamin = -etamax, etamax = etamax, priority=1) 

        # Restricted range TE |eta|<4.9
        etamax = 49
        for thrV in [3, 7, 500, 600, 1500, 3000, 3500, 5000, 6500, 8000, 9000]:
            LegacyThreshold('TE%ip0ETA%i' % (thrV, etamax), 'TE').addThrValue(CL.EtSumOff).addThrValue( thrV, etamin = -etamax, etamax = etamax, priority=1)

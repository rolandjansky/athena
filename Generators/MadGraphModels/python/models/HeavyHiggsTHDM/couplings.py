# This file was automatically created by FeynRules 1.7.214
# Mathematica version: 8.0 for Linux x86 (64-bit) (October 10, 2011)
# Date: Thu 3 Oct 2013 17:05:47


from object_library import all_couplings, Coupling

from function_library import complexconjugate, re, im, csc, sec, acsc, asec, cot



GC_1 = Coupling(name = 'GC_1',
                value = '-(ee*complex(0,1))/3.',
                order = {'QED':1})

GC_2 = Coupling(name = 'GC_2',
                value = '(2*ee*complex(0,1))/3.',
                order = {'QED':1})

GC_3 = Coupling(name = 'GC_3',
                value = '-(ee*complex(0,1))',
                order = {'QED':1})

GC_4 = Coupling(name = 'GC_4',
                value = 'ee*complex(0,1)',
                order = {'QED':1})

GC_5 = Coupling(name = 'GC_5',
                value = 'ee**2*complex(0,1)',
                order = {'QED':2})

GC_6 = Coupling(name = 'GC_6',
                value = '-G',
                order = {'QCD':1})

GC_7 = Coupling(name = 'GC_7',
                value = 'complex(0,1)*G',
                order = {'QCD':1})

GC_8 = Coupling(name = 'GC_8',
                value = 'complex(0,1)*G**2',
                order = {'QCD':2})

GC_9 = Coupling(name = 'GC_9',
                value = '-6*complex(0,1)*lam',
                order = {'QED':2})

GC_10 = Coupling(name = 'GC_10',
                 value = '(ee**2*complex(0,1))/(2.*sw**2)',
                 order = {'QED':2})

GC_11 = Coupling(name = 'GC_11',
                 value = '-((ee**2*complex(0,1))/sw**2)',
                 order = {'QED':2})

GC_12 = Coupling(name = 'GC_12',
                 value = '(cw**2*ee**2*complex(0,1))/sw**2',
                 order = {'QED':2})

GC_13 = Coupling(name = 'GC_13',
                 value = '(ee*complex(0,1))/(sw*cmath.sqrt(2))',
                 order = {'QED':1})

GC_14 = Coupling(name = 'GC_14',
                 value = '-(cw*ee*complex(0,1))/(2.*sw)',
                 order = {'QED':1})

GC_15 = Coupling(name = 'GC_15',
                 value = '(cw*ee*complex(0,1))/(2.*sw)',
                 order = {'QED':1})

GC_16 = Coupling(name = 'GC_16',
                 value = '(cw*ee*complex(0,1))/sw',
                 order = {'QED':1})

GC_17 = Coupling(name = 'GC_17',
                 value = '(-2*cw*ee**2*complex(0,1))/sw',
                 order = {'QED':2})

GC_18 = Coupling(name = 'GC_18',
                 value = '-(ee*complex(0,1)*sw)/(6.*cw)',
                 order = {'QED':1})

GC_19 = Coupling(name = 'GC_19',
                 value = '(ee*complex(0,1)*sw)/(2.*cw)',
                 order = {'QED':1})

GC_20 = Coupling(name = 'GC_20',
                 value = '(cw*ee*complex(0,1))/(2.*sw) + (ee*complex(0,1)*sw)/(2.*cw)',
                 order = {'QED':1})

GC_21 = Coupling(name = 'GC_21',
                 value = 'ee**2*complex(0,1) + (cw**2*ee**2*complex(0,1))/(2.*sw**2) + (ee**2*complex(0,1)*sw**2)/(2.*cw**2)',
                 order = {'QED':2})

GC_22 = Coupling(name = 'GC_22',
                 value = '-(aS*complex(0,1))/(3.*cmath.pi*vev**2)*(-3./2.)*cy**2',
                 order = {'QCD':2,'QED':2})

GC_23 = Coupling(name = 'GC_23',
                 value = '-(aS*G)/(3.*cmath.pi*vev**2)',
                 order = {'QCD':3,'QED':2})

GC_24 = Coupling(name = 'GC_24',
                 value = '(aS*complex(0,1)*G**2)/(3.*cmath.pi*vev**2)',
                 order = {'QCD':4,'QED':2})

GC_25 = Coupling(name = 'GC_25',
                 value = '(aS*complex(0,1))/(3.*cmath.pi*vev)*(3./2.)*cy',
                 order = {'QCD':2,'QED':1})

GC_65 = Coupling(name = 'GC_65',
                 value = '(aS*complex(0,1))/(3.*cmath.pi*vev)*(3./2.)*cyH',
                 order = {'QCD':2,'QED':1})

GC_26 = Coupling(name = 'GC_26',
                 value = '(aS*G)/(3.*cmath.pi*vev)',
                 order = {'QCD':3,'QED':1})

GC_27 = Coupling(name = 'GC_27',
                 value = '-(aS*complex(0,1)*G**2)/(3.*cmath.pi*vev)',
                 order = {'QCD':4,'QED':1})

GC_28 = Coupling(name = 'GC_28',
                 value = '-6*complex(0,1)*lam*vev*ctr',
                 order = {'QED':1})

GC_35 = Coupling(name = 'GC_35',
                 value = '-6*complex(0,1)*lam*vev*ctrH',
                 order = {'QED':1})

GC_29 = Coupling(name = 'GC_29',
                 value = '(ee**2*complex(0,1)*vev)/(2.*sw**2)',
                 order = {'QED':1})

GC_30 = Coupling(name = 'GC_30',
                 value = 'ee**2*complex(0,1)*vev + (cw**2*ee**2*complex(0,1)*vev)/(2.*sw**2) + (ee**2*complex(0,1)*sw**2*vev)/(2.*cw**2)',
                 order = {'QED':1})

GC_31 = Coupling(name = 'GC_31',
                 value = '-((complex(0,1)*yb)/cmath.sqrt(2))',
                 order = {'QED':1})

GC_32 = Coupling(name = 'GC_32',
                 value = '-((complex(0,1)*yt)/cmath.sqrt(2))',
                 order = {'QED':1})

GC_33 = Coupling(name = 'GC_33',
                 value = '-((complex(0,1)*ytau)/cmath.sqrt(2))',
                 order = {'QED':1})

GC_34 = Coupling(name = 'GC_34',
                 value = '-(aS*complex(0,1))/(3.*cmath.pi*vev**2)*(-3./2.)*cy**2',
                 order = {'QCD':2,'QED':2})

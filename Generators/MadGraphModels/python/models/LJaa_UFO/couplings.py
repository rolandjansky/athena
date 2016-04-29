# This file was automatically created by FeynRules 2.3.4
# Mathematica version: 9.0 for Mac OS X x86 (64-bit) (November 20, 2012)
# Date: Sun 24 Jan 2016 21:45:21


from object_library import all_couplings, Coupling

from function_library import complexconjugate, re, im, csc, sec, acsc, asec, cot



GC_1 = Coupling(name = 'GC_1',
                value = '2*complex(0,1)*GHD1ZD',
                order = {'DS':1})

GC_2 = Coupling(name = 'GC_2',
                value = '2*complex(0,1)*GHND12',
                order = {'QED':1})

GC_3 = Coupling(name = 'GC_3',
                value = '(complex(0,1)*gL)/cmath.sqrt(2)',
                order = {'QED':1})

GC_4 = Coupling(name = 'GC_4',
                value = '-(complex(0,1)*gL**2)',
                order = {'QED':2})

GC_5 = Coupling(name = 'GC_5',
                value = '(cV*complex(0,1)*gL**2)/2.',
                order = {'HIG':1,'QED':2})

GC_6 = Coupling(name = 'GC_6',
                value = '2*complex(0,1)*GND12HD1',
                order = {'DS':1})

GC_7 = Coupling(name = 'GC_7',
                value = '2*GND12ZD',
                order = {'DS':1})

GC_8 = Coupling(name = 'GC_8',
                value = '-gs',
                order = {'QCD':1})

GC_9 = Coupling(name = 'GC_9',
                value = 'complex(0,1)*gs',
                order = {'QCD':1})

GC_10 = Coupling(name = 'GC_10',
                 value = 'complex(0,1)*gs**2',
                 order = {'QCD':2})

GC_11 = Coupling(name = 'GC_11',
                 value = '(complex(0,1)*gL**4)/(gL**2 + gY**2)',
                 order = {'QED':2})

GC_12 = Coupling(name = 'GC_12',
                 value = '(-2*complex(0,1)*gL**3*gY)/(gL**2 + gY**2)',
                 order = {'QED':2})

GC_13 = Coupling(name = 'GC_13',
                 value = '(complex(0,1)*gL**2*gY**2)/(gL**2 + gY**2)',
                 order = {'QED':2})

GC_14 = Coupling(name = 'GC_14',
                 value = '(complex(0,1)*gL**2)/cmath.sqrt(gL**2 + gY**2)',
                 order = {'QED':1})

GC_15 = Coupling(name = 'GC_15',
                 value = '-(complex(0,1)*gL*gY)/(3.*cmath.sqrt(gL**2 + gY**2))',
                 order = {'QED':1})

GC_16 = Coupling(name = 'GC_16',
                 value = '(2*complex(0,1)*gL*gY)/(3.*cmath.sqrt(gL**2 + gY**2))',
                 order = {'QED':1})

GC_17 = Coupling(name = 'GC_17',
                 value = '-((complex(0,1)*gL*gY)/cmath.sqrt(gL**2 + gY**2))',
                 order = {'QED':1})

GC_18 = Coupling(name = 'GC_18',
                 value = '(complex(0,1)*gL*gY)/cmath.sqrt(gL**2 + gY**2)',
                 order = {'QED':1})

GC_19 = Coupling(name = 'GC_19',
                 value = '(complex(0,1)*gY**2)/(3.*cmath.sqrt(gL**2 + gY**2))',
                 order = {'QED':1})

GC_20 = Coupling(name = 'GC_20',
                 value = '(-2*complex(0,1)*gY**2)/(3.*cmath.sqrt(gL**2 + gY**2))',
                 order = {'QED':1})

GC_21 = Coupling(name = 'GC_21',
                 value = '(complex(0,1)*gY**2)/cmath.sqrt(gL**2 + gY**2)',
                 order = {'QED':1})

GC_22 = Coupling(name = 'GC_22',
                 value = '(complex(0,1)*cmath.sqrt(gL**2 + gY**2))/2.',
                 order = {'QED':1})

GC_23 = Coupling(name = 'GC_23',
                 value = '(cV*complex(0,1)*gL**2)/2. + (cV*complex(0,1)*gY**2)/2.',
                 order = {'HIG':1,'QED':2})

GC_24 = Coupling(name = 'GC_24',
                 value = '-(complex(0,1)*gL**2)/(2.*cmath.sqrt(gL**2 + gY**2)) - (complex(0,1)*gY**2)/(6.*cmath.sqrt(gL**2 + gY**2))',
                 order = {'QED':1})

GC_25 = Coupling(name = 'GC_25',
                 value = '(complex(0,1)*gL**2)/(2.*cmath.sqrt(gL**2 + gY**2)) - (complex(0,1)*gY**2)/(6.*cmath.sqrt(gL**2 + gY**2))',
                 order = {'QED':1})

GC_26 = Coupling(name = 'GC_26',
                 value = '-(complex(0,1)*gL**2)/(2.*cmath.sqrt(gL**2 + gY**2)) + (complex(0,1)*gY**2)/(2.*cmath.sqrt(gL**2 + gY**2))',
                 order = {'QED':1})

GC_27 = Coupling(name = 'GC_27',
                 value = 'complex(0,1)*GZDE',
                 order = {'QED':1})

GC_28 = Coupling(name = 'GC_28',
                 value = 'complex(0,1)*GZDMU',
                 order = {'QED':1})

GC_29 = Coupling(name = 'GC_29',
                 value = '-(complex(0,1)*GZDPI)',
                 order = {'QED':1})

GC_30 = Coupling(name = 'GC_30',
                 value = '(cgg*complex(0,1)*gs**2)/v',
                 order = {'HIG':1,'QCD':2})

GC_31 = Coupling(name = 'GC_31',
                 value = '(cgg*gs**3)/v',
                 order = {'HIG':1,'QCD':3})

GC_32 = Coupling(name = 'GC_32',
                 value = '-((cgg*complex(0,1)*gs**4)/v)',
                 order = {'HIG':1,'QCD':4})

GC_33 = Coupling(name = 'GC_33',
                 value = '-((cd*complex(0,1)*mB)/v)',
                 order = {'HIG':1})

GC_34 = Coupling(name = 'GC_34',
                 value = '-((cu*complex(0,1)*mT)/v)',
                 order = {'HIG':1})

GC_35 = Coupling(name = 'GC_35',
                 value = '-((cl*complex(0,1)*mTau)/v)',
                 order = {'HIG':1})

GC_36 = Coupling(name = 'GC_36',
                 value = '(cV*complex(0,1)*gL**2*v)/2.',
                 order = {'HIG':1,'QED':2})

GC_37 = Coupling(name = 'GC_37',
                 value = '(cV*complex(0,1)*gL**2*v)/2. + (cV*complex(0,1)*gY**2*v)/2.',
                 order = {'HIG':1,'QED':2})


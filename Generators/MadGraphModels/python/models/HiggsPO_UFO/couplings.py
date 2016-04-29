# This file was automatically created by FeynRules 2.3.1
# Mathematica version: 10.0 for Mac OS X x86 (64-bit) (September 10, 2014)
# Date: Sat 4 Jul 2015 04:38:51


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
                value = '-G',
                order = {'QCD':1})

GC_5 = Coupling(name = 'GC_5',
                value = 'complex(0,1)*G',
                order = {'QCD':1})

GC_6 = Coupling(name = 'GC_6',
                value = 'complex(0,1)*G**2',
                order = {'QCD':2})

GC_7 = Coupling(name = 'GC_7',
                value = '(2*complex(0,1)*kWW*MW**2)/vF + (2*complex(0,1)*MW**2*noise)/vF',
                order = {'HPO':1})

GC_8 = Coupling(name = 'GC_8',
                value = '(2*complex(0,1)*kZZ*MZ**2)/vF + (2*complex(0,1)*MZ**2*noise)/vF',
                order = {'HPO':1})

GC_9 = Coupling(name = 'GC_9',
                value = '(-2*eggSM*complex(0,1))/vF',
                order = {'QCD':2})

GC_10 = Coupling(name = 'GC_10',
                 value = '(-2*eWW*complex(0,1))/vF',
                 order = {'HPO':1})

GC_11 = Coupling(name = 'GC_11',
                 value = '(2*eWWCP*complex(0,1))/vF',
                 order = {'HPO':1})

GC_12 = Coupling(name = 'GC_12',
                 value = '(2*eZeL*complex(0,1))/vF',
                 order = {'HPO':1})

GC_13 = Coupling(name = 'GC_13',
                 value = '(2*eZeR*complex(0,1))/vF',
                 order = {'HPO':1})

GC_14 = Coupling(name = 'GC_14',
                 value = '(2*eZmuL*complex(0,1))/vF',
                 order = {'HPO':1})

GC_15 = Coupling(name = 'GC_15',
                 value = '(2*eZmuR*complex(0,1))/vF',
                 order = {'HPO':1})

GC_16 = Coupling(name = 'GC_16',
                 value = '(2*eZtauL*complex(0,1))/vF',
                 order = {'HPO':1})

GC_17 = Coupling(name = 'GC_17',
                 value = '(2*eZtauR*complex(0,1))/vF',
                 order = {'HPO':1})

GC_18 = Coupling(name = 'GC_18',
                 value = '(2*eZv*complex(0,1))/vF',
                 order = {'HPO':1})

GC_19 = Coupling(name = 'GC_19',
                 value = '(-2*eZZ*complex(0,1))/vF',
                 order = {'HPO':1})

GC_20 = Coupling(name = 'GC_20',
                 value = '-((eZZCP*complex(0,1))/vF)',
                 order = {'HPO':1})

GC_21 = Coupling(name = 'GC_21',
                 value = '(-2*eggSM*G)/vF',
                 order = {'QCD':3})

GC_22 = Coupling(name = 'GC_22',
                 value = '(2*eggSM*complex(0,1)*G**2)/vF',
                 order = {'QCD':4})

GC_23 = Coupling(name = 'GC_23',
                 value = '(-2*eAASM*complex(0,1)*kAA)/vF',
                 order = {'HPO':1})

GC_24 = Coupling(name = 'GC_24',
                 value = '(-2*eZASM*complex(0,1)*kZA)/vF',
                 order = {'HPO':1})

GC_25 = Coupling(name = 'GC_25',
                 value = '-((eAASM*complex(0,1)*lAACP)/vF)',
                 order = {'HPO':1})

GC_26 = Coupling(name = 'GC_26',
                 value = '(-2*eZASM*complex(0,1)*lZACP)/vF',
                 order = {'HPO':1})

GC_27 = Coupling(name = 'GC_27',
                 value = '(complex(0,1)*gWe*MW*cmath.sqrt(2))/vF',
                 order = {'QED':1})

GC_28 = Coupling(name = 'GC_28',
                 value = '(complex(0,1)*gWmu*MW*cmath.sqrt(2))/vF',
                 order = {'QED':1})

GC_29 = Coupling(name = 'GC_29',
                 value = '(complex(0,1)*gWtau*MW*cmath.sqrt(2))/vF',
                 order = {'QED':1})

GC_30 = Coupling(name = 'GC_30',
                 value = '(2*complex(0,1)*gZeL*MZ)/vF',
                 order = {'QED':1})

GC_31 = Coupling(name = 'GC_31',
                 value = '(2*complex(0,1)*gZeR*MZ)/vF',
                 order = {'QED':1})

GC_32 = Coupling(name = 'GC_32',
                 value = '(2*complex(0,1)*gZmuL*MZ)/vF',
                 order = {'QED':1})

GC_33 = Coupling(name = 'GC_33',
                 value = '(2*complex(0,1)*gZmuR*MZ)/vF',
                 order = {'QED':1})

GC_34 = Coupling(name = 'GC_34',
                 value = '(2*complex(0,1)*gZtauL*MZ)/vF',
                 order = {'QED':1})

GC_35 = Coupling(name = 'GC_35',
                 value = '(2*complex(0,1)*gZtauR*MZ)/vF',
                 order = {'QED':1})

GC_36 = Coupling(name = 'GC_36',
                 value = '(2*complex(0,1)*gZv*MZ)/vF',
                 order = {'QED':1})

GC_37 = Coupling(name = 'GC_37',
                 value = '-((complex(0,1)*kb*ybeff)/cmath.sqrt(2))',
                 order = {'YUK':1})

GC_38 = Coupling(name = 'GC_38',
                 value = '(lb*ybeff)/cmath.sqrt(2)',
                 order = {'YUK':1})

GC_39 = Coupling(name = 'GC_39',
                 value = '-((complex(0,1)*kc*yceff)/cmath.sqrt(2))',
                 order = {'YUK':1})

GC_40 = Coupling(name = 'GC_40',
                 value = '(lc*yceff)/cmath.sqrt(2)',
                 order = {'YUK':1})

GC_41 = Coupling(name = 'GC_41',
                 value = '-((complex(0,1)*kmu*ymueff)/cmath.sqrt(2))',
                 order = {'YUK':1})

GC_42 = Coupling(name = 'GC_42',
                 value = '(lmu*ymueff)/cmath.sqrt(2)',
                 order = {'YUK':1})

GC_43 = Coupling(name = 'GC_43',
                 value = '-((complex(0,1)*ktau*ytaueff)/cmath.sqrt(2))',
                 order = {'YUK':1})

GC_44 = Coupling(name = 'GC_44',
                 value = '(ltau*ytaueff)/cmath.sqrt(2)',
                 order = {'YUK':1})

GC_45 = Coupling(name = 'GC_45',
                 value = '(2*eWe*complex(0,1)*cmath.cos(phiWe))/vF - (2*eWe*cmath.sin(phiWe))/vF',
                 order = {'HPO':1})

GC_46 = Coupling(name = 'GC_46',
                 value = '(2*eWe*complex(0,1)*cmath.cos(phiWe))/vF + (2*eWe*cmath.sin(phiWe))/vF',
                 order = {'HPO':1})

GC_47 = Coupling(name = 'GC_47',
                 value = '(2*eWmu*complex(0,1)*cmath.cos(phiWmu))/vF - (2*eWmu*cmath.sin(phiWmu))/vF',
                 order = {'HPO':1})

GC_48 = Coupling(name = 'GC_48',
                 value = '(2*eWmu*complex(0,1)*cmath.cos(phiWmu))/vF + (2*eWmu*cmath.sin(phiWmu))/vF',
                 order = {'HPO':1})

GC_49 = Coupling(name = 'GC_49',
                 value = '(2*eWtau*complex(0,1)*cmath.cos(phiWtau))/vF - (2*eWtau*cmath.sin(phiWtau))/vF',
                 order = {'HPO':1})

GC_50 = Coupling(name = 'GC_50',
                 value = '(2*eWtau*complex(0,1)*cmath.cos(phiWtau))/vF + (2*eWtau*cmath.sin(phiWtau))/vF',
                 order = {'HPO':1})


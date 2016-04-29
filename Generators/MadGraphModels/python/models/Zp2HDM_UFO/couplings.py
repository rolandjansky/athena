# This file was automatically created by FeynRules 2.0.25
# Mathematica version: 9.0 for Mac OS X x86 (64-bit) (January 24, 2013)
# Date: Wed 11 Nov 2015 22:20:11


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
                value = '-gx',
                order = {'QED':1})

GC_10 = Coupling(name = 'GC_10',
                 value = '(complex(0,1)*gz)/2.',
                 order = {'QED':1})

GC_11 = Coupling(name = 'GC_11',
                 value = 'gz/2.',
                 order = {'QED':1})

GC_12 = Coupling(name = 'GC_12',
                 value = '(Ep*complex(0,1)*gz)/2.',
                 order = {'QED':1})

GC_13 = Coupling(name = 'GC_13',
                 value = '(Ep*gz)/2.',
                 order = {'QED':1})

GC_14 = Coupling(name = 'GC_14',
                 value = '(complex(0,1)*gz**2)/2.',
                 order = {'QED':2})

GC_15 = Coupling(name = 'GC_15',
                 value = '(Ep*complex(0,1)*gz**2)/2.',
                 order = {'QED':2})

GC_16 = Coupling(name = 'GC_16',
                 value = '-(Cb*I2a33)',
                 order = {'QED':1})

GC_17 = Coupling(name = 'GC_17',
                 value = 'Cb*I3a33',
                 order = {'QED':1})

GC_18 = Coupling(name = 'GC_18',
                 value = '-(I1a33*Sb)',
                 order = {'QED':1})

GC_19 = Coupling(name = 'GC_19',
                 value = 'I4a33*Sb',
                 order = {'QED':1})

GC_20 = Coupling(name = 'GC_20',
                 value = '-(Cb**2*ee*complex(0,1)) - ee*complex(0,1)*Sb**2',
                 order = {'QED':1})

GC_21 = Coupling(name = 'GC_21',
                 value = '2*Cb**2*ee**2*complex(0,1) + 2*ee**2*complex(0,1)*Sb**2',
                 order = {'QED':2})

GC_22 = Coupling(name = 'GC_22',
                 value = '(Ca**2*ee**2*complex(0,1))/(2.*sw**2) + (ee**2*complex(0,1)*Sa**2)/(2.*sw**2)',
                 order = {'QED':2})

GC_23 = Coupling(name = 'GC_23',
                 value = '(Cb**2*ee**2*complex(0,1))/(2.*sw**2) + (ee**2*complex(0,1)*Sb**2)/(2.*sw**2)',
                 order = {'QED':2})

GC_24 = Coupling(name = 'GC_24',
                 value = '-(Ca*Cb*ee**2*Ep)/(2.*cw) - (ee**2*Ep*Sa*Sb)/(2.*cw) - (Ca*Cb*ee*gz)/(2.*sw)',
                 order = {'QED':2})

GC_25 = Coupling(name = 'GC_25',
                 value = '(Ca*Cb*ee**2*Ep)/(2.*cw) + (ee**2*Ep*Sa*Sb)/(2.*cw) + (Ca*Cb*ee*gz)/(2.*sw)',
                 order = {'QED':2})

GC_26 = Coupling(name = 'GC_26',
                 value = '-(Cb**2*ee**2*Ep*complex(0,1))/(2.*cw) - (ee**2*Ep*complex(0,1)*Sb**2)/(2.*cw) - (Cb**2*ee*complex(0,1)*gz)/(2.*sw)',
                 order = {'QED':2})

GC_27 = Coupling(name = 'GC_27',
                 value = '(Ca*Cb*ee**2)/(2.*cw) + (ee**2*Sa*Sb)/(2.*cw) - (Ca*Cb*ee*Ep*gz)/(2.*sw)',
                 order = {'QED':2})

GC_28 = Coupling(name = 'GC_28',
                 value = '-(Ca*Cb*ee**2)/(2.*cw) - (ee**2*Sa*Sb)/(2.*cw) + (Ca*Cb*ee*Ep*gz)/(2.*sw)',
                 order = {'QED':2})

GC_29 = Coupling(name = 'GC_29',
                 value = '(Cb**2*ee**2*complex(0,1))/(2.*cw) + (ee**2*complex(0,1)*Sb**2)/(2.*cw) - (Cb**2*ee*Ep*complex(0,1)*gz)/(2.*sw)',
                 order = {'QED':2})

GC_30 = Coupling(name = 'GC_30',
                 value = '-(Cb*ee**2*Ep*Sa)/(2.*cw) + (Ca*ee**2*Ep*Sb)/(2.*cw) - (Cb*ee*gz*Sa)/(2.*sw)',
                 order = {'QED':2})

GC_31 = Coupling(name = 'GC_31',
                 value = '(Cb*ee**2*Ep*Sa)/(2.*cw) - (Ca*ee**2*Ep*Sb)/(2.*cw) + (Cb*ee*gz*Sa)/(2.*sw)',
                 order = {'QED':2})

GC_32 = Coupling(name = 'GC_32',
                 value = '(Cb*ee**2*Sa)/(2.*cw) - (Ca*ee**2*Sb)/(2.*cw) - (Cb*ee*Ep*gz*Sa)/(2.*sw)',
                 order = {'QED':2})

GC_33 = Coupling(name = 'GC_33',
                 value = '-(Cb*ee**2*Sa)/(2.*cw) + (Ca*ee**2*Sb)/(2.*cw) + (Cb*ee*Ep*gz*Sa)/(2.*sw)',
                 order = {'QED':2})

GC_34 = Coupling(name = 'GC_34',
                 value = '(Cb*ee*Sa)/(2.*sw) - (Ca*ee*Sb)/(2.*sw)',
                 order = {'QED':1})

GC_35 = Coupling(name = 'GC_35',
                 value = '(Cb*ee**2*Sa)/(2.*sw) - (Ca*ee**2*Sb)/(2.*sw)',
                 order = {'QED':2})

GC_36 = Coupling(name = 'GC_36',
                 value = '-(Cb*ee**2*Sa)/(2.*sw) + (Ca*ee**2*Sb)/(2.*sw)',
                 order = {'QED':2})

GC_37 = Coupling(name = 'GC_37',
                 value = '(Ca*Cb*ee)/(2.*sw) + (ee*Sa*Sb)/(2.*sw)',
                 order = {'QED':1})

GC_38 = Coupling(name = 'GC_38',
                 value = '-(Ca*Cb*ee**2)/(2.*sw) - (ee**2*Sa*Sb)/(2.*sw)',
                 order = {'QED':2})

GC_39 = Coupling(name = 'GC_39',
                 value = '(Ca*Cb*ee**2)/(2.*sw) + (ee**2*Sa*Sb)/(2.*sw)',
                 order = {'QED':2})

GC_40 = Coupling(name = 'GC_40',
                 value = '-(Cb**2*ee*complex(0,1))/(2.*sw) - (ee*complex(0,1)*Sb**2)/(2.*sw)',
                 order = {'QED':1})

GC_41 = Coupling(name = 'GC_41',
                 value = '(Cb**2*ee*complex(0,1))/(2.*sw) + (ee*complex(0,1)*Sb**2)/(2.*sw)',
                 order = {'QED':1})

GC_42 = Coupling(name = 'GC_42',
                 value = '-(Cb**2*ee**2*complex(0,1))/(2.*sw) - (ee**2*complex(0,1)*Sb**2)/(2.*sw)',
                 order = {'QED':2})

GC_43 = Coupling(name = 'GC_43',
                 value = '-((ee**2*complex(0,1))/sw**2)',
                 order = {'QED':2})

GC_44 = Coupling(name = 'GC_44',
                 value = '(cw**2*ee**2*complex(0,1))/sw**2',
                 order = {'QED':2})

GC_45 = Coupling(name = 'GC_45',
                 value = '-((cw**2*ee**2*Ep*complex(0,1))/sw**2)',
                 order = {'QED':2})

GC_46 = Coupling(name = 'GC_46',
                 value = '(ee*complex(0,1))/(sw*cmath.sqrt(2))',
                 order = {'QED':1})

GC_47 = Coupling(name = 'GC_47',
                 value = '-(cw*ee*complex(0,1))/(2.*sw)',
                 order = {'QED':1})

GC_48 = Coupling(name = 'GC_48',
                 value = '(cw*ee*complex(0,1))/(2.*sw)',
                 order = {'QED':1})

GC_49 = Coupling(name = 'GC_49',
                 value = '(cw*ee*complex(0,1))/sw',
                 order = {'QED':1})

GC_50 = Coupling(name = 'GC_50',
                 value = '(-2*cw*ee**2*complex(0,1))/sw',
                 order = {'QED':2})

GC_51 = Coupling(name = 'GC_51',
                 value = '-(cw*ee*Ep*complex(0,1))/(2.*sw)',
                 order = {'QED':1})

GC_52 = Coupling(name = 'GC_52',
                 value = '(cw*ee*Ep*complex(0,1))/(2.*sw)',
                 order = {'QED':1})

GC_53 = Coupling(name = 'GC_53',
                 value = '-((cw*ee*Ep*complex(0,1))/sw)',
                 order = {'QED':1})

GC_54 = Coupling(name = 'GC_54',
                 value = '(2*cw*ee**2*Ep*complex(0,1))/sw',
                 order = {'QED':2})

GC_55 = Coupling(name = 'GC_55',
                 value = '-(ee*complex(0,1)*sw)/(6.*cw)',
                 order = {'QED':1})

GC_56 = Coupling(name = 'GC_56',
                 value = '(ee*complex(0,1)*sw)/(2.*cw)',
                 order = {'QED':1})

GC_57 = Coupling(name = 'GC_57',
                 value = '(ee*Ep*complex(0,1)*sw)/(6.*cw)',
                 order = {'QED':1})

GC_58 = Coupling(name = 'GC_58',
                 value = '-(ee*Ep*complex(0,1)*sw)/(2.*cw)',
                 order = {'QED':1})

GC_59 = Coupling(name = 'GC_59',
                 value = '(cw*ee*complex(0,1))/(2.*sw) + (ee*complex(0,1)*sw)/(2.*cw)',
                 order = {'QED':1})

GC_60 = Coupling(name = 'GC_60',
                 value = '-(cw*ee*Ep*complex(0,1))/(2.*sw) - (ee*Ep*complex(0,1)*sw)/(2.*cw)',
                 order = {'QED':1})

GC_61 = Coupling(name = 'GC_61',
                 value = '(Ca**2*complex(0,1)*gz**2)/2. + (Ca**2*cw*ee*Ep*complex(0,1)*gz)/sw + (Ca**2*ee*Ep*complex(0,1)*gz*sw)/cw',
                 order = {'QED':2})

GC_62 = Coupling(name = 'GC_62',
                 value = '(Cb**2*complex(0,1)*gz**2)/2. - (Cb**2*cw*ee*Ep*complex(0,1)*gz)/sw + (Cb**2*ee*Ep*complex(0,1)*gz*sw)/cw',
                 order = {'QED':2})

GC_63 = Coupling(name = 'GC_63',
                 value = '(Cb**2*complex(0,1)*gz**2)/2. + (Cb**2*cw*ee*Ep*complex(0,1)*gz)/sw + (Cb**2*ee*Ep*complex(0,1)*gz*sw)/cw',
                 order = {'QED':2})

GC_64 = Coupling(name = 'GC_64',
                 value = '(Ca*Ep*complex(0,1)*gz**2*Sa)/2. - (Ca*cw*ee*complex(0,1)*gz*Sa)/(2.*sw) - (Ca*ee*complex(0,1)*gz*Sa*sw)/(2.*cw)',
                 order = {'QED':2})

GC_65 = Coupling(name = 'GC_65',
                 value = '-((Ca*cw*ee*Ep*complex(0,1)*gz*Sa)/sw) - (Ca*ee*Ep*complex(0,1)*gz*Sa*sw)/cw',
                 order = {'QED':2})

GC_66 = Coupling(name = 'GC_66',
                 value = '(Ca*complex(0,1)*gz**2*Sa)/2. + (Ca*cw*ee*Ep*complex(0,1)*gz*Sa)/sw + (Ca*ee*Ep*complex(0,1)*gz*Sa*sw)/cw',
                 order = {'QED':2})

GC_67 = Coupling(name = 'GC_67',
                 value = '(complex(0,1)*gz**2*Sa**2)/2. + (cw*ee*Ep*complex(0,1)*gz*Sa**2)/sw + (ee*Ep*complex(0,1)*gz*Sa**2*sw)/cw',
                 order = {'QED':2})

GC_68 = Coupling(name = 'GC_68',
                 value = '(Cb*Ep*gz*Sa)/2. - (Cb*cw*ee*Sa)/(2.*sw) + (Ca*cw*ee*Sb)/(2.*sw) - (Cb*ee*Sa*sw)/(2.*cw) + (Ca*ee*Sb*sw)/(2.*cw)',
                 order = {'QED':1})

GC_69 = Coupling(name = 'GC_69',
                 value = '(Cb*gz*Sa)/2. + (Cb*cw*ee*Ep*Sa)/(2.*sw) - (Ca*cw*ee*Ep*Sb)/(2.*sw) + (Cb*ee*Ep*Sa*sw)/(2.*cw) - (Ca*ee*Ep*Sb*sw)/(2.*cw)',
                 order = {'QED':1})

GC_70 = Coupling(name = 'GC_70',
                 value = '(Ca*Cb*Ep*gz)/2. - (Ca*Cb*cw*ee)/(2.*sw) - (cw*ee*Sa*Sb)/(2.*sw) - (Ca*Cb*ee*sw)/(2.*cw) - (ee*Sa*Sb*sw)/(2.*cw)',
                 order = {'QED':1})

GC_71 = Coupling(name = 'GC_71',
                 value = '(Ca*Cb*gz)/2. + (Ca*Cb*cw*ee*Ep)/(2.*sw) + (cw*ee*Ep*Sa*Sb)/(2.*sw) + (Ca*Cb*ee*Ep*sw)/(2.*cw) + (ee*Ep*Sa*Sb*sw)/(2.*cw)',
                 order = {'QED':1})

GC_72 = Coupling(name = 'GC_72',
                 value = '-(Cb**2*Ep*complex(0,1)*gz)/2. - (Cb**2*cw*ee*complex(0,1))/(2.*sw) - (cw*ee*complex(0,1)*Sb**2)/(2.*sw) + (Cb**2*ee*complex(0,1)*sw)/(2.*cw) + (ee*complex(0,1)*Sb**2*sw)/(2.*cw)',
                 order = {'QED':1})

GC_73 = Coupling(name = 'GC_73',
                 value = 'Cb**2*ee*Ep*complex(0,1)*gz + (Cb**2*cw*ee**2*complex(0,1))/sw + (cw*ee**2*complex(0,1)*Sb**2)/sw - (Cb**2*ee**2*complex(0,1)*sw)/cw - (ee**2*complex(0,1)*Sb**2*sw)/cw',
                 order = {'QED':2})

GC_74 = Coupling(name = 'GC_74',
                 value = '-(Cb**2*complex(0,1)*gz)/2. + (Cb**2*cw*ee*Ep*complex(0,1))/(2.*sw) + (cw*ee*Ep*complex(0,1)*Sb**2)/(2.*sw) - (Cb**2*ee*Ep*complex(0,1)*sw)/(2.*cw) - (ee*Ep*complex(0,1)*Sb**2*sw)/(2.*cw)',
                 order = {'QED':1})

GC_75 = Coupling(name = 'GC_75',
                 value = 'Cb**2*ee*complex(0,1)*gz - (Cb**2*cw*ee**2*Ep*complex(0,1))/sw - (cw*ee**2*Ep*complex(0,1)*Sb**2)/sw + (Cb**2*ee**2*Ep*complex(0,1)*sw)/cw + (ee**2*Ep*complex(0,1)*Sb**2*sw)/cw',
                 order = {'QED':2})

GC_76 = Coupling(name = 'GC_76',
                 value = 'Ca**2*ee**2*complex(0,1) + ee**2*complex(0,1)*Sa**2 + (Ca**2*cw**2*ee**2*complex(0,1))/(2.*sw**2) + (cw**2*ee**2*complex(0,1)*Sa**2)/(2.*sw**2) - (Ca**2*cw*ee*Ep*complex(0,1)*gz)/sw - (Ca**2*ee*Ep*complex(0,1)*gz*sw)/cw + (Ca**2*ee**2*complex(0,1)*sw**2)/(2.*cw**2) + (ee**2*complex(0,1)*Sa**2*sw**2)/(2.*cw**2)',
                 order = {'QED':2})

GC_77 = Coupling(name = 'GC_77',
                 value = 'Ca**2*ee**2*complex(0,1) + ee**2*complex(0,1)*Sa**2 + (Ca**2*cw**2*ee**2*complex(0,1))/(2.*sw**2) + (cw**2*ee**2*complex(0,1)*Sa**2)/(2.*sw**2) - (cw*ee*Ep*complex(0,1)*gz*Sa**2)/sw - (ee*Ep*complex(0,1)*gz*Sa**2*sw)/cw + (Ca**2*ee**2*complex(0,1)*sw**2)/(2.*cw**2) + (ee**2*complex(0,1)*Sa**2*sw**2)/(2.*cw**2)',
                 order = {'QED':2})

GC_78 = Coupling(name = 'GC_78',
                 value = '-(Ca**2*ee**2*Ep*complex(0,1)) + (Ca**2*Ep*complex(0,1)*gz**2)/2. - ee**2*Ep*complex(0,1)*Sa**2 - (Ca**2*cw**2*ee**2*Ep*complex(0,1))/(2.*sw**2) - (cw**2*ee**2*Ep*complex(0,1)*Sa**2)/(2.*sw**2) - (Ca**2*cw*ee*complex(0,1)*gz)/(2.*sw) - (Ca**2*ee*complex(0,1)*gz*sw)/(2.*cw) - (Ca**2*ee**2*Ep*complex(0,1)*sw**2)/(2.*cw**2) - (ee**2*Ep*complex(0,1)*Sa**2*sw**2)/(2.*cw**2)',
                 order = {'QED':2})

GC_79 = Coupling(name = 'GC_79',
                 value = '-(Ca**2*ee**2*Ep*complex(0,1)) - ee**2*Ep*complex(0,1)*Sa**2 + (Ep*complex(0,1)*gz**2*Sa**2)/2. - (Ca**2*cw**2*ee**2*Ep*complex(0,1))/(2.*sw**2) - (cw**2*ee**2*Ep*complex(0,1)*Sa**2)/(2.*sw**2) - (cw*ee*complex(0,1)*gz*Sa**2)/(2.*sw) - (ee*complex(0,1)*gz*Sa**2*sw)/(2.*cw) - (Ca**2*ee**2*Ep*complex(0,1)*sw**2)/(2.*cw**2) - (ee**2*Ep*complex(0,1)*Sa**2*sw**2)/(2.*cw**2)',
                 order = {'QED':2})

GC_80 = Coupling(name = 'GC_80',
                 value = 'Cb**2*ee**2*complex(0,1) + ee**2*complex(0,1)*Sb**2 + (Cb**2*cw**2*ee**2*complex(0,1))/(2.*sw**2) + (cw**2*ee**2*complex(0,1)*Sb**2)/(2.*sw**2) - (Cb**2*cw*ee*Ep*complex(0,1)*gz)/sw - (Cb**2*ee*Ep*complex(0,1)*gz*sw)/cw + (Cb**2*ee**2*complex(0,1)*sw**2)/(2.*cw**2) + (ee**2*complex(0,1)*Sb**2*sw**2)/(2.*cw**2)',
                 order = {'QED':2})

GC_81 = Coupling(name = 'GC_81',
                 value = '-(Cb**2*ee**2*complex(0,1)) - ee**2*complex(0,1)*Sb**2 + (Cb**2*cw**2*ee**2*complex(0,1))/(2.*sw**2) + (cw**2*ee**2*complex(0,1)*Sb**2)/(2.*sw**2) + (Cb**2*cw*ee*Ep*complex(0,1)*gz)/sw - (Cb**2*ee*Ep*complex(0,1)*gz*sw)/cw + (Cb**2*ee**2*complex(0,1)*sw**2)/(2.*cw**2) + (ee**2*complex(0,1)*Sb**2*sw**2)/(2.*cw**2)',
                 order = {'QED':2})

GC_82 = Coupling(name = 'GC_82',
                 value = '-(Cb**2*ee**2*Ep*complex(0,1)) + (Cb**2*Ep*complex(0,1)*gz**2)/2. - ee**2*Ep*complex(0,1)*Sb**2 - (Cb**2*cw**2*ee**2*Ep*complex(0,1))/(2.*sw**2) - (cw**2*ee**2*Ep*complex(0,1)*Sb**2)/(2.*sw**2) - (Cb**2*cw*ee*complex(0,1)*gz)/(2.*sw) - (Cb**2*ee*complex(0,1)*gz*sw)/(2.*cw) - (Cb**2*ee**2*Ep*complex(0,1)*sw**2)/(2.*cw**2) - (ee**2*Ep*complex(0,1)*Sb**2*sw**2)/(2.*cw**2)',
                 order = {'QED':2})

GC_83 = Coupling(name = 'GC_83',
                 value = 'Cb**2*ee**2*Ep*complex(0,1) + (Cb**2*Ep*complex(0,1)*gz**2)/2. + ee**2*Ep*complex(0,1)*Sb**2 - (Cb**2*cw**2*ee**2*Ep*complex(0,1))/(2.*sw**2) - (cw**2*ee**2*Ep*complex(0,1)*Sb**2)/(2.*sw**2) + (Cb**2*cw*ee*complex(0,1)*gz)/(2.*sw) - (Cb**2*ee*complex(0,1)*gz*sw)/(2.*cw) - (Cb**2*ee**2*Ep*complex(0,1)*sw**2)/(2.*cw**2) - (ee**2*Ep*complex(0,1)*Sb**2*sw**2)/(2.*cw**2)',
                 order = {'QED':2})

GC_84 = Coupling(name = 'GC_84',
                 value = '(complex(0,1)*gz**2*vp)/2.',
                 order = {'QED':1})

GC_85 = Coupling(name = 'GC_85',
                 value = '(Ep*complex(0,1)*gz**2*vp)/2.',
                 order = {'QED':1})

GC_86 = Coupling(name = 'GC_86',
                 value = '-(ee**2*complex(0,1)*Sa*vd)/(2.*sw**2) + (Ca*ee**2*complex(0,1)*vu)/(2.*sw**2)',
                 order = {'QED':1})

GC_87 = Coupling(name = 'GC_87',
                 value = '(Ca*ee**2*complex(0,1)*vd)/(2.*sw**2) + (ee**2*complex(0,1)*Sa*vu)/(2.*sw**2)',
                 order = {'QED':1})

GC_88 = Coupling(name = 'GC_88',
                 value = '(ee**2*Sb*vd)/(2.*sw) - (Cb*ee**2*vu)/(2.*sw)',
                 order = {'QED':1})

GC_89 = Coupling(name = 'GC_89',
                 value = '-(ee**2*Sb*vd)/(2.*sw) + (Cb*ee**2*vu)/(2.*sw)',
                 order = {'QED':1})

GC_90 = Coupling(name = 'GC_90',
                 value = '(ee**2*Ep*Sb*vd)/(2.*cw) - (Cb*ee**2*Ep*vu)/(2.*cw) - (Cb*ee*gz*vu)/(2.*sw)',
                 order = {'QED':1})

GC_91 = Coupling(name = 'GC_91',
                 value = '-(ee**2*Ep*Sb*vd)/(2.*cw) + (Cb*ee**2*Ep*vu)/(2.*cw) + (Cb*ee*gz*vu)/(2.*sw)',
                 order = {'QED':1})

GC_92 = Coupling(name = 'GC_92',
                 value = '-(ee**2*Sb*vd)/(2.*cw) + (Cb*ee**2*vu)/(2.*cw) - (Cb*ee*Ep*gz*vu)/(2.*sw)',
                 order = {'QED':1})

GC_93 = Coupling(name = 'GC_93',
                 value = '(ee**2*Sb*vd)/(2.*cw) - (Cb*ee**2*vu)/(2.*cw) + (Cb*ee*Ep*gz*vu)/(2.*sw)',
                 order = {'QED':1})

GC_94 = Coupling(name = 'GC_94',
                 value = '(Ca*complex(0,1)*gz**2*vu)/2. + (Ca*cw*ee*Ep*complex(0,1)*gz*vu)/sw + (Ca*ee*Ep*complex(0,1)*gz*sw*vu)/cw',
                 order = {'QED':1})

GC_95 = Coupling(name = 'GC_95',
                 value = '(complex(0,1)*gz**2*Sa*vu)/2. + (cw*ee*Ep*complex(0,1)*gz*Sa*vu)/sw + (ee*Ep*complex(0,1)*gz*Sa*sw*vu)/cw',
                 order = {'QED':1})

GC_96 = Coupling(name = 'GC_96',
                 value = '-(ee**2*complex(0,1)*Sa*vd) - (cw**2*ee**2*complex(0,1)*Sa*vd)/(2.*sw**2) - (ee**2*complex(0,1)*Sa*sw**2*vd)/(2.*cw**2) + Ca*ee**2*complex(0,1)*vu + (Ca*cw**2*ee**2*complex(0,1)*vu)/(2.*sw**2) - (Ca*cw*ee*Ep*complex(0,1)*gz*vu)/sw - (Ca*ee*Ep*complex(0,1)*gz*sw*vu)/cw + (Ca*ee**2*complex(0,1)*sw**2*vu)/(2.*cw**2)',
                 order = {'QED':1})

GC_97 = Coupling(name = 'GC_97',
                 value = 'ee**2*Ep*complex(0,1)*Sa*vd + (cw**2*ee**2*Ep*complex(0,1)*Sa*vd)/(2.*sw**2) + (ee**2*Ep*complex(0,1)*Sa*sw**2*vd)/(2.*cw**2) - Ca*ee**2*Ep*complex(0,1)*vu + (Ca*Ep*complex(0,1)*gz**2*vu)/2. - (Ca*cw**2*ee**2*Ep*complex(0,1)*vu)/(2.*sw**2) - (Ca*cw*ee*complex(0,1)*gz*vu)/(2.*sw) - (Ca*ee*complex(0,1)*gz*sw*vu)/(2.*cw) - (Ca*ee**2*Ep*complex(0,1)*sw**2*vu)/(2.*cw**2)',
                 order = {'QED':1})

GC_98 = Coupling(name = 'GC_98',
                 value = 'Ca*ee**2*complex(0,1)*vd + (Ca*cw**2*ee**2*complex(0,1)*vd)/(2.*sw**2) + (Ca*ee**2*complex(0,1)*sw**2*vd)/(2.*cw**2) + ee**2*complex(0,1)*Sa*vu + (cw**2*ee**2*complex(0,1)*Sa*vu)/(2.*sw**2) - (cw*ee*Ep*complex(0,1)*gz*Sa*vu)/sw - (ee*Ep*complex(0,1)*gz*Sa*sw*vu)/cw + (ee**2*complex(0,1)*Sa*sw**2*vu)/(2.*cw**2)',
                 order = {'QED':1})

GC_99 = Coupling(name = 'GC_99',
                 value = '-(Ca*ee**2*Ep*complex(0,1)*vd) - (Ca*cw**2*ee**2*Ep*complex(0,1)*vd)/(2.*sw**2) - (Ca*ee**2*Ep*complex(0,1)*sw**2*vd)/(2.*cw**2) - ee**2*Ep*complex(0,1)*Sa*vu + (Ep*complex(0,1)*gz**2*Sa*vu)/2. - (cw**2*ee**2*Ep*complex(0,1)*Sa*vu)/(2.*sw**2) - (cw*ee*complex(0,1)*gz*Sa*vu)/(2.*sw) - (ee*complex(0,1)*gz*Sa*sw*vu)/(2.*cw) - (ee**2*Ep*complex(0,1)*Sa*sw**2*vu)/(2.*cw**2)',
                 order = {'QED':1})

GC_100 = Coupling(name = 'GC_100',
                  value = '-((Ca*complex(0,1)*yb)/cmath.sqrt(2))',
                  order = {'QED':1})

GC_101 = Coupling(name = 'GC_101',
                  value = '(complex(0,1)*Sa*yb)/cmath.sqrt(2)',
                  order = {'QED':1})

GC_102 = Coupling(name = 'GC_102',
                  value = '(Sb*yb)/cmath.sqrt(2)',
                  order = {'QED':1})

GC_103 = Coupling(name = 'GC_103',
                  value = '-((Ca*complex(0,1)*yt)/cmath.sqrt(2))',
                  order = {'QED':1})

GC_104 = Coupling(name = 'GC_104',
                  value = '(Cb*yt)/cmath.sqrt(2)',
                  order = {'QED':1})

GC_105 = Coupling(name = 'GC_105',
                  value = '-((complex(0,1)*Sa*yt)/cmath.sqrt(2))',
                  order = {'QED':1})

GC_106 = Coupling(name = 'GC_106',
                  value = '-((Ca*complex(0,1)*ytau)/cmath.sqrt(2))',
                  order = {'QED':1})

GC_107 = Coupling(name = 'GC_107',
                  value = '(complex(0,1)*Sa*ytau)/cmath.sqrt(2)',
                  order = {'QED':1})

GC_108 = Coupling(name = 'GC_108',
                  value = '-(Sb*ytau)',
                  order = {'QED':1})

GC_109 = Coupling(name = 'GC_109',
                  value = 'Sb*ytau',
                  order = {'QED':1})

GC_110 = Coupling(name = 'GC_110',
                  value = '(Sb*ytau)/cmath.sqrt(2)',
                  order = {'QED':1})


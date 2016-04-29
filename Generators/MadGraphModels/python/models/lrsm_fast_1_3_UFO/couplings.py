# This file was automatically created by FeynRules 2.1
# Mathematica version: 8.0 for Mac OS X x86 (64-bit) (November 6, 2010)
# Date: Tue 2 Dec 2014 06:46:52


from object_library import all_couplings, Coupling

from function_library import complexconjugate, re, im, csc, sec, acsc, asec, cot



GC_1 = Coupling(name = 'GC_1',
                value = '-gs',
                order = {'QCD':1})

GC_2 = Coupling(name = 'GC_2',
                value = 'complex(0,1)*gs',
                order = {'QCD':1})

GC_3 = Coupling(name = 'GC_3',
                value = 'complex(0,1)*gs**2',
                order = {'QCD':2})

GC_4 = Coupling(name = 'GC_4',
                value = '-(cphi*cw*complex(0,1)*gw)/2.',
                order = {'EWL':1})

GC_5 = Coupling(name = 'GC_5',
                value = 'cphi*cw*cxi**2*complex(0,1)*gw',
                order = {'EWL':1})

GC_6 = Coupling(name = 'GC_6',
                value = 'cphi**2*cw**2*cxi**2*complex(0,1)*gw**2',
                order = {'EWL':2})

GC_7 = Coupling(name = 'GC_7',
                value = '-(cxi**4*complex(0,1)*gw**2)',
                order = {'EWL':2})

GC_8 = Coupling(name = 'GC_8',
                value = '-(cxi**4*complex(0,1)*gwR**2)',
                order = {'EWR':2})

GC_9 = Coupling(name = 'GC_9',
                value = '-(cw*complex(0,1)*gw*sphi)/2.',
                order = {'EWR':1})

GC_10 = Coupling(name = 'GC_10',
                 value = 'cw*cxi**2*complex(0,1)*gw*sphi',
                 order = {'EWR':1})

GC_11 = Coupling(name = 'GC_11',
                 value = 'cphi*cw**2*cxi**2*complex(0,1)*gw**2*sphi',
                 order = {'EWL':1,'EWR':1})

GC_12 = Coupling(name = 'GC_12',
                 value = 'cw**2*cxi**2*complex(0,1)*gw**2*sphi**2',
                 order = {'EWR':2})

GC_13 = Coupling(name = 'GC_13',
                 value = '-(cphi*complex(0,1)*g1*sw)/(6.*cw)',
                 order = {'QED':1})

GC_14 = Coupling(name = 'GC_14',
                 value = '(cphi*complex(0,1)*g1*sw)/(2.*cw)',
                 order = {'QED':1})

GC_15 = Coupling(name = 'GC_15',
                 value = '(complex(0,1)*gw*sw)/6.',
                 order = {'EWL':1})

GC_16 = Coupling(name = 'GC_16',
                 value = '-(complex(0,1)*gw*sw)/2.',
                 order = {'EWL':1})

GC_17 = Coupling(name = 'GC_17',
                 value = '(2*complex(0,1)*gw*sw)/3.',
                 order = {'EWL':1})

GC_18 = Coupling(name = 'GC_18',
                 value = '-(complex(0,1)*gw*sw)',
                 order = {'EWL':1})

GC_19 = Coupling(name = 'GC_19',
                 value = 'cxi**2*complex(0,1)*gw*sw',
                 order = {'EWL':1})

GC_20 = Coupling(name = 'GC_20',
                 value = '-2*cphi*cw*cxi**2*complex(0,1)*gw**2*sw',
                 order = {'EWL':2})

GC_21 = Coupling(name = 'GC_21',
                 value = '-(complex(0,1)*gwR*sw)/2.',
                 order = {'EWR':1})

GC_22 = Coupling(name = 'GC_22',
                 value = '(complex(0,1)*gwR*sw)/2.',
                 order = {'EWR':1})

GC_23 = Coupling(name = 'GC_23',
                 value = 'cxi**2*complex(0,1)*gwR*sw',
                 order = {'EWR':1})

GC_24 = Coupling(name = 'GC_24',
                 value = '(cphi*complex(0,1)*g1*I1a11*sw)/(2.*cw)',
                 order = {'QED':1})

GC_25 = Coupling(name = 'GC_25',
                 value = '(cphi*complex(0,1)*g1*I1a22*sw)/(2.*cw)',
                 order = {'QED':1})

GC_26 = Coupling(name = 'GC_26',
                 value = '(cphi*complex(0,1)*g1*I1a33*sw)/(2.*cw)',
                 order = {'QED':1})

GC_27 = Coupling(name = 'GC_27',
                 value = '-(cphi*complex(0,1)*g1*I2a11*sw)/(2.*cw)',
                 order = {'QED':1})

GC_28 = Coupling(name = 'GC_28',
                 value = '-(cphi*complex(0,1)*g1*I2a22*sw)/(2.*cw)',
                 order = {'QED':1})

GC_29 = Coupling(name = 'GC_29',
                 value = '-(cphi*complex(0,1)*g1*I2a33*sw)/(2.*cw)',
                 order = {'QED':1})

GC_30 = Coupling(name = 'GC_30',
                 value = '-(cphi*complex(0,1)*g1*I3a11*sw)/(2.*cw)',
                 order = {'QED':1})

GC_31 = Coupling(name = 'GC_31',
                 value = '(complex(0,1)*gw*I3a11*sw)/2.',
                 order = {'EWL':1})

GC_32 = Coupling(name = 'GC_32',
                 value = '-(complex(0,1)*gwR*I3a11*sw)/2.',
                 order = {'EWR':1})

GC_33 = Coupling(name = 'GC_33',
                 value = '-(cphi*complex(0,1)*g1*I3a22*sw)/(2.*cw)',
                 order = {'QED':1})

GC_34 = Coupling(name = 'GC_34',
                 value = '(complex(0,1)*gw*I3a22*sw)/2.',
                 order = {'EWL':1})

GC_35 = Coupling(name = 'GC_35',
                 value = '-(complex(0,1)*gwR*I3a22*sw)/2.',
                 order = {'EWR':1})

GC_36 = Coupling(name = 'GC_36',
                 value = '-(cphi*complex(0,1)*g1*I3a33*sw)/(2.*cw)',
                 order = {'QED':1})

GC_37 = Coupling(name = 'GC_37',
                 value = '(complex(0,1)*gw*I3a33*sw)/2.',
                 order = {'EWL':1})

GC_38 = Coupling(name = 'GC_38',
                 value = '-(complex(0,1)*gwR*I3a33*sw)/2.',
                 order = {'EWR':1})

GC_39 = Coupling(name = 'GC_39',
                 value = '(cphi*complex(0,1)*g1*I4a11*sw)/(2.*cw)',
                 order = {'QED':1})

GC_40 = Coupling(name = 'GC_40',
                 value = '-(complex(0,1)*gw*I4a11*sw)/2.',
                 order = {'EWL':1})

GC_41 = Coupling(name = 'GC_41',
                 value = '(complex(0,1)*gwR*I4a11*sw)/2.',
                 order = {'EWR':1})

GC_42 = Coupling(name = 'GC_42',
                 value = '(cphi*complex(0,1)*g1*I4a22*sw)/(2.*cw)',
                 order = {'QED':1})

GC_43 = Coupling(name = 'GC_43',
                 value = '-(complex(0,1)*gw*I4a22*sw)/2.',
                 order = {'EWL':1})

GC_44 = Coupling(name = 'GC_44',
                 value = '(complex(0,1)*gwR*I4a22*sw)/2.',
                 order = {'EWR':1})

GC_45 = Coupling(name = 'GC_45',
                 value = '(cphi*complex(0,1)*g1*I4a33*sw)/(2.*cw)',
                 order = {'QED':1})

GC_46 = Coupling(name = 'GC_46',
                 value = '-(complex(0,1)*gw*I4a33*sw)/2.',
                 order = {'EWL':1})

GC_47 = Coupling(name = 'GC_47',
                 value = '(complex(0,1)*gwR*I4a33*sw)/2.',
                 order = {'EWR':1})

GC_48 = Coupling(name = 'GC_48',
                 value = '(complex(0,1)*g1*sphi*sw)/(6.*cw)',
                 order = {'EWL':-1,'EWR':1,'QED':1})

GC_49 = Coupling(name = 'GC_49',
                 value = '-(complex(0,1)*g1*sphi*sw)/(2.*cw)',
                 order = {'EWL':-1,'EWR':1,'QED':1})

GC_50 = Coupling(name = 'GC_50',
                 value = '-2*cw*cxi**2*complex(0,1)*gw**2*sphi*sw',
                 order = {'EWL':1,'EWR':1})

GC_51 = Coupling(name = 'GC_51',
                 value = '-(complex(0,1)*g1*I1a11*sphi*sw)/(2.*cw)',
                 order = {'EWL':-1,'EWR':1,'QED':1})

GC_52 = Coupling(name = 'GC_52',
                 value = '-(complex(0,1)*g1*I1a22*sphi*sw)/(2.*cw)',
                 order = {'EWL':-1,'EWR':1,'QED':1})

GC_53 = Coupling(name = 'GC_53',
                 value = '-(complex(0,1)*g1*I1a33*sphi*sw)/(2.*cw)',
                 order = {'EWL':-1,'EWR':1,'QED':1})

GC_54 = Coupling(name = 'GC_54',
                 value = '(complex(0,1)*g1*I2a11*sphi*sw)/(2.*cw)',
                 order = {'EWL':-1,'EWR':1,'QED':1})

GC_55 = Coupling(name = 'GC_55',
                 value = '(complex(0,1)*g1*I2a22*sphi*sw)/(2.*cw)',
                 order = {'EWL':-1,'EWR':1,'QED':1})

GC_56 = Coupling(name = 'GC_56',
                 value = '(complex(0,1)*g1*I2a33*sphi*sw)/(2.*cw)',
                 order = {'EWL':-1,'EWR':1,'QED':1})

GC_57 = Coupling(name = 'GC_57',
                 value = '(complex(0,1)*g1*I3a11*sphi*sw)/(2.*cw)',
                 order = {'EWL':-1,'EWR':1,'QED':1})

GC_58 = Coupling(name = 'GC_58',
                 value = '(complex(0,1)*g1*I3a22*sphi*sw)/(2.*cw)',
                 order = {'EWL':-1,'EWR':1,'QED':1})

GC_59 = Coupling(name = 'GC_59',
                 value = '(complex(0,1)*g1*I3a33*sphi*sw)/(2.*cw)',
                 order = {'EWL':-1,'EWR':1,'QED':1})

GC_60 = Coupling(name = 'GC_60',
                 value = '-(complex(0,1)*g1*I4a11*sphi*sw)/(2.*cw)',
                 order = {'EWL':-1,'EWR':1,'QED':1})

GC_61 = Coupling(name = 'GC_61',
                 value = '-(complex(0,1)*g1*I4a22*sphi*sw)/(2.*cw)',
                 order = {'EWL':-1,'EWR':1,'QED':1})

GC_62 = Coupling(name = 'GC_62',
                 value = '-(complex(0,1)*g1*I4a33*sphi*sw)/(2.*cw)',
                 order = {'EWL':-1,'EWR':1,'QED':1})

GC_63 = Coupling(name = 'GC_63',
                 value = '-(cphi*complex(0,1)*gw*sw**2)/(6.*cw)',
                 order = {'EWL':1})

GC_64 = Coupling(name = 'GC_64',
                 value = '(cphi*complex(0,1)*gw*sw**2)/(2.*cw)',
                 order = {'EWL':1})

GC_65 = Coupling(name = 'GC_65',
                 value = 'cxi**2*complex(0,1)*gw**2*sw**2',
                 order = {'EWL':2})

GC_66 = Coupling(name = 'GC_66',
                 value = '-(cphi*complex(0,1)*gwR*sw**2)/(2.*cw)',
                 order = {'EWR':1})

GC_67 = Coupling(name = 'GC_67',
                 value = '(cphi*complex(0,1)*gwR*sw**2)/(2.*cw)',
                 order = {'EWR':1})

GC_68 = Coupling(name = 'GC_68',
                 value = '-((cphi*cxi**2*complex(0,1)*gwR*sw**2)/cw)',
                 order = {'EWR':1})

GC_69 = Coupling(name = 'GC_69',
                 value = '-(cphi*complex(0,1)*gw*I3a11*sw**2)/(2.*cw)',
                 order = {'EWL':1})

GC_70 = Coupling(name = 'GC_70',
                 value = '(cphi*complex(0,1)*gwR*I3a11*sw**2)/(2.*cw)',
                 order = {'EWR':1})

GC_71 = Coupling(name = 'GC_71',
                 value = '-(cphi*complex(0,1)*gw*I3a22*sw**2)/(2.*cw)',
                 order = {'EWL':1})

GC_72 = Coupling(name = 'GC_72',
                 value = '(cphi*complex(0,1)*gwR*I3a22*sw**2)/(2.*cw)',
                 order = {'EWR':1})

GC_73 = Coupling(name = 'GC_73',
                 value = '-(cphi*complex(0,1)*gw*I3a33*sw**2)/(2.*cw)',
                 order = {'EWL':1})

GC_74 = Coupling(name = 'GC_74',
                 value = '(cphi*complex(0,1)*gwR*I3a33*sw**2)/(2.*cw)',
                 order = {'EWR':1})

GC_75 = Coupling(name = 'GC_75',
                 value = '(cphi*complex(0,1)*gw*I4a11*sw**2)/(2.*cw)',
                 order = {'EWL':1})

GC_76 = Coupling(name = 'GC_76',
                 value = '-(cphi*complex(0,1)*gwR*I4a11*sw**2)/(2.*cw)',
                 order = {'EWR':1})

GC_77 = Coupling(name = 'GC_77',
                 value = '(cphi*complex(0,1)*gw*I4a22*sw**2)/(2.*cw)',
                 order = {'EWL':1})

GC_78 = Coupling(name = 'GC_78',
                 value = '-(cphi*complex(0,1)*gwR*I4a22*sw**2)/(2.*cw)',
                 order = {'EWR':1})

GC_79 = Coupling(name = 'GC_79',
                 value = '(cphi*complex(0,1)*gw*I4a33*sw**2)/(2.*cw)',
                 order = {'EWL':1})

GC_80 = Coupling(name = 'GC_80',
                 value = '-(cphi*complex(0,1)*gwR*I4a33*sw**2)/(2.*cw)',
                 order = {'EWR':1})

GC_81 = Coupling(name = 'GC_81',
                 value = '-(complex(0,1)*gwR*sphi*sw**2)/(2.*cw)',
                 order = {'EWL':-1,'EWR':2})

GC_82 = Coupling(name = 'GC_82',
                 value = '(complex(0,1)*gwR*sphi*sw**2)/(2.*cw)',
                 order = {'EWL':-1,'EWR':2})

GC_83 = Coupling(name = 'GC_83',
                 value = '-((cxi**2*complex(0,1)*gwR*sphi*sw**2)/cw)',
                 order = {'EWL':-1,'EWR':2})

GC_84 = Coupling(name = 'GC_84',
                 value = '(complex(0,1)*gwR*I3a11*sphi*sw**2)/(2.*cw)',
                 order = {'EWL':-1,'EWR':2})

GC_85 = Coupling(name = 'GC_85',
                 value = '(complex(0,1)*gwR*I3a22*sphi*sw**2)/(2.*cw)',
                 order = {'EWL':-1,'EWR':2})

GC_86 = Coupling(name = 'GC_86',
                 value = '(complex(0,1)*gwR*I3a33*sphi*sw**2)/(2.*cw)',
                 order = {'EWL':-1,'EWR':2})

GC_87 = Coupling(name = 'GC_87',
                 value = '-(complex(0,1)*gwR*I4a11*sphi*sw**2)/(2.*cw)',
                 order = {'EWL':-1,'EWR':2})

GC_88 = Coupling(name = 'GC_88',
                 value = '-(complex(0,1)*gwR*I4a22*sphi*sw**2)/(2.*cw)',
                 order = {'EWL':-1,'EWR':2})

GC_89 = Coupling(name = 'GC_89',
                 value = '-(complex(0,1)*gwR*I4a33*sphi*sw**2)/(2.*cw)',
                 order = {'EWL':-1,'EWR':2})

GC_90 = Coupling(name = 'GC_90',
                 value = '(complex(0,1)*g1*cmath.sqrt(1 - 2*sw**2))/6.',
                 order = {'QED':1})

GC_91 = Coupling(name = 'GC_91',
                 value = '(cphi*cxi**2*complex(0,1)*gwR*cmath.sqrt(1 - 2*sw**2))/cw',
                 order = {'EWR':1})

GC_92 = Coupling(name = 'GC_92',
                 value = '-(complex(0,1)*gwR*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                 order = {'EWL':-1,'EWR':2})

GC_93 = Coupling(name = 'GC_93',
                 value = '(complex(0,1)*gwR*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                 order = {'EWL':-1,'EWR':2})

GC_94 = Coupling(name = 'GC_94',
                 value = '(complex(0,1)*gwR*I3a11*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                 order = {'EWL':-1,'EWR':2})

GC_95 = Coupling(name = 'GC_95',
                 value = '(complex(0,1)*gwR*I3a22*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                 order = {'EWL':-1,'EWR':2})

GC_96 = Coupling(name = 'GC_96',
                 value = '(complex(0,1)*gwR*I3a33*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                 order = {'EWL':-1,'EWR':2})

GC_97 = Coupling(name = 'GC_97',
                 value = '-(complex(0,1)*gwR*I4a11*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                 order = {'EWL':-1,'EWR':2})

GC_98 = Coupling(name = 'GC_98',
                 value = '-(complex(0,1)*gwR*I4a22*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                 order = {'EWL':-1,'EWR':2})

GC_99 = Coupling(name = 'GC_99',
                 value = '-(complex(0,1)*gwR*I4a33*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                 order = {'EWL':-1,'EWR':2})

GC_100 = Coupling(name = 'GC_100',
                  value = '-(cphi*complex(0,1)*g1*sw*cmath.sqrt(1 - 2*sw**2))/(6.*cw)',
                  order = {'QED':1})

GC_101 = Coupling(name = 'GC_101',
                  value = '(-2*cphi*cxi**2*complex(0,1)*gwR**2*sw*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWR':2})

GC_102 = Coupling(name = 'GC_102',
                  value = '-(complex(0,1)*g1*sphi*sw*cmath.sqrt(1 - 2*sw**2))/(6.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_103 = Coupling(name = 'GC_103',
                  value = '(2*cxi**2*complex(0,1)*gwR**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':3})

GC_104 = Coupling(name = 'GC_104',
                  value = '-((cphi**2*cxi**2*complex(0,1)*gwR**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2)',
                  order = {'EWR':2})

GC_105 = Coupling(name = 'GC_105',
                  value = '(-2*cphi*cxi**2*complex(0,1)*gwR**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':3})

GC_106 = Coupling(name = 'GC_106',
                  value = '(2*cphi*cxi**2*complex(0,1)*gwR**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':3})

GC_107 = Coupling(name = 'GC_107',
                  value = '(cxi**2*complex(0,1)*gwR**2*sphi**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':4})

GC_108 = Coupling(name = 'GC_108',
                  value = '(cphi*cw*complex(0,1)*gw)/2. - (cphi*complex(0,1)*gw*sw**2)/(6.*cw)',
                  order = {'EWL':1})

GC_109 = Coupling(name = 'GC_109',
                  value = '-(cphi*cw*complex(0,1)*gw)/2. + (cphi*complex(0,1)*gw*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_110 = Coupling(name = 'GC_110',
                  value = '(cphi**2*cxi**2*complex(0,1)*gwR**2)/cw**2 - (2*cphi**2*cxi**2*complex(0,1)*gwR**2*sw**2)/cw**2',
                  order = {'EWR':2})

GC_111 = Coupling(name = 'GC_111',
                  value = '(cphi*cw*complex(0,1)*gw*I1a11)/2. + (cphi*complex(0,1)*gw*I1a11*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_112 = Coupling(name = 'GC_112',
                  value = '(cphi*cw*complex(0,1)*gw*I1a22)/2. + (cphi*complex(0,1)*gw*I1a22*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_113 = Coupling(name = 'GC_113',
                  value = '(cphi*cw*complex(0,1)*gw*I1a33)/2. + (cphi*complex(0,1)*gw*I1a33*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_114 = Coupling(name = 'GC_114',
                  value = '-(cphi*cw*complex(0,1)*gw*I2a11)/2. - (cphi*complex(0,1)*gw*I2a11*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_115 = Coupling(name = 'GC_115',
                  value = '-(cphi*cw*complex(0,1)*gw*I2a22)/2. - (cphi*complex(0,1)*gw*I2a22*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_116 = Coupling(name = 'GC_116',
                  value = '-(cphi*cw*complex(0,1)*gw*I2a33)/2. - (cphi*complex(0,1)*gw*I2a33*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_117 = Coupling(name = 'GC_117',
                  value = '(cw*complex(0,1)*gw*sphi)/2. - (complex(0,1)*gw*sphi*sw**2)/(6.*cw)',
                  order = {'EWR':1})

GC_118 = Coupling(name = 'GC_118',
                  value = '-(cw*complex(0,1)*gw*sphi)/2. + (complex(0,1)*gw*sphi*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_119 = Coupling(name = 'GC_119',
                  value = '(cw*complex(0,1)*gw*I1a11*sphi)/2. + (complex(0,1)*gw*I1a11*sphi*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_120 = Coupling(name = 'GC_120',
                  value = '(cw*complex(0,1)*gw*I1a22*sphi)/2. + (complex(0,1)*gw*I1a22*sphi*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_121 = Coupling(name = 'GC_121',
                  value = '(cw*complex(0,1)*gw*I1a33*sphi)/2. + (complex(0,1)*gw*I1a33*sphi*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_122 = Coupling(name = 'GC_122',
                  value = '-(cw*complex(0,1)*gw*I2a11*sphi)/2. - (complex(0,1)*gw*I2a11*sphi*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_123 = Coupling(name = 'GC_123',
                  value = '-(cw*complex(0,1)*gw*I2a22*sphi)/2. - (complex(0,1)*gw*I2a22*sphi*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_124 = Coupling(name = 'GC_124',
                  value = '-(cw*complex(0,1)*gw*I2a33*sphi)/2. - (complex(0,1)*gw*I2a33*sphi*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_125 = Coupling(name = 'GC_125',
                  value = '(cxi**2*complex(0,1)*gwR**2*sphi**2)/cw**2 - (2*cxi**2*complex(0,1)*gwR**2*sphi**2*sw**2)/cw**2',
                  order = {'EWL':-2,'EWR':4})

GC_126 = Coupling(name = 'GC_126',
                  value = '-(complex(0,1)*gw*sphi*sw**2)/(6.*cw) - (cphi*complex(0,1)*gwR*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_127 = Coupling(name = 'GC_127',
                  value = '(complex(0,1)*gw*sphi*sw**2)/(2.*cw) - (cphi*complex(0,1)*gwR*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_128 = Coupling(name = 'GC_128',
                  value = '-(complex(0,1)*gw*sphi*sw**2)/(6.*cw) + (cphi*complex(0,1)*gwR*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_129 = Coupling(name = 'GC_129',
                  value = '-(complex(0,1)*gw*I3a11*sphi*sw**2)/(2.*cw) - (cphi*complex(0,1)*gwR*I3a11*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_130 = Coupling(name = 'GC_130',
                  value = '-(complex(0,1)*gw*I3a22*sphi*sw**2)/(2.*cw) - (cphi*complex(0,1)*gwR*I3a22*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_131 = Coupling(name = 'GC_131',
                  value = '-(complex(0,1)*gw*I3a33*sphi*sw**2)/(2.*cw) - (cphi*complex(0,1)*gwR*I3a33*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_132 = Coupling(name = 'GC_132',
                  value = '(complex(0,1)*gw*I4a11*sphi*sw**2)/(2.*cw) + (cphi*complex(0,1)*gwR*I4a11*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_133 = Coupling(name = 'GC_133',
                  value = '(complex(0,1)*gw*I4a22*sphi*sw**2)/(2.*cw) + (cphi*complex(0,1)*gwR*I4a22*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_134 = Coupling(name = 'GC_134',
                  value = '(complex(0,1)*gw*I4a33*sphi*sw**2)/(2.*cw) + (cphi*complex(0,1)*gwR*I4a33*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_135 = Coupling(name = 'GC_135',
                  value = 'cphi*cw*cxi*complex(0,1)*gw*sxi',
                  order = {'EWR':1})

GC_136 = Coupling(name = 'GC_136',
                  value = 'cphi**2*cw**2*cxi*complex(0,1)*gw**2*sxi',
                  order = {'EWL':1,'EWR':1})

GC_137 = Coupling(name = 'GC_137',
                  value = '-(cxi**3*complex(0,1)*gw**2*sxi)',
                  order = {'EWL':1,'EWR':1})

GC_138 = Coupling(name = 'GC_138',
                  value = 'cphi*cw**2*cxi*complex(0,1)*gw**2*sphi*sxi',
                  order = {'EWR':2})

GC_139 = Coupling(name = 'GC_139',
                  value = 'cxi*complex(0,1)*gw*sw*sxi',
                  order = {'EWR':1})

GC_140 = Coupling(name = 'GC_140',
                  value = '-2*cphi*cw*cxi*complex(0,1)*gw**2*sw*sxi',
                  order = {'EWL':1,'EWR':1})

GC_141 = Coupling(name = 'GC_141',
                  value = '-(cxi*complex(0,1)*gwR*sw*sxi)',
                  order = {'EWL':-1,'EWR':2})

GC_142 = Coupling(name = 'GC_142',
                  value = '-2*cw*cxi*complex(0,1)*gw**2*sphi*sw*sxi',
                  order = {'EWR':2})

GC_143 = Coupling(name = 'GC_143',
                  value = 'cxi*complex(0,1)*gw**2*sw**2*sxi',
                  order = {'EWL':1,'EWR':1})

GC_144 = Coupling(name = 'GC_144',
                  value = '(cphi*cxi*complex(0,1)*gwR*sw**2*sxi)/cw',
                  order = {'EWL':-1,'EWR':2})

GC_145 = Coupling(name = 'GC_145',
                  value = '-(cxi*complex(0,1)*gwR**2*sw**2*sxi)',
                  order = {'EWL':-1,'EWR':3})

GC_146 = Coupling(name = 'GC_146',
                  value = '(cxi*complex(0,1)*gwR*sphi*sw**2*sxi)/cw',
                  order = {'EWL':-2,'EWR':3})

GC_147 = Coupling(name = 'GC_147',
                  value = '(-2*cphi*cxi*complex(0,1)*gwR**2*sw**3*sxi)/cw',
                  order = {'EWL':-1,'EWR':3})

GC_148 = Coupling(name = 'GC_148',
                  value = '(-2*cxi*complex(0,1)*gwR**2*sphi*sw**3*sxi)/cw',
                  order = {'EWL':-2,'EWR':4})

GC_149 = Coupling(name = 'GC_149',
                  value = '-((cphi**2*cxi*complex(0,1)*gwR**2*sw**4*sxi)/cw**2)',
                  order = {'EWL':-1,'EWR':3})

GC_150 = Coupling(name = 'GC_150',
                  value = '-((cxi*complex(0,1)*gwR**2*sphi**2*sw**4*sxi)/cw**2)',
                  order = {'EWL':-3,'EWR':5})

GC_151 = Coupling(name = 'GC_151',
                  value = '(cxi*complex(0,1)*gwR*sphi*sxi*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-2,'EWR':3})

GC_152 = Coupling(name = 'GC_152',
                  value = '(2*cphi*cxi*complex(0,1)*gwR**2*sw*sxi*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':3})

GC_153 = Coupling(name = 'GC_153',
                  value = '(-2*cxi*complex(0,1)*gwR**2*sphi*sw*sxi*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-2,'EWR':4})

GC_154 = Coupling(name = 'GC_154',
                  value = '(cphi**2*cxi*complex(0,1)*gwR**2*sw**2*sxi*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':3})

GC_155 = Coupling(name = 'GC_155',
                  value = '(-2*cphi*cxi*complex(0,1)*gwR**2*sphi*sw**2*sxi*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':4})

GC_156 = Coupling(name = 'GC_156',
                  value = '(2*cphi*cxi*complex(0,1)*gwR**2*sphi*sw**2*sxi*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':4})

GC_157 = Coupling(name = 'GC_157',
                  value = '-((cxi*complex(0,1)*gwR**2*sphi**2*sw**2*sxi*cmath.sqrt(1 - 2*sw**2))/cw**2)',
                  order = {'EWL':-3,'EWR':5})

GC_158 = Coupling(name = 'GC_158',
                  value = '-(cxi**2*complex(0,1)*gw**2*sxi**2)',
                  order = {'EWR':2})

GC_159 = Coupling(name = 'GC_159',
                  value = '-(cxi**2*complex(0,1)*gwR**2*sxi**2)',
                  order = {'EWL':-2,'EWR':4})

GC_160 = Coupling(name = 'GC_160',
                  value = 'cw*complex(0,1)*gw*sphi*sxi**2',
                  order = {'EWL':-2,'EWR':3})

GC_161 = Coupling(name = 'GC_161',
                  value = 'complex(0,1)*gw*sw*sxi**2',
                  order = {'EWL':-1,'EWR':2})

GC_162 = Coupling(name = 'GC_162',
                  value = 'complex(0,1)*gwR*sw*sxi**2',
                  order = {'EWL':-2,'EWR':3})

GC_163 = Coupling(name = 'GC_163',
                  value = '-((cphi*complex(0,1)*gwR*sw**2*sxi**2)/cw)',
                  order = {'EWL':-2,'EWR':3})

GC_164 = Coupling(name = 'GC_164',
                  value = 'complex(0,1)*gwR**2*sw**2*sxi**2',
                  order = {'EWL':-2,'EWR':4})

GC_165 = Coupling(name = 'GC_165',
                  value = '-((complex(0,1)*gwR*sphi*sw**2*sxi**2)/cw)',
                  order = {'EWL':-3,'EWR':4})

GC_166 = Coupling(name = 'GC_166',
                  value = '(2*cphi*complex(0,1)*gwR**2*sw**3*sxi**2)/cw',
                  order = {'EWL':-2,'EWR':4})

GC_167 = Coupling(name = 'GC_167',
                  value = '(2*complex(0,1)*gwR**2*sphi*sw**3*sxi**2)/cw',
                  order = {'EWL':-3,'EWR':5})

GC_168 = Coupling(name = 'GC_168',
                  value = '(cphi**2*complex(0,1)*gwR**2*sw**4*sxi**2)/cw**2',
                  order = {'EWL':-2,'EWR':4})

GC_169 = Coupling(name = 'GC_169',
                  value = '(complex(0,1)*gwR**2*sphi**2*sw**4*sxi**2)/cw**2',
                  order = {'EWL':-4,'EWR':6})

GC_170 = Coupling(name = 'GC_170',
                  value = '(cphi*complex(0,1)*gwR*sxi**2*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-2,'EWR':3})

GC_171 = Coupling(name = 'GC_171',
                  value = '-((complex(0,1)*gwR*sphi*sxi**2*cmath.sqrt(1 - 2*sw**2))/cw)',
                  order = {'EWL':-3,'EWR':4})

GC_172 = Coupling(name = 'GC_172',
                  value = '(-2*cphi*complex(0,1)*gwR**2*sw*sxi**2*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-2,'EWR':4})

GC_173 = Coupling(name = 'GC_173',
                  value = '(2*complex(0,1)*gwR**2*sphi*sw*sxi**2*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-3,'EWR':5})

GC_174 = Coupling(name = 'GC_174',
                  value = '-((cphi**2*complex(0,1)*gwR**2*sw**2*sxi**2*cmath.sqrt(1 - 2*sw**2))/cw**2)',
                  order = {'EWL':-2,'EWR':4})

GC_175 = Coupling(name = 'GC_175',
                  value = '(-2*cphi*complex(0,1)*gwR**2*sphi*sw**2*sxi**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-3,'EWR':5})

GC_176 = Coupling(name = 'GC_176',
                  value = '(2*cphi*complex(0,1)*gwR**2*sphi*sw**2*sxi**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-3,'EWR':5})

GC_177 = Coupling(name = 'GC_177',
                  value = '(complex(0,1)*gwR**2*sphi**2*sw**2*sxi**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-4,'EWR':6})

GC_178 = Coupling(name = 'GC_178',
                  value = 'cxi*complex(0,1)*gwR**2*sxi**3',
                  order = {'EWL':-3,'EWR':5})

GC_179 = Coupling(name = 'GC_179',
                  value = '-(complex(0,1)*gw**2*sxi**4)',
                  order = {'EWL':-2,'EWR':4})

GC_180 = Coupling(name = 'GC_180',
                  value = '-(complex(0,1)*gwR**2*sxi**4)',
                  order = {'EWL':-4,'EWR':6})

GC_181 = Coupling(name = 'GC_181',
                  value = '-((cphi**2*cxi*complex(0,1)*gwR**2*sxi)/cw**2) + cw**2*cxi*complex(0,1)*gw**2*sphi**2*sxi + (2*cphi**2*cxi*complex(0,1)*gwR**2*sw**2*sxi)/cw**2',
                  order = {'EWL':-1,'EWR':3})

GC_182 = Coupling(name = 'GC_182',
                  value = '-((cxi*complex(0,1)*gwR**2*sphi**2*sxi)/cw**2) + (2*cxi*complex(0,1)*gwR**2*sphi**2*sw**2*sxi)/cw**2',
                  order = {'EWL':-3,'EWR':5})

GC_183 = Coupling(name = 'GC_183',
                  value = '(cphi*cxi*complex(0,1)*gwR**2*sphi*sxi)/cw**2 - (2*cphi*cxi*complex(0,1)*gwR**2*sphi*sw**2*sxi)/cw**2 - (cphi*cxi*complex(0,1)*gwR**2*sphi*sw**4*sxi)/cw**2',
                  order = {'EWL':-2,'EWR':4})

GC_184 = Coupling(name = 'GC_184',
                  value = 'cw*cxi*complex(0,1)*gw*sphi*sxi - (cphi*cxi*complex(0,1)*gwR*sxi*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':2})

GC_185 = Coupling(name = 'GC_185',
                  value = 'cphi*cw*complex(0,1)*gw*sxi**2 - (cxi**2*complex(0,1)*gwR*sphi*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':2})

GC_186 = Coupling(name = 'GC_186',
                  value = '(cphi**2*cxi**2*complex(0,1)*gwR**2*sw**4)/cw**2 + cphi**2*cw**2*complex(0,1)*gw**2*sxi**2',
                  order = {'EWR':2})

GC_187 = Coupling(name = 'GC_187',
                  value = '-((cphi*cxi**2*complex(0,1)*gwR**2*sphi)/cw**2) + (2*cphi*cxi**2*complex(0,1)*gwR**2*sphi*sw**2)/cw**2 + (cphi*cxi**2*complex(0,1)*gwR**2*sphi*sw**4)/cw**2 + cphi*cw**2*complex(0,1)*gw**2*sphi*sxi**2',
                  order = {'EWL':-1,'EWR':3})

GC_188 = Coupling(name = 'GC_188',
                  value = '(cxi**2*complex(0,1)*gwR**2*sphi**2*sw**4)/cw**2 + cw**2*complex(0,1)*gw**2*sphi**2*sxi**2',
                  order = {'EWL':-2,'EWR':4})

GC_189 = Coupling(name = 'GC_189',
                  value = '(2*cphi*cxi**2*complex(0,1)*gwR**2*sw**3)/cw - 2*cphi*cw*complex(0,1)*gw**2*sw*sxi**2',
                  order = {'EWR':2})

GC_190 = Coupling(name = 'GC_190',
                  value = '(2*cxi**2*complex(0,1)*gwR**2*sphi*sw**3)/cw - 2*cw*complex(0,1)*gw**2*sphi*sw*sxi**2',
                  order = {'EWL':-1,'EWR':3})

GC_191 = Coupling(name = 'GC_191',
                  value = 'cxi**2*complex(0,1)*gwR**2*sw**2 + complex(0,1)*gw**2*sw**2*sxi**2',
                  order = {'EWR':2})

GC_192 = Coupling(name = 'GC_192',
                  value = '(cphi**2*complex(0,1)*gwR**2*sxi**2)/cw**2 - (2*cphi**2*complex(0,1)*gwR**2*sw**2*sxi**2)/cw**2',
                  order = {'EWL':-2,'EWR':4})

GC_193 = Coupling(name = 'GC_193',
                  value = '(complex(0,1)*gwR**2*sphi**2*sxi**2)/cw**2 - (2*complex(0,1)*gwR**2*sphi**2*sw**2*sxi**2)/cw**2',
                  order = {'EWL':-4,'EWR':6})

GC_194 = Coupling(name = 'GC_194',
                  value = '-((cphi*complex(0,1)*gwR**2*sphi*sxi**2)/cw**2) + (2*cphi*complex(0,1)*gwR**2*sphi*sw**2*sxi**2)/cw**2 + (cphi*complex(0,1)*gwR**2*sphi*sw**4*sxi**2)/cw**2',
                  order = {'EWL':-3,'EWR':5})

GC_195 = Coupling(name = 'GC_195',
                  value = 'cxi**3*complex(0,1)*gwR**2*sxi - cxi*complex(0,1)*gw**2*sxi**3',
                  order = {'EWL':-1,'EWR':3})

GC_196 = Coupling(name = 'GC_196',
                  value = '(cxi*complex(0,1)*gw*VlL1x1)/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_197 = Coupling(name = 'GC_197',
                  value = '(complex(0,1)*gw*sxi*VlL1x1)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_198 = Coupling(name = 'GC_198',
                  value = '(cxi*complex(0,1)*gw*VlL2x2)/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_199 = Coupling(name = 'GC_199',
                  value = '(complex(0,1)*gw*sxi*VlL2x2)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_200 = Coupling(name = 'GC_200',
                  value = '(cxi*complex(0,1)*gw*VlL3x3)/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_201 = Coupling(name = 'GC_201',
                  value = '(complex(0,1)*gw*sxi*VlL3x3)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_202 = Coupling(name = 'GC_202',
                  value = '(cxi*complex(0,1)*gwR*VlR1x1)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_203 = Coupling(name = 'GC_203',
                  value = '-((complex(0,1)*gwR*sxi*VlR1x1)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_204 = Coupling(name = 'GC_204',
                  value = '(cxi*complex(0,1)*gwR*VlR2x2)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_205 = Coupling(name = 'GC_205',
                  value = '-((complex(0,1)*gwR*sxi*VlR2x2)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_206 = Coupling(name = 'GC_206',
                  value = '(cxi*complex(0,1)*gwR*VlR3x3)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_207 = Coupling(name = 'GC_207',
                  value = '-((complex(0,1)*gwR*sxi*VlR3x3)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_208 = Coupling(name = 'GC_208',
                  value = '(cxi*complex(0,1)*gw*VqL1x1)/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_209 = Coupling(name = 'GC_209',
                  value = '(complex(0,1)*gw*sxi*VqL1x1)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_210 = Coupling(name = 'GC_210',
                  value = '(cxi*complex(0,1)*gw*VqL1x2)/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_211 = Coupling(name = 'GC_211',
                  value = '(complex(0,1)*gw*sxi*VqL1x2)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_212 = Coupling(name = 'GC_212',
                  value = '(cxi*complex(0,1)*gw*VqL1x3)/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_213 = Coupling(name = 'GC_213',
                  value = '(complex(0,1)*gw*sxi*VqL1x3)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_214 = Coupling(name = 'GC_214',
                  value = '(cxi*complex(0,1)*gw*VqL2x1)/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_215 = Coupling(name = 'GC_215',
                  value = '(complex(0,1)*gw*sxi*VqL2x1)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_216 = Coupling(name = 'GC_216',
                  value = '(cxi*complex(0,1)*gw*VqL2x2)/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_217 = Coupling(name = 'GC_217',
                  value = '(complex(0,1)*gw*sxi*VqL2x2)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_218 = Coupling(name = 'GC_218',
                  value = '(cxi*complex(0,1)*gw*VqL2x3)/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_219 = Coupling(name = 'GC_219',
                  value = '(complex(0,1)*gw*sxi*VqL2x3)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_220 = Coupling(name = 'GC_220',
                  value = '(cxi*complex(0,1)*gw*VqL3x1)/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_221 = Coupling(name = 'GC_221',
                  value = '(complex(0,1)*gw*sxi*VqL3x1)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_222 = Coupling(name = 'GC_222',
                  value = '(cxi*complex(0,1)*gw*VqL3x2)/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_223 = Coupling(name = 'GC_223',
                  value = '(complex(0,1)*gw*sxi*VqL3x2)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_224 = Coupling(name = 'GC_224',
                  value = '(cxi*complex(0,1)*gw*VqL3x3)/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_225 = Coupling(name = 'GC_225',
                  value = '(complex(0,1)*gw*sxi*VqL3x3)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_226 = Coupling(name = 'GC_226',
                  value = '(cxi*complex(0,1)*gwR*VqR1x1)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_227 = Coupling(name = 'GC_227',
                  value = '-((complex(0,1)*gwR*sxi*VqR1x1)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_228 = Coupling(name = 'GC_228',
                  value = '(cxi*complex(0,1)*gwR*VqR1x2)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_229 = Coupling(name = 'GC_229',
                  value = '-((complex(0,1)*gwR*sxi*VqR1x2)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_230 = Coupling(name = 'GC_230',
                  value = '(cxi*complex(0,1)*gwR*VqR1x3)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_231 = Coupling(name = 'GC_231',
                  value = '-((complex(0,1)*gwR*sxi*VqR1x3)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_232 = Coupling(name = 'GC_232',
                  value = '(cxi*complex(0,1)*gwR*VqR2x1)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_233 = Coupling(name = 'GC_233',
                  value = '-((complex(0,1)*gwR*sxi*VqR2x1)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_234 = Coupling(name = 'GC_234',
                  value = '(cxi*complex(0,1)*gwR*VqR2x2)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_235 = Coupling(name = 'GC_235',
                  value = '-((complex(0,1)*gwR*sxi*VqR2x2)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_236 = Coupling(name = 'GC_236',
                  value = '(cxi*complex(0,1)*gwR*VqR2x3)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_237 = Coupling(name = 'GC_237',
                  value = '-((complex(0,1)*gwR*sxi*VqR2x3)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_238 = Coupling(name = 'GC_238',
                  value = '(cxi*complex(0,1)*gwR*VqR3x1)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_239 = Coupling(name = 'GC_239',
                  value = '-((complex(0,1)*gwR*sxi*VqR3x1)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_240 = Coupling(name = 'GC_240',
                  value = '(cxi*complex(0,1)*gwR*VqR3x2)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_241 = Coupling(name = 'GC_241',
                  value = '-((complex(0,1)*gwR*sxi*VqR3x2)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_242 = Coupling(name = 'GC_242',
                  value = '(cxi*complex(0,1)*gwR*VqR3x3)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_243 = Coupling(name = 'GC_243',
                  value = '-((complex(0,1)*gwR*sxi*VqR3x3)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_244 = Coupling(name = 'GC_244',
                  value = '(cxi*complex(0,1)*gw*complexconjugate(VqL1x1))/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_245 = Coupling(name = 'GC_245',
                  value = '(complex(0,1)*gw*sxi*complexconjugate(VqL1x1))/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_246 = Coupling(name = 'GC_246',
                  value = '(cxi*complex(0,1)*gw*complexconjugate(VqL1x2))/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_247 = Coupling(name = 'GC_247',
                  value = '(complex(0,1)*gw*sxi*complexconjugate(VqL1x2))/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_248 = Coupling(name = 'GC_248',
                  value = '(cxi*complex(0,1)*gw*complexconjugate(VqL1x3))/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_249 = Coupling(name = 'GC_249',
                  value = '(complex(0,1)*gw*sxi*complexconjugate(VqL1x3))/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_250 = Coupling(name = 'GC_250',
                  value = '(cxi*complex(0,1)*gw*complexconjugate(VqL2x1))/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_251 = Coupling(name = 'GC_251',
                  value = '(complex(0,1)*gw*sxi*complexconjugate(VqL2x1))/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_252 = Coupling(name = 'GC_252',
                  value = '(cxi*complex(0,1)*gw*complexconjugate(VqL2x2))/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_253 = Coupling(name = 'GC_253',
                  value = '(complex(0,1)*gw*sxi*complexconjugate(VqL2x2))/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_254 = Coupling(name = 'GC_254',
                  value = '(cxi*complex(0,1)*gw*complexconjugate(VqL2x3))/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_255 = Coupling(name = 'GC_255',
                  value = '(complex(0,1)*gw*sxi*complexconjugate(VqL2x3))/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_256 = Coupling(name = 'GC_256',
                  value = '(cxi*complex(0,1)*gw*complexconjugate(VqL3x1))/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_257 = Coupling(name = 'GC_257',
                  value = '(complex(0,1)*gw*sxi*complexconjugate(VqL3x1))/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_258 = Coupling(name = 'GC_258',
                  value = '(cxi*complex(0,1)*gw*complexconjugate(VqL3x2))/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_259 = Coupling(name = 'GC_259',
                  value = '(complex(0,1)*gw*sxi*complexconjugate(VqL3x2))/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_260 = Coupling(name = 'GC_260',
                  value = '(cxi*complex(0,1)*gw*complexconjugate(VqL3x3))/cmath.sqrt(2)',
                  order = {'EWL':1})

GC_261 = Coupling(name = 'GC_261',
                  value = '(complex(0,1)*gw*sxi*complexconjugate(VqL3x3))/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_262 = Coupling(name = 'GC_262',
                  value = '(cxi*complex(0,1)*gwR*complexconjugate(VqR1x1))/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_263 = Coupling(name = 'GC_263',
                  value = '-((complex(0,1)*gwR*sxi*complexconjugate(VqR1x1))/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_264 = Coupling(name = 'GC_264',
                  value = '(cxi*complex(0,1)*gwR*complexconjugate(VqR1x2))/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_265 = Coupling(name = 'GC_265',
                  value = '-((complex(0,1)*gwR*sxi*complexconjugate(VqR1x2))/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_266 = Coupling(name = 'GC_266',
                  value = '(cxi*complex(0,1)*gwR*complexconjugate(VqR1x3))/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_267 = Coupling(name = 'GC_267',
                  value = '-((complex(0,1)*gwR*sxi*complexconjugate(VqR1x3))/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_268 = Coupling(name = 'GC_268',
                  value = '(cxi*complex(0,1)*gwR*complexconjugate(VqR2x1))/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_269 = Coupling(name = 'GC_269',
                  value = '-((complex(0,1)*gwR*sxi*complexconjugate(VqR2x1))/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_270 = Coupling(name = 'GC_270',
                  value = '(cxi*complex(0,1)*gwR*complexconjugate(VqR2x2))/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_271 = Coupling(name = 'GC_271',
                  value = '-((complex(0,1)*gwR*sxi*complexconjugate(VqR2x2))/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_272 = Coupling(name = 'GC_272',
                  value = '(cxi*complex(0,1)*gwR*complexconjugate(VqR2x3))/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_273 = Coupling(name = 'GC_273',
                  value = '-((complex(0,1)*gwR*sxi*complexconjugate(VqR2x3))/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_274 = Coupling(name = 'GC_274',
                  value = '(cxi*complex(0,1)*gwR*complexconjugate(VqR3x1))/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_275 = Coupling(name = 'GC_275',
                  value = '-((complex(0,1)*gwR*sxi*complexconjugate(VqR3x1))/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_276 = Coupling(name = 'GC_276',
                  value = '(cxi*complex(0,1)*gwR*complexconjugate(VqR3x2))/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_277 = Coupling(name = 'GC_277',
                  value = '-((complex(0,1)*gwR*sxi*complexconjugate(VqR3x2))/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_278 = Coupling(name = 'GC_278',
                  value = '(cxi*complex(0,1)*gwR*complexconjugate(VqR3x3))/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_279 = Coupling(name = 'GC_279',
                  value = '-((complex(0,1)*gwR*sxi*complexconjugate(VqR3x3))/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})


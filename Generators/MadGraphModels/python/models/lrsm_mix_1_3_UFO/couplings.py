# This file was automatically created by FeynRules 2.3.1
# Mathematica version: 10.1.0  for Mac OS X x86 (64-bit) (March 24, 2015)
# Date: Wed 27 May 2015 19:49:11


from object_library import all_couplings, Coupling

from function_library import complexconjugate, re, im, csc, sec, acsc, asec, cot



GC_1 = Coupling(name = 'GC_1',
                value = '-(AH*cth1*complex(0,1))',
                order = {'HIW':1})

GC_2 = Coupling(name = 'GC_2',
                value = '-(cth1*complex(0,1)*GH)',
                order = {'HIG':1})

GC_3 = Coupling(name = 'GC_3',
                value = '-gs',
                order = {'QCD':1})

GC_4 = Coupling(name = 'GC_4',
                value = 'complex(0,1)*gs',
                order = {'QCD':1})

GC_5 = Coupling(name = 'GC_5',
                value = '-(cth1*GH*gs)',
                order = {'HIG':1,'QCD':1})

GC_6 = Coupling(name = 'GC_6',
                value = 'complex(0,1)*gs**2',
                order = {'QCD':2})

GC_7 = Coupling(name = 'GC_7',
                value = 'cth1*complex(0,1)*GH*gs**2',
                order = {'HIG':1,'QCD':2})

GC_8 = Coupling(name = 'GC_8',
                value = '-(cphi*cw*gw)',
                order = {'EWL':1})

GC_9 = Coupling(name = 'GC_9',
                value = '-(cphi*cw*complex(0,1)*gw)/2.',
                order = {'EWL':1})

GC_10 = Coupling(name = 'GC_10',
                 value = '(cphi*cw*complex(0,1)*gw)/2.',
                 order = {'EWL':1})

GC_11 = Coupling(name = 'GC_11',
                 value = '-(cphi*cw*complex(0,1)*gw)',
                 order = {'EWL':1})

GC_12 = Coupling(name = 'GC_12',
                 value = '-(cxi*complex(0,1)*gw)',
                 order = {'EWL':1})

GC_13 = Coupling(name = 'GC_13',
                 value = 'cxi*complex(0,1)*gw',
                 order = {'EWL':1})

GC_14 = Coupling(name = 'GC_14',
                 value = '-((cxi*complex(0,1)*gw)/cmath.sqrt(2))',
                 order = {'EWL':1})

GC_15 = Coupling(name = 'GC_15',
                 value = '(cxi*complex(0,1)*gw)/cmath.sqrt(2)',
                 order = {'EWL':1})

GC_16 = Coupling(name = 'GC_16',
                 value = '(cxi*gw)/cmath.sqrt(2)',
                 order = {'EWL':1})

GC_17 = Coupling(name = 'GC_17',
                 value = '(CKML1x1*cxi*complex(0,1)*gw)/cmath.sqrt(2)',
                 order = {'EWL':1})

GC_18 = Coupling(name = 'GC_18',
                 value = '(CKML1x2*cxi*complex(0,1)*gw)/cmath.sqrt(2)',
                 order = {'EWL':1})

GC_19 = Coupling(name = 'GC_19',
                 value = '(CKML1x3*cxi*complex(0,1)*gw)/cmath.sqrt(2)',
                 order = {'EWL':1})

GC_20 = Coupling(name = 'GC_20',
                 value = '(CKML2x1*cxi*complex(0,1)*gw)/cmath.sqrt(2)',
                 order = {'EWL':1})

GC_21 = Coupling(name = 'GC_21',
                 value = '(CKML2x2*cxi*complex(0,1)*gw)/cmath.sqrt(2)',
                 order = {'EWL':1})

GC_22 = Coupling(name = 'GC_22',
                 value = '(CKML2x3*cxi*complex(0,1)*gw)/cmath.sqrt(2)',
                 order = {'EWL':1})

GC_23 = Coupling(name = 'GC_23',
                 value = '(CKML3x1*cxi*complex(0,1)*gw)/cmath.sqrt(2)',
                 order = {'EWL':1})

GC_24 = Coupling(name = 'GC_24',
                 value = '(CKML3x2*cxi*complex(0,1)*gw)/cmath.sqrt(2)',
                 order = {'EWL':1})

GC_25 = Coupling(name = 'GC_25',
                 value = '(CKML3x3*cxi*complex(0,1)*gw)/cmath.sqrt(2)',
                 order = {'EWL':1})

GC_26 = Coupling(name = 'GC_26',
                 value = 'cphi*cw*cxi**2*complex(0,1)*gw',
                 order = {'EWL':1})

GC_27 = Coupling(name = 'GC_27',
                 value = '2*cphi**2*cw**2*complex(0,1)*gw**2',
                 order = {'EWL':2})

GC_28 = Coupling(name = 'GC_28',
                 value = '-(cphi*cw*cxi*complex(0,1)*gw**2)',
                 order = {'EWL':2})

GC_29 = Coupling(name = 'GC_29',
                 value = '-((cphi*cw*cxi*gw**2)/cmath.sqrt(2))',
                 order = {'EWL':2})

GC_30 = Coupling(name = 'GC_30',
                 value = '-((cphi*cw*cxi*complex(0,1)*gw**2)/cmath.sqrt(2))',
                 order = {'EWL':2})

GC_31 = Coupling(name = 'GC_31',
                 value = '(cphi*cw*cxi*gw**2)/cmath.sqrt(2)',
                 order = {'EWL':2})

GC_32 = Coupling(name = 'GC_32',
                 value = 'cxi**2*complex(0,1)*gw**2',
                 order = {'EWL':2})

GC_33 = Coupling(name = 'GC_33',
                 value = '2*cxi**2*complex(0,1)*gw**2',
                 order = {'EWL':2})

GC_34 = Coupling(name = 'GC_34',
                 value = '-(cxi**2*gw**2*cmath.sqrt(2))',
                 order = {'EWL':2})

GC_35 = Coupling(name = 'GC_35',
                 value = '-(cxi**2*complex(0,1)*gw**2*cmath.sqrt(2))',
                 order = {'EWL':2})

GC_36 = Coupling(name = 'GC_36',
                 value = 'cxi**2*gw**2*cmath.sqrt(2)',
                 order = {'EWL':2})

GC_37 = Coupling(name = 'GC_37',
                 value = 'cphi**2*cw**2*cxi**2*complex(0,1)*gw**2',
                 order = {'EWL':2})

GC_38 = Coupling(name = 'GC_38',
                 value = '-(cxi**4*complex(0,1)*gw**2)',
                 order = {'EWL':2})

GC_39 = Coupling(name = 'GC_39',
                 value = '(CKMR1x1*cxi*complex(0,1)*gwR)/cmath.sqrt(2)',
                 order = {'EWR':1})

GC_40 = Coupling(name = 'GC_40',
                 value = '(CKMR1x2*cxi*complex(0,1)*gwR)/cmath.sqrt(2)',
                 order = {'EWR':1})

GC_41 = Coupling(name = 'GC_41',
                 value = '(CKMR1x3*cxi*complex(0,1)*gwR)/cmath.sqrt(2)',
                 order = {'EWR':1})

GC_42 = Coupling(name = 'GC_42',
                 value = '(CKMR2x1*cxi*complex(0,1)*gwR)/cmath.sqrt(2)',
                 order = {'EWR':1})

GC_43 = Coupling(name = 'GC_43',
                 value = '(CKMR2x2*cxi*complex(0,1)*gwR)/cmath.sqrt(2)',
                 order = {'EWR':1})

GC_44 = Coupling(name = 'GC_44',
                 value = '(CKMR2x3*cxi*complex(0,1)*gwR)/cmath.sqrt(2)',
                 order = {'EWR':1})

GC_45 = Coupling(name = 'GC_45',
                 value = '(CKMR3x1*cxi*complex(0,1)*gwR)/cmath.sqrt(2)',
                 order = {'EWR':1})

GC_46 = Coupling(name = 'GC_46',
                 value = '(CKMR3x2*cxi*complex(0,1)*gwR)/cmath.sqrt(2)',
                 order = {'EWR':1})

GC_47 = Coupling(name = 'GC_47',
                 value = '(CKMR3x3*cxi*complex(0,1)*gwR)/cmath.sqrt(2)',
                 order = {'EWR':1})

GC_48 = Coupling(name = 'GC_48',
                 value = 'cxi**2*complex(0,1)*gwR**2',
                 order = {'EWR':2})

GC_49 = Coupling(name = 'GC_49',
                 value = '-(cth1*cxi**2*complex(0,1)*gwR**2*cmath.sqrt(2))',
                 order = {'EWR':2})

GC_50 = Coupling(name = 'GC_50',
                 value = '-(cxi**4*complex(0,1)*gwR**2)',
                 order = {'EWR':2})

GC_51 = Coupling(name = 'GC_51',
                 value = '(cxi*complex(0,1)*gw*KL1x1)/cmath.sqrt(2)',
                 order = {'EWL':1})

GC_52 = Coupling(name = 'GC_52',
                 value = '(cxi*complex(0,1)*gw*KL2x2)/cmath.sqrt(2)',
                 order = {'EWL':1})

GC_53 = Coupling(name = 'GC_53',
                 value = '(cxi*complex(0,1)*gw*KL3x3)/cmath.sqrt(2)',
                 order = {'EWL':1})

GC_54 = Coupling(name = 'GC_54',
                 value = '(cxi*complex(0,1)*gw*KL4x1)/cmath.sqrt(2)',
                 order = {'EWL':1})

GC_55 = Coupling(name = 'GC_55',
                 value = '(cxi*complex(0,1)*gw*KL5x2)/cmath.sqrt(2)',
                 order = {'EWL':1})

GC_56 = Coupling(name = 'GC_56',
                 value = '(cxi*complex(0,1)*gw*KL6x3)/cmath.sqrt(2)',
                 order = {'EWL':1})

GC_57 = Coupling(name = 'GC_57',
                 value = '(cxi*complex(0,1)*gwR*KR1x1)/cmath.sqrt(2)',
                 order = {'EWR':1})

GC_58 = Coupling(name = 'GC_58',
                 value = '(cxi*complex(0,1)*gwR*KR2x2)/cmath.sqrt(2)',
                 order = {'EWR':1})

GC_59 = Coupling(name = 'GC_59',
                 value = '(cxi*complex(0,1)*gwR*KR3x3)/cmath.sqrt(2)',
                 order = {'EWR':1})

GC_60 = Coupling(name = 'GC_60',
                 value = '(cxi*complex(0,1)*gwR*KR4x1)/cmath.sqrt(2)',
                 order = {'EWR':1})

GC_61 = Coupling(name = 'GC_61',
                 value = '(cxi*complex(0,1)*gwR*KR5x2)/cmath.sqrt(2)',
                 order = {'EWR':1})

GC_62 = Coupling(name = 'GC_62',
                 value = '(cxi*complex(0,1)*gwR*KR6x3)/cmath.sqrt(2)',
                 order = {'EWR':1})

GC_63 = Coupling(name = 'GC_63',
                 value = '-(cw*gw*sphi)',
                 order = {'EWR':1})

GC_64 = Coupling(name = 'GC_64',
                 value = '-(cw*complex(0,1)*gw*sphi)/2.',
                 order = {'EWR':1})

GC_65 = Coupling(name = 'GC_65',
                 value = '(cw*complex(0,1)*gw*sphi)/2.',
                 order = {'EWR':1})

GC_66 = Coupling(name = 'GC_66',
                 value = '-(cw*complex(0,1)*gw*sphi)',
                 order = {'EWR':1})

GC_67 = Coupling(name = 'GC_67',
                 value = 'cw*cxi**2*complex(0,1)*gw*sphi',
                 order = {'EWR':1})

GC_68 = Coupling(name = 'GC_68',
                 value = '2*cphi*cw**2*complex(0,1)*gw**2*sphi',
                 order = {'EWL':1,'EWR':1})

GC_69 = Coupling(name = 'GC_69',
                 value = '-(cw*cxi*complex(0,1)*gw**2*sphi)',
                 order = {'EWL':1,'EWR':1})

GC_70 = Coupling(name = 'GC_70',
                 value = '-((cw*cxi*gw**2*sphi)/cmath.sqrt(2))',
                 order = {'EWL':1,'EWR':1})

GC_71 = Coupling(name = 'GC_71',
                 value = '-((cw*cxi*complex(0,1)*gw**2*sphi)/cmath.sqrt(2))',
                 order = {'EWL':1,'EWR':1})

GC_72 = Coupling(name = 'GC_72',
                 value = '(cw*cxi*gw**2*sphi)/cmath.sqrt(2)',
                 order = {'EWL':1,'EWR':1})

GC_73 = Coupling(name = 'GC_73',
                 value = 'cphi*cw**2*cxi**2*complex(0,1)*gw**2*sphi',
                 order = {'EWL':1,'EWR':1})

GC_74 = Coupling(name = 'GC_74',
                 value = '2*cw**2*complex(0,1)*gw**2*sphi**2',
                 order = {'EWR':2})

GC_75 = Coupling(name = 'GC_75',
                 value = 'cw**2*cxi**2*complex(0,1)*gw**2*sphi**2',
                 order = {'EWR':2})

GC_76 = Coupling(name = 'GC_76',
                 value = 'AD*complex(0,1)*sth1',
                 order = {'HIW':1})

GC_77 = Coupling(name = 'GC_77',
                 value = 'complex(0,1)*GD*sth1',
                 order = {'HIG':1})

GC_78 = Coupling(name = 'GC_78',
                 value = 'GD*gs*sth1',
                 order = {'HIG':1,'QCD':1})

GC_79 = Coupling(name = 'GC_79',
                 value = '-(complex(0,1)*GD*gs**2*sth1)',
                 order = {'HIG':1,'QCD':2})

GC_80 = Coupling(name = 'GC_80',
                 value = '-(cxi**2*complex(0,1)*gwR**2*sth1*cmath.sqrt(2))',
                 order = {'EWR':2})

GC_81 = Coupling(name = 'GC_81',
                 value = '-((cphi*g1*sw)/cw)',
                 order = {'QED':1})

GC_82 = Coupling(name = 'GC_82',
                 value = '-(cphi*complex(0,1)*g1*sw)/(6.*cw)',
                 order = {'QED':1})

GC_83 = Coupling(name = 'GC_83',
                 value = '(cphi*complex(0,1)*g1*sw)/(2.*cw)',
                 order = {'QED':1})

GC_84 = Coupling(name = 'GC_84',
                 value = '(cphi*complex(0,1)*g1*sw)/cw',
                 order = {'QED':1})

GC_85 = Coupling(name = 'GC_85',
                 value = '-(gw*sw)',
                 order = {'EWL':1})

GC_86 = Coupling(name = 'GC_86',
                 value = '-(complex(0,1)*gw*sw)/2.',
                 order = {'EWL':1})

GC_87 = Coupling(name = 'GC_87',
                 value = '(2*complex(0,1)*gw*sw)/3.',
                 order = {'EWL':1})

GC_88 = Coupling(name = 'GC_88',
                 value = '-(complex(0,1)*gw*sw)',
                 order = {'EWL':1})

GC_89 = Coupling(name = 'GC_89',
                 value = 'cxi**2*complex(0,1)*gw*sw',
                 order = {'EWL':1})

GC_90 = Coupling(name = 'GC_90',
                 value = '-2*cphi**2*complex(0,1)*g1*gw*sw',
                 order = {'EWL':1,'QED':1})

GC_91 = Coupling(name = 'GC_91',
                 value = '2*cphi**2*complex(0,1)*g1*gw*sw',
                 order = {'EWL':1,'QED':1})

GC_92 = Coupling(name = 'GC_92',
                 value = '(2*cphi*cxi*complex(0,1)*g1*gw*sw)/cw',
                 order = {'EWL':1,'QED':1})

GC_93 = Coupling(name = 'GC_93',
                 value = '-((cphi*cxi*g1*gw*sw*cmath.sqrt(2))/cw)',
                 order = {'EWL':1,'QED':1})

GC_94 = Coupling(name = 'GC_94',
                 value = '-((cphi*cxi*complex(0,1)*g1*gw*sw*cmath.sqrt(2))/cw)',
                 order = {'EWL':1,'QED':1})

GC_95 = Coupling(name = 'GC_95',
                 value = '(cphi*cxi*g1*gw*sw*cmath.sqrt(2))/cw',
                 order = {'EWL':1,'QED':1})

GC_96 = Coupling(name = 'GC_96',
                 value = '2*cphi*cw*complex(0,1)*gw**2*sw',
                 order = {'EWL':2})

GC_97 = Coupling(name = 'GC_97',
                 value = '-(cxi*complex(0,1)*gw**2*sw)',
                 order = {'EWL':2})

GC_98 = Coupling(name = 'GC_98',
                 value = '-((cxi*gw**2*sw)/cmath.sqrt(2))',
                 order = {'EWL':2})

GC_99 = Coupling(name = 'GC_99',
                 value = '-((cxi*complex(0,1)*gw**2*sw)/cmath.sqrt(2))',
                 order = {'EWL':2})

GC_100 = Coupling(name = 'GC_100',
                  value = '(cxi*gw**2*sw)/cmath.sqrt(2)',
                  order = {'EWL':2})

GC_101 = Coupling(name = 'GC_101',
                  value = '-2*cphi*cw*cxi**2*complex(0,1)*gw**2*sw',
                  order = {'EWL':2})

GC_102 = Coupling(name = 'GC_102',
                  value = '-(complex(0,1)*gwR*sw)/2.',
                  order = {'EWR':1})

GC_103 = Coupling(name = 'GC_103',
                  value = '(complex(0,1)*gwR*sw)/2.',
                  order = {'EWR':1})

GC_104 = Coupling(name = 'GC_104',
                  value = '-(complex(0,1)*gwR*sw)',
                  order = {'EWR':1})

GC_105 = Coupling(name = 'GC_105',
                  value = 'cxi**2*complex(0,1)*gwR*sw',
                  order = {'EWR':1})

GC_106 = Coupling(name = 'GC_106',
                  value = '(complex(0,1)*gw*I51a11*sw)/2.',
                  order = {'EWL':1})

GC_107 = Coupling(name = 'GC_107',
                  value = '-(complex(0,1)*gwR*I51a11*sw)/2.',
                  order = {'EWR':1})

GC_108 = Coupling(name = 'GC_108',
                  value = '(complex(0,1)*gw*I51a22*sw)/2.',
                  order = {'EWL':1})

GC_109 = Coupling(name = 'GC_109',
                  value = '-(complex(0,1)*gwR*I51a22*sw)/2.',
                  order = {'EWR':1})

GC_110 = Coupling(name = 'GC_110',
                  value = '(complex(0,1)*gw*I51a33*sw)/2.',
                  order = {'EWL':1})

GC_111 = Coupling(name = 'GC_111',
                  value = '-(complex(0,1)*gwR*I51a33*sw)/2.',
                  order = {'EWR':1})

GC_112 = Coupling(name = 'GC_112',
                  value = '(complex(0,1)*gw*I51a44*sw)/2.',
                  order = {'EWL':1})

GC_113 = Coupling(name = 'GC_113',
                  value = '-(complex(0,1)*gwR*I51a44*sw)/2.',
                  order = {'EWR':1})

GC_114 = Coupling(name = 'GC_114',
                  value = '(complex(0,1)*gw*I51a55*sw)/2.',
                  order = {'EWL':1})

GC_115 = Coupling(name = 'GC_115',
                  value = '-(complex(0,1)*gwR*I51a55*sw)/2.',
                  order = {'EWR':1})

GC_116 = Coupling(name = 'GC_116',
                  value = '(complex(0,1)*gw*I51a66*sw)/2.',
                  order = {'EWL':1})

GC_117 = Coupling(name = 'GC_117',
                  value = '-(complex(0,1)*gwR*I51a66*sw)/2.',
                  order = {'EWR':1})

GC_118 = Coupling(name = 'GC_118',
                  value = '-(complex(0,1)*gw*I52a11*sw)/2.',
                  order = {'EWL':1})

GC_119 = Coupling(name = 'GC_119',
                  value = '(complex(0,1)*gwR*I52a11*sw)/2.',
                  order = {'EWR':1})

GC_120 = Coupling(name = 'GC_120',
                  value = '-(complex(0,1)*gw*I52a22*sw)/2.',
                  order = {'EWL':1})

GC_121 = Coupling(name = 'GC_121',
                  value = '(complex(0,1)*gwR*I52a22*sw)/2.',
                  order = {'EWR':1})

GC_122 = Coupling(name = 'GC_122',
                  value = '-(complex(0,1)*gw*I52a33*sw)/2.',
                  order = {'EWL':1})

GC_123 = Coupling(name = 'GC_123',
                  value = '(complex(0,1)*gwR*I52a33*sw)/2.',
                  order = {'EWR':1})

GC_124 = Coupling(name = 'GC_124',
                  value = '-(complex(0,1)*gw*I52a44*sw)/2.',
                  order = {'EWL':1})

GC_125 = Coupling(name = 'GC_125',
                  value = '(complex(0,1)*gwR*I52a44*sw)/2.',
                  order = {'EWR':1})

GC_126 = Coupling(name = 'GC_126',
                  value = '-(complex(0,1)*gw*I52a55*sw)/2.',
                  order = {'EWL':1})

GC_127 = Coupling(name = 'GC_127',
                  value = '(complex(0,1)*gwR*I52a55*sw)/2.',
                  order = {'EWR':1})

GC_128 = Coupling(name = 'GC_128',
                  value = '-(complex(0,1)*gw*I52a66*sw)/2.',
                  order = {'EWL':1})

GC_129 = Coupling(name = 'GC_129',
                  value = '(complex(0,1)*gwR*I52a66*sw)/2.',
                  order = {'EWR':1})

GC_130 = Coupling(name = 'GC_130',
                  value = '(complex(0,1)*g1*sphi*sw)/(6.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_131 = Coupling(name = 'GC_131',
                  value = '-(complex(0,1)*g1*sphi*sw)/(2.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_132 = Coupling(name = 'GC_132',
                  value = '-((complex(0,1)*g1*sphi*sw)/cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_133 = Coupling(name = 'GC_133',
                  value = '(g1*sphi*sw)/cw',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_134 = Coupling(name = 'GC_134',
                  value = '-4*cphi*complex(0,1)*g1*gw*sphi*sw',
                  order = {'EWR':1,'QED':1})

GC_135 = Coupling(name = 'GC_135',
                  value = '4*cphi*complex(0,1)*g1*gw*sphi*sw',
                  order = {'EWR':1,'QED':1})

GC_136 = Coupling(name = 'GC_136',
                  value = '(-2*cxi*complex(0,1)*g1*gw*sphi*sw)/cw',
                  order = {'EWR':1,'QED':1})

GC_137 = Coupling(name = 'GC_137',
                  value = '-((cxi*g1*gw*sphi*sw*cmath.sqrt(2))/cw)',
                  order = {'EWR':1,'QED':1})

GC_138 = Coupling(name = 'GC_138',
                  value = '(cxi*complex(0,1)*g1*gw*sphi*sw*cmath.sqrt(2))/cw',
                  order = {'EWR':1,'QED':1})

GC_139 = Coupling(name = 'GC_139',
                  value = '(cxi*g1*gw*sphi*sw*cmath.sqrt(2))/cw',
                  order = {'EWR':1,'QED':1})

GC_140 = Coupling(name = 'GC_140',
                  value = '2*cw*complex(0,1)*gw**2*sphi*sw',
                  order = {'EWL':1,'EWR':1})

GC_141 = Coupling(name = 'GC_141',
                  value = '-2*cw*cxi**2*complex(0,1)*gw**2*sphi*sw',
                  order = {'EWL':1,'EWR':1})

GC_142 = Coupling(name = 'GC_142',
                  value = '-2*complex(0,1)*g1*gw*sphi**2*sw',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_143 = Coupling(name = 'GC_143',
                  value = '2*complex(0,1)*g1*gw*sphi**2*sw',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_144 = Coupling(name = 'GC_144',
                  value = '(2*cphi**2*complex(0,1)*g1**2*sw**2)/cw**2',
                  order = {'QED':2})

GC_145 = Coupling(name = 'GC_145',
                  value = '(2*cphi**2*cth1**2*complex(0,1)*g1**2*sw**2)/cw**2',
                  order = {'QED':2})

GC_146 = Coupling(name = 'GC_146',
                  value = '-(cphi*complex(0,1)*gw*sw**2)/(6.*cw)',
                  order = {'EWL':1})

GC_147 = Coupling(name = 'GC_147',
                  value = '(cphi*complex(0,1)*gw*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_148 = Coupling(name = 'GC_148',
                  value = '(-2*cphi*complex(0,1)*g1*gw*sw**2)/cw',
                  order = {'EWL':1,'QED':1})

GC_149 = Coupling(name = 'GC_149',
                  value = '(2*cphi*complex(0,1)*g1*gw*sw**2)/cw',
                  order = {'EWL':1,'QED':1})

GC_150 = Coupling(name = 'GC_150',
                  value = '2*complex(0,1)*gw**2*sw**2',
                  order = {'EWL':2})

GC_151 = Coupling(name = 'GC_151',
                  value = 'cxi**2*complex(0,1)*gw**2*sw**2',
                  order = {'EWL':2})

GC_152 = Coupling(name = 'GC_152',
                  value = '-(cphi*complex(0,1)*gwR*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_153 = Coupling(name = 'GC_153',
                  value = '(cphi*complex(0,1)*gwR*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_154 = Coupling(name = 'GC_154',
                  value = '(cphi*complex(0,1)*gwR*sw**2)/cw',
                  order = {'EWR':1})

GC_155 = Coupling(name = 'GC_155',
                  value = '-((cphi*cxi**2*complex(0,1)*gwR*sw**2)/cw)',
                  order = {'EWR':1})

GC_156 = Coupling(name = 'GC_156',
                  value = '2*complex(0,1)*gwR**2*sw**2',
                  order = {'EWR':2})

GC_157 = Coupling(name = 'GC_157',
                  value = '(cphi*complex(0,1)*gwR*I51a11*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_158 = Coupling(name = 'GC_158',
                  value = '(cphi*complex(0,1)*gwR*I51a22*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_159 = Coupling(name = 'GC_159',
                  value = '(cphi*complex(0,1)*gwR*I51a33*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_160 = Coupling(name = 'GC_160',
                  value = '(cphi*complex(0,1)*gwR*I51a44*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_161 = Coupling(name = 'GC_161',
                  value = '(cphi*complex(0,1)*gwR*I51a55*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_162 = Coupling(name = 'GC_162',
                  value = '(cphi*complex(0,1)*gwR*I51a66*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_163 = Coupling(name = 'GC_163',
                  value = '-(cphi*complex(0,1)*gwR*I52a11*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_164 = Coupling(name = 'GC_164',
                  value = '-(cphi*complex(0,1)*gwR*I52a22*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_165 = Coupling(name = 'GC_165',
                  value = '-(cphi*complex(0,1)*gwR*I52a33*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_166 = Coupling(name = 'GC_166',
                  value = '-(cphi*complex(0,1)*gwR*I52a44*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_167 = Coupling(name = 'GC_167',
                  value = '-(cphi*complex(0,1)*gwR*I52a55*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_168 = Coupling(name = 'GC_168',
                  value = '-(cphi*complex(0,1)*gwR*I52a66*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_169 = Coupling(name = 'GC_169',
                  value = '-(complex(0,1)*gw*sphi*sw**2)/(6.*cw)',
                  order = {'EWR':1})

GC_170 = Coupling(name = 'GC_170',
                  value = '(complex(0,1)*gw*sphi*sw**2)/(2.*cw)',
                  order = {'EWR':1})

GC_171 = Coupling(name = 'GC_171',
                  value = '(-2*complex(0,1)*g1*gw*sphi*sw**2)/cw',
                  order = {'EWR':1,'QED':1})

GC_172 = Coupling(name = 'GC_172',
                  value = '(2*complex(0,1)*g1*gw*sphi*sw**2)/cw',
                  order = {'EWR':1,'QED':1})

GC_173 = Coupling(name = 'GC_173',
                  value = '-(complex(0,1)*gwR*sphi*sw**2)/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_174 = Coupling(name = 'GC_174',
                  value = '(complex(0,1)*gwR*sphi*sw**2)/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_175 = Coupling(name = 'GC_175',
                  value = '(complex(0,1)*gwR*sphi*sw**2)/cw',
                  order = {'EWL':-1,'EWR':2})

GC_176 = Coupling(name = 'GC_176',
                  value = '-((cxi**2*complex(0,1)*gwR*sphi*sw**2)/cw)',
                  order = {'EWL':-1,'EWR':2})

GC_177 = Coupling(name = 'GC_177',
                  value = '(complex(0,1)*gwR*I51a11*sphi*sw**2)/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_178 = Coupling(name = 'GC_178',
                  value = '(complex(0,1)*gwR*I51a22*sphi*sw**2)/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_179 = Coupling(name = 'GC_179',
                  value = '(complex(0,1)*gwR*I51a33*sphi*sw**2)/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_180 = Coupling(name = 'GC_180',
                  value = '(complex(0,1)*gwR*I51a44*sphi*sw**2)/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_181 = Coupling(name = 'GC_181',
                  value = '(complex(0,1)*gwR*I51a55*sphi*sw**2)/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_182 = Coupling(name = 'GC_182',
                  value = '(complex(0,1)*gwR*I51a66*sphi*sw**2)/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_183 = Coupling(name = 'GC_183',
                  value = '-(complex(0,1)*gwR*I52a11*sphi*sw**2)/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_184 = Coupling(name = 'GC_184',
                  value = '-(complex(0,1)*gwR*I52a22*sphi*sw**2)/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_185 = Coupling(name = 'GC_185',
                  value = '-(complex(0,1)*gwR*I52a33*sphi*sw**2)/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_186 = Coupling(name = 'GC_186',
                  value = '-(complex(0,1)*gwR*I52a44*sphi*sw**2)/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_187 = Coupling(name = 'GC_187',
                  value = '-(complex(0,1)*gwR*I52a55*sphi*sw**2)/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_188 = Coupling(name = 'GC_188',
                  value = '-(complex(0,1)*gwR*I52a66*sphi*sw**2)/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_189 = Coupling(name = 'GC_189',
                  value = '(2*complex(0,1)*g1**2*sphi**2*sw**2)/cw**2',
                  order = {'EWL':-2,'EWR':2,'QED':2})

GC_190 = Coupling(name = 'GC_190',
                  value = '(2*cth1**2*complex(0,1)*g1**2*sphi**2*sw**2)/cw**2',
                  order = {'EWL':-2,'EWR':2,'QED':2})

GC_191 = Coupling(name = 'GC_191',
                  value = '(2*cphi**2*cth1*complex(0,1)*g1**2*sth1*sw**2)/cw**2',
                  order = {'QED':2})

GC_192 = Coupling(name = 'GC_192',
                  value = '(2*cth1*complex(0,1)*g1**2*sphi**2*sth1*sw**2)/cw**2',
                  order = {'EWL':-2,'EWR':2,'QED':2})

GC_193 = Coupling(name = 'GC_193',
                  value = '(2*cphi**2*complex(0,1)*g1**2*sth1**2*sw**2)/cw**2',
                  order = {'QED':2})

GC_194 = Coupling(name = 'GC_194',
                  value = '(2*complex(0,1)*g1**2*sphi**2*sth1**2*sw**2)/cw**2',
                  order = {'EWL':-2,'EWR':2,'QED':2})

GC_195 = Coupling(name = 'GC_195',
                  value = '(-2*cphi*complex(0,1)*gwR**2*sw**3)/cw',
                  order = {'EWR':2})

GC_196 = Coupling(name = 'GC_196',
                  value = '(-2*complex(0,1)*gwR**2*sphi*sw**3)/cw',
                  order = {'EWL':-1,'EWR':3})

GC_197 = Coupling(name = 'GC_197',
                  value = '(2*cphi**2*complex(0,1)*gwR**2*sw**4)/cw**2',
                  order = {'EWR':2})

GC_198 = Coupling(name = 'GC_198',
                  value = '(-4*cphi*complex(0,1)*g1**2*sphi*sw**4)/cw**2',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_199 = Coupling(name = 'GC_199',
                  value = '(-4*cphi*cth1**2*complex(0,1)*g1**2*sphi*sw**4)/cw**2',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_200 = Coupling(name = 'GC_200',
                  value = '(2*complex(0,1)*gwR**2*sphi**2*sw**4)/cw**2',
                  order = {'EWL':-2,'EWR':4})

GC_201 = Coupling(name = 'GC_201',
                  value = '(-4*cphi*cth1*complex(0,1)*g1**2*sphi*sth1*sw**4)/cw**2',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_202 = Coupling(name = 'GC_202',
                  value = '(-4*cphi*complex(0,1)*g1**2*sphi*sth1**2*sw**4)/cw**2',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_203 = Coupling(name = 'GC_203',
                  value = '-(complex(0,1)*gw*I51a41*sw)/4. - (complex(0,1)*gw*I52a14*sw)/4.',
                  order = {'EWL':1})

GC_204 = Coupling(name = 'GC_204',
                  value = '(complex(0,1)*gwR*I51a41*sw)/4. + (complex(0,1)*gwR*I52a14*sw)/4.',
                  order = {'EWR':1})

GC_205 = Coupling(name = 'GC_205',
                  value = '-(complex(0,1)*gw*I51a52*sw)/4. - (complex(0,1)*gw*I52a25*sw)/4.',
                  order = {'EWL':1})

GC_206 = Coupling(name = 'GC_206',
                  value = '(complex(0,1)*gwR*I51a52*sw)/4. + (complex(0,1)*gwR*I52a25*sw)/4.',
                  order = {'EWR':1})

GC_207 = Coupling(name = 'GC_207',
                  value = '-(complex(0,1)*gw*I51a63*sw)/4. - (complex(0,1)*gw*I52a36*sw)/4.',
                  order = {'EWL':1})

GC_208 = Coupling(name = 'GC_208',
                  value = '(complex(0,1)*gwR*I51a63*sw)/4. + (complex(0,1)*gwR*I52a36*sw)/4.',
                  order = {'EWR':1})

GC_209 = Coupling(name = 'GC_209',
                  value = '(complex(0,1)*gw*I51a14*sw)/4. + (complex(0,1)*gw*I52a41*sw)/4.',
                  order = {'EWL':1})

GC_210 = Coupling(name = 'GC_210',
                  value = '-(complex(0,1)*gwR*I51a14*sw)/4. - (complex(0,1)*gwR*I52a41*sw)/4.',
                  order = {'EWR':1})

GC_211 = Coupling(name = 'GC_211',
                  value = '(complex(0,1)*gw*I51a25*sw)/4. + (complex(0,1)*gw*I52a52*sw)/4.',
                  order = {'EWL':1})

GC_212 = Coupling(name = 'GC_212',
                  value = '-(complex(0,1)*gwR*I51a25*sw)/4. - (complex(0,1)*gwR*I52a52*sw)/4.',
                  order = {'EWR':1})

GC_213 = Coupling(name = 'GC_213',
                  value = '(complex(0,1)*gw*I51a36*sw)/4. + (complex(0,1)*gw*I52a63*sw)/4.',
                  order = {'EWL':1})

GC_214 = Coupling(name = 'GC_214',
                  value = '-(complex(0,1)*gwR*I51a36*sw)/4. - (complex(0,1)*gwR*I52a63*sw)/4.',
                  order = {'EWR':1})

GC_215 = Coupling(name = 'GC_215',
                  value = '-(cphi*complex(0,1)*g1*I51a11*sw)/(2.*cw) + (cphi*complex(0,1)*g1*I53a11*sw)/(2.*cw)',
                  order = {'QED':1})

GC_216 = Coupling(name = 'GC_216',
                  value = '-(cphi*complex(0,1)*g1*I51a22*sw)/(2.*cw) + (cphi*complex(0,1)*g1*I53a22*sw)/(2.*cw)',
                  order = {'QED':1})

GC_217 = Coupling(name = 'GC_217',
                  value = '-(cphi*complex(0,1)*g1*I51a33*sw)/(2.*cw) + (cphi*complex(0,1)*g1*I53a33*sw)/(2.*cw)',
                  order = {'QED':1})

GC_218 = Coupling(name = 'GC_218',
                  value = '-(cphi*complex(0,1)*g1*I51a44*sw)/(2.*cw) + (cphi*complex(0,1)*g1*I53a44*sw)/(2.*cw)',
                  order = {'QED':1})

GC_219 = Coupling(name = 'GC_219',
                  value = '-(cphi*complex(0,1)*g1*I51a55*sw)/(2.*cw) + (cphi*complex(0,1)*g1*I53a55*sw)/(2.*cw)',
                  order = {'QED':1})

GC_220 = Coupling(name = 'GC_220',
                  value = '-(cphi*complex(0,1)*g1*I51a66*sw)/(2.*cw) + (cphi*complex(0,1)*g1*I53a66*sw)/(2.*cw)',
                  order = {'QED':1})

GC_221 = Coupling(name = 'GC_221',
                  value = '(cphi*complex(0,1)*g1*I52a11*sw)/(2.*cw) - (cphi*complex(0,1)*g1*I54a11*sw)/(2.*cw)',
                  order = {'QED':1})

GC_222 = Coupling(name = 'GC_222',
                  value = '(cphi*complex(0,1)*g1*I51a41*sw)/(4.*cw) + (cphi*complex(0,1)*g1*I52a14*sw)/(4.*cw) - (cphi*complex(0,1)*g1*I53a41*sw)/(4.*cw) - (cphi*complex(0,1)*g1*I54a14*sw)/(4.*cw)',
                  order = {'QED':1})

GC_223 = Coupling(name = 'GC_223',
                  value = '(cphi*complex(0,1)*g1*I52a22*sw)/(2.*cw) - (cphi*complex(0,1)*g1*I54a22*sw)/(2.*cw)',
                  order = {'QED':1})

GC_224 = Coupling(name = 'GC_224',
                  value = '(cphi*complex(0,1)*g1*I51a52*sw)/(4.*cw) + (cphi*complex(0,1)*g1*I52a25*sw)/(4.*cw) - (cphi*complex(0,1)*g1*I53a52*sw)/(4.*cw) - (cphi*complex(0,1)*g1*I54a25*sw)/(4.*cw)',
                  order = {'QED':1})

GC_225 = Coupling(name = 'GC_225',
                  value = '(cphi*complex(0,1)*g1*I52a33*sw)/(2.*cw) - (cphi*complex(0,1)*g1*I54a33*sw)/(2.*cw)',
                  order = {'QED':1})

GC_226 = Coupling(name = 'GC_226',
                  value = '(cphi*complex(0,1)*g1*I51a63*sw)/(4.*cw) + (cphi*complex(0,1)*g1*I52a36*sw)/(4.*cw) - (cphi*complex(0,1)*g1*I53a63*sw)/(4.*cw) - (cphi*complex(0,1)*g1*I54a36*sw)/(4.*cw)',
                  order = {'QED':1})

GC_227 = Coupling(name = 'GC_227',
                  value = '-(cphi*complex(0,1)*g1*I51a14*sw)/(4.*cw) - (cphi*complex(0,1)*g1*I52a41*sw)/(4.*cw) + (cphi*complex(0,1)*g1*I53a14*sw)/(4.*cw) + (cphi*complex(0,1)*g1*I54a41*sw)/(4.*cw)',
                  order = {'QED':1})

GC_228 = Coupling(name = 'GC_228',
                  value = '(cphi*complex(0,1)*g1*I52a44*sw)/(2.*cw) - (cphi*complex(0,1)*g1*I54a44*sw)/(2.*cw)',
                  order = {'QED':1})

GC_229 = Coupling(name = 'GC_229',
                  value = '-(cphi*complex(0,1)*g1*I51a25*sw)/(4.*cw) - (cphi*complex(0,1)*g1*I52a52*sw)/(4.*cw) + (cphi*complex(0,1)*g1*I53a25*sw)/(4.*cw) + (cphi*complex(0,1)*g1*I54a52*sw)/(4.*cw)',
                  order = {'QED':1})

GC_230 = Coupling(name = 'GC_230',
                  value = '(cphi*complex(0,1)*g1*I52a55*sw)/(2.*cw) - (cphi*complex(0,1)*g1*I54a55*sw)/(2.*cw)',
                  order = {'QED':1})

GC_231 = Coupling(name = 'GC_231',
                  value = '-(cphi*complex(0,1)*g1*I51a36*sw)/(4.*cw) - (cphi*complex(0,1)*g1*I52a63*sw)/(4.*cw) + (cphi*complex(0,1)*g1*I53a36*sw)/(4.*cw) + (cphi*complex(0,1)*g1*I54a63*sw)/(4.*cw)',
                  order = {'QED':1})

GC_232 = Coupling(name = 'GC_232',
                  value = '(cphi*complex(0,1)*g1*I52a66*sw)/(2.*cw) - (cphi*complex(0,1)*g1*I54a66*sw)/(2.*cw)',
                  order = {'QED':1})

GC_233 = Coupling(name = 'GC_233',
                  value = '(complex(0,1)*g1*I51a11*sphi*sw)/(2.*cw) - (complex(0,1)*g1*I53a11*sphi*sw)/(2.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_234 = Coupling(name = 'GC_234',
                  value = '(complex(0,1)*g1*I51a22*sphi*sw)/(2.*cw) - (complex(0,1)*g1*I53a22*sphi*sw)/(2.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_235 = Coupling(name = 'GC_235',
                  value = '(complex(0,1)*g1*I51a33*sphi*sw)/(2.*cw) - (complex(0,1)*g1*I53a33*sphi*sw)/(2.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_236 = Coupling(name = 'GC_236',
                  value = '(complex(0,1)*g1*I51a44*sphi*sw)/(2.*cw) - (complex(0,1)*g1*I53a44*sphi*sw)/(2.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_237 = Coupling(name = 'GC_237',
                  value = '(complex(0,1)*g1*I51a55*sphi*sw)/(2.*cw) - (complex(0,1)*g1*I53a55*sphi*sw)/(2.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_238 = Coupling(name = 'GC_238',
                  value = '(complex(0,1)*g1*I51a66*sphi*sw)/(2.*cw) - (complex(0,1)*g1*I53a66*sphi*sw)/(2.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_239 = Coupling(name = 'GC_239',
                  value = '-(complex(0,1)*g1*I52a11*sphi*sw)/(2.*cw) + (complex(0,1)*g1*I54a11*sphi*sw)/(2.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_240 = Coupling(name = 'GC_240',
                  value = '-(complex(0,1)*g1*I51a41*sphi*sw)/(4.*cw) - (complex(0,1)*g1*I52a14*sphi*sw)/(4.*cw) + (complex(0,1)*g1*I53a41*sphi*sw)/(4.*cw) + (complex(0,1)*g1*I54a14*sphi*sw)/(4.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_241 = Coupling(name = 'GC_241',
                  value = '-(complex(0,1)*g1*I52a22*sphi*sw)/(2.*cw) + (complex(0,1)*g1*I54a22*sphi*sw)/(2.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_242 = Coupling(name = 'GC_242',
                  value = '-(complex(0,1)*g1*I51a52*sphi*sw)/(4.*cw) - (complex(0,1)*g1*I52a25*sphi*sw)/(4.*cw) + (complex(0,1)*g1*I53a52*sphi*sw)/(4.*cw) + (complex(0,1)*g1*I54a25*sphi*sw)/(4.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_243 = Coupling(name = 'GC_243',
                  value = '-(complex(0,1)*g1*I52a33*sphi*sw)/(2.*cw) + (complex(0,1)*g1*I54a33*sphi*sw)/(2.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_244 = Coupling(name = 'GC_244',
                  value = '-(complex(0,1)*g1*I51a63*sphi*sw)/(4.*cw) - (complex(0,1)*g1*I52a36*sphi*sw)/(4.*cw) + (complex(0,1)*g1*I53a63*sphi*sw)/(4.*cw) + (complex(0,1)*g1*I54a36*sphi*sw)/(4.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_245 = Coupling(name = 'GC_245',
                  value = '(complex(0,1)*g1*I51a14*sphi*sw)/(4.*cw) + (complex(0,1)*g1*I52a41*sphi*sw)/(4.*cw) - (complex(0,1)*g1*I53a14*sphi*sw)/(4.*cw) - (complex(0,1)*g1*I54a41*sphi*sw)/(4.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_246 = Coupling(name = 'GC_246',
                  value = '-(complex(0,1)*g1*I52a44*sphi*sw)/(2.*cw) + (complex(0,1)*g1*I54a44*sphi*sw)/(2.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_247 = Coupling(name = 'GC_247',
                  value = '(complex(0,1)*g1*I51a25*sphi*sw)/(4.*cw) + (complex(0,1)*g1*I52a52*sphi*sw)/(4.*cw) - (complex(0,1)*g1*I53a25*sphi*sw)/(4.*cw) - (complex(0,1)*g1*I54a52*sphi*sw)/(4.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_248 = Coupling(name = 'GC_248',
                  value = '-(complex(0,1)*g1*I52a55*sphi*sw)/(2.*cw) + (complex(0,1)*g1*I54a55*sphi*sw)/(2.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_249 = Coupling(name = 'GC_249',
                  value = '(complex(0,1)*g1*I51a36*sphi*sw)/(4.*cw) + (complex(0,1)*g1*I52a63*sphi*sw)/(4.*cw) - (complex(0,1)*g1*I53a36*sphi*sw)/(4.*cw) - (complex(0,1)*g1*I54a63*sphi*sw)/(4.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_250 = Coupling(name = 'GC_250',
                  value = '-(complex(0,1)*g1*I52a66*sphi*sw)/(2.*cw) + (complex(0,1)*g1*I54a66*sphi*sw)/(2.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_251 = Coupling(name = 'GC_251',
                  value = '(complex(0,1)*g1*cmath.sqrt(1 - 2*sw**2))/6.',
                  order = {'QED':1})

GC_252 = Coupling(name = 'GC_252',
                  value = '-(complex(0,1)*g1*cmath.sqrt(1 - 2*sw**2))',
                  order = {'QED':1})

GC_253 = Coupling(name = 'GC_253',
                  value = 'g1*cmath.sqrt(1 - 2*sw**2)',
                  order = {'QED':1})

GC_254 = Coupling(name = 'GC_254',
                  value = '-2*cxi*complex(0,1)*g1*gw*cmath.sqrt(1 - 2*sw**2)',
                  order = {'EWL':1,'QED':1})

GC_255 = Coupling(name = 'GC_255',
                  value = '-(cxi*g1*gw*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2))',
                  order = {'EWL':1,'QED':1})

GC_256 = Coupling(name = 'GC_256',
                  value = 'cxi*complex(0,1)*g1*gw*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2)',
                  order = {'EWL':1,'QED':1})

GC_257 = Coupling(name = 'GC_257',
                  value = 'cxi*g1*gw*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2)',
                  order = {'EWL':1,'QED':1})

GC_258 = Coupling(name = 'GC_258',
                  value = '-(cphi*complex(0,1)*gwR*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_259 = Coupling(name = 'GC_259',
                  value = '(cphi*complex(0,1)*gwR*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_260 = Coupling(name = 'GC_260',
                  value = '-((cphi*complex(0,1)*gwR*cmath.sqrt(1 - 2*sw**2))/cw)',
                  order = {'EWR':1})

GC_261 = Coupling(name = 'GC_261',
                  value = '(cphi*cxi**2*complex(0,1)*gwR*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWR':1})

GC_262 = Coupling(name = 'GC_262',
                  value = '-(complex(0,1)*gwR*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_263 = Coupling(name = 'GC_263',
                  value = '(complex(0,1)*gwR*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_264 = Coupling(name = 'GC_264',
                  value = '(complex(0,1)*gwR*sphi*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':2})

GC_265 = Coupling(name = 'GC_265',
                  value = '(complex(0,1)*gwR*I51a11*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_266 = Coupling(name = 'GC_266',
                  value = '(complex(0,1)*gwR*I51a22*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_267 = Coupling(name = 'GC_267',
                  value = '(complex(0,1)*gwR*I51a33*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_268 = Coupling(name = 'GC_268',
                  value = '(complex(0,1)*gwR*I51a44*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_269 = Coupling(name = 'GC_269',
                  value = '(complex(0,1)*gwR*I51a55*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_270 = Coupling(name = 'GC_270',
                  value = '(complex(0,1)*gwR*I51a66*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_271 = Coupling(name = 'GC_271',
                  value = '-(complex(0,1)*gwR*I52a11*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_272 = Coupling(name = 'GC_272',
                  value = '-(complex(0,1)*gwR*I52a22*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_273 = Coupling(name = 'GC_273',
                  value = '-(complex(0,1)*gwR*I52a33*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_274 = Coupling(name = 'GC_274',
                  value = '-(complex(0,1)*gwR*I52a44*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_275 = Coupling(name = 'GC_275',
                  value = '-(complex(0,1)*gwR*I52a55*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_276 = Coupling(name = 'GC_276',
                  value = '-(complex(0,1)*gwR*I52a66*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_277 = Coupling(name = 'GC_277',
                  value = '-((cphi*g1*sw*cmath.sqrt(1 - 2*sw**2))/cw)',
                  order = {'QED':1})

GC_278 = Coupling(name = 'GC_278',
                  value = '-(cphi*complex(0,1)*g1*sw*cmath.sqrt(1 - 2*sw**2))/(6.*cw)',
                  order = {'QED':1})

GC_279 = Coupling(name = 'GC_279',
                  value = '(cphi*complex(0,1)*g1*sw*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'QED':1})

GC_280 = Coupling(name = 'GC_280',
                  value = '(-2*cphi*complex(0,1)*g1**2*sw*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'QED':2})

GC_281 = Coupling(name = 'GC_281',
                  value = '(-2*cphi*cth1**2*complex(0,1)*g1**2*sw*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'QED':2})

GC_282 = Coupling(name = 'GC_282',
                  value = '-4*complex(0,1)*g1*gw*sw*cmath.sqrt(1 - 2*sw**2)',
                  order = {'EWL':1,'QED':1})

GC_283 = Coupling(name = 'GC_283',
                  value = '4*complex(0,1)*g1*gw*sw*cmath.sqrt(1 - 2*sw**2)',
                  order = {'EWL':1,'QED':1})

GC_284 = Coupling(name = 'GC_284',
                  value = '-4*cphi**2*complex(0,1)*g1*gw*sw*cmath.sqrt(1 - 2*sw**2)',
                  order = {'EWL':1,'QED':1})

GC_285 = Coupling(name = 'GC_285',
                  value = '4*cphi**2*complex(0,1)*g1*gw*sw*cmath.sqrt(1 - 2*sw**2)',
                  order = {'EWL':1,'QED':1})

GC_286 = Coupling(name = 'GC_286',
                  value = '(2*cphi*cxi*complex(0,1)*g1*gw*sw*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':1,'QED':1})

GC_287 = Coupling(name = 'GC_287',
                  value = '-((cphi*cxi*g1*gw*sw*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2))/cw)',
                  order = {'EWL':1,'QED':1})

GC_288 = Coupling(name = 'GC_288',
                  value = '-((cphi*cxi*complex(0,1)*g1*gw*sw*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2))/cw)',
                  order = {'EWL':1,'QED':1})

GC_289 = Coupling(name = 'GC_289',
                  value = '(cphi*cxi*g1*gw*sw*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':1,'QED':1})

GC_290 = Coupling(name = 'GC_290',
                  value = '4*complex(0,1)*g1*gwR*sw*cmath.sqrt(1 - 2*sw**2)',
                  order = {'EWR':1,'QED':1})

GC_291 = Coupling(name = 'GC_291',
                  value = '-4*cth1**2*complex(0,1)*g1*gwR*sw*cmath.sqrt(1 - 2*sw**2)',
                  order = {'EWR':1,'QED':1})

GC_292 = Coupling(name = 'GC_292',
                  value = '(-4*cphi**2*complex(0,1)*g1*gwR*sw*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWR':1,'QED':1})

GC_293 = Coupling(name = 'GC_293',
                  value = '(4*cphi**2*cth1**2*complex(0,1)*g1*gwR*sw*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWR':1,'QED':1})

GC_294 = Coupling(name = 'GC_294',
                  value = '(2*cphi*complex(0,1)*gwR**2*sw*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWR':2})

GC_295 = Coupling(name = 'GC_295',
                  value = '(-2*cphi*cxi**2*complex(0,1)*gwR**2*sw*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWR':2})

GC_296 = Coupling(name = 'GC_296',
                  value = '-((g1*sphi*sw*cmath.sqrt(1 - 2*sw**2))/cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_297 = Coupling(name = 'GC_297',
                  value = '-(complex(0,1)*g1*sphi*sw*cmath.sqrt(1 - 2*sw**2))/(6.*cw)',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_298 = Coupling(name = 'GC_298',
                  value = '(complex(0,1)*g1*sphi*sw*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':1,'QED':1})

GC_299 = Coupling(name = 'GC_299',
                  value = '(2*complex(0,1)*g1**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_300 = Coupling(name = 'GC_300',
                  value = '(2*cth1**2*complex(0,1)*g1**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_301 = Coupling(name = 'GC_301',
                  value = '-4*cphi*complex(0,1)*g1*gw*sphi*sw*cmath.sqrt(1 - 2*sw**2)',
                  order = {'EWR':1,'QED':1})

GC_302 = Coupling(name = 'GC_302',
                  value = '4*cphi*complex(0,1)*g1*gw*sphi*sw*cmath.sqrt(1 - 2*sw**2)',
                  order = {'EWR':1,'QED':1})

GC_303 = Coupling(name = 'GC_303',
                  value = '(2*cxi*complex(0,1)*g1*gw*sphi*sw*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWR':1,'QED':1})

GC_304 = Coupling(name = 'GC_304',
                  value = '-((cxi*g1*gw*sphi*sw*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2))/cw)',
                  order = {'EWR':1,'QED':1})

GC_305 = Coupling(name = 'GC_305',
                  value = '-((cxi*complex(0,1)*g1*gw*sphi*sw*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2))/cw)',
                  order = {'EWR':1,'QED':1})

GC_306 = Coupling(name = 'GC_306',
                  value = '(cxi*g1*gw*sphi*sw*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWR':1,'QED':1})

GC_307 = Coupling(name = 'GC_307',
                  value = '(-2*complex(0,1)*gwR**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':3})

GC_308 = Coupling(name = 'GC_308',
                  value = '(2*cxi**2*complex(0,1)*gwR**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':3})

GC_309 = Coupling(name = 'GC_309',
                  value = '-4*complex(0,1)*g1*gw*sphi**2*sw*cmath.sqrt(1 - 2*sw**2)',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_310 = Coupling(name = 'GC_310',
                  value = '4*complex(0,1)*g1*gw*sphi**2*sw*cmath.sqrt(1 - 2*sw**2)',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_311 = Coupling(name = 'GC_311',
                  value = '(-4*complex(0,1)*g1*gwR*sphi**2*sw*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':3,'QED':1})

GC_312 = Coupling(name = 'GC_312',
                  value = '(4*cth1**2*complex(0,1)*g1*gwR*sphi**2*sw*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':3,'QED':1})

GC_313 = Coupling(name = 'GC_313',
                  value = '(-2*cphi*cth1*complex(0,1)*g1**2*sth1*sw*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'QED':2})

GC_314 = Coupling(name = 'GC_314',
                  value = '-4*cth1*complex(0,1)*g1*gwR*sth1*sw*cmath.sqrt(1 - 2*sw**2)',
                  order = {'EWR':1,'QED':1})

GC_315 = Coupling(name = 'GC_315',
                  value = '(4*cphi**2*cth1*complex(0,1)*g1*gwR*sth1*sw*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWR':1,'QED':1})

GC_316 = Coupling(name = 'GC_316',
                  value = '(2*cth1*complex(0,1)*g1**2*sphi*sth1*sw*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_317 = Coupling(name = 'GC_317',
                  value = '(4*cth1*complex(0,1)*g1*gwR*sphi**2*sth1*sw*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':3,'QED':1})

GC_318 = Coupling(name = 'GC_318',
                  value = '(-2*cphi*complex(0,1)*g1**2*sth1**2*sw*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'QED':2})

GC_319 = Coupling(name = 'GC_319',
                  value = '-4*complex(0,1)*g1*gwR*sth1**2*sw*cmath.sqrt(1 - 2*sw**2)',
                  order = {'EWR':1,'QED':1})

GC_320 = Coupling(name = 'GC_320',
                  value = '(4*cphi**2*complex(0,1)*g1*gwR*sth1**2*sw*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWR':1,'QED':1})

GC_321 = Coupling(name = 'GC_321',
                  value = '(2*complex(0,1)*g1**2*sphi*sth1**2*sw*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_322 = Coupling(name = 'GC_322',
                  value = '(4*complex(0,1)*g1*gwR*sphi**2*sth1**2*sw*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':3,'QED':1})

GC_323 = Coupling(name = 'GC_323',
                  value = '(2*cphi**2*complex(0,1)*g1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'QED':2})

GC_324 = Coupling(name = 'GC_324',
                  value = '(2*cphi**2*cth1**2*complex(0,1)*g1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'QED':2})

GC_325 = Coupling(name = 'GC_325',
                  value = '(-4*cphi*complex(0,1)*g1*gwR*sw**2*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWR':1,'QED':1})

GC_326 = Coupling(name = 'GC_326',
                  value = '(4*cphi*cth1**2*complex(0,1)*g1*gwR*sw**2*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWR':1,'QED':1})

GC_327 = Coupling(name = 'GC_327',
                  value = '(-2*cphi**2*complex(0,1)*gwR**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWR':2})

GC_328 = Coupling(name = 'GC_328',
                  value = '-((cphi**2*cxi**2*complex(0,1)*gwR**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2)',
                  order = {'EWR':2})

GC_329 = Coupling(name = 'GC_329',
                  value = '(-4*cphi*complex(0,1)*g1**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_330 = Coupling(name = 'GC_330',
                  value = '(4*cphi*complex(0,1)*g1**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_331 = Coupling(name = 'GC_331',
                  value = '(-4*cphi*cth1**2*complex(0,1)*g1**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_332 = Coupling(name = 'GC_332',
                  value = '(4*cphi*cth1**2*complex(0,1)*g1**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_333 = Coupling(name = 'GC_333',
                  value = '(-4*complex(0,1)*g1*gwR*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_334 = Coupling(name = 'GC_334',
                  value = '(4*cth1**2*complex(0,1)*g1*gwR*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_335 = Coupling(name = 'GC_335',
                  value = '(-4*cphi*complex(0,1)*gwR**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':3})

GC_336 = Coupling(name = 'GC_336',
                  value = '(4*cphi*complex(0,1)*gwR**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':3})

GC_337 = Coupling(name = 'GC_337',
                  value = '(-2*cphi*cxi**2*complex(0,1)*gwR**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':3})

GC_338 = Coupling(name = 'GC_338',
                  value = '(2*cphi*cxi**2*complex(0,1)*gwR**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':3})

GC_339 = Coupling(name = 'GC_339',
                  value = '(-2*complex(0,1)*g1**2*sphi**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':2,'QED':2})

GC_340 = Coupling(name = 'GC_340',
                  value = '(-2*cth1**2*complex(0,1)*g1**2*sphi**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':2,'QED':2})

GC_341 = Coupling(name = 'GC_341',
                  value = '(2*complex(0,1)*gwR**2*sphi**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':4})

GC_342 = Coupling(name = 'GC_342',
                  value = '(cxi**2*complex(0,1)*gwR**2*sphi**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':4})

GC_343 = Coupling(name = 'GC_343',
                  value = '(2*cphi**2*cth1*complex(0,1)*g1**2*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'QED':2})

GC_344 = Coupling(name = 'GC_344',
                  value = '(4*cphi*cth1*complex(0,1)*g1*gwR*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWR':1,'QED':1})

GC_345 = Coupling(name = 'GC_345',
                  value = '(-4*cphi*cth1*complex(0,1)*g1**2*sphi*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_346 = Coupling(name = 'GC_346',
                  value = '(4*cphi*cth1*complex(0,1)*g1**2*sphi*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_347 = Coupling(name = 'GC_347',
                  value = '(4*cth1*complex(0,1)*g1*gwR*sphi*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_348 = Coupling(name = 'GC_348',
                  value = '(-2*cth1*complex(0,1)*g1**2*sphi**2*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':2,'QED':2})

GC_349 = Coupling(name = 'GC_349',
                  value = '(2*cphi**2*complex(0,1)*g1**2*sth1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'QED':2})

GC_350 = Coupling(name = 'GC_350',
                  value = '(4*cphi*complex(0,1)*g1*gwR*sth1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWR':1,'QED':1})

GC_351 = Coupling(name = 'GC_351',
                  value = '(-4*cphi*complex(0,1)*g1**2*sphi*sth1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_352 = Coupling(name = 'GC_352',
                  value = '(4*cphi*complex(0,1)*g1**2*sphi*sth1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_353 = Coupling(name = 'GC_353',
                  value = '(4*complex(0,1)*g1*gwR*sphi*sth1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_354 = Coupling(name = 'GC_354',
                  value = '(-2*complex(0,1)*g1**2*sphi**2*sth1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':2,'QED':2})

GC_355 = Coupling(name = 'GC_355',
                  value = '(4*cphi**2*complex(0,1)*g1*gwR*sw**3*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWR':1,'QED':1})

GC_356 = Coupling(name = 'GC_356',
                  value = '(-4*cphi**2*cth1**2*complex(0,1)*g1*gwR*sw**3*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWR':1,'QED':1})

GC_357 = Coupling(name = 'GC_357',
                  value = '(4*complex(0,1)*g1*gwR*sphi**2*sw**3*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':3,'QED':1})

GC_358 = Coupling(name = 'GC_358',
                  value = '(-4*cth1**2*complex(0,1)*g1*gwR*sphi**2*sw**3*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':3,'QED':1})

GC_359 = Coupling(name = 'GC_359',
                  value = '(-4*cphi**2*cth1*complex(0,1)*g1*gwR*sth1*sw**3*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWR':1,'QED':1})

GC_360 = Coupling(name = 'GC_360',
                  value = '(-4*cth1*complex(0,1)*g1*gwR*sphi**2*sth1*sw**3*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':3,'QED':1})

GC_361 = Coupling(name = 'GC_361',
                  value = '(-4*cphi**2*complex(0,1)*g1*gwR*sth1**2*sw**3*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWR':1,'QED':1})

GC_362 = Coupling(name = 'GC_362',
                  value = '(-4*complex(0,1)*g1*gwR*sphi**2*sth1**2*sw**3*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':3,'QED':1})

GC_363 = Coupling(name = 'GC_363',
                  value = '2*complex(0,1)*g1**2 - 4*complex(0,1)*g1**2*sw**2',
                  order = {'QED':2})

GC_364 = Coupling(name = 'GC_364',
                  value = '2*cth1**2*complex(0,1)*g1**2 - 4*cth1**2*complex(0,1)*g1**2*sw**2',
                  order = {'QED':2})

GC_365 = Coupling(name = 'GC_365',
                  value = '(2*cphi*complex(0,1)*g1*gwR)/cw - (6*cphi*complex(0,1)*g1*gwR*sw**2)/cw',
                  order = {'EWR':1,'QED':1})

GC_366 = Coupling(name = 'GC_366',
                  value = '(-2*cphi*cth1**2*complex(0,1)*g1*gwR)/cw + (6*cphi*cth1**2*complex(0,1)*g1*gwR*sw**2)/cw',
                  order = {'EWR':1,'QED':1})

GC_367 = Coupling(name = 'GC_367',
                  value = '(2*cphi**2*complex(0,1)*gwR**2)/cw**2 - (4*cphi**2*complex(0,1)*gwR**2*sw**2)/cw**2',
                  order = {'EWR':2})

GC_368 = Coupling(name = 'GC_368',
                  value = '(cphi**2*cxi**2*complex(0,1)*gwR**2)/cw**2 - (2*cphi**2*cxi**2*complex(0,1)*gwR**2*sw**2)/cw**2',
                  order = {'EWR':2})

GC_369 = Coupling(name = 'GC_369',
                  value = '-(cphi*complex(0,1)*gwR*I51a41*sw**2)/(4.*cw) - (cphi*complex(0,1)*gwR*I52a14*sw**2)/(4.*cw)',
                  order = {'EWR':1})

GC_370 = Coupling(name = 'GC_370',
                  value = '-(cphi*complex(0,1)*gwR*I51a52*sw**2)/(4.*cw) - (cphi*complex(0,1)*gwR*I52a25*sw**2)/(4.*cw)',
                  order = {'EWR':1})

GC_371 = Coupling(name = 'GC_371',
                  value = '-(cphi*complex(0,1)*gwR*I51a63*sw**2)/(4.*cw) - (cphi*complex(0,1)*gwR*I52a36*sw**2)/(4.*cw)',
                  order = {'EWR':1})

GC_372 = Coupling(name = 'GC_372',
                  value = '(cphi*complex(0,1)*gwR*I51a14*sw**2)/(4.*cw) + (cphi*complex(0,1)*gwR*I52a41*sw**2)/(4.*cw)',
                  order = {'EWR':1})

GC_373 = Coupling(name = 'GC_373',
                  value = '(cphi*complex(0,1)*gwR*I51a25*sw**2)/(4.*cw) + (cphi*complex(0,1)*gwR*I52a52*sw**2)/(4.*cw)',
                  order = {'EWR':1})

GC_374 = Coupling(name = 'GC_374',
                  value = '(cphi*complex(0,1)*gwR*I51a36*sw**2)/(4.*cw) + (cphi*complex(0,1)*gwR*I52a63*sw**2)/(4.*cw)',
                  order = {'EWR':1})

GC_375 = Coupling(name = 'GC_375',
                  value = '(cphi*cw*complex(0,1)*gw*I53a11)/2. - (cphi*complex(0,1)*gw*I51a11*sw**2)/(2.*cw) + (cphi*complex(0,1)*gw*I53a11*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_376 = Coupling(name = 'GC_376',
                  value = '(cphi*cw*complex(0,1)*gw*I53a22)/2. - (cphi*complex(0,1)*gw*I51a22*sw**2)/(2.*cw) + (cphi*complex(0,1)*gw*I53a22*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_377 = Coupling(name = 'GC_377',
                  value = '(cphi*cw*complex(0,1)*gw*I53a33)/2. - (cphi*complex(0,1)*gw*I51a33*sw**2)/(2.*cw) + (cphi*complex(0,1)*gw*I53a33*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_378 = Coupling(name = 'GC_378',
                  value = '(cphi*cw*complex(0,1)*gw*I53a44)/2. - (cphi*complex(0,1)*gw*I51a44*sw**2)/(2.*cw) + (cphi*complex(0,1)*gw*I53a44*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_379 = Coupling(name = 'GC_379',
                  value = '(cphi*cw*complex(0,1)*gw*I53a55)/2. - (cphi*complex(0,1)*gw*I51a55*sw**2)/(2.*cw) + (cphi*complex(0,1)*gw*I53a55*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_380 = Coupling(name = 'GC_380',
                  value = '(cphi*cw*complex(0,1)*gw*I53a66)/2. - (cphi*complex(0,1)*gw*I51a66*sw**2)/(2.*cw) + (cphi*complex(0,1)*gw*I53a66*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_381 = Coupling(name = 'GC_381',
                  value = '-(cphi*cw*complex(0,1)*gw*I54a11)/2. + (cphi*complex(0,1)*gw*I52a11*sw**2)/(2.*cw) - (cphi*complex(0,1)*gw*I54a11*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_382 = Coupling(name = 'GC_382',
                  value = '-(cphi*cw*complex(0,1)*gw*I53a41)/4. - (cphi*cw*complex(0,1)*gw*I54a14)/4. + (cphi*complex(0,1)*gw*I51a41*sw**2)/(4.*cw) + (cphi*complex(0,1)*gw*I52a14*sw**2)/(4.*cw) - (cphi*complex(0,1)*gw*I53a41*sw**2)/(4.*cw) - (cphi*complex(0,1)*gw*I54a14*sw**2)/(4.*cw)',
                  order = {'EWL':1})

GC_383 = Coupling(name = 'GC_383',
                  value = '-(cphi*cw*complex(0,1)*gw*I54a22)/2. + (cphi*complex(0,1)*gw*I52a22*sw**2)/(2.*cw) - (cphi*complex(0,1)*gw*I54a22*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_384 = Coupling(name = 'GC_384',
                  value = '-(cphi*cw*complex(0,1)*gw*I53a52)/4. - (cphi*cw*complex(0,1)*gw*I54a25)/4. + (cphi*complex(0,1)*gw*I51a52*sw**2)/(4.*cw) + (cphi*complex(0,1)*gw*I52a25*sw**2)/(4.*cw) - (cphi*complex(0,1)*gw*I53a52*sw**2)/(4.*cw) - (cphi*complex(0,1)*gw*I54a25*sw**2)/(4.*cw)',
                  order = {'EWL':1})

GC_385 = Coupling(name = 'GC_385',
                  value = '-(cphi*cw*complex(0,1)*gw*I54a33)/2. + (cphi*complex(0,1)*gw*I52a33*sw**2)/(2.*cw) - (cphi*complex(0,1)*gw*I54a33*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_386 = Coupling(name = 'GC_386',
                  value = '-(cphi*cw*complex(0,1)*gw*I53a63)/4. - (cphi*cw*complex(0,1)*gw*I54a36)/4. + (cphi*complex(0,1)*gw*I51a63*sw**2)/(4.*cw) + (cphi*complex(0,1)*gw*I52a36*sw**2)/(4.*cw) - (cphi*complex(0,1)*gw*I53a63*sw**2)/(4.*cw) - (cphi*complex(0,1)*gw*I54a36*sw**2)/(4.*cw)',
                  order = {'EWL':1})

GC_387 = Coupling(name = 'GC_387',
                  value = '(cphi*cw*complex(0,1)*gw*I53a14)/4. + (cphi*cw*complex(0,1)*gw*I54a41)/4. - (cphi*complex(0,1)*gw*I51a14*sw**2)/(4.*cw) - (cphi*complex(0,1)*gw*I52a41*sw**2)/(4.*cw) + (cphi*complex(0,1)*gw*I53a14*sw**2)/(4.*cw) + (cphi*complex(0,1)*gw*I54a41*sw**2)/(4.*cw)',
                  order = {'EWL':1})

GC_388 = Coupling(name = 'GC_388',
                  value = '-(cphi*cw*complex(0,1)*gw*I54a44)/2. + (cphi*complex(0,1)*gw*I52a44*sw**2)/(2.*cw) - (cphi*complex(0,1)*gw*I54a44*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_389 = Coupling(name = 'GC_389',
                  value = '(cphi*cw*complex(0,1)*gw*I53a25)/4. + (cphi*cw*complex(0,1)*gw*I54a52)/4. - (cphi*complex(0,1)*gw*I51a25*sw**2)/(4.*cw) - (cphi*complex(0,1)*gw*I52a52*sw**2)/(4.*cw) + (cphi*complex(0,1)*gw*I53a25*sw**2)/(4.*cw) + (cphi*complex(0,1)*gw*I54a52*sw**2)/(4.*cw)',
                  order = {'EWL':1})

GC_390 = Coupling(name = 'GC_390',
                  value = '-(cphi*cw*complex(0,1)*gw*I54a55)/2. + (cphi*complex(0,1)*gw*I52a55*sw**2)/(2.*cw) - (cphi*complex(0,1)*gw*I54a55*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_391 = Coupling(name = 'GC_391',
                  value = '(cphi*cw*complex(0,1)*gw*I53a36)/4. + (cphi*cw*complex(0,1)*gw*I54a63)/4. - (cphi*complex(0,1)*gw*I51a36*sw**2)/(4.*cw) - (cphi*complex(0,1)*gw*I52a63*sw**2)/(4.*cw) + (cphi*complex(0,1)*gw*I53a36*sw**2)/(4.*cw) + (cphi*complex(0,1)*gw*I54a63*sw**2)/(4.*cw)',
                  order = {'EWL':1})

GC_392 = Coupling(name = 'GC_392',
                  value = '-(cphi*cw*complex(0,1)*gw*I54a66)/2. + (cphi*complex(0,1)*gw*I52a66*sw**2)/(2.*cw) - (cphi*complex(0,1)*gw*I54a66*sw**2)/(2.*cw)',
                  order = {'EWL':1})

GC_393 = Coupling(name = 'GC_393',
                  value = '(-2*complex(0,1)*g1*gwR*sphi)/cw + (6*complex(0,1)*g1*gwR*sphi*sw**2)/cw',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_394 = Coupling(name = 'GC_394',
                  value = '(2*cth1**2*complex(0,1)*g1*gwR*sphi)/cw - (6*cth1**2*complex(0,1)*g1*gwR*sphi*sw**2)/cw',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_395 = Coupling(name = 'GC_395',
                  value = '-(complex(0,1)*gwR*I51a41*sphi*sw**2)/(4.*cw) - (complex(0,1)*gwR*I52a14*sphi*sw**2)/(4.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_396 = Coupling(name = 'GC_396',
                  value = '-(complex(0,1)*gwR*I51a52*sphi*sw**2)/(4.*cw) - (complex(0,1)*gwR*I52a25*sphi*sw**2)/(4.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_397 = Coupling(name = 'GC_397',
                  value = '-(complex(0,1)*gwR*I51a63*sphi*sw**2)/(4.*cw) - (complex(0,1)*gwR*I52a36*sphi*sw**2)/(4.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_398 = Coupling(name = 'GC_398',
                  value = '(complex(0,1)*gwR*I51a14*sphi*sw**2)/(4.*cw) + (complex(0,1)*gwR*I52a41*sphi*sw**2)/(4.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_399 = Coupling(name = 'GC_399',
                  value = '(complex(0,1)*gwR*I51a25*sphi*sw**2)/(4.*cw) + (complex(0,1)*gwR*I52a52*sphi*sw**2)/(4.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_400 = Coupling(name = 'GC_400',
                  value = '(complex(0,1)*gwR*I51a36*sphi*sw**2)/(4.*cw) + (complex(0,1)*gwR*I52a63*sphi*sw**2)/(4.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_401 = Coupling(name = 'GC_401',
                  value = '(2*complex(0,1)*gwR**2*sphi**2)/cw**2 - (4*complex(0,1)*gwR**2*sphi**2*sw**2)/cw**2',
                  order = {'EWL':-2,'EWR':4})

GC_402 = Coupling(name = 'GC_402',
                  value = '(cxi**2*complex(0,1)*gwR**2*sphi**2)/cw**2 - (2*cxi**2*complex(0,1)*gwR**2*sphi**2*sw**2)/cw**2',
                  order = {'EWL':-2,'EWR':4})

GC_403 = Coupling(name = 'GC_403',
                  value = '2*cth1*complex(0,1)*g1**2*sth1 - 4*cth1*complex(0,1)*g1**2*sth1*sw**2',
                  order = {'QED':2})

GC_404 = Coupling(name = 'GC_404',
                  value = '(-2*cphi*cth1*complex(0,1)*g1*gwR*sth1)/cw + (6*cphi*cth1*complex(0,1)*g1*gwR*sth1*sw**2)/cw',
                  order = {'EWR':1,'QED':1})

GC_405 = Coupling(name = 'GC_405',
                  value = '(2*cth1*complex(0,1)*g1*gwR*sphi*sth1)/cw - (6*cth1*complex(0,1)*g1*gwR*sphi*sth1*sw**2)/cw',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_406 = Coupling(name = 'GC_406',
                  value = '2*complex(0,1)*g1**2*sth1**2 - 4*complex(0,1)*g1**2*sth1**2*sw**2',
                  order = {'QED':2})

GC_407 = Coupling(name = 'GC_407',
                  value = '(-2*cphi*complex(0,1)*g1*gwR*sth1**2)/cw + (6*cphi*complex(0,1)*g1*gwR*sth1**2*sw**2)/cw',
                  order = {'EWR':1,'QED':1})

GC_408 = Coupling(name = 'GC_408',
                  value = '(2*complex(0,1)*g1*gwR*sphi*sth1**2)/cw - (6*complex(0,1)*g1*gwR*sphi*sth1**2*sw**2)/cw',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_409 = Coupling(name = 'GC_409',
                  value = '(-2*cphi*complex(0,1)*g1**2*sw)/cw + (4*cphi*complex(0,1)*g1**2*sw**3)/cw',
                  order = {'QED':2})

GC_410 = Coupling(name = 'GC_410',
                  value = '(-2*cphi*cth1**2*complex(0,1)*g1**2*sw)/cw + (4*cphi*cth1**2*complex(0,1)*g1**2*sw**3)/cw',
                  order = {'QED':2})

GC_411 = Coupling(name = 'GC_411',
                  value = '(-2*cphi**2*complex(0,1)*g1*gwR*sw)/cw**2 + (6*cphi**2*complex(0,1)*g1*gwR*sw**3)/cw**2',
                  order = {'EWR':1,'QED':1})

GC_412 = Coupling(name = 'GC_412',
                  value = '(2*cphi**2*cth1**2*complex(0,1)*g1*gwR*sw)/cw**2 - (6*cphi**2*cth1**2*complex(0,1)*g1*gwR*sw**3)/cw**2',
                  order = {'EWR':1,'QED':1})

GC_413 = Coupling(name = 'GC_413',
                  value = '(-2*complex(0,1)*g1**2*sphi*sw)/cw + (4*complex(0,1)*g1**2*sphi*sw**3)/cw',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_414 = Coupling(name = 'GC_414',
                  value = '(-2*cth1**2*complex(0,1)*g1**2*sphi*sw)/cw + (4*cth1**2*complex(0,1)*g1**2*sphi*sw**3)/cw',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_415 = Coupling(name = 'GC_415',
                  value = '(4*cphi*complex(0,1)*g1*gwR*sphi*sw)/cw**2 - (12*cphi*complex(0,1)*g1*gwR*sphi*sw**3)/cw**2',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_416 = Coupling(name = 'GC_416',
                  value = '(-4*cphi*complex(0,1)*g1*gwR*sphi*sw)/cw**2 + (12*cphi*complex(0,1)*g1*gwR*sphi*sw**3)/cw**2',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_417 = Coupling(name = 'GC_417',
                  value = '(4*cphi*cth1**2*complex(0,1)*g1*gwR*sphi*sw)/cw**2 - (12*cphi*cth1**2*complex(0,1)*g1*gwR*sphi*sw**3)/cw**2',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_418 = Coupling(name = 'GC_418',
                  value = '(-4*cphi*cth1**2*complex(0,1)*g1*gwR*sphi*sw)/cw**2 + (12*cphi*cth1**2*complex(0,1)*g1*gwR*sphi*sw**3)/cw**2',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_419 = Coupling(name = 'GC_419',
                  value = '(2*complex(0,1)*g1*gwR*sphi**2*sw)/cw**2 - (6*complex(0,1)*g1*gwR*sphi**2*sw**3)/cw**2',
                  order = {'EWL':-2,'EWR':3,'QED':1})

GC_420 = Coupling(name = 'GC_420',
                  value = '(-2*cth1**2*complex(0,1)*g1*gwR*sphi**2*sw)/cw**2 + (6*cth1**2*complex(0,1)*g1*gwR*sphi**2*sw**3)/cw**2',
                  order = {'EWL':-2,'EWR':3,'QED':1})

GC_421 = Coupling(name = 'GC_421',
                  value = '(-2*cphi*cth1*complex(0,1)*g1**2*sth1*sw)/cw + (4*cphi*cth1*complex(0,1)*g1**2*sth1*sw**3)/cw',
                  order = {'QED':2})

GC_422 = Coupling(name = 'GC_422',
                  value = '(2*cphi**2*cth1*complex(0,1)*g1*gwR*sth1*sw)/cw**2 - (6*cphi**2*cth1*complex(0,1)*g1*gwR*sth1*sw**3)/cw**2',
                  order = {'EWR':1,'QED':1})

GC_423 = Coupling(name = 'GC_423',
                  value = '(-2*cth1*complex(0,1)*g1**2*sphi*sth1*sw)/cw + (4*cth1*complex(0,1)*g1**2*sphi*sth1*sw**3)/cw',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_424 = Coupling(name = 'GC_424',
                  value = '(4*cphi*cth1*complex(0,1)*g1*gwR*sphi*sth1*sw)/cw**2 - (12*cphi*cth1*complex(0,1)*g1*gwR*sphi*sth1*sw**3)/cw**2',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_425 = Coupling(name = 'GC_425',
                  value = '(-4*cphi*cth1*complex(0,1)*g1*gwR*sphi*sth1*sw)/cw**2 + (12*cphi*cth1*complex(0,1)*g1*gwR*sphi*sth1*sw**3)/cw**2',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_426 = Coupling(name = 'GC_426',
                  value = '(-2*cth1*complex(0,1)*g1*gwR*sphi**2*sth1*sw)/cw**2 + (6*cth1*complex(0,1)*g1*gwR*sphi**2*sth1*sw**3)/cw**2',
                  order = {'EWL':-2,'EWR':3,'QED':1})

GC_427 = Coupling(name = 'GC_427',
                  value = '(-2*cphi*complex(0,1)*g1**2*sth1**2*sw)/cw + (4*cphi*complex(0,1)*g1**2*sth1**2*sw**3)/cw',
                  order = {'QED':2})

GC_428 = Coupling(name = 'GC_428',
                  value = '(2*cphi**2*complex(0,1)*g1*gwR*sth1**2*sw)/cw**2 - (6*cphi**2*complex(0,1)*g1*gwR*sth1**2*sw**3)/cw**2',
                  order = {'EWR':1,'QED':1})

GC_429 = Coupling(name = 'GC_429',
                  value = '(-2*complex(0,1)*g1**2*sphi*sth1**2*sw)/cw + (4*complex(0,1)*g1**2*sphi*sth1**2*sw**3)/cw',
                  order = {'EWL':-1,'EWR':1,'QED':2})

GC_430 = Coupling(name = 'GC_430',
                  value = '(4*cphi*complex(0,1)*g1*gwR*sphi*sth1**2*sw)/cw**2 - (12*cphi*complex(0,1)*g1*gwR*sphi*sth1**2*sw**3)/cw**2',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_431 = Coupling(name = 'GC_431',
                  value = '(-4*cphi*complex(0,1)*g1*gwR*sphi*sth1**2*sw)/cw**2 + (12*cphi*complex(0,1)*g1*gwR*sphi*sth1**2*sw**3)/cw**2',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_432 = Coupling(name = 'GC_432',
                  value = '(-2*complex(0,1)*g1*gwR*sphi**2*sth1**2*sw)/cw**2 + (6*complex(0,1)*g1*gwR*sphi**2*sth1**2*sw**3)/cw**2',
                  order = {'EWL':-2,'EWR':3,'QED':1})

GC_433 = Coupling(name = 'GC_433',
                  value = '(2*cphi**2*complex(0,1)*g1**2*sw**2)/cw**2 - (4*cphi**2*complex(0,1)*g1**2*sw**4)/cw**2',
                  order = {'QED':2})

GC_434 = Coupling(name = 'GC_434',
                  value = '(2*cphi**2*cth1**2*complex(0,1)*g1**2*sw**2)/cw**2 - (4*cphi**2*cth1**2*complex(0,1)*g1**2*sw**4)/cw**2',
                  order = {'QED':2})

GC_435 = Coupling(name = 'GC_435',
                  value = '(-2*cphi*complex(0,1)*gwR**2*sphi)/cw**2 + (4*cphi*complex(0,1)*gwR**2*sphi*sw**2)/cw**2 + (2*cphi*complex(0,1)*gwR**2*sphi*sw**4)/cw**2',
                  order = {'EWL':-1,'EWR':3})

GC_436 = Coupling(name = 'GC_436',
                  value = '(2*complex(0,1)*g1**2*sphi**2*sw**2)/cw**2 - (4*complex(0,1)*g1**2*sphi**2*sw**4)/cw**2',
                  order = {'EWL':-2,'EWR':2,'QED':2})

GC_437 = Coupling(name = 'GC_437',
                  value = '(2*cth1**2*complex(0,1)*g1**2*sphi**2*sw**2)/cw**2 - (4*cth1**2*complex(0,1)*g1**2*sphi**2*sw**4)/cw**2',
                  order = {'EWL':-2,'EWR':2,'QED':2})

GC_438 = Coupling(name = 'GC_438',
                  value = '(2*cphi**2*cth1*complex(0,1)*g1**2*sth1*sw**2)/cw**2 - (4*cphi**2*cth1*complex(0,1)*g1**2*sth1*sw**4)/cw**2',
                  order = {'QED':2})

GC_439 = Coupling(name = 'GC_439',
                  value = '(2*cth1*complex(0,1)*g1**2*sphi**2*sth1*sw**2)/cw**2 - (4*cth1*complex(0,1)*g1**2*sphi**2*sth1*sw**4)/cw**2',
                  order = {'EWL':-2,'EWR':2,'QED':2})

GC_440 = Coupling(name = 'GC_440',
                  value = '(2*cphi**2*complex(0,1)*g1**2*sth1**2*sw**2)/cw**2 - (4*cphi**2*complex(0,1)*g1**2*sth1**2*sw**4)/cw**2',
                  order = {'QED':2})

GC_441 = Coupling(name = 'GC_441',
                  value = '(2*complex(0,1)*g1**2*sphi**2*sth1**2*sw**2)/cw**2 - (4*complex(0,1)*g1**2*sphi**2*sth1**2*sw**4)/cw**2',
                  order = {'EWL':-2,'EWR':2,'QED':2})

GC_442 = Coupling(name = 'GC_442',
                  value = '-(complex(0,1)*gw*sphi*sw**2)/(6.*cw) - (cphi*complex(0,1)*gwR*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_443 = Coupling(name = 'GC_443',
                  value = '(cw*complex(0,1)*gw*I53a11*sphi)/2. - (complex(0,1)*gw*I51a11*sphi*sw**2)/(2.*cw) + (complex(0,1)*gw*I53a11*sphi*sw**2)/(2.*cw) - (cphi*complex(0,1)*gwR*I51a11*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_444 = Coupling(name = 'GC_444',
                  value = '(cw*complex(0,1)*gw*I53a22*sphi)/2. - (complex(0,1)*gw*I51a22*sphi*sw**2)/(2.*cw) + (complex(0,1)*gw*I53a22*sphi*sw**2)/(2.*cw) - (cphi*complex(0,1)*gwR*I51a22*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_445 = Coupling(name = 'GC_445',
                  value = '(cw*complex(0,1)*gw*I53a33*sphi)/2. - (complex(0,1)*gw*I51a33*sphi*sw**2)/(2.*cw) + (complex(0,1)*gw*I53a33*sphi*sw**2)/(2.*cw) - (cphi*complex(0,1)*gwR*I51a33*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_446 = Coupling(name = 'GC_446',
                  value = '(cw*complex(0,1)*gw*I53a44*sphi)/2. - (complex(0,1)*gw*I51a44*sphi*sw**2)/(2.*cw) + (complex(0,1)*gw*I53a44*sphi*sw**2)/(2.*cw) - (cphi*complex(0,1)*gwR*I51a44*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_447 = Coupling(name = 'GC_447',
                  value = '(cw*complex(0,1)*gw*I53a55*sphi)/2. - (complex(0,1)*gw*I51a55*sphi*sw**2)/(2.*cw) + (complex(0,1)*gw*I53a55*sphi*sw**2)/(2.*cw) - (cphi*complex(0,1)*gwR*I51a55*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_448 = Coupling(name = 'GC_448',
                  value = '(cw*complex(0,1)*gw*I53a66*sphi)/2. - (complex(0,1)*gw*I51a66*sphi*sw**2)/(2.*cw) + (complex(0,1)*gw*I53a66*sphi*sw**2)/(2.*cw) - (cphi*complex(0,1)*gwR*I51a66*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_449 = Coupling(name = 'GC_449',
                  value = '-(cw*complex(0,1)*gw*I54a11*sphi)/2. + (complex(0,1)*gw*I52a11*sphi*sw**2)/(2.*cw) - (complex(0,1)*gw*I54a11*sphi*sw**2)/(2.*cw) + (cphi*complex(0,1)*gwR*I52a11*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_450 = Coupling(name = 'GC_450',
                  value = '-(cw*complex(0,1)*gw*I53a41*sphi)/4. - (cw*complex(0,1)*gw*I54a14*sphi)/4. + (complex(0,1)*gw*I51a41*sphi*sw**2)/(4.*cw) + (complex(0,1)*gw*I52a14*sphi*sw**2)/(4.*cw) - (complex(0,1)*gw*I53a41*sphi*sw**2)/(4.*cw) - (complex(0,1)*gw*I54a14*sphi*sw**2)/(4.*cw) + (cphi*complex(0,1)*gwR*I51a41*cmath.sqrt(1 - 2*sw**2))/(4.*cw) + (cphi*complex(0,1)*gwR*I52a14*cmath.sqrt(1 - 2*sw**2))/(4.*cw)',
                  order = {'EWR':1})

GC_451 = Coupling(name = 'GC_451',
                  value = '-(cw*complex(0,1)*gw*I54a22*sphi)/2. + (complex(0,1)*gw*I52a22*sphi*sw**2)/(2.*cw) - (complex(0,1)*gw*I54a22*sphi*sw**2)/(2.*cw) + (cphi*complex(0,1)*gwR*I52a22*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_452 = Coupling(name = 'GC_452',
                  value = '-(cw*complex(0,1)*gw*I53a52*sphi)/4. - (cw*complex(0,1)*gw*I54a25*sphi)/4. + (complex(0,1)*gw*I51a52*sphi*sw**2)/(4.*cw) + (complex(0,1)*gw*I52a25*sphi*sw**2)/(4.*cw) - (complex(0,1)*gw*I53a52*sphi*sw**2)/(4.*cw) - (complex(0,1)*gw*I54a25*sphi*sw**2)/(4.*cw) + (cphi*complex(0,1)*gwR*I51a52*cmath.sqrt(1 - 2*sw**2))/(4.*cw) + (cphi*complex(0,1)*gwR*I52a25*cmath.sqrt(1 - 2*sw**2))/(4.*cw)',
                  order = {'EWR':1})

GC_453 = Coupling(name = 'GC_453',
                  value = '-(cw*complex(0,1)*gw*I54a33*sphi)/2. + (complex(0,1)*gw*I52a33*sphi*sw**2)/(2.*cw) - (complex(0,1)*gw*I54a33*sphi*sw**2)/(2.*cw) + (cphi*complex(0,1)*gwR*I52a33*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_454 = Coupling(name = 'GC_454',
                  value = '-(cw*complex(0,1)*gw*I53a63*sphi)/4. - (cw*complex(0,1)*gw*I54a36*sphi)/4. + (complex(0,1)*gw*I51a63*sphi*sw**2)/(4.*cw) + (complex(0,1)*gw*I52a36*sphi*sw**2)/(4.*cw) - (complex(0,1)*gw*I53a63*sphi*sw**2)/(4.*cw) - (complex(0,1)*gw*I54a36*sphi*sw**2)/(4.*cw) + (cphi*complex(0,1)*gwR*I51a63*cmath.sqrt(1 - 2*sw**2))/(4.*cw) + (cphi*complex(0,1)*gwR*I52a36*cmath.sqrt(1 - 2*sw**2))/(4.*cw)',
                  order = {'EWR':1})

GC_455 = Coupling(name = 'GC_455',
                  value = '(cw*complex(0,1)*gw*I53a14*sphi)/4. + (cw*complex(0,1)*gw*I54a41*sphi)/4. - (complex(0,1)*gw*I51a14*sphi*sw**2)/(4.*cw) - (complex(0,1)*gw*I52a41*sphi*sw**2)/(4.*cw) + (complex(0,1)*gw*I53a14*sphi*sw**2)/(4.*cw) + (complex(0,1)*gw*I54a41*sphi*sw**2)/(4.*cw) - (cphi*complex(0,1)*gwR*I51a14*cmath.sqrt(1 - 2*sw**2))/(4.*cw) - (cphi*complex(0,1)*gwR*I52a41*cmath.sqrt(1 - 2*sw**2))/(4.*cw)',
                  order = {'EWR':1})

GC_456 = Coupling(name = 'GC_456',
                  value = '-(cw*complex(0,1)*gw*I54a44*sphi)/2. + (complex(0,1)*gw*I52a44*sphi*sw**2)/(2.*cw) - (complex(0,1)*gw*I54a44*sphi*sw**2)/(2.*cw) + (cphi*complex(0,1)*gwR*I52a44*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_457 = Coupling(name = 'GC_457',
                  value = '(cw*complex(0,1)*gw*I53a25*sphi)/4. + (cw*complex(0,1)*gw*I54a52*sphi)/4. - (complex(0,1)*gw*I51a25*sphi*sw**2)/(4.*cw) - (complex(0,1)*gw*I52a52*sphi*sw**2)/(4.*cw) + (complex(0,1)*gw*I53a25*sphi*sw**2)/(4.*cw) + (complex(0,1)*gw*I54a52*sphi*sw**2)/(4.*cw) - (cphi*complex(0,1)*gwR*I51a25*cmath.sqrt(1 - 2*sw**2))/(4.*cw) - (cphi*complex(0,1)*gwR*I52a52*cmath.sqrt(1 - 2*sw**2))/(4.*cw)',
                  order = {'EWR':1})

GC_458 = Coupling(name = 'GC_458',
                  value = '-(cw*complex(0,1)*gw*I54a55*sphi)/2. + (complex(0,1)*gw*I52a55*sphi*sw**2)/(2.*cw) - (complex(0,1)*gw*I54a55*sphi*sw**2)/(2.*cw) + (cphi*complex(0,1)*gwR*I52a55*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_459 = Coupling(name = 'GC_459',
                  value = '(cw*complex(0,1)*gw*I53a36*sphi)/4. + (cw*complex(0,1)*gw*I54a63*sphi)/4. - (complex(0,1)*gw*I51a36*sphi*sw**2)/(4.*cw) - (complex(0,1)*gw*I52a63*sphi*sw**2)/(4.*cw) + (complex(0,1)*gw*I53a36*sphi*sw**2)/(4.*cw) + (complex(0,1)*gw*I54a63*sphi*sw**2)/(4.*cw) - (cphi*complex(0,1)*gwR*I51a36*cmath.sqrt(1 - 2*sw**2))/(4.*cw) - (cphi*complex(0,1)*gwR*I52a63*cmath.sqrt(1 - 2*sw**2))/(4.*cw)',
                  order = {'EWR':1})

GC_460 = Coupling(name = 'GC_460',
                  value = '-(cw*complex(0,1)*gw*I54a66*sphi)/2. + (complex(0,1)*gw*I52a66*sphi*sw**2)/(2.*cw) - (complex(0,1)*gw*I54a66*sphi*sw**2)/(2.*cw) + (cphi*complex(0,1)*gwR*I52a66*cmath.sqrt(1 - 2*sw**2))/(2.*cw)',
                  order = {'EWR':1})

GC_461 = Coupling(name = 'GC_461',
                  value = '-(complex(0,1)*gwR*I51a41*sphi*cmath.sqrt(1 - 2*sw**2))/(4.*cw) - (complex(0,1)*gwR*I52a14*sphi*cmath.sqrt(1 - 2*sw**2))/(4.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_462 = Coupling(name = 'GC_462',
                  value = '-(complex(0,1)*gwR*I51a52*sphi*cmath.sqrt(1 - 2*sw**2))/(4.*cw) - (complex(0,1)*gwR*I52a25*sphi*cmath.sqrt(1 - 2*sw**2))/(4.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_463 = Coupling(name = 'GC_463',
                  value = '-(complex(0,1)*gwR*I51a63*sphi*cmath.sqrt(1 - 2*sw**2))/(4.*cw) - (complex(0,1)*gwR*I52a36*sphi*cmath.sqrt(1 - 2*sw**2))/(4.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_464 = Coupling(name = 'GC_464',
                  value = '(complex(0,1)*gwR*I51a14*sphi*cmath.sqrt(1 - 2*sw**2))/(4.*cw) + (complex(0,1)*gwR*I52a41*sphi*cmath.sqrt(1 - 2*sw**2))/(4.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_465 = Coupling(name = 'GC_465',
                  value = '(complex(0,1)*gwR*I51a25*sphi*cmath.sqrt(1 - 2*sw**2))/(4.*cw) + (complex(0,1)*gwR*I52a52*sphi*cmath.sqrt(1 - 2*sw**2))/(4.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_466 = Coupling(name = 'GC_466',
                  value = '(complex(0,1)*gwR*I51a36*sphi*cmath.sqrt(1 - 2*sw**2))/(4.*cw) + (complex(0,1)*gwR*I52a63*sphi*cmath.sqrt(1 - 2*sw**2))/(4.*cw)',
                  order = {'EWL':-1,'EWR':2})

GC_467 = Coupling(name = 'GC_467',
                  value = '2*cphi*cw*complex(0,1)*g1*gw*cmath.sqrt(1 - 2*sw**2) - (2*cphi*complex(0,1)*g1*gw*sw**2*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':1,'QED':1})

GC_468 = Coupling(name = 'GC_468',
                  value = '-2*cphi*cw*complex(0,1)*g1*gw*cmath.sqrt(1 - 2*sw**2) + (2*cphi*complex(0,1)*g1*gw*sw**2*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':1,'QED':1})

GC_469 = Coupling(name = 'GC_469',
                  value = '2*cw*complex(0,1)*g1*gw*sphi*cmath.sqrt(1 - 2*sw**2) - (2*complex(0,1)*g1*gw*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWR':1,'QED':1})

GC_470 = Coupling(name = 'GC_470',
                  value = '-2*cw*complex(0,1)*g1*gw*sphi*cmath.sqrt(1 - 2*sw**2) + (2*complex(0,1)*g1*gw*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWR':1,'QED':1})

GC_471 = Coupling(name = 'GC_471',
                  value = '(4*cphi*complex(0,1)*g1*gwR*sphi*sw*cmath.sqrt(1 - 2*sw**2))/cw**2 + (4*cphi*complex(0,1)*g1*gwR*sphi*sw**3*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_472 = Coupling(name = 'GC_472',
                  value = '(-4*cphi*cth1**2*complex(0,1)*g1*gwR*sphi*sw*cmath.sqrt(1 - 2*sw**2))/cw**2 - (4*cphi*cth1**2*complex(0,1)*g1*gwR*sphi*sw**3*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_473 = Coupling(name = 'GC_473',
                  value = '(-4*cphi*cth1*complex(0,1)*g1*gwR*sphi*sth1*sw*cmath.sqrt(1 - 2*sw**2))/cw**2 - (4*cphi*cth1*complex(0,1)*g1*gwR*sphi*sth1*sw**3*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_474 = Coupling(name = 'GC_474',
                  value = '(-4*cphi*complex(0,1)*g1*gwR*sphi*sth1**2*sw*cmath.sqrt(1 - 2*sw**2))/cw**2 - (4*cphi*complex(0,1)*g1*gwR*sphi*sth1**2*sw**3*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_475 = Coupling(name = 'GC_475',
                  value = '-(complex(0,1)*gw*sxi)',
                  order = {'EWR':1})

GC_476 = Coupling(name = 'GC_476',
                  value = 'complex(0,1)*gw*sxi',
                  order = {'EWR':1})

GC_477 = Coupling(name = 'GC_477',
                  value = '-((complex(0,1)*gw*sxi)/cmath.sqrt(2))',
                  order = {'EWR':1})

GC_478 = Coupling(name = 'GC_478',
                  value = '(complex(0,1)*gw*sxi)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_479 = Coupling(name = 'GC_479',
                  value = '(gw*sxi)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_480 = Coupling(name = 'GC_480',
                  value = '(CKML1x1*complex(0,1)*gw*sxi)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_481 = Coupling(name = 'GC_481',
                  value = '(CKML1x2*complex(0,1)*gw*sxi)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_482 = Coupling(name = 'GC_482',
                  value = '(CKML1x3*complex(0,1)*gw*sxi)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_483 = Coupling(name = 'GC_483',
                  value = '(CKML2x1*complex(0,1)*gw*sxi)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_484 = Coupling(name = 'GC_484',
                  value = '(CKML2x2*complex(0,1)*gw*sxi)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_485 = Coupling(name = 'GC_485',
                  value = '(CKML2x3*complex(0,1)*gw*sxi)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_486 = Coupling(name = 'GC_486',
                  value = '(CKML3x1*complex(0,1)*gw*sxi)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_487 = Coupling(name = 'GC_487',
                  value = '(CKML3x2*complex(0,1)*gw*sxi)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_488 = Coupling(name = 'GC_488',
                  value = '(CKML3x3*complex(0,1)*gw*sxi)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_489 = Coupling(name = 'GC_489',
                  value = 'cphi*cw*cxi*complex(0,1)*gw*sxi',
                  order = {'EWR':1})

GC_490 = Coupling(name = 'GC_490',
                  value = '-(cphi*cw*complex(0,1)*gw**2*sxi)',
                  order = {'EWL':1,'EWR':1})

GC_491 = Coupling(name = 'GC_491',
                  value = '-((cphi*cw*gw**2*sxi)/cmath.sqrt(2))',
                  order = {'EWL':1,'EWR':1})

GC_492 = Coupling(name = 'GC_492',
                  value = '-((cphi*cw*complex(0,1)*gw**2*sxi)/cmath.sqrt(2))',
                  order = {'EWL':1,'EWR':1})

GC_493 = Coupling(name = 'GC_493',
                  value = '(cphi*cw*gw**2*sxi)/cmath.sqrt(2)',
                  order = {'EWL':1,'EWR':1})

GC_494 = Coupling(name = 'GC_494',
                  value = 'cxi*complex(0,1)*gw**2*sxi',
                  order = {'EWL':1,'EWR':1})

GC_495 = Coupling(name = 'GC_495',
                  value = '2*cxi*complex(0,1)*gw**2*sxi',
                  order = {'EWL':1,'EWR':1})

GC_496 = Coupling(name = 'GC_496',
                  value = '-(cxi*gw**2*sxi*cmath.sqrt(2))',
                  order = {'EWL':1,'EWR':1})

GC_497 = Coupling(name = 'GC_497',
                  value = '-(cxi*complex(0,1)*gw**2*sxi*cmath.sqrt(2))',
                  order = {'EWL':1,'EWR':1})

GC_498 = Coupling(name = 'GC_498',
                  value = 'cxi*gw**2*sxi*cmath.sqrt(2)',
                  order = {'EWL':1,'EWR':1})

GC_499 = Coupling(name = 'GC_499',
                  value = 'cphi**2*cw**2*cxi*complex(0,1)*gw**2*sxi',
                  order = {'EWL':1,'EWR':1})

GC_500 = Coupling(name = 'GC_500',
                  value = '-(cxi**3*complex(0,1)*gw**2*sxi)',
                  order = {'EWL':1,'EWR':1})

GC_501 = Coupling(name = 'GC_501',
                  value = '-((CKMR1x1*complex(0,1)*gwR*sxi)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_502 = Coupling(name = 'GC_502',
                  value = '-((CKMR1x2*complex(0,1)*gwR*sxi)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_503 = Coupling(name = 'GC_503',
                  value = '-((CKMR1x3*complex(0,1)*gwR*sxi)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_504 = Coupling(name = 'GC_504',
                  value = '-((CKMR2x1*complex(0,1)*gwR*sxi)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_505 = Coupling(name = 'GC_505',
                  value = '-((CKMR2x2*complex(0,1)*gwR*sxi)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_506 = Coupling(name = 'GC_506',
                  value = '-((CKMR2x3*complex(0,1)*gwR*sxi)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_507 = Coupling(name = 'GC_507',
                  value = '-((CKMR3x1*complex(0,1)*gwR*sxi)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_508 = Coupling(name = 'GC_508',
                  value = '-((CKMR3x2*complex(0,1)*gwR*sxi)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_509 = Coupling(name = 'GC_509',
                  value = '-((CKMR3x3*complex(0,1)*gwR*sxi)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_510 = Coupling(name = 'GC_510',
                  value = '-(cxi*complex(0,1)*gwR**2*sxi)',
                  order = {'EWL':-1,'EWR':3})

GC_511 = Coupling(name = 'GC_511',
                  value = 'cth1*cxi*complex(0,1)*gwR**2*sxi*cmath.sqrt(2)',
                  order = {'EWL':-1,'EWR':3})

GC_512 = Coupling(name = 'GC_512',
                  value = '(complex(0,1)*gw*KL1x1*sxi)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_513 = Coupling(name = 'GC_513',
                  value = '(complex(0,1)*gw*KL2x2*sxi)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_514 = Coupling(name = 'GC_514',
                  value = '(complex(0,1)*gw*KL3x3*sxi)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_515 = Coupling(name = 'GC_515',
                  value = '(complex(0,1)*gw*KL4x1*sxi)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_516 = Coupling(name = 'GC_516',
                  value = '(complex(0,1)*gw*KL5x2*sxi)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_517 = Coupling(name = 'GC_517',
                  value = '(complex(0,1)*gw*KL6x3*sxi)/cmath.sqrt(2)',
                  order = {'EWR':1})

GC_518 = Coupling(name = 'GC_518',
                  value = '-((complex(0,1)*gwR*KR1x1*sxi)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_519 = Coupling(name = 'GC_519',
                  value = '-((complex(0,1)*gwR*KR2x2*sxi)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_520 = Coupling(name = 'GC_520',
                  value = '-((complex(0,1)*gwR*KR3x3*sxi)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_521 = Coupling(name = 'GC_521',
                  value = '-((complex(0,1)*gwR*KR4x1*sxi)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_522 = Coupling(name = 'GC_522',
                  value = '-((complex(0,1)*gwR*KR5x2*sxi)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_523 = Coupling(name = 'GC_523',
                  value = '-((complex(0,1)*gwR*KR6x3*sxi)/cmath.sqrt(2))',
                  order = {'EWL':-1,'EWR':2})

GC_524 = Coupling(name = 'GC_524',
                  value = '-(cw*complex(0,1)*gw**2*sphi*sxi)',
                  order = {'EWR':2})

GC_525 = Coupling(name = 'GC_525',
                  value = '-((cw*gw**2*sphi*sxi)/cmath.sqrt(2))',
                  order = {'EWR':2})

GC_526 = Coupling(name = 'GC_526',
                  value = '-((cw*complex(0,1)*gw**2*sphi*sxi)/cmath.sqrt(2))',
                  order = {'EWR':2})

GC_527 = Coupling(name = 'GC_527',
                  value = '(cw*gw**2*sphi*sxi)/cmath.sqrt(2)',
                  order = {'EWR':2})

GC_528 = Coupling(name = 'GC_528',
                  value = 'cphi*cw**2*cxi*complex(0,1)*gw**2*sphi*sxi',
                  order = {'EWR':2})

GC_529 = Coupling(name = 'GC_529',
                  value = 'cxi*complex(0,1)*gwR**2*sth1*sxi*cmath.sqrt(2)',
                  order = {'EWL':-1,'EWR':3})

GC_530 = Coupling(name = 'GC_530',
                  value = 'cxi*complex(0,1)*gw*sw*sxi',
                  order = {'EWR':1})

GC_531 = Coupling(name = 'GC_531',
                  value = '(2*cphi*complex(0,1)*g1*gw*sw*sxi)/cw',
                  order = {'EWR':1,'QED':1})

GC_532 = Coupling(name = 'GC_532',
                  value = '-((cphi*g1*gw*sw*sxi*cmath.sqrt(2))/cw)',
                  order = {'EWR':1,'QED':1})

GC_533 = Coupling(name = 'GC_533',
                  value = '-((cphi*complex(0,1)*g1*gw*sw*sxi*cmath.sqrt(2))/cw)',
                  order = {'EWR':1,'QED':1})

GC_534 = Coupling(name = 'GC_534',
                  value = '(cphi*g1*gw*sw*sxi*cmath.sqrt(2))/cw',
                  order = {'EWR':1,'QED':1})

GC_535 = Coupling(name = 'GC_535',
                  value = '-(complex(0,1)*gw**2*sw*sxi)',
                  order = {'EWL':1,'EWR':1})

GC_536 = Coupling(name = 'GC_536',
                  value = '-((gw**2*sw*sxi)/cmath.sqrt(2))',
                  order = {'EWL':1,'EWR':1})

GC_537 = Coupling(name = 'GC_537',
                  value = '-((complex(0,1)*gw**2*sw*sxi)/cmath.sqrt(2))',
                  order = {'EWL':1,'EWR':1})

GC_538 = Coupling(name = 'GC_538',
                  value = '(gw**2*sw*sxi)/cmath.sqrt(2)',
                  order = {'EWL':1,'EWR':1})

GC_539 = Coupling(name = 'GC_539',
                  value = '-2*cphi*cw*cxi*complex(0,1)*gw**2*sw*sxi',
                  order = {'EWL':1,'EWR':1})

GC_540 = Coupling(name = 'GC_540',
                  value = '-(cxi*complex(0,1)*gwR*sw*sxi)',
                  order = {'EWL':-1,'EWR':2})

GC_541 = Coupling(name = 'GC_541',
                  value = '(-2*complex(0,1)*g1*gw*sphi*sw*sxi)/cw',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_542 = Coupling(name = 'GC_542',
                  value = '-((g1*gw*sphi*sw*sxi*cmath.sqrt(2))/cw)',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_543 = Coupling(name = 'GC_543',
                  value = '(complex(0,1)*g1*gw*sphi*sw*sxi*cmath.sqrt(2))/cw',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_544 = Coupling(name = 'GC_544',
                  value = '(g1*gw*sphi*sw*sxi*cmath.sqrt(2))/cw',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_545 = Coupling(name = 'GC_545',
                  value = '-2*cw*cxi*complex(0,1)*gw**2*sphi*sw*sxi',
                  order = {'EWR':2})

GC_546 = Coupling(name = 'GC_546',
                  value = 'cxi*complex(0,1)*gw**2*sw**2*sxi',
                  order = {'EWL':1,'EWR':1})

GC_547 = Coupling(name = 'GC_547',
                  value = '(cphi*cxi*complex(0,1)*gwR*sw**2*sxi)/cw',
                  order = {'EWL':-1,'EWR':2})

GC_548 = Coupling(name = 'GC_548',
                  value = '-(cxi*complex(0,1)*gwR**2*sw**2*sxi)',
                  order = {'EWL':-1,'EWR':3})

GC_549 = Coupling(name = 'GC_549',
                  value = '(cxi*complex(0,1)*gwR*sphi*sw**2*sxi)/cw',
                  order = {'EWL':-2,'EWR':3})

GC_550 = Coupling(name = 'GC_550',
                  value = '(-2*cphi*cxi*complex(0,1)*gwR**2*sw**3*sxi)/cw',
                  order = {'EWL':-1,'EWR':3})

GC_551 = Coupling(name = 'GC_551',
                  value = '(-2*cxi*complex(0,1)*gwR**2*sphi*sw**3*sxi)/cw',
                  order = {'EWL':-2,'EWR':4})

GC_552 = Coupling(name = 'GC_552',
                  value = '-((cphi**2*cxi*complex(0,1)*gwR**2*sw**4*sxi)/cw**2)',
                  order = {'EWL':-1,'EWR':3})

GC_553 = Coupling(name = 'GC_553',
                  value = '-((cxi*complex(0,1)*gwR**2*sphi**2*sw**4*sxi)/cw**2)',
                  order = {'EWL':-3,'EWR':5})

GC_554 = Coupling(name = 'GC_554',
                  value = '-2*complex(0,1)*g1*gw*sxi*cmath.sqrt(1 - 2*sw**2)',
                  order = {'EWR':1,'QED':1})

GC_555 = Coupling(name = 'GC_555',
                  value = '-(g1*gw*sxi*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2))',
                  order = {'EWR':1,'QED':1})

GC_556 = Coupling(name = 'GC_556',
                  value = 'complex(0,1)*g1*gw*sxi*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2)',
                  order = {'EWR':1,'QED':1})

GC_557 = Coupling(name = 'GC_557',
                  value = 'g1*gw*sxi*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2)',
                  order = {'EWR':1,'QED':1})

GC_558 = Coupling(name = 'GC_558',
                  value = '(cxi*complex(0,1)*gwR*sphi*sxi*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-2,'EWR':3})

GC_559 = Coupling(name = 'GC_559',
                  value = '(2*cphi*complex(0,1)*g1*gw*sw*sxi*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWR':1,'QED':1})

GC_560 = Coupling(name = 'GC_560',
                  value = '-((cphi*g1*gw*sw*sxi*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2))/cw)',
                  order = {'EWR':1,'QED':1})

GC_561 = Coupling(name = 'GC_561',
                  value = '-((cphi*complex(0,1)*g1*gw*sw*sxi*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2))/cw)',
                  order = {'EWR':1,'QED':1})

GC_562 = Coupling(name = 'GC_562',
                  value = '(cphi*g1*gw*sw*sxi*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWR':1,'QED':1})

GC_563 = Coupling(name = 'GC_563',
                  value = '(2*cphi*cxi*complex(0,1)*gwR**2*sw*sxi*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':3})

GC_564 = Coupling(name = 'GC_564',
                  value = '(2*complex(0,1)*g1*gw*sphi*sw*sxi*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_565 = Coupling(name = 'GC_565',
                  value = '-((g1*gw*sphi*sw*sxi*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2))/cw)',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_566 = Coupling(name = 'GC_566',
                  value = '-((complex(0,1)*g1*gw*sphi*sw*sxi*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2))/cw)',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_567 = Coupling(name = 'GC_567',
                  value = '(g1*gw*sphi*sw*sxi*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':2,'QED':1})

GC_568 = Coupling(name = 'GC_568',
                  value = '(-2*cxi*complex(0,1)*gwR**2*sphi*sw*sxi*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-2,'EWR':4})

GC_569 = Coupling(name = 'GC_569',
                  value = '(cphi**2*cxi*complex(0,1)*gwR**2*sw**2*sxi*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-1,'EWR':3})

GC_570 = Coupling(name = 'GC_570',
                  value = '(-2*cphi*cxi*complex(0,1)*gwR**2*sphi*sw**2*sxi*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':4})

GC_571 = Coupling(name = 'GC_571',
                  value = '(2*cphi*cxi*complex(0,1)*gwR**2*sphi*sw**2*sxi*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-2,'EWR':4})

GC_572 = Coupling(name = 'GC_572',
                  value = '-((cxi*complex(0,1)*gwR**2*sphi**2*sw**2*sxi*cmath.sqrt(1 - 2*sw**2))/cw**2)',
                  order = {'EWL':-3,'EWR':5})

GC_573 = Coupling(name = 'GC_573',
                  value = 'complex(0,1)*gw**2*sxi**2',
                  order = {'EWR':2})

GC_574 = Coupling(name = 'GC_574',
                  value = '2*complex(0,1)*gw**2*sxi**2',
                  order = {'EWR':2})

GC_575 = Coupling(name = 'GC_575',
                  value = '-(gw**2*sxi**2*cmath.sqrt(2))',
                  order = {'EWR':2})

GC_576 = Coupling(name = 'GC_576',
                  value = '-(complex(0,1)*gw**2*sxi**2*cmath.sqrt(2))',
                  order = {'EWR':2})

GC_577 = Coupling(name = 'GC_577',
                  value = 'gw**2*sxi**2*cmath.sqrt(2)',
                  order = {'EWR':2})

GC_578 = Coupling(name = 'GC_578',
                  value = '-(cxi**2*complex(0,1)*gw**2*sxi**2)',
                  order = {'EWR':2})

GC_579 = Coupling(name = 'GC_579',
                  value = 'complex(0,1)*gwR**2*sxi**2',
                  order = {'EWL':-2,'EWR':4})

GC_580 = Coupling(name = 'GC_580',
                  value = '-(cth1*complex(0,1)*gwR**2*sxi**2*cmath.sqrt(2))',
                  order = {'EWL':-2,'EWR':4})

GC_581 = Coupling(name = 'GC_581',
                  value = '-(cxi**2*complex(0,1)*gwR**2*sxi**2)',
                  order = {'EWL':-2,'EWR':4})

GC_582 = Coupling(name = 'GC_582',
                  value = 'cw*complex(0,1)*gw*sphi*sxi**2',
                  order = {'EWL':-2,'EWR':3})

GC_583 = Coupling(name = 'GC_583',
                  value = '-(complex(0,1)*gwR**2*sth1*sxi**2*cmath.sqrt(2))',
                  order = {'EWL':-2,'EWR':4})

GC_584 = Coupling(name = 'GC_584',
                  value = 'complex(0,1)*gw*sw*sxi**2',
                  order = {'EWL':-1,'EWR':2})

GC_585 = Coupling(name = 'GC_585',
                  value = 'complex(0,1)*gwR*sw*sxi**2',
                  order = {'EWL':-2,'EWR':3})

GC_586 = Coupling(name = 'GC_586',
                  value = '-((cphi*complex(0,1)*gwR*sw**2*sxi**2)/cw)',
                  order = {'EWL':-2,'EWR':3})

GC_587 = Coupling(name = 'GC_587',
                  value = 'complex(0,1)*gwR**2*sw**2*sxi**2',
                  order = {'EWL':-2,'EWR':4})

GC_588 = Coupling(name = 'GC_588',
                  value = '-((complex(0,1)*gwR*sphi*sw**2*sxi**2)/cw)',
                  order = {'EWL':-3,'EWR':4})

GC_589 = Coupling(name = 'GC_589',
                  value = '(2*cphi*complex(0,1)*gwR**2*sw**3*sxi**2)/cw',
                  order = {'EWL':-2,'EWR':4})

GC_590 = Coupling(name = 'GC_590',
                  value = '(2*complex(0,1)*gwR**2*sphi*sw**3*sxi**2)/cw',
                  order = {'EWL':-3,'EWR':5})

GC_591 = Coupling(name = 'GC_591',
                  value = '(cphi**2*complex(0,1)*gwR**2*sw**4*sxi**2)/cw**2',
                  order = {'EWL':-2,'EWR':4})

GC_592 = Coupling(name = 'GC_592',
                  value = '(complex(0,1)*gwR**2*sphi**2*sw**4*sxi**2)/cw**2',
                  order = {'EWL':-4,'EWR':6})

GC_593 = Coupling(name = 'GC_593',
                  value = '(cphi*complex(0,1)*gwR*sxi**2*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-2,'EWR':3})

GC_594 = Coupling(name = 'GC_594',
                  value = '-((complex(0,1)*gwR*sphi*sxi**2*cmath.sqrt(1 - 2*sw**2))/cw)',
                  order = {'EWL':-3,'EWR':4})

GC_595 = Coupling(name = 'GC_595',
                  value = '(-2*cphi*complex(0,1)*gwR**2*sw*sxi**2*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-2,'EWR':4})

GC_596 = Coupling(name = 'GC_596',
                  value = '(2*complex(0,1)*gwR**2*sphi*sw*sxi**2*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-3,'EWR':5})

GC_597 = Coupling(name = 'GC_597',
                  value = '-((cphi**2*complex(0,1)*gwR**2*sw**2*sxi**2*cmath.sqrt(1 - 2*sw**2))/cw**2)',
                  order = {'EWL':-2,'EWR':4})

GC_598 = Coupling(name = 'GC_598',
                  value = '(-2*cphi*complex(0,1)*gwR**2*sphi*sw**2*sxi**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-3,'EWR':5})

GC_599 = Coupling(name = 'GC_599',
                  value = '(2*cphi*complex(0,1)*gwR**2*sphi*sw**2*sxi**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-3,'EWR':5})

GC_600 = Coupling(name = 'GC_600',
                  value = '(complex(0,1)*gwR**2*sphi**2*sw**2*sxi**2*cmath.sqrt(1 - 2*sw**2))/cw**2',
                  order = {'EWL':-4,'EWR':6})

GC_601 = Coupling(name = 'GC_601',
                  value = 'cxi*complex(0,1)*gwR**2*sxi**3',
                  order = {'EWL':-3,'EWR':5})

GC_602 = Coupling(name = 'GC_602',
                  value = '-(complex(0,1)*gw**2*sxi**4)',
                  order = {'EWL':-2,'EWR':4})

GC_603 = Coupling(name = 'GC_603',
                  value = '-(complex(0,1)*gwR**2*sxi**4)',
                  order = {'EWL':-4,'EWR':6})

GC_604 = Coupling(name = 'GC_604',
                  value = '-((cphi**2*cxi*complex(0,1)*gwR**2*sxi)/cw**2) + cw**2*cxi*complex(0,1)*gw**2*sphi**2*sxi + (2*cphi**2*cxi*complex(0,1)*gwR**2*sw**2*sxi)/cw**2',
                  order = {'EWL':-1,'EWR':3})

GC_605 = Coupling(name = 'GC_605',
                  value = '-((cxi*complex(0,1)*gwR**2*sphi**2*sxi)/cw**2) + (2*cxi*complex(0,1)*gwR**2*sphi**2*sw**2*sxi)/cw**2',
                  order = {'EWL':-3,'EWR':5})

GC_606 = Coupling(name = 'GC_606',
                  value = '(cphi*cxi*complex(0,1)*gwR**2*sphi*sxi)/cw**2 - (2*cphi*cxi*complex(0,1)*gwR**2*sphi*sw**2*sxi)/cw**2 - (cphi*cxi*complex(0,1)*gwR**2*sphi*sw**4*sxi)/cw**2',
                  order = {'EWL':-2,'EWR':4})

GC_607 = Coupling(name = 'GC_607',
                  value = 'cw*cxi*complex(0,1)*gw*sphi*sxi - (cphi*cxi*complex(0,1)*gwR*sxi*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':2})

GC_608 = Coupling(name = 'GC_608',
                  value = 'cphi*cw*complex(0,1)*gw*sxi**2 - (cxi**2*complex(0,1)*gwR*sphi*cmath.sqrt(1 - 2*sw**2))/cw',
                  order = {'EWL':-1,'EWR':2})

GC_609 = Coupling(name = 'GC_609',
                  value = '(cphi**2*cxi**2*complex(0,1)*gwR**2*sw**4)/cw**2 + cphi**2*cw**2*complex(0,1)*gw**2*sxi**2',
                  order = {'EWR':2})

GC_610 = Coupling(name = 'GC_610',
                  value = '-((cphi*cxi**2*complex(0,1)*gwR**2*sphi)/cw**2) + (2*cphi*cxi**2*complex(0,1)*gwR**2*sphi*sw**2)/cw**2 + (cphi*cxi**2*complex(0,1)*gwR**2*sphi*sw**4)/cw**2 + cphi*cw**2*complex(0,1)*gw**2*sphi*sxi**2',
                  order = {'EWL':-1,'EWR':3})

GC_611 = Coupling(name = 'GC_611',
                  value = '(cxi**2*complex(0,1)*gwR**2*sphi**2*sw**4)/cw**2 + cw**2*complex(0,1)*gw**2*sphi**2*sxi**2',
                  order = {'EWL':-2,'EWR':4})

GC_612 = Coupling(name = 'GC_612',
                  value = '(2*cphi*cxi**2*complex(0,1)*gwR**2*sw**3)/cw - 2*cphi*cw*complex(0,1)*gw**2*sw*sxi**2',
                  order = {'EWR':2})

GC_613 = Coupling(name = 'GC_613',
                  value = '(2*cxi**2*complex(0,1)*gwR**2*sphi*sw**3)/cw - 2*cw*complex(0,1)*gw**2*sphi*sw*sxi**2',
                  order = {'EWL':-1,'EWR':3})

GC_614 = Coupling(name = 'GC_614',
                  value = 'cxi**2*complex(0,1)*gwR**2*sw**2 + complex(0,1)*gw**2*sw**2*sxi**2',
                  order = {'EWR':2})

GC_615 = Coupling(name = 'GC_615',
                  value = '(cphi**2*complex(0,1)*gwR**2*sxi**2)/cw**2 - (2*cphi**2*complex(0,1)*gwR**2*sw**2*sxi**2)/cw**2',
                  order = {'EWL':-2,'EWR':4})

GC_616 = Coupling(name = 'GC_616',
                  value = '(complex(0,1)*gwR**2*sphi**2*sxi**2)/cw**2 - (2*complex(0,1)*gwR**2*sphi**2*sw**2*sxi**2)/cw**2',
                  order = {'EWL':-4,'EWR':6})

GC_617 = Coupling(name = 'GC_617',
                  value = '-((cphi*complex(0,1)*gwR**2*sphi*sxi**2)/cw**2) + (2*cphi*complex(0,1)*gwR**2*sphi*sw**2*sxi**2)/cw**2 + (cphi*complex(0,1)*gwR**2*sphi*sw**4*sxi**2)/cw**2',
                  order = {'EWL':-3,'EWR':5})

GC_618 = Coupling(name = 'GC_618',
                  value = 'cxi**3*complex(0,1)*gwR**2*sxi - cxi*complex(0,1)*gw**2*sxi**3',
                  order = {'EWL':-1,'EWR':3})

GC_619 = Coupling(name = 'GC_619',
                  value = '-(cphi*cw*gw*k1**2)/(2.*vev**2) - (cphi*cw*gw*k2**2)/(2.*vev**2)',
                  order = {'EWL':1})

GC_620 = Coupling(name = 'GC_620',
                  value = '-(cphi*cw*complex(0,1)*gw*k1**2)/(2.*vev**2) - (cphi*cw*complex(0,1)*gw*k2**2)/(2.*vev**2)',
                  order = {'EWL':1})

GC_621 = Coupling(name = 'GC_621',
                  value = '-(cxi*complex(0,1)*gw*k1**2)/(2.*vev**2) - (cxi*complex(0,1)*gw*k2**2)/(2.*vev**2)',
                  order = {'EWL':1})

GC_622 = Coupling(name = 'GC_622',
                  value = '(cxi*complex(0,1)*gw*k1**2)/(2.*vev**2) + (cxi*complex(0,1)*gw*k2**2)/(2.*vev**2)',
                  order = {'EWL':1})

GC_623 = Coupling(name = 'GC_623',
                  value = '(cxi*gw*k1**2)/(2.*vev**2) + (cxi*gw*k2**2)/(2.*vev**2)',
                  order = {'EWL':1})

GC_624 = Coupling(name = 'GC_624',
                  value = '(cphi**2*cw**2*complex(0,1)*gw**2*k1**2)/(2.*vev**2) + (cphi**2*cw**2*complex(0,1)*gw**2*k2**2)/(2.*vev**2)',
                  order = {'EWL':2})

GC_625 = Coupling(name = 'GC_625',
                  value = '(cphi**2*cth1**2*cw**2*complex(0,1)*gw**2*k1**2)/(2.*vev**2) + (cphi**2*cth1**2*cw**2*complex(0,1)*gw**2*k2**2)/(2.*vev**2)',
                  order = {'EWL':2})

GC_626 = Coupling(name = 'GC_626',
                  value = '(cxi**2*complex(0,1)*gw**2*k1**2)/(2.*vev**2) + (cxi**2*complex(0,1)*gw**2*k2**2)/(2.*vev**2)',
                  order = {'EWL':2})

GC_627 = Coupling(name = 'GC_627',
                  value = '(cth1**2*cxi**2*complex(0,1)*gw**2*k1**2)/(2.*vev**2) + (cth1**2*cxi**2*complex(0,1)*gw**2*k2**2)/(2.*vev**2)',
                  order = {'EWL':2})

GC_628 = Coupling(name = 'GC_628',
                  value = '(cth1*cxi*complex(0,1)*gwR*k1**2)/(2.*vev**2) - (cth1*cxi*complex(0,1)*gwR*k2**2)/(2.*vev**2)',
                  order = {'EWR':1})

GC_629 = Coupling(name = 'GC_629',
                  value = '-(cth1*cxi*complex(0,1)*gwR*k1**2)/(2.*vev**2) + (cth1*cxi*complex(0,1)*gwR*k2**2)/(2.*vev**2)',
                  order = {'EWR':1})

GC_630 = Coupling(name = 'GC_630',
                  value = '-(cphi*cth1*cw*cxi*complex(0,1)*gw*gwR*k1**2)/(2.*vev**2) + (cphi*cth1*cw*cxi*complex(0,1)*gw*gwR*k2**2)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_631 = Coupling(name = 'GC_631',
                  value = '(cth1*cxi**2*gw*gwR*k1**2)/(2.*vev**2) - (cth1*cxi**2*gw*gwR*k2**2)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_632 = Coupling(name = 'GC_632',
                  value = '-(cth1*cxi**2*complex(0,1)*gw*gwR*k1**2)/(2.*vev**2) + (cth1*cxi**2*complex(0,1)*gw*gwR*k2**2)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_633 = Coupling(name = 'GC_633',
                  value = '-(cth1*cxi**2*gw*gwR*k1**2)/(2.*vev**2) + (cth1*cxi**2*gw*gwR*k2**2)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_634 = Coupling(name = 'GC_634',
                  value = '-(cth1*cw*cxi*complex(0,1)*gw*gwR*k1**2*sphi)/(2.*vev**2) + (cth1*cw*cxi*complex(0,1)*gw*gwR*k2**2*sphi)/(2.*vev**2)',
                  order = {'EWR':2})

GC_635 = Coupling(name = 'GC_635',
                  value = '-(cphi**2*cth1*cw**2*complex(0,1)*gw**2*k1**2*sth1)/(2.*vev**2) - (cphi**2*cth1*cw**2*complex(0,1)*gw**2*k2**2*sth1)/(2.*vev**2)',
                  order = {'EWL':2})

GC_636 = Coupling(name = 'GC_636',
                  value = '-(cth1*cxi**2*complex(0,1)*gw**2*k1**2*sth1)/(2.*vev**2) - (cth1*cxi**2*complex(0,1)*gw**2*k2**2*sth1)/(2.*vev**2)',
                  order = {'EWL':2})

GC_637 = Coupling(name = 'GC_637',
                  value = '(cxi*complex(0,1)*gwR*k1**2*sth1)/(2.*vev**2) - (cxi*complex(0,1)*gwR*k2**2*sth1)/(2.*vev**2)',
                  order = {'EWR':1})

GC_638 = Coupling(name = 'GC_638',
                  value = '-(cxi*complex(0,1)*gwR*k1**2*sth1)/(2.*vev**2) + (cxi*complex(0,1)*gwR*k2**2*sth1)/(2.*vev**2)',
                  order = {'EWR':1})

GC_639 = Coupling(name = 'GC_639',
                  value = '(cphi*cw*cxi*complex(0,1)*gw*gwR*k1**2*sth1)/(2.*vev**2) - (cphi*cw*cxi*complex(0,1)*gw*gwR*k2**2*sth1)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_640 = Coupling(name = 'GC_640',
                  value = '(cxi**2*gw*gwR*k1**2*sth1)/(2.*vev**2) - (cxi**2*gw*gwR*k2**2*sth1)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_641 = Coupling(name = 'GC_641',
                  value = '(cxi**2*complex(0,1)*gw*gwR*k1**2*sth1)/(2.*vev**2) - (cxi**2*complex(0,1)*gw*gwR*k2**2*sth1)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_642 = Coupling(name = 'GC_642',
                  value = '-(cxi**2*gw*gwR*k1**2*sth1)/(2.*vev**2) + (cxi**2*gw*gwR*k2**2*sth1)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_643 = Coupling(name = 'GC_643',
                  value = '(cw*cxi*complex(0,1)*gw*gwR*k1**2*sphi*sth1)/(2.*vev**2) - (cw*cxi*complex(0,1)*gw*gwR*k2**2*sphi*sth1)/(2.*vev**2)',
                  order = {'EWR':2})

GC_644 = Coupling(name = 'GC_644',
                  value = '(cphi**2*cw**2*complex(0,1)*gw**2*k1**2*sth1**2)/(2.*vev**2) + (cphi**2*cw**2*complex(0,1)*gw**2*k2**2*sth1**2)/(2.*vev**2)',
                  order = {'EWL':2})

GC_645 = Coupling(name = 'GC_645',
                  value = '(cxi**2*complex(0,1)*gw**2*k1**2*sth1**2)/(2.*vev**2) + (cxi**2*complex(0,1)*gw**2*k2**2*sth1**2)/(2.*vev**2)',
                  order = {'EWL':2})

GC_646 = Coupling(name = 'GC_646',
                  value = '-(gw*k1**2*sw)/(2.*vev**2) - (gw*k2**2*sw)/(2.*vev**2)',
                  order = {'EWL':1})

GC_647 = Coupling(name = 'GC_647',
                  value = '-(complex(0,1)*gw*k1**2*sw)/(2.*vev**2) - (complex(0,1)*gw*k2**2*sw)/(2.*vev**2)',
                  order = {'EWL':1})

GC_648 = Coupling(name = 'GC_648',
                  value = '(cphi*cw*complex(0,1)*gw**2*k1**2*sw)/(2.*vev**2) + (cphi*cw*complex(0,1)*gw**2*k2**2*sw)/(2.*vev**2)',
                  order = {'EWL':2})

GC_649 = Coupling(name = 'GC_649',
                  value = '(cphi*cth1**2*cw*complex(0,1)*gw**2*k1**2*sw)/(2.*vev**2) + (cphi*cth1**2*cw*complex(0,1)*gw**2*k2**2*sw)/(2.*vev**2)',
                  order = {'EWL':2})

GC_650 = Coupling(name = 'GC_650',
                  value = '-(complex(0,1)*gwR*k1**2*sw)/(2.*vev**2) - (complex(0,1)*gwR*k2**2*sw)/(2.*vev**2)',
                  order = {'EWR':1})

GC_651 = Coupling(name = 'GC_651',
                  value = '(gwR*k1**2*sw)/(2.*vev**2) + (gwR*k2**2*sw)/(2.*vev**2)',
                  order = {'EWR':1})

GC_652 = Coupling(name = 'GC_652',
                  value = '-(cxi*gw*gwR*k1**2*sw)/(2.*vev**2) - (cxi*gw*gwR*k2**2*sw)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_653 = Coupling(name = 'GC_653',
                  value = '(cxi*complex(0,1)*gw*gwR*k1**2*sw)/(2.*vev**2) + (cxi*complex(0,1)*gw*gwR*k2**2*sw)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_654 = Coupling(name = 'GC_654',
                  value = '(cxi*gw*gwR*k1**2*sw)/(2.*vev**2) + (cxi*gw*gwR*k2**2*sw)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_655 = Coupling(name = 'GC_655',
                  value = '-(cth1*cxi*complex(0,1)*gw*gwR*k1**2*sw)/(2.*vev**2) + (cth1*cxi*complex(0,1)*gw*gwR*k2**2*sw)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_656 = Coupling(name = 'GC_656',
                  value = '-(cphi*cth1*cw*complex(0,1)*gw**2*k1**2*sth1*sw)/(2.*vev**2) - (cphi*cth1*cw*complex(0,1)*gw**2*k2**2*sth1*sw)/(2.*vev**2)',
                  order = {'EWL':2})

GC_657 = Coupling(name = 'GC_657',
                  value = '(cxi*complex(0,1)*gw*gwR*k1**2*sth1*sw)/(2.*vev**2) - (cxi*complex(0,1)*gw*gwR*k2**2*sth1*sw)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_658 = Coupling(name = 'GC_658',
                  value = '(cphi*cw*complex(0,1)*gw**2*k1**2*sth1**2*sw)/(2.*vev**2) + (cphi*cw*complex(0,1)*gw**2*k2**2*sth1**2*sw)/(2.*vev**2)',
                  order = {'EWL':2})

GC_659 = Coupling(name = 'GC_659',
                  value = '(complex(0,1)*gw**2*k1**2*sw**2)/(2.*vev**2) + (complex(0,1)*gw**2*k2**2*sw**2)/(2.*vev**2)',
                  order = {'EWL':2})

GC_660 = Coupling(name = 'GC_660',
                  value = '(cth1**2*complex(0,1)*gw**2*k1**2*sw**2)/(2.*vev**2) + (cth1**2*complex(0,1)*gw**2*k2**2*sw**2)/(2.*vev**2)',
                  order = {'EWL':2})

GC_661 = Coupling(name = 'GC_661',
                  value = '-(cphi*gwR*k1**2*sw**2)/(2.*cw*vev**2) - (cphi*gwR*k2**2*sw**2)/(2.*cw*vev**2)',
                  order = {'EWR':1})

GC_662 = Coupling(name = 'GC_662',
                  value = '(cphi*complex(0,1)*gwR*k1**2*sw**2)/(2.*cw*vev**2) + (cphi*complex(0,1)*gwR*k2**2*sw**2)/(2.*cw*vev**2)',
                  order = {'EWR':1})

GC_663 = Coupling(name = 'GC_663',
                  value = '-((complex(0,1)*gw*gwR*k1**2*sw**2)/vev**2) - (complex(0,1)*gw*gwR*k2**2*sw**2)/vev**2',
                  order = {'EWL':1,'EWR':1})

GC_664 = Coupling(name = 'GC_664',
                  value = '(complex(0,1)*gw*gwR*k1**2*sw**2)/vev**2 + (complex(0,1)*gw*gwR*k2**2*sw**2)/vev**2',
                  order = {'EWL':1,'EWR':1})

GC_665 = Coupling(name = 'GC_665',
                  value = '-((cphi**2*complex(0,1)*gw*gwR*k1**2*sw**2)/vev**2) - (cphi**2*complex(0,1)*gw*gwR*k2**2*sw**2)/vev**2',
                  order = {'EWL':1,'EWR':1})

GC_666 = Coupling(name = 'GC_666',
                  value = '(cphi**2*complex(0,1)*gw*gwR*k1**2*sw**2)/vev**2 + (cphi**2*complex(0,1)*gw*gwR*k2**2*sw**2)/vev**2',
                  order = {'EWL':1,'EWR':1})

GC_667 = Coupling(name = 'GC_667',
                  value = '-((cth1**2*complex(0,1)*gw*gwR*k1**2*sw**2)/vev**2) - (cth1**2*complex(0,1)*gw*gwR*k2**2*sw**2)/vev**2',
                  order = {'EWL':1,'EWR':1})

GC_668 = Coupling(name = 'GC_668',
                  value = '(cphi**2*cth1**2*complex(0,1)*gw*gwR*k1**2*sw**2)/vev**2 + (cphi**2*cth1**2*complex(0,1)*gw*gwR*k2**2*sw**2)/vev**2',
                  order = {'EWL':1,'EWR':1})

GC_669 = Coupling(name = 'GC_669',
                  value = '-(cphi*cxi*gw*gwR*k1**2*sw**2)/(2.*cw*vev**2) - (cphi*cxi*gw*gwR*k2**2*sw**2)/(2.*cw*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_670 = Coupling(name = 'GC_670',
                  value = '-(cphi*cxi*complex(0,1)*gw*gwR*k1**2*sw**2)/(2.*cw*vev**2) - (cphi*cxi*complex(0,1)*gw*gwR*k2**2*sw**2)/(2.*cw*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_671 = Coupling(name = 'GC_671',
                  value = '(cphi*cxi*gw*gwR*k1**2*sw**2)/(2.*cw*vev**2) + (cphi*cxi*gw*gwR*k2**2*sw**2)/(2.*cw*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_672 = Coupling(name = 'GC_672',
                  value = '(complex(0,1)*gwR**2*k1**2*sw**2)/(2.*vev**2) + (complex(0,1)*gwR**2*k2**2*sw**2)/(2.*vev**2)',
                  order = {'EWR':2})

GC_673 = Coupling(name = 'GC_673',
                  value = '2*complex(0,1)*gwR**2*sth1**2*sw**2 + (cth1**2*complex(0,1)*gwR**2*k1**2*sw**2)/(2.*vev**2) + (cth1**2*complex(0,1)*gwR**2*k2**2*sw**2)/(2.*vev**2)',
                  order = {'EWR':2})

GC_674 = Coupling(name = 'GC_674',
                  value = '-(gwR*k1**2*sphi*sw**2)/(2.*cw*vev**2) - (gwR*k2**2*sphi*sw**2)/(2.*cw*vev**2)',
                  order = {'EWL':-1,'EWR':2})

GC_675 = Coupling(name = 'GC_675',
                  value = '(complex(0,1)*gwR*k1**2*sphi*sw**2)/(2.*cw*vev**2) + (complex(0,1)*gwR*k2**2*sphi*sw**2)/(2.*cw*vev**2)',
                  order = {'EWL':-1,'EWR':2})

GC_676 = Coupling(name = 'GC_676',
                  value = '-(cxi*gw*gwR*k1**2*sphi*sw**2)/(2.*cw*vev**2) - (cxi*gw*gwR*k2**2*sphi*sw**2)/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_677 = Coupling(name = 'GC_677',
                  value = '-(cxi*complex(0,1)*gw*gwR*k1**2*sphi*sw**2)/(2.*cw*vev**2) - (cxi*complex(0,1)*gw*gwR*k2**2*sphi*sw**2)/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_678 = Coupling(name = 'GC_678',
                  value = '(cxi*gw*gwR*k1**2*sphi*sw**2)/(2.*cw*vev**2) + (cxi*gw*gwR*k2**2*sphi*sw**2)/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_679 = Coupling(name = 'GC_679',
                  value = '(complex(0,1)*gwR**2*k1**2*sphi**2)/(2.*cw**2*vev**2) + (complex(0,1)*gwR**2*k2**2*sphi**2)/(2.*cw**2*vev**2) - (complex(0,1)*gwR**2*k1**2*sphi**2*sw**2)/(cw**2*vev**2) - (complex(0,1)*gwR**2*k2**2*sphi**2*sw**2)/(cw**2*vev**2)',
                  order = {'EWL':-2,'EWR':4})

GC_680 = Coupling(name = 'GC_680',
                  value = '(2*complex(0,1)*gwR**2*sphi**2*sth1**2)/cw**2 - (4*complex(0,1)*gwR**2*sphi**2*sth1**2*sw**2)/cw**2 + (cth1**2*complex(0,1)*gwR**2*k1**2*sphi**2)/(2.*cw**2*vev**2) + (cth1**2*complex(0,1)*gwR**2*k2**2*sphi**2)/(2.*cw**2*vev**2) - (cth1**2*complex(0,1)*gwR**2*k1**2*sphi**2*sw**2)/(cw**2*vev**2) - (cth1**2*complex(0,1)*gwR**2*k2**2*sphi**2*sw**2)/(cw**2*vev**2)',
                  order = {'EWL':-2,'EWR':4})

GC_681 = Coupling(name = 'GC_681',
                  value = '-(cth1*complex(0,1)*gw**2*k1**2*sth1*sw**2)/(2.*vev**2) - (cth1*complex(0,1)*gw**2*k2**2*sth1*sw**2)/(2.*vev**2)',
                  order = {'EWL':2})

GC_682 = Coupling(name = 'GC_682',
                  value = '(cth1*complex(0,1)*gw*gwR*k1**2*sth1*sw**2)/vev**2 + (cth1*complex(0,1)*gw*gwR*k2**2*sth1*sw**2)/vev**2',
                  order = {'EWL':1,'EWR':1})

GC_683 = Coupling(name = 'GC_683',
                  value = '-((cphi**2*cth1*complex(0,1)*gw*gwR*k1**2*sth1*sw**2)/vev**2) - (cphi**2*cth1*complex(0,1)*gw*gwR*k2**2*sth1*sw**2)/vev**2',
                  order = {'EWL':1,'EWR':1})

GC_684 = Coupling(name = 'GC_684',
                  value = '2*cth1*complex(0,1)*gwR**2*sth1*sw**2 - (cth1*complex(0,1)*gwR**2*k1**2*sth1*sw**2)/(2.*vev**2) - (cth1*complex(0,1)*gwR**2*k2**2*sth1*sw**2)/(2.*vev**2)',
                  order = {'EWR':2})

GC_685 = Coupling(name = 'GC_685',
                  value = '(2*cth1*complex(0,1)*gwR**2*sphi**2*sth1)/cw**2 - (4*cth1*complex(0,1)*gwR**2*sphi**2*sth1*sw**2)/cw**2 - (cth1*complex(0,1)*gwR**2*k1**2*sphi**2*sth1)/(2.*cw**2*vev**2) - (cth1*complex(0,1)*gwR**2*k2**2*sphi**2*sth1)/(2.*cw**2*vev**2) + (cth1*complex(0,1)*gwR**2*k1**2*sphi**2*sth1*sw**2)/(cw**2*vev**2) + (cth1*complex(0,1)*gwR**2*k2**2*sphi**2*sth1*sw**2)/(cw**2*vev**2)',
                  order = {'EWL':-2,'EWR':4})

GC_686 = Coupling(name = 'GC_686',
                  value = '(complex(0,1)*gw**2*k1**2*sth1**2*sw**2)/(2.*vev**2) + (complex(0,1)*gw**2*k2**2*sth1**2*sw**2)/(2.*vev**2)',
                  order = {'EWL':2})

GC_687 = Coupling(name = 'GC_687',
                  value = '-((complex(0,1)*gw*gwR*k1**2*sth1**2*sw**2)/vev**2) - (complex(0,1)*gw*gwR*k2**2*sth1**2*sw**2)/vev**2',
                  order = {'EWL':1,'EWR':1})

GC_688 = Coupling(name = 'GC_688',
                  value = '(cphi**2*complex(0,1)*gw*gwR*k1**2*sth1**2*sw**2)/vev**2 + (cphi**2*complex(0,1)*gw*gwR*k2**2*sth1**2*sw**2)/vev**2',
                  order = {'EWL':1,'EWR':1})

GC_689 = Coupling(name = 'GC_689',
                  value = '2*cth1**2*complex(0,1)*gwR**2*sw**2 + (complex(0,1)*gwR**2*k1**2*sth1**2*sw**2)/(2.*vev**2) + (complex(0,1)*gwR**2*k2**2*sth1**2*sw**2)/(2.*vev**2)',
                  order = {'EWR':2})

GC_690 = Coupling(name = 'GC_690',
                  value = '(2*cth1**2*complex(0,1)*gwR**2*sphi**2)/cw**2 - (4*cth1**2*complex(0,1)*gwR**2*sphi**2*sw**2)/cw**2 + (complex(0,1)*gwR**2*k1**2*sphi**2*sth1**2)/(2.*cw**2*vev**2) + (complex(0,1)*gwR**2*k2**2*sphi**2*sth1**2)/(2.*cw**2*vev**2) - (complex(0,1)*gwR**2*k1**2*sphi**2*sth1**2*sw**2)/(cw**2*vev**2) - (complex(0,1)*gwR**2*k2**2*sphi**2*sth1**2*sw**2)/(cw**2*vev**2)',
                  order = {'EWL':-2,'EWR':4})

GC_691 = Coupling(name = 'GC_691',
                  value = '(cphi*cw*complex(0,1)*gw*gwR*k1**2*sw)/(2.*vev**2) + (cphi*cw*complex(0,1)*gw*gwR*k2**2*sw)/(2.*vev**2) - (cphi*complex(0,1)*gw*gwR*k1**2*sw**3)/(2.*cw*vev**2) - (cphi*complex(0,1)*gw*gwR*k2**2*sw**3)/(2.*cw*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_692 = Coupling(name = 'GC_692',
                  value = '-(cphi*cw*complex(0,1)*gw*gwR*k1**2*sw)/(2.*vev**2) - (cphi*cw*complex(0,1)*gw*gwR*k2**2*sw)/(2.*vev**2) + (cphi*complex(0,1)*gw*gwR*k1**2*sw**3)/(2.*cw*vev**2) + (cphi*complex(0,1)*gw*gwR*k2**2*sw**3)/(2.*cw*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_693 = Coupling(name = 'GC_693',
                  value = '-(cphi*cth1**2*cw*complex(0,1)*gw*gwR*k1**2*sw)/(2.*vev**2) - (cphi*cth1**2*cw*complex(0,1)*gw*gwR*k2**2*sw)/(2.*vev**2) + (cphi*cth1**2*complex(0,1)*gw*gwR*k1**2*sw**3)/(2.*cw*vev**2) + (cphi*cth1**2*complex(0,1)*gw*gwR*k2**2*sw**3)/(2.*cw*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_694 = Coupling(name = 'GC_694',
                  value = '-(complex(0,1)*gwR**2*k1**2*sphi*sw**3)/(2.*cw*vev**2) - (complex(0,1)*gwR**2*k2**2*sphi*sw**3)/(2.*cw*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_695 = Coupling(name = 'GC_695',
                  value = '(-2*complex(0,1)*gwR**2*sphi*sth1**2*sw**3)/cw - (cth1**2*complex(0,1)*gwR**2*k1**2*sphi*sw**3)/(2.*cw*vev**2) - (cth1**2*complex(0,1)*gwR**2*k2**2*sphi*sw**3)/(2.*cw*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_696 = Coupling(name = 'GC_696',
                  value = '(cphi*cth1*cw*complex(0,1)*gw*gwR*k1**2*sth1*sw)/(2.*vev**2) + (cphi*cth1*cw*complex(0,1)*gw*gwR*k2**2*sth1*sw)/(2.*vev**2) - (cphi*cth1*complex(0,1)*gw*gwR*k1**2*sth1*sw**3)/(2.*cw*vev**2) - (cphi*cth1*complex(0,1)*gw*gwR*k2**2*sth1*sw**3)/(2.*cw*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_697 = Coupling(name = 'GC_697',
                  value = '(-2*cth1*complex(0,1)*gwR**2*sphi*sth1*sw**3)/cw + (cth1*complex(0,1)*gwR**2*k1**2*sphi*sth1*sw**3)/(2.*cw*vev**2) + (cth1*complex(0,1)*gwR**2*k2**2*sphi*sth1*sw**3)/(2.*cw*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_698 = Coupling(name = 'GC_698',
                  value = '-(cphi*cw*complex(0,1)*gw*gwR*k1**2*sth1**2*sw)/(2.*vev**2) - (cphi*cw*complex(0,1)*gw*gwR*k2**2*sth1**2*sw)/(2.*vev**2) + (cphi*complex(0,1)*gw*gwR*k1**2*sth1**2*sw**3)/(2.*cw*vev**2) + (cphi*complex(0,1)*gw*gwR*k2**2*sth1**2*sw**3)/(2.*cw*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_699 = Coupling(name = 'GC_699',
                  value = '(-2*cth1**2*complex(0,1)*gwR**2*sphi*sw**3)/cw - (complex(0,1)*gwR**2*k1**2*sphi*sth1**2*sw**3)/(2.*cw*vev**2) - (complex(0,1)*gwR**2*k2**2*sphi*sth1**2*sw**3)/(2.*cw*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_700 = Coupling(name = 'GC_700',
                  value = '(complex(0,1)*gwR**2*k1**2*sphi**2*sw**4)/(2.*cw**2*vev**2) + (complex(0,1)*gwR**2*k2**2*sphi**2*sw**4)/(2.*cw**2*vev**2)',
                  order = {'EWL':-2,'EWR':4})

GC_701 = Coupling(name = 'GC_701',
                  value = '(2*complex(0,1)*gwR**2*sphi**2*sth1**2*sw**4)/cw**2 + (cth1**2*complex(0,1)*gwR**2*k1**2*sphi**2*sw**4)/(2.*cw**2*vev**2) + (cth1**2*complex(0,1)*gwR**2*k2**2*sphi**2*sw**4)/(2.*cw**2*vev**2)',
                  order = {'EWL':-2,'EWR':4})

GC_702 = Coupling(name = 'GC_702',
                  value = '(2*cth1*complex(0,1)*gwR**2*sphi**2*sth1*sw**4)/cw**2 - (cth1*complex(0,1)*gwR**2*k1**2*sphi**2*sth1*sw**4)/(2.*cw**2*vev**2) - (cth1*complex(0,1)*gwR**2*k2**2*sphi**2*sth1*sw**4)/(2.*cw**2*vev**2)',
                  order = {'EWL':-2,'EWR':4})

GC_703 = Coupling(name = 'GC_703',
                  value = '(2*cth1**2*complex(0,1)*gwR**2*sphi**2*sw**4)/cw**2 + (complex(0,1)*gwR**2*k1**2*sphi**2*sth1**2*sw**4)/(2.*cw**2*vev**2) + (complex(0,1)*gwR**2*k2**2*sphi**2*sth1**2*sw**4)/(2.*cw**2*vev**2)',
                  order = {'EWL':-2,'EWR':4})

GC_704 = Coupling(name = 'GC_704',
                  value = '-(cw*complex(0,1)*gw*k1**2*sphi)/(2.*vev**2) - (cw*complex(0,1)*gw*k2**2*sphi)/(2.*vev**2) - (cphi*complex(0,1)*gwR*k1**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) - (cphi*complex(0,1)*gwR*k2**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWR':1})

GC_705 = Coupling(name = 'GC_705',
                  value = '-(cw*gw*k1**2*sphi)/(2.*vev**2) - (cw*gw*k2**2*sphi)/(2.*vev**2) + (cphi*gwR*k1**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) + (cphi*gwR*k2**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWR':1})

GC_706 = Coupling(name = 'GC_706',
                  value = '(cphi*cw**2*complex(0,1)*gw**2*k1**2*sphi)/(2.*vev**2) + (cphi*cw**2*complex(0,1)*gw**2*k2**2*sphi)/(2.*vev**2) - (cphi**2*complex(0,1)*gw*gwR*k1**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2) - (cphi**2*complex(0,1)*gw*gwR*k2**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_707 = Coupling(name = 'GC_707',
                  value = '(cphi*cw**2*complex(0,1)*gw**2*k1**2*sphi)/(2.*vev**2) + (cphi*cw**2*complex(0,1)*gw**2*k2**2*sphi)/(2.*vev**2) + (cphi**2*complex(0,1)*gw*gwR*k1**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2) + (cphi**2*complex(0,1)*gw*gwR*k2**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_708 = Coupling(name = 'GC_708',
                  value = '(cphi*cth1**2*cw**2*complex(0,1)*gw**2*k1**2*sphi)/(2.*vev**2) + (cphi*cth1**2*cw**2*complex(0,1)*gw**2*k2**2*sphi)/(2.*vev**2) - (cphi**2*cth1**2*complex(0,1)*gw*gwR*k1**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2) - (cphi**2*cth1**2*complex(0,1)*gw*gwR*k2**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_709 = Coupling(name = 'GC_709',
                  value = '-(cphi*cxi*gw*gwR*k1**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) - (cphi*cxi*gw*gwR*k2**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_710 = Coupling(name = 'GC_710',
                  value = '(cphi*cxi*complex(0,1)*gw*gwR*k1**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) + (cphi*cxi*complex(0,1)*gw*gwR*k2**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_711 = Coupling(name = 'GC_711',
                  value = '(cphi*cxi*gw*gwR*k1**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) + (cphi*cxi*gw*gwR*k2**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_712 = Coupling(name = 'GC_712',
                  value = '-(gwR*k1**2*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) - (gwR*k2**2*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWL':-1,'EWR':2})

GC_713 = Coupling(name = 'GC_713',
                  value = '(complex(0,1)*gwR*k1**2*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) + (complex(0,1)*gwR*k2**2*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWL':-1,'EWR':2})

GC_714 = Coupling(name = 'GC_714',
                  value = '(cphi**2*complex(0,1)*gwR**2*k1**2)/(2.*cw**2*vev**2) + (cphi**2*complex(0,1)*gwR**2*k2**2)/(2.*cw**2*vev**2) + (cw**2*complex(0,1)*gw**2*k1**2*sphi**2)/(2.*vev**2) + (cw**2*complex(0,1)*gw**2*k2**2*sphi**2)/(2.*vev**2) - (cphi**2*complex(0,1)*gwR**2*k1**2*sw**2)/(cw**2*vev**2) - (cphi**2*complex(0,1)*gwR**2*k2**2*sw**2)/(cw**2*vev**2) - (cphi*complex(0,1)*gw*gwR*k1**2*sphi*cmath.sqrt(1 - 2*sw**2))/vev**2 - (cphi*complex(0,1)*gw*gwR*k2**2*sphi*cmath.sqrt(1 - 2*sw**2))/vev**2',
                  order = {'EWR':2})

GC_715 = Coupling(name = 'GC_715',
                  value = '(cphi**2*complex(0,1)*gwR**2*k1**2*sw**4)/(2.*cw**2*vev**2) + (cphi**2*complex(0,1)*gwR**2*k2**2*sw**4)/(2.*cw**2*vev**2) - (cphi*complex(0,1)*gw*gwR*k1**2*sphi*cmath.sqrt(1 - 2*sw**2))/vev**2 - (cphi*complex(0,1)*gw*gwR*k2**2*sphi*cmath.sqrt(1 - 2*sw**2))/vev**2',
                  order = {'EWR':2})

GC_716 = Coupling(name = 'GC_716',
                  value = '(cphi**2*complex(0,1)*gwR**2*k1**2)/(2.*cw**2*vev**2) + (cphi**2*complex(0,1)*gwR**2*k2**2)/(2.*cw**2*vev**2) + (cw**2*complex(0,1)*gw**2*k1**2*sphi**2)/(2.*vev**2) + (cw**2*complex(0,1)*gw**2*k2**2*sphi**2)/(2.*vev**2) - (cphi**2*complex(0,1)*gwR**2*k1**2*sw**2)/(cw**2*vev**2) - (cphi**2*complex(0,1)*gwR**2*k2**2*sw**2)/(cw**2*vev**2) + (cphi*complex(0,1)*gw*gwR*k1**2*sphi*cmath.sqrt(1 - 2*sw**2))/vev**2 + (cphi*complex(0,1)*gw*gwR*k2**2*sphi*cmath.sqrt(1 - 2*sw**2))/vev**2',
                  order = {'EWR':2})

GC_717 = Coupling(name = 'GC_717',
                  value = '(cphi**2*complex(0,1)*gwR**2*k1**2*sw**4)/(2.*cw**2*vev**2) + (cphi**2*complex(0,1)*gwR**2*k2**2*sw**4)/(2.*cw**2*vev**2) + (cphi*complex(0,1)*gw*gwR*k1**2*sphi*cmath.sqrt(1 - 2*sw**2))/vev**2 + (cphi*complex(0,1)*gw*gwR*k2**2*sphi*cmath.sqrt(1 - 2*sw**2))/vev**2',
                  order = {'EWR':2})

GC_718 = Coupling(name = 'GC_718',
                  value = '(2*cphi**2*complex(0,1)*gwR**2*sth1**2)/cw**2 - (4*cphi**2*complex(0,1)*gwR**2*sth1**2*sw**2)/cw**2 + (cphi**2*cth1**2*complex(0,1)*gwR**2*k1**2)/(2.*cw**2*vev**2) + (cphi**2*cth1**2*complex(0,1)*gwR**2*k2**2)/(2.*cw**2*vev**2) + (cth1**2*cw**2*complex(0,1)*gw**2*k1**2*sphi**2)/(2.*vev**2) + (cth1**2*cw**2*complex(0,1)*gw**2*k2**2*sphi**2)/(2.*vev**2) - (cphi**2*cth1**2*complex(0,1)*gwR**2*k1**2*sw**2)/(cw**2*vev**2) - (cphi**2*cth1**2*complex(0,1)*gwR**2*k2**2*sw**2)/(cw**2*vev**2) - (cphi*cth1**2*complex(0,1)*gw*gwR*k1**2*sphi*cmath.sqrt(1 - 2*sw**2))/vev**2 - (cphi*cth1**2*complex(0,1)*gw*gwR*k2**2*sphi*cmath.sqrt(1 - 2*sw**2))/vev**2',
                  order = {'EWR':2})

GC_719 = Coupling(name = 'GC_719',
                  value = '(2*cphi**2*complex(0,1)*gwR**2*sth1**2*sw**4)/cw**2 + (cphi**2*cth1**2*complex(0,1)*gwR**2*k1**2*sw**4)/(2.*cw**2*vev**2) + (cphi**2*cth1**2*complex(0,1)*gwR**2*k2**2*sw**4)/(2.*cw**2*vev**2) + (cphi*cth1**2*complex(0,1)*gw*gwR*k1**2*sphi*cmath.sqrt(1 - 2*sw**2))/vev**2 + (cphi*cth1**2*complex(0,1)*gw*gwR*k2**2*sphi*cmath.sqrt(1 - 2*sw**2))/vev**2',
                  order = {'EWR':2})

GC_720 = Coupling(name = 'GC_720',
                  value = '-(cphi*complex(0,1)*gwR**2*k1**2*sphi)/(2.*cw**2*vev**2) - (cphi*complex(0,1)*gwR**2*k2**2*sphi)/(2.*cw**2*vev**2) + (cphi*complex(0,1)*gwR**2*k1**2*sphi*sw**2)/(cw**2*vev**2) + (cphi*complex(0,1)*gwR**2*k2**2*sphi*sw**2)/(cw**2*vev**2) + (cphi*complex(0,1)*gwR**2*k1**2*sphi*sw**4)/(2.*cw**2*vev**2) + (cphi*complex(0,1)*gwR**2*k2**2*sphi*sw**4)/(2.*cw**2*vev**2) - (complex(0,1)*gw*gwR*k1**2*sphi**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2) - (complex(0,1)*gw*gwR*k2**2*sphi**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_721 = Coupling(name = 'GC_721',
                  value = '-(cphi*complex(0,1)*gwR**2*k1**2*sphi)/(2.*cw**2*vev**2) - (cphi*complex(0,1)*gwR**2*k2**2*sphi)/(2.*cw**2*vev**2) + (cphi*complex(0,1)*gwR**2*k1**2*sphi*sw**2)/(cw**2*vev**2) + (cphi*complex(0,1)*gwR**2*k2**2*sphi*sw**2)/(cw**2*vev**2) + (cphi*complex(0,1)*gwR**2*k1**2*sphi*sw**4)/(2.*cw**2*vev**2) + (cphi*complex(0,1)*gwR**2*k2**2*sphi*sw**4)/(2.*cw**2*vev**2) + (complex(0,1)*gw*gwR*k1**2*sphi**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2) + (complex(0,1)*gw*gwR*k2**2*sphi**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_722 = Coupling(name = 'GC_722',
                  value = '(-2*cphi*complex(0,1)*gwR**2*sphi*sth1**2)/cw**2 + (4*cphi*complex(0,1)*gwR**2*sphi*sth1**2*sw**2)/cw**2 + (2*cphi*complex(0,1)*gwR**2*sphi*sth1**2*sw**4)/cw**2 - (cphi*cth1**2*complex(0,1)*gwR**2*k1**2*sphi)/(2.*cw**2*vev**2) - (cphi*cth1**2*complex(0,1)*gwR**2*k2**2*sphi)/(2.*cw**2*vev**2) + (cphi*cth1**2*complex(0,1)*gwR**2*k1**2*sphi*sw**2)/(cw**2*vev**2) + (cphi*cth1**2*complex(0,1)*gwR**2*k2**2*sphi*sw**2)/(cw**2*vev**2) + (cphi*cth1**2*complex(0,1)*gwR**2*k1**2*sphi*sw**4)/(2.*cw**2*vev**2) + (cphi*cth1**2*complex(0,1)*gwR**2*k2**2*sphi*sw**4)/(2.*cw**2*vev**2) + (cth1**2*complex(0,1)*gw*gwR*k1**2*sphi**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2) + (cth1**2*complex(0,1)*gw*gwR*k2**2*sphi**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_723 = Coupling(name = 'GC_723',
                  value = '-(cphi*cth1*cw**2*complex(0,1)*gw**2*k1**2*sphi*sth1)/(2.*vev**2) - (cphi*cth1*cw**2*complex(0,1)*gw**2*k2**2*sphi*sth1)/(2.*vev**2) + (cphi**2*cth1*complex(0,1)*gw*gwR*k1**2*sth1*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2) + (cphi**2*cth1*complex(0,1)*gw*gwR*k2**2*sth1*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_724 = Coupling(name = 'GC_724',
                  value = '(2*cphi**2*cth1*complex(0,1)*gwR**2*sth1*sw**4)/cw**2 - (cphi**2*cth1*complex(0,1)*gwR**2*k1**2*sth1*sw**4)/(2.*cw**2*vev**2) - (cphi**2*cth1*complex(0,1)*gwR**2*k2**2*sth1*sw**4)/(2.*cw**2*vev**2) - (cphi*cth1*complex(0,1)*gw*gwR*k1**2*sphi*sth1*cmath.sqrt(1 - 2*sw**2))/vev**2 - (cphi*cth1*complex(0,1)*gw*gwR*k2**2*sphi*sth1*cmath.sqrt(1 - 2*sw**2))/vev**2',
                  order = {'EWR':2})

GC_725 = Coupling(name = 'GC_725',
                  value = '(2*cphi**2*cth1*complex(0,1)*gwR**2*sth1)/cw**2 - (4*cphi**2*cth1*complex(0,1)*gwR**2*sth1*sw**2)/cw**2 - (cphi**2*cth1*complex(0,1)*gwR**2*k1**2*sth1)/(2.*cw**2*vev**2) - (cphi**2*cth1*complex(0,1)*gwR**2*k2**2*sth1)/(2.*cw**2*vev**2) - (cth1*cw**2*complex(0,1)*gw**2*k1**2*sphi**2*sth1)/(2.*vev**2) - (cth1*cw**2*complex(0,1)*gw**2*k2**2*sphi**2*sth1)/(2.*vev**2) + (cphi**2*cth1*complex(0,1)*gwR**2*k1**2*sth1*sw**2)/(cw**2*vev**2) + (cphi**2*cth1*complex(0,1)*gwR**2*k2**2*sth1*sw**2)/(cw**2*vev**2) + (cphi*cth1*complex(0,1)*gw*gwR*k1**2*sphi*sth1*cmath.sqrt(1 - 2*sw**2))/vev**2 + (cphi*cth1*complex(0,1)*gw*gwR*k2**2*sphi*sth1*cmath.sqrt(1 - 2*sw**2))/vev**2',
                  order = {'EWR':2})

GC_726 = Coupling(name = 'GC_726',
                  value = '(-2*cphi*cth1*complex(0,1)*gwR**2*sphi*sth1)/cw**2 + (4*cphi*cth1*complex(0,1)*gwR**2*sphi*sth1*sw**2)/cw**2 + (2*cphi*cth1*complex(0,1)*gwR**2*sphi*sth1*sw**4)/cw**2 + (cphi*cth1*complex(0,1)*gwR**2*k1**2*sphi*sth1)/(2.*cw**2*vev**2) + (cphi*cth1*complex(0,1)*gwR**2*k2**2*sphi*sth1)/(2.*cw**2*vev**2) - (cphi*cth1*complex(0,1)*gwR**2*k1**2*sphi*sth1*sw**2)/(cw**2*vev**2) - (cphi*cth1*complex(0,1)*gwR**2*k2**2*sphi*sth1*sw**2)/(cw**2*vev**2) - (cphi*cth1*complex(0,1)*gwR**2*k1**2*sphi*sth1*sw**4)/(2.*cw**2*vev**2) - (cphi*cth1*complex(0,1)*gwR**2*k2**2*sphi*sth1*sw**4)/(2.*cw**2*vev**2) - (cth1*complex(0,1)*gw*gwR*k1**2*sphi**2*sth1*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2) - (cth1*complex(0,1)*gw*gwR*k2**2*sphi**2*sth1*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_727 = Coupling(name = 'GC_727',
                  value = '(cphi*cw**2*complex(0,1)*gw**2*k1**2*sphi*sth1**2)/(2.*vev**2) + (cphi*cw**2*complex(0,1)*gw**2*k2**2*sphi*sth1**2)/(2.*vev**2) - (cphi**2*complex(0,1)*gw*gwR*k1**2*sth1**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2) - (cphi**2*complex(0,1)*gw*gwR*k2**2*sth1**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_728 = Coupling(name = 'GC_728',
                  value = '(2*cphi**2*cth1**2*complex(0,1)*gwR**2)/cw**2 - (4*cphi**2*cth1**2*complex(0,1)*gwR**2*sw**2)/cw**2 + (cphi**2*complex(0,1)*gwR**2*k1**2*sth1**2)/(2.*cw**2*vev**2) + (cphi**2*complex(0,1)*gwR**2*k2**2*sth1**2)/(2.*cw**2*vev**2) + (cw**2*complex(0,1)*gw**2*k1**2*sphi**2*sth1**2)/(2.*vev**2) + (cw**2*complex(0,1)*gw**2*k2**2*sphi**2*sth1**2)/(2.*vev**2) - (cphi**2*complex(0,1)*gwR**2*k1**2*sth1**2*sw**2)/(cw**2*vev**2) - (cphi**2*complex(0,1)*gwR**2*k2**2*sth1**2*sw**2)/(cw**2*vev**2) - (cphi*complex(0,1)*gw*gwR*k1**2*sphi*sth1**2*cmath.sqrt(1 - 2*sw**2))/vev**2 - (cphi*complex(0,1)*gw*gwR*k2**2*sphi*sth1**2*cmath.sqrt(1 - 2*sw**2))/vev**2',
                  order = {'EWR':2})

GC_729 = Coupling(name = 'GC_729',
                  value = '(2*cphi**2*cth1**2*complex(0,1)*gwR**2*sw**4)/cw**2 + (cphi**2*complex(0,1)*gwR**2*k1**2*sth1**2*sw**4)/(2.*cw**2*vev**2) + (cphi**2*complex(0,1)*gwR**2*k2**2*sth1**2*sw**4)/(2.*cw**2*vev**2) + (cphi*complex(0,1)*gw*gwR*k1**2*sphi*sth1**2*cmath.sqrt(1 - 2*sw**2))/vev**2 + (cphi*complex(0,1)*gw*gwR*k2**2*sphi*sth1**2*cmath.sqrt(1 - 2*sw**2))/vev**2',
                  order = {'EWR':2})

GC_730 = Coupling(name = 'GC_730',
                  value = '(-2*cphi*cth1**2*complex(0,1)*gwR**2*sphi)/cw**2 + (4*cphi*cth1**2*complex(0,1)*gwR**2*sphi*sw**2)/cw**2 + (2*cphi*cth1**2*complex(0,1)*gwR**2*sphi*sw**4)/cw**2 - (cphi*complex(0,1)*gwR**2*k1**2*sphi*sth1**2)/(2.*cw**2*vev**2) - (cphi*complex(0,1)*gwR**2*k2**2*sphi*sth1**2)/(2.*cw**2*vev**2) + (cphi*complex(0,1)*gwR**2*k1**2*sphi*sth1**2*sw**2)/(cw**2*vev**2) + (cphi*complex(0,1)*gwR**2*k2**2*sphi*sth1**2*sw**2)/(cw**2*vev**2) + (cphi*complex(0,1)*gwR**2*k1**2*sphi*sth1**2*sw**4)/(2.*cw**2*vev**2) + (cphi*complex(0,1)*gwR**2*k2**2*sphi*sth1**2*sw**4)/(2.*cw**2*vev**2) + (complex(0,1)*gw*gwR*k1**2*sphi**2*sth1**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2) + (complex(0,1)*gw*gwR*k2**2*sphi**2*sth1**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_731 = Coupling(name = 'GC_731',
                  value = '(cw*complex(0,1)*gw**2*k1**2*sphi*sw)/(2.*vev**2) + (cw*complex(0,1)*gw**2*k2**2*sphi*sw)/(2.*vev**2) - (cphi*complex(0,1)*gw*gwR*k1**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) - (cphi*complex(0,1)*gw*gwR*k2**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_732 = Coupling(name = 'GC_732',
                  value = '(cw*complex(0,1)*gw**2*k1**2*sphi*sw)/(2.*vev**2) + (cw*complex(0,1)*gw**2*k2**2*sphi*sw)/(2.*vev**2) + (cphi*complex(0,1)*gw*gwR*k1**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) + (cphi*complex(0,1)*gw*gwR*k2**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_733 = Coupling(name = 'GC_733',
                  value = '(cth1**2*cw*complex(0,1)*gw**2*k1**2*sphi*sw)/(2.*vev**2) + (cth1**2*cw*complex(0,1)*gw**2*k2**2*sphi*sw)/(2.*vev**2) - (cphi*cth1**2*complex(0,1)*gw*gwR*k1**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) - (cphi*cth1**2*complex(0,1)*gw*gwR*k2**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_734 = Coupling(name = 'GC_734',
                  value = '(cw*complex(0,1)*gw*gwR*k1**2*sphi*sw)/(2.*vev**2) + (cw*complex(0,1)*gw*gwR*k2**2*sphi*sw)/(2.*vev**2) - (complex(0,1)*gw*gwR*k1**2*sphi*sw**3)/(2.*cw*vev**2) - (complex(0,1)*gw*gwR*k2**2*sphi*sw**3)/(2.*cw*vev**2) + (cphi*complex(0,1)*gwR**2*k1**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) + (cphi*complex(0,1)*gwR**2*k2**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_735 = Coupling(name = 'GC_735',
                  value = '-(cw*complex(0,1)*gw*gwR*k1**2*sphi*sw)/(2.*vev**2) - (cw*complex(0,1)*gw*gwR*k2**2*sphi*sw)/(2.*vev**2) + (complex(0,1)*gw*gwR*k1**2*sphi*sw**3)/(2.*cw*vev**2) + (complex(0,1)*gw*gwR*k2**2*sphi*sw**3)/(2.*cw*vev**2) + (cphi*complex(0,1)*gwR**2*k1**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) + (cphi*complex(0,1)*gwR**2*k2**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_736 = Coupling(name = 'GC_736',
                  value = '-(cth1**2*cw*complex(0,1)*gw*gwR*k1**2*sphi*sw)/(2.*vev**2) - (cth1**2*cw*complex(0,1)*gw*gwR*k2**2*sphi*sw)/(2.*vev**2) + (cth1**2*complex(0,1)*gw*gwR*k1**2*sphi*sw**3)/(2.*cw*vev**2) + (cth1**2*complex(0,1)*gw*gwR*k2**2*sphi*sw**3)/(2.*cw*vev**2) + (2*cphi*complex(0,1)*gwR**2*sth1**2*sw*cmath.sqrt(1 - 2*sw**2))/cw + (cphi*cth1**2*complex(0,1)*gwR**2*k1**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) + (cphi*cth1**2*complex(0,1)*gwR**2*k2**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_737 = Coupling(name = 'GC_737',
                  value = '-(cphi*complex(0,1)*gwR**2*k1**2*sw**3)/(2.*cw*vev**2) - (cphi*complex(0,1)*gwR**2*k2**2*sw**3)/(2.*cw*vev**2) - (complex(0,1)*gw*gwR*k1**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) - (complex(0,1)*gw*gwR*k2**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_738 = Coupling(name = 'GC_738',
                  value = '-(cphi*complex(0,1)*gwR**2*k1**2*sw**3)/(2.*cw*vev**2) - (cphi*complex(0,1)*gwR**2*k2**2*sw**3)/(2.*cw*vev**2) + (complex(0,1)*gw*gwR*k1**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) + (complex(0,1)*gw*gwR*k2**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_739 = Coupling(name = 'GC_739',
                  value = '(-2*cphi*complex(0,1)*gwR**2*sth1**2*sw**3)/cw - (cphi*cth1**2*complex(0,1)*gwR**2*k1**2*sw**3)/(2.*cw*vev**2) - (cphi*cth1**2*complex(0,1)*gwR**2*k2**2*sw**3)/(2.*cw*vev**2) + (cth1**2*complex(0,1)*gw*gwR*k1**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) + (cth1**2*complex(0,1)*gw*gwR*k2**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_740 = Coupling(name = 'GC_740',
                  value = '-(complex(0,1)*gwR**2*k1**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) - (complex(0,1)*gwR**2*k2**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_741 = Coupling(name = 'GC_741',
                  value = '(-2*complex(0,1)*gwR**2*sphi*sth1**2*sw*cmath.sqrt(1 - 2*sw**2))/cw - (cth1**2*complex(0,1)*gwR**2*k1**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) - (cth1**2*complex(0,1)*gwR**2*k2**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_742 = Coupling(name = 'GC_742',
                  value = '-(cth1*cw*complex(0,1)*gw**2*k1**2*sphi*sth1*sw)/(2.*vev**2) - (cth1*cw*complex(0,1)*gw**2*k2**2*sphi*sth1*sw)/(2.*vev**2) + (cphi*cth1*complex(0,1)*gw*gwR*k1**2*sth1*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) + (cphi*cth1*complex(0,1)*gw*gwR*k2**2*sth1*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_743 = Coupling(name = 'GC_743',
                  value = '(cth1*cw*complex(0,1)*gw*gwR*k1**2*sphi*sth1*sw)/(2.*vev**2) + (cth1*cw*complex(0,1)*gw*gwR*k2**2*sphi*sth1*sw)/(2.*vev**2) - (cth1*complex(0,1)*gw*gwR*k1**2*sphi*sth1*sw**3)/(2.*cw*vev**2) - (cth1*complex(0,1)*gw*gwR*k2**2*sphi*sth1*sw**3)/(2.*cw*vev**2) + (2*cphi*cth1*complex(0,1)*gwR**2*sth1*sw*cmath.sqrt(1 - 2*sw**2))/cw - (cphi*cth1*complex(0,1)*gwR**2*k1**2*sth1*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) - (cphi*cth1*complex(0,1)*gwR**2*k2**2*sth1*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_744 = Coupling(name = 'GC_744',
                  value = '(-2*cphi*cth1*complex(0,1)*gwR**2*sth1*sw**3)/cw + (cphi*cth1*complex(0,1)*gwR**2*k1**2*sth1*sw**3)/(2.*cw*vev**2) + (cphi*cth1*complex(0,1)*gwR**2*k2**2*sth1*sw**3)/(2.*cw*vev**2) - (cth1*complex(0,1)*gw*gwR*k1**2*sphi*sth1*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) - (cth1*complex(0,1)*gw*gwR*k2**2*sphi*sth1*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_745 = Coupling(name = 'GC_745',
                  value = '(-2*cth1*complex(0,1)*gwR**2*sphi*sth1*sw*cmath.sqrt(1 - 2*sw**2))/cw + (cth1*complex(0,1)*gwR**2*k1**2*sphi*sth1*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) + (cth1*complex(0,1)*gwR**2*k2**2*sphi*sth1*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_746 = Coupling(name = 'GC_746',
                  value = '(cw*complex(0,1)*gw**2*k1**2*sphi*sth1**2*sw)/(2.*vev**2) + (cw*complex(0,1)*gw**2*k2**2*sphi*sth1**2*sw)/(2.*vev**2) - (cphi*complex(0,1)*gw*gwR*k1**2*sth1**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) - (cphi*complex(0,1)*gw*gwR*k2**2*sth1**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_747 = Coupling(name = 'GC_747',
                  value = '-(cw*complex(0,1)*gw*gwR*k1**2*sphi*sth1**2*sw)/(2.*vev**2) - (cw*complex(0,1)*gw*gwR*k2**2*sphi*sth1**2*sw)/(2.*vev**2) + (complex(0,1)*gw*gwR*k1**2*sphi*sth1**2*sw**3)/(2.*cw*vev**2) + (complex(0,1)*gw*gwR*k2**2*sphi*sth1**2*sw**3)/(2.*cw*vev**2) + (2*cphi*cth1**2*complex(0,1)*gwR**2*sw*cmath.sqrt(1 - 2*sw**2))/cw + (cphi*complex(0,1)*gwR**2*k1**2*sth1**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) + (cphi*complex(0,1)*gwR**2*k2**2*sth1**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_748 = Coupling(name = 'GC_748',
                  value = '(-2*cphi*cth1**2*complex(0,1)*gwR**2*sw**3)/cw - (cphi*complex(0,1)*gwR**2*k1**2*sth1**2*sw**3)/(2.*cw*vev**2) - (cphi*complex(0,1)*gwR**2*k2**2*sth1**2*sw**3)/(2.*cw*vev**2) + (complex(0,1)*gw*gwR*k1**2*sphi*sth1**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) + (complex(0,1)*gw*gwR*k2**2*sphi*sth1**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_749 = Coupling(name = 'GC_749',
                  value = '(-2*cth1**2*complex(0,1)*gwR**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/cw - (complex(0,1)*gwR**2*k1**2*sphi*sth1**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) - (complex(0,1)*gwR**2*k2**2*sphi*sth1**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_750 = Coupling(name = 'GC_750',
                  value = '-((cphi*complex(0,1)*gw*gwR*k1**2*sphi*sw**2)/vev**2) - (cphi*complex(0,1)*gw*gwR*k2**2*sphi*sw**2)/vev**2 - (cphi**2*complex(0,1)*gwR**2*k1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev**2) - (cphi**2*complex(0,1)*gwR**2*k2**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev**2)',
                  order = {'EWR':2})

GC_751 = Coupling(name = 'GC_751',
                  value = '(cphi*complex(0,1)*gw*gwR*k1**2*sphi*sw**2)/vev**2 + (cphi*complex(0,1)*gw*gwR*k2**2*sphi*sw**2)/vev**2 - (cphi**2*complex(0,1)*gwR**2*k1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev**2) - (cphi**2*complex(0,1)*gwR**2*k2**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev**2)',
                  order = {'EWR':2})

GC_752 = Coupling(name = 'GC_752',
                  value = '(cphi*cth1**2*complex(0,1)*gw*gwR*k1**2*sphi*sw**2)/vev**2 + (cphi*cth1**2*complex(0,1)*gw*gwR*k2**2*sphi*sw**2)/vev**2 - (2*cphi**2*complex(0,1)*gwR**2*sth1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2 - (cphi**2*cth1**2*complex(0,1)*gwR**2*k1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev**2) - (cphi**2*cth1**2*complex(0,1)*gwR**2*k2**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev**2)',
                  order = {'EWR':2})

GC_753 = Coupling(name = 'GC_753',
                  value = '-((complex(0,1)*gw*gwR*k1**2*sphi**2*sw**2)/vev**2) - (complex(0,1)*gw*gwR*k2**2*sphi**2*sw**2)/vev**2 - (cphi*complex(0,1)*gwR**2*k1**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev**2) - (cphi*complex(0,1)*gwR**2*k2**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_754 = Coupling(name = 'GC_754',
                  value = '(complex(0,1)*gw*gwR*k1**2*sphi**2*sw**2)/vev**2 + (complex(0,1)*gw*gwR*k2**2*sphi**2*sw**2)/vev**2 - (cphi*complex(0,1)*gwR**2*k1**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev**2) - (cphi*complex(0,1)*gwR**2*k2**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_755 = Coupling(name = 'GC_755',
                  value = '(cphi*complex(0,1)*gwR**2*k1**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev**2) + (cphi*complex(0,1)*gwR**2*k2**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_756 = Coupling(name = 'GC_756',
                  value = '(cth1**2*complex(0,1)*gw*gwR*k1**2*sphi**2*sw**2)/vev**2 + (cth1**2*complex(0,1)*gw*gwR*k2**2*sphi**2*sw**2)/vev**2 - (4*cphi*complex(0,1)*gwR**2*sphi*sth1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2 - (cphi*cth1**2*complex(0,1)*gwR**2*k1**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev**2) - (cphi*cth1**2*complex(0,1)*gwR**2*k2**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_757 = Coupling(name = 'GC_757',
                  value = '(4*cphi*complex(0,1)*gwR**2*sphi*sth1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2 + (cphi*cth1**2*complex(0,1)*gwR**2*k1**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev**2) + (cphi*cth1**2*complex(0,1)*gwR**2*k2**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_758 = Coupling(name = 'GC_758',
                  value = '(complex(0,1)*gwR**2*k1**2*sphi**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev**2) + (complex(0,1)*gwR**2*k2**2*sphi**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev**2)',
                  order = {'EWL':-2,'EWR':4})

GC_759 = Coupling(name = 'GC_759',
                  value = '(2*complex(0,1)*gwR**2*sphi**2*sth1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2 + (cth1**2*complex(0,1)*gwR**2*k1**2*sphi**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev**2) + (cth1**2*complex(0,1)*gwR**2*k2**2*sphi**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev**2)',
                  order = {'EWL':-2,'EWR':4})

GC_760 = Coupling(name = 'GC_760',
                  value = '-((cphi*cth1*complex(0,1)*gw*gwR*k1**2*sphi*sth1*sw**2)/vev**2) - (cphi*cth1*complex(0,1)*gw*gwR*k2**2*sphi*sth1*sw**2)/vev**2 - (2*cphi**2*cth1*complex(0,1)*gwR**2*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2 + (cphi**2*cth1*complex(0,1)*gwR**2*k1**2*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev**2) + (cphi**2*cth1*complex(0,1)*gwR**2*k2**2*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev**2)',
                  order = {'EWR':2})

GC_761 = Coupling(name = 'GC_761',
                  value = '(4*cphi*cth1*complex(0,1)*gwR**2*sphi*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2 - (cphi*cth1*complex(0,1)*gwR**2*k1**2*sphi*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev**2) - (cphi*cth1*complex(0,1)*gwR**2*k2**2*sphi*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_762 = Coupling(name = 'GC_762',
                  value = '-((cth1*complex(0,1)*gw*gwR*k1**2*sphi**2*sth1*sw**2)/vev**2) - (cth1*complex(0,1)*gw*gwR*k2**2*sphi**2*sth1*sw**2)/vev**2 - (4*cphi*cth1*complex(0,1)*gwR**2*sphi*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2 + (cphi*cth1*complex(0,1)*gwR**2*k1**2*sphi*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev**2) + (cphi*cth1*complex(0,1)*gwR**2*k2**2*sphi*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_763 = Coupling(name = 'GC_763',
                  value = '(2*cth1*complex(0,1)*gwR**2*sphi**2*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2 - (cth1*complex(0,1)*gwR**2*k1**2*sphi**2*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev**2) - (cth1*complex(0,1)*gwR**2*k2**2*sphi**2*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev**2)',
                  order = {'EWL':-2,'EWR':4})

GC_764 = Coupling(name = 'GC_764',
                  value = '(cphi*complex(0,1)*gw*gwR*k1**2*sphi*sth1**2*sw**2)/vev**2 + (cphi*complex(0,1)*gw*gwR*k2**2*sphi*sth1**2*sw**2)/vev**2 - (2*cphi**2*cth1**2*complex(0,1)*gwR**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2 - (cphi**2*complex(0,1)*gwR**2*k1**2*sth1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev**2) - (cphi**2*complex(0,1)*gwR**2*k2**2*sth1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev**2)',
                  order = {'EWR':2})

GC_765 = Coupling(name = 'GC_765',
                  value = '(complex(0,1)*gw*gwR*k1**2*sphi**2*sth1**2*sw**2)/vev**2 + (complex(0,1)*gw*gwR*k2**2*sphi**2*sth1**2*sw**2)/vev**2 - (4*cphi*cth1**2*complex(0,1)*gwR**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2 - (cphi*complex(0,1)*gwR**2*k1**2*sphi*sth1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev**2) - (cphi*complex(0,1)*gwR**2*k2**2*sphi*sth1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_766 = Coupling(name = 'GC_766',
                  value = '(4*cphi*cth1**2*complex(0,1)*gwR**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2 + (cphi*complex(0,1)*gwR**2*k1**2*sphi*sth1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev**2) + (cphi*complex(0,1)*gwR**2*k2**2*sphi*sth1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_767 = Coupling(name = 'GC_767',
                  value = '(2*cth1**2*complex(0,1)*gwR**2*sphi**2*sw**2*cmath.sqrt(1 - 2*sw**2))/cw**2 + (complex(0,1)*gwR**2*k1**2*sphi**2*sth1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev**2) + (complex(0,1)*gwR**2*k2**2*sphi**2*sth1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev**2)',
                  order = {'EWL':-2,'EWR':4})

GC_768 = Coupling(name = 'GC_768',
                  value = '-((cphi*cw*gw*gwR*k1*k2*sxi)/vev**2) + (cxi*gw*gwR*k1**2*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) + (cxi*gw*gwR*k2**2*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_769 = Coupling(name = 'GC_769',
                  value = '-((cphi*cw*complex(0,1)*gw*gwR*k1*k2*sxi)/vev**2) - (cxi*complex(0,1)*gw*gwR*k1**2*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) - (cxi*complex(0,1)*gw*gwR*k2**2*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_770 = Coupling(name = 'GC_770',
                  value = '(cphi*cw*gw*gwR*k1*k2*sxi)/vev**2 - (cxi*gw*gwR*k1**2*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) - (cxi*gw*gwR*k2**2*sphi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_771 = Coupling(name = 'GC_771',
                  value = '-((cxi*complex(0,1)*gwR*k1*k2)/vev**2) - (complex(0,1)*gw*k1**2*sxi)/(2.*vev**2) - (complex(0,1)*gw*k2**2*sxi)/(2.*vev**2)',
                  order = {'EWR':1})

GC_772 = Coupling(name = 'GC_772',
                  value = '(cxi*complex(0,1)*gwR*k1*k2)/vev**2 + (complex(0,1)*gw*k1**2*sxi)/(2.*vev**2) + (complex(0,1)*gw*k2**2*sxi)/(2.*vev**2)',
                  order = {'EWR':1})

GC_773 = Coupling(name = 'GC_773',
                  value = '-((cxi*gwR*k1*k2)/vev**2) + (gw*k1**2*sxi)/(2.*vev**2) + (gw*k2**2*sxi)/(2.*vev**2)',
                  order = {'EWR':1})

GC_774 = Coupling(name = 'GC_774',
                  value = '(cxi*complex(0,1)*gw**2*k1**2*sxi)/(2.*vev**2) + (cxi*complex(0,1)*gw**2*k2**2*sxi)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_775 = Coupling(name = 'GC_775',
                  value = '-((cxi**2*complex(0,1)*gw*gwR*k1*k2)/vev**2) + (cxi*complex(0,1)*gw**2*k1**2*sxi)/(2.*vev**2) + (cxi*complex(0,1)*gw**2*k2**2*sxi)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_776 = Coupling(name = 'GC_776',
                  value = '(cxi**2*complex(0,1)*gw*gwR*k1*k2)/vev**2 + (cxi*complex(0,1)*gw**2*k1**2*sxi)/(2.*vev**2) + (cxi*complex(0,1)*gw**2*k2**2*sxi)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_777 = Coupling(name = 'GC_777',
                  value = '-((cth1**2*cxi**2*complex(0,1)*gw*gwR*k1*k2)/vev**2) + (cth1**2*cxi*complex(0,1)*gw**2*k1**2*sxi)/(2.*vev**2) + (cth1**2*cxi*complex(0,1)*gw**2*k2**2*sxi)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_778 = Coupling(name = 'GC_778',
                  value = '(cth1*complex(0,1)*gwR*k1**2*sxi)/(2.*vev**2) - (cth1*complex(0,1)*gwR*k2**2*sxi)/(2.*vev**2)',
                  order = {'EWL':-1,'EWR':2})

GC_779 = Coupling(name = 'GC_779',
                  value = '-(cth1*complex(0,1)*gwR*k1**2*sxi)/(2.*vev**2) + (cth1*complex(0,1)*gwR*k2**2*sxi)/(2.*vev**2)',
                  order = {'EWL':-1,'EWR':2})

GC_780 = Coupling(name = 'GC_780',
                  value = '(cphi*cth1*cw*complex(0,1)*gw*gwR*k1**2*sxi)/(2.*vev**2) - (cphi*cth1*cw*complex(0,1)*gw*gwR*k2**2*sxi)/(2.*vev**2)',
                  order = {'EWR':2})

GC_781 = Coupling(name = 'GC_781',
                  value = '(cth1*cxi*complex(0,1)*gw*gwR*k1**2*sxi)/vev**2 - (cth1*cxi*complex(0,1)*gw*gwR*k2**2*sxi)/vev**2',
                  order = {'EWR':2})

GC_782 = Coupling(name = 'GC_782',
                  value = '-((cth1*cxi*complex(0,1)*gw*gwR*k1**2*sxi)/vev**2) + (cth1*cxi*complex(0,1)*gw*gwR*k2**2*sxi)/vev**2',
                  order = {'EWR':2})

GC_783 = Coupling(name = 'GC_783',
                  value = '-(cxi*complex(0,1)*gwR**2*k1**2*sxi)/(2.*vev**2) - (cxi*complex(0,1)*gwR**2*k2**2*sxi)/(2.*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_784 = Coupling(name = 'GC_784',
                  value = '(cth1*cw*complex(0,1)*gw*gwR*k1**2*sphi*sxi)/(2.*vev**2) - (cth1*cw*complex(0,1)*gw*gwR*k2**2*sphi*sxi)/(2.*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_785 = Coupling(name = 'GC_785',
                  value = '(cth1*cxi**2*complex(0,1)*gw*gwR*k1*k2*sth1)/vev**2 - (cth1*cxi*complex(0,1)*gw**2*k1**2*sth1*sxi)/(2.*vev**2) - (cth1*cxi*complex(0,1)*gw**2*k2**2*sth1*sxi)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_786 = Coupling(name = 'GC_786',
                  value = '(complex(0,1)*gwR*k1**2*sth1*sxi)/(2.*vev**2) - (complex(0,1)*gwR*k2**2*sth1*sxi)/(2.*vev**2)',
                  order = {'EWL':-1,'EWR':2})

GC_787 = Coupling(name = 'GC_787',
                  value = '-(complex(0,1)*gwR*k1**2*sth1*sxi)/(2.*vev**2) + (complex(0,1)*gwR*k2**2*sth1*sxi)/(2.*vev**2)',
                  order = {'EWL':-1,'EWR':2})

GC_788 = Coupling(name = 'GC_788',
                  value = '-(cphi*cw*complex(0,1)*gw*gwR*k1**2*sth1*sxi)/(2.*vev**2) + (cphi*cw*complex(0,1)*gw*gwR*k2**2*sth1*sxi)/(2.*vev**2)',
                  order = {'EWR':2})

GC_789 = Coupling(name = 'GC_789',
                  value = '(cxi*complex(0,1)*gw*gwR*k1**2*sth1*sxi)/vev**2 - (cxi*complex(0,1)*gw*gwR*k2**2*sth1*sxi)/vev**2',
                  order = {'EWR':2})

GC_790 = Coupling(name = 'GC_790',
                  value = '-((cxi*complex(0,1)*gw*gwR*k1**2*sth1*sxi)/vev**2) + (cxi*complex(0,1)*gw*gwR*k2**2*sth1*sxi)/vev**2',
                  order = {'EWR':2})

GC_791 = Coupling(name = 'GC_791',
                  value = '-(cw*complex(0,1)*gw*gwR*k1**2*sphi*sth1*sxi)/(2.*vev**2) + (cw*complex(0,1)*gw*gwR*k2**2*sphi*sth1*sxi)/(2.*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_792 = Coupling(name = 'GC_792',
                  value = '-((cxi**2*complex(0,1)*gw*gwR*k1*k2*sth1**2)/vev**2) + (cxi*complex(0,1)*gw**2*k1**2*sth1**2*sxi)/(2.*vev**2) + (cxi*complex(0,1)*gw**2*k2**2*sth1**2*sxi)/(2.*vev**2)',
                  order = {'EWL':1,'EWR':1})

GC_793 = Coupling(name = 'GC_793',
                  value = '-(gw*gwR*k1**2*sw*sxi)/(2.*vev**2) - (gw*gwR*k2**2*sw*sxi)/(2.*vev**2)',
                  order = {'EWR':2})

GC_794 = Coupling(name = 'GC_794',
                  value = '(complex(0,1)*gw*gwR*k1**2*sw*sxi)/(2.*vev**2) + (complex(0,1)*gw*gwR*k2**2*sw*sxi)/(2.*vev**2)',
                  order = {'EWR':2})

GC_795 = Coupling(name = 'GC_795',
                  value = '(gw*gwR*k1**2*sw*sxi)/(2.*vev**2) + (gw*gwR*k2**2*sw*sxi)/(2.*vev**2)',
                  order = {'EWR':2})

GC_796 = Coupling(name = 'GC_796',
                  value = '(cth1*complex(0,1)*gw*gwR*k1**2*sw*sxi)/(2.*vev**2) - (cth1*complex(0,1)*gw*gwR*k2**2*sw*sxi)/(2.*vev**2)',
                  order = {'EWR':2})

GC_797 = Coupling(name = 'GC_797',
                  value = '-(complex(0,1)*gw*gwR*k1**2*sth1*sw*sxi)/(2.*vev**2) + (complex(0,1)*gw*gwR*k2**2*sth1*sw*sxi)/(2.*vev**2)',
                  order = {'EWR':2})

GC_798 = Coupling(name = 'GC_798',
                  value = '-(cphi*gw*gwR*k1**2*sw**2*sxi)/(2.*cw*vev**2) - (cphi*gw*gwR*k2**2*sw**2*sxi)/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_799 = Coupling(name = 'GC_799',
                  value = '-(cphi*complex(0,1)*gw*gwR*k1**2*sw**2*sxi)/(2.*cw*vev**2) - (cphi*complex(0,1)*gw*gwR*k2**2*sw**2*sxi)/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_800 = Coupling(name = 'GC_800',
                  value = '(cphi*gw*gwR*k1**2*sw**2*sxi)/(2.*cw*vev**2) + (cphi*gw*gwR*k2**2*sw**2*sxi)/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_801 = Coupling(name = 'GC_801',
                  value = '-(gw*gwR*k1**2*sphi*sw**2*sxi)/(2.*cw*vev**2) - (gw*gwR*k2**2*sphi*sw**2*sxi)/(2.*cw*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_802 = Coupling(name = 'GC_802',
                  value = '-(complex(0,1)*gw*gwR*k1**2*sphi*sw**2*sxi)/(2.*cw*vev**2) - (complex(0,1)*gw*gwR*k2**2*sphi*sw**2*sxi)/(2.*cw*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_803 = Coupling(name = 'GC_803',
                  value = '(gw*gwR*k1**2*sphi*sw**2*sxi)/(2.*cw*vev**2) + (gw*gwR*k2**2*sphi*sw**2*sxi)/(2.*cw*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_804 = Coupling(name = 'GC_804',
                  value = '(cw*cxi*gw*gwR*k1*k2*sphi)/vev**2 - (cphi*gw*gwR*k1**2*sxi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) - (cphi*gw*gwR*k2**2*sxi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_805 = Coupling(name = 'GC_805',
                  value = '(cw*cxi*complex(0,1)*gw*gwR*k1*k2*sphi)/vev**2 + (cphi*complex(0,1)*gw*gwR*k1**2*sxi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) + (cphi*complex(0,1)*gw*gwR*k2**2*sxi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_806 = Coupling(name = 'GC_806',
                  value = '-((cw*cxi*gw*gwR*k1*k2*sphi)/vev**2) + (cphi*gw*gwR*k1**2*sxi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) + (cphi*gw*gwR*k2**2*sxi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWR':2})

GC_807 = Coupling(name = 'GC_807',
                  value = '-(gw*gwR*k1**2*sphi*sxi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) - (gw*gwR*k2**2*sphi*sxi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_808 = Coupling(name = 'GC_808',
                  value = '-(complex(0,1)*gw*gwR*k1**2*sphi*sxi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) - (complex(0,1)*gw*gwR*k2**2*sphi*sxi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_809 = Coupling(name = 'GC_809',
                  value = '(gw*gwR*k1**2*sphi*sxi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2) + (gw*gwR*k2**2*sphi*sxi*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_810 = Coupling(name = 'GC_810',
                  value = '-(cxi*complex(0,1)*gwR**2*k1**2*sxi)/(2.*vev**2) - (cxi*complex(0,1)*gwR**2*k2**2*sxi)/(2.*vev**2) - (complex(0,1)*gw*gwR*k1*k2*sxi**2)/vev**2',
                  order = {'EWL':-1,'EWR':3})

GC_811 = Coupling(name = 'GC_811',
                  value = '-(cxi*complex(0,1)*gwR**2*k1**2*sxi)/(2.*vev**2) - (cxi*complex(0,1)*gwR**2*k2**2*sxi)/(2.*vev**2) + (complex(0,1)*gw*gwR*k1*k2*sxi**2)/vev**2',
                  order = {'EWL':-1,'EWR':3})

GC_812 = Coupling(name = 'GC_812',
                  value = '-(cxi*complex(0,1)*gwR**2*sth1**2*sxi) - (cth1**2*cxi*complex(0,1)*gwR**2*k1**2*sxi)/(2.*vev**2) - (cth1**2*cxi*complex(0,1)*gwR**2*k2**2*sxi)/(2.*vev**2) + (cth1**2*complex(0,1)*gw*gwR*k1*k2*sxi**2)/vev**2',
                  order = {'EWL':-1,'EWR':3})

GC_813 = Coupling(name = 'GC_813',
                  value = '(cxi**2*complex(0,1)*gwR**2*k1**2)/(2.*vev**2) + (cxi**2*complex(0,1)*gwR**2*k2**2)/(2.*vev**2) + (complex(0,1)*gw**2*k1**2*sxi**2)/(2.*vev**2) + (complex(0,1)*gw**2*k2**2*sxi**2)/(2.*vev**2)',
                  order = {'EWR':2})

GC_814 = Coupling(name = 'GC_814',
                  value = '(cxi**2*complex(0,1)*gwR**2*k1**2)/(2.*vev**2) + (cxi**2*complex(0,1)*gwR**2*k2**2)/(2.*vev**2) - (2*cxi*complex(0,1)*gw*gwR*k1*k2*sxi)/vev**2 + (complex(0,1)*gw**2*k1**2*sxi**2)/(2.*vev**2) + (complex(0,1)*gw**2*k2**2*sxi**2)/(2.*vev**2)',
                  order = {'EWR':2})

GC_815 = Coupling(name = 'GC_815',
                  value = '(cxi**2*complex(0,1)*gwR**2*k1**2)/(2.*vev**2) + (cxi**2*complex(0,1)*gwR**2*k2**2)/(2.*vev**2) + (2*cxi*complex(0,1)*gw*gwR*k1*k2*sxi)/vev**2 + (complex(0,1)*gw**2*k1**2*sxi**2)/(2.*vev**2) + (complex(0,1)*gw**2*k2**2*sxi**2)/(2.*vev**2)',
                  order = {'EWR':2})

GC_816 = Coupling(name = 'GC_816',
                  value = 'cxi**2*complex(0,1)*gwR**2*sth1**2 + (cth1**2*cxi**2*complex(0,1)*gwR**2*k1**2)/(2.*vev**2) + (cth1**2*cxi**2*complex(0,1)*gwR**2*k2**2)/(2.*vev**2) - (2*cth1**2*cxi*complex(0,1)*gw*gwR*k1*k2*sxi)/vev**2 + (cth1**2*complex(0,1)*gw**2*k1**2*sxi**2)/(2.*vev**2) + (cth1**2*complex(0,1)*gw**2*k2**2*sxi**2)/(2.*vev**2)',
                  order = {'EWR':2})

GC_817 = Coupling(name = 'GC_817',
                  value = '(cth1*gw*gwR*k1**2*sxi**2)/(2.*vev**2) - (cth1*gw*gwR*k2**2*sxi**2)/(2.*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_818 = Coupling(name = 'GC_818',
                  value = '(cth1*complex(0,1)*gw*gwR*k1**2*sxi**2)/(2.*vev**2) - (cth1*complex(0,1)*gw*gwR*k2**2*sxi**2)/(2.*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_819 = Coupling(name = 'GC_819',
                  value = '-(cth1*gw*gwR*k1**2*sxi**2)/(2.*vev**2) + (cth1*gw*gwR*k2**2*sxi**2)/(2.*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_820 = Coupling(name = 'GC_820',
                  value = '(complex(0,1)*gwR**2*k1**2*sxi**2)/(2.*vev**2) + (complex(0,1)*gwR**2*k2**2*sxi**2)/(2.*vev**2)',
                  order = {'EWL':-2,'EWR':4})

GC_821 = Coupling(name = 'GC_821',
                  value = 'complex(0,1)*gwR**2*sth1**2*sxi**2 + (cth1**2*complex(0,1)*gwR**2*k1**2*sxi**2)/(2.*vev**2) + (cth1**2*complex(0,1)*gwR**2*k2**2*sxi**2)/(2.*vev**2)',
                  order = {'EWL':-2,'EWR':4})

GC_822 = Coupling(name = 'GC_822',
                  value = '-(cth1*cxi*complex(0,1)*gwR**2*sth1*sxi) + (cth1*cxi*complex(0,1)*gwR**2*k1**2*sth1*sxi)/(2.*vev**2) + (cth1*cxi*complex(0,1)*gwR**2*k2**2*sth1*sxi)/(2.*vev**2) - (cth1*complex(0,1)*gw*gwR*k1*k2*sth1*sxi**2)/vev**2',
                  order = {'EWL':-1,'EWR':3})

GC_823 = Coupling(name = 'GC_823',
                  value = 'cth1*cxi**2*complex(0,1)*gwR**2*sth1 - (cth1*cxi**2*complex(0,1)*gwR**2*k1**2*sth1)/(2.*vev**2) - (cth1*cxi**2*complex(0,1)*gwR**2*k2**2*sth1)/(2.*vev**2) + (2*cth1*cxi*complex(0,1)*gw*gwR*k1*k2*sth1*sxi)/vev**2 - (cth1*complex(0,1)*gw**2*k1**2*sth1*sxi**2)/(2.*vev**2) - (cth1*complex(0,1)*gw**2*k2**2*sth1*sxi**2)/(2.*vev**2)',
                  order = {'EWR':2})

GC_824 = Coupling(name = 'GC_824',
                  value = '(gw*gwR*k1**2*sth1*sxi**2)/(2.*vev**2) - (gw*gwR*k2**2*sth1*sxi**2)/(2.*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_825 = Coupling(name = 'GC_825',
                  value = '-(complex(0,1)*gw*gwR*k1**2*sth1*sxi**2)/(2.*vev**2) + (complex(0,1)*gw*gwR*k2**2*sth1*sxi**2)/(2.*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_826 = Coupling(name = 'GC_826',
                  value = '-(gw*gwR*k1**2*sth1*sxi**2)/(2.*vev**2) + (gw*gwR*k2**2*sth1*sxi**2)/(2.*vev**2)',
                  order = {'EWL':-1,'EWR':3})

GC_827 = Coupling(name = 'GC_827',
                  value = 'cth1*complex(0,1)*gwR**2*sth1*sxi**2 - (cth1*complex(0,1)*gwR**2*k1**2*sth1*sxi**2)/(2.*vev**2) - (cth1*complex(0,1)*gwR**2*k2**2*sth1*sxi**2)/(2.*vev**2)',
                  order = {'EWL':-2,'EWR':4})

GC_828 = Coupling(name = 'GC_828',
                  value = '-(cth1**2*cxi*complex(0,1)*gwR**2*sxi) - (cxi*complex(0,1)*gwR**2*k1**2*sth1**2*sxi)/(2.*vev**2) - (cxi*complex(0,1)*gwR**2*k2**2*sth1**2*sxi)/(2.*vev**2) + (complex(0,1)*gw*gwR*k1*k2*sth1**2*sxi**2)/vev**2',
                  order = {'EWL':-1,'EWR':3})

GC_829 = Coupling(name = 'GC_829',
                  value = 'cth1**2*cxi**2*complex(0,1)*gwR**2 + (cxi**2*complex(0,1)*gwR**2*k1**2*sth1**2)/(2.*vev**2) + (cxi**2*complex(0,1)*gwR**2*k2**2*sth1**2)/(2.*vev**2) - (2*cxi*complex(0,1)*gw*gwR*k1*k2*sth1**2*sxi)/vev**2 + (complex(0,1)*gw**2*k1**2*sth1**2*sxi**2)/(2.*vev**2) + (complex(0,1)*gw**2*k2**2*sth1**2*sxi**2)/(2.*vev**2)',
                  order = {'EWR':2})

GC_830 = Coupling(name = 'GC_830',
                  value = 'cth1**2*complex(0,1)*gwR**2*sxi**2 + (complex(0,1)*gwR**2*k1**2*sth1**2*sxi**2)/(2.*vev**2) + (complex(0,1)*gwR**2*k2**2*sth1**2*sxi**2)/(2.*vev**2)',
                  order = {'EWL':-2,'EWR':4})

GC_831 = Coupling(name = 'GC_831',
                  value = '(cphi**2*cth1*cw**2*complex(0,1)*gw**2*k1**2)/(2.*vev) + (cphi**2*cth1*cw**2*complex(0,1)*gw**2*k2**2)/(2.*vev)',
                  order = {'EWL':1})

GC_832 = Coupling(name = 'GC_832',
                  value = '(cth1*cxi**2*complex(0,1)*gw**2*k1**2)/(2.*vev) + (cth1*cxi**2*complex(0,1)*gw**2*k2**2)/(2.*vev)',
                  order = {'EWL':1})

GC_833 = Coupling(name = 'GC_833',
                  value = '-(cphi*cw*cxi*complex(0,1)*gw*gwR*k1**2)/(2.*vev) + (cphi*cw*cxi*complex(0,1)*gw*gwR*k2**2)/(2.*vev)',
                  order = {'EWR':1})

GC_834 = Coupling(name = 'GC_834',
                  value = '(cxi**2*gw*gwR*k1**2)/(2.*vev) - (cxi**2*gw*gwR*k2**2)/(2.*vev)',
                  order = {'EWR':1})

GC_835 = Coupling(name = 'GC_835',
                  value = '-(cxi**2*complex(0,1)*gw*gwR*k1**2)/(2.*vev) + (cxi**2*complex(0,1)*gw*gwR*k2**2)/(2.*vev)',
                  order = {'EWR':1})

GC_836 = Coupling(name = 'GC_836',
                  value = '-(cxi**2*gw*gwR*k1**2)/(2.*vev) + (cxi**2*gw*gwR*k2**2)/(2.*vev)',
                  order = {'EWR':1})

GC_837 = Coupling(name = 'GC_837',
                  value = '-((complex(0,1)*I10a11*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I9a11*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I10a11*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_838 = Coupling(name = 'GC_838',
                  value = '-((complex(0,1)*I10a12*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I9a12*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I10a12*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_839 = Coupling(name = 'GC_839',
                  value = '-((complex(0,1)*I10a13*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I9a13*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I10a13*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_840 = Coupling(name = 'GC_840',
                  value = '-((complex(0,1)*I10a21*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I9a21*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I10a21*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_841 = Coupling(name = 'GC_841',
                  value = '-((complex(0,1)*I10a22*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I9a22*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I10a22*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_842 = Coupling(name = 'GC_842',
                  value = '-((complex(0,1)*I10a23*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I9a23*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I10a23*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_843 = Coupling(name = 'GC_843',
                  value = '-((complex(0,1)*I10a31*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I9a31*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I10a31*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_844 = Coupling(name = 'GC_844',
                  value = '-((complex(0,1)*I10a32*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I9a32*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I10a32*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_845 = Coupling(name = 'GC_845',
                  value = '-((complex(0,1)*I10a33*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I9a33*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I10a33*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_846 = Coupling(name = 'GC_846',
                  value = '(complex(0,1)*I11a11*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I12a11*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I11a11*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_847 = Coupling(name = 'GC_847',
                  value = '(complex(0,1)*I11a12*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I12a12*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I11a12*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_848 = Coupling(name = 'GC_848',
                  value = '(complex(0,1)*I11a13*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I12a13*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I11a13*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_849 = Coupling(name = 'GC_849',
                  value = '(complex(0,1)*I11a21*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I12a21*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I11a21*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_850 = Coupling(name = 'GC_850',
                  value = '(complex(0,1)*I11a22*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I12a22*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I11a22*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_851 = Coupling(name = 'GC_851',
                  value = '(complex(0,1)*I11a23*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I12a23*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I11a23*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_852 = Coupling(name = 'GC_852',
                  value = '(complex(0,1)*I11a31*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I12a31*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I11a31*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_853 = Coupling(name = 'GC_853',
                  value = '(complex(0,1)*I11a32*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I12a32*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I11a32*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_854 = Coupling(name = 'GC_854',
                  value = '(complex(0,1)*I11a33*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I12a33*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I11a33*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_855 = Coupling(name = 'GC_855',
                  value = '(complex(0,1)*I1a11*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I2a11*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I1a11*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_856 = Coupling(name = 'GC_856',
                  value = '(complex(0,1)*I1a12*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I2a12*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I1a12*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_857 = Coupling(name = 'GC_857',
                  value = '(complex(0,1)*I1a13*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I2a13*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I1a13*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_858 = Coupling(name = 'GC_858',
                  value = '(complex(0,1)*I1a21*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I2a21*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I1a21*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_859 = Coupling(name = 'GC_859',
                  value = '(complex(0,1)*I1a22*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I2a22*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I1a22*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_860 = Coupling(name = 'GC_860',
                  value = '(complex(0,1)*I1a23*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I2a23*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I1a23*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_861 = Coupling(name = 'GC_861',
                  value = '(complex(0,1)*I1a31*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I2a31*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I1a31*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_862 = Coupling(name = 'GC_862',
                  value = '(complex(0,1)*I1a32*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I2a32*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I1a32*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_863 = Coupling(name = 'GC_863',
                  value = '(complex(0,1)*I1a33*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I2a33*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I1a33*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_864 = Coupling(name = 'GC_864',
                  value = '-((complex(0,1)*I21a11*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I22a11*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I13a11*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I15a11*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I21a11*k2**2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I22a11*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_865 = Coupling(name = 'GC_865',
                  value = '(I21a11*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (I22a11*k1**2)/((k1 - k2)*(k1 + k2)*vev) - (2*I13a11*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (2*I15a11*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (I21a11*k2**2)/((k1 - k2)*(k1 + k2)*vev) + (I22a11*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_866 = Coupling(name = 'GC_866',
                  value = '-((complex(0,1)*I21a22*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I22a22*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I13a22*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I15a22*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I21a22*k2**2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I22a22*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_867 = Coupling(name = 'GC_867',
                  value = '(I21a22*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (I22a22*k1**2)/((k1 - k2)*(k1 + k2)*vev) - (2*I13a22*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (2*I15a22*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (I21a22*k2**2)/((k1 - k2)*(k1 + k2)*vev) + (I22a22*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_868 = Coupling(name = 'GC_868',
                  value = '-((complex(0,1)*I21a33*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I22a33*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I13a33*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I15a33*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I21a33*k2**2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I22a33*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_869 = Coupling(name = 'GC_869',
                  value = '(I21a33*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (I22a33*k1**2)/((k1 - k2)*(k1 + k2)*vev) - (2*I13a33*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (2*I15a33*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (I21a33*k2**2)/((k1 - k2)*(k1 + k2)*vev) + (I22a33*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_870 = Coupling(name = 'GC_870',
                  value = '-(complex(0,1)*I21a14*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I21a41*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I22a14*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I22a41*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I13a14*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I13a41*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I15a14*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I15a41*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I21a14*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I21a41*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I22a14*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I22a41*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_871 = Coupling(name = 'GC_871',
                  value = '(I21a14*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I21a41*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I22a14*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I22a41*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I13a14*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (I13a41*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (I15a14*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (I15a41*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (I21a14*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I21a41*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I22a14*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I22a41*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_872 = Coupling(name = 'GC_872',
                  value = '-((complex(0,1)*I21a44*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I22a44*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I13a44*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I15a44*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I21a44*k2**2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I22a44*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_873 = Coupling(name = 'GC_873',
                  value = '(I21a44*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (I22a44*k1**2)/((k1 - k2)*(k1 + k2)*vev) - (2*I13a44*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (2*I15a44*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (I21a44*k2**2)/((k1 - k2)*(k1 + k2)*vev) + (I22a44*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_874 = Coupling(name = 'GC_874',
                  value = '-(complex(0,1)*I21a25*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I21a52*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I22a25*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I22a52*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I13a25*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I13a52*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I15a25*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I15a52*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I21a25*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I21a52*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I22a25*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I22a52*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_875 = Coupling(name = 'GC_875',
                  value = '(I21a25*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I21a52*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I22a25*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I22a52*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I13a25*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (I13a52*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (I15a25*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (I15a52*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (I21a25*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I21a52*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I22a25*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I22a52*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_876 = Coupling(name = 'GC_876',
                  value = '-((complex(0,1)*I21a55*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I22a55*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I13a55*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I15a55*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I21a55*k2**2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I22a55*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_877 = Coupling(name = 'GC_877',
                  value = '(I21a55*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (I22a55*k1**2)/((k1 - k2)*(k1 + k2)*vev) - (2*I13a55*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (2*I15a55*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (I21a55*k2**2)/((k1 - k2)*(k1 + k2)*vev) + (I22a55*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_878 = Coupling(name = 'GC_878',
                  value = '-(complex(0,1)*I21a36*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I21a63*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I22a36*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I22a63*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I13a36*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I13a63*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I15a36*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I15a63*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I21a36*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I21a63*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I22a36*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I22a63*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_879 = Coupling(name = 'GC_879',
                  value = '(I21a36*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I21a63*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I22a36*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I22a63*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I13a36*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (I13a63*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (I15a36*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (I15a63*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (I21a36*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I21a63*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I22a36*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I22a63*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_880 = Coupling(name = 'GC_880',
                  value = '-((complex(0,1)*I21a66*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I22a66*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I13a66*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I15a66*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I21a66*k2**2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I22a66*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_881 = Coupling(name = 'GC_881',
                  value = '(I21a66*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (I22a66*k1**2)/((k1 - k2)*(k1 + k2)*vev) - (2*I13a66*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (2*I15a66*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (I21a66*k2**2)/((k1 - k2)*(k1 + k2)*vev) + (I22a66*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_882 = Coupling(name = 'GC_882',
                  value = '-((I23a11*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (I24a11*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (2*I18a11*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (2*I19a11*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (I23a11*k2**2)/((k1 - k2)*(k1 + k2)*vev) - (I24a11*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_883 = Coupling(name = 'GC_883',
                  value = '-((complex(0,1)*I23a11*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I24a11*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I18a11*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I19a11*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I23a11*k2**2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I24a11*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_884 = Coupling(name = 'GC_884',
                  value = '-((I23a22*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (I24a22*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (2*I18a22*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (2*I19a22*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (I23a22*k2**2)/((k1 - k2)*(k1 + k2)*vev) - (I24a22*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_885 = Coupling(name = 'GC_885',
                  value = '-((complex(0,1)*I23a22*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I24a22*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I18a22*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I19a22*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I23a22*k2**2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I24a22*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_886 = Coupling(name = 'GC_886',
                  value = '-((I23a33*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (I24a33*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (2*I18a33*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (2*I19a33*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (I23a33*k2**2)/((k1 - k2)*(k1 + k2)*vev) - (I24a33*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_887 = Coupling(name = 'GC_887',
                  value = '-((complex(0,1)*I23a33*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I24a33*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I18a33*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I19a33*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I23a33*k2**2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I24a33*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_888 = Coupling(name = 'GC_888',
                  value = '-(I23a14*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I23a41*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I24a14*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I24a41*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I18a14*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (I18a41*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (I19a14*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (I19a41*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (I23a14*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I23a41*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I24a14*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I24a41*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_889 = Coupling(name = 'GC_889',
                  value = '-(complex(0,1)*I23a14*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I23a41*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I24a14*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I24a41*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I18a14*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I18a41*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I19a14*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I19a41*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I23a14*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I23a41*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I24a14*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I24a41*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_890 = Coupling(name = 'GC_890',
                  value = '-((I23a44*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (I24a44*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (2*I18a44*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (2*I19a44*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (I23a44*k2**2)/((k1 - k2)*(k1 + k2)*vev) - (I24a44*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_891 = Coupling(name = 'GC_891',
                  value = '-((complex(0,1)*I23a44*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I24a44*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I18a44*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I19a44*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I23a44*k2**2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I24a44*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_892 = Coupling(name = 'GC_892',
                  value = '-(I23a25*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I23a52*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I24a25*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I24a52*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I18a25*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (I18a52*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (I19a25*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (I19a52*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (I23a25*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I23a52*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I24a25*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I24a52*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_893 = Coupling(name = 'GC_893',
                  value = '-(complex(0,1)*I23a25*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I23a52*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I24a25*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I24a52*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I18a25*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I18a52*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I19a25*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I19a52*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I23a25*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I23a52*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I24a25*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I24a52*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_894 = Coupling(name = 'GC_894',
                  value = '-((I23a55*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (I24a55*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (2*I18a55*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (2*I19a55*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (I23a55*k2**2)/((k1 - k2)*(k1 + k2)*vev) - (I24a55*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_895 = Coupling(name = 'GC_895',
                  value = '-((complex(0,1)*I23a55*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I24a55*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I18a55*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I19a55*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I23a55*k2**2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I24a55*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_896 = Coupling(name = 'GC_896',
                  value = '-(I23a36*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I23a63*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I24a36*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I24a63*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (I18a36*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (I18a63*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (I19a36*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (I19a63*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (I23a36*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I23a63*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I24a36*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (I24a63*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_897 = Coupling(name = 'GC_897',
                  value = '-(complex(0,1)*I23a36*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I23a63*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I24a36*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I24a63*k1**2)/(2.*(k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I18a36*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I18a63*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I19a36*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I19a63*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I23a36*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I23a63*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I24a36*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I24a63*k2**2)/(2.*(k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_898 = Coupling(name = 'GC_898',
                  value = '-((I23a66*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (I24a66*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (2*I18a66*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (2*I19a66*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (I23a66*k2**2)/((k1 - k2)*(k1 + k2)*vev) - (I24a66*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_899 = Coupling(name = 'GC_899',
                  value = '-((complex(0,1)*I23a66*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I24a66*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I18a66*k1*k2)/((k1 - k2)*(k1 + k2)*vev) + (2*complex(0,1)*I19a66*k1*k2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I23a66*k2**2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I24a66*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_900 = Coupling(name = 'GC_900',
                  value = '-((complex(0,1)*I26a11*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I25a11*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I26a11*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_901 = Coupling(name = 'GC_901',
                  value = '-((complex(0,1)*I26a22*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I25a22*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I26a22*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_902 = Coupling(name = 'GC_902',
                  value = '-((complex(0,1)*I26a33*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I25a33*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I26a33*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_903 = Coupling(name = 'GC_903',
                  value = '-((complex(0,1)*I26a41*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I25a41*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I26a41*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_904 = Coupling(name = 'GC_904',
                  value = '-((complex(0,1)*I26a52*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I25a52*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I26a52*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_905 = Coupling(name = 'GC_905',
                  value = '-((complex(0,1)*I26a63*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I25a63*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I26a63*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_906 = Coupling(name = 'GC_906',
                  value = '(complex(0,1)*I27a11*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I28a11*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I27a11*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_907 = Coupling(name = 'GC_907',
                  value = '(complex(0,1)*I27a22*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I28a22*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I27a22*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_908 = Coupling(name = 'GC_908',
                  value = '(complex(0,1)*I27a33*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I28a33*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I27a33*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_909 = Coupling(name = 'GC_909',
                  value = '(complex(0,1)*I27a41*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I28a41*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I27a41*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_910 = Coupling(name = 'GC_910',
                  value = '(complex(0,1)*I27a52*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I28a52*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I27a52*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_911 = Coupling(name = 'GC_911',
                  value = '(complex(0,1)*I27a63*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I28a63*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I27a63*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_912 = Coupling(name = 'GC_912',
                  value = '(complex(0,1)*I29a11*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I30a11*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I29a11*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_913 = Coupling(name = 'GC_913',
                  value = '(complex(0,1)*I29a22*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I30a22*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I29a22*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_914 = Coupling(name = 'GC_914',
                  value = '(complex(0,1)*I29a33*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I30a33*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I29a33*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_915 = Coupling(name = 'GC_915',
                  value = '(complex(0,1)*I29a41*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I30a41*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I29a41*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_916 = Coupling(name = 'GC_916',
                  value = '(complex(0,1)*I29a52*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I30a52*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I29a52*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_917 = Coupling(name = 'GC_917',
                  value = '(complex(0,1)*I29a63*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (2*complex(0,1)*I30a63*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) + (complex(0,1)*I29a63*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_918 = Coupling(name = 'GC_918',
                  value = '-((complex(0,1)*I32a11*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I31a11*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I32a11*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_919 = Coupling(name = 'GC_919',
                  value = '-((complex(0,1)*I32a22*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I31a22*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I32a22*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_920 = Coupling(name = 'GC_920',
                  value = '-((complex(0,1)*I32a33*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I31a33*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I32a33*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_921 = Coupling(name = 'GC_921',
                  value = '-((complex(0,1)*I32a41*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I31a41*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I32a41*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_922 = Coupling(name = 'GC_922',
                  value = '-((complex(0,1)*I32a52*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I31a52*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I32a52*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_923 = Coupling(name = 'GC_923',
                  value = '-((complex(0,1)*I32a63*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I31a63*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I32a63*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_924 = Coupling(name = 'GC_924',
                  value = '-((I33a11*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (I33a11*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_925 = Coupling(name = 'GC_925',
                  value = '-((complex(0,1)*I33a11*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I33a11*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_926 = Coupling(name = 'GC_926',
                  value = '-((I33a22*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (I33a22*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_927 = Coupling(name = 'GC_927',
                  value = '-((complex(0,1)*I33a22*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I33a22*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_928 = Coupling(name = 'GC_928',
                  value = '-((I33a33*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (I33a33*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_929 = Coupling(name = 'GC_929',
                  value = '-((complex(0,1)*I33a33*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I33a33*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_930 = Coupling(name = 'GC_930',
                  value = '-((complex(0,1)*I34a11*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I34a11*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_931 = Coupling(name = 'GC_931',
                  value = '(I34a11*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (I34a11*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_932 = Coupling(name = 'GC_932',
                  value = '-((complex(0,1)*I34a22*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I34a22*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_933 = Coupling(name = 'GC_933',
                  value = '(I34a22*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (I34a22*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_934 = Coupling(name = 'GC_934',
                  value = '-((complex(0,1)*I34a33*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I34a33*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_935 = Coupling(name = 'GC_935',
                  value = '(I34a33*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (I34a33*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_936 = Coupling(name = 'GC_936',
                  value = '-((complex(0,1)*I4a11*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I3a11*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I4a11*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_937 = Coupling(name = 'GC_937',
                  value = '-((complex(0,1)*I4a12*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I3a12*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I4a12*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_938 = Coupling(name = 'GC_938',
                  value = '-((complex(0,1)*I4a13*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I3a13*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I4a13*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_939 = Coupling(name = 'GC_939',
                  value = '-((complex(0,1)*I4a21*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I3a21*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I4a21*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_940 = Coupling(name = 'GC_940',
                  value = '-((complex(0,1)*I4a22*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I3a22*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I4a22*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_941 = Coupling(name = 'GC_941',
                  value = '-((complex(0,1)*I4a23*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I3a23*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I4a23*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_942 = Coupling(name = 'GC_942',
                  value = '-((complex(0,1)*I4a31*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I3a31*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I4a31*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_943 = Coupling(name = 'GC_943',
                  value = '-((complex(0,1)*I4a32*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I3a32*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I4a32*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_944 = Coupling(name = 'GC_944',
                  value = '-((complex(0,1)*I4a33*k1**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)) + (2*complex(0,1)*I3a33*k1*k2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*I4a33*k2**2*cmath.sqrt(2))/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_945 = Coupling(name = 'GC_945',
                  value = '-((I5a11*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (I5a11*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_946 = Coupling(name = 'GC_946',
                  value = '-((complex(0,1)*I5a11*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I5a11*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_947 = Coupling(name = 'GC_947',
                  value = '-((I5a12*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (I5a12*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_948 = Coupling(name = 'GC_948',
                  value = '-((complex(0,1)*I5a12*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I5a12*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_949 = Coupling(name = 'GC_949',
                  value = '-((I5a13*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (I5a13*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_950 = Coupling(name = 'GC_950',
                  value = '-((complex(0,1)*I5a13*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I5a13*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_951 = Coupling(name = 'GC_951',
                  value = '-((I5a21*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (I5a21*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_952 = Coupling(name = 'GC_952',
                  value = '-((complex(0,1)*I5a21*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I5a21*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_953 = Coupling(name = 'GC_953',
                  value = '-((I5a22*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (I5a22*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_954 = Coupling(name = 'GC_954',
                  value = '-((complex(0,1)*I5a22*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I5a22*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_955 = Coupling(name = 'GC_955',
                  value = '-((I5a23*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (I5a23*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_956 = Coupling(name = 'GC_956',
                  value = '-((complex(0,1)*I5a23*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I5a23*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_957 = Coupling(name = 'GC_957',
                  value = '-((I5a31*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (I5a31*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_958 = Coupling(name = 'GC_958',
                  value = '-((complex(0,1)*I5a31*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I5a31*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_959 = Coupling(name = 'GC_959',
                  value = '-((I5a32*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (I5a32*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_960 = Coupling(name = 'GC_960',
                  value = '-((complex(0,1)*I5a32*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I5a32*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_961 = Coupling(name = 'GC_961',
                  value = '-((I5a33*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (I5a33*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_962 = Coupling(name = 'GC_962',
                  value = '-((complex(0,1)*I5a33*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I5a33*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_963 = Coupling(name = 'GC_963',
                  value = '-((complex(0,1)*I6a11*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I6a11*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_964 = Coupling(name = 'GC_964',
                  value = '(I6a11*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (I6a11*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_965 = Coupling(name = 'GC_965',
                  value = '-((complex(0,1)*I6a12*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I6a12*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_966 = Coupling(name = 'GC_966',
                  value = '(I6a12*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (I6a12*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_967 = Coupling(name = 'GC_967',
                  value = '-((complex(0,1)*I6a13*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I6a13*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_968 = Coupling(name = 'GC_968',
                  value = '(I6a13*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (I6a13*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_969 = Coupling(name = 'GC_969',
                  value = '-((complex(0,1)*I6a21*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I6a21*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_970 = Coupling(name = 'GC_970',
                  value = '(I6a21*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (I6a21*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_971 = Coupling(name = 'GC_971',
                  value = '-((complex(0,1)*I6a22*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I6a22*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_972 = Coupling(name = 'GC_972',
                  value = '(I6a22*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (I6a22*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_973 = Coupling(name = 'GC_973',
                  value = '-((complex(0,1)*I6a23*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I6a23*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_974 = Coupling(name = 'GC_974',
                  value = '(I6a23*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (I6a23*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_975 = Coupling(name = 'GC_975',
                  value = '-((complex(0,1)*I6a31*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I6a31*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_976 = Coupling(name = 'GC_976',
                  value = '(I6a31*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (I6a31*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_977 = Coupling(name = 'GC_977',
                  value = '-((complex(0,1)*I6a32*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I6a32*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_978 = Coupling(name = 'GC_978',
                  value = '(I6a32*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (I6a32*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_979 = Coupling(name = 'GC_979',
                  value = '-((complex(0,1)*I6a33*k1**2)/((k1 - k2)*(k1 + k2)*vev)) - (complex(0,1)*I6a33*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_980 = Coupling(name = 'GC_980',
                  value = '(I6a33*k1**2)/((k1 - k2)*(k1 + k2)*vev) + (I6a33*k2**2)/((k1 - k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_981 = Coupling(name = 'GC_981',
                  value = '(cth1*complex(0,1)*I13a11*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I15a11*k1**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I13a11*k2**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I15a11*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_982 = Coupling(name = 'GC_982',
                  value = '(cth1*complex(0,1)*I13a22*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I15a22*k1**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I13a22*k2**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I15a22*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_983 = Coupling(name = 'GC_983',
                  value = '(cth1*complex(0,1)*I13a33*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I15a33*k1**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I13a33*k2**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I15a33*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_984 = Coupling(name = 'GC_984',
                  value = '(cth1*complex(0,1)*I13a14*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I13a41*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I15a14*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I15a41*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I13a14*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I13a41*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I15a14*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I15a41*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_985 = Coupling(name = 'GC_985',
                  value = '(cth1*complex(0,1)*I13a44*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I15a44*k1**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I13a44*k2**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I15a44*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_986 = Coupling(name = 'GC_986',
                  value = '(cth1*complex(0,1)*I13a25*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I13a52*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I15a25*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I15a52*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I13a25*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I13a52*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I15a25*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I15a52*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_987 = Coupling(name = 'GC_987',
                  value = '(cth1*complex(0,1)*I13a55*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I15a55*k1**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I13a55*k2**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I15a55*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_988 = Coupling(name = 'GC_988',
                  value = '(cth1*complex(0,1)*I13a36*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I13a63*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I15a36*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I15a63*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I13a36*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I13a63*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I15a36*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I15a63*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_989 = Coupling(name = 'GC_989',
                  value = '(cth1*complex(0,1)*I13a66*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I15a66*k1**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I13a66*k2**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I15a66*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_990 = Coupling(name = 'GC_990',
                  value = '(cth1*complex(0,1)*I18a11*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I19a11*k1**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I18a11*k2**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I19a11*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_991 = Coupling(name = 'GC_991',
                  value = '(cth1*complex(0,1)*I18a22*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I19a22*k1**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I18a22*k2**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I19a22*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_992 = Coupling(name = 'GC_992',
                  value = '(cth1*complex(0,1)*I18a33*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I19a33*k1**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I18a33*k2**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I19a33*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_993 = Coupling(name = 'GC_993',
                  value = '(cth1*complex(0,1)*I18a14*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I18a41*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I19a14*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I19a41*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I18a14*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I18a41*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I19a14*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I19a41*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_994 = Coupling(name = 'GC_994',
                  value = '(cth1*complex(0,1)*I18a44*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I19a44*k1**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I18a44*k2**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I19a44*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_995 = Coupling(name = 'GC_995',
                  value = '(cth1*complex(0,1)*I18a25*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I18a52*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I19a25*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I19a52*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I18a25*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I18a52*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I19a25*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I19a52*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_996 = Coupling(name = 'GC_996',
                  value = '(cth1*complex(0,1)*I18a55*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I19a55*k1**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I18a55*k2**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I19a55*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_997 = Coupling(name = 'GC_997',
                  value = '(cth1*complex(0,1)*I18a36*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I18a63*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I19a36*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I19a63*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I18a36*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I18a63*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I19a36*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I19a63*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_998 = Coupling(name = 'GC_998',
                  value = '(cth1*complex(0,1)*I18a66*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (cth1*complex(0,1)*I19a66*k1**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I18a66*k2**2)/((-k1 + k2)*(k1 + k2)*vev) - (cth1*complex(0,1)*I19a66*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_999 = Coupling(name = 'GC_999',
                  value = '-((I7a11*k1**2)/((-k1 + k2)*(k1 + k2)*vev)) - (I7a11*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                  order = {'EWL':1})

GC_1000 = Coupling(name = 'GC_1000',
                   value = '(complex(0,1)*I7a11*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I7a11*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1001 = Coupling(name = 'GC_1001',
                   value = '-((I7a12*k1**2)/((-k1 + k2)*(k1 + k2)*vev)) - (I7a12*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1002 = Coupling(name = 'GC_1002',
                   value = '(complex(0,1)*I7a12*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I7a12*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1003 = Coupling(name = 'GC_1003',
                   value = '-((I7a13*k1**2)/((-k1 + k2)*(k1 + k2)*vev)) - (I7a13*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1004 = Coupling(name = 'GC_1004',
                   value = '(complex(0,1)*I7a13*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I7a13*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1005 = Coupling(name = 'GC_1005',
                   value = '-((I7a21*k1**2)/((-k1 + k2)*(k1 + k2)*vev)) - (I7a21*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1006 = Coupling(name = 'GC_1006',
                   value = '(complex(0,1)*I7a21*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I7a21*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1007 = Coupling(name = 'GC_1007',
                   value = '-((I7a22*k1**2)/((-k1 + k2)*(k1 + k2)*vev)) - (I7a22*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1008 = Coupling(name = 'GC_1008',
                   value = '(complex(0,1)*I7a22*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I7a22*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1009 = Coupling(name = 'GC_1009',
                   value = '-((I7a23*k1**2)/((-k1 + k2)*(k1 + k2)*vev)) - (I7a23*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1010 = Coupling(name = 'GC_1010',
                   value = '(complex(0,1)*I7a23*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I7a23*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1011 = Coupling(name = 'GC_1011',
                   value = '-((I7a31*k1**2)/((-k1 + k2)*(k1 + k2)*vev)) - (I7a31*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1012 = Coupling(name = 'GC_1012',
                   value = '(complex(0,1)*I7a31*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I7a31*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1013 = Coupling(name = 'GC_1013',
                   value = '-((I7a32*k1**2)/((-k1 + k2)*(k1 + k2)*vev)) - (I7a32*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1014 = Coupling(name = 'GC_1014',
                   value = '(complex(0,1)*I7a32*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I7a32*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1015 = Coupling(name = 'GC_1015',
                   value = '-((I7a33*k1**2)/((-k1 + k2)*(k1 + k2)*vev)) - (I7a33*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1016 = Coupling(name = 'GC_1016',
                   value = '(complex(0,1)*I7a33*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I7a33*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1017 = Coupling(name = 'GC_1017',
                   value = '(complex(0,1)*I8a11*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I8a11*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1018 = Coupling(name = 'GC_1018',
                   value = '(I8a11*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (I8a11*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1019 = Coupling(name = 'GC_1019',
                   value = '(complex(0,1)*I8a12*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I8a12*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1020 = Coupling(name = 'GC_1020',
                   value = '(I8a12*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (I8a12*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1021 = Coupling(name = 'GC_1021',
                   value = '(complex(0,1)*I8a13*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I8a13*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1022 = Coupling(name = 'GC_1022',
                   value = '(I8a13*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (I8a13*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1023 = Coupling(name = 'GC_1023',
                   value = '(complex(0,1)*I8a21*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I8a21*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1024 = Coupling(name = 'GC_1024',
                   value = '(I8a21*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (I8a21*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1025 = Coupling(name = 'GC_1025',
                   value = '(complex(0,1)*I8a22*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I8a22*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1026 = Coupling(name = 'GC_1026',
                   value = '(I8a22*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (I8a22*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1027 = Coupling(name = 'GC_1027',
                   value = '(complex(0,1)*I8a23*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I8a23*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1028 = Coupling(name = 'GC_1028',
                   value = '(I8a23*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (I8a23*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1029 = Coupling(name = 'GC_1029',
                   value = '(complex(0,1)*I8a31*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I8a31*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1030 = Coupling(name = 'GC_1030',
                   value = '(I8a31*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (I8a31*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1031 = Coupling(name = 'GC_1031',
                   value = '(complex(0,1)*I8a32*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I8a32*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1032 = Coupling(name = 'GC_1032',
                   value = '(I8a32*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (I8a32*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1033 = Coupling(name = 'GC_1033',
                   value = '(complex(0,1)*I8a33*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I8a33*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1034 = Coupling(name = 'GC_1034',
                   value = '(I8a33*k1**2)/((-k1 + k2)*(k1 + k2)*vev) + (I8a33*k2**2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1035 = Coupling(name = 'GC_1035',
                   value = '-(cw*cxi*complex(0,1)*gw*gwR*k1**2*sphi)/(2.*vev) + (cw*cxi*complex(0,1)*gw*gwR*k2**2*sphi)/(2.*vev)',
                   order = {'EWL':-1,'EWR':2})

GC_1036 = Coupling(name = 'GC_1036',
                   value = '-(cphi**2*cw**2*complex(0,1)*gw**2*k1**2*sth1)/(2.*vev) - (cphi**2*cw**2*complex(0,1)*gw**2*k2**2*sth1)/(2.*vev)',
                   order = {'EWL':1})

GC_1037 = Coupling(name = 'GC_1037',
                   value = '-(cxi**2*complex(0,1)*gw**2*k1**2*sth1)/(2.*vev) - (cxi**2*complex(0,1)*gw**2*k2**2*sth1)/(2.*vev)',
                   order = {'EWL':1})

GC_1038 = Coupling(name = 'GC_1038',
                   value = '-((complex(0,1)*I13a11*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)) - (complex(0,1)*I15a11*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I13a11*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I15a11*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1039 = Coupling(name = 'GC_1039',
                   value = '-((complex(0,1)*I13a22*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)) - (complex(0,1)*I15a22*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I13a22*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I15a22*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1040 = Coupling(name = 'GC_1040',
                   value = '-((complex(0,1)*I13a33*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)) - (complex(0,1)*I15a33*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I13a33*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I15a33*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1041 = Coupling(name = 'GC_1041',
                   value = '-(complex(0,1)*I13a14*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (complex(0,1)*I13a41*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (complex(0,1)*I15a14*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (complex(0,1)*I15a41*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I13a14*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I13a41*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I15a14*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I15a41*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1042 = Coupling(name = 'GC_1042',
                   value = '-((complex(0,1)*I13a44*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)) - (complex(0,1)*I15a44*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I13a44*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I15a44*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1043 = Coupling(name = 'GC_1043',
                   value = '-(complex(0,1)*I13a25*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (complex(0,1)*I13a52*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (complex(0,1)*I15a25*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (complex(0,1)*I15a52*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I13a25*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I13a52*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I15a25*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I15a52*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1044 = Coupling(name = 'GC_1044',
                   value = '-((complex(0,1)*I13a55*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)) - (complex(0,1)*I15a55*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I13a55*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I15a55*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1045 = Coupling(name = 'GC_1045',
                   value = '-(complex(0,1)*I13a36*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (complex(0,1)*I13a63*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (complex(0,1)*I15a36*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (complex(0,1)*I15a63*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I13a36*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I13a63*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I15a36*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I15a63*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1046 = Coupling(name = 'GC_1046',
                   value = '-((complex(0,1)*I13a66*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)) - (complex(0,1)*I15a66*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I13a66*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I15a66*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1047 = Coupling(name = 'GC_1047',
                   value = '-((complex(0,1)*I18a11*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)) - (complex(0,1)*I19a11*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I18a11*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I19a11*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1048 = Coupling(name = 'GC_1048',
                   value = '-((complex(0,1)*I18a22*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)) - (complex(0,1)*I19a22*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I18a22*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I19a22*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1049 = Coupling(name = 'GC_1049',
                   value = '-((complex(0,1)*I18a33*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)) - (complex(0,1)*I19a33*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I18a33*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I19a33*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1050 = Coupling(name = 'GC_1050',
                   value = '-(complex(0,1)*I18a14*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (complex(0,1)*I18a41*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (complex(0,1)*I19a14*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (complex(0,1)*I19a41*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I18a14*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I18a41*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I19a14*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I19a41*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1051 = Coupling(name = 'GC_1051',
                   value = '-((complex(0,1)*I18a44*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)) - (complex(0,1)*I19a44*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I18a44*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I19a44*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1052 = Coupling(name = 'GC_1052',
                   value = '-(complex(0,1)*I18a25*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (complex(0,1)*I18a52*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (complex(0,1)*I19a25*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (complex(0,1)*I19a52*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I18a25*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I18a52*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I19a25*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I19a52*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1053 = Coupling(name = 'GC_1053',
                   value = '-((complex(0,1)*I18a55*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)) - (complex(0,1)*I19a55*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I18a55*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I19a55*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1054 = Coupling(name = 'GC_1054',
                   value = '-(complex(0,1)*I18a36*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (complex(0,1)*I18a63*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (complex(0,1)*I19a36*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) - (complex(0,1)*I19a63*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I18a36*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I18a63*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I19a36*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I19a63*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1055 = Coupling(name = 'GC_1055',
                   value = '-((complex(0,1)*I18a66*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)) - (complex(0,1)*I19a66*k1**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I18a66*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev) + (complex(0,1)*I19a66*k2**2*sth1)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1056 = Coupling(name = 'GC_1056',
                   value = '(cphi*cth1*cw*complex(0,1)*gw**2*k1**2*sw)/(2.*vev) + (cphi*cth1*cw*complex(0,1)*gw**2*k2**2*sw)/(2.*vev)',
                   order = {'EWL':1})

GC_1057 = Coupling(name = 'GC_1057',
                   value = '-(cxi*complex(0,1)*gw*gwR*k1**2*sw)/(2.*vev) + (cxi*complex(0,1)*gw*gwR*k2**2*sw)/(2.*vev)',
                   order = {'EWR':1})

GC_1058 = Coupling(name = 'GC_1058',
                   value = '-(cphi*cw*complex(0,1)*gw**2*k1**2*sth1*sw)/(2.*vev) - (cphi*cw*complex(0,1)*gw**2*k2**2*sth1*sw)/(2.*vev)',
                   order = {'EWL':1})

GC_1059 = Coupling(name = 'GC_1059',
                   value = '(cth1*complex(0,1)*gw**2*k1**2*sw**2)/(2.*vev) + (cth1*complex(0,1)*gw**2*k2**2*sw**2)/(2.*vev)',
                   order = {'EWL':1})

GC_1060 = Coupling(name = 'GC_1060',
                   value = '(cth1*complex(0,1)*gwR**2*k1**2*sw**2)/(2.*vev) + (cth1*complex(0,1)*gwR**2*k2**2*sw**2)/(2.*vev)',
                   order = {'EWL':-1,'EWR':2})

GC_1061 = Coupling(name = 'GC_1061',
                   value = '(cth1*complex(0,1)*gwR**2*k1**2*sphi**2)/(2.*cw**2*vev) + (cth1*complex(0,1)*gwR**2*k2**2*sphi**2)/(2.*cw**2*vev) - (cth1*complex(0,1)*gwR**2*k1**2*sphi**2*sw**2)/(cw**2*vev) - (cth1*complex(0,1)*gwR**2*k2**2*sphi**2*sw**2)/(cw**2*vev)',
                   order = {'EWL':-3,'EWR':4})

GC_1062 = Coupling(name = 'GC_1062',
                   value = '-(complex(0,1)*gw**2*k1**2*sth1*sw**2)/(2.*vev) - (complex(0,1)*gw**2*k2**2*sth1*sw**2)/(2.*vev)',
                   order = {'EWL':1})

GC_1063 = Coupling(name = 'GC_1063',
                   value = '-(complex(0,1)*gwR**2*k1**2*sth1*sw**2)/(2.*vev) - (complex(0,1)*gwR**2*k2**2*sth1*sw**2)/(2.*vev)',
                   order = {'EWL':-1,'EWR':2})

GC_1064 = Coupling(name = 'GC_1064',
                   value = '-(complex(0,1)*gwR**2*k1**2*sphi**2*sth1)/(2.*cw**2*vev) - (complex(0,1)*gwR**2*k2**2*sphi**2*sth1)/(2.*cw**2*vev) + (complex(0,1)*gwR**2*k1**2*sphi**2*sth1*sw**2)/(cw**2*vev) + (complex(0,1)*gwR**2*k2**2*sphi**2*sth1*sw**2)/(cw**2*vev)',
                   order = {'EWL':-3,'EWR':4})

GC_1065 = Coupling(name = 'GC_1065',
                   value = '-(cth1*complex(0,1)*gwR**2*k1**2*sphi*sw**3)/(2.*cw*vev) - (cth1*complex(0,1)*gwR**2*k2**2*sphi*sw**3)/(2.*cw*vev)',
                   order = {'EWL':-2,'EWR':3})

GC_1066 = Coupling(name = 'GC_1066',
                   value = '(complex(0,1)*gwR**2*k1**2*sphi*sth1*sw**3)/(2.*cw*vev) + (complex(0,1)*gwR**2*k2**2*sphi*sth1*sw**3)/(2.*cw*vev)',
                   order = {'EWL':-2,'EWR':3})

GC_1067 = Coupling(name = 'GC_1067',
                   value = '(cth1*complex(0,1)*gwR**2*k1**2*sphi**2*sw**4)/(2.*cw**2*vev) + (cth1*complex(0,1)*gwR**2*k2**2*sphi**2*sw**4)/(2.*cw**2*vev)',
                   order = {'EWL':-3,'EWR':4})

GC_1068 = Coupling(name = 'GC_1068',
                   value = '-(complex(0,1)*gwR**2*k1**2*sphi**2*sth1*sw**4)/(2.*cw**2*vev) - (complex(0,1)*gwR**2*k2**2*sphi**2*sth1*sw**4)/(2.*cw**2*vev)',
                   order = {'EWL':-3,'EWR':4})

GC_1069 = Coupling(name = 'GC_1069',
                   value = '-(cth1*complex(0,1)*gwR**2*k1**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev) - (cth1*complex(0,1)*gwR**2*k2**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev)',
                   order = {'EWL':-2,'EWR':3})

GC_1070 = Coupling(name = 'GC_1070',
                   value = '(complex(0,1)*gwR**2*k1**2*sphi*sth1*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev) + (complex(0,1)*gwR**2*k2**2*sphi*sth1*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev)',
                   order = {'EWL':-2,'EWR':3})

GC_1071 = Coupling(name = 'GC_1071',
                   value = '(cth1*complex(0,1)*gwR**2*k1**2*sphi**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev) + (cth1*complex(0,1)*gwR**2*k2**2*sphi**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev)',
                   order = {'EWL':-3,'EWR':4})

GC_1072 = Coupling(name = 'GC_1072',
                   value = '-(complex(0,1)*gwR**2*k1**2*sphi**2*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev) - (complex(0,1)*gwR**2*k2**2*sphi**2*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev)',
                   order = {'EWL':-3,'EWR':4})

GC_1073 = Coupling(name = 'GC_1073',
                   value = '-((cth1*cxi**2*complex(0,1)*gw*gwR*k1*k2)/vev) + (cth1*cxi*complex(0,1)*gw**2*k1**2*sxi)/(2.*vev) + (cth1*cxi*complex(0,1)*gw**2*k2**2*sxi)/(2.*vev)',
                   order = {'EWR':1})

GC_1074 = Coupling(name = 'GC_1074',
                   value = '(cphi*cw*complex(0,1)*gw*gwR*k1**2*sxi)/(2.*vev) - (cphi*cw*complex(0,1)*gw*gwR*k2**2*sxi)/(2.*vev)',
                   order = {'EWL':-1,'EWR':2})

GC_1075 = Coupling(name = 'GC_1075',
                   value = '(cxi*complex(0,1)*gw*gwR*k1**2*sxi)/vev - (cxi*complex(0,1)*gw*gwR*k2**2*sxi)/vev',
                   order = {'EWL':-1,'EWR':2})

GC_1076 = Coupling(name = 'GC_1076',
                   value = '-((cxi*complex(0,1)*gw*gwR*k1**2*sxi)/vev) + (cxi*complex(0,1)*gw*gwR*k2**2*sxi)/vev',
                   order = {'EWL':-1,'EWR':2})

GC_1077 = Coupling(name = 'GC_1077',
                   value = '(cw*complex(0,1)*gw*gwR*k1**2*sphi*sxi)/(2.*vev) - (cw*complex(0,1)*gw*gwR*k2**2*sphi*sxi)/(2.*vev)',
                   order = {'EWL':-2,'EWR':3})

GC_1078 = Coupling(name = 'GC_1078',
                   value = '(cxi**2*complex(0,1)*gw*gwR*k1*k2*sth1)/vev - (cxi*complex(0,1)*gw**2*k1**2*sth1*sxi)/(2.*vev) - (cxi*complex(0,1)*gw**2*k2**2*sth1*sxi)/(2.*vev)',
                   order = {'EWR':1})

GC_1079 = Coupling(name = 'GC_1079',
                   value = '(complex(0,1)*gw*gwR*k1**2*sw*sxi)/(2.*vev) - (complex(0,1)*gw*gwR*k2**2*sw*sxi)/(2.*vev)',
                   order = {'EWL':-1,'EWR':2})

GC_1080 = Coupling(name = 'GC_1080',
                   value = '-(cth1*cxi*complex(0,1)*gwR**2*k1**2*sxi)/(2.*vev) - (cth1*cxi*complex(0,1)*gwR**2*k2**2*sxi)/(2.*vev) + (cth1*complex(0,1)*gw*gwR*k1*k2*sxi**2)/vev',
                   order = {'EWL':-2,'EWR':3})

GC_1081 = Coupling(name = 'GC_1081',
                   value = '(cth1*cxi**2*complex(0,1)*gwR**2*k1**2)/(2.*vev) + (cth1*cxi**2*complex(0,1)*gwR**2*k2**2)/(2.*vev) - (2*cth1*cxi*complex(0,1)*gw*gwR*k1*k2*sxi)/vev + (cth1*complex(0,1)*gw**2*k1**2*sxi**2)/(2.*vev) + (cth1*complex(0,1)*gw**2*k2**2*sxi**2)/(2.*vev)',
                   order = {'EWL':-1,'EWR':2})

GC_1082 = Coupling(name = 'GC_1082',
                   value = '(gw*gwR*k1**2*sxi**2)/(2.*vev) - (gw*gwR*k2**2*sxi**2)/(2.*vev)',
                   order = {'EWL':-2,'EWR':3})

GC_1083 = Coupling(name = 'GC_1083',
                   value = '(complex(0,1)*gw*gwR*k1**2*sxi**2)/(2.*vev) - (complex(0,1)*gw*gwR*k2**2*sxi**2)/(2.*vev)',
                   order = {'EWL':-2,'EWR':3})

GC_1084 = Coupling(name = 'GC_1084',
                   value = '-(gw*gwR*k1**2*sxi**2)/(2.*vev) + (gw*gwR*k2**2*sxi**2)/(2.*vev)',
                   order = {'EWL':-2,'EWR':3})

GC_1085 = Coupling(name = 'GC_1085',
                   value = '(cth1*complex(0,1)*gwR**2*k1**2*sxi**2)/(2.*vev) + (cth1*complex(0,1)*gwR**2*k2**2*sxi**2)/(2.*vev)',
                   order = {'EWL':-3,'EWR':4})

GC_1086 = Coupling(name = 'GC_1086',
                   value = '(cxi*complex(0,1)*gwR**2*k1**2*sth1*sxi)/(2.*vev) + (cxi*complex(0,1)*gwR**2*k2**2*sth1*sxi)/(2.*vev) - (complex(0,1)*gw*gwR*k1*k2*sth1*sxi**2)/vev',
                   order = {'EWL':-2,'EWR':3})

GC_1087 = Coupling(name = 'GC_1087',
                   value = '-(cxi**2*complex(0,1)*gwR**2*k1**2*sth1)/(2.*vev) - (cxi**2*complex(0,1)*gwR**2*k2**2*sth1)/(2.*vev) + (2*cxi*complex(0,1)*gw*gwR*k1*k2*sth1*sxi)/vev - (complex(0,1)*gw**2*k1**2*sth1*sxi**2)/(2.*vev) - (complex(0,1)*gw**2*k2**2*sth1*sxi**2)/(2.*vev)',
                   order = {'EWL':-1,'EWR':2})

GC_1088 = Coupling(name = 'GC_1088',
                   value = '-(complex(0,1)*gwR**2*k1**2*sth1*sxi**2)/(2.*vev) - (complex(0,1)*gwR**2*k2**2*sth1*sxi**2)/(2.*vev)',
                   order = {'EWL':-3,'EWR':4})

GC_1089 = Coupling(name = 'GC_1089',
                   value = '-((cphi*cw*cxi*gw*gwR*k1*k2)/vev**2)',
                   order = {'EWL':1,'EWR':1})

GC_1090 = Coupling(name = 'GC_1090',
                   value = '(cphi*cw*cxi*complex(0,1)*gw*gwR*k1*k2)/vev**2',
                   order = {'EWL':1,'EWR':1})

GC_1091 = Coupling(name = 'GC_1091',
                   value = '(cphi*cw*cxi*gw*gwR*k1*k2)/vev**2',
                   order = {'EWL':1,'EWR':1})

GC_1092 = Coupling(name = 'GC_1092',
                   value = '-((cxi**2*gw*gwR*k1*k2)/vev**2)',
                   order = {'EWL':1,'EWR':1})

GC_1093 = Coupling(name = 'GC_1093',
                   value = '(-2*cxi**2*complex(0,1)*gw*gwR*k1*k2)/vev**2',
                   order = {'EWL':1,'EWR':1})

GC_1094 = Coupling(name = 'GC_1094',
                   value = '(cxi**2*gw*gwR*k1*k2)/vev**2',
                   order = {'EWL':1,'EWR':1})

GC_1095 = Coupling(name = 'GC_1095',
                   value = '-((cxi*gw*gwR*k1*k2*sw)/vev**2)',
                   order = {'EWL':1,'EWR':1})

GC_1096 = Coupling(name = 'GC_1096',
                   value = '(cxi*complex(0,1)*gw*gwR*k1*k2*sw)/vev**2',
                   order = {'EWL':1,'EWR':1})

GC_1097 = Coupling(name = 'GC_1097',
                   value = '(cxi*gw*gwR*k1*k2*sw)/vev**2',
                   order = {'EWL':1,'EWR':1})

GC_1098 = Coupling(name = 'GC_1098',
                   value = '-((complex(0,1)*gwR*k1*k2*sxi)/vev**2)',
                   order = {'EWL':-1,'EWR':2})

GC_1099 = Coupling(name = 'GC_1099',
                   value = '(complex(0,1)*gwR*k1*k2*sxi)/vev**2',
                   order = {'EWL':-1,'EWR':2})

GC_1100 = Coupling(name = 'GC_1100',
                   value = '(gwR*k1*k2*sxi)/vev**2',
                   order = {'EWL':-1,'EWR':2})

GC_1101 = Coupling(name = 'GC_1101',
                   value = '(-2*cxi*complex(0,1)*gw*gwR*k1*k2*sxi)/vev**2',
                   order = {'EWR':2})

GC_1102 = Coupling(name = 'GC_1102',
                   value = '(2*cxi*complex(0,1)*gw*gwR*k1*k2*sxi)/vev**2',
                   order = {'EWR':2})

GC_1103 = Coupling(name = 'GC_1103',
                   value = '(-4*cxi*complex(0,1)*gw*gwR*k1*k2*sxi)/vev**2',
                   order = {'EWR':2})

GC_1104 = Coupling(name = 'GC_1104',
                   value = '(4*cxi*complex(0,1)*gw*gwR*k1*k2*sxi)/vev**2',
                   order = {'EWR':2})

GC_1105 = Coupling(name = 'GC_1105',
                   value = '(2*cth1**2*cxi*complex(0,1)*gw*gwR*k1*k2*sxi)/vev**2',
                   order = {'EWR':2})

GC_1106 = Coupling(name = 'GC_1106',
                   value = '-((cw*gw*gwR*k1*k2*sphi*sxi)/vev**2)',
                   order = {'EWL':-1,'EWR':3})

GC_1107 = Coupling(name = 'GC_1107',
                   value = '-((cw*complex(0,1)*gw*gwR*k1*k2*sphi*sxi)/vev**2)',
                   order = {'EWL':-1,'EWR':3})

GC_1108 = Coupling(name = 'GC_1108',
                   value = '(cw*gw*gwR*k1*k2*sphi*sxi)/vev**2',
                   order = {'EWL':-1,'EWR':3})

GC_1109 = Coupling(name = 'GC_1109',
                   value = '(-2*cth1*cxi*complex(0,1)*gw*gwR*k1*k2*sth1*sxi)/vev**2',
                   order = {'EWR':2})

GC_1110 = Coupling(name = 'GC_1110',
                   value = '(2*cxi*complex(0,1)*gw*gwR*k1*k2*sth1**2*sxi)/vev**2',
                   order = {'EWR':2})

GC_1111 = Coupling(name = 'GC_1111',
                   value = '-((gw*gwR*k1*k2*sw*sxi)/vev**2)',
                   order = {'EWR':2})

GC_1112 = Coupling(name = 'GC_1112',
                   value = '-((complex(0,1)*gw*gwR*k1*k2*sw*sxi)/vev**2)',
                   order = {'EWR':2})

GC_1113 = Coupling(name = 'GC_1113',
                   value = '(gw*gwR*k1*k2*sw*sxi)/vev**2',
                   order = {'EWR':2})

GC_1114 = Coupling(name = 'GC_1114',
                   value = '-((gw*gwR*k1*k2*sxi**2)/vev**2)',
                   order = {'EWL':-1,'EWR':3})

GC_1115 = Coupling(name = 'GC_1115',
                   value = '(2*complex(0,1)*gw*gwR*k1*k2*sxi**2)/vev**2',
                   order = {'EWL':-1,'EWR':3})

GC_1116 = Coupling(name = 'GC_1116',
                   value = '(gw*gwR*k1*k2*sxi**2)/vev**2',
                   order = {'EWL':-1,'EWR':3})

GC_1117 = Coupling(name = 'GC_1117',
                   value = '(2*cth1*cxi*complex(0,1)*gw*gwR*k1*k2*sxi)/vev',
                   order = {'EWL':-1,'EWR':2})

GC_1118 = Coupling(name = 'GC_1118',
                   value = '(-2*cxi*complex(0,1)*gw*gwR*k1*k2*sth1*sxi)/vev',
                   order = {'EWL':-1,'EWR':2})

GC_1119 = Coupling(name = 'GC_1119',
                   value = '2*cphi**2*cw**2*complex(0,1)*gw**2*vL',
                   order = {'EWL':1})

GC_1120 = Coupling(name = 'GC_1120',
                   value = '-((cphi*cw*cxi*complex(0,1)*gw**2*vL)/cmath.sqrt(2))',
                   order = {'EWL':1})

GC_1121 = Coupling(name = 'GC_1121',
                   value = 'cxi**2*complex(0,1)*gw**2*vL',
                   order = {'EWL':1})

GC_1122 = Coupling(name = 'GC_1122',
                   value = '-(cxi**2*complex(0,1)*gw**2*vL*cmath.sqrt(2))',
                   order = {'EWL':1})

GC_1123 = Coupling(name = 'GC_1123',
                   value = '2*cphi*cw**2*complex(0,1)*gw**2*sphi*vL',
                   order = {'EWR':1})

GC_1124 = Coupling(name = 'GC_1124',
                   value = '-((cw*cxi*complex(0,1)*gw**2*sphi*vL)/cmath.sqrt(2))',
                   order = {'EWR':1})

GC_1125 = Coupling(name = 'GC_1125',
                   value = '2*cw**2*complex(0,1)*gw**2*sphi**2*vL',
                   order = {'EWL':-1,'EWR':2})

GC_1126 = Coupling(name = 'GC_1126',
                   value = '2*cphi**2*complex(0,1)*g1*gw*sw*vL',
                   order = {'QED':1})

GC_1127 = Coupling(name = 'GC_1127',
                   value = '-((cphi*cxi*complex(0,1)*g1*gw*sw*vL*cmath.sqrt(2))/cw)',
                   order = {'QED':1})

GC_1128 = Coupling(name = 'GC_1128',
                   value = '2*cphi*cw*complex(0,1)*gw**2*sw*vL',
                   order = {'EWL':1})

GC_1129 = Coupling(name = 'GC_1129',
                   value = '-((cxi*complex(0,1)*gw**2*sw*vL)/cmath.sqrt(2))',
                   order = {'EWL':1})

GC_1130 = Coupling(name = 'GC_1130',
                   value = '-4*cphi*complex(0,1)*g1*gw*sphi*sw*vL',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1131 = Coupling(name = 'GC_1131',
                   value = '4*cphi*complex(0,1)*g1*gw*sphi*sw*vL',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1132 = Coupling(name = 'GC_1132',
                   value = '(cxi*complex(0,1)*g1*gw*sphi*sw*vL*cmath.sqrt(2))/cw',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1133 = Coupling(name = 'GC_1133',
                   value = '2*cw*complex(0,1)*gw**2*sphi*sw*vL',
                   order = {'EWR':1})

GC_1134 = Coupling(name = 'GC_1134',
                   value = '-2*complex(0,1)*g1*gw*sphi**2*sw*vL',
                   order = {'EWL':-2,'EWR':2,'QED':1})

GC_1135 = Coupling(name = 'GC_1135',
                   value = '(2*cphi**2*complex(0,1)*g1**2*sw**2*vL)/cw**2',
                   order = {'EWL':-1,'QED':2})

GC_1136 = Coupling(name = 'GC_1136',
                   value = '(2*cphi*complex(0,1)*g1*gw*sw**2*vL)/cw',
                   order = {'QED':1})

GC_1137 = Coupling(name = 'GC_1137',
                   value = '2*complex(0,1)*gw**2*sw**2*vL',
                   order = {'EWL':1})

GC_1138 = Coupling(name = 'GC_1138',
                   value = '(-2*complex(0,1)*g1*gw*sphi*sw**2*vL)/cw',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1139 = Coupling(name = 'GC_1139',
                   value = '(2*complex(0,1)*g1**2*sphi**2*sw**2*vL)/cw**2',
                   order = {'EWL':-3,'EWR':2,'QED':2})

GC_1140 = Coupling(name = 'GC_1140',
                   value = '(-4*cphi*complex(0,1)*g1**2*sphi*sw**4*vL)/cw**2',
                   order = {'EWL':-2,'EWR':1,'QED':2})

GC_1141 = Coupling(name = 'GC_1141',
                   value = 'cxi*complex(0,1)*g1*gw*vL*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2)',
                   order = {'QED':1})

GC_1142 = Coupling(name = 'GC_1142',
                   value = '(-2*cphi*complex(0,1)*g1**2*sw*vL*cmath.sqrt(1 - 2*sw**2))/cw',
                   order = {'EWL':-1,'QED':2})

GC_1143 = Coupling(name = 'GC_1143',
                   value = '-4*complex(0,1)*g1*gw*sw*vL*cmath.sqrt(1 - 2*sw**2)',
                   order = {'QED':1})

GC_1144 = Coupling(name = 'GC_1144',
                   value = '4*cphi**2*complex(0,1)*g1*gw*sw*vL*cmath.sqrt(1 - 2*sw**2)',
                   order = {'QED':1})

GC_1145 = Coupling(name = 'GC_1145',
                   value = '-((cphi*cxi*complex(0,1)*g1*gw*sw*vL*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2))/cw)',
                   order = {'QED':1})

GC_1146 = Coupling(name = 'GC_1146',
                   value = '(2*complex(0,1)*g1**2*sphi*sw*vL*cmath.sqrt(1 - 2*sw**2))/cw',
                   order = {'EWL':-2,'EWR':1,'QED':2})

GC_1147 = Coupling(name = 'GC_1147',
                   value = '4*cphi*complex(0,1)*g1*gw*sphi*sw*vL*cmath.sqrt(1 - 2*sw**2)',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1148 = Coupling(name = 'GC_1148',
                   value = '-((cxi*complex(0,1)*g1*gw*sphi*sw*vL*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2))/cw)',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1149 = Coupling(name = 'GC_1149',
                   value = '4*complex(0,1)*g1*gw*sphi**2*sw*vL*cmath.sqrt(1 - 2*sw**2)',
                   order = {'EWL':-2,'EWR':2,'QED':1})

GC_1150 = Coupling(name = 'GC_1150',
                   value = '(2*cphi**2*complex(0,1)*g1**2*sw**2*vL*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-1,'QED':2})

GC_1151 = Coupling(name = 'GC_1151',
                   value = '(-4*cphi*complex(0,1)*g1**2*sphi*sw**2*vL*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-2,'EWR':1,'QED':2})

GC_1152 = Coupling(name = 'GC_1152',
                   value = '(4*cphi*complex(0,1)*g1**2*sphi*sw**2*vL*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-2,'EWR':1,'QED':2})

GC_1153 = Coupling(name = 'GC_1153',
                   value = '(-2*complex(0,1)*g1**2*sphi**2*sw**2*vL*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-3,'EWR':2,'QED':2})

GC_1154 = Coupling(name = 'GC_1154',
                   value = '-((cphi*cw*complex(0,1)*gw**2*sxi*vL)/cmath.sqrt(2))',
                   order = {'EWR':1})

GC_1155 = Coupling(name = 'GC_1155',
                   value = 'cxi*complex(0,1)*gw**2*sxi*vL',
                   order = {'EWR':1})

GC_1156 = Coupling(name = 'GC_1156',
                   value = '-(cxi*complex(0,1)*gw**2*sxi*vL*cmath.sqrt(2))',
                   order = {'EWR':1})

GC_1157 = Coupling(name = 'GC_1157',
                   value = '-((cw*complex(0,1)*gw**2*sphi*sxi*vL)/cmath.sqrt(2))',
                   order = {'EWL':-1,'EWR':2})

GC_1158 = Coupling(name = 'GC_1158',
                   value = '-((cphi*complex(0,1)*g1*gw*sw*sxi*vL*cmath.sqrt(2))/cw)',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1159 = Coupling(name = 'GC_1159',
                   value = '-((complex(0,1)*gw**2*sw*sxi*vL)/cmath.sqrt(2))',
                   order = {'EWR':1})

GC_1160 = Coupling(name = 'GC_1160',
                   value = '(complex(0,1)*g1*gw*sphi*sw*sxi*vL*cmath.sqrt(2))/cw',
                   order = {'EWL':-2,'EWR':2,'QED':1})

GC_1161 = Coupling(name = 'GC_1161',
                   value = 'complex(0,1)*g1*gw*sxi*vL*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2)',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1162 = Coupling(name = 'GC_1162',
                   value = '-((cphi*complex(0,1)*g1*gw*sw*sxi*vL*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2))/cw)',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1163 = Coupling(name = 'GC_1163',
                   value = '-((complex(0,1)*g1*gw*sphi*sw*sxi*vL*cmath.sqrt(2)*cmath.sqrt(1 - 2*sw**2))/cw)',
                   order = {'EWL':-2,'EWR':2,'QED':1})

GC_1164 = Coupling(name = 'GC_1164',
                   value = 'complex(0,1)*gw**2*sxi**2*vL',
                   order = {'EWL':-1,'EWR':2})

GC_1165 = Coupling(name = 'GC_1165',
                   value = '-(complex(0,1)*gw**2*sxi**2*vL*cmath.sqrt(2))',
                   order = {'EWL':-1,'EWR':2})

GC_1166 = Coupling(name = 'GC_1166',
                   value = '2*complex(0,1)*g1**2*vL - 4*complex(0,1)*g1**2*sw**2*vL',
                   order = {'EWL':-1,'QED':2})

GC_1167 = Coupling(name = 'GC_1167',
                   value = '(-2*cphi*complex(0,1)*g1**2*sw*vL)/cw + (4*cphi*complex(0,1)*g1**2*sw**3*vL)/cw',
                   order = {'EWL':-1,'QED':2})

GC_1168 = Coupling(name = 'GC_1168',
                   value = '(-2*complex(0,1)*g1**2*sphi*sw*vL)/cw + (4*complex(0,1)*g1**2*sphi*sw**3*vL)/cw',
                   order = {'EWL':-2,'EWR':1,'QED':2})

GC_1169 = Coupling(name = 'GC_1169',
                   value = '(2*cphi**2*complex(0,1)*g1**2*sw**2*vL)/cw**2 - (4*cphi**2*complex(0,1)*g1**2*sw**4*vL)/cw**2',
                   order = {'EWL':-1,'QED':2})

GC_1170 = Coupling(name = 'GC_1170',
                   value = '(2*complex(0,1)*g1**2*sphi**2*sw**2*vL)/cw**2 - (4*complex(0,1)*g1**2*sphi**2*sw**4*vL)/cw**2',
                   order = {'EWL':-3,'EWR':2,'QED':2})

GC_1171 = Coupling(name = 'GC_1171',
                   value = '-2*cphi*cw*complex(0,1)*g1*gw*vL*cmath.sqrt(1 - 2*sw**2) + (2*cphi*complex(0,1)*g1*gw*sw**2*vL*cmath.sqrt(1 - 2*sw**2))/cw',
                   order = {'QED':1})

GC_1172 = Coupling(name = 'GC_1172',
                   value = '-2*cw*complex(0,1)*g1*gw*sphi*vL*cmath.sqrt(1 - 2*sw**2) + (2*complex(0,1)*g1*gw*sphi*sw**2*vL*cmath.sqrt(1 - 2*sw**2))/cw',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1173 = Coupling(name = 'GC_1173',
                   value = '(complex(0,1)*I35a11)/(vR*cmath.sqrt(2)) + (complex(0,1)*I36a11)/(vR*cmath.sqrt(2))',
                   order = {'EWR':1})

GC_1174 = Coupling(name = 'GC_1174',
                   value = '(complex(0,1)*I35a22)/(vR*cmath.sqrt(2)) + (complex(0,1)*I36a22)/(vR*cmath.sqrt(2))',
                   order = {'EWR':1})

GC_1175 = Coupling(name = 'GC_1175',
                   value = '(complex(0,1)*I35a33)/(vR*cmath.sqrt(2)) + (complex(0,1)*I36a33)/(vR*cmath.sqrt(2))',
                   order = {'EWR':1})

GC_1176 = Coupling(name = 'GC_1176',
                   value = '-I37a11/(2.*vR) - I38a11/(2.*vR)',
                   order = {'EWR':1})

GC_1177 = Coupling(name = 'GC_1177',
                   value = '-(complex(0,1)*I37a11)/(2.*vR) - (complex(0,1)*I38a11)/(2.*vR)',
                   order = {'EWR':1})

GC_1178 = Coupling(name = 'GC_1178',
                   value = '-I37a22/(2.*vR) - I38a22/(2.*vR)',
                   order = {'EWR':1})

GC_1179 = Coupling(name = 'GC_1179',
                   value = '-(complex(0,1)*I37a22)/(2.*vR) - (complex(0,1)*I38a22)/(2.*vR)',
                   order = {'EWR':1})

GC_1180 = Coupling(name = 'GC_1180',
                   value = '-I37a33/(2.*vR) - I38a33/(2.*vR)',
                   order = {'EWR':1})

GC_1181 = Coupling(name = 'GC_1181',
                   value = '-(complex(0,1)*I37a33)/(2.*vR) - (complex(0,1)*I38a33)/(2.*vR)',
                   order = {'EWR':1})

GC_1182 = Coupling(name = 'GC_1182',
                   value = '-I37a14/(4.*vR) - I37a41/(4.*vR) - I38a14/(4.*vR) - I38a41/(4.*vR)',
                   order = {'EWR':1})

GC_1183 = Coupling(name = 'GC_1183',
                   value = '-(complex(0,1)*I37a14)/(4.*vR) - (complex(0,1)*I37a41)/(4.*vR) - (complex(0,1)*I38a14)/(4.*vR) - (complex(0,1)*I38a41)/(4.*vR)',
                   order = {'EWR':1})

GC_1184 = Coupling(name = 'GC_1184',
                   value = '-I37a44/(2.*vR) - I38a44/(2.*vR)',
                   order = {'EWR':1})

GC_1185 = Coupling(name = 'GC_1185',
                   value = '-(complex(0,1)*I37a44)/(2.*vR) - (complex(0,1)*I38a44)/(2.*vR)',
                   order = {'EWR':1})

GC_1186 = Coupling(name = 'GC_1186',
                   value = '-I37a25/(4.*vR) - I37a52/(4.*vR) - I38a25/(4.*vR) - I38a52/(4.*vR)',
                   order = {'EWR':1})

GC_1187 = Coupling(name = 'GC_1187',
                   value = '-(complex(0,1)*I37a25)/(4.*vR) - (complex(0,1)*I37a52)/(4.*vR) - (complex(0,1)*I38a25)/(4.*vR) - (complex(0,1)*I38a52)/(4.*vR)',
                   order = {'EWR':1})

GC_1188 = Coupling(name = 'GC_1188',
                   value = '-I37a55/(2.*vR) - I38a55/(2.*vR)',
                   order = {'EWR':1})

GC_1189 = Coupling(name = 'GC_1189',
                   value = '-(complex(0,1)*I37a55)/(2.*vR) - (complex(0,1)*I38a55)/(2.*vR)',
                   order = {'EWR':1})

GC_1190 = Coupling(name = 'GC_1190',
                   value = '-I37a36/(4.*vR) - I37a63/(4.*vR) - I38a36/(4.*vR) - I38a63/(4.*vR)',
                   order = {'EWR':1})

GC_1191 = Coupling(name = 'GC_1191',
                   value = '-(complex(0,1)*I37a36)/(4.*vR) - (complex(0,1)*I37a63)/(4.*vR) - (complex(0,1)*I38a36)/(4.*vR) - (complex(0,1)*I38a63)/(4.*vR)',
                   order = {'EWR':1})

GC_1192 = Coupling(name = 'GC_1192',
                   value = '-I37a66/(2.*vR) - I38a66/(2.*vR)',
                   order = {'EWR':1})

GC_1193 = Coupling(name = 'GC_1193',
                   value = '-(complex(0,1)*I37a66)/(2.*vR) - (complex(0,1)*I38a66)/(2.*vR)',
                   order = {'EWR':1})

GC_1194 = Coupling(name = 'GC_1194',
                   value = '-(complex(0,1)*I39a11)/(2.*vR) - (complex(0,1)*I40a11)/(2.*vR)',
                   order = {'EWR':1})

GC_1195 = Coupling(name = 'GC_1195',
                   value = 'I39a11/(2.*vR) + I40a11/(2.*vR)',
                   order = {'EWR':1})

GC_1196 = Coupling(name = 'GC_1196',
                   value = '-(complex(0,1)*I39a22)/(2.*vR) - (complex(0,1)*I40a22)/(2.*vR)',
                   order = {'EWR':1})

GC_1197 = Coupling(name = 'GC_1197',
                   value = 'I39a22/(2.*vR) + I40a22/(2.*vR)',
                   order = {'EWR':1})

GC_1198 = Coupling(name = 'GC_1198',
                   value = '-(complex(0,1)*I39a33)/(2.*vR) - (complex(0,1)*I40a33)/(2.*vR)',
                   order = {'EWR':1})

GC_1199 = Coupling(name = 'GC_1199',
                   value = 'I39a33/(2.*vR) + I40a33/(2.*vR)',
                   order = {'EWR':1})

GC_1200 = Coupling(name = 'GC_1200',
                   value = '-(complex(0,1)*I39a14)/(4.*vR) - (complex(0,1)*I39a41)/(4.*vR) - (complex(0,1)*I40a14)/(4.*vR) - (complex(0,1)*I40a41)/(4.*vR)',
                   order = {'EWR':1})

GC_1201 = Coupling(name = 'GC_1201',
                   value = 'I39a14/(4.*vR) + I39a41/(4.*vR) + I40a14/(4.*vR) + I40a41/(4.*vR)',
                   order = {'EWR':1})

GC_1202 = Coupling(name = 'GC_1202',
                   value = '-(complex(0,1)*I39a44)/(2.*vR) - (complex(0,1)*I40a44)/(2.*vR)',
                   order = {'EWR':1})

GC_1203 = Coupling(name = 'GC_1203',
                   value = 'I39a44/(2.*vR) + I40a44/(2.*vR)',
                   order = {'EWR':1})

GC_1204 = Coupling(name = 'GC_1204',
                   value = '-(complex(0,1)*I39a25)/(4.*vR) - (complex(0,1)*I39a52)/(4.*vR) - (complex(0,1)*I40a25)/(4.*vR) - (complex(0,1)*I40a52)/(4.*vR)',
                   order = {'EWR':1})

GC_1205 = Coupling(name = 'GC_1205',
                   value = 'I39a25/(4.*vR) + I39a52/(4.*vR) + I40a25/(4.*vR) + I40a52/(4.*vR)',
                   order = {'EWR':1})

GC_1206 = Coupling(name = 'GC_1206',
                   value = '-(complex(0,1)*I39a55)/(2.*vR) - (complex(0,1)*I40a55)/(2.*vR)',
                   order = {'EWR':1})

GC_1207 = Coupling(name = 'GC_1207',
                   value = 'I39a55/(2.*vR) + I40a55/(2.*vR)',
                   order = {'EWR':1})

GC_1208 = Coupling(name = 'GC_1208',
                   value = '-(complex(0,1)*I39a36)/(4.*vR) - (complex(0,1)*I39a63)/(4.*vR) - (complex(0,1)*I40a36)/(4.*vR) - (complex(0,1)*I40a63)/(4.*vR)',
                   order = {'EWR':1})

GC_1209 = Coupling(name = 'GC_1209',
                   value = 'I39a36/(4.*vR) + I39a63/(4.*vR) + I40a36/(4.*vR) + I40a63/(4.*vR)',
                   order = {'EWR':1})

GC_1210 = Coupling(name = 'GC_1210',
                   value = '-(complex(0,1)*I39a66)/(2.*vR) - (complex(0,1)*I40a66)/(2.*vR)',
                   order = {'EWR':1})

GC_1211 = Coupling(name = 'GC_1211',
                   value = 'I39a66/(2.*vR) + I40a66/(2.*vR)',
                   order = {'EWR':1})

GC_1212 = Coupling(name = 'GC_1212',
                   value = '(complex(0,1)*I41a11)/(2.*vR) + (complex(0,1)*I42a11)/(2.*vR)',
                   order = {'EWR':1})

GC_1213 = Coupling(name = 'GC_1213',
                   value = '(complex(0,1)*I41a22)/(2.*vR) + (complex(0,1)*I42a22)/(2.*vR)',
                   order = {'EWR':1})

GC_1214 = Coupling(name = 'GC_1214',
                   value = '(complex(0,1)*I41a33)/(2.*vR) + (complex(0,1)*I42a33)/(2.*vR)',
                   order = {'EWR':1})

GC_1215 = Coupling(name = 'GC_1215',
                   value = '(complex(0,1)*I41a41)/(2.*vR) + (complex(0,1)*I42a41)/(2.*vR)',
                   order = {'EWR':1})

GC_1216 = Coupling(name = 'GC_1216',
                   value = '(complex(0,1)*I41a52)/(2.*vR) + (complex(0,1)*I42a52)/(2.*vR)',
                   order = {'EWR':1})

GC_1217 = Coupling(name = 'GC_1217',
                   value = '(complex(0,1)*I41a63)/(2.*vR) + (complex(0,1)*I42a63)/(2.*vR)',
                   order = {'EWR':1})

GC_1218 = Coupling(name = 'GC_1218',
                   value = '(complex(0,1)*I43a11)/(vR*cmath.sqrt(2)) + (complex(0,1)*I44a11)/(vR*cmath.sqrt(2))',
                   order = {'EWR':1})

GC_1219 = Coupling(name = 'GC_1219',
                   value = '(complex(0,1)*I43a22)/(vR*cmath.sqrt(2)) + (complex(0,1)*I44a22)/(vR*cmath.sqrt(2))',
                   order = {'EWR':1})

GC_1220 = Coupling(name = 'GC_1220',
                   value = '(complex(0,1)*I43a33)/(vR*cmath.sqrt(2)) + (complex(0,1)*I44a33)/(vR*cmath.sqrt(2))',
                   order = {'EWR':1})

GC_1221 = Coupling(name = 'GC_1221',
                   value = '(complex(0,1)*I45a11)/(vR*cmath.sqrt(2)) + (complex(0,1)*I46a11)/(vR*cmath.sqrt(2))',
                   order = {'EWR':1})

GC_1222 = Coupling(name = 'GC_1222',
                   value = '(complex(0,1)*I45a22)/(vR*cmath.sqrt(2)) + (complex(0,1)*I46a22)/(vR*cmath.sqrt(2))',
                   order = {'EWR':1})

GC_1223 = Coupling(name = 'GC_1223',
                   value = '(complex(0,1)*I45a33)/(vR*cmath.sqrt(2)) + (complex(0,1)*I46a33)/(vR*cmath.sqrt(2))',
                   order = {'EWR':1})

GC_1224 = Coupling(name = 'GC_1224',
                   value = '(complex(0,1)*I47a11)/(2.*vR) + (complex(0,1)*I48a11)/(2.*vR)',
                   order = {'EWR':1})

GC_1225 = Coupling(name = 'GC_1225',
                   value = '(complex(0,1)*I47a22)/(2.*vR) + (complex(0,1)*I48a22)/(2.*vR)',
                   order = {'EWR':1})

GC_1226 = Coupling(name = 'GC_1226',
                   value = '(complex(0,1)*I47a33)/(2.*vR) + (complex(0,1)*I48a33)/(2.*vR)',
                   order = {'EWR':1})

GC_1227 = Coupling(name = 'GC_1227',
                   value = '(complex(0,1)*I47a41)/(2.*vR) + (complex(0,1)*I48a41)/(2.*vR)',
                   order = {'EWR':1})

GC_1228 = Coupling(name = 'GC_1228',
                   value = '(complex(0,1)*I47a52)/(2.*vR) + (complex(0,1)*I48a52)/(2.*vR)',
                   order = {'EWR':1})

GC_1229 = Coupling(name = 'GC_1229',
                   value = '(complex(0,1)*I47a63)/(2.*vR) + (complex(0,1)*I48a63)/(2.*vR)',
                   order = {'EWR':1})

GC_1230 = Coupling(name = 'GC_1230',
                   value = '(complex(0,1)*I49a11)/(vR*cmath.sqrt(2)) + (complex(0,1)*I50a11)/(vR*cmath.sqrt(2))',
                   order = {'EWR':1})

GC_1231 = Coupling(name = 'GC_1231',
                   value = '(complex(0,1)*I49a22)/(vR*cmath.sqrt(2)) + (complex(0,1)*I50a22)/(vR*cmath.sqrt(2))',
                   order = {'EWR':1})

GC_1232 = Coupling(name = 'GC_1232',
                   value = '(complex(0,1)*I49a33)/(vR*cmath.sqrt(2)) + (complex(0,1)*I50a33)/(vR*cmath.sqrt(2))',
                   order = {'EWR':1})

GC_1233 = Coupling(name = 'GC_1233',
                   value = '(cth1*complex(0,1)*I14a11*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I16a11*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I14a11*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I16a11*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1234 = Coupling(name = 'GC_1234',
                   value = '(cth1*complex(0,1)*I14a22*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I16a22*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I14a22*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I16a22*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1235 = Coupling(name = 'GC_1235',
                   value = '(cth1*complex(0,1)*I14a33*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I16a33*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I14a33*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I16a33*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1236 = Coupling(name = 'GC_1236',
                   value = '(cth1*complex(0,1)*I14a14*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I14a41*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I16a14*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I16a41*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I14a14*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I14a41*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I16a14*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I16a41*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1237 = Coupling(name = 'GC_1237',
                   value = '(cth1*complex(0,1)*I14a44*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I16a44*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I14a44*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I16a44*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1238 = Coupling(name = 'GC_1238',
                   value = '(cth1*complex(0,1)*I14a25*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I14a52*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I16a25*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I16a52*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I14a25*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I14a52*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I16a25*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I16a52*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1239 = Coupling(name = 'GC_1239',
                   value = '(cth1*complex(0,1)*I14a55*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I16a55*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I14a55*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I16a55*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1240 = Coupling(name = 'GC_1240',
                   value = '(cth1*complex(0,1)*I14a36*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I14a63*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I16a36*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I16a63*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I14a36*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I14a63*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I16a36*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I16a63*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1241 = Coupling(name = 'GC_1241',
                   value = '(cth1*complex(0,1)*I14a66*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I16a66*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I14a66*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I16a66*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1242 = Coupling(name = 'GC_1242',
                   value = '(cth1*complex(0,1)*I17a11*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I20a11*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I17a11*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I20a11*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1243 = Coupling(name = 'GC_1243',
                   value = '(cth1*complex(0,1)*I17a22*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I20a22*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I17a22*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I20a22*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1244 = Coupling(name = 'GC_1244',
                   value = '(cth1*complex(0,1)*I17a33*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I20a33*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I17a33*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I20a33*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1245 = Coupling(name = 'GC_1245',
                   value = '(cth1*complex(0,1)*I17a14*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I17a41*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I20a14*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I20a41*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I17a14*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I17a41*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I20a14*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I20a41*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1246 = Coupling(name = 'GC_1246',
                   value = '(cth1*complex(0,1)*I17a44*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I20a44*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I17a44*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I20a44*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1247 = Coupling(name = 'GC_1247',
                   value = '(cth1*complex(0,1)*I17a25*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I17a52*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I20a25*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I20a52*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I17a25*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I17a52*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I20a25*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I20a52*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1248 = Coupling(name = 'GC_1248',
                   value = '(cth1*complex(0,1)*I17a55*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I20a55*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I17a55*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I20a55*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1249 = Coupling(name = 'GC_1249',
                   value = '(cth1*complex(0,1)*I17a36*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I17a63*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I20a36*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I20a63*k1**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I17a36*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I17a63*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I20a36*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I20a63*k2**2)/(4.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1250 = Coupling(name = 'GC_1250',
                   value = '(cth1*complex(0,1)*I17a66*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (cth1*complex(0,1)*I20a66*k1**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I17a66*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (cth1*complex(0,1)*I20a66*k2**2)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1251 = Coupling(name = 'GC_1251',
                   value = '(complex(0,1)*I14a11*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I16a11*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I14a11*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I16a11*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1252 = Coupling(name = 'GC_1252',
                   value = '(complex(0,1)*I14a22*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I16a22*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I14a22*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I16a22*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1253 = Coupling(name = 'GC_1253',
                   value = '(complex(0,1)*I14a33*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I16a33*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I14a33*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I16a33*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1254 = Coupling(name = 'GC_1254',
                   value = '(complex(0,1)*I14a14*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I14a41*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I16a14*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I16a41*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I14a14*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I14a41*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I16a14*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I16a41*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1255 = Coupling(name = 'GC_1255',
                   value = '(complex(0,1)*I14a44*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I16a44*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I14a44*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I16a44*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1256 = Coupling(name = 'GC_1256',
                   value = '(complex(0,1)*I14a25*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I14a52*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I16a25*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I16a52*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I14a25*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I14a52*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I16a25*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I16a52*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1257 = Coupling(name = 'GC_1257',
                   value = '(complex(0,1)*I14a55*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I16a55*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I14a55*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I16a55*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1258 = Coupling(name = 'GC_1258',
                   value = '(complex(0,1)*I14a36*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I14a63*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I16a36*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I16a63*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I14a36*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I14a63*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I16a36*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I16a63*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1259 = Coupling(name = 'GC_1259',
                   value = '(complex(0,1)*I14a66*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I16a66*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I14a66*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I16a66*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1260 = Coupling(name = 'GC_1260',
                   value = '(complex(0,1)*I17a11*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I20a11*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I17a11*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I20a11*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1261 = Coupling(name = 'GC_1261',
                   value = '(complex(0,1)*I17a22*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I20a22*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I17a22*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I20a22*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1262 = Coupling(name = 'GC_1262',
                   value = '(complex(0,1)*I17a33*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I20a33*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I17a33*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I20a33*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1263 = Coupling(name = 'GC_1263',
                   value = '(complex(0,1)*I17a14*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I17a41*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I20a14*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I20a41*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I17a14*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I17a41*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I20a14*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I20a41*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1264 = Coupling(name = 'GC_1264',
                   value = '(complex(0,1)*I17a44*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I20a44*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I17a44*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I20a44*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1265 = Coupling(name = 'GC_1265',
                   value = '(complex(0,1)*I17a25*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I17a52*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I20a25*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I20a52*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I17a25*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I17a52*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I20a25*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I20a52*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1266 = Coupling(name = 'GC_1266',
                   value = '(complex(0,1)*I17a55*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I20a55*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I17a55*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I20a55*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1267 = Coupling(name = 'GC_1267',
                   value = '(complex(0,1)*I17a36*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I17a63*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I20a36*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I20a63*k1**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I17a36*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I17a63*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I20a36*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I20a63*k2**2*sth1)/(4.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1268 = Coupling(name = 'GC_1268',
                   value = '(complex(0,1)*I17a66*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) + (complex(0,1)*I20a66*k1**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I17a66*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR) - (complex(0,1)*I20a66*k2**2*sth1)/(2.*(-k1 + k2)*(k1 + k2)*vR)',
                   order = {'EWR':1})

GC_1269 = Coupling(name = 'GC_1269',
                   value = '-(cxi**2*complex(0,1)*gwR**2*vR*cmath.sqrt(2))',
                   order = {'EWR':1})

GC_1270 = Coupling(name = 'GC_1270',
                   value = 'cth1*cxi**2*complex(0,1)*gwR**2*vR',
                   order = {'EWR':1})

GC_1271 = Coupling(name = 'GC_1271',
                   value = 'cxi**2*complex(0,1)*gwR**2*sth1*vR',
                   order = {'EWR':1})

GC_1272 = Coupling(name = 'GC_1272',
                   value = '(2*cphi**2*cth1*complex(0,1)*g1**2*sw**2*vR)/cw**2',
                   order = {'EWR':-1,'QED':2})

GC_1273 = Coupling(name = 'GC_1273',
                   value = '(2*cth1*complex(0,1)*g1**2*sphi**2*sw**2*vR)/cw**2',
                   order = {'EWL':-2,'EWR':1,'QED':2})

GC_1274 = Coupling(name = 'GC_1274',
                   value = '(2*cphi**2*complex(0,1)*g1**2*sth1*sw**2*vR)/cw**2',
                   order = {'EWR':-1,'QED':2})

GC_1275 = Coupling(name = 'GC_1275',
                   value = '(2*complex(0,1)*g1**2*sphi**2*sth1*sw**2*vR)/cw**2',
                   order = {'EWL':-2,'EWR':1,'QED':2})

GC_1276 = Coupling(name = 'GC_1276',
                   value = '(-4*cphi*cth1*complex(0,1)*g1**2*sphi*sw**4*vR)/cw**2',
                   order = {'EWL':-1,'QED':2})

GC_1277 = Coupling(name = 'GC_1277',
                   value = '(-4*cphi*complex(0,1)*g1**2*sphi*sth1*sw**4*vR)/cw**2',
                   order = {'EWL':-1,'QED':2})

GC_1278 = Coupling(name = 'GC_1278',
                   value = '(-2*cphi*cth1*complex(0,1)*g1**2*sw*vR*cmath.sqrt(1 - 2*sw**2))/cw',
                   order = {'EWR':-1,'QED':2})

GC_1279 = Coupling(name = 'GC_1279',
                   value = '-4*cth1*complex(0,1)*g1*gwR*sw*vR*cmath.sqrt(1 - 2*sw**2)',
                   order = {'QED':1})

GC_1280 = Coupling(name = 'GC_1280',
                   value = '(4*cphi**2*cth1*complex(0,1)*g1*gwR*sw*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'QED':1})

GC_1281 = Coupling(name = 'GC_1281',
                   value = '(2*cth1*complex(0,1)*g1**2*sphi*sw*vR*cmath.sqrt(1 - 2*sw**2))/cw',
                   order = {'EWL':-1,'QED':2})

GC_1282 = Coupling(name = 'GC_1282',
                   value = '(4*cth1*complex(0,1)*g1*gwR*sphi**2*sw*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-2,'EWR':2,'QED':1})

GC_1283 = Coupling(name = 'GC_1283',
                   value = '(-2*cphi*complex(0,1)*g1**2*sth1*sw*vR*cmath.sqrt(1 - 2*sw**2))/cw',
                   order = {'EWR':-1,'QED':2})

GC_1284 = Coupling(name = 'GC_1284',
                   value = '-4*complex(0,1)*g1*gwR*sth1*sw*vR*cmath.sqrt(1 - 2*sw**2)',
                   order = {'QED':1})

GC_1285 = Coupling(name = 'GC_1285',
                   value = '(4*cphi**2*complex(0,1)*g1*gwR*sth1*sw*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'QED':1})

GC_1286 = Coupling(name = 'GC_1286',
                   value = '(2*complex(0,1)*g1**2*sphi*sth1*sw*vR*cmath.sqrt(1 - 2*sw**2))/cw',
                   order = {'EWL':-1,'QED':2})

GC_1287 = Coupling(name = 'GC_1287',
                   value = '(4*complex(0,1)*g1*gwR*sphi**2*sth1*sw*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-2,'EWR':2,'QED':1})

GC_1288 = Coupling(name = 'GC_1288',
                   value = '(2*cphi**2*cth1*complex(0,1)*g1**2*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWR':-1,'QED':2})

GC_1289 = Coupling(name = 'GC_1289',
                   value = '(4*cphi*cth1*complex(0,1)*g1*gwR*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw',
                   order = {'QED':1})

GC_1290 = Coupling(name = 'GC_1290',
                   value = '(-4*cphi*cth1*complex(0,1)*g1**2*sphi*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-1,'QED':2})

GC_1291 = Coupling(name = 'GC_1291',
                   value = '(4*cphi*cth1*complex(0,1)*g1**2*sphi*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-1,'QED':2})

GC_1292 = Coupling(name = 'GC_1292',
                   value = '(4*cth1*complex(0,1)*g1*gwR*sphi*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1293 = Coupling(name = 'GC_1293',
                   value = '(-2*cth1*complex(0,1)*g1**2*sphi**2*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-2,'EWR':1,'QED':2})

GC_1294 = Coupling(name = 'GC_1294',
                   value = '(2*cphi**2*complex(0,1)*g1**2*sth1*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWR':-1,'QED':2})

GC_1295 = Coupling(name = 'GC_1295',
                   value = '(4*cphi*complex(0,1)*g1*gwR*sth1*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw',
                   order = {'QED':1})

GC_1296 = Coupling(name = 'GC_1296',
                   value = '(-4*cphi*complex(0,1)*g1**2*sphi*sth1*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-1,'QED':2})

GC_1297 = Coupling(name = 'GC_1297',
                   value = '(4*cphi*complex(0,1)*g1**2*sphi*sth1*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-1,'QED':2})

GC_1298 = Coupling(name = 'GC_1298',
                   value = '(4*complex(0,1)*g1*gwR*sphi*sth1*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1299 = Coupling(name = 'GC_1299',
                   value = '(-2*complex(0,1)*g1**2*sphi**2*sth1*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-2,'EWR':1,'QED':2})

GC_1300 = Coupling(name = 'GC_1300',
                   value = '(-4*cphi**2*cth1*complex(0,1)*g1*gwR*sw**3*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'QED':1})

GC_1301 = Coupling(name = 'GC_1301',
                   value = '(-4*cth1*complex(0,1)*g1*gwR*sphi**2*sw**3*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-2,'EWR':2,'QED':1})

GC_1302 = Coupling(name = 'GC_1302',
                   value = '(-4*cphi**2*complex(0,1)*g1*gwR*sth1*sw**3*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'QED':1})

GC_1303 = Coupling(name = 'GC_1303',
                   value = '(-4*complex(0,1)*g1*gwR*sphi**2*sth1*sw**3*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-2,'EWR':2,'QED':1})

GC_1304 = Coupling(name = 'GC_1304',
                   value = 'cxi*complex(0,1)*gwR**2*sxi*vR*cmath.sqrt(2)',
                   order = {'EWL':-1,'EWR':2})

GC_1305 = Coupling(name = 'GC_1305',
                   value = '-(cth1*cxi*complex(0,1)*gwR**2*sxi*vR)',
                   order = {'EWL':-1,'EWR':2})

GC_1306 = Coupling(name = 'GC_1306',
                   value = '-(cxi*complex(0,1)*gwR**2*sth1*sxi*vR)',
                   order = {'EWL':-1,'EWR':2})

GC_1307 = Coupling(name = 'GC_1307',
                   value = '-(complex(0,1)*gwR**2*sxi**2*vR*cmath.sqrt(2))',
                   order = {'EWL':-2,'EWR':3})

GC_1308 = Coupling(name = 'GC_1308',
                   value = 'cth1*complex(0,1)*gwR**2*sxi**2*vR',
                   order = {'EWL':-2,'EWR':3})

GC_1309 = Coupling(name = 'GC_1309',
                   value = 'complex(0,1)*gwR**2*sth1*sxi**2*vR',
                   order = {'EWL':-2,'EWR':3})

GC_1310 = Coupling(name = 'GC_1310',
                   value = '2*cth1*complex(0,1)*g1**2*vR - 4*cth1*complex(0,1)*g1**2*sw**2*vR',
                   order = {'EWR':-1,'QED':2})

GC_1311 = Coupling(name = 'GC_1311',
                   value = '(-2*cphi*cth1*complex(0,1)*g1*gwR*vR)/cw + (6*cphi*cth1*complex(0,1)*g1*gwR*sw**2*vR)/cw',
                   order = {'QED':1})

GC_1312 = Coupling(name = 'GC_1312',
                   value = '(complex(0,1)*gw*gwR*k1**2*sth1*sw**2)/vev + (complex(0,1)*gw*gwR*k2**2*sth1*sw**2)/vev + 2*cth1*complex(0,1)*gwR**2*sw**2*vR',
                   order = {'EWR':1})

GC_1313 = Coupling(name = 'GC_1313',
                   value = '(2*cphi**2*cth1*complex(0,1)*gwR**2*vR)/cw**2 - (4*cphi**2*cth1*complex(0,1)*gwR**2*sw**2*vR)/cw**2',
                   order = {'EWR':1})

GC_1314 = Coupling(name = 'GC_1314',
                   value = '(2*cth1*complex(0,1)*g1*gwR*sphi*vR)/cw - (6*cth1*complex(0,1)*g1*gwR*sphi*sw**2*vR)/cw',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1315 = Coupling(name = 'GC_1315',
                   value = '(2*cth1*complex(0,1)*gwR**2*sphi**2*vR)/cw**2 - (4*cth1*complex(0,1)*gwR**2*sphi**2*sw**2*vR)/cw**2 - (cphi*complex(0,1)*gwR**2*k1**2*sphi*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev) - (cphi*complex(0,1)*gwR**2*k2**2*sphi*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev)',
                   order = {'EWL':-2,'EWR':3})

GC_1316 = Coupling(name = 'GC_1316',
                   value = '2*complex(0,1)*g1**2*sth1*vR - 4*complex(0,1)*g1**2*sth1*sw**2*vR',
                   order = {'EWR':-1,'QED':2})

GC_1317 = Coupling(name = 'GC_1317',
                   value = '(-2*cphi*complex(0,1)*g1*gwR*sth1*vR)/cw + (6*cphi*complex(0,1)*g1*gwR*sth1*sw**2*vR)/cw',
                   order = {'QED':1})

GC_1318 = Coupling(name = 'GC_1318',
                   value = '-((cth1*complex(0,1)*gw*gwR*k1**2*sw**2)/vev) - (cth1*complex(0,1)*gw*gwR*k2**2*sw**2)/vev + 2*complex(0,1)*gwR**2*sth1*sw**2*vR',
                   order = {'EWR':1})

GC_1319 = Coupling(name = 'GC_1319',
                   value = '(2*cphi**2*complex(0,1)*gwR**2*sth1*vR)/cw**2 - (4*cphi**2*complex(0,1)*gwR**2*sth1*sw**2*vR)/cw**2',
                   order = {'EWR':1})

GC_1320 = Coupling(name = 'GC_1320',
                   value = '(2*complex(0,1)*g1*gwR*sphi*sth1*vR)/cw - (6*complex(0,1)*g1*gwR*sphi*sth1*sw**2*vR)/cw',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1321 = Coupling(name = 'GC_1321',
                   value = '(2*complex(0,1)*gwR**2*sphi**2*sth1*vR)/cw**2 - (4*complex(0,1)*gwR**2*sphi**2*sth1*sw**2*vR)/cw**2 + (cphi*cth1*complex(0,1)*gwR**2*k1**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev) + (cphi*cth1*complex(0,1)*gwR**2*k2**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev)',
                   order = {'EWL':-2,'EWR':3})

GC_1322 = Coupling(name = 'GC_1322',
                   value = '(-2*cphi*cth1*complex(0,1)*g1**2*sw*vR)/cw + (4*cphi*cth1*complex(0,1)*g1**2*sw**3*vR)/cw',
                   order = {'EWR':-1,'QED':2})

GC_1323 = Coupling(name = 'GC_1323',
                   value = '(2*cphi**2*cth1*complex(0,1)*g1*gwR*sw*vR)/cw**2 - (6*cphi**2*cth1*complex(0,1)*g1*gwR*sw**3*vR)/cw**2',
                   order = {'QED':1})

GC_1324 = Coupling(name = 'GC_1324',
                   value = '(cphi*cw*complex(0,1)*gw*gwR*k1**2*sth1*sw)/(2.*vev) + (cphi*cw*complex(0,1)*gw*gwR*k2**2*sth1*sw)/(2.*vev) - (cphi*complex(0,1)*gw*gwR*k1**2*sth1*sw**3)/(2.*cw*vev) - (cphi*complex(0,1)*gw*gwR*k2**2*sth1*sw**3)/(2.*cw*vev) - (2*cphi*cth1*complex(0,1)*gwR**2*sw**3*vR)/cw',
                   order = {'EWR':1})

GC_1325 = Coupling(name = 'GC_1325',
                   value = '(-2*cth1*complex(0,1)*g1**2*sphi*sw*vR)/cw + (4*cth1*complex(0,1)*g1**2*sphi*sw**3*vR)/cw',
                   order = {'EWL':-1,'QED':2})

GC_1326 = Coupling(name = 'GC_1326',
                   value = '(4*cphi*cth1*complex(0,1)*g1*gwR*sphi*sw*vR)/cw**2 - (12*cphi*cth1*complex(0,1)*g1*gwR*sphi*sw**3*vR)/cw**2',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1327 = Coupling(name = 'GC_1327',
                   value = '(-4*cphi*cth1*complex(0,1)*g1*gwR*sphi*sw*vR)/cw**2 + (12*cphi*cth1*complex(0,1)*g1*gwR*sphi*sw**3*vR)/cw**2',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1328 = Coupling(name = 'GC_1328',
                   value = '(cw*complex(0,1)*gw*gwR*k1**2*sphi*sth1*sw)/(2.*vev) + (cw*complex(0,1)*gw*gwR*k2**2*sphi*sth1*sw)/(2.*vev) - (complex(0,1)*gw*gwR*k1**2*sphi*sth1*sw**3)/(2.*cw*vev) - (complex(0,1)*gw*gwR*k2**2*sphi*sth1*sw**3)/(2.*cw*vev) - (2*cth1*complex(0,1)*gwR**2*sphi*sw**3*vR)/cw - (cphi*complex(0,1)*gwR**2*k1**2*sth1*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev) - (cphi*complex(0,1)*gwR**2*k2**2*sth1*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev)',
                   order = {'EWL':-1,'EWR':2})

GC_1329 = Coupling(name = 'GC_1329',
                   value = '(-2*cth1*complex(0,1)*g1*gwR*sphi**2*sw*vR)/cw**2 + (6*cth1*complex(0,1)*g1*gwR*sphi**2*sw**3*vR)/cw**2',
                   order = {'EWL':-2,'EWR':2,'QED':1})

GC_1330 = Coupling(name = 'GC_1330',
                   value = '(-2*cphi*complex(0,1)*g1**2*sth1*sw*vR)/cw + (4*cphi*complex(0,1)*g1**2*sth1*sw**3*vR)/cw',
                   order = {'EWR':-1,'QED':2})

GC_1331 = Coupling(name = 'GC_1331',
                   value = '(2*cphi**2*complex(0,1)*g1*gwR*sth1*sw*vR)/cw**2 - (6*cphi**2*complex(0,1)*g1*gwR*sth1*sw**3*vR)/cw**2',
                   order = {'QED':1})

GC_1332 = Coupling(name = 'GC_1332',
                   value = '-(cphi*cth1*cw*complex(0,1)*gw*gwR*k1**2*sw)/(2.*vev) - (cphi*cth1*cw*complex(0,1)*gw*gwR*k2**2*sw)/(2.*vev) + (cphi*cth1*complex(0,1)*gw*gwR*k1**2*sw**3)/(2.*cw*vev) + (cphi*cth1*complex(0,1)*gw*gwR*k2**2*sw**3)/(2.*cw*vev) - (2*cphi*complex(0,1)*gwR**2*sth1*sw**3*vR)/cw',
                   order = {'EWR':1})

GC_1333 = Coupling(name = 'GC_1333',
                   value = '(-2*complex(0,1)*g1**2*sphi*sth1*sw*vR)/cw + (4*complex(0,1)*g1**2*sphi*sth1*sw**3*vR)/cw',
                   order = {'EWL':-1,'QED':2})

GC_1334 = Coupling(name = 'GC_1334',
                   value = '(4*cphi*complex(0,1)*g1*gwR*sphi*sth1*sw*vR)/cw**2 - (12*cphi*complex(0,1)*g1*gwR*sphi*sth1*sw**3*vR)/cw**2',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1335 = Coupling(name = 'GC_1335',
                   value = '(-4*cphi*complex(0,1)*g1*gwR*sphi*sth1*sw*vR)/cw**2 + (12*cphi*complex(0,1)*g1*gwR*sphi*sth1*sw**3*vR)/cw**2',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1336 = Coupling(name = 'GC_1336',
                   value = '-(cth1*cw*complex(0,1)*gw*gwR*k1**2*sphi*sw)/(2.*vev) - (cth1*cw*complex(0,1)*gw*gwR*k2**2*sphi*sw)/(2.*vev) + (cth1*complex(0,1)*gw*gwR*k1**2*sphi*sw**3)/(2.*cw*vev) + (cth1*complex(0,1)*gw*gwR*k2**2*sphi*sw**3)/(2.*cw*vev) - (2*complex(0,1)*gwR**2*sphi*sth1*sw**3*vR)/cw + (cphi*cth1*complex(0,1)*gwR**2*k1**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev) + (cphi*cth1*complex(0,1)*gwR**2*k2**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev)',
                   order = {'EWL':-1,'EWR':2})

GC_1337 = Coupling(name = 'GC_1337',
                   value = '(-2*complex(0,1)*g1*gwR*sphi**2*sth1*sw*vR)/cw**2 + (6*complex(0,1)*g1*gwR*sphi**2*sth1*sw**3*vR)/cw**2',
                   order = {'EWL':-2,'EWR':2,'QED':1})

GC_1338 = Coupling(name = 'GC_1338',
                   value = '(2*cphi**2*cth1*complex(0,1)*g1**2*sw**2*vR)/cw**2 - (4*cphi**2*cth1*complex(0,1)*g1**2*sw**4*vR)/cw**2',
                   order = {'EWR':-1,'QED':2})

GC_1339 = Coupling(name = 'GC_1339',
                   value = '-((cphi**2*complex(0,1)*gw*gwR*k1**2*sth1*sw**2)/vev) - (cphi**2*complex(0,1)*gw*gwR*k2**2*sth1*sw**2)/vev + (2*cphi**2*cth1*complex(0,1)*gwR**2*sw**4*vR)/cw**2',
                   order = {'EWR':1})

GC_1340 = Coupling(name = 'GC_1340',
                   value = '-((cphi*complex(0,1)*gw*gwR*k1**2*sphi*sth1*sw**2)/vev) - (cphi*complex(0,1)*gw*gwR*k2**2*sphi*sth1*sw**2)/vev - (2*cphi*cth1*complex(0,1)*gwR**2*sphi*vR)/cw**2 + (4*cphi*cth1*complex(0,1)*gwR**2*sphi*sw**2*vR)/cw**2 + (2*cphi*cth1*complex(0,1)*gwR**2*sphi*sw**4*vR)/cw**2 + (cphi**2*complex(0,1)*gwR**2*k1**2*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev) + (cphi**2*complex(0,1)*gwR**2*k2**2*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev)',
                   order = {'EWL':-1,'EWR':2})

GC_1341 = Coupling(name = 'GC_1341',
                   value = '(2*cth1*complex(0,1)*g1**2*sphi**2*sw**2*vR)/cw**2 - (4*cth1*complex(0,1)*g1**2*sphi**2*sw**4*vR)/cw**2',
                   order = {'EWL':-2,'EWR':1,'QED':2})

GC_1342 = Coupling(name = 'GC_1342',
                   value = '-((complex(0,1)*gw*gwR*k1**2*sphi**2*sth1*sw**2)/vev) - (complex(0,1)*gw*gwR*k2**2*sphi**2*sth1*sw**2)/vev + (2*cth1*complex(0,1)*gwR**2*sphi**2*sw**4*vR)/cw**2 + (cphi*complex(0,1)*gwR**2*k1**2*sphi*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev) + (cphi*complex(0,1)*gwR**2*k2**2*sphi*sth1*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev)',
                   order = {'EWL':-2,'EWR':3})

GC_1343 = Coupling(name = 'GC_1343',
                   value = '(2*cphi**2*complex(0,1)*g1**2*sth1*sw**2*vR)/cw**2 - (4*cphi**2*complex(0,1)*g1**2*sth1*sw**4*vR)/cw**2',
                   order = {'EWR':-1,'QED':2})

GC_1344 = Coupling(name = 'GC_1344',
                   value = '(cphi**2*cth1*complex(0,1)*gw*gwR*k1**2*sw**2)/vev + (cphi**2*cth1*complex(0,1)*gw*gwR*k2**2*sw**2)/vev + (2*cphi**2*complex(0,1)*gwR**2*sth1*sw**4*vR)/cw**2',
                   order = {'EWR':1})

GC_1345 = Coupling(name = 'GC_1345',
                   value = '(cphi*cth1*complex(0,1)*gw*gwR*k1**2*sphi*sw**2)/vev + (cphi*cth1*complex(0,1)*gw*gwR*k2**2*sphi*sw**2)/vev - (2*cphi*complex(0,1)*gwR**2*sphi*sth1*vR)/cw**2 + (4*cphi*complex(0,1)*gwR**2*sphi*sth1*sw**2*vR)/cw**2 + (2*cphi*complex(0,1)*gwR**2*sphi*sth1*sw**4*vR)/cw**2 - (cphi**2*cth1*complex(0,1)*gwR**2*k1**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev) - (cphi**2*cth1*complex(0,1)*gwR**2*k2**2*sw**2*cmath.sqrt(1 - 2*sw**2))/(2.*cw**2*vev)',
                   order = {'EWL':-1,'EWR':2})

GC_1346 = Coupling(name = 'GC_1346',
                   value = '(2*complex(0,1)*g1**2*sphi**2*sth1*sw**2*vR)/cw**2 - (4*complex(0,1)*g1**2*sphi**2*sth1*sw**4*vR)/cw**2',
                   order = {'EWL':-2,'EWR':1,'QED':2})

GC_1347 = Coupling(name = 'GC_1347',
                   value = '(cth1*complex(0,1)*gw*gwR*k1**2*sphi**2*sw**2)/vev + (cth1*complex(0,1)*gw*gwR*k2**2*sphi**2*sw**2)/vev + (2*complex(0,1)*gwR**2*sphi**2*sth1*sw**4*vR)/cw**2 - (cphi*cth1*complex(0,1)*gwR**2*k1**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev) - (cphi*cth1*complex(0,1)*gwR**2*k2**2*sphi*sw**2*cmath.sqrt(1 - 2*sw**2))/(cw**2*vev)',
                   order = {'EWL':-2,'EWR':3})

GC_1348 = Coupling(name = 'GC_1348',
                   value = '-(cw*complex(0,1)*gw**2*k1**2*sphi*sth1*sw)/(2.*vev) - (cw*complex(0,1)*gw**2*k2**2*sphi*sth1*sw)/(2.*vev) + (cphi*complex(0,1)*gw*gwR*k1**2*sth1*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev) + (cphi*complex(0,1)*gw*gwR*k2**2*sth1*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev) + (2*cphi*cth1*complex(0,1)*gwR**2*sw*vR*cmath.sqrt(1 - 2*sw**2))/cw',
                   order = {'EWR':1})

GC_1349 = Coupling(name = 'GC_1349',
                   value = '(cphi*complex(0,1)*gwR**2*k1**2*sth1*sw**3)/(2.*cw*vev) + (cphi*complex(0,1)*gwR**2*k2**2*sth1*sw**3)/(2.*cw*vev) - (complex(0,1)*gw*gwR*k1**2*sphi*sth1*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev) - (complex(0,1)*gw*gwR*k2**2*sphi*sth1*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev) - (2*cth1*complex(0,1)*gwR**2*sphi*sw*vR*cmath.sqrt(1 - 2*sw**2))/cw',
                   order = {'EWL':-1,'EWR':2})

GC_1350 = Coupling(name = 'GC_1350',
                   value = '(cth1*cw*complex(0,1)*gw**2*k1**2*sphi*sw)/(2.*vev) + (cth1*cw*complex(0,1)*gw**2*k2**2*sphi*sw)/(2.*vev) - (cphi*cth1*complex(0,1)*gw*gwR*k1**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev) - (cphi*cth1*complex(0,1)*gw*gwR*k2**2*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev) + (2*cphi*complex(0,1)*gwR**2*sth1*sw*vR*cmath.sqrt(1 - 2*sw**2))/cw',
                   order = {'EWR':1})

GC_1351 = Coupling(name = 'GC_1351',
                   value = '-(cphi*cth1*complex(0,1)*gwR**2*k1**2*sw**3)/(2.*cw*vev) - (cphi*cth1*complex(0,1)*gwR**2*k2**2*sw**3)/(2.*cw*vev) + (cth1*complex(0,1)*gw*gwR*k1**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev) + (cth1*complex(0,1)*gw*gwR*k2**2*sphi*sw*cmath.sqrt(1 - 2*sw**2))/(2.*cw*vev) - (2*complex(0,1)*gwR**2*sphi*sth1*sw*vR*cmath.sqrt(1 - 2*sw**2))/cw',
                   order = {'EWL':-1,'EWR':2})

GC_1352 = Coupling(name = 'GC_1352',
                   value = '-(cphi*cw**2*complex(0,1)*gw**2*k1**2*sphi*sth1)/(2.*vev) - (cphi*cw**2*complex(0,1)*gw**2*k2**2*sphi*sth1)/(2.*vev) + (cphi**2*complex(0,1)*gw*gwR*k1**2*sth1*cmath.sqrt(1 - 2*sw**2))/(2.*vev) + (cphi**2*complex(0,1)*gw*gwR*k2**2*sth1*cmath.sqrt(1 - 2*sw**2))/(2.*vev) - (2*cphi**2*cth1*complex(0,1)*gwR**2*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWR':1})

GC_1353 = Coupling(name = 'GC_1353',
                   value = '-(cphi**2*complex(0,1)*gwR**2*k1**2*sth1)/(2.*cw**2*vev) - (cphi**2*complex(0,1)*gwR**2*k2**2*sth1)/(2.*cw**2*vev) - (cw**2*complex(0,1)*gw**2*k1**2*sphi**2*sth1)/(2.*vev) - (cw**2*complex(0,1)*gw**2*k2**2*sphi**2*sth1)/(2.*vev) + (cphi**2*complex(0,1)*gwR**2*k1**2*sth1*sw**2)/(cw**2*vev) + (cphi**2*complex(0,1)*gwR**2*k2**2*sth1*sw**2)/(cw**2*vev) + (cphi*complex(0,1)*gw*gwR*k1**2*sphi*sth1*cmath.sqrt(1 - 2*sw**2))/vev + (cphi*complex(0,1)*gw*gwR*k2**2*sphi*sth1*cmath.sqrt(1 - 2*sw**2))/vev - (4*cphi*cth1*complex(0,1)*gwR**2*sphi*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-1,'EWR':2})

GC_1354 = Coupling(name = 'GC_1354',
                   value = '-(cphi**2*complex(0,1)*gwR**2*k1**2*sth1*sw**4)/(2.*cw**2*vev) - (cphi**2*complex(0,1)*gwR**2*k2**2*sth1*sw**4)/(2.*cw**2*vev) - (cphi*complex(0,1)*gw*gwR*k1**2*sphi*sth1*cmath.sqrt(1 - 2*sw**2))/vev - (cphi*complex(0,1)*gw*gwR*k2**2*sphi*sth1*cmath.sqrt(1 - 2*sw**2))/vev + (4*cphi*cth1*complex(0,1)*gwR**2*sphi*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-1,'EWR':2})

GC_1355 = Coupling(name = 'GC_1355',
                   value = '(cphi*complex(0,1)*gwR**2*k1**2*sphi*sth1)/(2.*cw**2*vev) + (cphi*complex(0,1)*gwR**2*k2**2*sphi*sth1)/(2.*cw**2*vev) - (cphi*complex(0,1)*gwR**2*k1**2*sphi*sth1*sw**2)/(cw**2*vev) - (cphi*complex(0,1)*gwR**2*k2**2*sphi*sth1*sw**2)/(cw**2*vev) - (cphi*complex(0,1)*gwR**2*k1**2*sphi*sth1*sw**4)/(2.*cw**2*vev) - (cphi*complex(0,1)*gwR**2*k2**2*sphi*sth1*sw**4)/(2.*cw**2*vev) - (complex(0,1)*gw*gwR*k1**2*sphi**2*sth1*cmath.sqrt(1 - 2*sw**2))/(2.*vev) - (complex(0,1)*gw*gwR*k2**2*sphi**2*sth1*cmath.sqrt(1 - 2*sw**2))/(2.*vev) + (2*cth1*complex(0,1)*gwR**2*sphi**2*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-2,'EWR':3})

GC_1356 = Coupling(name = 'GC_1356',
                   value = '(cphi*cth1*cw**2*complex(0,1)*gw**2*k1**2*sphi)/(2.*vev) + (cphi*cth1*cw**2*complex(0,1)*gw**2*k2**2*sphi)/(2.*vev) - (cphi**2*cth1*complex(0,1)*gw*gwR*k1**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev) - (cphi**2*cth1*complex(0,1)*gw*gwR*k2**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev) - (2*cphi**2*complex(0,1)*gwR**2*sth1*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWR':1})

GC_1357 = Coupling(name = 'GC_1357',
                   value = '(cphi**2*cth1*complex(0,1)*gwR**2*k1**2)/(2.*cw**2*vev) + (cphi**2*cth1*complex(0,1)*gwR**2*k2**2)/(2.*cw**2*vev) + (cth1*cw**2*complex(0,1)*gw**2*k1**2*sphi**2)/(2.*vev) + (cth1*cw**2*complex(0,1)*gw**2*k2**2*sphi**2)/(2.*vev) - (cphi**2*cth1*complex(0,1)*gwR**2*k1**2*sw**2)/(cw**2*vev) - (cphi**2*cth1*complex(0,1)*gwR**2*k2**2*sw**2)/(cw**2*vev) - (cphi*cth1*complex(0,1)*gw*gwR*k1**2*sphi*cmath.sqrt(1 - 2*sw**2))/vev - (cphi*cth1*complex(0,1)*gw*gwR*k2**2*sphi*cmath.sqrt(1 - 2*sw**2))/vev - (4*cphi*complex(0,1)*gwR**2*sphi*sth1*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-1,'EWR':2})

GC_1358 = Coupling(name = 'GC_1358',
                   value = '(cphi**2*cth1*complex(0,1)*gwR**2*k1**2*sw**4)/(2.*cw**2*vev) + (cphi**2*cth1*complex(0,1)*gwR**2*k2**2*sw**4)/(2.*cw**2*vev) + (cphi*cth1*complex(0,1)*gw*gwR*k1**2*sphi*cmath.sqrt(1 - 2*sw**2))/vev + (cphi*cth1*complex(0,1)*gw*gwR*k2**2*sphi*cmath.sqrt(1 - 2*sw**2))/vev + (4*cphi*complex(0,1)*gwR**2*sphi*sth1*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-1,'EWR':2})

GC_1359 = Coupling(name = 'GC_1359',
                   value = '-(cphi*cth1*complex(0,1)*gwR**2*k1**2*sphi)/(2.*cw**2*vev) - (cphi*cth1*complex(0,1)*gwR**2*k2**2*sphi)/(2.*cw**2*vev) + (cphi*cth1*complex(0,1)*gwR**2*k1**2*sphi*sw**2)/(cw**2*vev) + (cphi*cth1*complex(0,1)*gwR**2*k2**2*sphi*sw**2)/(cw**2*vev) + (cphi*cth1*complex(0,1)*gwR**2*k1**2*sphi*sw**4)/(2.*cw**2*vev) + (cphi*cth1*complex(0,1)*gwR**2*k2**2*sphi*sw**4)/(2.*cw**2*vev) + (cth1*complex(0,1)*gw*gwR*k1**2*sphi**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev) + (cth1*complex(0,1)*gw*gwR*k2**2*sphi**2*cmath.sqrt(1 - 2*sw**2))/(2.*vev) + (2*complex(0,1)*gwR**2*sphi**2*sth1*sw**2*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-2,'EWR':3})

GC_1360 = Coupling(name = 'GC_1360',
                   value = '(-4*cphi*cth1*complex(0,1)*g1*gwR*sphi*sw*vR*cmath.sqrt(1 - 2*sw**2))/cw**2 - (4*cphi*cth1*complex(0,1)*g1*gwR*sphi*sw**3*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1361 = Coupling(name = 'GC_1361',
                   value = '(-4*cphi*complex(0,1)*g1*gwR*sphi*sth1*sw*vR*cmath.sqrt(1 - 2*sw**2))/cw**2 - (4*cphi*complex(0,1)*g1*gwR*sphi*sth1*sw**3*vR*cmath.sqrt(1 - 2*sw**2))/cw**2',
                   order = {'EWL':-1,'EWR':1,'QED':1})

GC_1362 = Coupling(name = 'GC_1362',
                   value = '(2*complex(0,1)*k1*k2*yDO1x1)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1363 = Coupling(name = 'GC_1363',
                   value = '(2*k1*k2*yDO1x1)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1364 = Coupling(name = 'GC_1364',
                   value = '-((cth1*complex(0,1)*k1**2*yDO1x1)/((k1 - k2)*(k1 + k2)*vev)) + (cth1*complex(0,1)*k2**2*yDO1x1)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1365 = Coupling(name = 'GC_1365',
                   value = '(complex(0,1)*k1**2*sth1*yDO1x1)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*k2**2*sth1*yDO1x1)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1366 = Coupling(name = 'GC_1366',
                   value = '(2*complex(0,1)*k1*k2*yDO2x2)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1367 = Coupling(name = 'GC_1367',
                   value = '(2*k1*k2*yDO2x2)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1368 = Coupling(name = 'GC_1368',
                   value = '-((cth1*complex(0,1)*k1**2*yDO2x2)/((k1 - k2)*(k1 + k2)*vev)) + (cth1*complex(0,1)*k2**2*yDO2x2)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1369 = Coupling(name = 'GC_1369',
                   value = '(complex(0,1)*k1**2*sth1*yDO2x2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*k2**2*sth1*yDO2x2)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1370 = Coupling(name = 'GC_1370',
                   value = '(2*complex(0,1)*k1*k2*yDO3x3)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1371 = Coupling(name = 'GC_1371',
                   value = '(2*k1*k2*yDO3x3)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1372 = Coupling(name = 'GC_1372',
                   value = '-((cth1*complex(0,1)*k1**2*yDO3x3)/((k1 - k2)*(k1 + k2)*vev)) + (cth1*complex(0,1)*k2**2*yDO3x3)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1373 = Coupling(name = 'GC_1373',
                   value = '(complex(0,1)*k1**2*sth1*yDO3x3)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*k2**2*sth1*yDO3x3)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1374 = Coupling(name = 'GC_1374',
                   value = '(2*complex(0,1)*k1*k2*yML1x1)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1375 = Coupling(name = 'GC_1375',
                   value = '(2*k1*k2*yML1x1)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1376 = Coupling(name = 'GC_1376',
                   value = '-((cth1*complex(0,1)*k1**2*yML1x1)/((k1 - k2)*(k1 + k2)*vev)) + (cth1*complex(0,1)*k2**2*yML1x1)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1377 = Coupling(name = 'GC_1377',
                   value = '(complex(0,1)*k1**2*sth1*yML1x1)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*k2**2*sth1*yML1x1)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1378 = Coupling(name = 'GC_1378',
                   value = '(2*complex(0,1)*k1*k2*yML2x2)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1379 = Coupling(name = 'GC_1379',
                   value = '(2*k1*k2*yML2x2)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1380 = Coupling(name = 'GC_1380',
                   value = '-((cth1*complex(0,1)*k1**2*yML2x2)/((k1 - k2)*(k1 + k2)*vev)) + (cth1*complex(0,1)*k2**2*yML2x2)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1381 = Coupling(name = 'GC_1381',
                   value = '(complex(0,1)*k1**2*sth1*yML2x2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*k2**2*sth1*yML2x2)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1382 = Coupling(name = 'GC_1382',
                   value = '(2*complex(0,1)*k1*k2*yML3x3)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1383 = Coupling(name = 'GC_1383',
                   value = '(2*k1*k2*yML3x3)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1384 = Coupling(name = 'GC_1384',
                   value = '-((cth1*complex(0,1)*k1**2*yML3x3)/((k1 - k2)*(k1 + k2)*vev)) + (cth1*complex(0,1)*k2**2*yML3x3)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1385 = Coupling(name = 'GC_1385',
                   value = '(complex(0,1)*k1**2*sth1*yML3x3)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*k2**2*sth1*yML3x3)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1386 = Coupling(name = 'GC_1386',
                   value = '(-2*complex(0,1)*k1*k2*yMU1x1)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1387 = Coupling(name = 'GC_1387',
                   value = '(2*k1*k2*yMU1x1)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1388 = Coupling(name = 'GC_1388',
                   value = '-((cth1*complex(0,1)*k1**2*yMU1x1)/((k1 - k2)*(k1 + k2)*vev)) + (cth1*complex(0,1)*k2**2*yMU1x1)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1389 = Coupling(name = 'GC_1389',
                   value = '(complex(0,1)*k1**2*sth1*yMU1x1)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*k2**2*sth1*yMU1x1)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1390 = Coupling(name = 'GC_1390',
                   value = '(-2*complex(0,1)*k1*k2*yMU2x2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1391 = Coupling(name = 'GC_1391',
                   value = '(2*k1*k2*yMU2x2)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1392 = Coupling(name = 'GC_1392',
                   value = '-((cth1*complex(0,1)*k1**2*yMU2x2)/((k1 - k2)*(k1 + k2)*vev)) + (cth1*complex(0,1)*k2**2*yMU2x2)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1393 = Coupling(name = 'GC_1393',
                   value = '(complex(0,1)*k1**2*sth1*yMU2x2)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*k2**2*sth1*yMU2x2)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1394 = Coupling(name = 'GC_1394',
                   value = '(-2*complex(0,1)*k1*k2*yMU3x3)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1395 = Coupling(name = 'GC_1395',
                   value = '(2*k1*k2*yMU3x3)/((-k1 + k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1396 = Coupling(name = 'GC_1396',
                   value = '-((cth1*complex(0,1)*k1**2*yMU3x3)/((k1 - k2)*(k1 + k2)*vev)) + (cth1*complex(0,1)*k2**2*yMU3x3)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1397 = Coupling(name = 'GC_1397',
                   value = '(complex(0,1)*k1**2*sth1*yMU3x3)/((k1 - k2)*(k1 + k2)*vev) - (complex(0,1)*k2**2*sth1*yMU3x3)/((k1 - k2)*(k1 + k2)*vev)',
                   order = {'EWL':1})

GC_1398 = Coupling(name = 'GC_1398',
                   value = '(cxi*complex(0,1)*gw*complexconjugate(CKML1x1))/cmath.sqrt(2)',
                   order = {'EWL':1})

GC_1399 = Coupling(name = 'GC_1399',
                   value = '(complex(0,1)*gw*sxi*complexconjugate(CKML1x1))/cmath.sqrt(2)',
                   order = {'EWR':1})

GC_1400 = Coupling(name = 'GC_1400',
                   value = '(cxi*complex(0,1)*gw*complexconjugate(CKML1x2))/cmath.sqrt(2)',
                   order = {'EWL':1})

GC_1401 = Coupling(name = 'GC_1401',
                   value = '(complex(0,1)*gw*sxi*complexconjugate(CKML1x2))/cmath.sqrt(2)',
                   order = {'EWR':1})

GC_1402 = Coupling(name = 'GC_1402',
                   value = '(cxi*complex(0,1)*gw*complexconjugate(CKML1x3))/cmath.sqrt(2)',
                   order = {'EWL':1})

GC_1403 = Coupling(name = 'GC_1403',
                   value = '(complex(0,1)*gw*sxi*complexconjugate(CKML1x3))/cmath.sqrt(2)',
                   order = {'EWR':1})

GC_1404 = Coupling(name = 'GC_1404',
                   value = '(cxi*complex(0,1)*gw*complexconjugate(CKML2x1))/cmath.sqrt(2)',
                   order = {'EWL':1})

GC_1405 = Coupling(name = 'GC_1405',
                   value = '(complex(0,1)*gw*sxi*complexconjugate(CKML2x1))/cmath.sqrt(2)',
                   order = {'EWR':1})

GC_1406 = Coupling(name = 'GC_1406',
                   value = '(cxi*complex(0,1)*gw*complexconjugate(CKML2x2))/cmath.sqrt(2)',
                   order = {'EWL':1})

GC_1407 = Coupling(name = 'GC_1407',
                   value = '(complex(0,1)*gw*sxi*complexconjugate(CKML2x2))/cmath.sqrt(2)',
                   order = {'EWR':1})

GC_1408 = Coupling(name = 'GC_1408',
                   value = '(cxi*complex(0,1)*gw*complexconjugate(CKML2x3))/cmath.sqrt(2)',
                   order = {'EWL':1})

GC_1409 = Coupling(name = 'GC_1409',
                   value = '(complex(0,1)*gw*sxi*complexconjugate(CKML2x3))/cmath.sqrt(2)',
                   order = {'EWR':1})

GC_1410 = Coupling(name = 'GC_1410',
                   value = '(cxi*complex(0,1)*gw*complexconjugate(CKML3x1))/cmath.sqrt(2)',
                   order = {'EWL':1})

GC_1411 = Coupling(name = 'GC_1411',
                   value = '(complex(0,1)*gw*sxi*complexconjugate(CKML3x1))/cmath.sqrt(2)',
                   order = {'EWR':1})

GC_1412 = Coupling(name = 'GC_1412',
                   value = '(cxi*complex(0,1)*gw*complexconjugate(CKML3x2))/cmath.sqrt(2)',
                   order = {'EWL':1})

GC_1413 = Coupling(name = 'GC_1413',
                   value = '(complex(0,1)*gw*sxi*complexconjugate(CKML3x2))/cmath.sqrt(2)',
                   order = {'EWR':1})

GC_1414 = Coupling(name = 'GC_1414',
                   value = '(cxi*complex(0,1)*gw*complexconjugate(CKML3x3))/cmath.sqrt(2)',
                   order = {'EWL':1})

GC_1415 = Coupling(name = 'GC_1415',
                   value = '(complex(0,1)*gw*sxi*complexconjugate(CKML3x3))/cmath.sqrt(2)',
                   order = {'EWR':1})

GC_1416 = Coupling(name = 'GC_1416',
                   value = '(cxi*complex(0,1)*gwR*complexconjugate(CKMR1x1))/cmath.sqrt(2)',
                   order = {'EWR':1})

GC_1417 = Coupling(name = 'GC_1417',
                   value = '-((complex(0,1)*gwR*sxi*complexconjugate(CKMR1x1))/cmath.sqrt(2))',
                   order = {'EWL':-1,'EWR':2})

GC_1418 = Coupling(name = 'GC_1418',
                   value = '(cxi*complex(0,1)*gwR*complexconjugate(CKMR1x2))/cmath.sqrt(2)',
                   order = {'EWR':1})

GC_1419 = Coupling(name = 'GC_1419',
                   value = '-((complex(0,1)*gwR*sxi*complexconjugate(CKMR1x2))/cmath.sqrt(2))',
                   order = {'EWL':-1,'EWR':2})

GC_1420 = Coupling(name = 'GC_1420',
                   value = '(cxi*complex(0,1)*gwR*complexconjugate(CKMR1x3))/cmath.sqrt(2)',
                   order = {'EWR':1})

GC_1421 = Coupling(name = 'GC_1421',
                   value = '-((complex(0,1)*gwR*sxi*complexconjugate(CKMR1x3))/cmath.sqrt(2))',
                   order = {'EWL':-1,'EWR':2})

GC_1422 = Coupling(name = 'GC_1422',
                   value = '(cxi*complex(0,1)*gwR*complexconjugate(CKMR2x1))/cmath.sqrt(2)',
                   order = {'EWR':1})

GC_1423 = Coupling(name = 'GC_1423',
                   value = '-((complex(0,1)*gwR*sxi*complexconjugate(CKMR2x1))/cmath.sqrt(2))',
                   order = {'EWL':-1,'EWR':2})

GC_1424 = Coupling(name = 'GC_1424',
                   value = '(cxi*complex(0,1)*gwR*complexconjugate(CKMR2x2))/cmath.sqrt(2)',
                   order = {'EWR':1})

GC_1425 = Coupling(name = 'GC_1425',
                   value = '-((complex(0,1)*gwR*sxi*complexconjugate(CKMR2x2))/cmath.sqrt(2))',
                   order = {'EWL':-1,'EWR':2})

GC_1426 = Coupling(name = 'GC_1426',
                   value = '(cxi*complex(0,1)*gwR*complexconjugate(CKMR2x3))/cmath.sqrt(2)',
                   order = {'EWR':1})

GC_1427 = Coupling(name = 'GC_1427',
                   value = '-((complex(0,1)*gwR*sxi*complexconjugate(CKMR2x3))/cmath.sqrt(2))',
                   order = {'EWL':-1,'EWR':2})

GC_1428 = Coupling(name = 'GC_1428',
                   value = '(cxi*complex(0,1)*gwR*complexconjugate(CKMR3x1))/cmath.sqrt(2)',
                   order = {'EWR':1})

GC_1429 = Coupling(name = 'GC_1429',
                   value = '-((complex(0,1)*gwR*sxi*complexconjugate(CKMR3x1))/cmath.sqrt(2))',
                   order = {'EWL':-1,'EWR':2})

GC_1430 = Coupling(name = 'GC_1430',
                   value = '(cxi*complex(0,1)*gwR*complexconjugate(CKMR3x2))/cmath.sqrt(2)',
                   order = {'EWR':1})

GC_1431 = Coupling(name = 'GC_1431',
                   value = '-((complex(0,1)*gwR*sxi*complexconjugate(CKMR3x2))/cmath.sqrt(2))',
                   order = {'EWL':-1,'EWR':2})

GC_1432 = Coupling(name = 'GC_1432',
                   value = '(cxi*complex(0,1)*gwR*complexconjugate(CKMR3x3))/cmath.sqrt(2)',
                   order = {'EWR':1})

GC_1433 = Coupling(name = 'GC_1433',
                   value = '-((complex(0,1)*gwR*sxi*complexconjugate(CKMR3x3))/cmath.sqrt(2))',
                   order = {'EWL':-1,'EWR':2})


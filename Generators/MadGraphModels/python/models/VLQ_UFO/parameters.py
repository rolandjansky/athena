# This file was automatically created by FeynRules $Revision: 573 $
# Mathematica version: 8.0 for Microsoft Windows (32-bit) (February 24, 2011)
# Date: Tue 2 Jul 2013 00:32:00



from object_library import all_parameters, Parameter


from function_library import complexconjugate, re, im, csc, sec, acsc, asec

# This is a default parameter object representing 0.
ZERO = Parameter(name = 'ZERO',
                 nature = 'internal',
                 type = 'real',
                 value = '0.0',
                 texname = '0')

# User-defined parameters.
CKM11 = Parameter(name = 'CKM11',
                  nature = 'external',
                  type = 'complex',
                  value = 0.97428,
                  texname = '\\text{CKM11}',
                  lhablock = 'CKMBlock',
                  lhacode = [ 1, 1 ])

CKM12 = Parameter(name = 'CKM12',
                  nature = 'external',
                  type = 'complex',
                  value = 0.2253,
                  texname = '\\text{CKM12}',
                  lhablock = 'CKMBlock',
                  lhacode = [ 1, 2 ])

CKM13 = Parameter(name = 'CKM13',
                  nature = 'external',
                  type = 'complex',
                  value = 0.00347,
                  texname = '\\text{CKM13}',
                  lhablock = 'CKMBlock',
                  lhacode = [ 1, 3 ])

CKM21 = Parameter(name = 'CKM21',
                  nature = 'external',
                  type = 'complex',
                  value = 0.2252,
                  texname = '\\text{CKM21}',
                  lhablock = 'CKMBlock',
                  lhacode = [ 2, 1 ])

CKM22 = Parameter(name = 'CKM22',
                  nature = 'external',
                  type = 'complex',
                  value = 0.97345,
                  texname = '\\text{CKM22}',
                  lhablock = 'CKMBlock',
                  lhacode = [ 2, 2 ])

CKM23 = Parameter(name = 'CKM23',
                  nature = 'external',
                  type = 'complex',
                  value = 0.041,
                  texname = '\\text{CKM23}',
                  lhablock = 'CKMBlock',
                  lhacode = [ 2, 3 ])

CKM31 = Parameter(name = 'CKM31',
                  nature = 'external',
                  type = 'complex',
                  value = 0.00862,
                  texname = '\\text{CKM31}',
                  lhablock = 'CKMBlock',
                  lhacode = [ 3, 1 ])

CKM32 = Parameter(name = 'CKM32',
                  nature = 'external',
                  type = 'complex',
                  value = 0.0403,
                  texname = '\\text{CKM32}',
                  lhablock = 'CKMBlock',
                  lhacode = [ 3, 2 ])

CKM33 = Parameter(name = 'CKM33',
                  nature = 'external',
                  type = 'complex',
                  value = 0.999152,
                  texname = '\\text{CKM33}',
                  lhablock = 'CKMBlock',
                  lhacode = [ 3, 3 ])

KX = Parameter(name = 'KX',
               nature = 'external',
               type = 'real',
               value = 1,
               texname = '\\text{KX}',
               lhablock = 'Kappa',
               lhacode = [ 1 ])

KT = Parameter(name = 'KT',
               nature = 'external',
               type = 'real',
               value = 1,
               texname = '\\text{KT}',
               lhablock = 'Kappa',
               lhacode = [ 2 ])

KB = Parameter(name = 'KB',
               nature = 'external',
               type = 'real',
               value = 1,
               texname = '\\text{KB}',
               lhablock = 'Kappa',
               lhacode = [ 3 ])

KY = Parameter(name = 'KY',
               nature = 'external',
               type = 'real',
               value = 1,
               texname = '\\text{KY}',
               lhablock = 'Kappa',
               lhacode = [ 4 ])

aEWM1 = Parameter(name = 'aEWM1',
                  nature = 'external',
                  type = 'real',
                  value = 127.9,
                  texname = '\\text{aEWM1}',
                  lhablock = 'SMINPUTS',
                  lhacode = [ 1 ])

Gf = Parameter(name = 'Gf',
               nature = 'external',
               type = 'real',
               value = 0.00001166,
               texname = 'G_f',
               lhablock = 'SMINPUTS',
               lhacode = [ 2 ])

aS = Parameter(name = 'aS',
               nature = 'external',
               type = 'real',
               value = 0.118,
               texname = '\\text{aS}',
               lhablock = 'SMINPUTS',
               lhacode = [ 3 ])

xitpw = Parameter(name = 'xitpw',
                  nature = 'external',
                  type = 'real',
                  value = 0.4,
                  texname = '\\text{xitpw}',
                  lhablock = 'Xi',
                  lhacode = [ 1 ])

xitpz = Parameter(name = 'xitpz',
                  nature = 'external',
                  type = 'real',
                  value = 0.3,
                  texname = '\\text{xitpz}',
                  lhablock = 'Xi',
                  lhacode = [ 2 ])

xitph = Parameter(name = 'xitph',
                  nature = 'external',
                  type = 'real',
                  value = 0.3,
                  texname = '\\text{xitph}',
                  lhablock = 'Xi',
                  lhacode = [ 3 ])

xibpw = Parameter(name = 'xibpw',
                  nature = 'external',
                  type = 'real',
                  value = 0.4,
                  texname = '\\text{xibpw}',
                  lhablock = 'Xi',
                  lhacode = [ 4 ])

xibpz = Parameter(name = 'xibpz',
                  nature = 'external',
                  type = 'real',
                  value = 0.3,
                  texname = '\\text{xibpz}',
                  lhablock = 'Xi',
                  lhacode = [ 5 ])

xibph = Parameter(name = 'xibph',
                  nature = 'external',
                  type = 'real',
                  value = 0.3,
                  texname = '\\text{xibph}',
                  lhablock = 'Xi',
                  lhacode = [ 6 ])

ymb = Parameter(name = 'ymb',
                nature = 'external',
                type = 'real',
                value = 4.2,
                texname = '\\text{ymb}',
                lhablock = 'YUKAWA',
                lhacode = [ 5 ])

ymt = Parameter(name = 'ymt',
                nature = 'external',
                type = 'real',
                value = 174.3,
                texname = '\\text{ymt}',
                lhablock = 'YUKAWA',
                lhacode = [ 6 ])

ymtau = Parameter(name = 'ymtau',
                  nature = 'external',
                  type = 'real',
                  value = 1.777,
                  texname = '\\text{ymtau}',
                  lhablock = 'YUKAWA',
                  lhacode = [ 15 ])

zetaXuL = Parameter(name = 'zetaXuL',
                    nature = 'external',
                    type = 'real',
                    value = 0.3,
                    texname = '\\text{zetaXuL}',
                    lhablock = 'Zeta',
                    lhacode = [ 1 ])

zetaXcL = Parameter(name = 'zetaXcL',
                    nature = 'external',
                    type = 'real',
                    value = 0.3,
                    texname = '\\text{zetaXcL}',
                    lhablock = 'Zeta',
                    lhacode = [ 2 ])

zetaXtL = Parameter(name = 'zetaXtL',
                    nature = 'external',
                    type = 'real',
                    value = 0.4,
                    texname = '\\text{zetaXtL}',
                    lhablock = 'Zeta',
                    lhacode = [ 3 ])

zetaTuL = Parameter(name = 'zetaTuL',
                    nature = 'external',
                    type = 'real',
                    value = 0.3,
                    texname = '\\text{zetaTuL}',
                    lhablock = 'Zeta',
                    lhacode = [ 4 ])

zetaTcL = Parameter(name = 'zetaTcL',
                    nature = 'external',
                    type = 'real',
                    value = 0.3,
                    texname = '\\text{zetaTcL}',
                    lhablock = 'Zeta',
                    lhacode = [ 5 ])

zetaTtL = Parameter(name = 'zetaTtL',
                    nature = 'external',
                    type = 'real',
                    value = 0.4,
                    texname = '\\text{zetaTtL}',
                    lhablock = 'Zeta',
                    lhacode = [ 6 ])

zetaBdL = Parameter(name = 'zetaBdL',
                    nature = 'external',
                    type = 'real',
                    value = 0.3,
                    texname = '\\text{zetaBdL}',
                    lhablock = 'Zeta',
                    lhacode = [ 7 ])

zetaBsL = Parameter(name = 'zetaBsL',
                    nature = 'external',
                    type = 'real',
                    value = 0.3,
                    texname = '\\text{zetaBsL}',
                    lhablock = 'Zeta',
                    lhacode = [ 8 ])

zetaBbL = Parameter(name = 'zetaBbL',
                    nature = 'external',
                    type = 'real',
                    value = 0.4,
                    texname = '\\text{zetaBbL}',
                    lhablock = 'Zeta',
                    lhacode = [ 9 ])

zetaYdL = Parameter(name = 'zetaYdL',
                    nature = 'external',
                    type = 'real',
                    value = 0.3,
                    texname = '\\text{zetaYdL}',
                    lhablock = 'Zeta',
                    lhacode = [ 10 ])

zetaYsL = Parameter(name = 'zetaYsL',
                    nature = 'external',
                    type = 'real',
                    value = 0.3,
                    texname = '\\text{zetaYsL}',
                    lhablock = 'Zeta',
                    lhacode = [ 11 ])

zetaYbL = Parameter(name = 'zetaYbL',
                    nature = 'external',
                    type = 'real',
                    value = 0.4,
                    texname = '\\text{zetaYbL}',
                    lhablock = 'Zeta',
                    lhacode = [ 12 ])

zetaXuR = Parameter(name = 'zetaXuR',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{zetaXuR}',
                    lhablock = 'Zeta',
                    lhacode = [ 13 ])

zetaXcR = Parameter(name = 'zetaXcR',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{zetaXcR}',
                    lhablock = 'Zeta',
                    lhacode = [ 14 ])

zetaXtR = Parameter(name = 'zetaXtR',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{zetaXtR}',
                    lhablock = 'Zeta',
                    lhacode = [ 15 ])

zetaTuR = Parameter(name = 'zetaTuR',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{zetaTuR}',
                    lhablock = 'Zeta',
                    lhacode = [ 16 ])

zetaTcR = Parameter(name = 'zetaTcR',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{zetaTcR}',
                    lhablock = 'Zeta',
                    lhacode = [ 17 ])

zetaTtR = Parameter(name = 'zetaTtR',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{zetaTtR}',
                    lhablock = 'Zeta',
                    lhacode = [ 18 ])

zetaBdR = Parameter(name = 'zetaBdR',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{zetaBdR}',
                    lhablock = 'Zeta',
                    lhacode = [ 19 ])

zetaBsR = Parameter(name = 'zetaBsR',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{zetaBsR}',
                    lhablock = 'Zeta',
                    lhacode = [ 20 ])

zetaBbR = Parameter(name = 'zetaBbR',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{zetaBbR}',
                    lhablock = 'Zeta',
                    lhacode = [ 21 ])

zetaYdR = Parameter(name = 'zetaYdR',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{zetaYdR}',
                    lhablock = 'Zeta',
                    lhacode = [ 22 ])

zetaYsR = Parameter(name = 'zetaYsR',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{zetaYsR}',
                    lhablock = 'Zeta',
                    lhacode = [ 23 ])

zetaYbR = Parameter(name = 'zetaYbR',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{zetaYbR}',
                    lhablock = 'Zeta',
                    lhacode = [ 24 ])

MTA = Parameter(name = 'MTA',
                nature = 'external',
                type = 'real',
                value = 1.777,
                texname = '\\text{MTA}',
                lhablock = 'MASS',
                lhacode = [ 15 ])

MT = Parameter(name = 'MT',
               nature = 'external',
               type = 'real',
               value = 174.3,
               texname = '\\text{MT}',
               lhablock = 'MASS',
               lhacode = [ 6 ])

MB = Parameter(name = 'MB',
               nature = 'external',
               type = 'real',
               value = 4.2,
               texname = '\\text{MB}',
               lhablock = 'MASS',
               lhacode = [ 5 ])

MX = Parameter(name = 'MX',
               nature = 'external',
               type = 'real',
               value = 600,
               texname = '\\text{MX}',
               lhablock = 'MASS',
               lhacode = [ 6000005 ])

MTP = Parameter(name = 'MTP',
                nature = 'external',
                type = 'real',
                value = 600,
                texname = '\\text{MTP}',
                lhablock = 'MASS',
                lhacode = [ 6000006 ])

MBP = Parameter(name = 'MBP',
                nature = 'external',
                type = 'real',
                value = 600,
                texname = '\\text{MBP}',
                lhablock = 'MASS',
                lhacode = [ 6000007 ])

MY = Parameter(name = 'MY',
               nature = 'external',
               type = 'real',
               value = 600,
               texname = '\\text{MY}',
               lhablock = 'MASS',
               lhacode = [ 6000008 ])

MZ = Parameter(name = 'MZ',
               nature = 'external',
               type = 'real',
               value = 91.1876,
               texname = '\\text{MZ}',
               lhablock = 'MASS',
               lhacode = [ 23 ])

MH = Parameter(name = 'MH',
               nature = 'external',
               type = 'real',
               value = 125,
               texname = '\\text{MH}',
               lhablock = 'MASS',
               lhacode = [ 25 ])

WT = Parameter(name = 'WT',
               nature = 'external',
               type = 'real',
               value = 1.5101349,
               texname = '\\text{WT}',
               lhablock = 'DECAY',
               lhacode = [ 6 ])

WX = Parameter(name = 'WX',
               nature = 'external',
               type = 'real',
               value = 1,
               texname = '\\text{WX}',
               lhablock = 'DECAY',
               lhacode = [ 6000005 ])

WTP = Parameter(name = 'WTP',
                nature = 'external',
                type = 'real',
                value = 1,
                texname = '\\text{WTP}',
                lhablock = 'DECAY',
                lhacode = [ 6000006 ])

WBP = Parameter(name = 'WBP',
                nature = 'external',
                type = 'real',
                value = 1,
                texname = '\\text{WBP}',
                lhablock = 'DECAY',
                lhacode = [ 6000007 ])

WY = Parameter(name = 'WY',
               nature = 'external',
               type = 'real',
               value = 1,
               texname = '\\text{WY}',
               lhablock = 'DECAY',
               lhacode = [ 6000008 ])

WZ = Parameter(name = 'WZ',
               nature = 'external',
               type = 'real',
               value = 2.44639985,
               texname = '\\text{WZ}',
               lhablock = 'DECAY',
               lhacode = [ 23 ])

WW = Parameter(name = 'WW',
               nature = 'external',
               type = 'real',
               value = 2.0353557,
               texname = '\\text{WW}',
               lhablock = 'DECAY',
               lhacode = [ 24 ])

WH = Parameter(name = 'WH',
               nature = 'external',
               type = 'real',
               value = 0.00679485838,
               texname = '\\text{WH}',
               lhablock = 'DECAY',
               lhacode = [ 25 ])

gamma0bph = Parameter(name = 'gamma0bph',
                      nature = 'internal',
                      type = 'real',
                      value = '(1 - MH**2/MBP**2)**2/2.',
                      texname = '\\text{gamma0bph}')

gamma0bpz = Parameter(name = 'gamma0bpz',
                      nature = 'internal',
                      type = 'real',
                      value = '((1 - MZ**2/MBP**2)*(1 + MZ**2/MBP**2 - (2*MZ**4)/MBP**4))/2.',
                      texname = '\\text{gamma0bpz}')

gamma0tph = Parameter(name = 'gamma0tph',
                      nature = 'internal',
                      type = 'real',
                      value = '(1 - MH**2/MTP**2)**2/2.',
                      texname = '\\text{gamma0tph}')

gamma0tpz = Parameter(name = 'gamma0tpz',
                      nature = 'internal',
                      type = 'real',
                      value = '((1 - MZ**2/MTP**2)*(1 + MZ**2/MTP**2 - (2*MZ**4)/MTP**4))/2.',
                      texname = '\\text{gamma0tpz}')

aEW = Parameter(name = 'aEW',
                nature = 'internal',
                type = 'real',
                value = '1/aEWM1',
                texname = '\\text{aEW}')

G = Parameter(name = 'G',
              nature = 'internal',
              type = 'real',
              value = '2*cmath.sqrt(aS)*cmath.sqrt(cmath.pi)',
              texname = 'G')

MW = Parameter(name = 'MW',
               nature = 'internal',
               type = 'real',
               value = 'cmath.sqrt(MZ**2/2. + cmath.sqrt(MZ**4/4. - (aEW*cmath.pi*MZ**2)/(Gf*cmath.sqrt(2))))',
               texname = 'M_W')

ee = Parameter(name = 'ee',
               nature = 'internal',
               type = 'real',
               value = '2*cmath.sqrt(aEW)*cmath.sqrt(cmath.pi)',
               texname = 'e')

KBbLh = Parameter(name = 'KBbLh',
                  nature = 'internal',
                  type = 'real',
                  value = 'cmath.sqrt((xibph*zetaBbL)/gamma0bph)',
                  texname = '\\text{KBbLh}')

KBbRh = Parameter(name = 'KBbRh',
                  nature = 'internal',
                  type = 'real',
                  value = 'cmath.sqrt((xibph*zetaBbR)/gamma0bph)',
                  texname = '\\text{KBbRh}')

KBdLh = Parameter(name = 'KBdLh',
                  nature = 'internal',
                  type = 'real',
                  value = 'cmath.sqrt((xibph*zetaBdL)/gamma0bph)',
                  texname = '\\text{KBdLh}')

KBdRh = Parameter(name = 'KBdRh',
                  nature = 'internal',
                  type = 'real',
                  value = 'cmath.sqrt((xibph*zetaBdR)/gamma0bph)',
                  texname = '\\text{KBdRh}')

KBsLh = Parameter(name = 'KBsLh',
                  nature = 'internal',
                  type = 'real',
                  value = 'cmath.sqrt((xibph*zetaBsL)/gamma0bph)',
                  texname = '\\text{KBsLh}')

KBsRh = Parameter(name = 'KBsRh',
                  nature = 'internal',
                  type = 'real',
                  value = 'cmath.sqrt((xibph*zetaBsR)/gamma0bph)',
                  texname = '\\text{KBsRh}')

KTcLh = Parameter(name = 'KTcLh',
                  nature = 'internal',
                  type = 'real',
                  value = 'cmath.sqrt((xitph*zetaTcL)/gamma0tph)',
                  texname = '\\text{KTcLh}')

KTcRh = Parameter(name = 'KTcRh',
                  nature = 'internal',
                  type = 'real',
                  value = 'cmath.sqrt((xitph*zetaTcR)/gamma0tph)',
                  texname = '\\text{KTcRh}')

KTtLh = Parameter(name = 'KTtLh',
                  nature = 'internal',
                  type = 'real',
                  value = 'cmath.sqrt((xitph*zetaTtL)/gamma0tph)',
                  texname = '\\text{KTtLh}')

KTtRh = Parameter(name = 'KTtRh',
                  nature = 'internal',
                  type = 'real',
                  value = 'cmath.sqrt((xitph*zetaTtR)/gamma0tph)',
                  texname = '\\text{KTtRh}')

KTuLh = Parameter(name = 'KTuLh',
                  nature = 'internal',
                  type = 'real',
                  value = 'cmath.sqrt((xitph*zetaTuL)/gamma0tph)',
                  texname = '\\text{KTuLh}')

KTuRh = Parameter(name = 'KTuRh',
                  nature = 'internal',
                  type = 'real',
                  value = 'cmath.sqrt((xitph*zetaTuR)/gamma0tph)',
                  texname = '\\text{KTuRh}')

gamma0bpw = Parameter(name = 'gamma0bpw',
                      nature = 'internal',
                      type = 'real',
                      value = '(1 - MW**2/MBP**2)*(1 + MW**2/MBP**2 - (2*MW**4)/MBP**4)',
                      texname = '\\text{gamma0bpw}')

gamma0tpw = Parameter(name = 'gamma0tpw',
                      nature = 'internal',
                      type = 'real',
                      value = '(1 - MW**2/MTP**2)*(1 + MW**2/MTP**2 - (2*MW**4)/MTP**4)',
                      texname = '\\text{gamma0tpw}')

gamma0xw = Parameter(name = 'gamma0xw',
                     nature = 'internal',
                     type = 'real',
                     value = '(1 - MW**2/MX**2)*(1 - (2*MW**4)/MX**4 + MW**2/MX**2)',
                     texname = '\\text{gamma0xw}')

gamma0yw = Parameter(name = 'gamma0yw',
                     nature = 'internal',
                     type = 'real',
                     value = '(1 - MW**2/MY**2)*(1 - (2*MW**4)/MY**4 + MW**2/MY**2)',
                     texname = '\\text{gamma0yw}')

sw2 = Parameter(name = 'sw2',
                nature = 'internal',
                type = 'real',
                value = '1 - MW**2/MZ**2',
                texname = '\\text{sw2}')

cw = Parameter(name = 'cw',
               nature = 'internal',
               type = 'real',
               value = 'cmath.sqrt(1 - sw2)',
               texname = 'c_w')

sw = Parameter(name = 'sw',
               nature = 'internal',
               type = 'real',
               value = 'cmath.sqrt(sw2)',
               texname = 's_w')

g1 = Parameter(name = 'g1',
               nature = 'internal',
               type = 'real',
               value = 'ee/cw',
               texname = 'g_1')

gw = Parameter(name = 'gw',
               nature = 'internal',
               type = 'real',
               value = 'ee/sw',
               texname = 'g_w')

KBdLw = Parameter(name = 'KBdLw',
                  nature = 'internal',
                  type = 'real',
                  value = '(ee*cmath.sqrt((xibpw*zetaBdL)/gamma0bpw))/(sw*cmath.sqrt(2))',
                  texname = '\\text{KBdLw}')

KBdRw = Parameter(name = 'KBdRw',
                  nature = 'internal',
                  type = 'real',
                  value = '(ee*cmath.sqrt((xibpw*zetaBdR)/gamma0bpw))/(sw*cmath.sqrt(2))',
                  texname = '\\text{KBdRw}')

KBsLw = Parameter(name = 'KBsLw',
                  nature = 'internal',
                  type = 'real',
                  value = '(ee*cmath.sqrt((xibpw*zetaBsL)/gamma0bpw))/(sw*cmath.sqrt(2))',
                  texname = '\\text{KBsLw}')

KTcLw = Parameter(name = 'KTcLw',
                  nature = 'internal',
                  type = 'real',
                  value = '(ee*cmath.sqrt((xitpw*zetaTcL)/gamma0tpw))/(sw*cmath.sqrt(2))',
                  texname = '\\text{KTcLw}')

KTcLz = Parameter(name = 'KTcLz',
                  nature = 'internal',
                  type = 'real',
                  value = '(ee*cmath.sqrt((xitpz*zetaTcL)/gamma0tpz))/(2.*cw*sw)',
                  texname = '\\text{KTcLz}')

KTcRw = Parameter(name = 'KTcRw',
                  nature = 'internal',
                  type = 'real',
                  value = '(ee*cmath.sqrt((xitpw*zetaTcR)/gamma0tpw))/(sw*cmath.sqrt(2))',
                  texname = '\\text{KTcRw}')

KTcRz = Parameter(name = 'KTcRz',
                  nature = 'internal',
                  type = 'real',
                  value = '(ee*cmath.sqrt((xitpz*zetaTcR)/gamma0tpz))/(2.*cw*sw)',
                  texname = '\\text{KTcRz}')

KTtLw = Parameter(name = 'KTtLw',
                  nature = 'internal',
                  type = 'real',
                  value = '(ee*cmath.sqrt((xitpw*zetaTtL)/gamma0tpw))/(sw*cmath.sqrt(2))',
                  texname = '\\text{KTtLw}')

KTtLz = Parameter(name = 'KTtLz',
                  nature = 'internal',
                  type = 'real',
                  value = '(ee*cmath.sqrt((xitpz*zetaTtL)/gamma0tpz))/(2.*cw*sw)',
                  texname = '\\text{KTtLz}')

KTtRw = Parameter(name = 'KTtRw',
                  nature = 'internal',
                  type = 'real',
                  value = '(ee*cmath.sqrt((xitpw*zetaTtR)/gamma0tpw))/(sw*cmath.sqrt(2))',
                  texname = '\\text{KTtRw}')

KTtRz = Parameter(name = 'KTtRz',
                  nature = 'internal',
                  type = 'real',
                  value = '(ee*cmath.sqrt((xitpz*zetaTtR)/gamma0tpz))/(2.*cw*sw)',
                  texname = '\\text{KTtRz}')

KTuLw = Parameter(name = 'KTuLw',
                  nature = 'internal',
                  type = 'real',
                  value = '(ee*cmath.sqrt((xitpw*zetaTuL)/gamma0tpw))/(sw*cmath.sqrt(2))',
                  texname = '\\text{KTuLw}')

KTuLz = Parameter(name = 'KTuLz',
                  nature = 'internal',
                  type = 'real',
                  value = '(ee*cmath.sqrt((xitpz*zetaTuL)/gamma0tpz))/(2.*cw*sw)',
                  texname = '\\text{KTuLz}')

KTuRw = Parameter(name = 'KTuRw',
                  nature = 'internal',
                  type = 'real',
                  value = '(ee*cmath.sqrt((xitpw*zetaTuR)/gamma0tpw))/(sw*cmath.sqrt(2))',
                  texname = '\\text{KTuRw}')

KTuRz = Parameter(name = 'KTuRz',
                  nature = 'internal',
                  type = 'real',
                  value = '(ee*cmath.sqrt((xitpz*zetaTuR)/gamma0tpz))/(2.*cw*sw)',
                  texname = '\\text{KTuRz}')

KXcL = Parameter(name = 'KXcL',
                 nature = 'internal',
                 type = 'real',
                 value = '(ee*cmath.sqrt(zetaXcL/gamma0xw))/(sw*cmath.sqrt(2))',
                 texname = '\\text{KXcL}')

KXcR = Parameter(name = 'KXcR',
                 nature = 'internal',
                 type = 'real',
                 value = '(ee*cmath.sqrt(zetaXcR/gamma0xw))/(sw*cmath.sqrt(2))',
                 texname = '\\text{KXcR}')

KXtL = Parameter(name = 'KXtL',
                 nature = 'internal',
                 type = 'real',
                 value = '(ee*cmath.sqrt(zetaXtL/gamma0xw))/(sw*cmath.sqrt(2))',
                 texname = '\\text{KXtL}')

KXtR = Parameter(name = 'KXtR',
                 nature = 'internal',
                 type = 'real',
                 value = '(ee*cmath.sqrt(zetaXtR/gamma0xw))/(sw*cmath.sqrt(2))',
                 texname = '\\text{KXtR}')

KXuL = Parameter(name = 'KXuL',
                 nature = 'internal',
                 type = 'real',
                 value = '(ee*cmath.sqrt(zetaXuL/gamma0xw))/(sw*cmath.sqrt(2))',
                 texname = '\\text{KXuL}')

KXuR = Parameter(name = 'KXuR',
                 nature = 'internal',
                 type = 'real',
                 value = '(ee*cmath.sqrt(zetaXuR/gamma0xw))/(sw*cmath.sqrt(2))',
                 texname = '\\text{KXuR}')

KYbL = Parameter(name = 'KYbL',
                 nature = 'internal',
                 type = 'real',
                 value = '(ee*cmath.sqrt(zetaYbL/gamma0yw))/(sw*cmath.sqrt(2))',
                 texname = '\\text{KYbL}')

KYbR = Parameter(name = 'KYbR',
                 nature = 'internal',
                 type = 'real',
                 value = '(ee*cmath.sqrt(zetaYbR/gamma0yw))/(sw*cmath.sqrt(2))',
                 texname = '\\text{KYbR}')

KYdL = Parameter(name = 'KYdL',
                 nature = 'internal',
                 type = 'real',
                 value = '(ee*cmath.sqrt(zetaYdL/gamma0yw))/(sw*cmath.sqrt(2))',
                 texname = '\\text{KYdL}')

KYdR = Parameter(name = 'KYdR',
                 nature = 'internal',
                 type = 'real',
                 value = '(ee*cmath.sqrt(zetaYdR/gamma0yw))/(sw*cmath.sqrt(2))',
                 texname = '\\text{KYdR}')

KYsL = Parameter(name = 'KYsL',
                 nature = 'internal',
                 type = 'real',
                 value = '(ee*cmath.sqrt(zetaYsL/gamma0yw))/(sw*cmath.sqrt(2))',
                 texname = '\\text{KYsL}')

KYsR = Parameter(name = 'KYsR',
                 nature = 'internal',
                 type = 'real',
                 value = '(ee*cmath.sqrt(zetaYsR/gamma0yw))/(sw*cmath.sqrt(2))',
                 texname = '\\text{KYsR}')

v = Parameter(name = 'v',
              nature = 'internal',
              type = 'real',
              value = '(2*MW*sw)/ee',
              texname = 'v')

KBbLw = Parameter(name = 'KBbLw',
                  nature = 'internal',
                  type = 'real',
                  value = '(gw*cmath.sqrt((xibpw*zetaBbL)/gamma0bpw))/cmath.sqrt(2)',
                  texname = '\\text{KBbLw}')

KBbLz = Parameter(name = 'KBbLz',
                  nature = 'internal',
                  type = 'real',
                  value = '(gw*cmath.sqrt((xibpz*zetaBbL)/gamma0bpz))/(2.*cw)',
                  texname = '\\text{KBbLz}')

KBbRw = Parameter(name = 'KBbRw',
                  nature = 'internal',
                  type = 'real',
                  value = '(gw*cmath.sqrt((xibpw*zetaBbR)/gamma0bpw))/cmath.sqrt(2)',
                  texname = '\\text{KBbRw}')

KBbRz = Parameter(name = 'KBbRz',
                  nature = 'internal',
                  type = 'real',
                  value = '(gw*cmath.sqrt((xibpz*zetaBbR)/gamma0bpz))/(2.*cw)',
                  texname = '\\text{KBbRz}')

KBdLz = Parameter(name = 'KBdLz',
                  nature = 'internal',
                  type = 'real',
                  value = '(gw*cmath.sqrt((xibpz*zetaBdL)/gamma0bpz))/(2.*cw)',
                  texname = '\\text{KBdLz}')

KBdRz = Parameter(name = 'KBdRz',
                  nature = 'internal',
                  type = 'real',
                  value = '(gw*cmath.sqrt((xibpz*zetaBdR)/gamma0bpz))/(2.*cw)',
                  texname = '\\text{KBdRz}')

KBsLz = Parameter(name = 'KBsLz',
                  nature = 'internal',
                  type = 'real',
                  value = '(gw*cmath.sqrt((xibpz*zetaBsL)/gamma0bpz))/(2.*cw)',
                  texname = '\\text{KBsLz}')

KBsRw = Parameter(name = 'KBsRw',
                  nature = 'internal',
                  type = 'real',
                  value = '(gw*cmath.sqrt((xibpw*zetaBsR)/gamma0bpw))/cmath.sqrt(2)',
                  texname = '\\text{KBsRw}')

KBsRz = Parameter(name = 'KBsRz',
                  nature = 'internal',
                  type = 'real',
                  value = '(gw*cmath.sqrt((xibpz*zetaBsR)/gamma0bpz))/(2.*cw)',
                  texname = '\\text{KBsRz}')

lam = Parameter(name = 'lam',
                nature = 'internal',
                type = 'real',
                value = 'MH**2/(2.*v**2)',
                texname = '\\text{lam}')

yb = Parameter(name = 'yb',
               nature = 'internal',
               type = 'real',
               value = '(ymb*cmath.sqrt(2))/v',
               texname = '\\text{yb}')

yt = Parameter(name = 'yt',
               nature = 'internal',
               type = 'real',
               value = '(ymt*cmath.sqrt(2))/v',
               texname = '\\text{yt}')

ytau = Parameter(name = 'ytau',
                 nature = 'internal',
                 type = 'real',
                 value = '(ymtau*cmath.sqrt(2))/v',
                 texname = '\\text{ytau}')

muH = Parameter(name = 'muH',
                nature = 'internal',
                type = 'real',
                value = 'cmath.sqrt(lam*v**2)',
                texname = '\\mu')


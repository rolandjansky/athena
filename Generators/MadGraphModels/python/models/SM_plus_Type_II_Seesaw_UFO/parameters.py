# This file was automatically created by FeynRules 2.0.8
# Mathematica version: 9.0 for Mac OS X x86 (64-bit) (January 24, 2013)
# Date: Thu 16 Jan 2014 15:28:43



from object_library import all_parameters, Parameter


from function_library import complexconjugate, re, im, csc, sec, acsc, asec, cot

# This is a default parameter object representing 0.
ZERO = Parameter(name = 'ZERO',
                 nature = 'internal',
                 type = 'real',
                 value = '0.0',
                 texname = '0')

# User-defined parameters.
cabi = Parameter(name = 'cabi',
                 nature = 'external',
                 type = 'real',
                 value = 0.227736,
                 texname = '\\theta _c',
                 lhablock = 'CKMBLOCK',
                 lhacode = [ 1 ])

th13 = Parameter(name = 'th13',
                 nature = 'external',
                 type = 'real',
                 value = 0.15707963267948966,
                 texname = '\\theta _{13}',
                 lhablock = 'CKMBLOCK',
                 lhacode = [ 2 ])

th12 = Parameter(name = 'th12',
                 nature = 'external',
                 type = 'real',
                 value = 0.6003932626860493,
                 texname = '\\theta _{12}',
                 lhablock = 'CKMBLOCK',
                 lhacode = [ 3 ])

th23 = Parameter(name = 'th23',
                 nature = 'external',
                 type = 'real',
                 value = 0.7120943348136864,
                 texname = '\\theta _{23}',
                 lhablock = 'CKMBLOCK',
                 lhacode = [ 4 ])

mv2 = Parameter(name = 'mv2',
                nature = 'external',
                type = 'real',
                value = 8.746427842267952e-12,
                texname = '\\text{mv2}',
                lhablock = 'NEWMASSES',
                lhacode = [ 2 ])

mv3 = Parameter(name = 'mv3',
                nature = 'external',
                type = 'real',
                value = 5.049752469181039e-11,
                texname = '\\text{mv3}',
                lhablock = 'NEWMASSES',
                lhacode = [ 3 ])

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
               value = 0.0000116637,
               texname = 'G_f',
               lhablock = 'SMINPUTS',
               lhacode = [ 2 ])

aS = Parameter(name = 'aS',
               nature = 'external',
               type = 'real',
               value = 0.1184,
               texname = '\\alpha _s',
               lhablock = 'SMINPUTS',
               lhacode = [ 3 ])

sThetaW = Parameter(name = 'sThetaW',
                    nature = 'external',
                    type = 'real',
                    value = 0.23146,
                    texname = '\\text{sThetaW}',
                    lhablock = 'SMINPUTS',
                    lhacode = [ 4 ])

vevT = Parameter(name = 'vevT',
                 nature = 'external',
                 type = 'real',
                 value = 1.,
                 texname = 'v_t',
                 lhablock = 'Triplet',
                 lhacode = [ 2 ])

ymdo = Parameter(name = 'ymdo',
                 nature = 'external',
                 type = 'real',
                 value = 0.00504,
                 texname = '\\text{ymdo}',
                 lhablock = 'YUKAWA',
                 lhacode = [ 1 ])

ymup = Parameter(name = 'ymup',
                 nature = 'external',
                 type = 'real',
                 value = 0.00255,
                 texname = '\\text{ymup}',
                 lhablock = 'YUKAWA',
                 lhacode = [ 2 ])

yms = Parameter(name = 'yms',
                nature = 'external',
                type = 'real',
                value = 0.101,
                texname = '\\text{yms}',
                lhablock = 'YUKAWA',
                lhacode = [ 3 ])

ymc = Parameter(name = 'ymc',
                nature = 'external',
                type = 'real',
                value = 1.27,
                texname = '\\text{ymc}',
                lhablock = 'YUKAWA',
                lhacode = [ 4 ])

ymb = Parameter(name = 'ymb',
                nature = 'external',
                type = 'real',
                value = 4.7,
                texname = '\\text{ymb}',
                lhablock = 'YUKAWA',
                lhacode = [ 5 ])

ymt = Parameter(name = 'ymt',
                nature = 'external',
                type = 'real',
                value = 172,
                texname = '\\text{ymt}',
                lhablock = 'YUKAWA',
                lhacode = [ 6 ])

yme = Parameter(name = 'yme',
                nature = 'external',
                type = 'real',
                value = 0.000511,
                texname = '\\text{yme}',
                lhablock = 'YUKAWA',
                lhacode = [ 11 ])

ymm = Parameter(name = 'ymm',
                nature = 'external',
                type = 'real',
                value = 0.10566,
                texname = '\\text{ymm}',
                lhablock = 'YUKAWA',
                lhacode = [ 13 ])

ymtau = Parameter(name = 'ymtau',
                  nature = 'external',
                  type = 'real',
                  value = 1.777,
                  texname = '\\text{ymtau}',
                  lhablock = 'YUKAWA',
                  lhacode = [ 15 ])

tAlpha = Parameter(name = 'tAlpha',
                   nature = 'external',
                   type = 'real',
                   value = 0.5,
                   texname = '\\tan (\\alpha )',
                   lhablock = 'FRBlock',
                   lhacode = [ 1 ])

Me = Parameter(name = 'Me',
               nature = 'external',
               type = 'real',
               value = 0.000511,
               texname = '\\text{Me}',
               lhablock = 'MASS',
               lhacode = [ 11 ])

MM = Parameter(name = 'MM',
               nature = 'external',
               type = 'real',
               value = 0.10566,
               texname = '\\text{MM}',
               lhablock = 'MASS',
               lhacode = [ 13 ])

MTA = Parameter(name = 'MTA',
                nature = 'external',
                type = 'real',
                value = 1.777,
                texname = '\\text{MTA}',
                lhablock = 'MASS',
                lhacode = [ 15 ])

MU = Parameter(name = 'MU',
               nature = 'external',
               type = 'real',
               value = 0.00255,
               texname = 'M',
               lhablock = 'MASS',
               lhacode = [ 2 ])

MC = Parameter(name = 'MC',
               nature = 'external',
               type = 'real',
               value = 1.27,
               texname = '\\text{MC}',
               lhablock = 'MASS',
               lhacode = [ 4 ])

MT = Parameter(name = 'MT',
               nature = 'external',
               type = 'real',
               value = 172,
               texname = '\\text{MT}',
               lhablock = 'MASS',
               lhacode = [ 6 ])

MD = Parameter(name = 'MD',
               nature = 'external',
               type = 'real',
               value = 0.00504,
               texname = '\\text{MD}',
               lhablock = 'MASS',
               lhacode = [ 1 ])

MS = Parameter(name = 'MS',
               nature = 'external',
               type = 'real',
               value = 0.101,
               texname = '\\text{MS}',
               lhablock = 'MASS',
               lhacode = [ 3 ])

MB = Parameter(name = 'MB',
               nature = 'external',
               type = 'real',
               value = 4.7,
               texname = '\\text{MB}',
               lhablock = 'MASS',
               lhacode = [ 5 ])

MH = Parameter(name = 'MH',
               nature = 'external',
               type = 'real',
               value = 125,
               texname = '\\text{MH}',
               lhablock = 'MASS',
               lhacode = [ 25 ])

MH2 = Parameter(name = 'MH2',
                nature = 'external',
                type = 'real',
                value = 400,
                texname = '\\text{MH2}',
                lhablock = 'MASS',
                lhacode = [ 35 ])

MA0 = Parameter(name = 'MA0',
                nature = 'external',
                type = 'real',
                value = 500,
                texname = '\\text{MA0}',
                lhablock = 'MASS',
                lhacode = [ 36 ])

MHP = Parameter(name = 'MHP',
                nature = 'external',
                type = 'real',
                value = 450,
                texname = '\\text{MHP}',
                lhablock = 'MASS',
                lhacode = [ 37 ])

MH2P = Parameter(name = 'MH2P',
                 nature = 'external',
                 type = 'real',
                 value = 200,
                 texname = '\\text{MH2P}',
                 lhablock = 'MASS',
                 lhacode = [ 9900041 ])

WZ = Parameter(name = 'WZ',
               nature = 'external',
               type = 'real',
               value = 2.4952,
               texname = '\\text{WZ}',
               lhablock = 'DECAY',
               lhacode = [ 23 ])

WW = Parameter(name = 'WW',
               nature = 'external',
               type = 'real',
               value = 2.085,
               texname = '\\text{WW}',
               lhablock = 'DECAY',
               lhacode = [ 24 ])

WT = Parameter(name = 'WT',
               nature = 'external',
               type = 'real',
               value = 1.50833649,
               texname = '\\text{WT}',
               lhablock = 'DECAY',
               lhacode = [ 6 ])

WH = Parameter(name = 'WH',
               nature = 'external',
               type = 'real',
               value = 1.,
               texname = '\\text{WH}',
               lhablock = 'DECAY',
               lhacode = [ 25 ])

WH2 = Parameter(name = 'WH2',
                nature = 'external',
                type = 'real',
                value = 1.,
                texname = '\\text{WH2}',
                lhablock = 'DECAY',
                lhacode = [ 35 ])

WA0 = Parameter(name = 'WA0',
                nature = 'external',
                type = 'real',
                value = 1.,
                texname = '\\text{WA0}',
                lhablock = 'DECAY',
                lhacode = [ 36 ])

WHP = Parameter(name = 'WHP',
                nature = 'external',
                type = 'real',
                value = 1.,
                texname = '\\text{WHP}',
                lhablock = 'DECAY',
                lhacode = [ 37 ])

WH2P = Parameter(name = 'WH2P',
                 nature = 'external',
                 type = 'real',
                 value = 1.,
                 texname = '\\text{WH2P}',
                 lhablock = 'DECAY',
                 lhacode = [ 9900041 ])

cAlpha = Parameter(name = 'cAlpha',
                   nature = 'internal',
                   type = 'real',
                   value = 'cmath.sqrt(1/(1 + tAlpha**2))',
                   texname = 'c_{\\alpha }')

CKM1x1 = Parameter(name = 'CKM1x1',
                   nature = 'internal',
                   type = 'complex',
                   value = 'cmath.cos(cabi)',
                   texname = '\\text{CKM1x1}')

CKM1x2 = Parameter(name = 'CKM1x2',
                   nature = 'internal',
                   type = 'complex',
                   value = 'cmath.sin(cabi)',
                   texname = '\\text{CKM1x2}')

CKM2x1 = Parameter(name = 'CKM2x1',
                   nature = 'internal',
                   type = 'complex',
                   value = '-cmath.sin(cabi)',
                   texname = '\\text{CKM2x1}')

CKM2x2 = Parameter(name = 'CKM2x2',
                   nature = 'internal',
                   type = 'complex',
                   value = 'cmath.cos(cabi)',
                   texname = '\\text{CKM2x2}')

CKM3x3 = Parameter(name = 'CKM3x3',
                   nature = 'internal',
                   type = 'complex',
                   value = '1',
                   texname = '\\text{CKM3x3}')

cw = Parameter(name = 'cw',
               nature = 'internal',
               type = 'real',
               value = 'cmath.sqrt(1 - sThetaW)',
               texname = 'c_w')

PMNS1x1 = Parameter(name = 'PMNS1x1',
                    nature = 'internal',
                    type = 'complex',
                    value = 'cmath.cos(th12)*cmath.cos(th13)',
                    texname = '\\text{PMNS1x1}')

PMNS1x2 = Parameter(name = 'PMNS1x2',
                    nature = 'internal',
                    type = 'complex',
                    value = 'cmath.cos(th13)*cmath.sin(th12)',
                    texname = '\\text{PMNS1x2}')

PMNS1x3 = Parameter(name = 'PMNS1x3',
                    nature = 'internal',
                    type = 'complex',
                    value = 'cmath.sin(th13)',
                    texname = '\\text{PMNS1x3}')

PMNS2x1 = Parameter(name = 'PMNS2x1',
                    nature = 'internal',
                    type = 'complex',
                    value = '-(cmath.cos(th23)*cmath.sin(th12)) - cmath.cos(th12)*cmath.sin(th13)*cmath.sin(th23)',
                    texname = '\\text{PMNS2x1}')

PMNS2x2 = Parameter(name = 'PMNS2x2',
                    nature = 'internal',
                    type = 'complex',
                    value = 'cmath.cos(th12)*cmath.cos(th23) - cmath.sin(th12)*cmath.sin(th13)*cmath.sin(th23)',
                    texname = '\\text{PMNS2x2}')

PMNS2x3 = Parameter(name = 'PMNS2x3',
                    nature = 'internal',
                    type = 'complex',
                    value = 'cmath.cos(th13)*cmath.sin(th23)',
                    texname = '\\text{PMNS2x3}')

PMNS3x1 = Parameter(name = 'PMNS3x1',
                    nature = 'internal',
                    type = 'complex',
                    value = '-(cmath.cos(th12)*cmath.cos(th23)*cmath.sin(th13)) + cmath.sin(th12)*cmath.sin(th23)',
                    texname = '\\text{PMNS3x1}')

PMNS3x2 = Parameter(name = 'PMNS3x2',
                    nature = 'internal',
                    type = 'complex',
                    value = '-(cmath.cos(th23)*cmath.sin(th12)*cmath.sin(th13)) - cmath.cos(th12)*cmath.sin(th23)',
                    texname = '\\text{PMNS3x2}')

PMNS3x3 = Parameter(name = 'PMNS3x3',
                    nature = 'internal',
                    type = 'complex',
                    value = 'cmath.cos(th13)*cmath.cos(th23)',
                    texname = '\\text{PMNS3x3}')

sAlpha = Parameter(name = 'sAlpha',
                   nature = 'internal',
                   type = 'real',
                   value = 'tAlpha/cmath.sqrt(1 + tAlpha**2)',
                   texname = 's_{\\alpha }')

sw = Parameter(name = 'sw',
               nature = 'internal',
               type = 'real',
               value = 'cmath.sqrt(sThetaW)',
               texname = 's_w')

aEW = Parameter(name = 'aEW',
                nature = 'internal',
                type = 'real',
                value = '1/aEWM1',
                texname = '\\alpha _{\\text{EW}}')

G = Parameter(name = 'G',
              nature = 'internal',
              type = 'real',
              value = '2*cmath.sqrt(aS)*cmath.sqrt(cmath.pi)',
              texname = 'G')

vev = Parameter(name = 'vev',
                nature = 'internal',
                type = 'real',
                value = 'cmath.sqrt(1/Gf)/2**0.25',
                texname = '\\text{vev}')

MW = Parameter(name = 'MW',
               nature = 'internal',
               type = 'real',
               value = '(cmath.sqrt(cmath.pi)*cmath.sqrt(aEW/(Gf*sThetaW)))/2**0.25',
               texname = 'M_W')

ee = Parameter(name = 'ee',
               nature = 'internal',
               type = 'real',
               value = '2*cmath.sqrt(aEW)*cmath.sqrt(cmath.pi)',
               texname = 'e')

vevD = Parameter(name = 'vevD',
                 nature = 'internal',
                 type = 'real',
                 value = 'cmath.sqrt(vev**2 - 2*vevT**2)',
                 texname = 'v_d')

yneu1x1 = Parameter(name = 'yneu1x1',
                    nature = 'internal',
                    type = 'complex',
                    value = '(mv2*PMNS2x1**2 + mv3*PMNS3x1**2)/vevT',
                    texname = '\\text{yneu1x1}')

yneu1x2 = Parameter(name = 'yneu1x2',
                    nature = 'internal',
                    type = 'complex',
                    value = '(mv2*PMNS2x1*PMNS2x2 + mv3*PMNS3x1*PMNS3x2)/vevT',
                    texname = '\\text{yneu1x2}')

yneu1x3 = Parameter(name = 'yneu1x3',
                    nature = 'internal',
                    type = 'complex',
                    value = '(mv2*PMNS2x1*PMNS2x3 + mv3*PMNS3x1*PMNS3x3)/vevT',
                    texname = '\\text{yneu1x3}')

yneu2x1 = Parameter(name = 'yneu2x1',
                    nature = 'internal',
                    type = 'complex',
                    value = '(mv2*PMNS2x1*PMNS2x2 + mv3*PMNS3x1*PMNS3x2)/vevT',
                    texname = '\\text{yneu2x1}')

yneu2x2 = Parameter(name = 'yneu2x2',
                    nature = 'internal',
                    type = 'complex',
                    value = '(mv2*PMNS2x2**2 + mv3*PMNS3x2**2)/vevT',
                    texname = '\\text{yneu2x2}')

yneu2x3 = Parameter(name = 'yneu2x3',
                    nature = 'internal',
                    type = 'complex',
                    value = '(mv2*PMNS2x2*PMNS2x3 + mv3*PMNS3x2*PMNS3x3)/vevT',
                    texname = '\\text{yneu2x3}')

yneu3x1 = Parameter(name = 'yneu3x1',
                    nature = 'internal',
                    type = 'complex',
                    value = '(mv2*PMNS2x1*PMNS2x3 + mv3*PMNS3x1*PMNS3x3)/vevT',
                    texname = '\\text{yneu3x1}')

yneu3x2 = Parameter(name = 'yneu3x2',
                    nature = 'internal',
                    type = 'complex',
                    value = '(mv2*PMNS2x2*PMNS2x3 + mv3*PMNS3x2*PMNS3x3)/vevT',
                    texname = '\\text{yneu3x2}')

yneu3x3 = Parameter(name = 'yneu3x3',
                    nature = 'internal',
                    type = 'complex',
                    value = '(mv2*PMNS2x3**2 + mv3*PMNS3x3**2)/vevT',
                    texname = '\\text{yneu3x3}')

MZ = Parameter(name = 'MZ',
               nature = 'internal',
               type = 'real',
               value = '(ee*cmath.sqrt(vevD**2 + 4*vevT**2))/(2.*cw*sw)',
               texname = 'M_Z')

tBetaP = Parameter(name = 'tBetaP',
                   nature = 'internal',
                   type = 'real',
                   value = '(vevT*cmath.sqrt(2))/vevD',
                   texname = '\\tan (\\text{$\\beta $P})')

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

lam = Parameter(name = 'lam',
                nature = 'internal',
                type = 'real',
                value = '(2*(cAlpha**2*MH**2 + MH2**2*sAlpha**2))/vevD**2',
                texname = '\\lambda')

lam1 = Parameter(name = 'lam1',
                 nature = 'internal',
                 type = 'real',
                 value = '(cAlpha*(MH**2 - MH2**2)*sAlpha)/(vevD*vevT) + (4*MHP**2)/(vevD**2 + 2*vevT**2) - (2*MA0**2)/(vevD**2 + 4*vevT**2)',
                 texname = '\\lambda _1')

lam2 = Parameter(name = 'lam2',
                 nature = 'internal',
                 type = 'real',
                 value = '(MH2P**2 + (cAlpha**2*MH2**2 + MH**2*sAlpha**2)/2. - (2*MHP**2*vevD**2)/(vevD**2 + 2*vevT**2) + (MA0**2*vevD**2)/(2.*(vevD**2 + 4*vevT**2)))/vevT**2',
                 texname = '\\lambda _2')

lam3 = Parameter(name = 'lam3',
                 nature = 'internal',
                 type = 'real',
                 value = '(-MH2P**2 + (2*MHP**2*vevD**2)/(vevD**2 + 2*vevT**2) - (MA0**2*vevD**2)/(vevD**2 + 4*vevT**2))/vevT**2',
                 texname = '\\lambda _3')

lam4 = Parameter(name = 'lam4',
                 nature = 'internal',
                 type = 'real',
                 value = '(-4*MHP**2)/(vevD**2 + 2*vevT**2) + (4*MA0**2)/(vevD**2 + 4*vevT**2)',
                 texname = '\\lambda _4')

muT = Parameter(name = 'muT',
                nature = 'internal',
                type = 'real',
                value = '(MA0**2*vevT*cmath.sqrt(2))/(vevD**2 + 4*vevT**2)',
                texname = '\\mu')

yb = Parameter(name = 'yb',
               nature = 'internal',
               type = 'real',
               value = '(ymb*cmath.sqrt(2))/vevD',
               texname = '\\text{yb}')

yc = Parameter(name = 'yc',
               nature = 'internal',
               type = 'real',
               value = '(ymc*cmath.sqrt(2))/vevD',
               texname = '\\text{yc}')

ydo = Parameter(name = 'ydo',
                nature = 'internal',
                type = 'real',
                value = '(ymdo*cmath.sqrt(2))/vevD',
                texname = '\\text{ydo}')

ye = Parameter(name = 'ye',
               nature = 'internal',
               type = 'real',
               value = '(yme*cmath.sqrt(2))/vevD',
               texname = '\\text{ye}')

ym = Parameter(name = 'ym',
               nature = 'internal',
               type = 'real',
               value = '(ymm*cmath.sqrt(2))/vevD',
               texname = '\\text{ym}')

ys = Parameter(name = 'ys',
               nature = 'internal',
               type = 'real',
               value = '(yms*cmath.sqrt(2))/vevD',
               texname = '\\text{ys}')

yt = Parameter(name = 'yt',
               nature = 'internal',
               type = 'real',
               value = '(ymt*cmath.sqrt(2))/vevD',
               texname = '\\text{yt}')

ytau = Parameter(name = 'ytau',
                 nature = 'internal',
                 type = 'real',
                 value = '(ymtau*cmath.sqrt(2))/vevD',
                 texname = '\\text{ytau}')

yup = Parameter(name = 'yup',
                nature = 'internal',
                type = 'real',
                value = '(ymup*cmath.sqrt(2))/vevD',
                texname = '\\text{yup}')

cBetaP = Parameter(name = 'cBetaP',
                   nature = 'internal',
                   type = 'real',
                   value = 'cmath.sqrt(1/(1 + tBetaP**2))',
                   texname = 'c_{\\beta _P}')

MassT2 = Parameter(name = 'MassT2',
                   nature = 'internal',
                   type = 'real',
                   value = '(2*muT*vevD**2 - (lam1 + lam4)*vevD**2*vevT*cmath.sqrt(2) - 2*(lam2 + lam3)*vevT**3*cmath.sqrt(2))/(2.*vevT*cmath.sqrt(2))',
                   texname = 'M_{\\Delta }{}^2')

muH2 = Parameter(name = 'muH2',
                 nature = 'internal',
                 type = 'real',
                 value = '(lam*vevD**2)/4. + ((lam1 + lam4)*vevT**2)/2. - muT*vevT*cmath.sqrt(2)',
                 texname = 'm_H{}^2')

sBetaP = Parameter(name = 'sBetaP',
                   nature = 'internal',
                   type = 'real',
                   value = 'cmath.sqrt(tBetaP**2/(1 + tBetaP**2))',
                   texname = 's_{\\beta _P}')

tBeta = Parameter(name = 'tBeta',
                  nature = 'internal',
                  type = 'real',
                  value = 'tBetaP*cmath.sqrt(2)',
                  texname = '\\tan (\\beta )')

cBeta = Parameter(name = 'cBeta',
                  nature = 'internal',
                  type = 'real',
                  value = 'cmath.sqrt(1/(1 + tBeta**2))',
                  texname = 'c_{\\beta }')

sBeta = Parameter(name = 'sBeta',
                  nature = 'internal',
                  type = 'real',
                  value = 'cmath.sqrt(tBeta**2/(1 + tBeta**2))',
                  texname = 's_{\\beta }')

GH1 = Parameter(name = 'GH1',
                nature = 'internal',
                type = 'real',
                value = '-(cAlpha*G**2*(1 + (13*MH**6)/(16800.*MT**6) + MH**4/(168.*MT**4) + (7*MH**2)/(120.*MT**2)))/(12.*cBetaP*cmath.pi**2*vev)',
                texname = 'G_{\\text{H1}}')

GH2 = Parameter(name = 'GH2',
                nature = 'internal',
                type = 'real',
                value = '-(G**2*(1 + (13*MH2**6)/(16800.*MT**6) + MH2**4/(168.*MT**4) + (7*MH2**2)/(120.*MT**2))*sAlpha)/(12.*cBetaP*cmath.pi**2*vev)',
                texname = 'G_{\\text{H2}}')

GA0 = Parameter(name = 'GA0',
                nature = 'internal',
                type = 'real',
                value = '-(G**2*(1 + MA0**6/(560.*MT**6) + MA0**4/(90.*MT**4) + MA0**2/(12.*MT**2))*sBeta)/(8.*cBetaP*cmath.pi**2*vev)',
                texname = 'G_{\\text{A0}}')

I1a11 = Parameter(name = 'I1a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ydo*complexconjugate(CKM1x1)',
                  texname = '\\text{I1a11}')

I1a12 = Parameter(name = 'I1a12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ydo*complexconjugate(CKM2x1)',
                  texname = '\\text{I1a12}')

I1a21 = Parameter(name = 'I1a21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ys*complexconjugate(CKM1x2)',
                  texname = '\\text{I1a21}')

I1a22 = Parameter(name = 'I1a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ys*complexconjugate(CKM2x2)',
                  texname = '\\text{I1a22}')

I1a33 = Parameter(name = 'I1a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yb*complexconjugate(CKM3x3)',
                  texname = '\\text{I1a33}')

I2a11 = Parameter(name = 'I2a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yup*complexconjugate(CKM1x1)',
                  texname = '\\text{I2a11}')

I2a12 = Parameter(name = 'I2a12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yc*complexconjugate(CKM2x1)',
                  texname = '\\text{I2a12}')

I2a21 = Parameter(name = 'I2a21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yup*complexconjugate(CKM1x2)',
                  texname = '\\text{I2a21}')

I2a22 = Parameter(name = 'I2a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yc*complexconjugate(CKM2x2)',
                  texname = '\\text{I2a22}')

I2a33 = Parameter(name = 'I2a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yt*complexconjugate(CKM3x3)',
                  texname = '\\text{I2a33}')

I3a11 = Parameter(name = 'I3a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ye*complexconjugate(PMNS1x1)',
                  texname = '\\text{I3a11}')

I3a12 = Parameter(name = 'I3a12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ye*complexconjugate(PMNS2x1)',
                  texname = '\\text{I3a12}')

I3a13 = Parameter(name = 'I3a13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ye*complexconjugate(PMNS3x1)',
                  texname = '\\text{I3a13}')

I3a21 = Parameter(name = 'I3a21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ym*complexconjugate(PMNS1x2)',
                  texname = '\\text{I3a21}')

I3a22 = Parameter(name = 'I3a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ym*complexconjugate(PMNS2x2)',
                  texname = '\\text{I3a22}')

I3a23 = Parameter(name = 'I3a23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ym*complexconjugate(PMNS3x2)',
                  texname = '\\text{I3a23}')

I3a31 = Parameter(name = 'I3a31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ytau*complexconjugate(PMNS1x3)',
                  texname = '\\text{I3a31}')

I3a32 = Parameter(name = 'I3a32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ytau*complexconjugate(PMNS2x3)',
                  texname = '\\text{I3a32}')

I3a33 = Parameter(name = 'I3a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ytau*complexconjugate(PMNS3x3)',
                  texname = '\\text{I3a33}')

I4a11 = Parameter(name = 'I4a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM1x1*yup',
                  texname = '\\text{I4a11}')

I4a12 = Parameter(name = 'I4a12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM1x2*yup',
                  texname = '\\text{I4a12}')

I4a21 = Parameter(name = 'I4a21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM2x1*yc',
                  texname = '\\text{I4a21}')

I4a22 = Parameter(name = 'I4a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM2x2*yc',
                  texname = '\\text{I4a22}')

I4a33 = Parameter(name = 'I4a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM3x3*yt',
                  texname = '\\text{I4a33}')

I5a11 = Parameter(name = 'I5a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM1x1*ydo',
                  texname = '\\text{I5a11}')

I5a12 = Parameter(name = 'I5a12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM1x2*ys',
                  texname = '\\text{I5a12}')

I5a21 = Parameter(name = 'I5a21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM2x1*ydo',
                  texname = '\\text{I5a21}')

I5a22 = Parameter(name = 'I5a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM2x2*ys',
                  texname = '\\text{I5a22}')

I5a33 = Parameter(name = 'I5a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM3x3*yb',
                  texname = '\\text{I5a33}')

I6a11 = Parameter(name = 'I6a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'PMNS1x1*ye',
                  texname = '\\text{I6a11}')

I6a12 = Parameter(name = 'I6a12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'PMNS1x2*ym',
                  texname = '\\text{I6a12}')

I6a13 = Parameter(name = 'I6a13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'PMNS1x3*ytau',
                  texname = '\\text{I6a13}')

I6a21 = Parameter(name = 'I6a21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'PMNS2x1*ye',
                  texname = '\\text{I6a21}')

I6a22 = Parameter(name = 'I6a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'PMNS2x2*ym',
                  texname = '\\text{I6a22}')

I6a23 = Parameter(name = 'I6a23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'PMNS2x3*ytau',
                  texname = '\\text{I6a23}')

I6a31 = Parameter(name = 'I6a31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'PMNS3x1*ye',
                  texname = '\\text{I6a31}')

I6a32 = Parameter(name = 'I6a32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'PMNS3x2*ym',
                  texname = '\\text{I6a32}')

I6a33 = Parameter(name = 'I6a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'PMNS3x3*ytau',
                  texname = '\\text{I6a33}')


# This file was automatically created by FeynRules 2.0.17
# Mathematica version: 8.0 for Mac OS X x86 (64-bit) (November 6, 2010)
# Date: Wed 10 Dec 2014 14:05:51



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

lam2 = Parameter(name = 'lam2',
                 nature = 'external',
                 type = 'real',
                 value = 0.1,
                 texname = '\\lambda _2',
                 lhablock = 'POTENTIALPARAM',
                 lhacode = [ 1 ])

lam3 = Parameter(name = 'lam3',
                 nature = 'external',
                 type = 'real',
                 value = 0.1,
                 texname = '\\lambda _3',
                 lhablock = 'POTENTIALPARAM',
                 lhacode = [ 2 ])

lam4 = Parameter(name = 'lam4',
                 nature = 'external',
                 type = 'real',
                 value = 0.1,
                 texname = '\\lambda _4',
                 lhablock = 'POTENTIALPARAM',
                 lhacode = [ 3 ])

lam5 = Parameter(name = 'lam5',
                 nature = 'external',
                 type = 'real',
                 value = 0.1,
                 texname = '\\lambda _5',
                 lhablock = 'POTENTIALPARAM',
                 lhacode = [ 4 ])

M1coeff = Parameter(name = 'M1coeff',
                    nature = 'external',
                    type = 'real',
                    value = 100,
                    texname = 'M_1',
                    lhablock = 'POTENTIALPARAM',
                    lhacode = [ 5 ])

M2coeff = Parameter(name = 'M2coeff',
                    nature = 'external',
                    type = 'real',
                    value = 100,
                    texname = 'M_2',
                    lhablock = 'POTENTIALPARAM',
                    lhacode = [ 6 ])

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
               texname = '\\text{aS}',
               lhablock = 'SMINPUTS',
               lhacode = [ 3 ])

tanth = Parameter(name = 'tanth',
                  nature = 'external',
                  type = 'real',
                  value = 0.1,
                  texname = 't_H',
                  lhablock = 'VEV',
                  lhacode = [ 1 ])

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
                 value = 0.0025499999999999997,
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
                value = 172.,
                texname = '\\text{ymt}',
                lhablock = 'YUKAWA',
                lhacode = [ 6 ])

yme = Parameter(name = 'yme',
                nature = 'external',
                type = 'real',
                value = 0.0005110000000000001,
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

Me = Parameter(name = 'Me',
               nature = 'external',
               type = 'real',
               value = 0.0005110000000000001,
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
               value = 0.0025499999999999997,
               texname = 'M',
               lhablock = 'MASS',
               lhacode = [ 2 ])

MC = Parameter(name = 'MC',
               nature = 'external',
               type = 'real',
               value = 1.42,
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

MZ = Parameter(name = 'MZ',
               nature = 'external',
               type = 'real',
               value = 91.1876,
               texname = '\\text{MZ}',
               lhablock = 'MASS',
               lhacode = [ 23 ])

Mh = Parameter(name = 'Mh',
               nature = 'external',
               type = 'real',
               value = 125,
               texname = '\\text{Mh}',
               lhablock = 'MASS',
               lhacode = [ 25 ])

WT = Parameter(name = 'WT',
               nature = 'external',
               type = 'real',
               value = 1.50833649,
               texname = '\\text{WT}',
               lhablock = 'DECAY',
               lhacode = [ 6 ])

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

Wh = Parameter(name = 'Wh',
               nature = 'external',
               type = 'real',
               value = 0.00575308848,
               texname = '\\text{Wh}',
               lhablock = 'DECAY',
               lhacode = [ 25 ])

WH = Parameter(name = 'WH',
               nature = 'external',
               type = 'real',
               value = 1,
               texname = '\\text{WH}',
               lhablock = 'DECAY',
               lhacode = [ 252 ])

WH3p = Parameter(name = 'WH3p',
                 nature = 'external',
                 type = 'real',
                 value = 1,
                 texname = '\\text{WH3p}',
                 lhablock = 'DECAY',
                 lhacode = [ 253 ])

WH3z = Parameter(name = 'WH3z',
                 nature = 'external',
                 type = 'real',
                 value = 1,
                 texname = '\\text{WH3z}',
                 lhablock = 'DECAY',
                 lhacode = [ 254 ])

WH5pp = Parameter(name = 'WH5pp',
                  nature = 'external',
                  type = 'real',
                  value = 1,
                  texname = '\\text{WH5pp}',
                  lhablock = 'DECAY',
                  lhacode = [ 255 ])

WH5p = Parameter(name = 'WH5p',
                 nature = 'external',
                 type = 'real',
                 value = 1,
                 texname = '\\text{WH5p}',
                 lhablock = 'DECAY',
                 lhacode = [ 256 ])

WH5z = Parameter(name = 'WH5z',
                 nature = 'external',
                 type = 'real',
                 value = 1,
                 texname = '\\text{WH5z}',
                 lhablock = 'DECAY',
                 lhacode = [ 257 ])

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

v = Parameter(name = 'v',
              nature = 'internal',
              type = 'real',
              value = '1/(2**0.25*cmath.sqrt(Gf))',
              texname = 'v')

sh = Parameter(name = 'sh',
               nature = 'internal',
               type = 'real',
               value = 'tanth/cmath.sqrt(1 + tanth**2)',
               texname = 's_H')

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

CKM1x3 = Parameter(name = 'CKM1x3',
                   nature = 'internal',
                   type = 'complex',
                   value = '0',
                   texname = '\\text{CKM1x3}')

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

CKM2x3 = Parameter(name = 'CKM2x3',
                   nature = 'internal',
                   type = 'complex',
                   value = '0',
                   texname = '\\text{CKM2x3}')

CKM3x1 = Parameter(name = 'CKM3x1',
                   nature = 'internal',
                   type = 'complex',
                   value = '0',
                   texname = '\\text{CKM3x1}')

CKM3x2 = Parameter(name = 'CKM3x2',
                   nature = 'internal',
                   type = 'complex',
                   value = '0',
                   texname = '\\text{CKM3x2}')

CKM3x3 = Parameter(name = 'CKM3x3',
                   nature = 'internal',
                   type = 'complex',
                   value = '1',
                   texname = '\\text{CKM3x3}')

ch = Parameter(name = 'ch',
               nature = 'internal',
               type = 'real',
               value = 'cmath.sqrt(1 - sh**2)',
               texname = 'c_H')

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

vchi = Parameter(name = 'vchi',
                 nature = 'internal',
                 type = 'real',
                 value = 'sh/(2.*2**0.75*cmath.sqrt(Gf))',
                 texname = 'v_{\\chi }')

MH3 = Parameter(name = 'MH3',
                nature = 'internal',
                type = 'real',
                value = 'cmath.sqrt(v**2*(lam5/2. + M1coeff/(4.*vchi)))',
                texname = 'M_3')

sw2 = Parameter(name = 'sw2',
                nature = 'internal',
                type = 'real',
                value = '1 - MW**2/MZ**2',
                texname = '\\text{sw2}')

vphi = Parameter(name = 'vphi',
                 nature = 'internal',
                 type = 'real',
                 value = '(2*vchi*cmath.sqrt(2))/tanth',
                 texname = 'v_{\\phi }')

cw = Parameter(name = 'cw',
               nature = 'internal',
               type = 'real',
               value = 'cmath.sqrt(1 - sw2)',
               texname = 'c_w')

Mat12sq = Parameter(name = 'Mat12sq',
                    nature = 'internal',
                    type = 'real',
                    value = '((-M1coeff + 4*(2*lam2 - lam5)*vchi)*vphi*cmath.sqrt(3))/2.',
                    texname = '\\text{Mat12sq}')

Mat22sq = Parameter(name = 'Mat22sq',
                    nature = 'internal',
                    type = 'real',
                    value = '-6*M2coeff*vchi + 8*(lam3 + 3*lam4)*vchi**2 + (M1coeff*vphi**2)/(4.*vchi)',
                    texname = '\\text{Mat22sq}')

MH5 = Parameter(name = 'MH5',
                nature = 'internal',
                type = 'real',
                value = 'cmath.sqrt(12*M2coeff*vchi + 8*lam3*vchi**2 + (3*lam5*vphi**2)/2. + (M1coeff*vphi**2)/(4.*vchi))',
                texname = 'M_5')

mu3sq = Parameter(name = 'mu3sq',
                  nature = 'internal',
                  type = 'real',
                  value = '6*M2coeff*vchi - 4*(lam3 + 3*lam4)*vchi**2 - (2*lam2 - lam5)*vphi**2 + (M1coeff*vphi**2)/(4.*vchi)',
                  texname = '\\text{mu3sq}')

sw = Parameter(name = 'sw',
               nature = 'internal',
               type = 'real',
               value = 'cmath.sqrt(sw2)',
               texname = 's_w')

yb = Parameter(name = 'yb',
               nature = 'internal',
               type = 'real',
               value = '(ymb*cmath.sqrt(2))/vphi',
               texname = '\\text{yb}')

yc = Parameter(name = 'yc',
               nature = 'internal',
               type = 'real',
               value = '(ymc*cmath.sqrt(2))/vphi',
               texname = '\\text{yc}')

ydo = Parameter(name = 'ydo',
                nature = 'internal',
                type = 'real',
                value = '(ymdo*cmath.sqrt(2))/vphi',
                texname = '\\text{ydo}')

ye = Parameter(name = 'ye',
               nature = 'internal',
               type = 'real',
               value = '(yme*cmath.sqrt(2))/vphi',
               texname = '\\text{ye}')

ym = Parameter(name = 'ym',
               nature = 'internal',
               type = 'real',
               value = '(ymm*cmath.sqrt(2))/vphi',
               texname = '\\text{ym}')

ys = Parameter(name = 'ys',
               nature = 'internal',
               type = 'real',
               value = '(yms*cmath.sqrt(2))/vphi',
               texname = '\\text{ys}')

yt = Parameter(name = 'yt',
               nature = 'internal',
               type = 'real',
               value = '(ymt*cmath.sqrt(2))/vphi',
               texname = '\\text{yt}')

ytau = Parameter(name = 'ytau',
                 nature = 'internal',
                 type = 'real',
                 value = '(ymtau*cmath.sqrt(2))/vphi',
                 texname = '\\text{ytau}')

yup = Parameter(name = 'yup',
                nature = 'internal',
                type = 'real',
                value = '(ymup*cmath.sqrt(2))/vphi',
                texname = '\\text{yup}')

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

lam1 = Parameter(name = 'lam1',
                 nature = 'internal',
                 type = 'real',
                 value = '(Mh**2 + Mat12sq**2/(Mat22sq - Mh**2))/(8.*vphi**2)',
                 texname = '\\lambda _1')

Mat11sq = Parameter(name = 'Mat11sq',
                    nature = 'internal',
                    type = 'real',
                    value = '8*lam1*vphi**2',
                    texname = '\\text{Mat11sq}')

mu2sq = Parameter(name = 'mu2sq',
                  nature = 'internal',
                  type = 'real',
                  value = '(3*M1coeff*vchi)/2. - 3*(2*lam2 - lam5)*vchi**2 - 4*lam1*vphi**2',
                  texname = '\\text{mu2sq}')

MH = Parameter(name = 'MH',
               nature = 'internal',
               type = 'real',
               value = 'cmath.sqrt(Mat11sq + Mat22sq - Mh**2)',
               texname = 'M_H')

sa = Parameter(name = 'sa',
               nature = 'internal',
               type = 'real',
               value = 'cmath.sin(0.5*cmath.asin((2*Mat12sq)/(-Mh**2 + MH**2)))',
               texname = 's_{\\alpha }')

ca = Parameter(name = 'ca',
               nature = 'internal',
               type = 'real',
               value = 'cmath.sqrt(1 - sa**2)',
               texname = 'c_{\\alpha }')


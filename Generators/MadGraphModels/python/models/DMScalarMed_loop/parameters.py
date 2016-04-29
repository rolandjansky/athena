# This file was automatically created by FeynRules 2.1
# Mathematica version: 9.0 for Mac OS X x86 (64-bit) (January 24, 2013)
# Date: Sun 16 Nov 2014 16:58:56



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

# Mchi = Parameter(name = 'Mchi',
#                  nature = 'external',
#                  type = 'real',
#                  value = 1.7,
#                  texname = 'M_{\\chi }',
#                  lhablock = 'DMMASS',
#                  lhacode = [ 1 ])

# Mphi = Parameter(name = 'Mphi',
#                  nature = 'external',
#                  type = 'real',
#                  value = 125.5,
#                  texname = 'M_{\\phi }',
#                  lhablock = 'DMMASS',
#                  lhacode = [ 2 ])

# Wphi = Parameter(name = 'Wphi',
#                  nature = 'external',
#                  type = 'real',
#                  value = 4.99,
#                  texname = 'W_{\\phi }',
#                  lhablock = 'DMMASS',
#                  lhacode = [ 3 ])

gDM = Parameter(name = 'gDM',
                nature = 'external',
                type = 'real',
                value = 1.,
                texname = 'g_{\\chi }',
                lhablock = 'DMMASS',
                lhacode = [ 4 ])

gSM = Parameter(name = 'gSM',
                nature = 'external',
                type = 'real',
                value = 1.,
                texname = 'g_{\\text{SM}}',
                lhablock = 'DMMASS',
                lhacode = [ 5 ])

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

Gphi = Parameter(name = 'Gphi',
                 nature = 'external',
                 type = 'real',
                 value = 0.0000130155,
                 texname = 'G_{\\tau }',
                 lhablock = 'FRBlock',
                 lhacode = [ 1 ])

Fphi = Parameter(name = 'Fphi',
                 nature = 'external',
                 type = 'real',
                 value = 1.146e-6,
                 texname = 'F_{\\tau }',
                 lhablock = 'FRBlock',
                 lhacode = [ 2 ])

MZ = Parameter(name = 'MZ',
               nature = 'external',
               type = 'real',
               value = 91.1876,
               texname = '\\text{MZ}',
               lhablock = 'MASS',
               lhacode = [ 23 ])

Me = Parameter(name = 'Me',
               nature = 'external',
               type = 'real',
               value = 0.000511,
               texname = '\\text{Me}',
               lhablock = 'MASS',
               lhacode = [ 11 ])

MMU = Parameter(name = 'MMU',
                nature = 'external',
                type = 'real',
                value = 0.10566,
                texname = '\\text{MMU}',
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

Mphi = Parameter(name = 'Mphi',
                 nature = 'external',
                 type = 'real',
                 value = 125.5,
                 texname = 'M_{\\phi }',
                 lhablock = 'MASS',
                 lhacode = [ 1000000 ])

Mchi = Parameter(name = 'Mchi',
                 nature = 'external',
                 type = 'real',
                 value = 1.7,
                 texname = 'M_{\\chi }',
                 lhablock = 'MASS',
                 lhacode = [ 1000022 ])

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
               value = 0.00407,
               texname = '\\text{WH}',
               lhablock = 'DECAY',
               lhacode = [ 25 ])

Wphi = Parameter(name = 'Wphi',
                 nature = 'external',
                 type = 'real',
                 value = 4.988,
                 texname = 'W_{\\phi }',
                 lhablock = 'DECAY',
                 lhacode = [ 1000000 ])

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

tau = Parameter(name = 'tau',
                nature = 'internal',
                type = 'real',
                value = '(4*ymt**2)/Mphi**2',
                texname = '\\tau')

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

vev = Parameter(name = 'vev',
                nature = 'internal',
                type = 'real',
                value = '(2*MW*sw)/ee',
                texname = '\\text{vev}')

lam = Parameter(name = 'lam',
                nature = 'internal',
                type = 'real',
                value = 'MH**2/(2.*vev**2)',
                texname = '\\text{lam}')

yb = Parameter(name = 'yb',
               nature = 'internal',
               type = 'real',
               value = '(ymb*cmath.sqrt(2))/vev',
               texname = '\\text{yb}')

yc = Parameter(name = 'yc',
               nature = 'internal',
               type = 'real',
               value = '(ymc*cmath.sqrt(2))/vev',
               texname = '\\text{yc}')

ydo = Parameter(name = 'ydo',
                nature = 'internal',
                type = 'real',
                value = '(ymdo*cmath.sqrt(2))/vev',
                texname = '\\text{ydo}')

ye = Parameter(name = 'ye',
               nature = 'internal',
               type = 'real',
               value = '(yme*cmath.sqrt(2))/vev',
               texname = '\\text{ye}')

ym = Parameter(name = 'ym',
               nature = 'internal',
               type = 'real',
               value = '(ymm*cmath.sqrt(2))/vev',
               texname = '\\text{ym}')

ys = Parameter(name = 'ys',
               nature = 'internal',
               type = 'real',
               value = '(yms*cmath.sqrt(2))/vev',
               texname = '\\text{ys}')

yt = Parameter(name = 'yt',
               nature = 'internal',
               type = 'real',
               value = '(ymt*cmath.sqrt(2))/vev',
               texname = '\\text{yt}')

ytau = Parameter(name = 'ytau',
                 nature = 'internal',
                 type = 'real',
                 value = '(ymtau*cmath.sqrt(2))/vev',
                 texname = '\\text{ytau}')

yup = Parameter(name = 'yup',
                nature = 'internal',
                type = 'real',
                value = '(ymup*cmath.sqrt(2))/vev',
                texname = '\\text{yup}')

muH = Parameter(name = 'muH',
                nature = 'internal',
                type = 'real',
                value = 'cmath.sqrt(lam*vev**2)',
                texname = '\\mu')

I1b11 = Parameter(name = 'I1b11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ydo*complexconjugate(CKM1x1)',
                  texname = '\\text{I1b11}')

I1b12 = Parameter(name = 'I1b12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ydo*complexconjugate(CKM2x1)',
                  texname = '\\text{I1b12}')

I1b13 = Parameter(name = 'I1b13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ydo*complexconjugate(CKM3x1)',
                  texname = '\\text{I1b13}')

I1b21 = Parameter(name = 'I1b21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ys*complexconjugate(CKM1x2)',
                  texname = '\\text{I1b21}')

I1b22 = Parameter(name = 'I1b22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ys*complexconjugate(CKM2x2)',
                  texname = '\\text{I1b22}')

I1b23 = Parameter(name = 'I1b23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ys*complexconjugate(CKM3x2)',
                  texname = '\\text{I1b23}')

I1b31 = Parameter(name = 'I1b31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yb*complexconjugate(CKM1x3)',
                  texname = '\\text{I1b31}')

I1b32 = Parameter(name = 'I1b32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yb*complexconjugate(CKM2x3)',
                  texname = '\\text{I1b32}')

I1b33 = Parameter(name = 'I1b33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yb*complexconjugate(CKM3x3)',
                  texname = '\\text{I1b33}')

I2b11 = Parameter(name = 'I2b11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yup*complexconjugate(CKM1x1)',
                  texname = '\\text{I2b11}')

I2b12 = Parameter(name = 'I2b12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yc*complexconjugate(CKM2x1)',
                  texname = '\\text{I2b12}')

I2b13 = Parameter(name = 'I2b13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yt*complexconjugate(CKM3x1)',
                  texname = '\\text{I2b13}')

I2b21 = Parameter(name = 'I2b21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yup*complexconjugate(CKM1x2)',
                  texname = '\\text{I2b21}')

I2b22 = Parameter(name = 'I2b22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yc*complexconjugate(CKM2x2)',
                  texname = '\\text{I2b22}')

I2b23 = Parameter(name = 'I2b23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yt*complexconjugate(CKM3x2)',
                  texname = '\\text{I2b23}')

I2b31 = Parameter(name = 'I2b31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yup*complexconjugate(CKM1x3)',
                  texname = '\\text{I2b31}')

I2b32 = Parameter(name = 'I2b32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yc*complexconjugate(CKM2x3)',
                  texname = '\\text{I2b32}')

I2b33 = Parameter(name = 'I2b33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yt*complexconjugate(CKM3x3)',
                  texname = '\\text{I2b33}')

I3b11 = Parameter(name = 'I3b11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM1x1*yup',
                  texname = '\\text{I3b11}')

I3b12 = Parameter(name = 'I3b12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM1x2*yup',
                  texname = '\\text{I3b12}')

I3b13 = Parameter(name = 'I3b13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM1x3*yup',
                  texname = '\\text{I3b13}')

I3b21 = Parameter(name = 'I3b21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM2x1*yc',
                  texname = '\\text{I3b21}')

I3b22 = Parameter(name = 'I3b22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM2x2*yc',
                  texname = '\\text{I3b22}')

I3b23 = Parameter(name = 'I3b23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM2x3*yc',
                  texname = '\\text{I3b23}')

I3b31 = Parameter(name = 'I3b31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM3x1*yt',
                  texname = '\\text{I3b31}')

I3b32 = Parameter(name = 'I3b32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM3x2*yt',
                  texname = '\\text{I3b32}')

I3b33 = Parameter(name = 'I3b33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM3x3*yt',
                  texname = '\\text{I3b33}')

I4b11 = Parameter(name = 'I4b11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM1x1*ydo',
                  texname = '\\text{I4b11}')

I4b12 = Parameter(name = 'I4b12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM1x2*ys',
                  texname = '\\text{I4b12}')

I4b13 = Parameter(name = 'I4b13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM1x3*yb',
                  texname = '\\text{I4b13}')

I4b21 = Parameter(name = 'I4b21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM2x1*ydo',
                  texname = '\\text{I4b21}')

I4b22 = Parameter(name = 'I4b22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM2x2*ys',
                  texname = '\\text{I4b22}')

I4b23 = Parameter(name = 'I4b23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM2x3*yb',
                  texname = '\\text{I4b23}')

I4b31 = Parameter(name = 'I4b31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM3x1*ydo',
                  texname = '\\text{I4b31}')

I4b32 = Parameter(name = 'I4b32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM3x2*ys',
                  texname = '\\text{I4b32}')

I4b33 = Parameter(name = 'I4b33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM3x3*yb',
                  texname = '\\text{I4b33}')


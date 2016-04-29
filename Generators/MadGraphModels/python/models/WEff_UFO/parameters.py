# This file was automatically created by FeynRules 1.7.49
# Mathematica version: 7.0 for Linux x86 (64-bit) (February 18, 2009)
# Date: Tue 3 Jul 2012 10:45:16



from object_library import all_parameters, Parameter


from function_library import complexconjugate, re, im, csc, sec, acsc, asec

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

CLl1x1 = Parameter(name = 'CLl1x1',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{CLl1x1}',
                   lhablock = 'CLl',
                   lhacode = [ 1, 1 ])

CLl1x2 = Parameter(name = 'CLl1x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CLl1x2}',
                   lhablock = 'CLl',
                   lhacode = [ 1, 2 ])

CLl1x3 = Parameter(name = 'CLl1x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CLl1x3}',
                   lhablock = 'CLl',
                   lhacode = [ 1, 3 ])

CLl2x1 = Parameter(name = 'CLl2x1',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CLl2x1}',
                   lhablock = 'CLl',
                   lhacode = [ 2, 1 ])

CLl2x2 = Parameter(name = 'CLl2x2',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{CLl2x2}',
                   lhablock = 'CLl',
                   lhacode = [ 2, 2 ])

CLl2x3 = Parameter(name = 'CLl2x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CLl2x3}',
                   lhablock = 'CLl',
                   lhacode = [ 2, 3 ])

CLl3x1 = Parameter(name = 'CLl3x1',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CLl3x1}',
                   lhablock = 'CLl',
                   lhacode = [ 3, 1 ])

CLl3x2 = Parameter(name = 'CLl3x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CLl3x2}',
                   lhablock = 'CLl',
                   lhacode = [ 3, 2 ])

CLl3x3 = Parameter(name = 'CLl3x3',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{CLl3x3}',
                   lhablock = 'CLl',
                   lhacode = [ 3, 3 ])

CLq1x1 = Parameter(name = 'CLq1x1',
                   nature = 'external',
                   type = 'real',
                   value = 0.97418,
                   texname = '\\text{CLq1x1}',
                   lhablock = 'CLq',
                   lhacode = [ 1, 1 ])

CLq1x2 = Parameter(name = 'CLq1x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.225773,
                   texname = '\\text{CLq1x2}',
                   lhablock = 'CLq',
                   lhacode = [ 1, 2 ])

CLq1x3 = Parameter(name = 'CLq1x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CLq1x3}',
                   lhablock = 'CLq',
                   lhacode = [ 1, 3 ])

CLq2x1 = Parameter(name = 'CLq2x1',
                   nature = 'external',
                   type = 'real',
                   value = -0.225773,
                   texname = '\\text{CLq2x1}',
                   lhablock = 'CLq',
                   lhacode = [ 2, 1 ])

CLq2x2 = Parameter(name = 'CLq2x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.97418,
                   texname = '\\text{CLq2x2}',
                   lhablock = 'CLq',
                   lhacode = [ 2, 2 ])

CLq2x3 = Parameter(name = 'CLq2x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CLq2x3}',
                   lhablock = 'CLq',
                   lhacode = [ 2, 3 ])

CLq3x1 = Parameter(name = 'CLq3x1',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CLq3x1}',
                   lhablock = 'CLq',
                   lhacode = [ 3, 1 ])

CLq3x2 = Parameter(name = 'CLq3x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CLq3x2}',
                   lhablock = 'CLq',
                   lhacode = [ 3, 2 ])

CLq3x3 = Parameter(name = 'CLq3x3',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{CLq3x3}',
                   lhablock = 'CLq',
                   lhacode = [ 3, 3 ])

CRl1x1 = Parameter(name = 'CRl1x1',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{CRl1x1}',
                   lhablock = 'CRl',
                   lhacode = [ 1, 1 ])

CRl1x2 = Parameter(name = 'CRl1x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CRl1x2}',
                   lhablock = 'CRl',
                   lhacode = [ 1, 2 ])

CRl1x3 = Parameter(name = 'CRl1x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CRl1x3}',
                   lhablock = 'CRl',
                   lhacode = [ 1, 3 ])

CRl2x1 = Parameter(name = 'CRl2x1',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CRl2x1}',
                   lhablock = 'CRl',
                   lhacode = [ 2, 1 ])

CRl2x2 = Parameter(name = 'CRl2x2',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{CRl2x2}',
                   lhablock = 'CRl',
                   lhacode = [ 2, 2 ])

CRl2x3 = Parameter(name = 'CRl2x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CRl2x3}',
                   lhablock = 'CRl',
                   lhacode = [ 2, 3 ])

CRl3x1 = Parameter(name = 'CRl3x1',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CRl3x1}',
                   lhablock = 'CRl',
                   lhacode = [ 3, 1 ])

CRl3x2 = Parameter(name = 'CRl3x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CRl3x2}',
                   lhablock = 'CRl',
                   lhacode = [ 3, 2 ])

CRl3x3 = Parameter(name = 'CRl3x3',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{CRl3x3}',
                   lhablock = 'CRl',
                   lhacode = [ 3, 3 ])

CRq1x1 = Parameter(name = 'CRq1x1',
                   nature = 'external',
                   type = 'real',
                   value = 0.97418,
                   texname = '\\text{CRq1x1}',
                   lhablock = 'CRq',
                   lhacode = [ 1, 1 ])

CRq1x2 = Parameter(name = 'CRq1x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.225773,
                   texname = '\\text{CRq1x2}',
                   lhablock = 'CRq',
                   lhacode = [ 1, 2 ])

CRq1x3 = Parameter(name = 'CRq1x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CRq1x3}',
                   lhablock = 'CRq',
                   lhacode = [ 1, 3 ])

CRq2x1 = Parameter(name = 'CRq2x1',
                   nature = 'external',
                   type = 'real',
                   value = -0.225773,
                   texname = '\\text{CRq2x1}',
                   lhablock = 'CRq',
                   lhacode = [ 2, 1 ])

CRq2x2 = Parameter(name = 'CRq2x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.97418,
                   texname = '\\text{CRq2x2}',
                   lhablock = 'CRq',
                   lhacode = [ 2, 2 ])

CRq2x3 = Parameter(name = 'CRq2x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CRq2x3}',
                   lhablock = 'CRq',
                   lhacode = [ 2, 3 ])

CRq3x1 = Parameter(name = 'CRq3x1',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{CRq3x1}',
                   lhablock = 'CRq',
                   lhacode = [ 3, 1 ])

CRq3x2 = Parameter(name = 'CRq3x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{CRq3x2}',
                   lhablock = 'CRq',
                   lhacode = [ 3, 2 ])

CRq3x3 = Parameter(name = 'CRq3x3',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{CRq3x3}',
                   lhablock = 'CRq',
                   lhacode = [ 3, 3 ])

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

gL = Parameter(name = 'gL',
               nature = 'external',
               type = 'real',
               value = 0,
               texname = 'g_L',
               lhablock = 'WPCOUP',
               lhacode = [ 1 ])

gR = Parameter(name = 'gR',
               nature = 'external',
               type = 'real',
               value = 0.64839716719502682,
               texname = 'g_R',
               lhablock = 'WPCOUP',
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
                value = 172.5,
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
               value = 172.5,
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
               value = 4.8,
               texname = '\\text{MB}',
               lhablock = 'MASS',
               lhacode = [ 5 ])

MH = Parameter(name = 'MH',
               nature = 'external',
               type = 'real',
               value = 120,
               texname = '\\text{MH}',
               lhablock = 'MASS',
               lhacode = [ 25 ])

MWp = Parameter(name = 'MWp',
                nature = 'external',
                type = 'real',
                value = 500.,
                texname = '\\text{MWp}',
                lhablock = 'MASS',
                lhacode = [ 34 ])

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
               value = 0.00575308848,
               texname = '\\text{WH}',
               lhablock = 'DECAY',
               lhacode = [ 25 ])

WWp = Parameter(name = 'WWp',
                nature = 'external',
                type = 'real',
                value = 12,
                texname = '\\text{WWp}',
                lhablock = 'DECAY',
                lhacode = [ 34 ])

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

### Default settings:
# MW = Parameter(name = 'MW',
#                nature = 'internal',
#                type = 'real',
#                value = 'cmath.sqrt(MZ**2/2. + cmath.sqrt(MZ**4/4. - (aEW*cmath.pi*MZ**2)/(Gf*cmath.sqrt(2))))',
#                texname = 'M_W')

### Redefine MW:
MW = Parameter(name = 'MW',
               nature = 'internal',
               type = 'real',
               value = '80.399',
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
                texname = '\\mu ')


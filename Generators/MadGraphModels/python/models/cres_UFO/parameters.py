# This file was automatically created by FeynRules 2.3.5
# Mathematica version: 10.1.0  for Linux x86 (64-bit) (March 24, 2015)
# Date: Wed 5 Aug 2015 09:52:21



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

Gtheta = Parameter(name = 'Gtheta',
                   nature = 'external',
                   type = 'real',
                   value = 0.7853981633974483,
                   texname = '\\theta _g',
                   lhablock = 'FRBlock',
                   lhacode = [ 1 ])

Gshgg = Parameter(name = 'Gshgg',
                  nature = 'external',
                  type = 'real',
                  value = 0,
                  texname = 'g_{\\text{Sgg}}',
                  lhablock = 'FRBlock',
                  lhacode = [ 2 ])

GghuuL = Parameter(name = 'GghuuL',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = 'g_{\\text{$\\rho $uu}}{}^L',
                   lhablock = 'FRBlock',
                   lhacode = [ 3 ])

GghddL = Parameter(name = 'GghddL',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = 'g_{\\text{$\\rho $dd}}{}^L',
                   lhablock = 'FRBlock',
                   lhacode = [ 4 ])

GghuuR = Parameter(name = 'GghuuR',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = 'g_{\\text{$\\rho $uu}}{}^R',
                   lhablock = 'FRBlock',
                   lhacode = [ 5 ])

GghddR = Parameter(name = 'GghddR',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = 'g_{\\text{$\\rho $dd}}{}^R',
                   lhablock = 'FRBlock',
                   lhacode = [ 6 ])

GghuhuL = Parameter(name = 'GghuhuL',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = 'g_{\\rho  u u_H}{}^L',
                    lhablock = 'FRBlock',
                    lhacode = [ 7 ])

GghdhdL = Parameter(name = 'GghdhdL',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = 'g_{d \\rho  d_H}{}^L',
                    lhablock = 'FRBlock',
                    lhacode = [ 8 ])

GghuhuR = Parameter(name = 'GghuhuR',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = 'g_{\\rho  u u_H}{}^R',
                    lhablock = 'FRBlock',
                    lhacode = [ 9 ])

GghdhdR = Parameter(name = 'GghdhdR',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = 'g_{d \\rho  d_H}{}^R',
                    lhablock = 'FRBlock',
                    lhacode = [ 10 ])

GghuhuhL = Parameter(name = 'GghuhuhL',
                     nature = 'external',
                     type = 'real',
                     value = 0,
                     texname = 'g_{\\rho  u_H^2}{}^L',
                     lhablock = 'FRBlock',
                     lhacode = [ 11 ])

GghdhdhL = Parameter(name = 'GghdhdhL',
                     nature = 'external',
                     type = 'real',
                     value = 0,
                     texname = 'g_{\\rho  d_H^2}{}^L',
                     lhablock = 'FRBlock',
                     lhacode = [ 12 ])

GghuhuhR = Parameter(name = 'GghuhuhR',
                     nature = 'external',
                     type = 'real',
                     value = 0,
                     texname = 'g_{\\rho  u_H^2}{}^R',
                     lhablock = 'FRBlock',
                     lhacode = [ 13 ])

GghdhdhR = Parameter(name = 'GghdhdhR',
                     nature = 'external',
                     type = 'real',
                     value = 0,
                     texname = 'g_{\\rho  d_H^2}{}^R',
                     lhablock = 'FRBlock',
                     lhacode = [ 14 ])

GguhuL = Parameter(name = 'GguhuL',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = 'g_{g u u_H}{}^L',
                   lhablock = 'FRBlock',
                   lhacode = [ 15 ])

GgdhdL = Parameter(name = 'GgdhdL',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = 'g_{d g d_H}{}^L',
                   lhablock = 'FRBlock',
                   lhacode = [ 16 ])

GguhuR = Parameter(name = 'GguhuR',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = 'g_{g u u_H}{}^R',
                   lhablock = 'FRBlock',
                   lhacode = [ 17 ])

GgdhdR = Parameter(name = 'GgdhdR',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = 'g_{d g d_H}{}^R',
                   lhablock = 'FRBlock',
                   lhacode = [ 18 ])

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

MSH = Parameter(name = 'MSH',
                nature = 'external',
                type = 'real',
                value = 2000,
                texname = '\\text{MSH}',
                lhablock = 'MASS',
                lhacode = [ 2001 ])

MUHe = Parameter(name = 'MUHe',
                 nature = 'external',
                 type = 'real',
                 value = 1000,
                 texname = '\\text{MUHe}',
                 lhablock = 'MASS',
                 lhacode = [ 992 ])

MCHe = Parameter(name = 'MCHe',
                 nature = 'external',
                 type = 'real',
                 value = 1000,
                 texname = '\\text{MCHe}',
                 lhablock = 'MASS',
                 lhacode = [ 994 ])

MTHe = Parameter(name = 'MTHe',
                 nature = 'external',
                 type = 'real',
                 value = 1000,
                 texname = '\\text{MTHe}',
                 lhablock = 'MASS',
                 lhacode = [ 996 ])

MDHe = Parameter(name = 'MDHe',
                 nature = 'external',
                 type = 'real',
                 value = 1000,
                 texname = '\\text{MDHe}',
                 lhablock = 'MASS',
                 lhacode = [ 991 ])

MSHe = Parameter(name = 'MSHe',
                 nature = 'external',
                 type = 'real',
                 value = 1000,
                 texname = '\\text{MSHe}',
                 lhablock = 'MASS',
                 lhacode = [ 993 ])

MBHe = Parameter(name = 'MBHe',
                 nature = 'external',
                 type = 'real',
                 value = 1000,
                 texname = '\\text{MBHe}',
                 lhablock = 'MASS',
                 lhacode = [ 995 ])

MGH = Parameter(name = 'MGH',
                nature = 'external',
                type = 'real',
                value = 2000,
                texname = '\\text{MGH}',
                lhablock = 'MASS',
                lhacode = [ 1999 ])

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

WSH = Parameter(name = 'WSH',
                nature = 'external',
                type = 'real',
                value = 100,
                texname = '\\text{WSH}',
                lhablock = 'DECAY',
                lhacode = [ 2001 ])

WUHe = Parameter(name = 'WUHe',
                 nature = 'external',
                 type = 'real',
                 value = 50,
                 texname = '\\text{WUHe}',
                 lhablock = 'DECAY',
                 lhacode = [ 992 ])

WCHe = Parameter(name = 'WCHe',
                 nature = 'external',
                 type = 'real',
                 value = 50,
                 texname = '\\text{WCHe}',
                 lhablock = 'DECAY',
                 lhacode = [ 994 ])

WTHe = Parameter(name = 'WTHe',
                 nature = 'external',
                 type = 'real',
                 value = 50,
                 texname = '\\text{WTHe}',
                 lhablock = 'DECAY',
                 lhacode = [ 996 ])

WDHe = Parameter(name = 'WDHe',
                 nature = 'external',
                 type = 'real',
                 value = 50,
                 texname = '\\text{WDHe}',
                 lhablock = 'DECAY',
                 lhacode = [ 991 ])

WSHe = Parameter(name = 'WSHe',
                 nature = 'external',
                 type = 'real',
                 value = 50,
                 texname = '\\text{WSHe}',
                 lhablock = 'DECAY',
                 lhacode = [ 993 ])

WBHe = Parameter(name = 'WBHe',
                 nature = 'external',
                 type = 'real',
                 value = 50,
                 texname = '\\text{WBHe}',
                 lhablock = 'DECAY',
                 lhacode = [ 995 ])

WGH = Parameter(name = 'WGH',
                nature = 'external',
                type = 'real',
                value = 100,
                texname = '\\text{WGH}',
                lhablock = 'DECAY',
                lhacode = [ 1999 ])

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

I1a13 = Parameter(name = 'I1a13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ydo*complexconjugate(CKM3x1)',
                  texname = '\\text{I1a13}')

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

I1a23 = Parameter(name = 'I1a23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'ys*complexconjugate(CKM3x2)',
                  texname = '\\text{I1a23}')

I1a31 = Parameter(name = 'I1a31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yb*complexconjugate(CKM1x3)',
                  texname = '\\text{I1a31}')

I1a32 = Parameter(name = 'I1a32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yb*complexconjugate(CKM2x3)',
                  texname = '\\text{I1a32}')

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

I2a13 = Parameter(name = 'I2a13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yt*complexconjugate(CKM3x1)',
                  texname = '\\text{I2a13}')

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

I2a23 = Parameter(name = 'I2a23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yt*complexconjugate(CKM3x2)',
                  texname = '\\text{I2a23}')

I2a31 = Parameter(name = 'I2a31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yup*complexconjugate(CKM1x3)',
                  texname = '\\text{I2a31}')

I2a32 = Parameter(name = 'I2a32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yc*complexconjugate(CKM2x3)',
                  texname = '\\text{I2a32}')

I2a33 = Parameter(name = 'I2a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yt*complexconjugate(CKM3x3)',
                  texname = '\\text{I2a33}')

I3a11 = Parameter(name = 'I3a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM1x1*yup',
                  texname = '\\text{I3a11}')

I3a12 = Parameter(name = 'I3a12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM1x2*yup',
                  texname = '\\text{I3a12}')

I3a13 = Parameter(name = 'I3a13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM1x3*yup',
                  texname = '\\text{I3a13}')

I3a21 = Parameter(name = 'I3a21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM2x1*yc',
                  texname = '\\text{I3a21}')

I3a22 = Parameter(name = 'I3a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM2x2*yc',
                  texname = '\\text{I3a22}')

I3a23 = Parameter(name = 'I3a23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM2x3*yc',
                  texname = '\\text{I3a23}')

I3a31 = Parameter(name = 'I3a31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM3x1*yt',
                  texname = '\\text{I3a31}')

I3a32 = Parameter(name = 'I3a32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM3x2*yt',
                  texname = '\\text{I3a32}')

I3a33 = Parameter(name = 'I3a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM3x3*yt',
                  texname = '\\text{I3a33}')

I4a11 = Parameter(name = 'I4a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM1x1*ydo',
                  texname = '\\text{I4a11}')

I4a12 = Parameter(name = 'I4a12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM1x2*ys',
                  texname = '\\text{I4a12}')

I4a13 = Parameter(name = 'I4a13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM1x3*yb',
                  texname = '\\text{I4a13}')

I4a21 = Parameter(name = 'I4a21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM2x1*ydo',
                  texname = '\\text{I4a21}')

I4a22 = Parameter(name = 'I4a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM2x2*ys',
                  texname = '\\text{I4a22}')

I4a23 = Parameter(name = 'I4a23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM2x3*yb',
                  texname = '\\text{I4a23}')

I4a31 = Parameter(name = 'I4a31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM3x1*ydo',
                  texname = '\\text{I4a31}')

I4a32 = Parameter(name = 'I4a32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM3x2*ys',
                  texname = '\\text{I4a32}')

I4a33 = Parameter(name = 'I4a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKM3x3*yb',
                  texname = '\\text{I4a33}')


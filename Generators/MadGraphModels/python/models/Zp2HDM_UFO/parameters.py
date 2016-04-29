# This file was automatically created by FeynRules 2.0.25
# Mathematica version: 9.0 for Mac OS X x86 (64-bit) (January 24, 2013)
# Date: Wed 11 Nov 2015 22:20:11



from object_library import all_parameters, Parameter


from function_library import complexconjugate, re, im, csc, sec, acsc, asec, cot

# This is a default parameter object representing 0.
ZERO = Parameter(name = 'ZERO',
                 nature = 'internal',
                 type = 'real',
                 value = '0.0',
                 texname = '0')

# User-defined parameters.
gx = Parameter(name = 'gx',
               nature = 'external',
               type = 'real',
               value = 1,
               texname = '\\text{gx}',
               lhablock = 'DMINPUTS',
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

ymtau = Parameter(name = 'ymtau',
                  nature = 'external',
                  type = 'real',
                  value = 1.777,
                  texname = '\\text{ymtau}',
                  lhablock = 'YUKAWA',
                  lhacode = [ 15 ])

Tb = Parameter(name = 'Tb',
               nature = 'external',
               type = 'real',
               value = 3.,
               texname = '\\text{Tb}',
               lhablock = 'ZpINPUTS',
               lhacode = [ 1 ])

gz = Parameter(name = 'gz',
               nature = 'external',
               type = 'real',
               value = 0.1,
               texname = '\\text{gz}',
               lhablock = 'ZpINPUTS',
               lhacode = [ 2 ])

MZ = Parameter(name = 'MZ',
               nature = 'external',
               type = 'real',
               value = 91.1876,
               texname = '\\text{MZ}',
               lhablock = 'MASS',
               lhacode = [ 23 ])

MZp = Parameter(name = 'MZp',
                nature = 'external',
                type = 'real',
                value = 1000,
                texname = '\\text{MZp}',
                lhablock = 'MASS',
                lhacode = [ 32 ])

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

MT = Parameter(name = 'MT',
               nature = 'external',
               type = 'real',
               value = 172,
               texname = '\\text{MT}',
               lhablock = 'MASS',
               lhacode = [ 6 ])

MB = Parameter(name = 'MB',
               nature = 'external',
               type = 'real',
               value = 4.7,
               texname = '\\text{MB}',
               lhablock = 'MASS',
               lhacode = [ 5 ])

Mh = Parameter(name = 'Mh',
               nature = 'external',
               type = 'real',
               value = 125,
               texname = '\\text{Mh}',
               lhablock = 'MASS',
               lhacode = [ 25 ])

MH = Parameter(name = 'MH',
               nature = 'external',
               type = 'real',
               value = 300,
               texname = '\\text{MH}',
               lhablock = 'MASS',
               lhacode = [ 26 ])

MHP = Parameter(name = 'MHP',
                nature = 'external',
                type = 'real',
                value = 300,
                texname = '\\text{MHP}',
                lhablock = 'MASS',
                lhacode = [ 27 ])

MA0 = Parameter(name = 'MA0',
                nature = 'external',
                type = 'real',
                value = 300,
                texname = '\\text{MA0}',
                lhablock = 'MASS',
                lhacode = [ 28 ])

MX = Parameter(name = 'MX',
               nature = 'external',
               type = 'real',
               value = 100,
               texname = '\\text{MX}',
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

WZp = Parameter(name = 'WZp',
                nature = 'external',
                type = 'real',
                value = 1,
                texname = '\\text{WZp}',
                lhablock = 'DECAY',
                lhacode = [ 32 ])

WT = Parameter(name = 'WT',
               nature = 'external',
               type = 'real',
               value = 1.50833649,
               texname = '\\text{WT}',
               lhablock = 'DECAY',
               lhacode = [ 6 ])

Wh = Parameter(name = 'Wh',
               nature = 'external',
               type = 'real',
               value = 0.00407,
               texname = '\\text{Wh}',
               lhablock = 'DECAY',
               lhacode = [ 25 ])

WH = Parameter(name = 'WH',
               nature = 'external',
               type = 'real',
               value = 10,
               texname = '\\text{WH}',
               lhablock = 'DECAY',
               lhacode = [ 26 ])

WHP = Parameter(name = 'WHP',
                nature = 'external',
                type = 'real',
                value = 10,
                texname = '\\text{WHP}',
                lhablock = 'DECAY',
                lhacode = [ 27 ])

WA0 = Parameter(name = 'WA0',
                nature = 'external',
                type = 'real',
                value = 10,
                texname = '\\text{WA0}',
                lhablock = 'DECAY',
                lhacode = [ 28 ])

Wphip = Parameter(name = 'Wphip',
                  nature = 'external',
                  type = 'real',
                  value = 10,
                  texname = '\\text{Wphip}',
                  lhablock = 'DECAY',
                  lhacode = [ 29 ])

Cb = Parameter(name = 'Cb',
               nature = 'internal',
               type = 'real',
               value = '1/cmath.sqrt(1 + Tb**2)',
               texname = '\\text{Cb}')

Mphip = Parameter(name = 'Mphip',
                  nature = 'internal',
                  type = 'real',
                  value = '300.',
                  texname = 'M_{\\text{phip}}')

QZPhiD = Parameter(name = 'QZPhiD',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{QZPhiD}')

QZPhiP = Parameter(name = 'QZPhiP',
                   nature = 'internal',
                   type = 'real',
                   value = '0.5',
                   texname = '\\text{QZPhiP}')

QZPhiU = Parameter(name = 'QZPhiU',
                   nature = 'internal',
                   type = 'real',
                   value = '0.5',
                   texname = '\\text{QZPhiU}')

Sa = Parameter(name = 'Sa',
               nature = 'internal',
               type = 'real',
               value = '-(1/cmath.sqrt(1 + Tb**2))',
               texname = '\\text{Sa}')

Sb = Parameter(name = 'Sb',
               nature = 'internal',
               type = 'real',
               value = 'Tb/cmath.sqrt(1 + Tb**2)',
               texname = '\\text{Sb}')

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

Ca = Parameter(name = 'Ca',
               nature = 'internal',
               type = 'real',
               value = 'cmath.sqrt(1 - Sa**2)',
               texname = '\\text{Ca}')

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

vd = Parameter(name = 'vd',
               nature = 'internal',
               type = 'real',
               value = '(2*Cb*MW*sw)/ee',
               texname = '\\text{vd}')

vev = Parameter(name = 'vev',
                nature = 'internal',
                type = 'real',
                value = '(2*MW*sw)/ee',
                texname = '\\text{vev}')

vu = Parameter(name = 'vu',
               nature = 'internal',
               type = 'real',
               value = '(2*MW*Sb*sw)/ee',
               texname = '\\text{vu}')

DeltaM2 = Parameter(name = 'DeltaM2',
                    nature = 'internal',
                    type = 'real',
                    value = '(ee*gz*(QZPhiD*vd**2 + QZPhiU*vu**2))/(2.*cw*sw)',
                    texname = '\\text{DeltaM2}')

vp = Parameter(name = 'vp',
               nature = 'internal',
               type = 'real',
               value = 'cmath.sqrt((MZp**2/gz**2 - QZPhiD**2*vd**2 - QZPhiU**2*vu**2)/QZPhiP**2)',
               texname = '\\text{vp}')

yb = Parameter(name = 'yb',
               nature = 'internal',
               type = 'real',
               value = '(ymb*cmath.sqrt(2))/vd',
               texname = '\\text{yb}')

yt = Parameter(name = 'yt',
               nature = 'internal',
               type = 'real',
               value = '(ymt*cmath.sqrt(2))/vu',
               texname = '\\text{yt}')

ytau = Parameter(name = 'ytau',
                 nature = 'internal',
                 type = 'real',
                 value = '(ymtau*cmath.sqrt(2))/vd',
                 texname = '\\text{ytau}')

Ep = Parameter(name = 'Ep',
               nature = 'internal',
               type = 'real',
               value = 'DeltaM2/(-MZ**2 + MZp**2)',
               texname = '\\text{Ep}')

I1a33 = Parameter(name = 'I1a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yb',
                  texname = '\\text{I1a33}')

I2a33 = Parameter(name = 'I2a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yt',
                  texname = '\\text{I2a33}')

I3a33 = Parameter(name = 'I3a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yt',
                  texname = '\\text{I3a33}')

I4a33 = Parameter(name = 'I4a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yb',
                  texname = '\\text{I4a33}')


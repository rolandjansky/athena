# This file was automatically created by FeynRules =.2.2
# Mathematica version: 9.0 for Linux x86 (64-bit) (February 7, 2013)
# Date: Fri 29 May 2015 17:02:46



from object_library import all_parameters, Parameter


from function_library import complexconjugate, re, im, csc, sec, acsc, asec, cot

# This is a default parameter object representing 0.
ZERO = Parameter(name = 'ZERO',
                 nature = 'internal',
                 type = 'real',
                 value = '0.0',
                 texname = '0')

# User-defined parameters.
lHHRR = Parameter(name = 'lHHRR',
                  nature = 'external',
                  type = 'real',
                  value = 0.,
                  texname = '\\text{lHHRR}',
                  lhablock = 'HHDMinputs',
                  lhacode = [ 1 ])

lRRXX = Parameter(name = 'lRRXX',
                  nature = 'external',
                  type = 'real',
                  value = 0.,
                  texname = '\\text{lRRXX}',
                  lhablock = 'HHDMinputs',
                  lhacode = [ 2 ])

lHHXX = Parameter(name = 'lHHXX',
                  nature = 'external',
                  type = 'real',
                  value = 0.,
                  texname = '\\text{lHHXX}',
                  lhablock = 'HHDMinputs',
                  lhacode = [ 3 ])

lHRXX = Parameter(name = 'lHRXX',
                  nature = 'external',
                  type = 'real',
                  value = 1.,
                  texname = '\\text{lHRXX}',
                  lhablock = 'HHDMinputs',
                  lhacode = [ 4 ])

lHHR = Parameter(name = 'lHHR',
                 nature = 'external',
                 type = 'real',
                 value = 1.,
                 texname = '\\text{lHHR}',
                 lhablock = 'HHDMinputs',
                 lhacode = [ 5 ])

lHXX = Parameter(name = 'lHXX',
                 nature = 'external',
                 type = 'real',
                 value = 1.,
                 texname = '\\text{lHXX}',
                 lhablock = 'HHDMinputs',
                 lhacode = [ 6 ])

lRXX = Parameter(name = 'lRXX',
                 nature = 'external',
                 type = 'real',
                 value = 0.,
                 texname = '\\text{lRXX}',
                 lhablock = 'HHDMinputs',
                 lhacode = [ 7 ])

btg = Parameter(name = 'btg',
                nature = 'external',
                type = 'real',
                value = 1.,
                texname = '\\text{btg}',
                lhablock = 'HHDMinputs',
                lhacode = [ 8 ])

btW = Parameter(name = 'btW',
                nature = 'external',
                type = 'real',
                value = 1.,
                texname = '\\text{btW}',
                lhablock = 'HHDMinputs',
                lhacode = [ 9 ])

btZ = Parameter(name = 'btZ',
                nature = 'external',
                type = 'real',
                value = 0.,
                texname = '\\text{btZ}',
                lhablock = 'HHDMinputs',
                lhacode = [ 10 ])

btb = Parameter(name = 'btb',
                nature = 'external',
                type = 'real',
                value = 1.,
                texname = '\\text{btb}',
                lhablock = 'HHDMinputs',
                lhacode = [ 11 ])

kpg = Parameter(name = 'kpg',
                nature = 'external',
                type = 'real',
                value = 1.,
                texname = '\\text{kpg}',
                lhablock = 'HHDMinputs',
                lhacode = [ 12 ])

kpV = Parameter(name = 'kpV',
                nature = 'external',
                type = 'real',
                value = 1.,
                texname = '\\text{kpV}',
                lhablock = 'HHDMinputs',
                lhacode = [ 13 ])

KfacH = Parameter(name = 'KfacH',
                  nature = 'external',
                  type = 'real',
                  value = 1.,
                  texname = '\\text{KfacH}',
                  lhablock = 'HHDMinputs',
                  lhacode = [ 14 ])

KfacR = Parameter(name = 'KfacR',
                  nature = 'external',
                  type = 'real',
                  value = 1.,
                  texname = '\\text{KfacR}',
                  lhablock = 'HHDMinputs',
                  lhacode = [ 15 ])

tth = Parameter(name = 'tth',
                nature = 'external',
                type = 'real',
                value = 1.,
                texname = '\\text{tth}',
                lhablock = 'HHDMinputs',
                lhacode = [ 16 ])

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

MZ = Parameter(name = 'MZ',
               nature = 'external',
               type = 'real',
               value = 91.1876,
               texname = '\\text{MZ}',
               lhablock = 'MASS',
               lhacode = [ 23 ])

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

MH = Parameter(name = 'MH',
               nature = 'external',
               type = 'real',
               value = 125,
               texname = '\\text{MH}',
               lhablock = 'MASS',
               lhacode = [ 25 ])

MX = Parameter(name = 'MX',
               nature = 'external',
               type = 'real',
               value = 50.,
               texname = '\\text{MX}',
               lhablock = 'MASS',
               lhacode = [ 1000022 ])

MR = Parameter(name = 'MR',
               nature = 'external',
               type = 'real',
               value = 300.,
               texname = '\\text{MR}',
               lhablock = 'MASS',
               lhacode = [ 35 ])

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

WX = Parameter(name = 'WX',
               nature = 'external',
               type = 'real',
               value = 0.,
               texname = '\\text{WX}',
               lhablock = 'DECAY',
               lhacode = [ 1000022 ])

WR = Parameter(name = 'WR',
               nature = 'external',
               type = 'real',
               value = 3.4952,
               texname = '\\text{WR}',
               lhablock = 'DECAY',
               lhacode = [ 35 ])

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

kHaa = Parameter(name = 'kHaa',
                 nature = 'internal',
                 type = 'real',
                 value = '(47*ee**2*(1 - (2*MH**4)/(987.*MT**4) - (14*MH**2)/(705.*MT**2) + (213*MH**12)/(2.634632e7*MW**12) + (5*MH**10)/(119756.*MW**10) + (41*MH**8)/(180950.*MW**8) + (87*MH**6)/(65800.*MW**6) + (57*MH**4)/(6580.*MW**4) + (33*MH**2)/(470.*MW**2)))/(72.*cmath.pi**2*vev)',
                 texname = 'A_H')

kHgg = Parameter(name = 'kHgg',
                 nature = 'internal',
                 type = 'real',
                 value = '-(aS*kpg*(-1.3333333333333333 - MH**12/(229320.*MT**12) - (19*MH**10)/(756756.*MT**10) - (8*MH**8)/(51975.*MT**8) - (13*MH**6)/(12600.*MT**6) - MH**4/(126.*MT**4) - (7*MH**2)/(90.*MT**2))*cmath.sqrt(KfacH))/(4.*cmath.pi*vev)',
                 texname = 'k_{\\text{Hgg}}')

kHZa = Parameter(name = 'kHZa',
                 nature = 'internal',
                 type = 'real',
                 value = '(-1.8061385181545853*aEW)/vev',
                 texname = 'k_{\\text{HZa}}')

kRgg = Parameter(name = 'kRgg',
                 nature = 'internal',
                 type = 'real',
                 value = '-(aS*btg*(-1.3333333333333333 - MR**12/(229320.*MT**12) - (19*MR**10)/(756756.*MT**10) - (8*MR**8)/(51975.*MT**8) - (13*MR**6)/(12600.*MT**6) - MR**4/(126.*MT**4) - (7*MR**2)/(90.*MT**2))*cmath.sqrt(KfacR))/(4.*cmath.pi*vev)',
                 texname = 'k_{\\text{Rgg}}')

lam = Parameter(name = 'lam',
                nature = 'internal',
                type = 'real',
                value = 'MH**2/(2.*vev**2)',
                texname = '\\text{lam}')

muHHR = Parameter(name = 'muHHR',
                  nature = 'internal',
                  type = 'real',
                  value = 'lHHR*vev',
                  texname = '\\mu _{\\text{hhH}}')

muHXX = Parameter(name = 'muHXX',
                  nature = 'internal',
                  type = 'real',
                  value = 'lHXX*vev',
                  texname = '\\mu _{\\text{hXX}}')

muRXX = Parameter(name = 'muRXX',
                  nature = 'internal',
                  type = 'real',
                  value = 'lRXX*vev',
                  texname = '\\mu _{\\text{HXX}}')

yb = Parameter(name = 'yb',
               nature = 'internal',
               type = 'real',
               value = '(ymb*cmath.sqrt(2))/vev',
               texname = '\\text{yb}')

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

muH = Parameter(name = 'muH',
                nature = 'internal',
                type = 'real',
                value = 'cmath.sqrt(lam*vev**2)',
                texname = '\\mu')

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


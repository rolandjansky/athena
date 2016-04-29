# This file was automatically created by FeynRules 2.0.25
# Mathematica version: 9.0 for Mac OS X x86 (64-bit) (January 24, 2013)
# Date: Tue 19 Aug 2014 17:54:53



from object_library import all_parameters, Parameter


from function_library import complexconjugate, re, im, csc, sec, acsc, asec, cot

# This is a default parameter object representing 0.
ZERO = Parameter(name = 'ZERO',
                 nature = 'internal',
                 type = 'real',
                 value = '0.0',
                 texname = '0')

# User-defined parameters.
gz = Parameter(name = 'gz',
               nature = 'external',
               type = 'real',
               value = 0.5,
               texname = '\\text{gz}',
               lhablock = 'DMINPUTS',
               lhacode = [ 1 ])

gzu = Parameter(name = 'gzu',
                nature = 'external',
                type = 'real',
                value = 0.1,
                texname = '\\text{gzu}',
                lhablock = 'DMINPUTS',
                lhacode = [ 2 ])

gzd = Parameter(name = 'gzd',
                nature = 'external',
                type = 'real',
                value = 0.1,
                texname = '\\text{gzd}',
                lhablock = 'DMINPUTS',
                lhacode = [ 3 ])

gzl = Parameter(name = 'gzl',
                nature = 'external',
                type = 'real',
                value = 0,
                texname = '\\text{gzl}',
                lhablock = 'DMINPUTS',
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

MH = Parameter(name = 'MH',
               nature = 'external',
               type = 'real',
               value = 120,
               texname = '\\text{MH}',
               lhablock = 'MASS',
               lhacode = [ 25 ])

MHD = Parameter(name = 'MHD',
                nature = 'external',
                type = 'real',
                value = 200,
                texname = '\\text{MHD}',
                lhablock = 'MASS',
                lhacode = [ 26 ])

Mphix = Parameter(name = 'Mphix',
                  nature = 'external',
                  type = 'real',
                  value = 50,
                  texname = '\\text{Mphix}',
                  lhablock = 'MASS',
                  lhacode = [ 1000022 ])

MZp = Parameter(name = 'MZp',
                nature = 'external',
                type = 'real',
                value = 300,
                texname = '\\text{MZp}',
                lhablock = 'MASS',
                lhacode = [ 56 ])

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

WhD = Parameter(name = 'WhD',
                nature = 'external',
                type = 'real',
                value = 1,
                texname = '\\text{WhD}',
                lhablock = 'DECAY',
                lhacode = [ 26 ])

WZp = Parameter(name = 'WZp',
                nature = 'external',
                type = 'real',
                value = 1,
                texname = '\\text{WZp}',
                lhablock = 'DECAY',
                lhacode = [ 56 ])

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

lmix = Parameter(name = 'lmix',
                 nature = 'internal',
                 type = 'real',
                 value = '0.1',
                 texname = '\\text{lmix}')

vevD = Parameter(name = 'vevD',
                 nature = 'internal',
                 type = 'real',
                 value = 'MZp/gz',
                 texname = '\\text{vevD}')

muxSq = Parameter(name = 'muxSq',
                  nature = 'internal',
                  type = 'real',
                  value = 'Mphix**2 - lmix*vevD**2',
                  texname = '\\text{muxSq}')

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

lamD = Parameter(name = 'lamD',
                 nature = 'internal',
                 type = 'real',
                 value = 'MHD**2/(2.*vevD**2)',
                 texname = '\\text{lamD}')

muD = Parameter(name = 'muD',
                nature = 'internal',
                type = 'real',
                value = 'cmath.sqrt(lamD*vevD**2)',
                texname = '\\text{$\\mu $D}')

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


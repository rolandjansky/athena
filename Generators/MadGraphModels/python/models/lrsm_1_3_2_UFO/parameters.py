# This file was automatically created by FeynRules 2.1
# Mathematica version: 8.0 for Mac OS X x86 (64-bit) (November 6, 2010)
# Date: Tue 2 Dec 2014 07:52:51



from object_library import all_parameters, Parameter


from function_library import complexconjugate, re, im, csc, sec, acsc, asec, cot

# This is a default parameter object representing 0.
ZERO = Parameter(name = 'ZERO',
                 nature = 'internal',
                 type = 'real',
                 value = '0.0',
                 texname = '0')

# User-defined parameters.
s12 = Parameter(name = 's12',
                nature = 'external',
                type = 'real',
                value = 0.221,
                texname = 's_{12}',
                lhablock = 'CKMBLOCK',
                lhacode = [ 1 ])

s23 = Parameter(name = 's23',
                nature = 'external',
                type = 'real',
                value = 0.041,
                texname = 's_{23}',
                lhablock = 'CKMBLOCK',
                lhacode = [ 2 ])

s13 = Parameter(name = 's13',
                nature = 'external',
                type = 'real',
                value = 0.0035,
                texname = 's_{13}',
                lhablock = 'CKMBLOCK',
                lhacode = [ 3 ])

lambda2 = Parameter(name = 'lambda2',
                    nature = 'external',
                    type = 'real',
                    value = 0.2,
                    texname = '\\lambda _2',
                    lhablock = 'HIGGSBLOCK',
                    lhacode = [ 2 ])

lambda3 = Parameter(name = 'lambda3',
                    nature = 'external',
                    type = 'real',
                    value = -0.234,
                    texname = '\\lambda _3',
                    lhablock = 'HIGGSBLOCK',
                    lhacode = [ 3 ])

lambda4 = Parameter(name = 'lambda4',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\lambda _4',
                    lhablock = 'HIGGSBLOCK',
                    lhacode = [ 4 ])

rho2 = Parameter(name = 'rho2',
                 nature = 'external',
                 type = 'real',
                 value = 0.05,
                 texname = '\\rho _2',
                 lhablock = 'HIGGSBLOCK',
                 lhacode = [ 6 ])

rho3 = Parameter(name = 'rho3',
                 nature = 'external',
                 type = 'real',
                 value = 1.25,
                 texname = '\\rho _3',
                 lhablock = 'HIGGSBLOCK',
                 lhacode = [ 7 ])

rho4 = Parameter(name = 'rho4',
                 nature = 'external',
                 type = 'real',
                 value = 0.125,
                 texname = '\\rho _4',
                 lhablock = 'HIGGSBLOCK',
                 lhacode = [ 8 ])

alpha1 = Parameter(name = 'alpha1',
                   nature = 'external',
                   type = 'real',
                   value = 0.01,
                   texname = '\\alpha _1',
                   lhablock = 'HIGGSBLOCK',
                   lhacode = [ 9 ])

alpha2 = Parameter(name = 'alpha2',
                   nature = 'external',
                   type = 'real',
                   value = 0.5,
                   texname = '\\alpha _2',
                   lhablock = 'HIGGSBLOCK',
                   lhacode = [ 10 ])

alpha3 = Parameter(name = 'alpha3',
                   nature = 'external',
                   type = 'real',
                   value = 0.5,
                   texname = '\\alpha _3',
                   lhablock = 'HIGGSBLOCK',
                   lhacode = [ 11 ])

VKe = Parameter(name = 'VKe',
                nature = 'external',
                type = 'real',
                value = 0,
                texname = '\\text{KLRmix}_e',
                lhablock = 'KLRBLOCK',
                lhacode = [ 1 ])

VKmu = Parameter(name = 'VKmu',
                 nature = 'external',
                 type = 'real',
                 value = 0,
                 texname = '\\text{KLRmix}_{\\mu }',
                 lhablock = 'KLRBLOCK',
                 lhacode = [ 2 ])

VKta = Parameter(name = 'VKta',
                 nature = 'external',
                 type = 'real',
                 value = 0,
                 texname = '\\text{KLRmix}_{\\text{ta}}',
                 lhablock = 'KLRBLOCK',
                 lhacode = [ 3 ])

Wl11 = Parameter(name = 'Wl11',
                 nature = 'external',
                 type = 'real',
                 value = 1,
                 texname = '\\text{Wl}_{11}',
                 lhablock = 'MIXINGBLOCK',
                 lhacode = [ 1 ])

Wl22 = Parameter(name = 'Wl22',
                 nature = 'external',
                 type = 'real',
                 value = 1,
                 texname = '\\text{Wl}_{22}',
                 lhablock = 'MIXINGBLOCK',
                 lhacode = [ 2 ])

Wl33 = Parameter(name = 'Wl33',
                 nature = 'external',
                 type = 'real',
                 value = 1,
                 texname = '\\text{Wl}_{33}',
                 lhablock = 'MIXINGBLOCK',
                 lhacode = [ 3 ])

WU11 = Parameter(name = 'WU11',
                 nature = 'external',
                 type = 'real',
                 value = 1,
                 texname = '\\text{WU}_{11}',
                 lhablock = 'MIXINGBLOCK',
                 lhacode = [ 4 ])

WU22 = Parameter(name = 'WU22',
                 nature = 'external',
                 type = 'real',
                 value = 1,
                 texname = '\\text{WU}_{22}',
                 lhablock = 'MIXINGBLOCK',
                 lhacode = [ 5 ])

WU33 = Parameter(name = 'WU33',
                 nature = 'external',
                 type = 'real',
                 value = -1,
                 texname = '\\text{WU}_{33}',
                 lhablock = 'MIXINGBLOCK',
                 lhacode = [ 6 ])

WD11 = Parameter(name = 'WD11',
                 nature = 'external',
                 type = 'real',
                 value = 1,
                 texname = '\\text{WD}_{11}',
                 lhablock = 'MIXINGBLOCK',
                 lhacode = [ 7 ])

WD22 = Parameter(name = 'WD22',
                 nature = 'external',
                 type = 'real',
                 value = 1,
                 texname = '\\text{WD}_{22}',
                 lhablock = 'MIXINGBLOCK',
                 lhacode = [ 8 ])

WD33 = Parameter(name = 'WD33',
                 nature = 'external',
                 type = 'real',
                 value = 1,
                 texname = '\\text{WD}_{33}',
                 lhablock = 'MIXINGBLOCK',
                 lhacode = [ 9 ])

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

sw2 = Parameter(name = 'sw2',
                nature = 'external',
                type = 'real',
                value = 0.23126,
                texname = '\\text{sw2}',
                lhablock = 'SMINPUTS',
                lhacode = [ 4 ])

tanb = Parameter(name = 'tanb',
                 nature = 'external',
                 type = 'real',
                 value = 0,
                 texname = '\\text{tanb}',
                 lhablock = 'VEVS',
                 lhacode = [ 1 ])

vL = Parameter(name = 'vL',
               nature = 'external',
               type = 'real',
               value = 0,
               texname = '\\text{vL}',
               lhablock = 'VEVS',
               lhacode = [ 2 ])

MZ = Parameter(name = 'MZ',
               nature = 'external',
               type = 'real',
               value = 91.1876,
               texname = '\\text{MZ}',
               lhablock = 'MASS',
               lhacode = [ 23 ])

MW = Parameter(name = 'MW',
               nature = 'external',
               type = 'real',
               value = 80.385,
               texname = '\\text{MW}',
               lhablock = 'MASS',
               lhacode = [ 24 ])

MW2 = Parameter(name = 'MW2',
                nature = 'external',
                type = 'real',
                value = 3000.,
                texname = '\\text{MW2}',
                lhablock = 'MASS',
                lhacode = [ 34 ])

Me = Parameter(name = 'Me',
               nature = 'external',
               type = 'real',
               value = 0.0005110000000000001,
               texname = '\\text{Me}',
               lhablock = 'MASS',
               lhacode = [ 11 ])

Mmu = Parameter(name = 'Mmu',
                nature = 'external',
                type = 'real',
                value = 0.10566,
                texname = '\\text{Mmu}',
                lhablock = 'MASS',
                lhacode = [ 13 ])

Mta = Parameter(name = 'Mta',
                nature = 'external',
                type = 'real',
                value = 1.777,
                texname = '\\text{Mta}',
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

Mv1 = Parameter(name = 'Mv1',
                nature = 'external',
                type = 'real',
                value = 1.e-10,
                texname = '\\text{Mv1}',
                lhablock = 'MASS',
                lhacode = [ 12 ])

Mv2 = Parameter(name = 'Mv2',
                nature = 'external',
                type = 'real',
                value = 1.e-10,
                texname = '\\text{Mv2}',
                lhablock = 'MASS',
                lhacode = [ 14 ])

Mv3 = Parameter(name = 'Mv3',
                nature = 'external',
                type = 'real',
                value = 1.e-10,
                texname = '\\text{Mv3}',
                lhablock = 'MASS',
                lhacode = [ 16 ])

MN1 = Parameter(name = 'MN1',
                nature = 'external',
                type = 'real',
                value = 100,
                texname = '\\text{MN1}',
                lhablock = 'MASS',
                lhacode = [ 9900012 ])

MN2 = Parameter(name = 'MN2',
                nature = 'external',
                type = 'real',
                value = 100,
                texname = '\\text{MN2}',
                lhablock = 'MASS',
                lhacode = [ 9900014 ])

MN3 = Parameter(name = 'MN3',
                nature = 'external',
                type = 'real',
                value = 100,
                texname = '\\text{MN3}',
                lhablock = 'MASS',
                lhacode = [ 9900016 ])

MH = Parameter(name = 'MH',
               nature = 'external',
               type = 'real',
               value = 125.5,
               texname = '\\text{MH}',
               lhablock = 'MASS',
               lhacode = [ 25 ])

MH02 = Parameter(name = 'MH02',
                 nature = 'external',
                 type = 'real',
                 value = 500,
                 texname = '\\text{MH02}',
                 lhablock = 'MASS',
                 lhacode = [ 43 ])

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

WW2 = Parameter(name = 'WW2',
                nature = 'external',
                type = 'real',
                value = 101.2708,
                texname = '\\text{WW2}',
                lhablock = 'DECAY',
                lhacode = [ 34 ])

WZ2 = Parameter(name = 'WZ2',
                nature = 'external',
                type = 'real',
                value = 80,
                texname = '\\text{WZ2}',
                lhablock = 'DECAY',
                lhacode = [ 32 ])

WT = Parameter(name = 'WT',
               nature = 'external',
               type = 'real',
               value = 1.50833649,
               texname = '\\text{WT}',
               lhablock = 'DECAY',
               lhacode = [ 6 ])

WN1 = Parameter(name = 'WN1',
                nature = 'external',
                type = 'real',
                value = 1.4048320000000002e-9,
                texname = '\\text{WN1}',
                lhablock = 'DECAY',
                lhacode = [ 9900012 ])

WN2 = Parameter(name = 'WN2',
                nature = 'external',
                type = 'real',
                value = 1.404054e-9,
                texname = '\\text{WN2}',
                lhablock = 'DECAY',
                lhacode = [ 9900014 ])

WN3 = Parameter(name = 'WN3',
                nature = 'external',
                type = 'real',
                value = 1.395897e-9,
                texname = '\\text{WN3}',
                lhablock = 'DECAY',
                lhacode = [ 9900016 ])

WH = Parameter(name = 'WH',
               nature = 'external',
               type = 'real',
               value = 0.0057,
               texname = '\\text{WH}',
               lhablock = 'DECAY',
               lhacode = [ 25 ])

WH01 = Parameter(name = 'WH01',
                 nature = 'external',
                 type = 'real',
                 value = 0.00575308848,
                 texname = '\\text{WH01}',
                 lhablock = 'DECAY',
                 lhacode = [ 35 ])

WH02 = Parameter(name = 'WH02',
                 nature = 'external',
                 type = 'real',
                 value = 0.00575308848,
                 texname = '\\text{WH02}',
                 lhablock = 'DECAY',
                 lhacode = [ 43 ])

WH03 = Parameter(name = 'WH03',
                 nature = 'external',
                 type = 'real',
                 value = 0.00575308848,
                 texname = '\\text{WH03}',
                 lhablock = 'DECAY',
                 lhacode = [ 44 ])

WHP1 = Parameter(name = 'WHP1',
                 nature = 'external',
                 type = 'real',
                 value = 0.006,
                 texname = '\\text{WHP1}',
                 lhablock = 'DECAY',
                 lhacode = [ 37 ])

WHP2 = Parameter(name = 'WHP2',
                 nature = 'external',
                 type = 'real',
                 value = 0.006,
                 texname = '\\text{WHP2}',
                 lhablock = 'DECAY',
                 lhacode = [ 38 ])

WHPPL = Parameter(name = 'WHPPL',
                  nature = 'external',
                  type = 'real',
                  value = 0.007,
                  texname = '\\text{WHPPL}',
                  lhablock = 'DECAY',
                  lhacode = [ 61 ])

WHPPR = Parameter(name = 'WHPPR',
                  nature = 'external',
                  type = 'real',
                  value = 0.007,
                  texname = '\\text{WHPPR}',
                  lhablock = 'DECAY',
                  lhacode = [ 62 ])

WA01 = Parameter(name = 'WA01',
                 nature = 'external',
                 type = 'real',
                 value = 0.006,
                 texname = '\\text{WA01}',
                 lhablock = 'DECAY',
                 lhacode = [ 36 ])

WA02 = Parameter(name = 'WA02',
                 nature = 'external',
                 type = 'real',
                 value = 0.006,
                 texname = '\\text{WA02}',
                 lhablock = 'DECAY',
                 lhacode = [ 45 ])

aEW = Parameter(name = 'aEW',
                nature = 'internal',
                type = 'real',
                value = '1/aEWM1',
                texname = '\\alpha _{\\text{EW}}')

sw = Parameter(name = 'sw',
               nature = 'internal',
               type = 'real',
               value = 'cmath.sqrt(sw2)',
               texname = 's_w')

cw = Parameter(name = 'cw',
               nature = 'internal',
               type = 'real',
               value = 'cmath.sqrt(1 - sw2)',
               texname = 'c_w')

gs = Parameter(name = 'gs',
               nature = 'internal',
               type = 'real',
               value = '2*cmath.sqrt(aS)*cmath.sqrt(cmath.pi)',
               texname = 'g_s')

vev = Parameter(name = 'vev',
                nature = 'internal',
                type = 'real',
                value = '1/(2**0.25*cmath.sqrt(Gf))',
                texname = '\\text{Vev}')

yML1x1 = Parameter(name = 'yML1x1',
                   nature = 'internal',
                   type = 'real',
                   value = 'Me',
                   texname = '\\text{yML1x1}')

yML2x2 = Parameter(name = 'yML2x2',
                   nature = 'internal',
                   type = 'real',
                   value = 'Mmu',
                   texname = '\\text{yML2x2}')

yML3x3 = Parameter(name = 'yML3x3',
                   nature = 'internal',
                   type = 'real',
                   value = 'Mta',
                   texname = '\\text{yML3x3}')

yNL1x1 = Parameter(name = 'yNL1x1',
                   nature = 'internal',
                   type = 'real',
                   value = 'Mv1',
                   texname = '\\text{yNL1x1}')

yNL2x2 = Parameter(name = 'yNL2x2',
                   nature = 'internal',
                   type = 'real',
                   value = 'Mv2',
                   texname = '\\text{yNL2x2}')

yNL3x3 = Parameter(name = 'yNL3x3',
                   nature = 'internal',
                   type = 'real',
                   value = 'Mv3',
                   texname = '\\text{yNL3x3}')

yNL4x4 = Parameter(name = 'yNL4x4',
                   nature = 'internal',
                   type = 'real',
                   value = 'MN1',
                   texname = '\\text{yNL4x4}')

yNL5x5 = Parameter(name = 'yNL5x5',
                   nature = 'internal',
                   type = 'real',
                   value = 'MN2',
                   texname = '\\text{yNL5x5}')

yNL6x6 = Parameter(name = 'yNL6x6',
                   nature = 'internal',
                   type = 'real',
                   value = 'MN3',
                   texname = '\\text{yNL6x6}')

yMU1x1 = Parameter(name = 'yMU1x1',
                   nature = 'internal',
                   type = 'real',
                   value = 'MU',
                   texname = '\\text{yMU1x1}')

yMU2x2 = Parameter(name = 'yMU2x2',
                   nature = 'internal',
                   type = 'real',
                   value = 'MC',
                   texname = '\\text{yMU2x2}')

yMU3x3 = Parameter(name = 'yMU3x3',
                   nature = 'internal',
                   type = 'real',
                   value = 'MT',
                   texname = '\\text{yMU3x3}')

yDO1x1 = Parameter(name = 'yDO1x1',
                   nature = 'internal',
                   type = 'real',
                   value = 'MD',
                   texname = '\\text{yDO1x1}')

yDO2x2 = Parameter(name = 'yDO2x2',
                   nature = 'internal',
                   type = 'real',
                   value = 'MS',
                   texname = '\\text{yDO2x2}')

yDO3x3 = Parameter(name = 'yDO3x3',
                   nature = 'internal',
                   type = 'real',
                   value = 'MB',
                   texname = '\\text{yDO3x3}')

CKML1x1 = Parameter(name = 'CKML1x1',
                    nature = 'internal',
                    type = 'complex',
                    value = 'cmath.sqrt(1 - s12**2)*cmath.sqrt(1 - s13**2)',
                    texname = '\\text{CKML1x1}')

CKML1x2 = Parameter(name = 'CKML1x2',
                    nature = 'internal',
                    type = 'complex',
                    value = 's12*cmath.sqrt(1 - s13**2)',
                    texname = '\\text{CKML1x2}')

CKML1x3 = Parameter(name = 'CKML1x3',
                    nature = 'internal',
                    type = 'complex',
                    value = 's13',
                    texname = '\\text{CKML1x3}')

CKML2x1 = Parameter(name = 'CKML2x1',
                    nature = 'internal',
                    type = 'complex',
                    value = '-(s13*s23*cmath.sqrt(1 - s12**2)) - s12*cmath.sqrt(1 - s23**2)',
                    texname = '\\text{CKML2x1}')

CKML2x2 = Parameter(name = 'CKML2x2',
                    nature = 'internal',
                    type = 'complex',
                    value = '-(s12*s13*s23) + cmath.sqrt(1 - s12**2)*cmath.sqrt(1 - s23**2)',
                    texname = '\\text{CKML2x2}')

CKML2x3 = Parameter(name = 'CKML2x3',
                    nature = 'internal',
                    type = 'complex',
                    value = 's23*cmath.sqrt(1 - s13**2)',
                    texname = '\\text{CKML2x3}')

CKML3x1 = Parameter(name = 'CKML3x1',
                    nature = 'internal',
                    type = 'complex',
                    value = 's12*s23 - s13*cmath.sqrt(1 - s12**2)*cmath.sqrt(1 - s23**2)',
                    texname = '\\text{CKML3x1}')

CKML3x2 = Parameter(name = 'CKML3x2',
                    nature = 'internal',
                    type = 'complex',
                    value = '-(s23*cmath.sqrt(1 - s12**2)) - s12*s13*cmath.sqrt(1 - s23**2)',
                    texname = '\\text{CKML3x2}')

CKML3x3 = Parameter(name = 'CKML3x3',
                    nature = 'internal',
                    type = 'complex',
                    value = 'cmath.sqrt(1 - s13**2)*cmath.sqrt(1 - s23**2)',
                    texname = '\\text{CKML3x3}')

Wl1x1 = Parameter(name = 'Wl1x1',
                  nature = 'internal',
                  type = 'real',
                  value = 'Wl11',
                  texname = '\\text{Wl1x1}')

Wl2x2 = Parameter(name = 'Wl2x2',
                  nature = 'internal',
                  type = 'real',
                  value = 'Wl22',
                  texname = '\\text{Wl2x2}')

Wl3x3 = Parameter(name = 'Wl3x3',
                  nature = 'internal',
                  type = 'real',
                  value = 'Wl33',
                  texname = '\\text{Wl3x3}')

WU1x1 = Parameter(name = 'WU1x1',
                  nature = 'internal',
                  type = 'real',
                  value = 'WU11',
                  texname = '\\text{WU1x1}')

WU2x2 = Parameter(name = 'WU2x2',
                  nature = 'internal',
                  type = 'real',
                  value = 'WU22',
                  texname = '\\text{WU2x2}')

WU3x3 = Parameter(name = 'WU3x3',
                  nature = 'internal',
                  type = 'real',
                  value = 'WU33',
                  texname = '\\text{WU3x3}')

WD1x1 = Parameter(name = 'WD1x1',
                  nature = 'internal',
                  type = 'real',
                  value = 'WD11',
                  texname = '\\text{WD1x1}')

WD2x2 = Parameter(name = 'WD2x2',
                  nature = 'internal',
                  type = 'real',
                  value = 'WD22',
                  texname = '\\text{WD2x2}')

WD3x3 = Parameter(name = 'WD3x3',
                  nature = 'internal',
                  type = 'real',
                  value = 'WD33',
                  texname = '\\text{WD3x3}')

KL1x1 = Parameter(name = 'KL1x1',
                  nature = 'internal',
                  type = 'real',
                  value = '1',
                  texname = '\\text{KL1x1}')

KL2x2 = Parameter(name = 'KL2x2',
                  nature = 'internal',
                  type = 'real',
                  value = '1',
                  texname = '\\text{KL2x2}')

KL3x3 = Parameter(name = 'KL3x3',
                  nature = 'internal',
                  type = 'real',
                  value = '1',
                  texname = '\\text{KL3x3}')

KL4x1 = Parameter(name = 'KL4x1',
                  nature = 'internal',
                  type = 'real',
                  value = 'VKe',
                  texname = '\\text{KL4x1}')

KL5x2 = Parameter(name = 'KL5x2',
                  nature = 'internal',
                  type = 'real',
                  value = 'VKmu',
                  texname = '\\text{KL5x2}')

KL6x3 = Parameter(name = 'KL6x3',
                  nature = 'internal',
                  type = 'real',
                  value = 'VKta',
                  texname = '\\text{KL6x3}')

KR1x1 = Parameter(name = 'KR1x1',
                  nature = 'internal',
                  type = 'real',
                  value = '-VKe',
                  texname = '\\text{KR1x1}')

KR2x2 = Parameter(name = 'KR2x2',
                  nature = 'internal',
                  type = 'real',
                  value = '-VKmu',
                  texname = '\\text{KR2x2}')

KR3x3 = Parameter(name = 'KR3x3',
                  nature = 'internal',
                  type = 'real',
                  value = '-VKta',
                  texname = '\\text{KR3x3}')

KR4x1 = Parameter(name = 'KR4x1',
                  nature = 'internal',
                  type = 'real',
                  value = '1',
                  texname = '\\text{KR4x1}')

KR5x2 = Parameter(name = 'KR5x2',
                  nature = 'internal',
                  type = 'real',
                  value = '1',
                  texname = '\\text{KR5x2}')

KR6x3 = Parameter(name = 'KR6x3',
                  nature = 'internal',
                  type = 'real',
                  value = '1',
                  texname = '\\text{KR6x3}')

CKMR1x1 = Parameter(name = 'CKMR1x1',
                    nature = 'internal',
                    type = 'complex',
                    value = 'CKML1x1*WD1x1*WU1x1',
                    texname = '\\text{CKMR1x1}')

CKMR1x2 = Parameter(name = 'CKMR1x2',
                    nature = 'internal',
                    type = 'complex',
                    value = 'CKML1x2*WD2x2*WU1x1',
                    texname = '\\text{CKMR1x2}')

CKMR1x3 = Parameter(name = 'CKMR1x3',
                    nature = 'internal',
                    type = 'complex',
                    value = 'CKML1x3*WD3x3*WU1x1',
                    texname = '\\text{CKMR1x3}')

CKMR2x1 = Parameter(name = 'CKMR2x1',
                    nature = 'internal',
                    type = 'complex',
                    value = 'CKML2x1*WD1x1*WU2x2',
                    texname = '\\text{CKMR2x1}')

CKMR2x2 = Parameter(name = 'CKMR2x2',
                    nature = 'internal',
                    type = 'complex',
                    value = 'CKML2x2*WD2x2*WU2x2',
                    texname = '\\text{CKMR2x2}')

CKMR2x3 = Parameter(name = 'CKMR2x3',
                    nature = 'internal',
                    type = 'complex',
                    value = 'CKML2x3*WD3x3*WU2x2',
                    texname = '\\text{CKMR2x3}')

CKMR3x1 = Parameter(name = 'CKMR3x1',
                    nature = 'internal',
                    type = 'complex',
                    value = 'CKML3x1*WD1x1*WU3x3',
                    texname = '\\text{CKMR3x1}')

CKMR3x2 = Parameter(name = 'CKMR3x2',
                    nature = 'internal',
                    type = 'complex',
                    value = 'CKML3x2*WD2x2*WU3x3',
                    texname = '\\text{CKMR3x2}')

CKMR3x3 = Parameter(name = 'CKMR3x3',
                    nature = 'internal',
                    type = 'complex',
                    value = 'CKML3x3*WD3x3*WU3x3',
                    texname = '\\text{CKMR3x3}')

MZ2 = Parameter(name = 'MZ2',
                nature = 'internal',
                type = 'real',
                value = '(cw*MW2*cmath.sqrt(2))/cmath.sqrt(1 - 2*sw**2)',
                texname = 'M_{\\text{Z2}}')

ee = Parameter(name = 'ee',
               nature = 'internal',
               type = 'real',
               value = '2*cmath.sqrt(aEW)*cmath.sqrt(cmath.pi)',
               texname = 'e')

k1 = Parameter(name = 'k1',
               nature = 'internal',
               type = 'real',
               value = 'vev/cmath.sqrt(1 + tanb**2)',
               texname = '\\text{k1}')

k2 = Parameter(name = 'k2',
               nature = 'internal',
               type = 'real',
               value = '(tanb*vev)/cmath.sqrt(1 + tanb**2)',
               texname = '\\text{k2}')

eps = Parameter(name = 'eps',
                nature = 'internal',
                type = 'real',
                value = '(2*k1*k2)/vev**2',
                texname = '\\text{eps}')

g1 = Parameter(name = 'g1',
               nature = 'internal',
               type = 'real',
               value = 'ee/cmath.sqrt(1 - 2*sw**2)',
               texname = 'g_1')

gw = Parameter(name = 'gw',
               nature = 'internal',
               type = 'real',
               value = 'ee/sw',
               texname = 'g_w')

gwR = Parameter(name = 'gwR',
                nature = 'internal',
                type = 'real',
                value = 'gw',
                texname = 'g_{w_R}')

vR = Parameter(name = 'vR',
               nature = 'internal',
               type = 'real',
               value = '(MW2*cmath.sqrt(2))/gwR',
               texname = 'v_R')

MA01 = Parameter(name = 'MA01',
                 nature = 'internal',
                 type = 'real',
                 value = 'cmath.sqrt(-2*(2*lambda2 - lambda3)*vev**2 + (alpha3*vR**2)/(2.*cmath.sqrt(1 - eps**2)))',
                 texname = 'M_{\\text{A01}}')

MH01 = Parameter(name = 'MH01',
                 nature = 'internal',
                 type = 'real',
                 value = 'cmath.sqrt((alpha3*vR**2)/cmath.sqrt(1 - eps**2))/cmath.sqrt(2)',
                 texname = 'M_{\\text{H01}}')

MHP2 = Parameter(name = 'MHP2',
                 nature = 'internal',
                 type = 'real',
                 value = 'cmath.sqrt(alpha3*(vR**2/cmath.sqrt(1 - eps**2) + (vev**2*cmath.sqrt(1 - eps**2))/2.))/cmath.sqrt(2)',
                 texname = 'M_{\\text{HP2}}')

MHPPR = Parameter(name = 'MHPPR',
                  nature = 'internal',
                  type = 'real',
                  value = 'cmath.sqrt(2*rho2*vR**2 + (alpha3*vev**2*cmath.sqrt(1 - eps**2))/2.)',
                  texname = 'M_{\\text{HPPR}}')

rho1 = Parameter(name = 'rho1',
                 nature = 'internal',
                 type = 'real',
                 value = 'MH02**2/(2.*vR**2)',
                 texname = '\\rho _1')

sphi = Parameter(name = 'sphi',
                 nature = 'internal',
                 type = 'real',
                 value = '-((1 - 2*sw**2)**1.5*vev**2)/(4.*cw**4*vR**2)',
                 texname = 's_{\\phi }')

sxi = Parameter(name = 'sxi',
                nature = 'internal',
                type = 'real',
                value = '-((k1*k2)/vR**2)',
                texname = '\\xi _{\\text{LR}}')

cphi = Parameter(name = 'cphi',
                 nature = 'internal',
                 type = 'real',
                 value = 'cmath.sqrt(1 - sphi**2)',
                 texname = 'c_{\\phi }')

cxi = Parameter(name = 'cxi',
                nature = 'internal',
                type = 'real',
                value = 'cmath.sqrt(1 - sxi**2)',
                texname = 'c_{\\theta }')

MA02 = Parameter(name = 'MA02',
                 nature = 'internal',
                 type = 'real',
                 value = 'cmath.sqrt((-2*rho1 + rho3)*vR**2)/cmath.sqrt(2)',
                 texname = 'M_{\\text{A02}}')

MH03 = Parameter(name = 'MH03',
                 nature = 'internal',
                 type = 'real',
                 value = 'cmath.sqrt((-2*rho1 + rho3)*vR**2)/cmath.sqrt(2)',
                 texname = 'M_{\\text{H03}}')

MHP1 = Parameter(name = 'MHP1',
                 nature = 'internal',
                 type = 'real',
                 value = 'cmath.sqrt(((-2*rho1 + rho3)*vR**2)/2. + (alpha3*vev**2*cmath.sqrt(1 - eps**2))/4.)',
                 texname = 'M_{\\text{HP1}}')

MHPPL = Parameter(name = 'MHPPL',
                  nature = 'internal',
                  type = 'real',
                  value = 'cmath.sqrt((-2*rho1 + rho3)*vR**2 + alpha3*vev**2*cmath.sqrt(1 - eps**2))/cmath.sqrt(2)',
                  texname = 'M_{\\text{HPPL}}')

lambda1 = Parameter(name = 'lambda1',
                    nature = 'internal',
                    type = 'real',
                    value = 'alpha1**2/(4.*rho1) + MH**2/(2.*vev**2)',
                    texname = '\\lambda _1')

I1a11 = Parameter(name = 'I1a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR1x1*yDO1x1',
                  texname = '\\text{I1a11}')

I1a12 = Parameter(name = 'I1a12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR1x2*yDO2x2',
                  texname = '\\text{I1a12}')

I1a13 = Parameter(name = 'I1a13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR1x3*yDO3x3',
                  texname = '\\text{I1a13}')

I1a21 = Parameter(name = 'I1a21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR2x1*yDO1x1',
                  texname = '\\text{I1a21}')

I1a22 = Parameter(name = 'I1a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR2x2*yDO2x2',
                  texname = '\\text{I1a22}')

I1a23 = Parameter(name = 'I1a23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR2x3*yDO3x3',
                  texname = '\\text{I1a23}')

I1a31 = Parameter(name = 'I1a31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR3x1*yDO1x1',
                  texname = '\\text{I1a31}')

I1a32 = Parameter(name = 'I1a32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR3x2*yDO2x2',
                  texname = '\\text{I1a32}')

I1a33 = Parameter(name = 'I1a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR3x3*yDO3x3',
                  texname = '\\text{I1a33}')

I10a11 = Parameter(name = 'I10a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yMU1x1*complexconjugate(CKMR1x1)',
                   texname = '\\text{I10a11}')

I10a12 = Parameter(name = 'I10a12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yMU2x2*complexconjugate(CKMR2x1)',
                   texname = '\\text{I10a12}')

I10a13 = Parameter(name = 'I10a13',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yMU3x3*complexconjugate(CKMR3x1)',
                   texname = '\\text{I10a13}')

I10a21 = Parameter(name = 'I10a21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yMU1x1*complexconjugate(CKMR1x2)',
                   texname = '\\text{I10a21}')

I10a22 = Parameter(name = 'I10a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yMU2x2*complexconjugate(CKMR2x2)',
                   texname = '\\text{I10a22}')

I10a23 = Parameter(name = 'I10a23',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yMU3x3*complexconjugate(CKMR3x2)',
                   texname = '\\text{I10a23}')

I10a31 = Parameter(name = 'I10a31',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yMU1x1*complexconjugate(CKMR1x3)',
                   texname = '\\text{I10a31}')

I10a32 = Parameter(name = 'I10a32',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yMU2x2*complexconjugate(CKMR2x3)',
                   texname = '\\text{I10a32}')

I10a33 = Parameter(name = 'I10a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yMU3x3*complexconjugate(CKMR3x3)',
                   texname = '\\text{I10a33}')

I11a11 = Parameter(name = 'I11a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yDO1x1*complexconjugate(CKMR1x1)',
                   texname = '\\text{I11a11}')

I11a12 = Parameter(name = 'I11a12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yDO1x1*complexconjugate(CKMR2x1)',
                   texname = '\\text{I11a12}')

I11a13 = Parameter(name = 'I11a13',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yDO1x1*complexconjugate(CKMR3x1)',
                   texname = '\\text{I11a13}')

I11a21 = Parameter(name = 'I11a21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yDO2x2*complexconjugate(CKMR1x2)',
                   texname = '\\text{I11a21}')

I11a22 = Parameter(name = 'I11a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yDO2x2*complexconjugate(CKMR2x2)',
                   texname = '\\text{I11a22}')

I11a23 = Parameter(name = 'I11a23',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yDO2x2*complexconjugate(CKMR3x2)',
                   texname = '\\text{I11a23}')

I11a31 = Parameter(name = 'I11a31',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yDO3x3*complexconjugate(CKMR1x3)',
                   texname = '\\text{I11a31}')

I11a32 = Parameter(name = 'I11a32',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yDO3x3*complexconjugate(CKMR2x3)',
                   texname = '\\text{I11a32}')

I11a33 = Parameter(name = 'I11a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yDO3x3*complexconjugate(CKMR3x3)',
                   texname = '\\text{I11a33}')

I12a11 = Parameter(name = 'I12a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yMU1x1*complexconjugate(CKML1x1)',
                   texname = '\\text{I12a11}')

I12a12 = Parameter(name = 'I12a12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yMU2x2*complexconjugate(CKML2x1)',
                   texname = '\\text{I12a12}')

I12a13 = Parameter(name = 'I12a13',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yMU3x3*complexconjugate(CKML3x1)',
                   texname = '\\text{I12a13}')

I12a21 = Parameter(name = 'I12a21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yMU1x1*complexconjugate(CKML1x2)',
                   texname = '\\text{I12a21}')

I12a22 = Parameter(name = 'I12a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yMU2x2*complexconjugate(CKML2x2)',
                   texname = '\\text{I12a22}')

I12a23 = Parameter(name = 'I12a23',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yMU3x3*complexconjugate(CKML3x2)',
                   texname = '\\text{I12a23}')

I12a31 = Parameter(name = 'I12a31',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yMU1x1*complexconjugate(CKML1x3)',
                   texname = '\\text{I12a31}')

I12a32 = Parameter(name = 'I12a32',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yMU2x2*complexconjugate(CKML2x3)',
                   texname = '\\text{I12a32}')

I12a33 = Parameter(name = 'I12a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yMU3x3*complexconjugate(CKML3x3)',
                   texname = '\\text{I12a33}')

I13a11 = Parameter(name = 'I13a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1**2*KR1x1**2*yNL1x1 + KL1x1*KL4x1*KR1x1*KR4x1*yNL4x4',
                   texname = '\\text{I13a11}')

I13a14 = Parameter(name = 'I13a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1*KR1x1**2*yNL1x1 + KL4x1**2*KR1x1*KR4x1*yNL4x4',
                   texname = '\\text{I13a14}')

I13a22 = Parameter(name = 'I13a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2**2*KR2x2**2*yNL2x2 + KL2x2*KL5x2*KR2x2*KR5x2*yNL5x5',
                   texname = '\\text{I13a22}')

I13a25 = Parameter(name = 'I13a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2*KR2x2**2*yNL2x2 + KL5x2**2*KR2x2*KR5x2*yNL5x5',
                   texname = '\\text{I13a25}')

I13a33 = Parameter(name = 'I13a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3**2*KR3x3**2*yNL3x3 + KL3x3*KL6x3*KR3x3*KR6x3*yNL6x6',
                   texname = '\\text{I13a33}')

I13a36 = Parameter(name = 'I13a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3*KR3x3**2*yNL3x3 + KL6x3**2*KR3x3*KR6x3*yNL6x6',
                   texname = '\\text{I13a36}')

I13a41 = Parameter(name = 'I13a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1**2*KR1x1*KR4x1*yNL1x1 + KL1x1*KL4x1*KR4x1**2*yNL4x4',
                   texname = '\\text{I13a41}')

I13a44 = Parameter(name = 'I13a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1*KR1x1*KR4x1*yNL1x1 + KL4x1**2*KR4x1**2*yNL4x4',
                   texname = '\\text{I13a44}')

I13a52 = Parameter(name = 'I13a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2**2*KR2x2*KR5x2*yNL2x2 + KL2x2*KL5x2*KR5x2**2*yNL5x5',
                   texname = '\\text{I13a52}')

I13a55 = Parameter(name = 'I13a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2*KR2x2*KR5x2*yNL2x2 + KL5x2**2*KR5x2**2*yNL5x5',
                   texname = '\\text{I13a55}')

I13a63 = Parameter(name = 'I13a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3**2*KR3x3*KR6x3*yNL3x3 + KL3x3*KL6x3*KR6x3**2*yNL6x6',
                   texname = '\\text{I13a63}')

I13a66 = Parameter(name = 'I13a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3*KR3x3*KR6x3*yNL3x3 + KL6x3**2*KR6x3**2*yNL6x6',
                   texname = '\\text{I13a66}')

I14a11 = Parameter(name = 'I14a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1**2*KR1x1**2*yNL1x1 + KL1x1*KL4x1*KR1x1*KR4x1*yNL4x4',
                   texname = '\\text{I14a11}')

I14a14 = Parameter(name = 'I14a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1**2*KR1x1*KR4x1*yNL1x1 + KL1x1*KL4x1*KR4x1**2*yNL4x4',
                   texname = '\\text{I14a14}')

I14a22 = Parameter(name = 'I14a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2**2*KR2x2**2*yNL2x2 + KL2x2*KL5x2*KR2x2*KR5x2*yNL5x5',
                   texname = '\\text{I14a22}')

I14a25 = Parameter(name = 'I14a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2**2*KR2x2*KR5x2*yNL2x2 + KL2x2*KL5x2*KR5x2**2*yNL5x5',
                   texname = '\\text{I14a25}')

I14a33 = Parameter(name = 'I14a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3**2*KR3x3**2*yNL3x3 + KL3x3*KL6x3*KR3x3*KR6x3*yNL6x6',
                   texname = '\\text{I14a33}')

I14a36 = Parameter(name = 'I14a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3**2*KR3x3*KR6x3*yNL3x3 + KL3x3*KL6x3*KR6x3**2*yNL6x6',
                   texname = '\\text{I14a36}')

I14a41 = Parameter(name = 'I14a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1*KR1x1**2*yNL1x1 + KL4x1**2*KR1x1*KR4x1*yNL4x4',
                   texname = '\\text{I14a41}')

I14a44 = Parameter(name = 'I14a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1*KR1x1*KR4x1*yNL1x1 + KL4x1**2*KR4x1**2*yNL4x4',
                   texname = '\\text{I14a44}')

I14a52 = Parameter(name = 'I14a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2*KR2x2**2*yNL2x2 + KL5x2**2*KR2x2*KR5x2*yNL5x5',
                   texname = '\\text{I14a52}')

I14a55 = Parameter(name = 'I14a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2*KR2x2*KR5x2*yNL2x2 + KL5x2**2*KR5x2**2*yNL5x5',
                   texname = '\\text{I14a55}')

I14a63 = Parameter(name = 'I14a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3*KR3x3**2*yNL3x3 + KL6x3**2*KR3x3*KR6x3*yNL6x6',
                   texname = '\\text{I14a63}')

I14a66 = Parameter(name = 'I14a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3*KR3x3*KR6x3*yNL3x3 + KL6x3**2*KR6x3**2*yNL6x6',
                   texname = '\\text{I14a66}')

I15a11 = Parameter(name = 'I15a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1**2*KR1x1**2*yNL1x1 + KL1x1*KL4x1*KR1x1*KR4x1*yNL4x4',
                   texname = '\\text{I15a11}')

I15a14 = Parameter(name = 'I15a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1**2*KR1x1*KR4x1*yNL1x1 + KL1x1*KL4x1*KR4x1**2*yNL4x4',
                   texname = '\\text{I15a14}')

I15a22 = Parameter(name = 'I15a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2**2*KR2x2**2*yNL2x2 + KL2x2*KL5x2*KR2x2*KR5x2*yNL5x5',
                   texname = '\\text{I15a22}')

I15a25 = Parameter(name = 'I15a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2**2*KR2x2*KR5x2*yNL2x2 + KL2x2*KL5x2*KR5x2**2*yNL5x5',
                   texname = '\\text{I15a25}')

I15a33 = Parameter(name = 'I15a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3**2*KR3x3**2*yNL3x3 + KL3x3*KL6x3*KR3x3*KR6x3*yNL6x6',
                   texname = '\\text{I15a33}')

I15a36 = Parameter(name = 'I15a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3**2*KR3x3*KR6x3*yNL3x3 + KL3x3*KL6x3*KR6x3**2*yNL6x6',
                   texname = '\\text{I15a36}')

I15a41 = Parameter(name = 'I15a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1*KR1x1**2*yNL1x1 + KL4x1**2*KR1x1*KR4x1*yNL4x4',
                   texname = '\\text{I15a41}')

I15a44 = Parameter(name = 'I15a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1*KR1x1*KR4x1*yNL1x1 + KL4x1**2*KR4x1**2*yNL4x4',
                   texname = '\\text{I15a44}')

I15a52 = Parameter(name = 'I15a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2*KR2x2**2*yNL2x2 + KL5x2**2*KR2x2*KR5x2*yNL5x5',
                   texname = '\\text{I15a52}')

I15a55 = Parameter(name = 'I15a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2*KR2x2*KR5x2*yNL2x2 + KL5x2**2*KR5x2**2*yNL5x5',
                   texname = '\\text{I15a55}')

I15a63 = Parameter(name = 'I15a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3*KR3x3**2*yNL3x3 + KL6x3**2*KR3x3*KR6x3*yNL6x6',
                   texname = '\\text{I15a63}')

I15a66 = Parameter(name = 'I15a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3*KR3x3*KR6x3*yNL3x3 + KL6x3**2*KR6x3**2*yNL6x6',
                   texname = '\\text{I15a66}')

I16a11 = Parameter(name = 'I16a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1**2*KR1x1**2*yNL1x1 + KL1x1*KL4x1*KR1x1*KR4x1*yNL4x4',
                   texname = '\\text{I16a11}')

I16a14 = Parameter(name = 'I16a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1*KR1x1**2*yNL1x1 + KL4x1**2*KR1x1*KR4x1*yNL4x4',
                   texname = '\\text{I16a14}')

I16a22 = Parameter(name = 'I16a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2**2*KR2x2**2*yNL2x2 + KL2x2*KL5x2*KR2x2*KR5x2*yNL5x5',
                   texname = '\\text{I16a22}')

I16a25 = Parameter(name = 'I16a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2*KR2x2**2*yNL2x2 + KL5x2**2*KR2x2*KR5x2*yNL5x5',
                   texname = '\\text{I16a25}')

I16a33 = Parameter(name = 'I16a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3**2*KR3x3**2*yNL3x3 + KL3x3*KL6x3*KR3x3*KR6x3*yNL6x6',
                   texname = '\\text{I16a33}')

I16a36 = Parameter(name = 'I16a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3*KR3x3**2*yNL3x3 + KL6x3**2*KR3x3*KR6x3*yNL6x6',
                   texname = '\\text{I16a36}')

I16a41 = Parameter(name = 'I16a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1**2*KR1x1*KR4x1*yNL1x1 + KL1x1*KL4x1*KR4x1**2*yNL4x4',
                   texname = '\\text{I16a41}')

I16a44 = Parameter(name = 'I16a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1*KR1x1*KR4x1*yNL1x1 + KL4x1**2*KR4x1**2*yNL4x4',
                   texname = '\\text{I16a44}')

I16a52 = Parameter(name = 'I16a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2**2*KR2x2*KR5x2*yNL2x2 + KL2x2*KL5x2*KR5x2**2*yNL5x5',
                   texname = '\\text{I16a52}')

I16a55 = Parameter(name = 'I16a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2*KR2x2*KR5x2*yNL2x2 + KL5x2**2*KR5x2**2*yNL5x5',
                   texname = '\\text{I16a55}')

I16a63 = Parameter(name = 'I16a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3**2*KR3x3*KR6x3*yNL3x3 + KL3x3*KL6x3*KR6x3**2*yNL6x6',
                   texname = '\\text{I16a63}')

I16a66 = Parameter(name = 'I16a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3*KR3x3*KR6x3*yNL3x3 + KL6x3**2*KR6x3**2*yNL6x6',
                   texname = '\\text{I16a66}')

I17a11 = Parameter(name = 'I17a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KR1x1*yML1x1',
                   texname = '\\text{I17a11}')

I17a14 = Parameter(name = 'I17a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1*KR1x1*yML1x1',
                   texname = '\\text{I17a14}')

I17a22 = Parameter(name = 'I17a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KR2x2*yML2x2',
                   texname = '\\text{I17a22}')

I17a25 = Parameter(name = 'I17a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2*KR2x2*yML2x2',
                   texname = '\\text{I17a25}')

I17a33 = Parameter(name = 'I17a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KR3x3*yML3x3',
                   texname = '\\text{I17a33}')

I17a36 = Parameter(name = 'I17a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3*KR3x3*yML3x3',
                   texname = '\\text{I17a36}')

I17a41 = Parameter(name = 'I17a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KR4x1*yML1x1',
                   texname = '\\text{I17a41}')

I17a44 = Parameter(name = 'I17a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1*KR4x1*yML1x1',
                   texname = '\\text{I17a44}')

I17a52 = Parameter(name = 'I17a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KR5x2*yML2x2',
                   texname = '\\text{I17a52}')

I17a55 = Parameter(name = 'I17a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2*KR5x2*yML2x2',
                   texname = '\\text{I17a55}')

I17a63 = Parameter(name = 'I17a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KR6x3*yML3x3',
                   texname = '\\text{I17a63}')

I17a66 = Parameter(name = 'I17a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3*KR6x3*yML3x3',
                   texname = '\\text{I17a66}')

I18a11 = Parameter(name = 'I18a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KR1x1*yML1x1',
                   texname = '\\text{I18a11}')

I18a14 = Parameter(name = 'I18a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KR4x1*yML1x1',
                   texname = '\\text{I18a14}')

I18a22 = Parameter(name = 'I18a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KR2x2*yML2x2',
                   texname = '\\text{I18a22}')

I18a25 = Parameter(name = 'I18a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KR5x2*yML2x2',
                   texname = '\\text{I18a25}')

I18a33 = Parameter(name = 'I18a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KR3x3*yML3x3',
                   texname = '\\text{I18a33}')

I18a36 = Parameter(name = 'I18a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KR6x3*yML3x3',
                   texname = '\\text{I18a36}')

I18a41 = Parameter(name = 'I18a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1*KR1x1*yML1x1',
                   texname = '\\text{I18a41}')

I18a44 = Parameter(name = 'I18a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1*KR4x1*yML1x1',
                   texname = '\\text{I18a44}')

I18a52 = Parameter(name = 'I18a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2*KR2x2*yML2x2',
                   texname = '\\text{I18a52}')

I18a55 = Parameter(name = 'I18a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2*KR5x2*yML2x2',
                   texname = '\\text{I18a55}')

I18a63 = Parameter(name = 'I18a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3*KR3x3*yML3x3',
                   texname = '\\text{I18a63}')

I18a66 = Parameter(name = 'I18a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3*KR6x3*yML3x3',
                   texname = '\\text{I18a66}')

I19a11 = Parameter(name = 'I19a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KR1x1*yML1x1',
                   texname = '\\text{I19a11}')

I19a14 = Parameter(name = 'I19a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KR4x1*yML1x1',
                   texname = '\\text{I19a14}')

I19a22 = Parameter(name = 'I19a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KR2x2*yML2x2',
                   texname = '\\text{I19a22}')

I19a25 = Parameter(name = 'I19a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KR5x2*yML2x2',
                   texname = '\\text{I19a25}')

I19a33 = Parameter(name = 'I19a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KR3x3*yML3x3',
                   texname = '\\text{I19a33}')

I19a36 = Parameter(name = 'I19a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KR6x3*yML3x3',
                   texname = '\\text{I19a36}')

I19a41 = Parameter(name = 'I19a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1*KR1x1*yML1x1',
                   texname = '\\text{I19a41}')

I19a44 = Parameter(name = 'I19a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1*KR4x1*yML1x1',
                   texname = '\\text{I19a44}')

I19a52 = Parameter(name = 'I19a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2*KR2x2*yML2x2',
                   texname = '\\text{I19a52}')

I19a55 = Parameter(name = 'I19a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2*KR5x2*yML2x2',
                   texname = '\\text{I19a55}')

I19a63 = Parameter(name = 'I19a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3*KR3x3*yML3x3',
                   texname = '\\text{I19a63}')

I19a66 = Parameter(name = 'I19a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3*KR6x3*yML3x3',
                   texname = '\\text{I19a66}')

I2a11 = Parameter(name = 'I2a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML1x1*yMU1x1',
                  texname = '\\text{I2a11}')

I2a12 = Parameter(name = 'I2a12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML1x2*yMU1x1',
                  texname = '\\text{I2a12}')

I2a13 = Parameter(name = 'I2a13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML1x3*yMU1x1',
                  texname = '\\text{I2a13}')

I2a21 = Parameter(name = 'I2a21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML2x1*yMU2x2',
                  texname = '\\text{I2a21}')

I2a22 = Parameter(name = 'I2a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML2x2*yMU2x2',
                  texname = '\\text{I2a22}')

I2a23 = Parameter(name = 'I2a23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML2x3*yMU2x2',
                  texname = '\\text{I2a23}')

I2a31 = Parameter(name = 'I2a31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML3x1*yMU3x3',
                  texname = '\\text{I2a31}')

I2a32 = Parameter(name = 'I2a32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML3x2*yMU3x3',
                  texname = '\\text{I2a32}')

I2a33 = Parameter(name = 'I2a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML3x3*yMU3x3',
                  texname = '\\text{I2a33}')

I20a11 = Parameter(name = 'I20a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KR1x1*yML1x1',
                   texname = '\\text{I20a11}')

I20a14 = Parameter(name = 'I20a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1*KR1x1*yML1x1',
                   texname = '\\text{I20a14}')

I20a22 = Parameter(name = 'I20a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KR2x2*yML2x2',
                   texname = '\\text{I20a22}')

I20a25 = Parameter(name = 'I20a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2*KR2x2*yML2x2',
                   texname = '\\text{I20a25}')

I20a33 = Parameter(name = 'I20a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KR3x3*yML3x3',
                   texname = '\\text{I20a33}')

I20a36 = Parameter(name = 'I20a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3*KR3x3*yML3x3',
                   texname = '\\text{I20a36}')

I20a41 = Parameter(name = 'I20a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KR4x1*yML1x1',
                   texname = '\\text{I20a41}')

I20a44 = Parameter(name = 'I20a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1*KR4x1*yML1x1',
                   texname = '\\text{I20a44}')

I20a52 = Parameter(name = 'I20a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KR5x2*yML2x2',
                   texname = '\\text{I20a52}')

I20a55 = Parameter(name = 'I20a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2*KR5x2*yML2x2',
                   texname = '\\text{I20a55}')

I20a63 = Parameter(name = 'I20a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KR6x3*yML3x3',
                   texname = '\\text{I20a63}')

I20a66 = Parameter(name = 'I20a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3*KR6x3*yML3x3',
                   texname = '\\text{I20a66}')

I21a11 = Parameter(name = 'I21a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*yML1x1',
                   texname = '\\text{I21a11}')

I21a22 = Parameter(name = 'I21a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*yML2x2',
                   texname = '\\text{I21a22}')

I21a33 = Parameter(name = 'I21a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*yML3x3',
                   texname = '\\text{I21a33}')

I21a41 = Parameter(name = 'I21a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1*yML1x1',
                   texname = '\\text{I21a41}')

I21a52 = Parameter(name = 'I21a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2*yML2x2',
                   texname = '\\text{I21a52}')

I21a63 = Parameter(name = 'I21a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3*yML3x3',
                   texname = '\\text{I21a63}')

I22a11 = Parameter(name = 'I22a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1**2*KR1x1*yNL1x1 + KL1x1*KL4x1*KR4x1*yNL4x4',
                   texname = '\\text{I22a11}')

I22a22 = Parameter(name = 'I22a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2**2*KR2x2*yNL2x2 + KL2x2*KL5x2*KR5x2*yNL5x5',
                   texname = '\\text{I22a22}')

I22a33 = Parameter(name = 'I22a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3**2*KR3x3*yNL3x3 + KL3x3*KL6x3*KR6x3*yNL6x6',
                   texname = '\\text{I22a33}')

I22a41 = Parameter(name = 'I22a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1*KR1x1*yNL1x1 + KL4x1**2*KR4x1*yNL4x4',
                   texname = '\\text{I22a41}')

I22a52 = Parameter(name = 'I22a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2*KR2x2*yNL2x2 + KL5x2**2*KR5x2*yNL5x5',
                   texname = '\\text{I22a52}')

I22a63 = Parameter(name = 'I22a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3*KR3x3*yNL3x3 + KL6x3**2*KR6x3*yNL6x6',
                   texname = '\\text{I22a63}')

I23a11 = Parameter(name = 'I23a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1*yML1x1',
                   texname = '\\text{I23a11}')

I23a22 = Parameter(name = 'I23a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2*yML2x2',
                   texname = '\\text{I23a22}')

I23a33 = Parameter(name = 'I23a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3*yML3x3',
                   texname = '\\text{I23a33}')

I23a41 = Parameter(name = 'I23a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR4x1*yML1x1',
                   texname = '\\text{I23a41}')

I23a52 = Parameter(name = 'I23a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR5x2*yML2x2',
                   texname = '\\text{I23a52}')

I23a63 = Parameter(name = 'I23a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR6x3*yML3x3',
                   texname = '\\text{I23a63}')

I24a11 = Parameter(name = 'I24a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KR1x1**2*yNL1x1 + KL4x1*KR1x1*KR4x1*yNL4x4',
                   texname = '\\text{I24a11}')

I24a22 = Parameter(name = 'I24a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KR2x2**2*yNL2x2 + KL5x2*KR2x2*KR5x2*yNL5x5',
                   texname = '\\text{I24a22}')

I24a33 = Parameter(name = 'I24a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KR3x3**2*yNL3x3 + KL6x3*KR3x3*KR6x3*yNL6x6',
                   texname = '\\text{I24a33}')

I24a41 = Parameter(name = 'I24a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KR1x1*KR4x1*yNL1x1 + KL4x1*KR4x1**2*yNL4x4',
                   texname = '\\text{I24a41}')

I24a52 = Parameter(name = 'I24a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KR2x2*KR5x2*yNL2x2 + KL5x2*KR5x2**2*yNL5x5',
                   texname = '\\text{I24a52}')

I24a63 = Parameter(name = 'I24a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KR3x3*KR6x3*yNL3x3 + KL6x3*KR6x3**2*yNL6x6',
                   texname = '\\text{I24a63}')

I25a11 = Parameter(name = 'I25a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1*yML1x1',
                   texname = '\\text{I25a11}')

I25a22 = Parameter(name = 'I25a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2*yML2x2',
                   texname = '\\text{I25a22}')

I25a33 = Parameter(name = 'I25a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3*yML3x3',
                   texname = '\\text{I25a33}')

I25a41 = Parameter(name = 'I25a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR4x1*yML1x1',
                   texname = '\\text{I25a41}')

I25a52 = Parameter(name = 'I25a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR5x2*yML2x2',
                   texname = '\\text{I25a52}')

I25a63 = Parameter(name = 'I25a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR6x3*yML3x3',
                   texname = '\\text{I25a63}')

I26a11 = Parameter(name = 'I26a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KR1x1**2*yNL1x1 + KL4x1*KR1x1*KR4x1*yNL4x4',
                   texname = '\\text{I26a11}')

I26a22 = Parameter(name = 'I26a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KR2x2**2*yNL2x2 + KL5x2*KR2x2*KR5x2*yNL5x5',
                   texname = '\\text{I26a22}')

I26a33 = Parameter(name = 'I26a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KR3x3**2*yNL3x3 + KL6x3*KR3x3*KR6x3*yNL6x6',
                   texname = '\\text{I26a33}')

I26a41 = Parameter(name = 'I26a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KR1x1*KR4x1*yNL1x1 + KL4x1*KR4x1**2*yNL4x4',
                   texname = '\\text{I26a41}')

I26a52 = Parameter(name = 'I26a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KR2x2*KR5x2*yNL2x2 + KL5x2*KR5x2**2*yNL5x5',
                   texname = '\\text{I26a52}')

I26a63 = Parameter(name = 'I26a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KR3x3*KR6x3*yNL3x3 + KL6x3*KR6x3**2*yNL6x6',
                   texname = '\\text{I26a63}')

I27a11 = Parameter(name = 'I27a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*yML1x1',
                   texname = '\\text{I27a11}')

I27a22 = Parameter(name = 'I27a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*yML2x2',
                   texname = '\\text{I27a22}')

I27a33 = Parameter(name = 'I27a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*yML3x3',
                   texname = '\\text{I27a33}')

I27a41 = Parameter(name = 'I27a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1*yML1x1',
                   texname = '\\text{I27a41}')

I27a52 = Parameter(name = 'I27a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2*yML2x2',
                   texname = '\\text{I27a52}')

I27a63 = Parameter(name = 'I27a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3*yML3x3',
                   texname = '\\text{I27a63}')

I28a11 = Parameter(name = 'I28a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1**2*KR1x1*yNL1x1 + KL1x1*KL4x1*KR4x1*yNL4x4',
                   texname = '\\text{I28a11}')

I28a22 = Parameter(name = 'I28a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2**2*KR2x2*yNL2x2 + KL2x2*KL5x2*KR5x2*yNL5x5',
                   texname = '\\text{I28a22}')

I28a33 = Parameter(name = 'I28a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3**2*KR3x3*yNL3x3 + KL3x3*KL6x3*KR6x3*yNL6x6',
                   texname = '\\text{I28a33}')

I28a41 = Parameter(name = 'I28a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1*KR1x1*yNL1x1 + KL4x1**2*KR4x1*yNL4x4',
                   texname = '\\text{I28a41}')

I28a52 = Parameter(name = 'I28a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2*KR2x2*yNL2x2 + KL5x2**2*KR5x2*yNL5x5',
                   texname = '\\text{I28a52}')

I28a63 = Parameter(name = 'I28a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3*KR3x3*yNL3x3 + KL6x3**2*KR6x3*yNL6x6',
                   texname = '\\text{I28a63}')

I29a11 = Parameter(name = 'I29a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KR1x1*yNL1x1 + KL4x1*KR4x1*yNL4x4',
                   texname = '\\text{I29a11}')

I29a22 = Parameter(name = 'I29a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KR2x2*yNL2x2 + KL5x2*KR5x2*yNL5x5',
                   texname = '\\text{I29a22}')

I29a33 = Parameter(name = 'I29a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KR3x3*yNL3x3 + KL6x3*KR6x3*yNL6x6',
                   texname = '\\text{I29a33}')

I3a11 = Parameter(name = 'I3a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML1x1*yDO1x1',
                  texname = '\\text{I3a11}')

I3a12 = Parameter(name = 'I3a12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML1x2*yDO2x2',
                  texname = '\\text{I3a12}')

I3a13 = Parameter(name = 'I3a13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML1x3*yDO3x3',
                  texname = '\\text{I3a13}')

I3a21 = Parameter(name = 'I3a21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML2x1*yDO1x1',
                  texname = '\\text{I3a21}')

I3a22 = Parameter(name = 'I3a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML2x2*yDO2x2',
                  texname = '\\text{I3a22}')

I3a23 = Parameter(name = 'I3a23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML2x3*yDO3x3',
                  texname = '\\text{I3a23}')

I3a31 = Parameter(name = 'I3a31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML3x1*yDO1x1',
                  texname = '\\text{I3a31}')

I3a32 = Parameter(name = 'I3a32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML3x2*yDO2x2',
                  texname = '\\text{I3a32}')

I3a33 = Parameter(name = 'I3a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML3x3*yDO3x3',
                  texname = '\\text{I3a33}')

I30a11 = Parameter(name = 'I30a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KR1x1*yNL1x1 + KL4x1*KR4x1*yNL4x4',
                   texname = '\\text{I30a11}')

I30a22 = Parameter(name = 'I30a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KR2x2*yNL2x2 + KL5x2*KR5x2*yNL5x5',
                   texname = '\\text{I30a22}')

I30a33 = Parameter(name = 'I30a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KR3x3*yNL3x3 + KL6x3*KR6x3*yNL6x6',
                   texname = '\\text{I30a33}')

I31a11 = Parameter(name = 'I31a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**4*yNL1x1 + KR1x1**2*KR4x1**2*yNL4x4',
                   texname = '\\text{I31a11}')

I31a14 = Parameter(name = 'I31a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**3*KR4x1*yNL1x1 + KR1x1*KR4x1**3*yNL4x4',
                   texname = '\\text{I31a14}')

I31a22 = Parameter(name = 'I31a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**4*yNL2x2 + KR2x2**2*KR5x2**2*yNL5x5',
                   texname = '\\text{I31a22}')

I31a25 = Parameter(name = 'I31a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**3*KR5x2*yNL2x2 + KR2x2*KR5x2**3*yNL5x5',
                   texname = '\\text{I31a25}')

I31a33 = Parameter(name = 'I31a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**4*yNL3x3 + KR3x3**2*KR6x3**2*yNL6x6',
                   texname = '\\text{I31a33}')

I31a36 = Parameter(name = 'I31a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**3*KR6x3*yNL3x3 + KR3x3*KR6x3**3*yNL6x6',
                   texname = '\\text{I31a36}')

I31a41 = Parameter(name = 'I31a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**3*KR4x1*yNL1x1 + KR1x1*KR4x1**3*yNL4x4',
                   texname = '\\text{I31a41}')

I31a44 = Parameter(name = 'I31a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**2*KR4x1**2*yNL1x1 + KR4x1**4*yNL4x4',
                   texname = '\\text{I31a44}')

I31a52 = Parameter(name = 'I31a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**3*KR5x2*yNL2x2 + KR2x2*KR5x2**3*yNL5x5',
                   texname = '\\text{I31a52}')

I31a55 = Parameter(name = 'I31a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**2*KR5x2**2*yNL2x2 + KR5x2**4*yNL5x5',
                   texname = '\\text{I31a55}')

I31a63 = Parameter(name = 'I31a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**3*KR6x3*yNL3x3 + KR3x3*KR6x3**3*yNL6x6',
                   texname = '\\text{I31a63}')

I31a66 = Parameter(name = 'I31a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**2*KR6x3**2*yNL3x3 + KR6x3**4*yNL6x6',
                   texname = '\\text{I31a66}')

I32a11 = Parameter(name = 'I32a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**4*yNL1x1 + KR1x1**2*KR4x1**2*yNL4x4',
                   texname = '\\text{I32a11}')

I32a14 = Parameter(name = 'I32a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**3*KR4x1*yNL1x1 + KR1x1*KR4x1**3*yNL4x4',
                   texname = '\\text{I32a14}')

I32a22 = Parameter(name = 'I32a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**4*yNL2x2 + KR2x2**2*KR5x2**2*yNL5x5',
                   texname = '\\text{I32a22}')

I32a25 = Parameter(name = 'I32a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**3*KR5x2*yNL2x2 + KR2x2*KR5x2**3*yNL5x5',
                   texname = '\\text{I32a25}')

I32a33 = Parameter(name = 'I32a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**4*yNL3x3 + KR3x3**2*KR6x3**2*yNL6x6',
                   texname = '\\text{I32a33}')

I32a36 = Parameter(name = 'I32a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**3*KR6x3*yNL3x3 + KR3x3*KR6x3**3*yNL6x6',
                   texname = '\\text{I32a36}')

I32a41 = Parameter(name = 'I32a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**3*KR4x1*yNL1x1 + KR1x1*KR4x1**3*yNL4x4',
                   texname = '\\text{I32a41}')

I32a44 = Parameter(name = 'I32a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**2*KR4x1**2*yNL1x1 + KR4x1**4*yNL4x4',
                   texname = '\\text{I32a44}')

I32a52 = Parameter(name = 'I32a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**3*KR5x2*yNL2x2 + KR2x2*KR5x2**3*yNL5x5',
                   texname = '\\text{I32a52}')

I32a55 = Parameter(name = 'I32a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**2*KR5x2**2*yNL2x2 + KR5x2**4*yNL5x5',
                   texname = '\\text{I32a55}')

I32a63 = Parameter(name = 'I32a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**3*KR6x3*yNL3x3 + KR3x3*KR6x3**3*yNL6x6',
                   texname = '\\text{I32a63}')

I32a66 = Parameter(name = 'I32a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**2*KR6x3**2*yNL3x3 + KR6x3**4*yNL6x6',
                   texname = '\\text{I32a66}')

I33a11 = Parameter(name = 'I33a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**4*yNL1x1 + KR1x1**2*KR4x1**2*yNL4x4',
                   texname = '\\text{I33a11}')

I33a14 = Parameter(name = 'I33a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**3*KR4x1*yNL1x1 + KR1x1*KR4x1**3*yNL4x4',
                   texname = '\\text{I33a14}')

I33a22 = Parameter(name = 'I33a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**4*yNL2x2 + KR2x2**2*KR5x2**2*yNL5x5',
                   texname = '\\text{I33a22}')

I33a25 = Parameter(name = 'I33a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**3*KR5x2*yNL2x2 + KR2x2*KR5x2**3*yNL5x5',
                   texname = '\\text{I33a25}')

I33a33 = Parameter(name = 'I33a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**4*yNL3x3 + KR3x3**2*KR6x3**2*yNL6x6',
                   texname = '\\text{I33a33}')

I33a36 = Parameter(name = 'I33a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**3*KR6x3*yNL3x3 + KR3x3*KR6x3**3*yNL6x6',
                   texname = '\\text{I33a36}')

I33a41 = Parameter(name = 'I33a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**3*KR4x1*yNL1x1 + KR1x1*KR4x1**3*yNL4x4',
                   texname = '\\text{I33a41}')

I33a44 = Parameter(name = 'I33a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**2*KR4x1**2*yNL1x1 + KR4x1**4*yNL4x4',
                   texname = '\\text{I33a44}')

I33a52 = Parameter(name = 'I33a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**3*KR5x2*yNL2x2 + KR2x2*KR5x2**3*yNL5x5',
                   texname = '\\text{I33a52}')

I33a55 = Parameter(name = 'I33a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**2*KR5x2**2*yNL2x2 + KR5x2**4*yNL5x5',
                   texname = '\\text{I33a55}')

I33a63 = Parameter(name = 'I33a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**3*KR6x3*yNL3x3 + KR3x3*KR6x3**3*yNL6x6',
                   texname = '\\text{I33a63}')

I33a66 = Parameter(name = 'I33a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**2*KR6x3**2*yNL3x3 + KR6x3**4*yNL6x6',
                   texname = '\\text{I33a66}')

I34a11 = Parameter(name = 'I34a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**4*yNL1x1 + KR1x1**2*KR4x1**2*yNL4x4',
                   texname = '\\text{I34a11}')

I34a14 = Parameter(name = 'I34a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**3*KR4x1*yNL1x1 + KR1x1*KR4x1**3*yNL4x4',
                   texname = '\\text{I34a14}')

I34a22 = Parameter(name = 'I34a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**4*yNL2x2 + KR2x2**2*KR5x2**2*yNL5x5',
                   texname = '\\text{I34a22}')

I34a25 = Parameter(name = 'I34a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**3*KR5x2*yNL2x2 + KR2x2*KR5x2**3*yNL5x5',
                   texname = '\\text{I34a25}')

I34a33 = Parameter(name = 'I34a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**4*yNL3x3 + KR3x3**2*KR6x3**2*yNL6x6',
                   texname = '\\text{I34a33}')

I34a36 = Parameter(name = 'I34a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**3*KR6x3*yNL3x3 + KR3x3*KR6x3**3*yNL6x6',
                   texname = '\\text{I34a36}')

I34a41 = Parameter(name = 'I34a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**3*KR4x1*yNL1x1 + KR1x1*KR4x1**3*yNL4x4',
                   texname = '\\text{I34a41}')

I34a44 = Parameter(name = 'I34a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**2*KR4x1**2*yNL1x1 + KR4x1**4*yNL4x4',
                   texname = '\\text{I34a44}')

I34a52 = Parameter(name = 'I34a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**3*KR5x2*yNL2x2 + KR2x2*KR5x2**3*yNL5x5',
                   texname = '\\text{I34a52}')

I34a55 = Parameter(name = 'I34a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**2*KR5x2**2*yNL2x2 + KR5x2**4*yNL5x5',
                   texname = '\\text{I34a55}')

I34a63 = Parameter(name = 'I34a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**3*KR6x3*yNL3x3 + KR3x3*KR6x3**3*yNL6x6',
                   texname = '\\text{I34a63}')

I34a66 = Parameter(name = 'I34a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**2*KR6x3**2*yNL3x3 + KR6x3**4*yNL6x6',
                   texname = '\\text{I34a66}')

I35a11 = Parameter(name = 'I35a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**2*yNL1x1 + KR4x1**2*yNL4x4',
                   texname = '\\text{I35a11}')

I35a22 = Parameter(name = 'I35a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**2*yNL2x2 + KR5x2**2*yNL5x5',
                   texname = '\\text{I35a22}')

I35a33 = Parameter(name = 'I35a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**2*yNL3x3 + KR6x3**2*yNL6x6',
                   texname = '\\text{I35a33}')

I36a11 = Parameter(name = 'I36a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**2*yNL1x1 + KR4x1**2*yNL4x4',
                   texname = '\\text{I36a11}')

I36a22 = Parameter(name = 'I36a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**2*yNL2x2 + KR5x2**2*yNL5x5',
                   texname = '\\text{I36a22}')

I36a33 = Parameter(name = 'I36a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**2*yNL3x3 + KR6x3**2*yNL6x6',
                   texname = '\\text{I36a33}')

I37a11 = Parameter(name = 'I37a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1**2*KR1x1**2*Wl1x1**2*yNL1x1 + KL1x1**2*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I37a11}')

I37a14 = Parameter(name = 'I37a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1*KR1x1**2*Wl1x1**2*yNL1x1 + KL1x1*KL4x1*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I37a14}')

I37a22 = Parameter(name = 'I37a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2**2*KR2x2**2*Wl2x2**2*yNL2x2 + KL2x2**2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I37a22}')

I37a25 = Parameter(name = 'I37a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2*KR2x2**2*Wl2x2**2*yNL2x2 + KL2x2*KL5x2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I37a25}')

I37a33 = Parameter(name = 'I37a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3**2*KR3x3**2*Wl3x3**2*yNL3x3 + KL3x3**2*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I37a33}')

I37a36 = Parameter(name = 'I37a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3*KR3x3**2*Wl3x3**2*yNL3x3 + KL3x3*KL6x3*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I37a36}')

I37a41 = Parameter(name = 'I37a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1*KR1x1**2*Wl1x1**2*yNL1x1 + KL1x1*KL4x1*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I37a41}')

I37a44 = Parameter(name = 'I37a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1**2*KR1x1**2*Wl1x1**2*yNL1x1 + KL4x1**2*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I37a44}')

I37a52 = Parameter(name = 'I37a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2*KR2x2**2*Wl2x2**2*yNL2x2 + KL2x2*KL5x2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I37a52}')

I37a55 = Parameter(name = 'I37a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2**2*KR2x2**2*Wl2x2**2*yNL2x2 + KL5x2**2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I37a55}')

I37a63 = Parameter(name = 'I37a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3*KR3x3**2*Wl3x3**2*yNL3x3 + KL3x3*KL6x3*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I37a63}')

I37a66 = Parameter(name = 'I37a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3**2*KR3x3**2*Wl3x3**2*yNL3x3 + KL6x3**2*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I37a66}')

I38a11 = Parameter(name = 'I38a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1**2*KR1x1**2*Wl1x1**2*yNL1x1 + KL1x1**2*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I38a11}')

I38a14 = Parameter(name = 'I38a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1*KR1x1**2*Wl1x1**2*yNL1x1 + KL1x1*KL4x1*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I38a14}')

I38a22 = Parameter(name = 'I38a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2**2*KR2x2**2*Wl2x2**2*yNL2x2 + KL2x2**2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I38a22}')

I38a25 = Parameter(name = 'I38a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2*KR2x2**2*Wl2x2**2*yNL2x2 + KL2x2*KL5x2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I38a25}')

I38a33 = Parameter(name = 'I38a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3**2*KR3x3**2*Wl3x3**2*yNL3x3 + KL3x3**2*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I38a33}')

I38a36 = Parameter(name = 'I38a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3*KR3x3**2*Wl3x3**2*yNL3x3 + KL3x3*KL6x3*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I38a36}')

I38a41 = Parameter(name = 'I38a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1*KR1x1**2*Wl1x1**2*yNL1x1 + KL1x1*KL4x1*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I38a41}')

I38a44 = Parameter(name = 'I38a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1**2*KR1x1**2*Wl1x1**2*yNL1x1 + KL4x1**2*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I38a44}')

I38a52 = Parameter(name = 'I38a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2*KR2x2**2*Wl2x2**2*yNL2x2 + KL2x2*KL5x2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I38a52}')

I38a55 = Parameter(name = 'I38a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2**2*KR2x2**2*Wl2x2**2*yNL2x2 + KL5x2**2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I38a55}')

I38a63 = Parameter(name = 'I38a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3*KR3x3**2*Wl3x3**2*yNL3x3 + KL3x3*KL6x3*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I38a63}')

I38a66 = Parameter(name = 'I38a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3**2*KR3x3**2*Wl3x3**2*yNL3x3 + KL6x3**2*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I38a66}')

I39a11 = Parameter(name = 'I39a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1**2*KR1x1**2*Wl1x1**2*yNL1x1 + KL1x1**2*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I39a11}')

I39a14 = Parameter(name = 'I39a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1*KR1x1**2*Wl1x1**2*yNL1x1 + KL1x1*KL4x1*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I39a14}')

I39a22 = Parameter(name = 'I39a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2**2*KR2x2**2*Wl2x2**2*yNL2x2 + KL2x2**2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I39a22}')

I39a25 = Parameter(name = 'I39a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2*KR2x2**2*Wl2x2**2*yNL2x2 + KL2x2*KL5x2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I39a25}')

I39a33 = Parameter(name = 'I39a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3**2*KR3x3**2*Wl3x3**2*yNL3x3 + KL3x3**2*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I39a33}')

I39a36 = Parameter(name = 'I39a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3*KR3x3**2*Wl3x3**2*yNL3x3 + KL3x3*KL6x3*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I39a36}')

I39a41 = Parameter(name = 'I39a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1*KR1x1**2*Wl1x1**2*yNL1x1 + KL1x1*KL4x1*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I39a41}')

I39a44 = Parameter(name = 'I39a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1**2*KR1x1**2*Wl1x1**2*yNL1x1 + KL4x1**2*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I39a44}')

I39a52 = Parameter(name = 'I39a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2*KR2x2**2*Wl2x2**2*yNL2x2 + KL2x2*KL5x2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I39a52}')

I39a55 = Parameter(name = 'I39a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2**2*KR2x2**2*Wl2x2**2*yNL2x2 + KL5x2**2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I39a55}')

I39a63 = Parameter(name = 'I39a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3*KR3x3**2*Wl3x3**2*yNL3x3 + KL3x3*KL6x3*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I39a63}')

I39a66 = Parameter(name = 'I39a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3**2*KR3x3**2*Wl3x3**2*yNL3x3 + KL6x3**2*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I39a66}')

I4a11 = Parameter(name = 'I4a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR1x1*yMU1x1',
                  texname = '\\text{I4a11}')

I4a12 = Parameter(name = 'I4a12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR1x2*yMU1x1',
                  texname = '\\text{I4a12}')

I4a13 = Parameter(name = 'I4a13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR1x3*yMU1x1',
                  texname = '\\text{I4a13}')

I4a21 = Parameter(name = 'I4a21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR2x1*yMU2x2',
                  texname = '\\text{I4a21}')

I4a22 = Parameter(name = 'I4a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR2x2*yMU2x2',
                  texname = '\\text{I4a22}')

I4a23 = Parameter(name = 'I4a23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR2x3*yMU2x2',
                  texname = '\\text{I4a23}')

I4a31 = Parameter(name = 'I4a31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR3x1*yMU3x3',
                  texname = '\\text{I4a31}')

I4a32 = Parameter(name = 'I4a32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR3x2*yMU3x3',
                  texname = '\\text{I4a32}')

I4a33 = Parameter(name = 'I4a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR3x3*yMU3x3',
                  texname = '\\text{I4a33}')

I40a11 = Parameter(name = 'I40a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1**2*KR1x1**2*Wl1x1**2*yNL1x1 + KL1x1**2*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I40a11}')

I40a14 = Parameter(name = 'I40a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1*KR1x1**2*Wl1x1**2*yNL1x1 + KL1x1*KL4x1*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I40a14}')

I40a22 = Parameter(name = 'I40a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2**2*KR2x2**2*Wl2x2**2*yNL2x2 + KL2x2**2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I40a22}')

I40a25 = Parameter(name = 'I40a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2*KR2x2**2*Wl2x2**2*yNL2x2 + KL2x2*KL5x2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I40a25}')

I40a33 = Parameter(name = 'I40a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3**2*KR3x3**2*Wl3x3**2*yNL3x3 + KL3x3**2*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I40a33}')

I40a36 = Parameter(name = 'I40a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3*KR3x3**2*Wl3x3**2*yNL3x3 + KL3x3*KL6x3*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I40a36}')

I40a41 = Parameter(name = 'I40a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1*KR1x1**2*Wl1x1**2*yNL1x1 + KL1x1*KL4x1*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I40a41}')

I40a44 = Parameter(name = 'I40a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1**2*KR1x1**2*Wl1x1**2*yNL1x1 + KL4x1**2*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I40a44}')

I40a52 = Parameter(name = 'I40a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2*KR2x2**2*Wl2x2**2*yNL2x2 + KL2x2*KL5x2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I40a52}')

I40a55 = Parameter(name = 'I40a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2**2*KR2x2**2*Wl2x2**2*yNL2x2 + KL5x2**2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I40a55}')

I40a63 = Parameter(name = 'I40a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3*KR3x3**2*Wl3x3**2*yNL3x3 + KL3x3*KL6x3*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I40a63}')

I40a66 = Parameter(name = 'I40a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3**2*KR3x3**2*Wl3x3**2*yNL3x3 + KL6x3**2*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I40a66}')

I41a11 = Parameter(name = 'I41a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KR1x1**2*Wl1x1**2*yNL1x1 + KL1x1*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I41a11}')

I41a22 = Parameter(name = 'I41a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KR2x2**2*Wl2x2**2*yNL2x2 + KL2x2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I41a22}')

I41a33 = Parameter(name = 'I41a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KR3x3**2*Wl3x3**2*yNL3x3 + KL3x3*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I41a33}')

I41a41 = Parameter(name = 'I41a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1*KR1x1**2*Wl1x1**2*yNL1x1 + KL4x1*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I41a41}')

I41a52 = Parameter(name = 'I41a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2*KR2x2**2*Wl2x2**2*yNL2x2 + KL5x2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I41a52}')

I41a63 = Parameter(name = 'I41a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3*KR3x3**2*Wl3x3**2*yNL3x3 + KL6x3*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I41a63}')

I42a11 = Parameter(name = 'I42a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KR1x1**2*Wl1x1**2*yNL1x1 + KL1x1*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I42a11}')

I42a22 = Parameter(name = 'I42a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KR2x2**2*Wl2x2**2*yNL2x2 + KL2x2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I42a22}')

I42a33 = Parameter(name = 'I42a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KR3x3**2*Wl3x3**2*yNL3x3 + KL3x3*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I42a33}')

I42a41 = Parameter(name = 'I42a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1*KR1x1**2*Wl1x1**2*yNL1x1 + KL4x1*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I42a41}')

I42a52 = Parameter(name = 'I42a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2*KR2x2**2*Wl2x2**2*yNL2x2 + KL5x2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I42a52}')

I42a63 = Parameter(name = 'I42a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3*KR3x3**2*Wl3x3**2*yNL3x3 + KL6x3*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I42a63}')

I43a11 = Parameter(name = 'I43a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**2*Wl1x1**2*yNL1x1 + KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I43a11}')

I43a22 = Parameter(name = 'I43a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**2*Wl2x2**2*yNL2x2 + KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I43a22}')

I43a33 = Parameter(name = 'I43a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**2*Wl3x3**2*yNL3x3 + KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I43a33}')

I44a11 = Parameter(name = 'I44a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**2*Wl1x1**2*yNL1x1 + KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I44a11}')

I44a22 = Parameter(name = 'I44a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**2*Wl2x2**2*yNL2x2 + KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I44a22}')

I44a33 = Parameter(name = 'I44a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**2*Wl3x3**2*yNL3x3 + KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I44a33}')

I45a11 = Parameter(name = 'I45a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**2*yNL1x1 + KR4x1**2*yNL4x4',
                   texname = '\\text{I45a11}')

I45a22 = Parameter(name = 'I45a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**2*yNL2x2 + KR5x2**2*yNL5x5',
                   texname = '\\text{I45a22}')

I45a33 = Parameter(name = 'I45a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**2*yNL3x3 + KR6x3**2*yNL6x6',
                   texname = '\\text{I45a33}')

I46a11 = Parameter(name = 'I46a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**2*yNL1x1 + KR4x1**2*yNL4x4',
                   texname = '\\text{I46a11}')

I46a22 = Parameter(name = 'I46a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**2*yNL2x2 + KR5x2**2*yNL5x5',
                   texname = '\\text{I46a22}')

I46a33 = Parameter(name = 'I46a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**2*yNL3x3 + KR6x3**2*yNL6x6',
                   texname = '\\text{I46a33}')

I47a11 = Parameter(name = 'I47a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KR1x1**2*Wl1x1**2*yNL1x1 + KL1x1*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I47a11}')

I47a22 = Parameter(name = 'I47a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KR2x2**2*Wl2x2**2*yNL2x2 + KL2x2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I47a22}')

I47a33 = Parameter(name = 'I47a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KR3x3**2*Wl3x3**2*yNL3x3 + KL3x3*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I47a33}')

I47a41 = Parameter(name = 'I47a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1*KR1x1**2*Wl1x1**2*yNL1x1 + KL4x1*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I47a41}')

I47a52 = Parameter(name = 'I47a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2*KR2x2**2*Wl2x2**2*yNL2x2 + KL5x2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I47a52}')

I47a63 = Parameter(name = 'I47a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3*KR3x3**2*Wl3x3**2*yNL3x3 + KL6x3*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I47a63}')

I48a11 = Parameter(name = 'I48a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KR1x1**2*Wl1x1**2*yNL1x1 + KL1x1*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I48a11}')

I48a22 = Parameter(name = 'I48a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KR2x2**2*Wl2x2**2*yNL2x2 + KL2x2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I48a22}')

I48a33 = Parameter(name = 'I48a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KR3x3**2*Wl3x3**2*yNL3x3 + KL3x3*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I48a33}')

I48a41 = Parameter(name = 'I48a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1*KR1x1**2*Wl1x1**2*yNL1x1 + KL4x1*KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I48a41}')

I48a52 = Parameter(name = 'I48a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2*KR2x2**2*Wl2x2**2*yNL2x2 + KL5x2*KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I48a52}')

I48a63 = Parameter(name = 'I48a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3*KR3x3**2*Wl3x3**2*yNL3x3 + KL6x3*KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I48a63}')

I49a11 = Parameter(name = 'I49a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**2*Wl1x1**2*yNL1x1 + KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I49a11}')

I49a22 = Parameter(name = 'I49a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**2*Wl2x2**2*yNL2x2 + KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I49a22}')

I49a33 = Parameter(name = 'I49a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**2*Wl3x3**2*yNL3x3 + KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I49a33}')

I5a11 = Parameter(name = 'I5a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML1x1*yMU1x1*complexconjugate(CKMR1x1) + CKML2x1*yMU2x2*complexconjugate(CKMR2x1) + CKML3x1*yMU3x3*complexconjugate(CKMR3x1)',
                  texname = '\\text{I5a11}')

I5a12 = Parameter(name = 'I5a12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML1x2*yMU1x1*complexconjugate(CKMR1x1) + CKML2x2*yMU2x2*complexconjugate(CKMR2x1) + CKML3x2*yMU3x3*complexconjugate(CKMR3x1)',
                  texname = '\\text{I5a12}')

I5a13 = Parameter(name = 'I5a13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML1x3*yMU1x1*complexconjugate(CKMR1x1) + CKML2x3*yMU2x2*complexconjugate(CKMR2x1) + CKML3x3*yMU3x3*complexconjugate(CKMR3x1)',
                  texname = '\\text{I5a13}')

I5a21 = Parameter(name = 'I5a21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML1x1*yMU1x1*complexconjugate(CKMR1x2) + CKML2x1*yMU2x2*complexconjugate(CKMR2x2) + CKML3x1*yMU3x3*complexconjugate(CKMR3x2)',
                  texname = '\\text{I5a21}')

I5a22 = Parameter(name = 'I5a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML1x2*yMU1x1*complexconjugate(CKMR1x2) + CKML2x2*yMU2x2*complexconjugate(CKMR2x2) + CKML3x2*yMU3x3*complexconjugate(CKMR3x2)',
                  texname = '\\text{I5a22}')

I5a23 = Parameter(name = 'I5a23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML1x3*yMU1x1*complexconjugate(CKMR1x2) + CKML2x3*yMU2x2*complexconjugate(CKMR2x2) + CKML3x3*yMU3x3*complexconjugate(CKMR3x2)',
                  texname = '\\text{I5a23}')

I5a31 = Parameter(name = 'I5a31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML1x1*yMU1x1*complexconjugate(CKMR1x3) + CKML2x1*yMU2x2*complexconjugate(CKMR2x3) + CKML3x1*yMU3x3*complexconjugate(CKMR3x3)',
                  texname = '\\text{I5a31}')

I5a32 = Parameter(name = 'I5a32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML1x2*yMU1x1*complexconjugate(CKMR1x3) + CKML2x2*yMU2x2*complexconjugate(CKMR2x3) + CKML3x2*yMU3x3*complexconjugate(CKMR3x3)',
                  texname = '\\text{I5a32}')

I5a33 = Parameter(name = 'I5a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML1x3*yMU1x1*complexconjugate(CKMR1x3) + CKML2x3*yMU2x2*complexconjugate(CKMR2x3) + CKML3x3*yMU3x3*complexconjugate(CKMR3x3)',
                  texname = '\\text{I5a33}')

I50a11 = Parameter(name = 'I50a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**2*Wl1x1**2*yNL1x1 + KR4x1**2*Wl1x1**2*yNL4x4',
                   texname = '\\text{I50a11}')

I50a22 = Parameter(name = 'I50a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**2*Wl2x2**2*yNL2x2 + KR5x2**2*Wl2x2**2*yNL5x5',
                   texname = '\\text{I50a22}')

I50a33 = Parameter(name = 'I50a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**2*Wl3x3**2*yNL3x3 + KR6x3**2*Wl3x3**2*yNL6x6',
                   texname = '\\text{I50a33}')

I51a11 = Parameter(name = 'I51a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**2',
                   texname = '\\text{I51a11}')

I51a14 = Parameter(name = 'I51a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1*KR4x1',
                   texname = '\\text{I51a14}')

I51a22 = Parameter(name = 'I51a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**2',
                   texname = '\\text{I51a22}')

I51a25 = Parameter(name = 'I51a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2*KR5x2',
                   texname = '\\text{I51a25}')

I51a33 = Parameter(name = 'I51a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**2',
                   texname = '\\text{I51a33}')

I51a36 = Parameter(name = 'I51a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3*KR6x3',
                   texname = '\\text{I51a36}')

I51a41 = Parameter(name = 'I51a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1*KR4x1',
                   texname = '\\text{I51a41}')

I51a44 = Parameter(name = 'I51a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR4x1**2',
                   texname = '\\text{I51a44}')

I51a52 = Parameter(name = 'I51a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2*KR5x2',
                   texname = '\\text{I51a52}')

I51a55 = Parameter(name = 'I51a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR5x2**2',
                   texname = '\\text{I51a55}')

I51a63 = Parameter(name = 'I51a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3*KR6x3',
                   texname = '\\text{I51a63}')

I51a66 = Parameter(name = 'I51a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR6x3**2',
                   texname = '\\text{I51a66}')

I52a11 = Parameter(name = 'I52a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1**2',
                   texname = '\\text{I52a11}')

I52a14 = Parameter(name = 'I52a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1*KR4x1',
                   texname = '\\text{I52a14}')

I52a22 = Parameter(name = 'I52a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2**2',
                   texname = '\\text{I52a22}')

I52a25 = Parameter(name = 'I52a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2*KR5x2',
                   texname = '\\text{I52a25}')

I52a33 = Parameter(name = 'I52a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3**2',
                   texname = '\\text{I52a33}')

I52a36 = Parameter(name = 'I52a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3*KR6x3',
                   texname = '\\text{I52a36}')

I52a41 = Parameter(name = 'I52a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR1x1*KR4x1',
                   texname = '\\text{I52a41}')

I52a44 = Parameter(name = 'I52a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR4x1**2',
                   texname = '\\text{I52a44}')

I52a52 = Parameter(name = 'I52a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR2x2*KR5x2',
                   texname = '\\text{I52a52}')

I52a55 = Parameter(name = 'I52a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR5x2**2',
                   texname = '\\text{I52a55}')

I52a63 = Parameter(name = 'I52a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR3x3*KR6x3',
                   texname = '\\text{I52a63}')

I52a66 = Parameter(name = 'I52a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KR6x3**2',
                   texname = '\\text{I52a66}')

I53a11 = Parameter(name = 'I53a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1**2',
                   texname = '\\text{I53a11}')

I53a14 = Parameter(name = 'I53a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1',
                   texname = '\\text{I53a14}')

I53a22 = Parameter(name = 'I53a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2**2',
                   texname = '\\text{I53a22}')

I53a25 = Parameter(name = 'I53a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2',
                   texname = '\\text{I53a25}')

I53a33 = Parameter(name = 'I53a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3**2',
                   texname = '\\text{I53a33}')

I53a36 = Parameter(name = 'I53a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3',
                   texname = '\\text{I53a36}')

I53a41 = Parameter(name = 'I53a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1',
                   texname = '\\text{I53a41}')

I53a44 = Parameter(name = 'I53a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1**2',
                   texname = '\\text{I53a44}')

I53a52 = Parameter(name = 'I53a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2',
                   texname = '\\text{I53a52}')

I53a55 = Parameter(name = 'I53a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2**2',
                   texname = '\\text{I53a55}')

I53a63 = Parameter(name = 'I53a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3',
                   texname = '\\text{I53a63}')

I53a66 = Parameter(name = 'I53a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3**2',
                   texname = '\\text{I53a66}')

I54a11 = Parameter(name = 'I54a11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1**2',
                   texname = '\\text{I54a11}')

I54a14 = Parameter(name = 'I54a14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1',
                   texname = '\\text{I54a14}')

I54a22 = Parameter(name = 'I54a22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2**2',
                   texname = '\\text{I54a22}')

I54a25 = Parameter(name = 'I54a25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2',
                   texname = '\\text{I54a25}')

I54a33 = Parameter(name = 'I54a33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3**2',
                   texname = '\\text{I54a33}')

I54a36 = Parameter(name = 'I54a36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3',
                   texname = '\\text{I54a36}')

I54a41 = Parameter(name = 'I54a41',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL1x1*KL4x1',
                   texname = '\\text{I54a41}')

I54a44 = Parameter(name = 'I54a44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL4x1**2',
                   texname = '\\text{I54a44}')

I54a52 = Parameter(name = 'I54a52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL2x2*KL5x2',
                   texname = '\\text{I54a52}')

I54a55 = Parameter(name = 'I54a55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL5x2**2',
                   texname = '\\text{I54a55}')

I54a63 = Parameter(name = 'I54a63',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL3x3*KL6x3',
                   texname = '\\text{I54a63}')

I54a66 = Parameter(name = 'I54a66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'KL6x3**2',
                   texname = '\\text{I54a66}')

I6a11 = Parameter(name = 'I6a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR1x1*yMU1x1*complexconjugate(CKML1x1) + CKMR2x1*yMU2x2*complexconjugate(CKML2x1) + CKMR3x1*yMU3x3*complexconjugate(CKML3x1)',
                  texname = '\\text{I6a11}')

I6a12 = Parameter(name = 'I6a12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR1x2*yMU1x1*complexconjugate(CKML1x1) + CKMR2x2*yMU2x2*complexconjugate(CKML2x1) + CKMR3x2*yMU3x3*complexconjugate(CKML3x1)',
                  texname = '\\text{I6a12}')

I6a13 = Parameter(name = 'I6a13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR1x3*yMU1x1*complexconjugate(CKML1x1) + CKMR2x3*yMU2x2*complexconjugate(CKML2x1) + CKMR3x3*yMU3x3*complexconjugate(CKML3x1)',
                  texname = '\\text{I6a13}')

I6a21 = Parameter(name = 'I6a21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR1x1*yMU1x1*complexconjugate(CKML1x2) + CKMR2x1*yMU2x2*complexconjugate(CKML2x2) + CKMR3x1*yMU3x3*complexconjugate(CKML3x2)',
                  texname = '\\text{I6a21}')

I6a22 = Parameter(name = 'I6a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR1x2*yMU1x1*complexconjugate(CKML1x2) + CKMR2x2*yMU2x2*complexconjugate(CKML2x2) + CKMR3x2*yMU3x3*complexconjugate(CKML3x2)',
                  texname = '\\text{I6a22}')

I6a23 = Parameter(name = 'I6a23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR1x3*yMU1x1*complexconjugate(CKML1x2) + CKMR2x3*yMU2x2*complexconjugate(CKML2x2) + CKMR3x3*yMU3x3*complexconjugate(CKML3x2)',
                  texname = '\\text{I6a23}')

I6a31 = Parameter(name = 'I6a31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR1x1*yMU1x1*complexconjugate(CKML1x3) + CKMR2x1*yMU2x2*complexconjugate(CKML2x3) + CKMR3x1*yMU3x3*complexconjugate(CKML3x3)',
                  texname = '\\text{I6a31}')

I6a32 = Parameter(name = 'I6a32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR1x2*yMU1x1*complexconjugate(CKML1x3) + CKMR2x2*yMU2x2*complexconjugate(CKML2x3) + CKMR3x2*yMU3x3*complexconjugate(CKML3x3)',
                  texname = '\\text{I6a32}')

I6a33 = Parameter(name = 'I6a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR1x3*yMU1x1*complexconjugate(CKML1x3) + CKMR2x3*yMU2x2*complexconjugate(CKML2x3) + CKMR3x3*yMU3x3*complexconjugate(CKML3x3)',
                  texname = '\\text{I6a33}')

I7a11 = Parameter(name = 'I7a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR1x1*yDO1x1*complexconjugate(CKML1x1) + CKMR1x2*yDO2x2*complexconjugate(CKML1x2) + CKMR1x3*yDO3x3*complexconjugate(CKML1x3)',
                  texname = '\\text{I7a11}')

I7a12 = Parameter(name = 'I7a12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR1x1*yDO1x1*complexconjugate(CKML2x1) + CKMR1x2*yDO2x2*complexconjugate(CKML2x2) + CKMR1x3*yDO3x3*complexconjugate(CKML2x3)',
                  texname = '\\text{I7a12}')

I7a13 = Parameter(name = 'I7a13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR1x1*yDO1x1*complexconjugate(CKML3x1) + CKMR1x2*yDO2x2*complexconjugate(CKML3x2) + CKMR1x3*yDO3x3*complexconjugate(CKML3x3)',
                  texname = '\\text{I7a13}')

I7a21 = Parameter(name = 'I7a21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR2x1*yDO1x1*complexconjugate(CKML1x1) + CKMR2x2*yDO2x2*complexconjugate(CKML1x2) + CKMR2x3*yDO3x3*complexconjugate(CKML1x3)',
                  texname = '\\text{I7a21}')

I7a22 = Parameter(name = 'I7a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR2x1*yDO1x1*complexconjugate(CKML2x1) + CKMR2x2*yDO2x2*complexconjugate(CKML2x2) + CKMR2x3*yDO3x3*complexconjugate(CKML2x3)',
                  texname = '\\text{I7a22}')

I7a23 = Parameter(name = 'I7a23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR2x1*yDO1x1*complexconjugate(CKML3x1) + CKMR2x2*yDO2x2*complexconjugate(CKML3x2) + CKMR2x3*yDO3x3*complexconjugate(CKML3x3)',
                  texname = '\\text{I7a23}')

I7a31 = Parameter(name = 'I7a31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR3x1*yDO1x1*complexconjugate(CKML1x1) + CKMR3x2*yDO2x2*complexconjugate(CKML1x2) + CKMR3x3*yDO3x3*complexconjugate(CKML1x3)',
                  texname = '\\text{I7a31}')

I7a32 = Parameter(name = 'I7a32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR3x1*yDO1x1*complexconjugate(CKML2x1) + CKMR3x2*yDO2x2*complexconjugate(CKML2x2) + CKMR3x3*yDO3x3*complexconjugate(CKML2x3)',
                  texname = '\\text{I7a32}')

I7a33 = Parameter(name = 'I7a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKMR3x1*yDO1x1*complexconjugate(CKML3x1) + CKMR3x2*yDO2x2*complexconjugate(CKML3x2) + CKMR3x3*yDO3x3*complexconjugate(CKML3x3)',
                  texname = '\\text{I7a33}')

I8a11 = Parameter(name = 'I8a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML1x1*yDO1x1*complexconjugate(CKMR1x1) + CKML1x2*yDO2x2*complexconjugate(CKMR1x2) + CKML1x3*yDO3x3*complexconjugate(CKMR1x3)',
                  texname = '\\text{I8a11}')

I8a12 = Parameter(name = 'I8a12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML1x1*yDO1x1*complexconjugate(CKMR2x1) + CKML1x2*yDO2x2*complexconjugate(CKMR2x2) + CKML1x3*yDO3x3*complexconjugate(CKMR2x3)',
                  texname = '\\text{I8a12}')

I8a13 = Parameter(name = 'I8a13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML1x1*yDO1x1*complexconjugate(CKMR3x1) + CKML1x2*yDO2x2*complexconjugate(CKMR3x2) + CKML1x3*yDO3x3*complexconjugate(CKMR3x3)',
                  texname = '\\text{I8a13}')

I8a21 = Parameter(name = 'I8a21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML2x1*yDO1x1*complexconjugate(CKMR1x1) + CKML2x2*yDO2x2*complexconjugate(CKMR1x2) + CKML2x3*yDO3x3*complexconjugate(CKMR1x3)',
                  texname = '\\text{I8a21}')

I8a22 = Parameter(name = 'I8a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML2x1*yDO1x1*complexconjugate(CKMR2x1) + CKML2x2*yDO2x2*complexconjugate(CKMR2x2) + CKML2x3*yDO3x3*complexconjugate(CKMR2x3)',
                  texname = '\\text{I8a22}')

I8a23 = Parameter(name = 'I8a23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML2x1*yDO1x1*complexconjugate(CKMR3x1) + CKML2x2*yDO2x2*complexconjugate(CKMR3x2) + CKML2x3*yDO3x3*complexconjugate(CKMR3x3)',
                  texname = '\\text{I8a23}')

I8a31 = Parameter(name = 'I8a31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML3x1*yDO1x1*complexconjugate(CKMR1x1) + CKML3x2*yDO2x2*complexconjugate(CKMR1x2) + CKML3x3*yDO3x3*complexconjugate(CKMR1x3)',
                  texname = '\\text{I8a31}')

I8a32 = Parameter(name = 'I8a32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML3x1*yDO1x1*complexconjugate(CKMR2x1) + CKML3x2*yDO2x2*complexconjugate(CKMR2x2) + CKML3x3*yDO3x3*complexconjugate(CKMR2x3)',
                  texname = '\\text{I8a32}')

I8a33 = Parameter(name = 'I8a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'CKML3x1*yDO1x1*complexconjugate(CKMR3x1) + CKML3x2*yDO2x2*complexconjugate(CKMR3x2) + CKML3x3*yDO3x3*complexconjugate(CKMR3x3)',
                  texname = '\\text{I8a33}')

I9a11 = Parameter(name = 'I9a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yDO1x1*complexconjugate(CKML1x1)',
                  texname = '\\text{I9a11}')

I9a12 = Parameter(name = 'I9a12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yDO1x1*complexconjugate(CKML2x1)',
                  texname = '\\text{I9a12}')

I9a13 = Parameter(name = 'I9a13',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yDO1x1*complexconjugate(CKML3x1)',
                  texname = '\\text{I9a13}')

I9a21 = Parameter(name = 'I9a21',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yDO2x2*complexconjugate(CKML1x2)',
                  texname = '\\text{I9a21}')

I9a22 = Parameter(name = 'I9a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yDO2x2*complexconjugate(CKML2x2)',
                  texname = '\\text{I9a22}')

I9a23 = Parameter(name = 'I9a23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yDO2x2*complexconjugate(CKML3x2)',
                  texname = '\\text{I9a23}')

I9a31 = Parameter(name = 'I9a31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yDO3x3*complexconjugate(CKML1x3)',
                  texname = '\\text{I9a31}')

I9a32 = Parameter(name = 'I9a32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yDO3x3*complexconjugate(CKML2x3)',
                  texname = '\\text{I9a32}')

I9a33 = Parameter(name = 'I9a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yDO3x3*complexconjugate(CKML3x3)',
                  texname = '\\text{I9a33}')


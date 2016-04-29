# This file was automatically created by FeynRules 2.1
# Mathematica version: 8.0 for Mac OS X x86 (64-bit) (November 6, 2010)
# Date: Tue 2 Dec 2014 06:46:52



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
               value = 0.000511,
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
                value = 101.2589,
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
                value = 1.3925030000000002e-9,
                texname = '\\text{WN1}',
                lhablock = 'DECAY',
                lhacode = [ 9900012 ])

WN2 = Parameter(name = 'WN2',
                nature = 'external',
                type = 'real',
                value = 1.3960940000000001e-9,
                texname = '\\text{WN2}',
                lhablock = 'DECAY',
                lhacode = [ 9900014 ])

WN3 = Parameter(name = 'WN3',
                nature = 'external',
                type = 'real',
                value = 1.3920740000000002e-9,
                texname = '\\text{WN3}',
                lhablock = 'DECAY',
                lhacode = [ 9900016 ])

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

VqL1x1 = Parameter(name = 'VqL1x1',
                   nature = 'internal',
                   type = 'complex',
                   value = 'cmath.sqrt(1 - s12**2)*cmath.sqrt(1 - s13**2)',
                   texname = '\\text{VqL1x1}')

VqL1x2 = Parameter(name = 'VqL1x2',
                   nature = 'internal',
                   type = 'complex',
                   value = 's12*cmath.sqrt(1 - s13**2)',
                   texname = '\\text{VqL1x2}')

VqL1x3 = Parameter(name = 'VqL1x3',
                   nature = 'internal',
                   type = 'complex',
                   value = 's13',
                   texname = '\\text{VqL1x3}')

VqL2x1 = Parameter(name = 'VqL2x1',
                   nature = 'internal',
                   type = 'complex',
                   value = '-(s13*s23*cmath.sqrt(1 - s12**2)) - s12*cmath.sqrt(1 - s23**2)',
                   texname = '\\text{VqL2x1}')

VqL2x2 = Parameter(name = 'VqL2x2',
                   nature = 'internal',
                   type = 'complex',
                   value = '-(s12*s13*s23) + cmath.sqrt(1 - s12**2)*cmath.sqrt(1 - s23**2)',
                   texname = '\\text{VqL2x2}')

VqL2x3 = Parameter(name = 'VqL2x3',
                   nature = 'internal',
                   type = 'complex',
                   value = 's23*cmath.sqrt(1 - s13**2)',
                   texname = '\\text{VqL2x3}')

VqL3x1 = Parameter(name = 'VqL3x1',
                   nature = 'internal',
                   type = 'complex',
                   value = 's12*s23 - s13*cmath.sqrt(1 - s12**2)*cmath.sqrt(1 - s23**2)',
                   texname = '\\text{VqL3x1}')

VqL3x2 = Parameter(name = 'VqL3x2',
                   nature = 'internal',
                   type = 'complex',
                   value = '-(s23*cmath.sqrt(1 - s12**2)) - s12*s13*cmath.sqrt(1 - s23**2)',
                   texname = '\\text{VqL3x2}')

VqL3x3 = Parameter(name = 'VqL3x3',
                   nature = 'internal',
                   type = 'complex',
                   value = 'cmath.sqrt(1 - s13**2)*cmath.sqrt(1 - s23**2)',
                   texname = '\\text{VqL3x3}')

VlL1x1 = Parameter(name = 'VlL1x1',
                   nature = 'internal',
                   type = 'real',
                   value = '1',
                   texname = '\\text{VlL1x1}')

VlL2x2 = Parameter(name = 'VlL2x2',
                   nature = 'internal',
                   type = 'real',
                   value = '1',
                   texname = '\\text{VlL2x2}')

VlL3x3 = Parameter(name = 'VlL3x3',
                   nature = 'internal',
                   type = 'real',
                   value = '1',
                   texname = '\\text{VlL3x3}')

VlR1x1 = Parameter(name = 'VlR1x1',
                   nature = 'internal',
                   type = 'real',
                   value = '1',
                   texname = '\\text{VlR1x1}')

VlR2x2 = Parameter(name = 'VlR2x2',
                   nature = 'internal',
                   type = 'real',
                   value = '1',
                   texname = '\\text{VlR2x2}')

VlR3x3 = Parameter(name = 'VlR3x3',
                   nature = 'internal',
                   type = 'real',
                   value = '1',
                   texname = '\\text{VlR3x3}')

MZ2 = Parameter(name = 'MZ2',
                nature = 'internal',
                type = 'real',
                value = '(cw*MW2*cmath.sqrt(2))/cmath.sqrt(1 - 2*sw**2)',
                texname = 'M_{\\text{Z2}}')

VqR1x1 = Parameter(name = 'VqR1x1',
                   nature = 'internal',
                   type = 'complex',
                   value = 'VqL1x1',
                   texname = '\\text{VqR1x1}')

VqR1x2 = Parameter(name = 'VqR1x2',
                   nature = 'internal',
                   type = 'complex',
                   value = 'VqL1x2',
                   texname = '\\text{VqR1x2}')

VqR1x3 = Parameter(name = 'VqR1x3',
                   nature = 'internal',
                   type = 'complex',
                   value = 'VqL1x3',
                   texname = '\\text{VqR1x3}')

VqR2x1 = Parameter(name = 'VqR2x1',
                   nature = 'internal',
                   type = 'complex',
                   value = 'VqL2x1',
                   texname = '\\text{VqR2x1}')

VqR2x2 = Parameter(name = 'VqR2x2',
                   nature = 'internal',
                   type = 'complex',
                   value = 'VqL2x2',
                   texname = '\\text{VqR2x2}')

VqR2x3 = Parameter(name = 'VqR2x3',
                   nature = 'internal',
                   type = 'complex',
                   value = 'VqL2x3',
                   texname = '\\text{VqR2x3}')

VqR3x1 = Parameter(name = 'VqR3x1',
                   nature = 'internal',
                   type = 'complex',
                   value = 'VqL3x1',
                   texname = '\\text{VqR3x1}')

VqR3x2 = Parameter(name = 'VqR3x2',
                   nature = 'internal',
                   type = 'complex',
                   value = 'VqL3x2',
                   texname = '\\text{VqR3x2}')

VqR3x3 = Parameter(name = 'VqR3x3',
                   nature = 'internal',
                   type = 'complex',
                   value = 'VqL3x3',
                   texname = '\\text{VqR3x3}')

ee = Parameter(name = 'ee',
               nature = 'internal',
               type = 'real',
               value = '2*cmath.sqrt(aEW)*cmath.sqrt(cmath.pi)',
               texname = 'e')

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

sphi = Parameter(name = 'sphi',
                 nature = 'internal',
                 type = 'real',
                 value = '-((1 - 2*sw**2)**1.5*vev**2)/(4.*cw**4*vR**2)',
                 texname = 's_{\\phi }')

sxi = Parameter(name = 'sxi',
                nature = 'internal',
                type = 'real',
                value = '-((tanb*vev**2)/((1 + tanb**2)*vR**2))',
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

I1a11 = Parameter(name = 'I1a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'VlL1x1**2',
                  texname = '\\text{I1a11}')

I1a22 = Parameter(name = 'I1a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'VlL2x2**2',
                  texname = '\\text{I1a22}')

I1a33 = Parameter(name = 'I1a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'VlL3x3**2',
                  texname = '\\text{I1a33}')

I2a11 = Parameter(name = 'I2a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'VlL1x1**2',
                  texname = '\\text{I2a11}')

I2a22 = Parameter(name = 'I2a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'VlL2x2**2',
                  texname = '\\text{I2a22}')

I2a33 = Parameter(name = 'I2a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'VlL3x3**2',
                  texname = '\\text{I2a33}')

I3a11 = Parameter(name = 'I3a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'VlR1x1**2',
                  texname = '\\text{I3a11}')

I3a22 = Parameter(name = 'I3a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'VlR2x2**2',
                  texname = '\\text{I3a22}')

I3a33 = Parameter(name = 'I3a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'VlR3x3**2',
                  texname = '\\text{I3a33}')

I4a11 = Parameter(name = 'I4a11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'VlR1x1**2',
                  texname = '\\text{I4a11}')

I4a22 = Parameter(name = 'I4a22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'VlR2x2**2',
                  texname = '\\text{I4a22}')

I4a33 = Parameter(name = 'I4a33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'VlR3x3**2',
                  texname = '\\text{I4a33}')


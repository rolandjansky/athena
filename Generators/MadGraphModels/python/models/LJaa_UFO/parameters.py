# This file was automatically created by FeynRules 2.3.4
# Mathematica version: 9.0 for Mac OS X x86 (64-bit) (November 20, 2012)
# Date: Sun 24 Jan 2016 21:45:21



from object_library import all_parameters, Parameter


from function_library import complexconjugate, re, im, csc, sec, acsc, asec, cot

# This is a default parameter object representing 0.
ZERO = Parameter(name = 'ZERO',
                 nature = 'internal',
                 type = 'real',
                 value = '0.0',
                 texname = '0')

# User-defined parameters.
GHND12 = Parameter(name = 'GHND12',
                   nature = 'external',
                   type = 'real',
                   value = 1,
                   texname = '\\text{GHND12}',
                   lhablock = 'DS',
                   lhacode = [ 1 ])

GND12HD1 = Parameter(name = 'GND12HD1',
                     nature = 'external',
                     type = 'real',
                     value = 1,
                     texname = '\\text{GND12HD1}',
                     lhablock = 'DS',
                     lhacode = [ 2 ])

GND12ZD = Parameter(name = 'GND12ZD',
                    nature = 'external',
                    type = 'real',
                    value = 1,
                    texname = '\\text{GND12ZD}',
                    lhablock = 'DS',
                    lhacode = [ 3 ])

GHD1ZD = Parameter(name = 'GHD1ZD',
                   nature = 'external',
                   type = 'real',
                   value = 1,
                   texname = '\\text{GHD1ZD}',
                   lhablock = 'DS',
                   lhacode = [ 4 ])

GZDE = Parameter(name = 'GZDE',
                 nature = 'external',
                 type = 'real',
                 value = 1,
                 texname = '\\text{GZDE}',
                 lhablock = 'DS',
                 lhacode = [ 5 ])

GZDMU = Parameter(name = 'GZDMU',
                  nature = 'external',
                  type = 'real',
                  value = 1,
                  texname = '\\text{GZDMU}',
                  lhablock = 'DS',
                  lhacode = [ 6 ])

GZDPI = Parameter(name = 'GZDPI',
                  nature = 'external',
                  type = 'real',
                  value = 1,
                  texname = '\\text{GZDPI}',
                  lhablock = 'DS',
                  lhacode = [ 7 ])

cgg = Parameter(name = 'cgg',
                nature = 'external',
                type = 'real',
                value = 0.0121469,
                texname = '\\text{cgg}',
                lhablock = 'HIGGS',
                lhacode = [ 1 ])

cu = Parameter(name = 'cu',
               nature = 'external',
               type = 'real',
               value = 1,
               texname = 'c_u',
               lhablock = 'HIGGS',
               lhacode = [ 2 ])

cd = Parameter(name = 'cd',
               nature = 'external',
               type = 'real',
               value = 1,
               texname = 'c_d',
               lhablock = 'HIGGS',
               lhacode = [ 3 ])

cl = Parameter(name = 'cl',
               nature = 'external',
               type = 'real',
               value = 1,
               texname = 'c_l',
               lhablock = 'HIGGS',
               lhacode = [ 4 ])

cV = Parameter(name = 'cV',
               nature = 'external',
               type = 'real',
               value = 1,
               texname = 'c_V',
               lhablock = 'HIGGS',
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
               texname = '\\text{aS}',
               lhablock = 'SMINPUTS',
               lhacode = [ 3 ])

gs = Parameter(name = 'gs',
               nature = 'external',
               type = 'real',
               value = 1.22,
               texname = 'g_s',
               lhablock = 'SMINPUTS',
               lhacode = [ 4 ])

gL = Parameter(name = 'gL',
               nature = 'external',
               type = 'real',
               value = 0.650192,
               texname = 'g_L',
               lhablock = 'SMINPUTS',
               lhacode = [ 5 ])

gY = Parameter(name = 'gY',
               nature = 'external',
               type = 'real',
               value = 0.35776,
               texname = 'g_Y',
               lhablock = 'SMINPUTS',
               lhacode = [ 6 ])

mE = Parameter(name = 'mE',
               nature = 'external',
               type = 'real',
               value = 0.000510998928,
               texname = '\\text{mE}',
               lhablock = 'MASS',
               lhacode = [ 11 ])

mM = Parameter(name = 'mM',
               nature = 'external',
               type = 'real',
               value = 0.1056,
               texname = '\\text{mM}',
               lhablock = 'MASS',
               lhacode = [ 13 ])

mTau = Parameter(name = 'mTau',
                 nature = 'external',
                 type = 'real',
                 value = 1.777,
                 texname = '\\text{mTau}',
                 lhablock = 'MASS',
                 lhacode = [ 15 ])

mT = Parameter(name = 'mT',
               nature = 'external',
               type = 'real',
               value = 173.2,
               texname = '\\text{mT}',
               lhablock = 'MASS',
               lhacode = [ 6 ])

mB = Parameter(name = 'mB',
               nature = 'external',
               type = 'real',
               value = 5.1,
               texname = '\\text{mB}',
               lhablock = 'MASS',
               lhacode = [ 5 ])

ND1MASS = Parameter(name = 'ND1MASS',
                    nature = 'external',
                    type = 'real',
                    value = 2,
                    texname = '\\text{ND1MASS}',
                    lhablock = 'MASS',
                    lhacode = [ 3000015 ])

ND2MASS = Parameter(name = 'ND2MASS',
                    nature = 'external',
                    type = 'real',
                    value = 5,
                    texname = '\\text{ND2MASS}',
                    lhablock = 'MASS',
                    lhacode = [ 3000016 ])

mH = Parameter(name = 'mH',
               nature = 'external',
               type = 'real',
               value = 125.09,
               texname = '\\text{mH}',
               lhablock = 'MASS',
               lhacode = [ 25 ])

HD1MASS = Parameter(name = 'HD1MASS',
                    nature = 'external',
                    type = 'real',
                    value = 2,
                    texname = '\\text{HD1MASS}',
                    lhablock = 'MASS',
                    lhacode = [ 3000005 ])

MPI = Parameter(name = 'MPI',
                nature = 'external',
                type = 'real',
                value = 0.1396,
                texname = '\\text{MPI}',
                lhablock = 'MASS',
                lhacode = [ 211 ])

mZ = Parameter(name = 'mZ',
               nature = 'external',
               type = 'real',
               value = 91.1876,
               texname = '\\text{mZ}',
               lhablock = 'MASS',
               lhacode = [ 23 ])

mW = Parameter(name = 'mW',
               nature = 'external',
               type = 'real',
               value = 80.385,
               texname = '\\text{mW}',
               lhablock = 'MASS',
               lhacode = [ 24 ])

ZDMASS = Parameter(name = 'ZDMASS',
                   nature = 'external',
                   type = 'real',
                   value = 0.4,
                   texname = '\\text{ZDMASS}',
                   lhablock = 'MASS',
                   lhacode = [ 3000001 ])

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
               value = 0.004,
               texname = '\\text{WH}',
               lhablock = 'DECAY',
               lhacode = [ 25 ])

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

aEW = Parameter(name = 'aEW',
                nature = 'internal',
                type = 'real',
                value = '1/aEWM1',
                texname = '\\alpha _{\\text{EW}}')

cw = Parameter(name = 'cw',
               nature = 'internal',
               type = 'real',
               value = 'gL/cmath.sqrt(gL**2 + gY**2)',
               texname = 'c_w')

sw = Parameter(name = 'sw',
               nature = 'internal',
               type = 'real',
               value = 'gY/cmath.sqrt(gL**2 + gY**2)',
               texname = 's_w')

v = Parameter(name = 'v',
              nature = 'internal',
              type = 'real',
              value = '1/(2**0.25*cmath.sqrt(Gf))',
              texname = 'v')

ee = Parameter(name = 'ee',
               nature = 'internal',
               type = 'real',
               value = '2*cmath.sqrt(aEW)*cmath.sqrt(cmath.pi)',
               texname = 'e')


# This file was automatically created by FeynRules 2.3.1
# Mathematica version: 10.0 for Mac OS X x86 (64-bit) (September 10, 2014)
# Date: Sat 4 Jul 2015 04:38:51



from object_library import all_parameters, Parameter


from function_library import complexconjugate, re, im, csc, sec, acsc, asec, cot

# This is a default parameter object representing 0.
ZERO = Parameter(name = 'ZERO',
                 nature = 'internal',
                 type = 'real',
                 value = '0.0',
                 texname = '0')

# User-defined parameters.
kb = Parameter(name = 'kb',
               nature = 'external',
               type = 'real',
               value = 1,
               texname = 'k_b',
               lhablock = 'HPO2f',
               lhacode = [ 1 ])

kc = Parameter(name = 'kc',
               nature = 'external',
               type = 'real',
               value = 1,
               texname = 'k_b',
               lhablock = 'HPO2f',
               lhacode = [ 2 ])

ktau = Parameter(name = 'ktau',
                 nature = 'external',
                 type = 'real',
                 value = 1,
                 texname = 'k_{\\tau }',
                 lhablock = 'HPO2f',
                 lhacode = [ 3 ])

kmu = Parameter(name = 'kmu',
                nature = 'external',
                type = 'real',
                value = 1,
                texname = 'k_{\\mu }',
                lhablock = 'HPO2f',
                lhacode = [ 4 ])

lb = Parameter(name = 'lb',
               nature = 'external',
               type = 'real',
               value = 0,
               texname = '\\lambda _b{}^{\\text{CP}}',
               lhablock = 'HPO2f',
               lhacode = [ 11 ])

lc = Parameter(name = 'lc',
               nature = 'external',
               type = 'real',
               value = 0,
               texname = '\\lambda _c{}^{\\text{CP}}',
               lhablock = 'HPO2f',
               lhacode = [ 12 ])

ltau = Parameter(name = 'ltau',
                 nature = 'external',
                 type = 'real',
                 value = 0,
                 texname = '\\lambda _{\\tau }{}^{\\text{CP}}',
                 lhablock = 'HPO2f',
                 lhacode = [ 13 ])

lmu = Parameter(name = 'lmu',
                nature = 'external',
                type = 'real',
                value = 0,
                texname = '\\lambda _{\\mu }{}^{\\text{CP}}',
                lhablock = 'HPO2f',
                lhacode = [ 14 ])

kZZ = Parameter(name = 'kZZ',
                nature = 'external',
                type = 'real',
                value = 1,
                texname = 'k_{\\text{ZZ}}',
                lhablock = 'HPO4f',
                lhacode = [ 1 ])

kWW = Parameter(name = 'kWW',
                nature = 'external',
                type = 'real',
                value = 1,
                texname = 'k_{\\text{WW}}',
                lhablock = 'HPO4f',
                lhacode = [ 2 ])

kAA = Parameter(name = 'kAA',
                nature = 'external',
                type = 'real',
                value = 1,
                texname = '\\epsilon _{\\gamma ^2}',
                lhablock = 'HPO4f',
                lhacode = [ 3 ])

kZA = Parameter(name = 'kZA',
                nature = 'external',
                type = 'real',
                value = 1,
                texname = '\\epsilon _{\\gamma  Z}',
                lhablock = 'HPO4f',
                lhacode = [ 4 ])

eZZ = Parameter(name = 'eZZ',
                nature = 'external',
                type = 'real',
                value = 0,
                texname = '\\epsilon _{\\text{ZZ}}',
                lhablock = 'HPO4f',
                lhacode = [ 5 ])

eWW = Parameter(name = 'eWW',
                nature = 'external',
                type = 'real',
                value = 0,
                texname = '\\epsilon _{\\text{WW}}',
                lhablock = 'HPO4f',
                lhacode = [ 6 ])

lAACP = Parameter(name = 'lAACP',
                  nature = 'external',
                  type = 'real',
                  value = 0,
                  texname = '\\lambda _{\\gamma ^2}{}^{\\text{CP}}',
                  lhablock = 'HPO4f',
                  lhacode = [ 7 ])

lZACP = Parameter(name = 'lZACP',
                  nature = 'external',
                  type = 'real',
                  value = 0,
                  texname = '\\lambda _{\\gamma  Z}{}^{\\text{CP}}',
                  lhablock = 'HPO4f',
                  lhacode = [ 8 ])

eZZCP = Parameter(name = 'eZZCP',
                  nature = 'external',
                  type = 'real',
                  value = 0,
                  texname = '\\epsilon _{\\text{ZZ}}{}^{\\text{CP}}',
                  lhablock = 'HPO4f',
                  lhacode = [ 9 ])

eWWCP = Parameter(name = 'eWWCP',
                  nature = 'external',
                  type = 'real',
                  value = 0,
                  texname = '\\epsilon _{\\text{WW}}{}^{\\text{CP}}',
                  lhablock = 'HPO4f',
                  lhacode = [ 10 ])

eZeL = Parameter(name = 'eZeL',
                 nature = 'external',
                 type = 'real',
                 value = 0,
                 texname = '\\epsilon _{\\text{ZeL}}',
                 lhablock = 'HPO4f',
                 lhacode = [ 11 ])

eZmuL = Parameter(name = 'eZmuL',
                  nature = 'external',
                  type = 'real',
                  value = 0,
                  texname = '\\epsilon _{\\text{Z$\\mu $L}}',
                  lhablock = 'HPO4f',
                  lhacode = [ 12 ])

eZtauL = Parameter(name = 'eZtauL',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\epsilon _{\\text{Z$\\tau $L}}',
                   lhablock = 'HPO4f',
                   lhacode = [ 13 ])

eZeR = Parameter(name = 'eZeR',
                 nature = 'external',
                 type = 'real',
                 value = 0,
                 texname = '\\epsilon _{\\text{ZeR}}',
                 lhablock = 'HPO4f',
                 lhacode = [ 14 ])

eZmuR = Parameter(name = 'eZmuR',
                  nature = 'external',
                  type = 'real',
                  value = 0,
                  texname = '\\epsilon _{\\text{Z$\\mu $R}}',
                  lhablock = 'HPO4f',
                  lhacode = [ 15 ])

eZtauR = Parameter(name = 'eZtauR',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\epsilon _{\\text{Z$\\tau $R}}',
                   lhablock = 'HPO4f',
                   lhacode = [ 16 ])

eZv = Parameter(name = 'eZv',
                nature = 'external',
                type = 'real',
                value = 0,
                texname = '\\epsilon _{\\text{Z$\\nu $}}',
                lhablock = 'HPO4f',
                lhacode = [ 17 ])

eWe = Parameter(name = 'eWe',
                nature = 'external',
                type = 'real',
                value = 0,
                texname = '\\epsilon _{\\text{We}}',
                lhablock = 'HPO4f',
                lhacode = [ 18 ])

eWmu = Parameter(name = 'eWmu',
                 nature = 'external',
                 type = 'real',
                 value = 0,
                 texname = '\\epsilon _{\\text{W$\\mu $}}',
                 lhablock = 'HPO4f',
                 lhacode = [ 19 ])

eWtau = Parameter(name = 'eWtau',
                  nature = 'external',
                  type = 'real',
                  value = 0,
                  texname = '\\epsilon _{\\text{W$\\tau $}}',
                  lhablock = 'HPO4f',
                  lhacode = [ 20 ])

phiWe = Parameter(name = 'phiWe',
                  nature = 'external',
                  type = 'real',
                  value = 0,
                  texname = '\\phi _{\\text{We}}',
                  lhablock = 'HPO4f',
                  lhacode = [ 21 ])

phiWmu = Parameter(name = 'phiWmu',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\phi _{\\mu  W}',
                   lhablock = 'HPO4f',
                   lhacode = [ 22 ])

phiWtau = Parameter(name = 'phiWtau',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\phi _{\\tau  W}',
                    lhablock = 'HPO4f',
                    lhacode = [ 23 ])

eAASM = Parameter(name = 'eAASM',
                  nature = 'external',
                  type = 'real',
                  value = 0.0038,
                  texname = '\\epsilon _{\\gamma ^2}',
                  lhablock = 'HPOSM',
                  lhacode = [ 1 ])

eZASM = Parameter(name = 'eZASM',
                  nature = 'external',
                  type = 'real',
                  value = 0.0069,
                  texname = '\\epsilon _{\\gamma  Z}',
                  lhablock = 'HPOSM',
                  lhacode = [ 2 ])

ybeff = Parameter(name = 'ybeff',
                  nature = 'external',
                  type = 'real',
                  value = 0.0177,
                  texname = 'y_{\\text{eff}}{}^b',
                  lhablock = 'HPOSM',
                  lhacode = [ 3 ])

yceff = Parameter(name = 'yceff',
                  nature = 'external',
                  type = 'real',
                  value = 0.00398,
                  texname = 'y_{\\text{eff}}{}^c',
                  lhablock = 'HPOSM',
                  lhacode = [ 4 ])

ytaueff = Parameter(name = 'ytaueff',
                    nature = 'external',
                    type = 'real',
                    value = 0.0102,
                    texname = 'y_{\\text{eff}}{}^{\\tau }',
                    lhablock = 'HPOSM',
                    lhacode = [ 5 ])

ymueff = Parameter(name = 'ymueff',
                   nature = 'external',
                   type = 'real',
                   value = 0.000599,
                   texname = 'y_{\\text{eff}}{}^{\\mu }',
                   lhablock = 'HPOSM',
                   lhacode = [ 6 ])

aEWM1 = Parameter(name = 'aEWM1',
                  nature = 'external',
                  type = 'real',
                  value = 128.941,
                  texname = '\\text{aEWM1}',
                  lhablock = 'SMPARAM',
                  lhacode = [ 1 ])

vF = Parameter(name = 'vF',
               nature = 'external',
               type = 'real',
               value = 246.22,
               texname = 'v_F',
               lhablock = 'SMPARAM',
               lhacode = [ 2 ])

aS = Parameter(name = 'aS',
               nature = 'external',
               type = 'real',
               value = 0.119,
               texname = '\\alpha _s',
               lhablock = 'SMPARAM',
               lhacode = [ 3 ])

gZeL = Parameter(name = 'gZeL',
                 nature = 'external',
                 type = 'real',
                 value = -0.2696,
                 texname = 'g_{\\text{ZeL}}',
                 lhablock = 'WZPole',
                 lhacode = [ 1 ])

gZmuL = Parameter(name = 'gZmuL',
                  nature = 'external',
                  type = 'real',
                  value = -0.269,
                  texname = 'g_{\\text{Z$\\mu $L}}',
                  lhablock = 'WZPole',
                  lhacode = [ 2 ])

gZtauL = Parameter(name = 'gZtauL',
                   nature = 'external',
                   type = 'real',
                   value = -0.2693,
                   texname = 'g_{\\text{Z$\\tau $L}}',
                   lhablock = 'WZPole',
                   lhacode = [ 3 ])

gZeR = Parameter(name = 'gZeR',
                 nature = 'external',
                 type = 'real',
                 value = 0.2315,
                 texname = 'g_{\\text{ZeR}}',
                 lhablock = 'WZPole',
                 lhacode = [ 4 ])

gZmuR = Parameter(name = 'gZmuR',
                  nature = 'external',
                  type = 'real',
                  value = 0.232,
                  texname = 'g_{\\text{Z$\\mu $R}}',
                  lhablock = 'WZPole',
                  lhacode = [ 5 ])

gZtauR = Parameter(name = 'gZtauR',
                   nature = 'external',
                   type = 'real',
                   value = 0.2327,
                   texname = 'g_{\\text{Z$\\tau $R}}',
                   lhablock = 'WZPole',
                   lhacode = [ 6 ])

gZv = Parameter(name = 'gZv',
                nature = 'external',
                type = 'real',
                value = 0.5,
                texname = 'g_{\\text{Z$\\nu $}}',
                lhablock = 'WZPole',
                lhacode = [ 7 ])

gWe = Parameter(name = 'gWe',
                nature = 'external',
                type = 'real',
                value = 0.994,
                texname = 'g_{\\text{We}}',
                lhablock = 'WZPole',
                lhacode = [ 8 ])

gWmu = Parameter(name = 'gWmu',
                 nature = 'external',
                 type = 'real',
                 value = 0.991,
                 texname = 'g_{\\text{W$\\mu $}}',
                 lhablock = 'WZPole',
                 lhacode = [ 9 ])

gWtau = Parameter(name = 'gWtau',
                  nature = 'external',
                  type = 'real',
                  value = 1.025,
                  texname = 'g_{\\text{W$\\tau $}}',
                  lhablock = 'WZPole',
                  lhacode = [ 10 ])

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

MTA = Parameter(name = 'MTA',
                nature = 'external',
                type = 'real',
                value = 1.777,
                texname = '\\text{MTA}',
                lhablock = 'MASS',
                lhacode = [ 15 ])

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

eggSM = Parameter(name = 'eggSM',
                  nature = 'internal',
                  type = 'real',
                  value = '-0.0065',
                  texname = '\\text{eggSM}')

noise = Parameter(name = 'noise',
                  nature = 'internal',
                  type = 'real',
                  value = '9.96300008259327e-11',
                  texname = '\\text{noise}')

ee = Parameter(name = 'ee',
               nature = 'internal',
               type = 'real',
               value = '2*cmath.sqrt(aEW)*cmath.sqrt(cmath.pi)',
               texname = 'e')


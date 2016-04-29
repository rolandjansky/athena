# This file was automatically created by FeynRules $Revision: 709 $
# Mathematica version: 8.0 for Linux x86 (64-bit) (February 23, 2011)
# Date: Tue 25 Oct 2011 09:50:07



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

theta13 = Parameter(name = 'theta13',
                    nature = 'external',
                    type = 'real',
                    value = 0.1,
                    texname = '\\theta _{13}',
                    lhablock = 'CKMBLOCK',
                    lhacode = [ 2 ])

theta12 = Parameter(name = 'theta12',
                    nature = 'external',
                    type = 'real',
                    value = 0.6,
                    texname = '\\theta _{12}',
                    lhablock = 'CKMBLOCK',
                    lhacode = [ 3 ])

theta23 = Parameter(name = 'theta23',
                    nature = 'external',
                    type = 'real',
                    value = 0.75,
                    texname = '\\theta _{23}',
                    lhablock = 'CKMBLOCK',
                    lhacode = [ 4 ])

Ve = Parameter(name = 'Ve',
               nature = 'external',
               type = 'real',
               value = 0.055,
               texname = '\\text{Ve}',
               lhablock = 'MIXING',
               lhacode = [ 1 ])

Vm = Parameter(name = 'Vm',
               nature = 'external',
               type = 'real',
               value = 0.063,
               texname = '\\text{Vm}',
               lhablock = 'MIXING',
               lhacode = [ 2 ])

Vtt = Parameter(name = 'Vtt',
                nature = 'external',
                type = 'real',
                value = 0,
                texname = '\\text{Vtt}',
                lhablock = 'MIXING',
                lhacode = [ 3 ])

mv1 = Parameter(name = 'mv1',
                nature = 'external',
                type = 'real',
                value = 0,
                texname = '\\text{mv1}',
                lhablock = 'NEWMASSES',
                lhacode = [ 1 ])

mv2 = Parameter(name = 'mv2',
                nature = 'external',
                type = 'real',
                value = 0,
                texname = '\\text{mv2}',
                lhablock = 'NEWMASSES',
                lhacode = [ 2 ])

mv3 = Parameter(name = 'mv3',
                nature = 'external',
                type = 'real',
                value = 0,
                texname = '\\text{mv3}',
                lhablock = 'NEWMASSES',
                lhacode = [ 3 ])

mtr = Parameter(name = 'mtr',
                nature = 'external',
                type = 'real',
                value = 300.0,
                texname = '\\text{mtr}',
                lhablock = 'NEWMASSES',
                lhacode = [ 4 ])

mtrm = Parameter(name = 'mtrm',
                 nature = 'external',
                 type = 'real',
                 value = 300.0,
                 texname = '\\text{mtrm}',
                 lhablock = 'NEWMASSES',
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
               value = 0.000011663900000000002,
               texname = 'G_f',
               lhablock = 'SMINPUTS',
               lhacode = [ 2 ])

aS = Parameter(name = 'aS',
               nature = 'external',
               type = 'real',
               value = 0.118,
               texname = '\\text{aS}',
               lhablock = 'SMINPUTS',
               lhacode = [ 3 ])

ymc = Parameter(name = 'ymc',
                nature = 'external',
                type = 'real',
                value = 1.42,
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
                value = 174.3,
                texname = '\\text{ymt}',
                lhablock = 'YUKAWA',
                lhacode = [ 6 ])

yme = Parameter(name = 'yme',
                nature = 'external',
                type = 'real',
                value = 0,
                texname = '\\text{yme}',
                lhablock = 'YUKAWA',
                lhacode = [ 13 ])

ymm = Parameter(name = 'ymm',
                nature = 'external',
                type = 'real',
                value = 0,
                texname = '\\text{ymm}',
                lhablock = 'YUKAWA',
                lhacode = [ 14 ])

ymtau = Parameter(name = 'ymtau',
                  nature = 'external',
                  type = 'real',
                  value = 1.777,
                  texname = '\\text{ymtau}',
                  lhablock = 'YUKAWA',
                  lhacode = [ 15 ])

Mtr0 = Parameter(name = 'Mtr0',
                 nature = 'external',
                 type = 'real',
                 value = 300.0,
                 texname = '\\text{Mtr0}',
                 lhablock = 'MASS',
                 lhacode = [ 8000018 ])

Me = Parameter(name = 'Me',
               nature = 'external',
               type = 'real',
               value = 0.0005110000000000001,
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

Mtrch = Parameter(name = 'Mtrch',
                  nature = 'external',
                  type = 'real',
                  value = 300.0,
                  texname = '\\text{Mtrch}',
                  lhablock = 'MASS',
                  lhacode = [ 8000020 ])

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
               value = 1.42,
               texname = '\\text{MC}',
               lhablock = 'MASS',
               lhacode = [ 4 ])

MT = Parameter(name = 'MT',
               nature = 'external',
               type = 'real',
               value = 174.3,
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
               value = 0.104,
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

MZ = Parameter(name = 'MZ',
               nature = 'external',
               type = 'real',
               value = 91.188,
               texname = '\\text{MZ}',
               lhablock = 'MASS',
               lhacode = [ 23 ])

MH = Parameter(name = 'MH',
               nature = 'external',
               type = 'real',
               value = 125,
               texname = '\\text{MH}',
               lhablock = 'MASS',
               lhacode = [ 25 ])

Wtr0 = Parameter(name = 'Wtr0',
                 nature = 'external',
                 type = 'real',
                 value = 0.1,
                 texname = '\\text{Wtr0}',
                 lhablock = 'DECAY',
                 lhacode = [ 8000018 ])

Wtau = Parameter(name = 'Wtau',
                 nature = 'external',
                 type = 'real',
                 value = 0.1,
                 texname = '\\text{Wtau}',
                 lhablock = 'DECAY',
                 lhacode = [ 15 ])

Wtrch = Parameter(name = 'Wtrch',
                  nature = 'external',
                  type = 'real',
                  value = 0.1,
                  texname = '\\text{Wtrch}',
                  lhablock = 'DECAY',
                  lhacode = [ 8000020 ])

WC = Parameter(name = 'WC',
               nature = 'external',
               type = 'real',
               value = 0.1,
               texname = '\\text{WC}',
               lhablock = 'DECAY',
               lhacode = [ 4 ])

WT = Parameter(name = 'WT',
               nature = 'external',
               type = 'real',
               value = 1.50833649,
               texname = '\\text{WT}',
               lhablock = 'DECAY',
               lhacode = [ 6 ])

WB = Parameter(name = 'WB',
               nature = 'external',
               type = 'real',
               value = 0.1,
               texname = '\\text{WB}',
               lhablock = 'DECAY',
               lhacode = [ 5 ])

WZ = Parameter(name = 'WZ',
               nature = 'external',
               type = 'real',
               value = 2.44140351,
               texname = '\\text{WZ}',
               lhablock = 'DECAY',
               lhacode = [ 23 ])

WW = Parameter(name = 'WW',
               nature = 'external',
               type = 'real',
               value = 2.04759951,
               texname = '\\text{WW}',
               lhablock = 'DECAY',
               lhacode = [ 24 ])

WH = Parameter(name = 'WH',
               nature = 'external',
               type = 'real',
               value = 0.00575308848,
               texname = '\\text{WH}',
               lhablock = 'DECAY',
               lhacode = [ 25 ])

aEW = Parameter(name = 'aEW',
                nature = 'internal',
                type = 'real',
                value = '1/aEWM1',
                texname = '\\text{aEW}')

G = Parameter(name = 'G',
              nature = 'internal',
              type = 'real',
              value = '2*cmath.sqrt(aS)*cmath.sqrt(cmath.pi)',
              texname = 'G')

useless = Parameter(name = 'useless',
                    nature = 'internal',
                    type = 'real',
                    value = '0',
                    texname = '\\text{useless}')

yu = Parameter(name = 'yu',
               nature = 'internal',
               type = 'real',
               value = '0',
               texname = '\\text{yu}')

yd = Parameter(name = 'yd',
               nature = 'internal',
               type = 'real',
               value = '0',
               texname = '\\text{yd}')

ys = Parameter(name = 'ys',
               nature = 'internal',
               type = 'real',
               value = '0',
               texname = '\\text{ys}')

CKM11 = Parameter(name = 'CKM11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'cmath.cos(cabi)',
                  texname = '\\text{CKM11}')

CKM12 = Parameter(name = 'CKM12',
                  nature = 'internal',
                  type = 'complex',
                  value = 'cmath.sin(cabi)',
                  texname = '\\text{CKM12}')

CKM13 = Parameter(name = 'CKM13',
                  nature = 'internal',
                  type = 'complex',
                  value = '0',
                  texname = '\\text{CKM13}')

CKM21 = Parameter(name = 'CKM21',
                  nature = 'internal',
                  type = 'complex',
                  value = '-cmath.sin(cabi)',
                  texname = '\\text{CKM21}')

CKM22 = Parameter(name = 'CKM22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'cmath.cos(cabi)',
                  texname = '\\text{CKM22}')

CKM23 = Parameter(name = 'CKM23',
                  nature = 'internal',
                  type = 'complex',
                  value = '0',
                  texname = '\\text{CKM23}')

CKM31 = Parameter(name = 'CKM31',
                  nature = 'internal',
                  type = 'complex',
                  value = '0',
                  texname = '\\text{CKM31}')

CKM32 = Parameter(name = 'CKM32',
                  nature = 'internal',
                  type = 'complex',
                  value = '0',
                  texname = '\\text{CKM32}')

CKM33 = Parameter(name = 'CKM33',
                  nature = 'internal',
                  type = 'complex',
                  value = '1',
                  texname = '\\text{CKM33}')

PMNS11 = Parameter(name = 'PMNS11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'cmath.cos(theta12)*cmath.cos(theta13)',
                   texname = '\\text{PMNS11}')

PMNS12 = Parameter(name = 'PMNS12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'cmath.cos(theta13)*cmath.sin(theta12)',
                   texname = '\\text{PMNS12}')

PMNS13 = Parameter(name = 'PMNS13',
                   nature = 'internal',
                   type = 'complex',
                   value = 'cmath.sin(theta13)',
                   texname = '\\text{PMNS13}')

PMNS21 = Parameter(name = 'PMNS21',
                   nature = 'internal',
                   type = 'complex',
                   value = '-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23)',
                   texname = '\\text{PMNS21}')

PMNS22 = Parameter(name = 'PMNS22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23)',
                   texname = '\\text{PMNS22}')

PMNS23 = Parameter(name = 'PMNS23',
                   nature = 'internal',
                   type = 'complex',
                   value = 'cmath.cos(theta13)*cmath.sin(theta23)',
                   texname = '\\text{PMNS23}')

PMNS31 = Parameter(name = 'PMNS31',
                   nature = 'internal',
                   type = 'complex',
                   value = '-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)',
                   texname = '\\text{PMNS31}')

PMNS32 = Parameter(name = 'PMNS32',
                   nature = 'internal',
                   type = 'complex',
                   value = '-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)',
                   texname = '\\text{PMNS32}')

PMNS33 = Parameter(name = 'PMNS33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'cmath.cos(theta13)*cmath.cos(theta23)',
                   texname = '\\text{PMNS33}')

gCCL11 = Parameter(name = 'gCCL11',
                   nature = 'internal',
                   type = 'real',
                   value = 'cmath.cos(theta12)*cmath.cos(theta13) + (Ve**2*cmath.cos(theta12)*cmath.cos(theta13) + Ve*Vtt*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Ve*Vm*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))/2.',
                   texname = '\\text{gCCL11}')

gCCL12 = Parameter(name = 'gCCL12',
                   nature = 'internal',
                   type = 'real',
                   value = 'cmath.cos(theta13)*cmath.sin(theta12) + (Ve**2*cmath.cos(theta13)*cmath.sin(theta12) + Ve*Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Ve*Vm*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))/2.',
                   texname = '\\text{gCCL12}')

gCCL13 = Parameter(name = 'gCCL13',
                   nature = 'internal',
                   type = 'real',
                   value = 'cmath.sin(theta13) + (Ve*Vtt*cmath.cos(theta13)*cmath.cos(theta23) + Ve**2*cmath.sin(theta13) + Ve*Vm*cmath.cos(theta13)*cmath.sin(theta23))/2.',
                   texname = '\\text{gCCL13}')

gCCL14 = Parameter(name = 'gCCL14',
                   nature = 'internal',
                   type = 'real',
                   value = '-Ve',
                   texname = '\\text{gCCL14}')

gCCL21 = Parameter(name = 'gCCL21',
                   nature = 'internal',
                   type = 'real',
                   value = '-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23) + (Ve*Vm*cmath.cos(theta12)*cmath.cos(theta13) + Vm*Vtt*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Vm**2*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))/2.',
                   texname = '\\text{gCCL21}')

gCCL22 = Parameter(name = 'gCCL22',
                   nature = 'internal',
                   type = 'real',
                   value = 'cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23) + (Ve*Vm*cmath.cos(theta13)*cmath.sin(theta12) + Vm*Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Vm**2*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))/2.',
                   texname = '\\text{gCCL22}')

gCCL23 = Parameter(name = 'gCCL23',
                   nature = 'internal',
                   type = 'real',
                   value = 'cmath.cos(theta13)*cmath.sin(theta23) + (Vm*Vtt*cmath.cos(theta13)*cmath.cos(theta23) + Ve*Vm*cmath.sin(theta13) + Vm**2*cmath.cos(theta13)*cmath.sin(theta23))/2.',
                   texname = '\\text{gCCL23}')

gCCL24 = Parameter(name = 'gCCL24',
                   nature = 'internal',
                   type = 'real',
                   value = '-Vm',
                   texname = '\\text{gCCL24}')

gCCL31 = Parameter(name = 'gCCL31',
                   nature = 'internal',
                   type = 'real',
                   value = '-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23) + (Ve*Vtt*cmath.cos(theta12)*cmath.cos(theta13) + Vtt**2*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Vm*Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))/2.',
                   texname = '\\text{gCCL31}')

gCCL32 = Parameter(name = 'gCCL32',
                   nature = 'internal',
                   type = 'real',
                   value = '-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23) + (Ve*Vtt*cmath.cos(theta13)*cmath.sin(theta12) + Vtt**2*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Vm*Vtt*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))/2.',
                   texname = '\\text{gCCL32}')

gCCL33 = Parameter(name = 'gCCL33',
                   nature = 'internal',
                   type = 'real',
                   value = 'cmath.cos(theta13)*cmath.cos(theta23) + (Vtt**2*cmath.cos(theta13)*cmath.cos(theta23) + Ve*Vtt*cmath.sin(theta13) + Vm*Vtt*cmath.cos(theta13)*cmath.sin(theta23))/2.',
                   texname = '\\text{gCCL33}')

gCCL34 = Parameter(name = 'gCCL34',
                   nature = 'internal',
                   type = 'real',
                   value = '-Vtt',
                   texname = '\\text{gCCL34}')

gCCL41 = Parameter(name = 'gCCL41',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{gCCL41}')

gCCL42 = Parameter(name = 'gCCL42',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{gCCL42}')

gCCL43 = Parameter(name = 'gCCL43',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{gCCL43}')

gCCL44 = Parameter(name = 'gCCL44',
                   nature = 'internal',
                   type = 'real',
                   value = '(1 + (-Ve**2 - Vm**2 - Vtt**2)/2.)*cmath.sqrt(2)',
                   texname = '\\text{gCCL44}')

gCCR11 = Parameter(name = 'gCCR11',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{gCCR11}')

gCCR12 = Parameter(name = 'gCCR12',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{gCCR12}')

gCCR13 = Parameter(name = 'gCCR13',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{gCCR13}')

gCCR14 = Parameter(name = 'gCCR14',
                   nature = 'internal',
                   type = 'real',
                   value = '-((Ve*yme*cmath.sqrt(2))/mtr)',
                   texname = '\\text{gCCR14}')

gCCR21 = Parameter(name = 'gCCR21',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{gCCR21}')

gCCR22 = Parameter(name = 'gCCR22',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{gCCR22}')

gCCR23 = Parameter(name = 'gCCR23',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{gCCR23}')

gCCR24 = Parameter(name = 'gCCR24',
                   nature = 'internal',
                   type = 'real',
                   value = '-((Vm*ymm*cmath.sqrt(2))/mtr)',
                   texname = '\\text{gCCR24}')

gCCR31 = Parameter(name = 'gCCR31',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{gCCR31}')

gCCR32 = Parameter(name = 'gCCR32',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{gCCR32}')

gCCR33 = Parameter(name = 'gCCR33',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{gCCR33}')

gCCR34 = Parameter(name = 'gCCR34',
                   nature = 'internal',
                   type = 'real',
                   value = '-((Vtt*ymtau*cmath.sqrt(2))/mtr)',
                   texname = '\\text{gCCR34}')

gCCR41 = Parameter(name = 'gCCR41',
                   nature = 'internal',
                   type = 'real',
                   value = '-(Ve*cmath.cos(theta12)*cmath.cos(theta13)) - Vtt*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) - Vm*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23))',
                   texname = '\\text{gCCR41}')

gCCR42 = Parameter(name = 'gCCR42',
                   nature = 'internal',
                   type = 'real',
                   value = '-(Ve*cmath.cos(theta13)*cmath.sin(theta12)) - Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) - Vm*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23))',
                   texname = '\\text{gCCR42}')

gCCR43 = Parameter(name = 'gCCR43',
                   nature = 'internal',
                   type = 'real',
                   value = '-(Vtt*cmath.cos(theta13)*cmath.cos(theta23)) - Ve*cmath.sin(theta13) - Vm*cmath.cos(theta13)*cmath.sin(theta23)',
                   texname = '\\text{gCCR43}')

gCCR44 = Parameter(name = 'gCCR44',
                   nature = 'internal',
                   type = 'real',
                   value = '1 + (-Ve**2 - Vm**2 - Vtt**2)/2.',
                   texname = '\\text{gCCR44}')

gNCL12 = Parameter(name = 'gNCL12',
                   nature = 'internal',
                   type = 'real',
                   value = 'Ve*Vm',
                   texname = '\\text{gNCL12}')

gNCL13 = Parameter(name = 'gNCL13',
                   nature = 'internal',
                   type = 'real',
                   value = 'Ve*Vtt',
                   texname = '\\text{gNCL13}')

gNCL14 = Parameter(name = 'gNCL14',
                   nature = 'internal',
                   type = 'real',
                   value = 'Ve/cmath.sqrt(2)',
                   texname = '\\text{gNCL14}')

gNCL21 = Parameter(name = 'gNCL21',
                   nature = 'internal',
                   type = 'real',
                   value = 'Ve*Vm',
                   texname = '\\text{gNCL21}')

gNCL23 = Parameter(name = 'gNCL23',
                   nature = 'internal',
                   type = 'real',
                   value = 'Vm*Vtt',
                   texname = '\\text{gNCL23}')

gNCL24 = Parameter(name = 'gNCL24',
                   nature = 'internal',
                   type = 'real',
                   value = 'Vm/cmath.sqrt(2)',
                   texname = '\\text{gNCL24}')

gNCL31 = Parameter(name = 'gNCL31',
                   nature = 'internal',
                   type = 'real',
                   value = 'Ve*Vtt',
                   texname = '\\text{gNCL31}')

gNCL32 = Parameter(name = 'gNCL32',
                   nature = 'internal',
                   type = 'real',
                   value = 'Vm*Vtt',
                   texname = '\\text{gNCL32}')

gNCL34 = Parameter(name = 'gNCL34',
                   nature = 'internal',
                   type = 'real',
                   value = 'Vtt/cmath.sqrt(2)',
                   texname = '\\text{gNCL34}')

gNCL41 = Parameter(name = 'gNCL41',
                   nature = 'internal',
                   type = 'real',
                   value = 'Ve/cmath.sqrt(2)',
                   texname = '\\text{gNCL41}')

gNCL42 = Parameter(name = 'gNCL42',
                   nature = 'internal',
                   type = 'real',
                   value = 'Vm/cmath.sqrt(2)',
                   texname = '\\text{gNCL42}')

gNCL43 = Parameter(name = 'gNCL43',
                   nature = 'internal',
                   type = 'real',
                   value = 'Vtt/cmath.sqrt(2)',
                   texname = '\\text{gNCL43}')

gNCR12 = Parameter(name = 'gNCR12',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{gNCR12}')

gNCR13 = Parameter(name = 'gNCR13',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{gNCR13}')

gNCR14 = Parameter(name = 'gNCR14',
                   nature = 'internal',
                   type = 'real',
                   value = '(Ve*yme*cmath.sqrt(2))/mtr',
                   texname = '\\text{gNCR14}')

gNCR21 = Parameter(name = 'gNCR21',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{gNCR21}')

gNCR23 = Parameter(name = 'gNCR23',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{gNCR23}')

gNCR24 = Parameter(name = 'gNCR24',
                   nature = 'internal',
                   type = 'real',
                   value = '(Vm*ymm*cmath.sqrt(2))/mtr',
                   texname = '\\text{gNCR24}')

gNCR31 = Parameter(name = 'gNCR31',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{gNCR31}')

gNCR32 = Parameter(name = 'gNCR32',
                   nature = 'internal',
                   type = 'real',
                   value = '0',
                   texname = '\\text{gNCR32}')

gNCR34 = Parameter(name = 'gNCR34',
                   nature = 'internal',
                   type = 'real',
                   value = '(Vtt*ymtau*cmath.sqrt(2))/mtr',
                   texname = '\\text{gNCR34}')

gNCR41 = Parameter(name = 'gNCR41',
                   nature = 'internal',
                   type = 'real',
                   value = '(Ve*yme*cmath.sqrt(2))/mtr',
                   texname = '\\text{gNCR41}')

gNCR42 = Parameter(name = 'gNCR42',
                   nature = 'internal',
                   type = 'real',
                   value = '(Vm*ymm*cmath.sqrt(2))/mtr',
                   texname = '\\text{gNCR42}')

gNCR43 = Parameter(name = 'gNCR43',
                   nature = 'internal',
                   type = 'real',
                   value = '(Vtt*ymtau*cmath.sqrt(2))/mtr',
                   texname = '\\text{gNCR43}')

gNCnu11 = Parameter(name = 'gNCnu11',
                    nature = 'internal',
                    type = 'real',
                    value = '1 - cmath.cos(theta12)*cmath.cos(theta13)*(Ve**2*cmath.cos(theta12)*cmath.cos(theta13) + Ve*Vtt*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Ve*Vm*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23))) - (-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23))*(Ve*Vm*cmath.cos(theta12)*cmath.cos(theta13) + Vm*Vtt*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Vm**2*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23))) - (-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23))*(Ve*Vtt*cmath.cos(theta12)*cmath.cos(theta13) + Vtt**2*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Vm*Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))',
                    texname = '\\text{gNCnu11}')

gNCnu12 = Parameter(name = 'gNCnu12',
                    nature = 'internal',
                    type = 'real',
                    value = '-(cmath.cos(theta13)*cmath.sin(theta12)*(Ve**2*cmath.cos(theta12)*cmath.cos(theta13) + Ve*Vtt*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Ve*Vm*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))) - (cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23))*(Ve*Vm*cmath.cos(theta12)*cmath.cos(theta13) + Vm*Vtt*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Vm**2*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23))) - (-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23))*(Ve*Vtt*cmath.cos(theta12)*cmath.cos(theta13) + Vtt**2*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Vm*Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))',
                    texname = '\\text{gNCnu12}')

gNCnu13 = Parameter(name = 'gNCnu13',
                    nature = 'internal',
                    type = 'real',
                    value = '-(cmath.sin(theta13)*(Ve**2*cmath.cos(theta12)*cmath.cos(theta13) + Ve*Vtt*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Ve*Vm*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))) - cmath.cos(theta13)*cmath.sin(theta23)*(Ve*Vm*cmath.cos(theta12)*cmath.cos(theta13) + Vm*Vtt*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Vm**2*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23))) - cmath.cos(theta13)*cmath.cos(theta23)*(Ve*Vtt*cmath.cos(theta12)*cmath.cos(theta13) + Vtt**2*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Vm*Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))',
                    texname = '\\text{gNCnu13}')

gNCnu14 = Parameter(name = 'gNCnu14',
                    nature = 'internal',
                    type = 'real',
                    value = 'Ve*cmath.cos(theta12)*cmath.cos(theta13) + Vtt*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Vm*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23))',
                    texname = '\\text{gNCnu14}')

gNCnu21 = Parameter(name = 'gNCnu21',
                    nature = 'internal',
                    type = 'real',
                    value = '-(cmath.cos(theta12)*cmath.cos(theta13)*(Ve**2*cmath.cos(theta13)*cmath.sin(theta12) + Ve*Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Ve*Vm*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))) - (-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23))*(Ve*Vm*cmath.cos(theta13)*cmath.sin(theta12) + Vm*Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Vm**2*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23))) - (-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23))*(Ve*Vtt*cmath.cos(theta13)*cmath.sin(theta12) + Vtt**2*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Vm*Vtt*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))',
                    texname = '\\text{gNCnu21}')

gNCnu22 = Parameter(name = 'gNCnu22',
                    nature = 'internal',
                    type = 'real',
                    value = '1 - cmath.cos(theta13)*cmath.sin(theta12)*(Ve**2*cmath.cos(theta13)*cmath.sin(theta12) + Ve*Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Ve*Vm*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23))) - (cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23))*(Ve*Vm*cmath.cos(theta13)*cmath.sin(theta12) + Vm*Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Vm**2*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23))) - (-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23))*(Ve*Vtt*cmath.cos(theta13)*cmath.sin(theta12) + Vtt**2*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Vm*Vtt*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))',
                    texname = '\\text{gNCnu22}')

gNCnu23 = Parameter(name = 'gNCnu23',
                    nature = 'internal',
                    type = 'real',
                    value = '-(cmath.sin(theta13)*(Ve**2*cmath.cos(theta13)*cmath.sin(theta12) + Ve*Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Ve*Vm*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))) - cmath.cos(theta13)*cmath.sin(theta23)*(Ve*Vm*cmath.cos(theta13)*cmath.sin(theta12) + Vm*Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Vm**2*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23))) - cmath.cos(theta13)*cmath.cos(theta23)*(Ve*Vtt*cmath.cos(theta13)*cmath.sin(theta12) + Vtt**2*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Vm*Vtt*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))',
                    texname = '\\text{gNCnu23}')

gNCnu24 = Parameter(name = 'gNCnu24',
                    nature = 'internal',
                    type = 'real',
                    value = 'Ve*cmath.cos(theta13)*cmath.sin(theta12) + Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Vm*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23))',
                    texname = '\\text{gNCnu24}')

gNCnu31 = Parameter(name = 'gNCnu31',
                    nature = 'internal',
                    type = 'real',
                    value = '-(cmath.cos(theta12)*cmath.cos(theta13)*(Ve*Vtt*cmath.cos(theta13)*cmath.cos(theta23) + Ve**2*cmath.sin(theta13) + Ve*Vm*cmath.cos(theta13)*cmath.sin(theta23))) - (Vtt**2*cmath.cos(theta13)*cmath.cos(theta23) + Ve*Vtt*cmath.sin(theta13) + Vm*Vtt*cmath.cos(theta13)*cmath.sin(theta23))*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) - (Vm*Vtt*cmath.cos(theta13)*cmath.cos(theta23) + Ve*Vm*cmath.sin(theta13) + Vm**2*cmath.cos(theta13)*cmath.sin(theta23))*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23))',
                    texname = '\\text{gNCnu31}')

gNCnu32 = Parameter(name = 'gNCnu32',
                    nature = 'internal',
                    type = 'real',
                    value = '-(cmath.cos(theta13)*cmath.sin(theta12)*(Ve*Vtt*cmath.cos(theta13)*cmath.cos(theta23) + Ve**2*cmath.sin(theta13) + Ve*Vm*cmath.cos(theta13)*cmath.sin(theta23))) - (-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23))*(Vtt**2*cmath.cos(theta13)*cmath.cos(theta23) + Ve*Vtt*cmath.sin(theta13) + Vm*Vtt*cmath.cos(theta13)*cmath.sin(theta23)) - (Vm*Vtt*cmath.cos(theta13)*cmath.cos(theta23) + Ve*Vm*cmath.sin(theta13) + Vm**2*cmath.cos(theta13)*cmath.sin(theta23))*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23))',
                    texname = '\\text{gNCnu32}')

gNCnu33 = Parameter(name = 'gNCnu33',
                    nature = 'internal',
                    type = 'real',
                    value = '1 - cmath.sin(theta13)*(Ve*Vtt*cmath.cos(theta13)*cmath.cos(theta23) + Ve**2*cmath.sin(theta13) + Ve*Vm*cmath.cos(theta13)*cmath.sin(theta23)) - cmath.cos(theta13)*cmath.sin(theta23)*(Vm*Vtt*cmath.cos(theta13)*cmath.cos(theta23) + Ve*Vm*cmath.sin(theta13) + Vm**2*cmath.cos(theta13)*cmath.sin(theta23)) - cmath.cos(theta13)*cmath.cos(theta23)*(Vtt**2*cmath.cos(theta13)*cmath.cos(theta23) + Ve*Vtt*cmath.sin(theta13) + Vm*Vtt*cmath.cos(theta13)*cmath.sin(theta23))',
                    texname = '\\text{gNCnu33}')

gNCnu34 = Parameter(name = 'gNCnu34',
                    nature = 'internal',
                    type = 'real',
                    value = 'Vtt*cmath.cos(theta13)*cmath.cos(theta23) + Ve*cmath.sin(theta13) + Vm*cmath.cos(theta13)*cmath.sin(theta23)',
                    texname = '\\text{gNCnu34}')

gNCnu41 = Parameter(name = 'gNCnu41',
                    nature = 'internal',
                    type = 'real',
                    value = 'Ve*cmath.cos(theta12)*cmath.cos(theta13) + Vtt*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Vm*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23))',
                    texname = '\\text{gNCnu41}')

gNCnu42 = Parameter(name = 'gNCnu42',
                    nature = 'internal',
                    type = 'real',
                    value = 'Ve*cmath.cos(theta13)*cmath.sin(theta12) + Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Vm*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23))',
                    texname = '\\text{gNCnu42}')

gNCnu43 = Parameter(name = 'gNCnu43',
                    nature = 'internal',
                    type = 'real',
                    value = 'Vtt*cmath.cos(theta13)*cmath.cos(theta23) + Ve*cmath.sin(theta13) + Vm*cmath.cos(theta13)*cmath.sin(theta23)',
                    texname = '\\text{gNCnu43}')

gNCnu44 = Parameter(name = 'gNCnu44',
                    nature = 'internal',
                    type = 'real',
                    value = 'Ve**2 + Vm**2 + Vtt**2',
                    texname = '\\text{gNCnu44}')

gHnuR12 = Parameter(name = 'gHnuR12',
                    nature = 'internal',
                    type = 'real',
                    value = '0',
                    texname = '\\text{gHnuR12}')

gHnuR13 = Parameter(name = 'gHnuR13',
                    nature = 'internal',
                    type = 'real',
                    value = '0',
                    texname = '\\text{gHnuR13}')

gHnuR21 = Parameter(name = 'gHnuR21',
                    nature = 'internal',
                    type = 'real',
                    value = '0',
                    texname = '\\text{gHnuR21}')

gHnuR23 = Parameter(name = 'gHnuR23',
                    nature = 'internal',
                    type = 'real',
                    value = '0',
                    texname = '\\text{gHnuR23}')

gHnuR31 = Parameter(name = 'gHnuR31',
                    nature = 'internal',
                    type = 'real',
                    value = '0',
                    texname = '\\text{gHnuR31}')

gHnuR32 = Parameter(name = 'gHnuR32',
                    nature = 'internal',
                    type = 'real',
                    value = '0',
                    texname = '\\text{gHnuR32}')

getanuR12 = Parameter(name = 'getanuR12',
                      nature = 'internal',
                      type = 'real',
                      value = '0',
                      texname = '\\text{getanuR12}')

getanuR13 = Parameter(name = 'getanuR13',
                      nature = 'internal',
                      type = 'real',
                      value = '0',
                      texname = '\\text{getanuR13}')

getanuR21 = Parameter(name = 'getanuR21',
                      nature = 'internal',
                      type = 'real',
                      value = '0',
                      texname = '\\text{getanuR21}')

getanuR23 = Parameter(name = 'getanuR23',
                      nature = 'internal',
                      type = 'real',
                      value = '0',
                      texname = '\\text{getanuR23}')

getanuR31 = Parameter(name = 'getanuR31',
                      nature = 'internal',
                      type = 'real',
                      value = '0',
                      texname = '\\text{getanuR31}')

getanuR32 = Parameter(name = 'getanuR32',
                      nature = 'internal',
                      type = 'real',
                      value = '0',
                      texname = '\\text{getanuR32}')

gPhiL44 = Parameter(name = 'gPhiL44',
                    nature = 'internal',
                    type = 'real',
                    value = '0',
                    texname = '\\text{gPhiL44}')

gPhiR44 = Parameter(name = 'gPhiR44',
                    nature = 'internal',
                    type = 'real',
                    value = '0',
                    texname = '\\text{gPhiR44}')

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

gNCL11 = Parameter(name = 'gNCL11',
                   nature = 'internal',
                   type = 'real',
                   value = '0.5 - cw**2 - Ve**2',
                   texname = '\\text{gNCL11}')

gNCL22 = Parameter(name = 'gNCL22',
                   nature = 'internal',
                   type = 'real',
                   value = '0.5 - cw**2 - Vm**2',
                   texname = '\\text{gNCL22}')

gNCL33 = Parameter(name = 'gNCL33',
                   nature = 'internal',
                   type = 'real',
                   value = '0.5 - cw**2 - Vtt**2',
                   texname = '\\text{gNCL33}')

gNCL44 = Parameter(name = 'gNCL44',
                   nature = 'internal',
                   type = 'real',
                   value = '-cw**2 + Ve**2 + Vm**2 + Vtt**2',
                   texname = '\\text{gNCL44}')

gNCR11 = Parameter(name = 'gNCR11',
                   nature = 'internal',
                   type = 'real',
                   value = '1 - cw**2',
                   texname = '\\text{gNCR11}')

gNCR22 = Parameter(name = 'gNCR22',
                   nature = 'internal',
                   type = 'real',
                   value = '1 - cw**2',
                   texname = '\\text{gNCR22}')

gNCR33 = Parameter(name = 'gNCR33',
                   nature = 'internal',
                   type = 'real',
                   value = '1 - cw**2',
                   texname = '\\text{gNCR33}')

gNCR44 = Parameter(name = 'gNCR44',
                   nature = 'internal',
                   type = 'real',
                   value = '-cw**2',
                   texname = '\\text{gNCR44}')

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

v = Parameter(name = 'v',
              nature = 'internal',
              type = 'real',
              value = '(2*MW*sw)/ee',
              texname = 'v')

gPhiL11 = Parameter(name = 'gPhiL11',
                    nature = 'internal',
                    type = 'real',
                    value = '(yme*cmath.sqrt(2)*((1 - Ve**2/2.)*cmath.cos(theta12)*cmath.cos(theta13) - (Ve*Vtt*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)))/2. - (Ve*Vm*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))/2.))/v',
                    texname = '\\text{gPhiL11}')

gPhiL12 = Parameter(name = 'gPhiL12',
                    nature = 'internal',
                    type = 'real',
                    value = '(yme*cmath.sqrt(2)*((1 - Ve**2/2.)*cmath.cos(theta13)*cmath.sin(theta12) - (Ve*Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)))/2. - (Ve*Vm*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))/2.))/v',
                    texname = '\\text{gPhiL12}')

gPhiL13 = Parameter(name = 'gPhiL13',
                    nature = 'internal',
                    type = 'real',
                    value = '(yme*cmath.sqrt(2)*(-(Ve*Vtt*cmath.cos(theta13)*cmath.cos(theta23))/2. + (1 - Ve**2/2.)*cmath.sin(theta13) - (Ve*Vm*cmath.cos(theta13)*cmath.sin(theta23))/2.))/v',
                    texname = '\\text{gPhiL13}')

gPhiL14 = Parameter(name = 'gPhiL14',
                    nature = 'internal',
                    type = 'real',
                    value = '(Ve*yme*cmath.sqrt(2))/v',
                    texname = '\\text{gPhiL14}')

gPhiL21 = Parameter(name = 'gPhiL21',
                    nature = 'internal',
                    type = 'real',
                    value = '(ymm*cmath.sqrt(2)*(-(Ve*Vm*cmath.cos(theta12)*cmath.cos(theta13))/2. - (Vm*Vtt*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)))/2. + (1 - Vm**2/2.)*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23))))/v',
                    texname = '\\text{gPhiL21}')

gPhiL22 = Parameter(name = 'gPhiL22',
                    nature = 'internal',
                    type = 'real',
                    value = '(ymm*cmath.sqrt(2)*(-(Ve*Vm*cmath.cos(theta13)*cmath.sin(theta12))/2. - (Vm*Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)))/2. + (1 - Vm**2/2.)*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23))))/v',
                    texname = '\\text{gPhiL22}')

gPhiL23 = Parameter(name = 'gPhiL23',
                    nature = 'internal',
                    type = 'real',
                    value = '(ymm*cmath.sqrt(2)*(-(Vm*Vtt*cmath.cos(theta13)*cmath.cos(theta23))/2. - (Ve*Vm*cmath.sin(theta13))/2. + (1 - Vm**2/2.)*cmath.cos(theta13)*cmath.sin(theta23)))/v',
                    texname = '\\text{gPhiL23}')

gPhiL24 = Parameter(name = 'gPhiL24',
                    nature = 'internal',
                    type = 'real',
                    value = '(Vm*ymm*cmath.sqrt(2))/v',
                    texname = '\\text{gPhiL24}')

gPhiL31 = Parameter(name = 'gPhiL31',
                    nature = 'internal',
                    type = 'real',
                    value = '(ymtau*cmath.sqrt(2)*(-(Ve*Vtt*cmath.cos(theta12)*cmath.cos(theta13))/2. + (1 - Vtt**2/2.)*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) - (Vm*Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))/2.))/v',
                    texname = '\\text{gPhiL31}')

gPhiL32 = Parameter(name = 'gPhiL32',
                    nature = 'internal',
                    type = 'real',
                    value = '(ymtau*cmath.sqrt(2)*(-(Ve*Vtt*cmath.cos(theta13)*cmath.sin(theta12))/2. + (1 - Vtt**2/2.)*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) - (Vm*Vtt*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))/2.))/v',
                    texname = '\\text{gPhiL32}')

gPhiL33 = Parameter(name = 'gPhiL33',
                    nature = 'internal',
                    type = 'real',
                    value = '(ymtau*cmath.sqrt(2)*((1 - Vtt**2/2.)*cmath.cos(theta13)*cmath.cos(theta23) - (Ve*Vtt*cmath.sin(theta13))/2. - (Vm*Vtt*cmath.cos(theta13)*cmath.sin(theta23))/2.))/v',
                    texname = '\\text{gPhiL33}')

gPhiL34 = Parameter(name = 'gPhiL34',
                    nature = 'internal',
                    type = 'real',
                    value = '(Vtt*ymtau*cmath.sqrt(2))/v',
                    texname = '\\text{gPhiL34}')

gPhiL41 = Parameter(name = 'gPhiL41',
                    nature = 'internal',
                    type = 'real',
                    value = '(2*(Ve*yme**2*cmath.cos(theta12)*cmath.cos(theta13) + Vtt*ymtau**2*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Vm*ymm**2*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23))))/(mtr*v)',
                    texname = '\\text{gPhiL41}')

gPhiL42 = Parameter(name = 'gPhiL42',
                    nature = 'internal',
                    type = 'real',
                    value = '(2*(Ve*yme**2*cmath.cos(theta13)*cmath.sin(theta12) + Vtt*ymtau**2*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Vm*ymm**2*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23))))/(mtr*v)',
                    texname = '\\text{gPhiL42}')

gPhiL43 = Parameter(name = 'gPhiL43',
                    nature = 'internal',
                    type = 'real',
                    value = '(2*(Vtt*ymtau**2*cmath.cos(theta13)*cmath.cos(theta23) + Ve*yme**2*cmath.sin(theta13) + Vm*ymm**2*cmath.cos(theta13)*cmath.sin(theta23)))/(mtr*v)',
                    texname = '\\text{gPhiL43}')

gPhiR11 = Parameter(name = 'gPhiR11',
                    nature = 'internal',
                    type = 'real',
                    value = '-((mv1*cmath.cos(theta12)*cmath.cos(theta13)*cmath.sqrt(2))/v)',
                    texname = '\\text{gPhiR11}')

gPhiR12 = Parameter(name = 'gPhiR12',
                    nature = 'internal',
                    type = 'real',
                    value = '-((mv2*cmath.cos(theta13)*cmath.sqrt(2)*cmath.sin(theta12))/v)',
                    texname = '\\text{gPhiR12}')

gPhiR13 = Parameter(name = 'gPhiR13',
                    nature = 'internal',
                    type = 'real',
                    value = '-((mv3*cmath.sqrt(2)*cmath.sin(theta13))/v)',
                    texname = '\\text{gPhiR13}')

gPhiR14 = Parameter(name = 'gPhiR14',
                    nature = 'internal',
                    type = 'real',
                    value = '-((mtr*Ve*(-2 + 3*Ve**2 + 3*Vm**2 + 3*Vtt**2))/(v*cmath.sqrt(2))) - (2*cmath.sqrt(2)*(mv3*cmath.sin(theta13)*(Vtt*cmath.cos(theta13)*cmath.cos(theta23) + Ve*cmath.sin(theta13) + Vm*cmath.cos(theta13)*cmath.sin(theta23)) + mv1*cmath.cos(theta12)*cmath.cos(theta13)*(Ve*cmath.cos(theta12)*cmath.cos(theta13) + Vtt*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Vm*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23))) + mv2*cmath.cos(theta13)*cmath.sin(theta12)*(Ve*cmath.cos(theta13)*cmath.sin(theta12) + Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Vm*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))))/v',
                    texname = '\\text{gPhiR14}')

gPhiR21 = Parameter(name = 'gPhiR21',
                    nature = 'internal',
                    type = 'real',
                    value = '-((mv1*cmath.sqrt(2)*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))/v)',
                    texname = '\\text{gPhiR21}')

gPhiR22 = Parameter(name = 'gPhiR22',
                    nature = 'internal',
                    type = 'real',
                    value = '-((mv2*cmath.sqrt(2)*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))/v)',
                    texname = '\\text{gPhiR22}')

gPhiR23 = Parameter(name = 'gPhiR23',
                    nature = 'internal',
                    type = 'real',
                    value = '-((mv3*cmath.cos(theta13)*cmath.sqrt(2)*cmath.sin(theta23))/v)',
                    texname = '\\text{gPhiR23}')

gPhiR24 = Parameter(name = 'gPhiR24',
                    nature = 'internal',
                    type = 'real',
                    value = '-((mtr*Vm*(-2 + 3*Ve**2 + 3*Vm**2 + 3*Vtt**2))/(v*cmath.sqrt(2))) - (2*cmath.sqrt(2)*(mv3*cmath.cos(theta13)*cmath.sin(theta23)*(Vtt*cmath.cos(theta13)*cmath.cos(theta23) + Ve*cmath.sin(theta13) + Vm*cmath.cos(theta13)*cmath.sin(theta23)) + mv1*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23))*(Ve*cmath.cos(theta12)*cmath.cos(theta13) + Vtt*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Vm*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23))) + mv2*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23))*(Ve*cmath.cos(theta13)*cmath.sin(theta12) + Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Vm*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))))/v',
                    texname = '\\text{gPhiR24}')

gPhiR31 = Parameter(name = 'gPhiR31',
                    nature = 'internal',
                    type = 'real',
                    value = '-((mv1*cmath.sqrt(2)*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)))/v)',
                    texname = '\\text{gPhiR31}')

gPhiR32 = Parameter(name = 'gPhiR32',
                    nature = 'internal',
                    type = 'real',
                    value = '-((mv2*cmath.sqrt(2)*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)))/v)',
                    texname = '\\text{gPhiR32}')

gPhiR33 = Parameter(name = 'gPhiR33',
                    nature = 'internal',
                    type = 'real',
                    value = '-((mv3*cmath.cos(theta13)*cmath.cos(theta23)*cmath.sqrt(2))/v)',
                    texname = '\\text{gPhiR33}')

gPhiR34 = Parameter(name = 'gPhiR34',
                    nature = 'internal',
                    type = 'real',
                    value = '-((mtr*Vtt*(-2 + 3*Ve**2 + 3*Vm**2 + 3*Vtt**2))/(v*cmath.sqrt(2))) - (2*cmath.sqrt(2)*(mv3*cmath.cos(theta13)*cmath.cos(theta23)*(Vtt*cmath.cos(theta13)*cmath.cos(theta23) + Ve*cmath.sin(theta13) + Vm*cmath.cos(theta13)*cmath.sin(theta23)) + mv1*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23))*(Ve*cmath.cos(theta12)*cmath.cos(theta13) + Vtt*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Vm*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23))) + mv2*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23))*(Ve*cmath.cos(theta13)*cmath.sin(theta12) + Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Vm*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23)))))/v',
                    texname = '\\text{gPhiR34}')

gPhiR41 = Parameter(name = 'gPhiR41',
                    nature = 'internal',
                    type = 'real',
                    value = '(mtr*(-2 + Ve**2 + Vm**2 + Vtt**2)*(cmath.sin(theta12)*(-(Vm*cmath.cos(theta23)) + Vtt*cmath.sin(theta23)) + cmath.cos(theta12)*(Ve*cmath.cos(theta13) - cmath.sin(theta13)*(Vtt*cmath.cos(theta23) + Vm*cmath.sin(theta23)))))/v',
                    texname = '\\text{gPhiR41}')

gPhiR42 = Parameter(name = 'gPhiR42',
                    nature = 'internal',
                    type = 'real',
                    value = '(mtr*(-2 + Ve**2 + Vm**2 + Vtt**2)*(cmath.cos(theta12)*(Vm*cmath.cos(theta23) - Vtt*cmath.sin(theta23)) + cmath.sin(theta12)*(Ve*cmath.cos(theta13) - cmath.sin(theta13)*(Vtt*cmath.cos(theta23) + Vm*cmath.sin(theta23)))))/v',
                    texname = '\\text{gPhiR42}')

gPhiR43 = Parameter(name = 'gPhiR43',
                    nature = 'internal',
                    type = 'real',
                    value = '(mtr*(-2 + Ve**2 + Vm**2 + Vtt**2)*(Ve*cmath.sin(theta13) + cmath.cos(theta13)*(Vtt*cmath.cos(theta23) + Vm*cmath.sin(theta23))))/v',
                    texname = '\\text{gPhiR43}')

getalR11 = Parameter(name = 'getalR11',
                     nature = 'internal',
                     type = 'real',
                     value = '((1 + Ve**2)*yme)/v',
                     texname = '\\text{getalR11}')

getalR12 = Parameter(name = 'getalR12',
                     nature = 'internal',
                     type = 'real',
                     value = '(Ve*Vm*ymm)/v',
                     texname = '\\text{getalR12}')

getalR13 = Parameter(name = 'getalR13',
                     nature = 'internal',
                     type = 'real',
                     value = '(Ve*Vtt*ymtau)/v',
                     texname = '\\text{getalR13}')

getalR14 = Parameter(name = 'getalR14',
                     nature = 'internal',
                     type = 'real',
                     value = '-((mtr*Ve*(1 - Ve**2 - Vm**2 - Vtt**2)*cmath.sqrt(2))/v) + (Ve*yme**2*cmath.sqrt(2))/(mtr*v)',
                     texname = '\\text{getalR14}')

getalR21 = Parameter(name = 'getalR21',
                     nature = 'internal',
                     type = 'real',
                     value = '(Ve*Vm*yme)/v',
                     texname = '\\text{getalR21}')

getalR22 = Parameter(name = 'getalR22',
                     nature = 'internal',
                     type = 'real',
                     value = '((1 + Vm**2)*ymm)/v',
                     texname = '\\text{getalR22}')

getalR23 = Parameter(name = 'getalR23',
                     nature = 'internal',
                     type = 'real',
                     value = '(Vm*Vtt*ymtau)/v',
                     texname = '\\text{getalR23}')

getalR24 = Parameter(name = 'getalR24',
                     nature = 'internal',
                     type = 'real',
                     value = '-((mtr*Vm*(1 - Ve**2 - Vm**2 - Vtt**2)*cmath.sqrt(2))/v) + (Vm*ymm**2*cmath.sqrt(2))/(mtr*v)',
                     texname = '\\text{getalR24}')

getalR31 = Parameter(name = 'getalR31',
                     nature = 'internal',
                     type = 'real',
                     value = '(Ve*Vtt*yme)/v',
                     texname = '\\text{getalR31}')

getalR32 = Parameter(name = 'getalR32',
                     nature = 'internal',
                     type = 'real',
                     value = '(Vm*Vtt*ymm)/v',
                     texname = '\\text{getalR32}')

getalR33 = Parameter(name = 'getalR33',
                     nature = 'internal',
                     type = 'real',
                     value = '((1 + Vtt**2)*ymtau)/v',
                     texname = '\\text{getalR33}')

getalR34 = Parameter(name = 'getalR34',
                     nature = 'internal',
                     type = 'real',
                     value = '-((mtr*Vtt*(1 - Ve**2 - Vm**2 - Vtt**2)*cmath.sqrt(2))/v) + (Vtt*ymtau**2*cmath.sqrt(2))/(mtr*v)',
                     texname = '\\text{getalR34}')

getalR41 = Parameter(name = 'getalR41',
                     nature = 'internal',
                     type = 'real',
                     value = '(Ve*yme*cmath.sqrt(2))/v',
                     texname = '\\text{getalR41}')

getalR42 = Parameter(name = 'getalR42',
                     nature = 'internal',
                     type = 'real',
                     value = '(Vm*ymm*cmath.sqrt(2))/v',
                     texname = '\\text{getalR42}')

getalR43 = Parameter(name = 'getalR43',
                     nature = 'internal',
                     type = 'real',
                     value = '(Vtt*ymtau*cmath.sqrt(2))/v',
                     texname = '\\text{getalR43}')

getalR44 = Parameter(name = 'getalR44',
                     nature = 'internal',
                     type = 'real',
                     value = '(-2*mtr*(Ve**2 + Vm**2 + Vtt**2))/v',
                     texname = '\\text{getalR44}')

getanuR11 = Parameter(name = 'getanuR11',
                      nature = 'internal',
                      type = 'real',
                      value = '-((mv1*cmath.sqrt(2))/v)',
                      texname = '\\text{getanuR11}')

getanuR14 = Parameter(name = 'getanuR14',
                      nature = 'internal',
                      type = 'real',
                      value = '(mtr*(-1 + Ve**2 + Vm**2 + Vtt**2)*cmath.sqrt(2)*(cmath.sin(theta12)*(-(Vm*cmath.cos(theta23)) + Vtt*cmath.sin(theta23)) + cmath.cos(theta12)*(Ve*cmath.cos(theta13) - cmath.sin(theta13)*(Vtt*cmath.cos(theta23) + Vm*cmath.sin(theta23)))))/v',
                      texname = '\\text{getanuR14}')

getanuR22 = Parameter(name = 'getanuR22',
                      nature = 'internal',
                      type = 'real',
                      value = '-((mv2*cmath.sqrt(2))/v)',
                      texname = '\\text{getanuR22}')

getanuR24 = Parameter(name = 'getanuR24',
                      nature = 'internal',
                      type = 'real',
                      value = '(mtr*(-1 + Ve**2 + Vm**2 + Vtt**2)*cmath.sqrt(2)*(cmath.cos(theta12)*(Vm*cmath.cos(theta23) - Vtt*cmath.sin(theta23)) + cmath.sin(theta12)*(Ve*cmath.cos(theta13) - cmath.sin(theta13)*(Vtt*cmath.cos(theta23) + Vm*cmath.sin(theta23)))))/v',
                      texname = '\\text{getanuR24}')

getanuR33 = Parameter(name = 'getanuR33',
                      nature = 'internal',
                      type = 'real',
                      value = '-((mv3*cmath.sqrt(2))/v)',
                      texname = '\\text{getanuR33}')

getanuR34 = Parameter(name = 'getanuR34',
                      nature = 'internal',
                      type = 'real',
                      value = '(mtr*(-1 + Ve**2 + Vm**2 + Vtt**2)*cmath.sqrt(2)*(Ve*cmath.sin(theta13) + cmath.cos(theta13)*(Vtt*cmath.cos(theta23) + Vm*cmath.sin(theta23))))/v',
                      texname = '\\text{getanuR34}')

getanuR41 = Parameter(name = 'getanuR41',
                      nature = 'internal',
                      type = 'real',
                      value = '-((mv1*cmath.sqrt(2)*(Ve*cmath.cos(theta12)*cmath.cos(theta13) + Vtt*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Vm*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23))))/v)',
                      texname = '\\text{getanuR41}')

getanuR42 = Parameter(name = 'getanuR42',
                      nature = 'internal',
                      type = 'real',
                      value = '-((mv2*cmath.sqrt(2)*(Ve*cmath.cos(theta13)*cmath.sin(theta12) + Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Vm*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23))))/v)',
                      texname = '\\text{getanuR42}')

getanuR43 = Parameter(name = 'getanuR43',
                      nature = 'internal',
                      type = 'real',
                      value = '-((mv3*cmath.sqrt(2)*(Vtt*cmath.cos(theta13)*cmath.cos(theta23) + Ve*cmath.sin(theta13) + Vm*cmath.cos(theta13)*cmath.sin(theta23)))/v)',
                      texname = '\\text{getanuR43}')

getanuR44 = Parameter(name = 'getanuR44',
                      nature = 'internal',
                      type = 'real',
                      value = '-((mtr*(Ve**2 + Vm**2 + Vtt**2)*cmath.sqrt(2))/v)',
                      texname = '\\text{getanuR44}')

gHlR11 = Parameter(name = 'gHlR11',
                   nature = 'internal',
                   type = 'real',
                   value = '((1 - 3*Ve**2)*yme)/v',
                   texname = '\\text{gHlR11}')

gHlR12 = Parameter(name = 'gHlR12',
                   nature = 'internal',
                   type = 'real',
                   value = '(-3*Ve*Vm*ymm)/v',
                   texname = '\\text{gHlR12}')

gHlR13 = Parameter(name = 'gHlR13',
                   nature = 'internal',
                   type = 'real',
                   value = '(-3*Ve*Vtt*ymtau)/v',
                   texname = '\\text{gHlR13}')

gHlR14 = Parameter(name = 'gHlR14',
                   nature = 'internal',
                   type = 'real',
                   value = '(mtr*Ve*(1 - Ve**2 - Vm**2 - Vtt**2)*cmath.sqrt(2))/v + (Ve*yme**2*cmath.sqrt(2))/(mtr*v)',
                   texname = '\\text{gHlR14}')

gHlR21 = Parameter(name = 'gHlR21',
                   nature = 'internal',
                   type = 'real',
                   value = '(-3*Ve*Vm*yme)/v',
                   texname = '\\text{gHlR21}')

gHlR22 = Parameter(name = 'gHlR22',
                   nature = 'internal',
                   type = 'real',
                   value = '((1 - 3*Vm**2)*ymm)/v',
                   texname = '\\text{gHlR22}')

gHlR23 = Parameter(name = 'gHlR23',
                   nature = 'internal',
                   type = 'real',
                   value = '(-3*Vm*Vtt*ymtau)/v',
                   texname = '\\text{gHlR23}')

gHlR24 = Parameter(name = 'gHlR24',
                   nature = 'internal',
                   type = 'real',
                   value = '(mtr*Vm*(1 - Ve**2 - Vm**2 - Vtt**2)*cmath.sqrt(2))/v + (Vm*ymm**2*cmath.sqrt(2))/(mtr*v)',
                   texname = '\\text{gHlR24}')

gHlR31 = Parameter(name = 'gHlR31',
                   nature = 'internal',
                   type = 'real',
                   value = '(-3*Ve*Vtt*yme)/v',
                   texname = '\\text{gHlR31}')

gHlR32 = Parameter(name = 'gHlR32',
                   nature = 'internal',
                   type = 'real',
                   value = '(-3*Vm*Vtt*ymm)/v',
                   texname = '\\text{gHlR32}')

gHlR33 = Parameter(name = 'gHlR33',
                   nature = 'internal',
                   type = 'real',
                   value = '((1 - 3*Vtt**2)*ymtau)/v',
                   texname = '\\text{gHlR33}')

gHlR34 = Parameter(name = 'gHlR34',
                   nature = 'internal',
                   type = 'real',
                   value = '(mtr*Vtt*(1 - Ve**2 - Vm**2 - Vtt**2)*cmath.sqrt(2))/v + (Vtt*ymtau**2*cmath.sqrt(2))/(mtr*v)',
                   texname = '\\text{gHlR34}')

gHlR41 = Parameter(name = 'gHlR41',
                   nature = 'internal',
                   type = 'real',
                   value = '(Ve*yme*cmath.sqrt(2))/v',
                   texname = '\\text{gHlR41}')

gHlR42 = Parameter(name = 'gHlR42',
                   nature = 'internal',
                   type = 'real',
                   value = '(Vm*ymm*cmath.sqrt(2))/v',
                   texname = '\\text{gHlR42}')

gHlR43 = Parameter(name = 'gHlR43',
                   nature = 'internal',
                   type = 'real',
                   value = '(Vtt*ymtau*cmath.sqrt(2))/v',
                   texname = '\\text{gHlR43}')

gHlR44 = Parameter(name = 'gHlR44',
                   nature = 'internal',
                   type = 'real',
                   value = '(2*mtr*(Ve**2 + Vm**2 + Vtt**2))/v',
                   texname = '\\text{gHlR44}')

gHnuR11 = Parameter(name = 'gHnuR11',
                    nature = 'internal',
                    type = 'real',
                    value = '(mv1*cmath.sqrt(2))/v',
                    texname = '\\text{gHnuR11}')

gHnuR14 = Parameter(name = 'gHnuR14',
                    nature = 'internal',
                    type = 'real',
                    value = '-((mtr*(-1 + Ve**2 + Vm**2 + Vtt**2)*cmath.sqrt(2)*(cmath.sin(theta12)*(-(Vm*cmath.cos(theta23)) + Vtt*cmath.sin(theta23)) + cmath.cos(theta12)*(Ve*cmath.cos(theta13) - cmath.sin(theta13)*(Vtt*cmath.cos(theta23) + Vm*cmath.sin(theta23)))))/v)',
                    texname = '\\text{gHnuR14}')

gHnuR22 = Parameter(name = 'gHnuR22',
                    nature = 'internal',
                    type = 'real',
                    value = '(mv2*cmath.sqrt(2))/v',
                    texname = '\\text{gHnuR22}')

gHnuR24 = Parameter(name = 'gHnuR24',
                    nature = 'internal',
                    type = 'real',
                    value = '-((mtr*(-1 + Ve**2 + Vm**2 + Vtt**2)*cmath.sqrt(2)*(cmath.cos(theta12)*(Vm*cmath.cos(theta23) - Vtt*cmath.sin(theta23)) + cmath.sin(theta12)*(Ve*cmath.cos(theta13) - cmath.sin(theta13)*(Vtt*cmath.cos(theta23) + Vm*cmath.sin(theta23)))))/v)',
                    texname = '\\text{gHnuR24}')

gHnuR33 = Parameter(name = 'gHnuR33',
                    nature = 'internal',
                    type = 'real',
                    value = '(mv3*cmath.sqrt(2))/v',
                    texname = '\\text{gHnuR33}')

gHnuR34 = Parameter(name = 'gHnuR34',
                    nature = 'internal',
                    type = 'real',
                    value = '-((mtr*(-1 + Ve**2 + Vm**2 + Vtt**2)*cmath.sqrt(2)*(Ve*cmath.sin(theta13) + cmath.cos(theta13)*(Vtt*cmath.cos(theta23) + Vm*cmath.sin(theta23))))/v)',
                    texname = '\\text{gHnuR34}')

gHnuR41 = Parameter(name = 'gHnuR41',
                    nature = 'internal',
                    type = 'real',
                    value = '(mv1*cmath.sqrt(2)*(Ve*cmath.cos(theta12)*cmath.cos(theta13) + Vtt*(-(cmath.cos(theta12)*cmath.cos(theta23)*cmath.sin(theta13)) + cmath.sin(theta12)*cmath.sin(theta23)) + Vm*(-(cmath.cos(theta23)*cmath.sin(theta12)) - cmath.cos(theta12)*cmath.sin(theta13)*cmath.sin(theta23))))/v',
                    texname = '\\text{gHnuR41}')

gHnuR42 = Parameter(name = 'gHnuR42',
                    nature = 'internal',
                    type = 'real',
                    value = '(mv2*cmath.sqrt(2)*(Ve*cmath.cos(theta13)*cmath.sin(theta12) + Vtt*(-(cmath.cos(theta23)*cmath.sin(theta12)*cmath.sin(theta13)) - cmath.cos(theta12)*cmath.sin(theta23)) + Vm*(cmath.cos(theta12)*cmath.cos(theta23) - cmath.sin(theta12)*cmath.sin(theta13)*cmath.sin(theta23))))/v',
                    texname = '\\text{gHnuR42}')

gHnuR43 = Parameter(name = 'gHnuR43',
                    nature = 'internal',
                    type = 'real',
                    value = '(mv3*cmath.sqrt(2)*(Vtt*cmath.cos(theta13)*cmath.cos(theta23) + Ve*cmath.sin(theta13) + Vm*cmath.cos(theta13)*cmath.sin(theta23)))/v',
                    texname = '\\text{gHnuR43}')

gHnuR44 = Parameter(name = 'gHnuR44',
                    nature = 'internal',
                    type = 'real',
                    value = '(mtr*(Ve**2 + Vm**2 + Vtt**2)*cmath.sqrt(2))/v',
                    texname = '\\text{gHnuR44}')

lam = Parameter(name = 'lam',
                nature = 'internal',
                type = 'real',
                value = 'MH**2/(2.*v**2)',
                texname = '\\text{lam}')

yb = Parameter(name = 'yb',
               nature = 'internal',
               type = 'real',
               value = '(ymb*cmath.sqrt(2))/v',
               texname = '\\text{yb}')

yc = Parameter(name = 'yc',
               nature = 'internal',
               type = 'real',
               value = '(ymc*cmath.sqrt(2))/v',
               texname = '\\text{yc}')

ye = Parameter(name = 'ye',
               nature = 'internal',
               type = 'real',
               value = '(yme*cmath.sqrt(2))/v',
               texname = '\\text{ye}')

ym = Parameter(name = 'ym',
               nature = 'internal',
               type = 'real',
               value = '(ymm*cmath.sqrt(2))/v',
               texname = '\\text{ym}')

yt = Parameter(name = 'yt',
               nature = 'internal',
               type = 'real',
               value = '(ymt*cmath.sqrt(2))/v',
               texname = '\\text{yt}')

ytau = Parameter(name = 'ytau',
                 nature = 'internal',
                 type = 'real',
                 value = '(ymtau*cmath.sqrt(2))/v',
                 texname = '\\text{ytau}')

muH = Parameter(name = 'muH',
                nature = 'internal',
                type = 'real',
                value = 'cmath.sqrt(lam*v**2)',
                texname = '\\mu')


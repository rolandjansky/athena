# This file was automatically created by FeynRules 1.7.100
# Mathematica version: 7.0 for Linux x86 (64-bit) (February 18, 2009)
# Date: Wed 28 Nov 2012 10:15:27



from object_library import all_parameters, Parameter


from function_library import complexconjugate, re, im, csc, sec, acsc, asec

# This is a default parameter object representing 0.
ZERO = Parameter(name = 'ZERO',
                 nature = 'internal',
                 type = 'real',
                 value = '0.0',
                 texname = '0')

# User-defined parameters.
A0FC1x1 = Parameter(name = 'A0FC1x1',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{A0FC1x1}',
                    lhablock = 'A0FC',
                    lhacode = [ 1, 1 ])

A0FC1x2 = Parameter(name = 'A0FC1x2',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{A0FC1x2}',
                    lhablock = 'A0FC',
                    lhacode = [ 1, 2 ])

A0FC1x3 = Parameter(name = 'A0FC1x3',
                    nature = 'external',
                    type = 'real',
                    value = 0.1,
                    texname = '\\text{A0FC1x3}',
                    lhablock = 'A0FC',
                    lhacode = [ 1, 3 ])

A0FC2x1 = Parameter(name = 'A0FC2x1',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{A0FC2x1}',
                    lhablock = 'A0FC',
                    lhacode = [ 2, 1 ])

A0FC2x2 = Parameter(name = 'A0FC2x2',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{A0FC2x2}',
                    lhablock = 'A0FC',
                    lhacode = [ 2, 2 ])

A0FC2x3 = Parameter(name = 'A0FC2x3',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{A0FC2x3}',
                    lhablock = 'A0FC',
                    lhacode = [ 2, 3 ])

A0FC3x1 = Parameter(name = 'A0FC3x1',
                    nature = 'external',
                    type = 'real',
                    value = 0.1,
                    texname = '\\text{A0FC3x1}',
                    lhablock = 'A0FC',
                    lhacode = [ 3, 1 ])

A0FC3x2 = Parameter(name = 'A0FC3x2',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{A0FC3x2}',
                    lhablock = 'A0FC',
                    lhacode = [ 3, 2 ])

A0FC3x3 = Parameter(name = 'A0FC3x3',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{A0FC3x3}',
                    lhablock = 'A0FC',
                    lhacode = [ 3, 3 ])

A12S1 = Parameter(name = 'A12S1',
                  nature = 'external',
                  type = 'real',
                  value = 0.,
                  texname = '\\text{A12S1}',
                  lhablock = 'A12S',
                  lhacode = [ 1 ])

A12S2 = Parameter(name = 'A12S2',
                  nature = 'external',
                  type = 'real',
                  value = 0.,
                  texname = '\\text{A12S2}',
                  lhablock = 'A12S',
                  lhacode = [ 2 ])

A12S3 = Parameter(name = 'A12S3',
                  nature = 'external',
                  type = 'real',
                  value = 0.1,
                  texname = '\\text{A12S3}',
                  lhablock = 'A12S',
                  lhacode = [ 3 ])

A12V1 = Parameter(name = 'A12V1',
                  nature = 'external',
                  type = 'real',
                  value = 0.,
                  texname = '\\text{A12V1}',
                  lhablock = 'A12V',
                  lhacode = [ 1 ])

A12V2 = Parameter(name = 'A12V2',
                  nature = 'external',
                  type = 'real',
                  value = 0.,
                  texname = '\\text{A12V2}',
                  lhablock = 'A12V',
                  lhacode = [ 2 ])

A12V3 = Parameter(name = 'A12V3',
                  nature = 'external',
                  type = 'real',
                  value = 0.1,
                  texname = '\\text{A12V3}',
                  lhablock = 'A12V',
                  lhacode = [ 3 ])

A1FC1x1 = Parameter(name = 'A1FC1x1',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{A1FC1x1}',
                    lhablock = 'A1FC',
                    lhacode = [ 1, 1 ])

A1FC1x2 = Parameter(name = 'A1FC1x2',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{A1FC1x2}',
                    lhablock = 'A1FC',
                    lhacode = [ 1, 2 ])

A1FC1x3 = Parameter(name = 'A1FC1x3',
                    nature = 'external',
                    type = 'real',
                    value = 0.1,
                    texname = '\\text{A1FC1x3}',
                    lhablock = 'A1FC',
                    lhacode = [ 1, 3 ])

A1FC2x1 = Parameter(name = 'A1FC2x1',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{A1FC2x1}',
                    lhablock = 'A1FC',
                    lhacode = [ 2, 1 ])

A1FC2x2 = Parameter(name = 'A1FC2x2',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{A1FC2x2}',
                    lhablock = 'A1FC',
                    lhacode = [ 2, 2 ])

A1FC2x3 = Parameter(name = 'A1FC2x3',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{A1FC2x3}',
                    lhablock = 'A1FC',
                    lhacode = [ 2, 3 ])

A1FC3x1 = Parameter(name = 'A1FC3x1',
                    nature = 'external',
                    type = 'real',
                    value = 0.1,
                    texname = '\\text{A1FC3x1}',
                    lhablock = 'A1FC',
                    lhacode = [ 3, 1 ])

A1FC3x2 = Parameter(name = 'A1FC3x2',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{A1FC3x2}',
                    lhablock = 'A1FC',
                    lhacode = [ 3, 2 ])

A1FC3x3 = Parameter(name = 'A1FC3x3',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{A1FC3x3}',
                    lhablock = 'A1FC',
                    lhacode = [ 3, 3 ])

AQS1x1 = Parameter(name = 'AQS1x1',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{AQS1x1}',
                   lhablock = 'AQS',
                   lhacode = [ 1, 1 ])

AQS1x2 = Parameter(name = 'AQS1x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.1,
                   texname = '\\text{AQS1x2}',
                   lhablock = 'AQS',
                   lhacode = [ 1, 2 ])

AQS1x3 = Parameter(name = 'AQS1x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{AQS1x3}',
                   lhablock = 'AQS',
                   lhacode = [ 1, 3 ])

AQS2x1 = Parameter(name = 'AQS2x1',
                   nature = 'external',
                   type = 'real',
                   value = -0.1,
                   texname = '\\text{AQS2x1}',
                   lhablock = 'AQS',
                   lhacode = [ 2, 1 ])

AQS2x2 = Parameter(name = 'AQS2x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{AQS2x2}',
                   lhablock = 'AQS',
                   lhacode = [ 2, 2 ])

AQS2x3 = Parameter(name = 'AQS2x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{AQS2x3}',
                   lhablock = 'AQS',
                   lhacode = [ 2, 3 ])

AQS3x1 = Parameter(name = 'AQS3x1',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{AQS3x1}',
                   lhablock = 'AQS',
                   lhacode = [ 3, 1 ])

AQS3x2 = Parameter(name = 'AQS3x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{AQS3x2}',
                   lhablock = 'AQS',
                   lhacode = [ 3, 2 ])

AQS3x3 = Parameter(name = 'AQS3x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{AQS3x3}',
                   lhablock = 'AQS',
                   lhacode = [ 3, 3 ])

AQV1x1 = Parameter(name = 'AQV1x1',
                   nature = 'external',
                   type = 'real',
                   value = 0.1,
                   texname = '\\text{AQV1x1}',
                   lhablock = 'AQV',
                   lhacode = [ 1, 1 ])

AQV1x2 = Parameter(name = 'AQV1x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{AQV1x2}',
                   lhablock = 'AQV',
                   lhacode = [ 1, 2 ])

AQV1x3 = Parameter(name = 'AQV1x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{AQV1x3}',
                   lhablock = 'AQV',
                   lhacode = [ 1, 3 ])

AQV2x1 = Parameter(name = 'AQV2x1',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{AQV2x1}',
                   lhablock = 'AQV',
                   lhacode = [ 2, 1 ])

AQV2x2 = Parameter(name = 'AQV2x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{AQV2x2}',
                   lhablock = 'AQV',
                   lhacode = [ 2, 2 ])

AQV2x3 = Parameter(name = 'AQV2x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{AQV2x3}',
                   lhablock = 'AQV',
                   lhacode = [ 2, 3 ])

AQV3x1 = Parameter(name = 'AQV3x1',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{AQV3x1}',
                   lhablock = 'AQV',
                   lhacode = [ 3, 1 ])

AQV3x2 = Parameter(name = 'AQV3x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{AQV3x2}',
                   lhablock = 'AQV',
                   lhacode = [ 3, 2 ])

AQV3x3 = Parameter(name = 'AQV3x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{AQV3x3}',
                   lhablock = 'AQV',
                   lhacode = [ 3, 3 ])

B0FC1x1 = Parameter(name = 'B0FC1x1',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{B0FC1x1}',
                    lhablock = 'B0FC',
                    lhacode = [ 1, 1 ])

B0FC1x2 = Parameter(name = 'B0FC1x2',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{B0FC1x2}',
                    lhablock = 'B0FC',
                    lhacode = [ 1, 2 ])

B0FC1x3 = Parameter(name = 'B0FC1x3',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{B0FC1x3}',
                    lhablock = 'B0FC',
                    lhacode = [ 1, 3 ])

B0FC2x1 = Parameter(name = 'B0FC2x1',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{B0FC2x1}',
                    lhablock = 'B0FC',
                    lhacode = [ 2, 1 ])

B0FC2x2 = Parameter(name = 'B0FC2x2',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{B0FC2x2}',
                    lhablock = 'B0FC',
                    lhacode = [ 2, 2 ])

B0FC2x3 = Parameter(name = 'B0FC2x3',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{B0FC2x3}',
                    lhablock = 'B0FC',
                    lhacode = [ 2, 3 ])

B0FC3x1 = Parameter(name = 'B0FC3x1',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{B0FC3x1}',
                    lhablock = 'B0FC',
                    lhacode = [ 3, 1 ])

B0FC3x2 = Parameter(name = 'B0FC3x2',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{B0FC3x2}',
                    lhablock = 'B0FC',
                    lhacode = [ 3, 2 ])

B0FC3x3 = Parameter(name = 'B0FC3x3',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{B0FC3x3}',
                    lhablock = 'B0FC',
                    lhacode = [ 3, 3 ])

B12S1 = Parameter(name = 'B12S1',
                  nature = 'external',
                  type = 'real',
                  value = 0,
                  texname = '\\text{B12S1}',
                  lhablock = 'B12S',
                  lhacode = [ 1 ])

B12S2 = Parameter(name = 'B12S2',
                  nature = 'external',
                  type = 'real',
                  value = 0,
                  texname = '\\text{B12S2}',
                  lhablock = 'B12S',
                  lhacode = [ 2 ])

B12S3 = Parameter(name = 'B12S3',
                  nature = 'external',
                  type = 'real',
                  value = 0,
                  texname = '\\text{B12S3}',
                  lhablock = 'B12S',
                  lhacode = [ 3 ])

B12V1 = Parameter(name = 'B12V1',
                  nature = 'external',
                  type = 'real',
                  value = 0,
                  texname = '\\text{B12V1}',
                  lhablock = 'B12V',
                  lhacode = [ 1 ])

B12V2 = Parameter(name = 'B12V2',
                  nature = 'external',
                  type = 'real',
                  value = 0,
                  texname = '\\text{B12V2}',
                  lhablock = 'B12V',
                  lhacode = [ 2 ])

B12V3 = Parameter(name = 'B12V3',
                  nature = 'external',
                  type = 'real',
                  value = 0,
                  texname = '\\text{B12V3}',
                  lhablock = 'B12V',
                  lhacode = [ 3 ])

B1FC1x1 = Parameter(name = 'B1FC1x1',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{B1FC1x1}',
                    lhablock = 'B1FC',
                    lhacode = [ 1, 1 ])

B1FC1x2 = Parameter(name = 'B1FC1x2',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{B1FC1x2}',
                    lhablock = 'B1FC',
                    lhacode = [ 1, 2 ])

B1FC1x3 = Parameter(name = 'B1FC1x3',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{B1FC1x3}',
                    lhablock = 'B1FC',
                    lhacode = [ 1, 3 ])

B1FC2x1 = Parameter(name = 'B1FC2x1',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{B1FC2x1}',
                    lhablock = 'B1FC',
                    lhacode = [ 2, 1 ])

B1FC2x2 = Parameter(name = 'B1FC2x2',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{B1FC2x2}',
                    lhablock = 'B1FC',
                    lhacode = [ 2, 2 ])

B1FC2x3 = Parameter(name = 'B1FC2x3',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{B1FC2x3}',
                    lhablock = 'B1FC',
                    lhacode = [ 2, 3 ])

B1FC3x1 = Parameter(name = 'B1FC3x1',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{B1FC3x1}',
                    lhablock = 'B1FC',
                    lhacode = [ 3, 1 ])

B1FC3x2 = Parameter(name = 'B1FC3x2',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{B1FC3x2}',
                    lhablock = 'B1FC',
                    lhacode = [ 3, 2 ])

B1FC3x3 = Parameter(name = 'B1FC3x3',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{B1FC3x3}',
                    lhablock = 'B1FC',
                    lhacode = [ 3, 3 ])

BQS1x1 = Parameter(name = 'BQS1x1',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{BQS1x1}',
                   lhablock = 'BQS',
                   lhacode = [ 1, 1 ])

BQS1x2 = Parameter(name = 'BQS1x2',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{BQS1x2}',
                   lhablock = 'BQS',
                   lhacode = [ 1, 2 ])

BQS1x3 = Parameter(name = 'BQS1x3',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{BQS1x3}',
                   lhablock = 'BQS',
                   lhacode = [ 1, 3 ])

BQS2x1 = Parameter(name = 'BQS2x1',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{BQS2x1}',
                   lhablock = 'BQS',
                   lhacode = [ 2, 1 ])

BQS2x2 = Parameter(name = 'BQS2x2',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{BQS2x2}',
                   lhablock = 'BQS',
                   lhacode = [ 2, 2 ])

BQS2x3 = Parameter(name = 'BQS2x3',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{BQS2x3}',
                   lhablock = 'BQS',
                   lhacode = [ 2, 3 ])

BQS3x1 = Parameter(name = 'BQS3x1',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{BQS3x1}',
                   lhablock = 'BQS',
                   lhacode = [ 3, 1 ])

BQS3x2 = Parameter(name = 'BQS3x2',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{BQS3x2}',
                   lhablock = 'BQS',
                   lhacode = [ 3, 2 ])

BQS3x3 = Parameter(name = 'BQS3x3',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{BQS3x3}',
                   lhablock = 'BQS',
                   lhacode = [ 3, 3 ])

BQV1x1 = Parameter(name = 'BQV1x1',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{BQV1x1}',
                   lhablock = 'BQV',
                   lhacode = [ 1, 1 ])

BQV1x2 = Parameter(name = 'BQV1x2',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{BQV1x2}',
                   lhablock = 'BQV',
                   lhacode = [ 1, 2 ])

BQV1x3 = Parameter(name = 'BQV1x3',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{BQV1x3}',
                   lhablock = 'BQV',
                   lhacode = [ 1, 3 ])

BQV2x1 = Parameter(name = 'BQV2x1',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{BQV2x1}',
                   lhablock = 'BQV',
                   lhacode = [ 2, 1 ])

BQV2x2 = Parameter(name = 'BQV2x2',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{BQV2x2}',
                   lhablock = 'BQV',
                   lhacode = [ 2, 2 ])

BQV2x3 = Parameter(name = 'BQV2x3',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{BQV2x3}',
                   lhablock = 'BQV',
                   lhacode = [ 2, 3 ])

BQV3x1 = Parameter(name = 'BQV3x1',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{BQV3x1}',
                   lhablock = 'BQV',
                   lhacode = [ 3, 1 ])

BQV3x2 = Parameter(name = 'BQV3x2',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{BQV3x2}',
                   lhablock = 'BQV',
                   lhacode = [ 3, 2 ])

BQV3x3 = Parameter(name = 'BQV3x3',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{BQV3x3}',
                   lhablock = 'BQV',
                   lhacode = [ 3, 3 ])

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

tA12S1 = Parameter(name = 'tA12S1',
                   nature = 'external',
                   type = 'real',
                   value = 0.1,
                   texname = '\\text{tA12S1}',
                   lhablock = 'tA12S',
                   lhacode = [ 1 ])

tA12S2 = Parameter(name = 'tA12S2',
                   nature = 'external',
                   type = 'real',
                   value = 0.1,
                   texname = '\\text{tA12S2}',
                   lhablock = 'tA12S',
                   lhacode = [ 2 ])

tA12S3 = Parameter(name = 'tA12S3',
                   nature = 'external',
                   type = 'real',
                   value = 0.,
                   texname = '\\text{tA12S3}',
                   lhablock = 'tA12S',
                   lhacode = [ 3 ])

tAQS1x1 = Parameter(name = 'tAQS1x1',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{tAQS1x1}',
                    lhablock = 'tAQS',
                    lhacode = [ 1, 1 ])

tAQS1x2 = Parameter(name = 'tAQS1x2',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{tAQS1x2}',
                    lhablock = 'tAQS',
                    lhacode = [ 1, 2 ])

tAQS1x3 = Parameter(name = 'tAQS1x3',
                    nature = 'external',
                    type = 'real',
                    value = 0.1,
                    texname = '\\text{tAQS1x3}',
                    lhablock = 'tAQS',
                    lhacode = [ 1, 3 ])

tAQS2x1 = Parameter(name = 'tAQS2x1',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{tAQS2x1}',
                    lhablock = 'tAQS',
                    lhacode = [ 2, 1 ])

tAQS2x2 = Parameter(name = 'tAQS2x2',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{tAQS2x2}',
                    lhablock = 'tAQS',
                    lhacode = [ 2, 2 ])

tAQS2x3 = Parameter(name = 'tAQS2x3',
                    nature = 'external',
                    type = 'real',
                    value = 0.1,
                    texname = '\\text{tAQS2x3}',
                    lhablock = 'tAQS',
                    lhacode = [ 2, 3 ])

tAQS3x1 = Parameter(name = 'tAQS3x1',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{tAQS3x1}',
                    lhablock = 'tAQS',
                    lhacode = [ 3, 1 ])

tAQS3x2 = Parameter(name = 'tAQS3x2',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{tAQS3x2}',
                    lhablock = 'tAQS',
                    lhacode = [ 3, 2 ])

tAQS3x3 = Parameter(name = 'tAQS3x3',
                    nature = 'external',
                    type = 'real',
                    value = 0.,
                    texname = '\\text{tAQS3x3}',
                    lhablock = 'tAQS',
                    lhacode = [ 3, 3 ])

tB12S1 = Parameter(name = 'tB12S1',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{tB12S1}',
                   lhablock = 'tB12S',
                   lhacode = [ 1 ])

tB12S2 = Parameter(name = 'tB12S2',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{tB12S2}',
                   lhablock = 'tB12S',
                   lhacode = [ 2 ])

tB12S3 = Parameter(name = 'tB12S3',
                   nature = 'external',
                   type = 'real',
                   value = 0,
                   texname = '\\text{tB12S3}',
                   lhablock = 'tB12S',
                   lhacode = [ 3 ])

tBQS1x1 = Parameter(name = 'tBQS1x1',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{tBQS1x1}',
                    lhablock = 'tBQS',
                    lhacode = [ 1, 1 ])

tBQS1x2 = Parameter(name = 'tBQS1x2',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{tBQS1x2}',
                    lhablock = 'tBQS',
                    lhacode = [ 1, 2 ])

tBQS1x3 = Parameter(name = 'tBQS1x3',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{tBQS1x3}',
                    lhablock = 'tBQS',
                    lhacode = [ 1, 3 ])

tBQS2x1 = Parameter(name = 'tBQS2x1',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{tBQS2x1}',
                    lhablock = 'tBQS',
                    lhacode = [ 2, 1 ])

tBQS2x2 = Parameter(name = 'tBQS2x2',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{tBQS2x2}',
                    lhablock = 'tBQS',
                    lhacode = [ 2, 2 ])

tBQS2x3 = Parameter(name = 'tBQS2x3',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{tBQS2x3}',
                    lhablock = 'tBQS',
                    lhacode = [ 2, 3 ])

tBQS3x1 = Parameter(name = 'tBQS3x1',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{tBQS3x1}',
                    lhablock = 'tBQS',
                    lhacode = [ 3, 1 ])

tBQS3x2 = Parameter(name = 'tBQS3x2',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{tBQS3x2}',
                    lhablock = 'tBQS',
                    lhacode = [ 3, 2 ])

tBQS3x3 = Parameter(name = 'tBQS3x3',
                    nature = 'external',
                    type = 'real',
                    value = 0,
                    texname = '\\text{tBQS3x3}',
                    lhablock = 'tBQS',
                    lhacode = [ 3, 3 ])

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
               value = 120,
               texname = '\\text{MH}',
               lhablock = 'MASS',
               lhacode = [ 25 ])

MFM = Parameter(name = 'MFM',
                nature = 'external',
                type = 'real',
                value = 50,
                texname = '\\text{MFM}',
                lhablock = 'MASS',
                lhacode = [ 9000003 ])

MVM = Parameter(name = 'MVM',
                nature = 'external',
                type = 'real',
                value = 50.,
                texname = '\\text{MVM}',
                lhablock = 'MASS',
                lhacode = [ 9000002 ])

MVC = Parameter(name = 'MVC',
                nature = 'external',
                type = 'real',
                value = 500,
                texname = '\\text{MVC}',
                lhablock = 'MASS',
                lhacode = [ 9000005 ])

MSM = Parameter(name = 'MSM',
                nature = 'external',
                type = 'real',
                value = 50,
                texname = '\\text{MSM}',
                lhablock = 'MASS',
                lhacode = [ 9000001 ])

MSC = Parameter(name = 'MSC',
                nature = 'external',
                type = 'real',
                value = 1000,
                texname = '\\text{MSC}',
                lhablock = 'MASS',
                lhacode = [ 9000004 ])

tMSC = Parameter(name = 'tMSC',
                 nature = 'external',
                 type = 'real',
                 value = 1000,
                 texname = '\\text{tMSC}',
                 lhablock = 'MASS',
                 lhacode = [ 9000006 ])

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

WFM = Parameter(name = 'WFM',
                nature = 'external',
                type = 'real',
                value = 10,
                texname = '\\text{WFM}',
                lhablock = 'DECAY',
                lhacode = [ 9000003 ])

WVM = Parameter(name = 'WVM',
                nature = 'external',
                type = 'real',
                value = 10.,
                texname = '\\text{WVM}',
                lhablock = 'DECAY',
                lhacode = [ 9000002 ])

WVC = Parameter(name = 'WVC',
                nature = 'external',
                type = 'real',
                value = 10,
                texname = '\\text{WVC}',
                lhablock = 'DECAY',
                lhacode = [ 9000005 ])

WSM = Parameter(name = 'WSM',
                nature = 'external',
                type = 'real',
                value = 10.,
                texname = '\\text{WSM}',
                lhablock = 'DECAY',
                lhacode = [ 9000001 ])

WSC = Parameter(name = 'WSC',
                nature = 'external',
                type = 'real',
                value = 10.,
                texname = '\\text{WSC}',
                lhablock = 'DECAY',
                lhacode = [ 9000004 ])

tWSC = Parameter(name = 'tWSC',
                 nature = 'external',
                 type = 'real',
                 value = 10.,
                 texname = '\\text{tWSC}',
                 lhablock = 'DECAY',
                 lhacode = [ 9000006 ])

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
                texname = '\\mu ')


# This file was automatically created by FeynRules $Revision: 999 $
# Mathematica version: 8.0 for Linux x86 (64-bit) (February 23, 2011)
# Date: Tue 20 Mar 2012 12:29:51



from object_library import all_parameters, Parameter


from function_library import complexconjugate, re, im, csc, sec, acsc, asec

# This is a default parameter object representing 0.
ZERO = Parameter(name = 'ZERO',
                 nature = 'internal',
                 type = 'real',
                 value = '0.0',
                 texname = '0')

# User-defined parameters.
RRd1x3 = Parameter(name = 'RRd1x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.9862151,
                   texname = '\\text{RRd1x3}',
                   lhablock = 'DSQMIX',
                   lhacode = [ 1, 3 ])

RRd1x6 = Parameter(name = 'RRd1x6',
                   nature = 'external',
                   type = 'real',
                   value = 0.1654681,
                   texname = '\\text{RRd1x6}',
                   lhablock = 'DSQMIX',
                   lhacode = [ 1, 6 ])

RRd2x3 = Parameter(name = 'RRd2x3',
                   nature = 'external',
                   type = 'real',
                   value = -0.1654681,
                   texname = '\\text{RRd2x3}',
                   lhablock = 'DSQMIX',
                   lhacode = [ 2, 3 ])

RRd2x6 = Parameter(name = 'RRd2x6',
                   nature = 'external',
                   type = 'real',
                   value = 0.9862151,
                   texname = '\\text{RRd2x6}',
                   lhablock = 'DSQMIX',
                   lhacode = [ 2, 6 ])

RRd3x5 = Parameter(name = 'RRd3x5',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{RRd3x5}',
                   lhablock = 'DSQMIX',
                   lhacode = [ 3, 5 ])

RRd4x4 = Parameter(name = 'RRd4x4',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{RRd4x4}',
                   lhablock = 'DSQMIX',
                   lhacode = [ 4, 4 ])

RRd5x2 = Parameter(name = 'RRd5x2',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{RRd5x2}',
                   lhablock = 'DSQMIX',
                   lhacode = [ 5, 2 ])

RRd6x1 = Parameter(name = 'RRd6x1',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{RRd6x1}',
                   lhablock = 'DSQMIX',
                   lhacode = [ 6, 1 ])

alp = Parameter(name = 'alp',
                nature = 'external',
                type = 'real',
                value = -0.1075727,
                texname = '\\alpha',
                lhablock = 'FRALPHA',
                lhacode = [ 1 ])

RMUH = Parameter(name = 'RMUH',
                 nature = 'external',
                 type = 'real',
                 value = 464.2435,
                 texname = '\\text{RMUH}',
                 lhablock = 'HMIX',
                 lhacode = [ 1 ])

tb = Parameter(name = 'tb',
               nature = 'external',
               type = 'real',
               value = 9.749107,
               texname = 't_b',
               lhablock = 'HMIX',
               lhacode = [ 2 ])

MA2 = Parameter(name = 'MA2',
                nature = 'external',
                type = 'real',
                value = 325121.6,
                texname = 'm_A^2',
                lhablock = 'HMIX',
                lhacode = [ 4 ])

ICKM1x3 = Parameter(name = 'ICKM1x3',
                    nature = 'external',
                    type = 'real',
                    value = -0.003349042,
                    texname = '\\text{ICKM1x3}',
                    lhablock = 'IMVCKM',
                    lhacode = [ 1, 3 ])

ICKM2x1 = Parameter(name = 'ICKM2x1',
                    nature = 'external',
                    type = 'real',
                    value = -0.0001377152,
                    texname = '\\text{ICKM2x1}',
                    lhablock = 'IMVCKM',
                    lhacode = [ 2, 1 ])

ICKM2x2 = Parameter(name = 'ICKM2x2',
                    nature = 'external',
                    type = 'real',
                    value = -0.00003212913,
                    texname = '\\text{ICKM2x2}',
                    lhablock = 'IMVCKM',
                    lhacode = [ 2, 2 ])

ICKM3x1 = Parameter(name = 'ICKM3x1',
                    nature = 'external',
                    type = 'real',
                    value = -0.00325855,
                    texname = '\\text{ICKM3x1}',
                    lhablock = 'IMVCKM',
                    lhacode = [ 3, 1 ])

ICKM3x2 = Parameter(name = 'ICKM3x2',
                    nature = 'external',
                    type = 'real',
                    value = -0.0007602238,
                    texname = '\\text{ICKM3x2}',
                    lhablock = 'IMVCKM',
                    lhacode = [ 3, 2 ])

RmD21x1 = Parameter(name = 'RmD21x1',
                    nature = 'external',
                    type = 'real',
                    value = 354397.9,
                    texname = '\\text{RmD21x1}',
                    lhablock = 'MSD2',
                    lhacode = [ 1, 1 ])

RmD22x2 = Parameter(name = 'RmD22x2',
                    nature = 'external',
                    type = 'real',
                    value = 354392.9,
                    texname = '\\text{RmD22x2}',
                    lhablock = 'MSD2',
                    lhacode = [ 2, 2 ])

RmD23x3 = Parameter(name = 'RmD23x3',
                    nature = 'external',
                    type = 'real',
                    value = 346135.1,
                    texname = '\\text{RmD23x3}',
                    lhablock = 'MSD2',
                    lhacode = [ 3, 3 ])

RmE21x1 = Parameter(name = 'RmE21x1',
                    nature = 'external',
                    type = 'real',
                    value = 98195.12,
                    texname = '\\text{RmE21x1}',
                    lhablock = 'MSE2',
                    lhacode = [ 1, 1 ])

RmE22x2 = Parameter(name = 'RmE22x2',
                    nature = 'external',
                    type = 'real',
                    value = 98178.83,
                    texname = '\\text{RmE22x2}',
                    lhablock = 'MSE2',
                    lhacode = [ 2, 2 ])

RmE23x3 = Parameter(name = 'RmE23x3',
                    nature = 'external',
                    type = 'real',
                    value = 93612.93,
                    texname = '\\text{RmE23x3}',
                    lhablock = 'MSE2',
                    lhacode = [ 3, 3 ])

RmL21x1 = Parameter(name = 'RmL21x1',
                    nature = 'external',
                    type = 'real',
                    value = 117412.8,
                    texname = '\\text{RmL21x1}',
                    lhablock = 'MSL2',
                    lhacode = [ 1, 1 ])

RmL22x2 = Parameter(name = 'RmL22x2',
                    nature = 'external',
                    type = 'real',
                    value = 117404.7,
                    texname = '\\text{RmL22x2}',
                    lhablock = 'MSL2',
                    lhacode = [ 2, 2 ])

RmL23x3 = Parameter(name = 'RmL23x3',
                    nature = 'external',
                    type = 'real',
                    value = 115141.1,
                    texname = '\\text{RmL23x3}',
                    lhablock = 'MSL2',
                    lhacode = [ 3, 3 ])

RMx1 = Parameter(name = 'RMx1',
                 nature = 'external',
                 type = 'real',
                 value = 102.1024,
                 texname = '\\text{RMx1}',
                 lhablock = 'MSOFT',
                 lhacode = [ 1 ])

RMx2 = Parameter(name = 'RMx2',
                 nature = 'external',
                 type = 'real',
                 value = 192.6534,
                 texname = '\\text{RMx2}',
                 lhablock = 'MSOFT',
                 lhacode = [ 2 ])

RMx3 = Parameter(name = 'RMx3',
                 nature = 'external',
                 type = 'real',
                 value = 588.4243,
                 texname = '\\text{RMx3}',
                 lhablock = 'MSOFT',
                 lhacode = [ 3 ])

mHu2 = Parameter(name = 'mHu2',
                 nature = 'external',
                 type = 'real',
                 value = 102141.1,
                 texname = 'm_{H_u}^2',
                 lhablock = 'MSOFT',
                 lhacode = [ 21 ])

mHd2 = Parameter(name = 'mHd2',
                 nature = 'external',
                 type = 'real',
                 value = -215451.9,
                 texname = 'm_{H_d}^2',
                 lhablock = 'MSOFT',
                 lhacode = [ 22 ])

RmQ21x1 = Parameter(name = 'RmQ21x1',
                    nature = 'external',
                    type = 'real',
                    value = 375246.6,
                    texname = '\\text{RmQ21x1}',
                    lhablock = 'MSQ2',
                    lhacode = [ 1, 1 ])

RmQ22x2 = Parameter(name = 'RmQ22x2',
                    nature = 'external',
                    type = 'real',
                    value = 375241.8,
                    texname = '\\text{RmQ22x2}',
                    lhablock = 'MSQ2',
                    lhacode = [ 2, 2 ])

RmQ23x3 = Parameter(name = 'RmQ23x3',
                    nature = 'external',
                    type = 'real',
                    value = 269949.8,
                    texname = '\\text{RmQ23x3}',
                    lhablock = 'MSQ2',
                    lhacode = [ 3, 3 ])

RmU21x1 = Parameter(name = 'RmU21x1',
                    nature = 'external',
                    type = 'real',
                    value = 356488.1,
                    texname = '\\text{RmU21x1}',
                    lhablock = 'MSU2',
                    lhacode = [ 1, 1 ])

RmU22x2 = Parameter(name = 'RmU22x2',
                    nature = 'external',
                    type = 'real',
                    value = 356483.5,
                    texname = '\\text{RmU22x2}',
                    lhablock = 'MSU2',
                    lhacode = [ 2, 2 ])

RmU23x3 = Parameter(name = 'RmU23x3',
                    nature = 'external',
                    type = 'real',
                    value = 149411.4,
                    texname = '\\text{RmU23x3}',
                    lhablock = 'MSU2',
                    lhacode = [ 3, 3 ])

RNN1x1 = Parameter(name = 'RNN1x1',
                   nature = 'external',
                   type = 'real',
                   value = -0.9927936,
                   texname = '\\text{RNN1x1}',
                   lhablock = 'NMIX',
                   lhacode = [ 1, 1 ])

RNN1x2 = Parameter(name = 'RNN1x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.03701124,
                   texname = '\\text{RNN1x2}',
                   lhablock = 'NMIX',
                   lhacode = [ 1, 2 ])

RNN1x3 = Parameter(name = 'RNN1x3',
                   nature = 'external',
                   type = 'real',
                   value = -0.1087277,
                   texname = '\\text{RNN1x3}',
                   lhablock = 'NMIX',
                   lhacode = [ 1, 3 ])

RNN1x4 = Parameter(name = 'RNN1x4',
                   nature = 'external',
                   type = 'real',
                   value = 0.03419427,
                   texname = '\\text{RNN1x4}',
                   lhablock = 'NMIX',
                   lhacode = [ 1, 4 ])

RNN2x1 = Parameter(name = 'RNN2x1',
                   nature = 'external',
                   type = 'real',
                   value = -0.06145626,
                   texname = '\\text{RNN2x1}',
                   lhablock = 'NMIX',
                   lhacode = [ 2, 1 ])

RNN2x2 = Parameter(name = 'RNN2x2',
                   nature = 'external',
                   type = 'real',
                   value = -0.9731574,
                   texname = '\\text{RNN2x2}',
                   lhablock = 'NMIX',
                   lhacode = [ 2, 2 ])

RNN2x3 = Parameter(name = 'RNN2x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.1992759,
                   texname = '\\text{RNN2x3}',
                   lhablock = 'NMIX',
                   lhacode = [ 2, 3 ])

RNN2x4 = Parameter(name = 'RNN2x4',
                   nature = 'external',
                   type = 'real',
                   value = -0.09734984,
                   texname = '\\text{RNN2x4}',
                   lhablock = 'NMIX',
                   lhacode = [ 2, 4 ])

RNN3x1 = Parameter(name = 'RNN3x1',
                   nature = 'external',
                   type = 'real',
                   value = 0.04928328,
                   texname = '\\text{RNN3x1}',
                   lhablock = 'NMIX',
                   lhacode = [ 3, 1 ])

RNN3x2 = Parameter(name = 'RNN3x2',
                   nature = 'external',
                   type = 'real',
                   value = -0.07521552,
                   texname = '\\text{RNN3x2}',
                   lhablock = 'NMIX',
                   lhacode = [ 3, 2 ])

RNN3x3 = Parameter(name = 'RNN3x3',
                   nature = 'external',
                   type = 'real',
                   value = -0.6987909,
                   texname = '\\text{RNN3x3}',
                   lhablock = 'NMIX',
                   lhacode = [ 3, 3 ])

RNN3x4 = Parameter(name = 'RNN3x4',
                   nature = 'external',
                   type = 'real',
                   value = -0.7096514,
                   texname = '\\text{RNN3x4}',
                   lhablock = 'NMIX',
                   lhacode = [ 3, 4 ])

RNN4x1 = Parameter(name = 'RNN4x1',
                   nature = 'external',
                   type = 'real',
                   value = 0.09030553,
                   texname = '\\text{RNN4x1}',
                   lhablock = 'NMIX',
                   lhacode = [ 4, 1 ])

RNN4x2 = Parameter(name = 'RNN4x2',
                   nature = 'external',
                   type = 'real',
                   value = -0.2143305,
                   texname = '\\text{RNN4x2}',
                   lhablock = 'NMIX',
                   lhacode = [ 4, 2 ])

RNN4x3 = Parameter(name = 'RNN4x3',
                   nature = 'external',
                   type = 'real',
                   value = -0.6783499,
                   texname = '\\text{RNN4x3}',
                   lhablock = 'NMIX',
                   lhacode = [ 4, 3 ])

RNN4x4 = Parameter(name = 'RNN4x4',
                   nature = 'external',
                   type = 'real',
                   value = 0.6969568,
                   texname = '\\text{RNN4x4}',
                   lhablock = 'NMIX',
                   lhacode = [ 4, 4 ])

RLLE1x2x1 = Parameter(name = 'RLLE1x2x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RLLE1x2x1}',
                      lhablock = 'RVLAMLLE',
                      lhacode = [ 1, 2, 1 ])

RLLE1x2x2 = Parameter(name = 'RLLE1x2x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RLLE1x2x2}',
                      lhablock = 'RVLAMLLE',
                      lhacode = [ 1, 2, 2 ])

RLLE1x2x3 = Parameter(name = 'RLLE1x2x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RLLE1x2x3}',
                      lhablock = 'RVLAMLLE',
                      lhacode = [ 1, 2, 3 ])

RLLE1x3x1 = Parameter(name = 'RLLE1x3x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RLLE1x3x1}',
                      lhablock = 'RVLAMLLE',
                      lhacode = [ 1, 3, 1 ])

RLLE1x3x2 = Parameter(name = 'RLLE1x3x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RLLE1x3x2}',
                      lhablock = 'RVLAMLLE',
                      lhacode = [ 1, 3, 2 ])

RLLE1x3x3 = Parameter(name = 'RLLE1x3x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RLLE1x3x3}',
                      lhablock = 'RVLAMLLE',
                      lhacode = [ 1, 3, 3 ])

RLLE2x1x1 = Parameter(name = 'RLLE2x1x1',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RLLE2x1x1}',
                      lhablock = 'RVLAMLLE',
                      lhacode = [ 2, 1, 1 ])

RLLE2x1x2 = Parameter(name = 'RLLE2x1x2',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RLLE2x1x2}',
                      lhablock = 'RVLAMLLE',
                      lhacode = [ 2, 1, 2 ])

RLLE2x1x3 = Parameter(name = 'RLLE2x1x3',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RLLE2x1x3}',
                      lhablock = 'RVLAMLLE',
                      lhacode = [ 2, 1, 3 ])

RLLE2x3x1 = Parameter(name = 'RLLE2x3x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RLLE2x3x1}',
                      lhablock = 'RVLAMLLE',
                      lhacode = [ 2, 3, 1 ])

RLLE2x3x2 = Parameter(name = 'RLLE2x3x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RLLE2x3x2}',
                      lhablock = 'RVLAMLLE',
                      lhacode = [ 2, 3, 2 ])

RLLE2x3x3 = Parameter(name = 'RLLE2x3x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RLLE2x3x3}',
                      lhablock = 'RVLAMLLE',
                      lhacode = [ 2, 3, 3 ])

RLLE3x1x1 = Parameter(name = 'RLLE3x1x1',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RLLE3x1x1}',
                      lhablock = 'RVLAMLLE',
                      lhacode = [ 3, 1, 1 ])

RLLE3x1x2 = Parameter(name = 'RLLE3x1x2',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RLLE3x1x2}',
                      lhablock = 'RVLAMLLE',
                      lhacode = [ 3, 1, 2 ])

RLLE3x1x3 = Parameter(name = 'RLLE3x1x3',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RLLE3x1x3}',
                      lhablock = 'RVLAMLLE',
                      lhacode = [ 3, 1, 3 ])

RLLE3x2x1 = Parameter(name = 'RLLE3x2x1',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RLLE3x2x1}',
                      lhablock = 'RVLAMLLE',
                      lhacode = [ 3, 2, 1 ])

RLLE3x2x2 = Parameter(name = 'RLLE3x2x2',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RLLE3x2x2}',
                      lhablock = 'RVLAMLLE',
                      lhacode = [ 3, 2, 2 ])

RLLE3x2x3 = Parameter(name = 'RLLE3x2x3',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RLLE3x2x3}',
                      lhablock = 'RVLAMLLE',
                      lhacode = [ 3, 2, 3 ])

RLQD1x1x1 = Parameter(name = 'RLQD1x1x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD1x1x1}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 1, 1, 1 ])

RLQD1x1x2 = Parameter(name = 'RLQD1x1x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD1x1x2}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 1, 1, 2 ])

RLQD1x1x3 = Parameter(name = 'RLQD1x1x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD1x1x3}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 1, 1, 3 ])

RLQD1x2x1 = Parameter(name = 'RLQD1x2x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD1x2x1}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 1, 2, 1 ])

RLQD1x2x2 = Parameter(name = 'RLQD1x2x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD1x2x2}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 1, 2, 2 ])

RLQD1x2x3 = Parameter(name = 'RLQD1x2x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD1x2x3}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 1, 2, 3 ])

RLQD1x3x1 = Parameter(name = 'RLQD1x3x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD1x3x1}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 1, 3, 1 ])

RLQD1x3x2 = Parameter(name = 'RLQD1x3x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD1x3x2}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 1, 3, 2 ])

RLQD1x3x3 = Parameter(name = 'RLQD1x3x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD1x3x3}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 1, 3, 3 ])

RLQD2x1x1 = Parameter(name = 'RLQD2x1x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD2x1x1}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 2, 1, 1 ])

RLQD2x1x2 = Parameter(name = 'RLQD2x1x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD2x1x2}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 2, 1, 2 ])

RLQD2x1x3 = Parameter(name = 'RLQD2x1x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD2x1x3}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 2, 1, 3 ])

RLQD2x2x1 = Parameter(name = 'RLQD2x2x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD2x2x1}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 2, 2, 1 ])

RLQD2x2x2 = Parameter(name = 'RLQD2x2x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD2x2x2}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 2, 2, 2 ])

RLQD2x2x3 = Parameter(name = 'RLQD2x2x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD2x2x3}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 2, 2, 3 ])

RLQD2x3x1 = Parameter(name = 'RLQD2x3x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD2x3x1}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 2, 3, 1 ])

RLQD2x3x2 = Parameter(name = 'RLQD2x3x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD2x3x2}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 2, 3, 2 ])

RLQD2x3x3 = Parameter(name = 'RLQD2x3x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD2x3x3}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 2, 3, 3 ])

RLQD3x1x1 = Parameter(name = 'RLQD3x1x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD3x1x1}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 3, 1, 1 ])

RLQD3x1x2 = Parameter(name = 'RLQD3x1x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD3x1x2}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 3, 1, 2 ])

RLQD3x1x3 = Parameter(name = 'RLQD3x1x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD3x1x3}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 3, 1, 3 ])

RLQD3x2x1 = Parameter(name = 'RLQD3x2x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD3x2x1}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 3, 2, 1 ])

RLQD3x2x2 = Parameter(name = 'RLQD3x2x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD3x2x2}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 3, 2, 2 ])

RLQD3x2x3 = Parameter(name = 'RLQD3x2x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD3x2x3}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 3, 2, 3 ])

RLQD3x3x1 = Parameter(name = 'RLQD3x3x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD3x3x1}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 3, 3, 1 ])

RLQD3x3x2 = Parameter(name = 'RLQD3x3x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD3x3x2}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 3, 3, 2 ])

RLQD3x3x3 = Parameter(name = 'RLQD3x3x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RLQD3x3x3}',
                      lhablock = 'RVLAMLQD',
                      lhacode = [ 3, 3, 3 ])

RLDD1x1x2 = Parameter(name = 'RLDD1x1x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RLDD1x1x2}',
                      lhablock = 'RVLAMUDD',
                      lhacode = [ 1, 1, 2 ])

RLDD1x1x3 = Parameter(name = 'RLDD1x1x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RLDD1x1x3}',
                      lhablock = 'RVLAMUDD',
                      lhacode = [ 1, 1, 3 ])

RLDD1x2x1 = Parameter(name = 'RLDD1x2x1',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RLDD1x2x1}',
                      lhablock = 'RVLAMUDD',
                      lhacode = [ 1, 2, 1 ])

RLDD1x2x3 = Parameter(name = 'RLDD1x2x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RLDD1x2x3}',
                      lhablock = 'RVLAMUDD',
                      lhacode = [ 1, 2, 3 ])

RLDD1x3x1 = Parameter(name = 'RLDD1x3x1',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RLDD1x3x1}',
                      lhablock = 'RVLAMUDD',
                      lhacode = [ 1, 3, 1 ])

RLDD1x3x2 = Parameter(name = 'RLDD1x3x2',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RLDD1x3x2}',
                      lhablock = 'RVLAMUDD',
                      lhacode = [ 1, 3, 2 ])

RLDD2x1x2 = Parameter(name = 'RLDD2x1x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RLDD2x1x2}',
                      lhablock = 'RVLAMUDD',
                      lhacode = [ 2, 1, 2 ])

RLDD2x1x3 = Parameter(name = 'RLDD2x1x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RLDD2x1x3}',
                      lhablock = 'RVLAMUDD',
                      lhacode = [ 2, 1, 3 ])

RLDD2x2x1 = Parameter(name = 'RLDD2x2x1',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RLDD2x2x1}',
                      lhablock = 'RVLAMUDD',
                      lhacode = [ 2, 2, 1 ])

RLDD2x2x3 = Parameter(name = 'RLDD2x2x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RLDD2x2x3}',
                      lhablock = 'RVLAMUDD',
                      lhacode = [ 2, 2, 3 ])

RLDD2x3x1 = Parameter(name = 'RLDD2x3x1',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RLDD2x3x1}',
                      lhablock = 'RVLAMUDD',
                      lhacode = [ 2, 3, 1 ])

RLDD2x3x2 = Parameter(name = 'RLDD2x3x2',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RLDD2x3x2}',
                      lhablock = 'RVLAMUDD',
                      lhacode = [ 2, 3, 2 ])

RLDD3x1x2 = Parameter(name = 'RLDD3x1x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RLDD3x1x2}',
                      lhablock = 'RVLAMUDD',
                      lhacode = [ 3, 1, 2 ])

RLDD3x1x3 = Parameter(name = 'RLDD3x1x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RLDD3x1x3}',
                      lhablock = 'RVLAMUDD',
                      lhacode = [ 3, 1, 3 ])

RLDD3x2x1 = Parameter(name = 'RLDD3x2x1',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RLDD3x2x1}',
                      lhablock = 'RVLAMUDD',
                      lhacode = [ 3, 2, 1 ])

RLDD3x2x3 = Parameter(name = 'RLDD3x2x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RLDD3x2x3}',
                      lhablock = 'RVLAMUDD',
                      lhacode = [ 3, 2, 3 ])

RLDD3x3x1 = Parameter(name = 'RLDD3x3x1',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RLDD3x3x1}',
                      lhablock = 'RVLAMUDD',
                      lhacode = [ 3, 3, 1 ])

RLDD3x3x2 = Parameter(name = 'RLDD3x3x2',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RLDD3x3x2}',
                      lhablock = 'RVLAMUDD',
                      lhacode = [ 3, 3, 2 ])

RTLE1x2x1 = Parameter(name = 'RTLE1x2x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RTLE1x2x1}',
                      lhablock = 'RVTLLE',
                      lhacode = [ 1, 2, 1 ])

RTLE1x2x2 = Parameter(name = 'RTLE1x2x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RTLE1x2x2}',
                      lhablock = 'RVTLLE',
                      lhacode = [ 1, 2, 2 ])

RTLE1x2x3 = Parameter(name = 'RTLE1x2x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RTLE1x2x3}',
                      lhablock = 'RVTLLE',
                      lhacode = [ 1, 2, 3 ])

RTLE1x3x1 = Parameter(name = 'RTLE1x3x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RTLE1x3x1}',
                      lhablock = 'RVTLLE',
                      lhacode = [ 1, 3, 1 ])

RTLE1x3x2 = Parameter(name = 'RTLE1x3x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RTLE1x3x2}',
                      lhablock = 'RVTLLE',
                      lhacode = [ 1, 3, 2 ])

RTLE1x3x3 = Parameter(name = 'RTLE1x3x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RTLE1x3x3}',
                      lhablock = 'RVTLLE',
                      lhacode = [ 1, 3, 3 ])

RTLE2x1x1 = Parameter(name = 'RTLE2x1x1',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RTLE2x1x1}',
                      lhablock = 'RVTLLE',
                      lhacode = [ 2, 1, 1 ])

RTLE2x1x2 = Parameter(name = 'RTLE2x1x2',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RTLE2x1x2}',
                      lhablock = 'RVTLLE',
                      lhacode = [ 2, 1, 2 ])

RTLE2x1x3 = Parameter(name = 'RTLE2x1x3',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RTLE2x1x3}',
                      lhablock = 'RVTLLE',
                      lhacode = [ 2, 1, 3 ])

RTLE2x3x1 = Parameter(name = 'RTLE2x3x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RTLE2x3x1}',
                      lhablock = 'RVTLLE',
                      lhacode = [ 2, 3, 1 ])

RTLE2x3x2 = Parameter(name = 'RTLE2x3x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RTLE2x3x2}',
                      lhablock = 'RVTLLE',
                      lhacode = [ 2, 3, 2 ])

RTLE2x3x3 = Parameter(name = 'RTLE2x3x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RTLE2x3x3}',
                      lhablock = 'RVTLLE',
                      lhacode = [ 2, 3, 3 ])

RTLE3x1x1 = Parameter(name = 'RTLE3x1x1',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RTLE3x1x1}',
                      lhablock = 'RVTLLE',
                      lhacode = [ 3, 1, 1 ])

RTLE3x1x2 = Parameter(name = 'RTLE3x1x2',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RTLE3x1x2}',
                      lhablock = 'RVTLLE',
                      lhacode = [ 3, 1, 2 ])

RTLE3x1x3 = Parameter(name = 'RTLE3x1x3',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RTLE3x1x3}',
                      lhablock = 'RVTLLE',
                      lhacode = [ 3, 1, 3 ])

RTLE3x2x1 = Parameter(name = 'RTLE3x2x1',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RTLE3x2x1}',
                      lhablock = 'RVTLLE',
                      lhacode = [ 3, 2, 1 ])

RTLE3x2x2 = Parameter(name = 'RTLE3x2x2',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RTLE3x2x2}',
                      lhablock = 'RVTLLE',
                      lhacode = [ 3, 2, 2 ])

RTLE3x2x3 = Parameter(name = 'RTLE3x2x3',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RTLE3x2x3}',
                      lhablock = 'RVTLLE',
                      lhacode = [ 3, 2, 3 ])

RTQD1x1x1 = Parameter(name = 'RTQD1x1x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD1x1x1}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 1, 1, 1 ])

RTQD1x1x2 = Parameter(name = 'RTQD1x1x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD1x1x2}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 1, 1, 2 ])

RTQD1x1x3 = Parameter(name = 'RTQD1x1x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD1x1x3}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 1, 1, 3 ])

RTQD1x2x1 = Parameter(name = 'RTQD1x2x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD1x2x1}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 1, 2, 1 ])

RTQD1x2x2 = Parameter(name = 'RTQD1x2x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD1x2x2}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 1, 2, 2 ])

RTQD1x2x3 = Parameter(name = 'RTQD1x2x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD1x2x3}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 1, 2, 3 ])

RTQD1x3x1 = Parameter(name = 'RTQD1x3x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD1x3x1}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 1, 3, 1 ])

RTQD1x3x2 = Parameter(name = 'RTQD1x3x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD1x3x2}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 1, 3, 2 ])

RTQD1x3x3 = Parameter(name = 'RTQD1x3x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD1x3x3}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 1, 3, 3 ])

RTQD2x1x1 = Parameter(name = 'RTQD2x1x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD2x1x1}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 2, 1, 1 ])

RTQD2x1x2 = Parameter(name = 'RTQD2x1x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD2x1x2}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 2, 1, 2 ])

RTQD2x1x3 = Parameter(name = 'RTQD2x1x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD2x1x3}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 2, 1, 3 ])

RTQD2x2x1 = Parameter(name = 'RTQD2x2x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD2x2x1}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 2, 2, 1 ])

RTQD2x2x2 = Parameter(name = 'RTQD2x2x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD2x2x2}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 2, 2, 2 ])

RTQD2x2x3 = Parameter(name = 'RTQD2x2x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD2x2x3}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 2, 2, 3 ])

RTQD2x3x1 = Parameter(name = 'RTQD2x3x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD2x3x1}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 2, 3, 1 ])

RTQD2x3x2 = Parameter(name = 'RTQD2x3x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD2x3x2}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 2, 3, 2 ])

RTQD2x3x3 = Parameter(name = 'RTQD2x3x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD2x3x3}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 2, 3, 3 ])

RTQD3x1x1 = Parameter(name = 'RTQD3x1x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD3x1x1}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 3, 1, 1 ])

RTQD3x1x2 = Parameter(name = 'RTQD3x1x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD3x1x2}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 3, 1, 2 ])

RTQD3x1x3 = Parameter(name = 'RTQD3x1x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD3x1x3}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 3, 1, 3 ])

RTQD3x2x1 = Parameter(name = 'RTQD3x2x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD3x2x1}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 3, 2, 1 ])

RTQD3x2x2 = Parameter(name = 'RTQD3x2x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD3x2x2}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 3, 2, 2 ])

RTQD3x2x3 = Parameter(name = 'RTQD3x2x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD3x2x3}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 3, 2, 3 ])

RTQD3x3x1 = Parameter(name = 'RTQD3x3x1',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD3x3x1}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 3, 3, 1 ])

RTQD3x3x2 = Parameter(name = 'RTQD3x3x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD3x3x2}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 3, 3, 2 ])

RTQD3x3x3 = Parameter(name = 'RTQD3x3x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.1,
                      texname = '\\text{RTQD3x3x3}',
                      lhablock = 'RVTLQD',
                      lhacode = [ 3, 3, 3 ])

RTDD1x1x2 = Parameter(name = 'RTDD1x1x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RTDD1x1x2}',
                      lhablock = 'RVTUDD',
                      lhacode = [ 1, 1, 2 ])

RTDD1x1x3 = Parameter(name = 'RTDD1x1x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RTDD1x1x3}',
                      lhablock = 'RVTUDD',
                      lhacode = [ 1, 1, 3 ])

RTDD1x2x1 = Parameter(name = 'RTDD1x2x1',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RTDD1x2x1}',
                      lhablock = 'RVTUDD',
                      lhacode = [ 1, 2, 1 ])

RTDD1x2x3 = Parameter(name = 'RTDD1x2x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RTDD1x2x3}',
                      lhablock = 'RVTUDD',
                      lhacode = [ 1, 2, 3 ])

RTDD1x3x1 = Parameter(name = 'RTDD1x3x1',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RTDD1x3x1}',
                      lhablock = 'RVTUDD',
                      lhacode = [ 1, 3, 1 ])

RTDD1x3x2 = Parameter(name = 'RTDD1x3x2',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RTDD1x3x2}',
                      lhablock = 'RVTUDD',
                      lhacode = [ 1, 3, 2 ])

RTDD2x1x2 = Parameter(name = 'RTDD2x1x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RTDD2x1x2}',
                      lhablock = 'RVTUDD',
                      lhacode = [ 2, 1, 2 ])

RTDD2x1x3 = Parameter(name = 'RTDD2x1x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RTDD2x1x3}',
                      lhablock = 'RVTUDD',
                      lhacode = [ 2, 1, 3 ])

RTDD2x2x1 = Parameter(name = 'RTDD2x2x1',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RTDD2x2x1}',
                      lhablock = 'RVTUDD',
                      lhacode = [ 2, 2, 1 ])

RTDD2x2x3 = Parameter(name = 'RTDD2x2x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RTDD2x2x3}',
                      lhablock = 'RVTUDD',
                      lhacode = [ 2, 2, 3 ])

RTDD2x3x1 = Parameter(name = 'RTDD2x3x1',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RTDD2x3x1}',
                      lhablock = 'RVTUDD',
                      lhacode = [ 2, 3, 1 ])

RTDD2x3x2 = Parameter(name = 'RTDD2x3x2',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RTDD2x3x2}',
                      lhablock = 'RVTUDD',
                      lhacode = [ 2, 3, 2 ])

RTDD3x1x2 = Parameter(name = 'RTDD3x1x2',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RTDD3x1x2}',
                      lhablock = 'RVTUDD',
                      lhacode = [ 3, 1, 2 ])

RTDD3x1x3 = Parameter(name = 'RTDD3x1x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RTDD3x1x3}',
                      lhablock = 'RVTUDD',
                      lhacode = [ 3, 1, 3 ])

RTDD3x2x1 = Parameter(name = 'RTDD3x2x1',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RTDD3x2x1}',
                      lhablock = 'RVTUDD',
                      lhacode = [ 3, 2, 1 ])

RTDD3x2x3 = Parameter(name = 'RTDD3x2x3',
                      nature = 'external',
                      type = 'real',
                      value = 0.2,
                      texname = '\\text{RTDD3x2x3}',
                      lhablock = 'RVTUDD',
                      lhacode = [ 3, 2, 3 ])

RTDD3x3x1 = Parameter(name = 'RTDD3x3x1',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RTDD3x3x1}',
                      lhablock = 'RVTUDD',
                      lhacode = [ 3, 3, 1 ])

RTDD3x3x2 = Parameter(name = 'RTDD3x3x2',
                      nature = 'external',
                      type = 'real',
                      value = -0.2,
                      texname = '\\text{RTDD3x3x2}',
                      lhablock = 'RVTUDD',
                      lhacode = [ 3, 3, 2 ])

RRl1x3 = Parameter(name = 'RRl1x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.3474325,
                   texname = '\\text{RRl1x3}',
                   lhablock = 'SELMIX',
                   lhacode = [ 1, 3 ])

RRl1x6 = Parameter(name = 'RRl1x6',
                   nature = 'external',
                   type = 'real',
                   value = 0.937705,
                   texname = '\\text{RRl1x6}',
                   lhablock = 'SELMIX',
                   lhacode = [ 1, 6 ])

RRl2x5 = Parameter(name = 'RRl2x5',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{RRl2x5}',
                   lhablock = 'SELMIX',
                   lhacode = [ 2, 5 ])

RRl3x4 = Parameter(name = 'RRl3x4',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{RRl3x4}',
                   lhablock = 'SELMIX',
                   lhacode = [ 3, 4 ])

RRl4x1 = Parameter(name = 'RRl4x1',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{RRl4x1}',
                   lhablock = 'SELMIX',
                   lhacode = [ 4, 1 ])

RRl5x2 = Parameter(name = 'RRl5x2',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{RRl5x2}',
                   lhablock = 'SELMIX',
                   lhacode = [ 5, 2 ])

RRl6x3 = Parameter(name = 'RRl6x3',
                   nature = 'external',
                   type = 'real',
                   value = -0.937705,
                   texname = '\\text{RRl6x3}',
                   lhablock = 'SELMIX',
                   lhacode = [ 6, 3 ])

RRl6x6 = Parameter(name = 'RRl6x6',
                   nature = 'external',
                   type = 'real',
                   value = 0.3474325,
                   texname = '\\text{RRl6x6}',
                   lhablock = 'SELMIX',
                   lhacode = [ 6, 6 ])

aEWM1 = Parameter(name = 'aEWM1',
                  nature = 'external',
                  type = 'real',
                  value = 127.934,
                  texname = '\\alpha _w^{-1}',
                  lhablock = 'SMINPUTS',
                  lhacode = [ 1 ])

aS = Parameter(name = 'aS',
               nature = 'external',
               type = 'real',
               value = 0.1176,
               texname = '\\alpha _s',
               lhablock = 'SMINPUTS',
               lhacode = [ 3 ])

RRn1x3 = Parameter(name = 'RRn1x3',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{RRn1x3}',
                   lhablock = 'SNUMIX',
                   lhacode = [ 1, 3 ])

RRn2x2 = Parameter(name = 'RRn2x2',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{RRn2x2}',
                   lhablock = 'SNUMIX',
                   lhacode = [ 2, 2 ])

RRn3x1 = Parameter(name = 'RRn3x1',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{RRn3x1}',
                   lhablock = 'SNUMIX',
                   lhacode = [ 3, 1 ])

Rtd1x1 = Parameter(name = 'Rtd1x1',
                   nature = 'external',
                   type = 'real',
                   value = -0.2551888,
                   texname = '\\text{Rtd1x1}',
                   lhablock = 'TD',
                   lhacode = [ 1, 1 ])

Rtd2x2 = Parameter(name = 'Rtd2x2',
                   nature = 'external',
                   type = 'real',
                   value = -4.374642,
                   texname = '\\text{Rtd2x2}',
                   lhablock = 'TD',
                   lhacode = [ 2, 2 ])

Rtd3x3 = Parameter(name = 'Rtd3x3',
                   nature = 'external',
                   type = 'real',
                   value = -167.718,
                   texname = '\\text{Rtd3x3}',
                   lhablock = 'TD',
                   lhacode = [ 3, 3 ])

Rte1x1 = Parameter(name = 'Rte1x1',
                   nature = 'external',
                   type = 'real',
                   value = -0.02155743,
                   texname = '\\text{Rte1x1}',
                   lhablock = 'TE',
                   lhacode = [ 1, 1 ])

Rte2x2 = Parameter(name = 'Rte2x2',
                   nature = 'external',
                   type = 'real',
                   value = -4.457265,
                   texname = '\\text{Rte2x2}',
                   lhablock = 'TE',
                   lhacode = [ 2, 2 ])

Rte3x3 = Parameter(name = 'Rte3x3',
                   nature = 'external',
                   type = 'real',
                   value = -74.36274,
                   texname = '\\text{Rte3x3}',
                   lhablock = 'TE',
                   lhacode = [ 3, 3 ])

Rtu1x1 = Parameter(name = 'Rtu1x1',
                   nature = 'external',
                   type = 'real',
                   value = -0.008900702,
                   texname = '\\text{Rtu1x1}',
                   lhablock = 'TU',
                   lhacode = [ 1, 1 ])

Rtu2x2 = Parameter(name = 'Rtu2x2',
                   nature = 'external',
                   type = 'real',
                   value = -3.560256,
                   texname = '\\text{Rtu2x2}',
                   lhablock = 'TU',
                   lhacode = [ 2, 2 ])

Rtu3x3 = Parameter(name = 'Rtu3x3',
                   nature = 'external',
                   type = 'real',
                   value = -588.7617,
                   texname = '\\text{Rtu3x3}',
                   lhablock = 'TU',
                   lhacode = [ 3, 3 ])

RUU1x1 = Parameter(name = 'RUU1x1',
                   nature = 'external',
                   type = 'real',
                   value = -0.9576948,
                   texname = '\\text{RUU1x1}',
                   lhablock = 'UMIX',
                   lhacode = [ 1, 1 ])

RUU1x2 = Parameter(name = 'RUU1x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.2877859,
                   texname = '\\text{RUU1x2}',
                   lhablock = 'UMIX',
                   lhacode = [ 1, 2 ])

RUU2x1 = Parameter(name = 'RUU2x1',
                   nature = 'external',
                   type = 'real',
                   value = 0.2877859,
                   texname = '\\text{RUU2x1}',
                   lhablock = 'UMIX',
                   lhacode = [ 2, 1 ])

RUU2x2 = Parameter(name = 'RUU2x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.9576948,
                   texname = '\\text{RUU2x2}',
                   lhablock = 'UMIX',
                   lhacode = [ 2, 2 ])

RMNS1x1 = Parameter(name = 'RMNS1x1',
                    nature = 'external',
                    type = 'real',
                    value = 1.,
                    texname = '\\text{RMNS1x1}',
                    lhablock = 'UPMNS',
                    lhacode = [ 1, 1 ])

RMNS2x2 = Parameter(name = 'RMNS2x2',
                    nature = 'external',
                    type = 'real',
                    value = 1.,
                    texname = '\\text{RMNS2x2}',
                    lhablock = 'UPMNS',
                    lhacode = [ 2, 2 ])

RMNS3x3 = Parameter(name = 'RMNS3x3',
                    nature = 'external',
                    type = 'real',
                    value = 1.,
                    texname = '\\text{RMNS3x3}',
                    lhablock = 'UPMNS',
                    lhacode = [ 3, 3 ])

RRu1x3 = Parameter(name = 'RRu1x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.5111614,
                   texname = '\\text{RRu1x3}',
                   lhablock = 'USQMIX',
                   lhacode = [ 1, 3 ])

RRu1x6 = Parameter(name = 'RRu1x6',
                   nature = 'external',
                   type = 'real',
                   value = 0.8594848,
                   texname = '\\text{RRu1x6}',
                   lhablock = 'USQMIX',
                   lhacode = [ 1, 6 ])

RRu2x3 = Parameter(name = 'RRu2x3',
                   nature = 'external',
                   type = 'real',
                   value = -0.8594848,
                   texname = '\\text{RRu2x3}',
                   lhablock = 'USQMIX',
                   lhacode = [ 2, 3 ])

RRu2x6 = Parameter(name = 'RRu2x6',
                   nature = 'external',
                   type = 'real',
                   value = 0.5111614,
                   texname = '\\text{RRu2x6}',
                   lhablock = 'USQMIX',
                   lhacode = [ 2, 6 ])

RRu3x5 = Parameter(name = 'RRu3x5',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{RRu3x5}',
                   lhablock = 'USQMIX',
                   lhacode = [ 3, 5 ])

RRu4x4 = Parameter(name = 'RRu4x4',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{RRu4x4}',
                   lhablock = 'USQMIX',
                   lhacode = [ 4, 4 ])

RRu5x1 = Parameter(name = 'RRu5x1',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{RRu5x1}',
                   lhablock = 'USQMIX',
                   lhacode = [ 5, 1 ])

RRu6x2 = Parameter(name = 'RRu6x2',
                   nature = 'external',
                   type = 'real',
                   value = 1.,
                   texname = '\\text{RRu6x2}',
                   lhablock = 'USQMIX',
                   lhacode = [ 6, 2 ])

RCKM1x1 = Parameter(name = 'RCKM1x1',
                    nature = 'external',
                    type = 'real',
                    value = 0.9738404,
                    texname = '\\text{RCKM1x1}',
                    lhablock = 'VCKM',
                    lhacode = [ 1, 1 ])

RCKM1x2 = Parameter(name = 'RCKM1x2',
                    nature = 'external',
                    type = 'real',
                    value = 0.2271982,
                    texname = '\\text{RCKM1x2}',
                    lhablock = 'VCKM',
                    lhacode = [ 1, 2 ])

RCKM1x3 = Parameter(name = 'RCKM1x3',
                    nature = 'external',
                    type = 'real',
                    value = 0.002173989,
                    texname = '\\text{RCKM1x3}',
                    lhablock = 'VCKM',
                    lhacode = [ 1, 3 ])

RCKM2x1 = Parameter(name = 'RCKM2x1',
                    nature = 'external',
                    type = 'real',
                    value = -0.2270868,
                    texname = '\\text{RCKM2x1}',
                    lhablock = 'VCKM',
                    lhacode = [ 2, 1 ])

RCKM2x2 = Parameter(name = 'RCKM2x2',
                    nature = 'external',
                    type = 'real',
                    value = 0.9729587,
                    texname = '\\text{RCKM2x2}',
                    lhablock = 'VCKM',
                    lhacode = [ 2, 2 ])

RCKM2x3 = Parameter(name = 'RCKM2x3',
                    nature = 'external',
                    type = 'real',
                    value = 0.04222469,
                    texname = '\\text{RCKM2x3}',
                    lhablock = 'VCKM',
                    lhacode = [ 2, 3 ])

RCKM3x1 = Parameter(name = 'RCKM3x1',
                    nature = 'external',
                    type = 'real',
                    value = 0.007478279,
                    texname = '\\text{RCKM3x1}',
                    lhablock = 'VCKM',
                    lhacode = [ 3, 1 ])

RCKM3x2 = Parameter(name = 'RCKM3x2',
                    nature = 'external',
                    type = 'real',
                    value = -0.04161426,
                    texname = '\\text{RCKM3x2}',
                    lhablock = 'VCKM',
                    lhacode = [ 3, 2 ])

RCKM3x3 = Parameter(name = 'RCKM3x3',
                    nature = 'external',
                    type = 'real',
                    value = 0.9991002,
                    texname = '\\text{RCKM3x3}',
                    lhablock = 'VCKM',
                    lhacode = [ 3, 3 ])

RVV1x1 = Parameter(name = 'RVV1x1',
                   nature = 'external',
                   type = 'real',
                   value = -0.9899065,
                   texname = '\\text{RVV1x1}',
                   lhablock = 'VMIX',
                   lhacode = [ 1, 1 ])

RVV1x2 = Parameter(name = 'RVV1x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.1417221,
                   texname = '\\text{RVV1x2}',
                   lhablock = 'VMIX',
                   lhacode = [ 1, 2 ])

RVV2x1 = Parameter(name = 'RVV2x1',
                   nature = 'external',
                   type = 'real',
                   value = 0.1417221,
                   texname = '\\text{RVV2x1}',
                   lhablock = 'VMIX',
                   lhacode = [ 2, 1 ])

RVV2x2 = Parameter(name = 'RVV2x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.9899065,
                   texname = '\\text{RVV2x2}',
                   lhablock = 'VMIX',
                   lhacode = [ 2, 2 ])

Ryd1x1 = Parameter(name = 'Ryd1x1',
                   nature = 'external',
                   type = 'real',
                   value = 0.0001884671,
                   texname = '\\text{Ryd1x1}',
                   lhablock = 'YD',
                   lhacode = [ 1, 1 ])

Ryd2x2 = Parameter(name = 'Ryd2x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.003230863,
                   texname = '\\text{Ryd2x2}',
                   lhablock = 'YD',
                   lhacode = [ 2, 2 ])

Ryd3x3 = Parameter(name = 'Ryd3x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.1360783,
                   texname = '\\text{Ryd3x3}',
                   lhablock = 'YD',
                   lhacode = [ 3, 3 ])

Rye1x1 = Parameter(name = 'Rye1x1',
                   nature = 'external',
                   type = 'real',
                   value = 0.00002882278,
                   texname = '\\text{Rye1x1}',
                   lhablock = 'YE',
                   lhacode = [ 1, 1 ])

Rye2x2 = Parameter(name = 'Rye2x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.005959638,
                   texname = '\\text{Rye2x2}',
                   lhablock = 'YE',
                   lhacode = [ 2, 2 ])

Rye3x3 = Parameter(name = 'Rye3x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.1002358,
                   texname = '\\text{Rye3x3}',
                   lhablock = 'YE',
                   lhacode = [ 3, 3 ])

Ryu1x1 = Parameter(name = 'Ryu1x1',
                   nature = 'external',
                   type = 'real',
                   value = 8.789391e-6,
                   texname = '\\text{Ryu1x1}',
                   lhablock = 'YU',
                   lhacode = [ 1, 1 ])

Ryu2x2 = Parameter(name = 'Ryu2x2',
                   nature = 'external',
                   type = 'real',
                   value = 0.003515756,
                   texname = '\\text{Ryu2x2}',
                   lhablock = 'YU',
                   lhacode = [ 2, 2 ])

Ryu3x3 = Parameter(name = 'Ryu3x3',
                   nature = 'external',
                   type = 'real',
                   value = 0.8829349,
                   texname = '\\text{Ryu3x3}',
                   lhablock = 'YU',
                   lhacode = [ 3, 3 ])

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
               value = 79.829013114276,
               texname = '\\text{MW}',
               lhablock = 'MASS',
               lhacode = [ 24 ])

Mneu1 = Parameter(name = 'Mneu1',
                  nature = 'external',
                  type = 'real',
                  value = 99.48601,
                  texname = '\\text{Mneu1}',
                  lhablock = 'MASS',
                  lhacode = [ 1000022 ])

Mneu2 = Parameter(name = 'Mneu2',
                  nature = 'external',
                  type = 'real',
                  value = 189.5047,
                  texname = '\\text{Mneu2}',
                  lhablock = 'MASS',
                  lhacode = [ 1000023 ])

Mneu3 = Parameter(name = 'Mneu3',
                  nature = 'external',
                  type = 'real',
                  value = -469.5446,
                  texname = '\\text{Mneu3}',
                  lhablock = 'MASS',
                  lhacode = [ 1000025 ])

Mneu4 = Parameter(name = 'Mneu4',
                  nature = 'external',
                  type = 'real',
                  value = 477.3438,
                  texname = '\\text{Mneu4}',
                  lhablock = 'MASS',
                  lhacode = [ 1000035 ])

Mch1 = Parameter(name = 'Mch1',
                 nature = 'external',
                 type = 'real',
                 value = 189.468,
                 texname = '\\text{Mch1}',
                 lhablock = 'MASS',
                 lhacode = [ 1000024 ])

Mch2 = Parameter(name = 'Mch2',
                 nature = 'external',
                 type = 'real',
                 value = 480.5668,
                 texname = '\\text{Mch2}',
                 lhablock = 'MASS',
                 lhacode = [ 1000037 ])

Mgo = Parameter(name = 'Mgo',
                nature = 'external',
                type = 'real',
                value = 616.1578,
                texname = '\\text{Mgo}',
                lhablock = 'MASS',
                lhacode = [ 1000021 ])

MH01 = Parameter(name = 'MH01',
                 nature = 'external',
                 type = 'real',
                 value = 113.9668,
                 texname = '\\text{MH01}',
                 lhablock = 'MASS',
                 lhacode = [ 25 ])

MH02 = Parameter(name = 'MH02',
                 nature = 'external',
                 type = 'real',
                 value = 564.3097,
                 texname = '\\text{MH02}',
                 lhablock = 'MASS',
                 lhacode = [ 35 ])

MA0 = Parameter(name = 'MA0',
                nature = 'external',
                type = 'real',
                value = 564.6897,
                texname = '\\text{MA0}',
                lhablock = 'MASS',
                lhacode = [ 36 ])

MH = Parameter(name = 'MH',
               nature = 'external',
               type = 'real',
               value = 570.4586,
               texname = '\\text{MH}',
               lhablock = 'MASS',
               lhacode = [ 37 ])

Mta = Parameter(name = 'Mta',
                nature = 'external',
                type = 'real',
                value = 1.77699,
                texname = '\\text{Mta}',
                lhablock = 'MASS',
                lhacode = [ 15 ])

MT = Parameter(name = 'MT',
               nature = 'external',
               type = 'real',
               value = 173.1,
               texname = '\\text{MT}',
               lhablock = 'MASS',
               lhacode = [ 6 ])

MB = Parameter(name = 'MB',
               nature = 'external',
               type = 'real',
               value = 4.83238808,
               texname = '\\text{MB}',
               lhablock = 'MASS',
               lhacode = [ 5 ])

Msn1 = Parameter(name = 'Msn1',
                 nature = 'external',
                 type = 'real',
                 value = 333.6805,
                 texname = '\\text{Msn1}',
                 lhablock = 'MASS',
                 lhacode = [ 1000012 ])

Msn2 = Parameter(name = 'Msn2',
                 nature = 'external',
                 type = 'real',
                 value = 337.0975,
                 texname = '\\text{Msn2}',
                 lhablock = 'MASS',
                 lhacode = [ 1000014 ])

Msn3 = Parameter(name = 'Msn3',
                 nature = 'external',
                 type = 'real',
                 value = 337.1096,
                 texname = '\\text{Msn3}',
                 lhablock = 'MASS',
                 lhacode = [ 1000016 ])

Msl1 = Parameter(name = 'Msl1',
                 nature = 'external',
                 type = 'real',
                 value = 303.753,
                 texname = '\\text{Msl1}',
                 lhablock = 'MASS',
                 lhacode = [ 1000011 ])

Msl2 = Parameter(name = 'Msl2',
                 nature = 'external',
                 type = 'real',
                 value = 316.8669,
                 texname = '\\text{Msl2}',
                 lhablock = 'MASS',
                 lhacode = [ 1000013 ])

Msl3 = Parameter(name = 'Msl3',
                 nature = 'external',
                 type = 'real',
                 value = 316.9185,
                 texname = '\\text{Msl3}',
                 lhablock = 'MASS',
                 lhacode = [ 1000015 ])

Msl4 = Parameter(name = 'Msl4',
                 nature = 'external',
                 type = 'real',
                 value = 346.4667,
                 texname = '\\text{Msl4}',
                 lhablock = 'MASS',
                 lhacode = [ 2000011 ])

Msl5 = Parameter(name = 'Msl5',
                 nature = 'external',
                 type = 'real',
                 value = 346.478,
                 texname = '\\text{Msl5}',
                 lhablock = 'MASS',
                 lhacode = [ 2000013 ])

Msl6 = Parameter(name = 'Msl6',
                 nature = 'external',
                 type = 'real',
                 value = 348.1667,
                 texname = '\\text{Msl6}',
                 lhablock = 'MASS',
                 lhacode = [ 2000015 ])

Msu1 = Parameter(name = 'Msu1',
                 nature = 'external',
                 type = 'real',
                 value = 348.2026,
                 texname = '\\text{Msu1}',
                 lhablock = 'MASS',
                 lhacode = [ 1000002 ])

Msu2 = Parameter(name = 'Msu2',
                 nature = 'external',
                 type = 'real',
                 value = 613.3992,
                 texname = '\\text{Msu2}',
                 lhablock = 'MASS',
                 lhacode = [ 1000004 ])

Msu3 = Parameter(name = 'Msu3',
                 nature = 'external',
                 type = 'real',
                 value = 615.7041,
                 texname = '\\text{Msu3}',
                 lhablock = 'MASS',
                 lhacode = [ 1000006 ])

Msu4 = Parameter(name = 'Msu4',
                 nature = 'external',
                 type = 'real',
                 value = 615.7276,
                 texname = '\\text{Msu4}',
                 lhablock = 'MASS',
                 lhacode = [ 2000002 ])

Msu5 = Parameter(name = 'Msu5',
                 nature = 'external',
                 type = 'real',
                 value = 630.2198,
                 texname = '\\text{Msu5}',
                 lhablock = 'MASS',
                 lhacode = [ 2000004 ])

Msu6 = Parameter(name = 'Msu6',
                 nature = 'external',
                 type = 'real',
                 value = 630.2357,
                 texname = '\\text{Msu6}',
                 lhablock = 'MASS',
                 lhacode = [ 2000006 ])

Msd1 = Parameter(name = 'Msd1',
                 nature = 'external',
                 type = 'real',
                 value = 537.874,
                 texname = '\\text{Msd1}',
                 lhablock = 'MASS',
                 lhacode = [ 1000001 ])

Msd2 = Parameter(name = 'Msd2',
                 nature = 'external',
                 type = 'real',
                 value = 610.5113,
                 texname = '\\text{Msd2}',
                 lhablock = 'MASS',
                 lhacode = [ 1000003 ])

Msd3 = Parameter(name = 'Msd3',
                 nature = 'external',
                 type = 'real',
                 value = 615.5144,
                 texname = '\\text{Msd3}',
                 lhablock = 'MASS',
                 lhacode = [ 1000005 ])

Msd4 = Parameter(name = 'Msd4',
                 nature = 'external',
                 type = 'real',
                 value = 615.5223,
                 texname = '\\text{Msd4}',
                 lhablock = 'MASS',
                 lhacode = [ 2000001 ])

Msd5 = Parameter(name = 'Msd5',
                 nature = 'external',
                 type = 'real',
                 value = 634.9333,
                 texname = '\\text{Msd5}',
                 lhablock = 'MASS',
                 lhacode = [ 2000003 ])

Msd6 = Parameter(name = 'Msd6',
                 nature = 'external',
                 type = 'real',
                 value = 634.9335,
                 texname = '\\text{Msd6}',
                 lhablock = 'MASS',
                 lhacode = [ 2000005 ])

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
               value = 2.141,
               texname = '\\text{WW}',
               lhablock = 'DECAY',
               lhacode = [ 24 ])

Wneu2 = Parameter(name = 'Wneu2',
                  nature = 'external',
                  type = 'real',
                  value = 0.00004107365,
                  texname = '\\text{Wneu2}',
                  lhablock = 'DECAY',
                  lhacode = [ 1000023 ])

Wneu3 = Parameter(name = 'Wneu3',
                  nature = 'external',
                  type = 'real',
                  value = 3.091347,
                  texname = '\\text{Wneu3}',
                  lhablock = 'DECAY',
                  lhacode = [ 1000025 ])

Wneu4 = Parameter(name = 'Wneu4',
                  nature = 'external',
                  type = 'real',
                  value = 3.193022,
                  texname = '\\text{Wneu4}',
                  lhablock = 'DECAY',
                  lhacode = [ 1000035 ])

Wch1 = Parameter(name = 'Wch1',
                 nature = 'external',
                 type = 'real',
                 value = 0.00105656,
                 texname = '\\text{Wch1}',
                 lhablock = 'DECAY',
                 lhacode = [ 1000024 ])

Wch2 = Parameter(name = 'Wch2',
                 nature = 'external',
                 type = 'real',
                 value = 5.260644,
                 texname = '\\text{Wch2}',
                 lhablock = 'DECAY',
                 lhacode = [ 1000037 ])

Wgo = Parameter(name = 'Wgo',
                nature = 'external',
                type = 'real',
                value = 2.524968,
                texname = '\\text{Wgo}',
                lhablock = 'DECAY',
                lhacode = [ 1000021 ])

WH01 = Parameter(name = 'WH01',
                 nature = 'external',
                 type = 'real',
                 value = 0.003518961,
                 texname = '\\text{WH01}',
                 lhablock = 'DECAY',
                 lhacode = [ 25 ])

WH02 = Parameter(name = 'WH02',
                 nature = 'external',
                 type = 'real',
                 value = 1.389974,
                 texname = '\\text{WH02}',
                 lhablock = 'DECAY',
                 lhacode = [ 35 ])

WA0 = Parameter(name = 'WA0',
                nature = 'external',
                type = 'real',
                value = 1.742413,
                texname = '\\text{WA0}',
                lhablock = 'DECAY',
                lhacode = [ 36 ])

WH = Parameter(name = 'WH',
               nature = 'external',
               type = 'real',
               value = 1.176094,
               texname = '\\text{WH}',
               lhablock = 'DECAY',
               lhacode = [ 37 ])

WT = Parameter(name = 'WT',
               nature = 'external',
               type = 'real',
               value = 1.463305,
               texname = '\\text{WT}',
               lhablock = 'DECAY',
               lhacode = [ 6 ])

Wsn1 = Parameter(name = 'Wsn1',
                 nature = 'external',
                 type = 'real',
                 value = 2.205383,
                 texname = '\\text{Wsn1}',
                 lhablock = 'DECAY',
                 lhacode = [ 1000012 ])

Wsn2 = Parameter(name = 'Wsn2',
                 nature = 'external',
                 type = 'real',
                 value = 2.263504,
                 texname = '\\text{Wsn2}',
                 lhablock = 'DECAY',
                 lhacode = [ 1000014 ])

Wsn3 = Parameter(name = 'Wsn3',
                 nature = 'external',
                 type = 'real',
                 value = 2.263504,
                 texname = '\\text{Wsn3}',
                 lhablock = 'DECAY',
                 lhacode = [ 1000016 ])

Wsl1 = Parameter(name = 'Wsl1',
                 nature = 'external',
                 type = 'real',
                 value = 1.333692,
                 texname = '\\text{Wsl1}',
                 lhablock = 'DECAY',
                 lhacode = [ 1000011 ])

Wsl2 = Parameter(name = 'Wsl2',
                 nature = 'external',
                 type = 'real',
                 value = 1.315543,
                 texname = '\\text{Wsl2}',
                 lhablock = 'DECAY',
                 lhacode = [ 1000013 ])

Wsl3 = Parameter(name = 'Wsl3',
                 nature = 'external',
                 type = 'real',
                 value = 1.315543,
                 texname = '\\text{Wsl3}',
                 lhablock = 'DECAY',
                 lhacode = [ 1000015 ])

Wsl4 = Parameter(name = 'Wsl4',
                 nature = 'external',
                 type = 'real',
                 value = 2.332189,
                 texname = '\\text{Wsl4}',
                 lhablock = 'DECAY',
                 lhacode = [ 2000011 ])

Wsl5 = Parameter(name = 'Wsl5',
                 nature = 'external',
                 type = 'real',
                 value = 2.332189,
                 texname = '\\text{Wsl5}',
                 lhablock = 'DECAY',
                 lhacode = [ 2000013 ])

Wsl6 = Parameter(name = 'Wsl6',
                 nature = 'external',
                 type = 'real',
                 value = 2.187202,
                 texname = '\\text{Wsl6}',
                 lhablock = 'DECAY',
                 lhacode = [ 2000015 ])

Wsu1 = Parameter(name = 'Wsu1',
                 nature = 'external',
                 type = 'real',
                 value = 1.34786,
                 texname = '\\text{Wsu1}',
                 lhablock = 'DECAY',
                 lhacode = [ 1000002 ])

Wsu2 = Parameter(name = 'Wsu2',
                 nature = 'external',
                 type = 'real',
                 value = 8.6374,
                 texname = '\\text{Wsu2}',
                 lhablock = 'DECAY',
                 lhacode = [ 1000004 ])

Wsu3 = Parameter(name = 'Wsu3',
                 nature = 'external',
                 type = 'real',
                 value = 1.28374,
                 texname = '\\text{Wsu3}',
                 lhablock = 'DECAY',
                 lhacode = [ 1000006 ])

Wsu4 = Parameter(name = 'Wsu4',
                 nature = 'external',
                 type = 'real',
                 value = 1.28374,
                 texname = '\\text{Wsu4}',
                 lhablock = 'DECAY',
                 lhacode = [ 2000002 ])

Wsu5 = Parameter(name = 'Wsu5',
                 nature = 'external',
                 type = 'real',
                 value = 6.37685,
                 texname = '\\text{Wsu5}',
                 lhablock = 'DECAY',
                 lhacode = [ 2000004 ])

Wsu6 = Parameter(name = 'Wsu6',
                 nature = 'external',
                 type = 'real',
                 value = 6.37685,
                 texname = '\\text{Wsu6}',
                 lhablock = 'DECAY',
                 lhacode = [ 2000006 ])

Wsd1 = Parameter(name = 'Wsd1',
                 nature = 'external',
                 type = 'real',
                 value = 7.06064,
                 texname = '\\text{Wsd1}',
                 lhablock = 'DECAY',
                 lhacode = [ 1000001 ])

Wsd2 = Parameter(name = 'Wsd2',
                 nature = 'external',
                 type = 'real',
                 value = 0.604363,
                 texname = '\\text{Wsd2}',
                 lhablock = 'DECAY',
                 lhacode = [ 1000003 ])

Wsd3 = Parameter(name = 'Wsd3',
                 nature = 'external',
                 type = 'real',
                 value = 0.320863,
                 texname = '\\text{Wsd3}',
                 lhablock = 'DECAY',
                 lhacode = [ 1000005 ])

Wsd4 = Parameter(name = 'Wsd4',
                 nature = 'external',
                 type = 'real',
                 value = 0.320863,
                 texname = '\\text{Wsd4}',
                 lhablock = 'DECAY',
                 lhacode = [ 2000001 ])

Wsd5 = Parameter(name = 'Wsd5',
                 nature = 'external',
                 type = 'real',
                 value = 6.30448,
                 texname = '\\text{Wsd5}',
                 lhablock = 'DECAY',
                 lhacode = [ 2000003 ])

Wsd6 = Parameter(name = 'Wsd6',
                 nature = 'external',
                 type = 'real',
                 value = 6.30448,
                 texname = '\\text{Wsd6}',
                 lhablock = 'DECAY',
                 lhacode = [ 2000005 ])

beta = Parameter(name = 'beta',
                 nature = 'internal',
                 type = 'real',
                 value = 'cmath.atan(tb)',
                 texname = '\\beta')

cw = Parameter(name = 'cw',
               nature = 'internal',
               type = 'real',
               value = 'MW/MZ',
               texname = 'c_w')

mD21x1 = Parameter(name = 'mD21x1',
                   nature = 'internal',
                   type = 'complex',
                   value = 'RmD21x1',
                   texname = '\\text{mD21x1}')

mD22x2 = Parameter(name = 'mD22x2',
                   nature = 'internal',
                   type = 'complex',
                   value = 'RmD22x2',
                   texname = '\\text{mD22x2}')

mD23x3 = Parameter(name = 'mD23x3',
                   nature = 'internal',
                   type = 'complex',
                   value = 'RmD23x3',
                   texname = '\\text{mD23x3}')

mE21x1 = Parameter(name = 'mE21x1',
                   nature = 'internal',
                   type = 'complex',
                   value = 'RmE21x1',
                   texname = '\\text{mE21x1}')

mE22x2 = Parameter(name = 'mE22x2',
                   nature = 'internal',
                   type = 'complex',
                   value = 'RmE22x2',
                   texname = '\\text{mE22x2}')

mE23x3 = Parameter(name = 'mE23x3',
                   nature = 'internal',
                   type = 'complex',
                   value = 'RmE23x3',
                   texname = '\\text{mE23x3}')

mL21x1 = Parameter(name = 'mL21x1',
                   nature = 'internal',
                   type = 'complex',
                   value = 'RmL21x1',
                   texname = '\\text{mL21x1}')

mL22x2 = Parameter(name = 'mL22x2',
                   nature = 'internal',
                   type = 'complex',
                   value = 'RmL22x2',
                   texname = '\\text{mL22x2}')

mL23x3 = Parameter(name = 'mL23x3',
                   nature = 'internal',
                   type = 'complex',
                   value = 'RmL23x3',
                   texname = '\\text{mL23x3}')

mQ21x1 = Parameter(name = 'mQ21x1',
                   nature = 'internal',
                   type = 'complex',
                   value = 'RmQ21x1',
                   texname = '\\text{mQ21x1}')

mQ22x2 = Parameter(name = 'mQ22x2',
                   nature = 'internal',
                   type = 'complex',
                   value = 'RmQ22x2',
                   texname = '\\text{mQ22x2}')

mQ23x3 = Parameter(name = 'mQ23x3',
                   nature = 'internal',
                   type = 'complex',
                   value = 'RmQ23x3',
                   texname = '\\text{mQ23x3}')

mU21x1 = Parameter(name = 'mU21x1',
                   nature = 'internal',
                   type = 'complex',
                   value = 'RmU21x1',
                   texname = '\\text{mU21x1}')

mU22x2 = Parameter(name = 'mU22x2',
                   nature = 'internal',
                   type = 'complex',
                   value = 'RmU22x2',
                   texname = '\\text{mU22x2}')

mU23x3 = Parameter(name = 'mU23x3',
                   nature = 'internal',
                   type = 'complex',
                   value = 'RmU23x3',
                   texname = '\\text{mU23x3}')

MUH = Parameter(name = 'MUH',
                nature = 'internal',
                type = 'complex',
                value = 'RMUH',
                texname = '\\mu')

Mx1 = Parameter(name = 'Mx1',
                nature = 'internal',
                type = 'complex',
                value = 'RMx1',
                texname = 'M_1')

Mx2 = Parameter(name = 'Mx2',
                nature = 'internal',
                type = 'complex',
                value = 'RMx2',
                texname = 'M_2')

Mx3 = Parameter(name = 'Mx3',
                nature = 'internal',
                type = 'complex',
                value = 'RMx3',
                texname = 'M_3')

NN1x1 = Parameter(name = 'NN1x1',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RNN1x1',
                  texname = '\\text{NN1x1}')

NN1x2 = Parameter(name = 'NN1x2',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RNN1x2',
                  texname = '\\text{NN1x2}')

NN1x3 = Parameter(name = 'NN1x3',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RNN1x3',
                  texname = '\\text{NN1x3}')

NN1x4 = Parameter(name = 'NN1x4',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RNN1x4',
                  texname = '\\text{NN1x4}')

NN2x1 = Parameter(name = 'NN2x1',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RNN2x1',
                  texname = '\\text{NN2x1}')

NN2x2 = Parameter(name = 'NN2x2',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RNN2x2',
                  texname = '\\text{NN2x2}')

NN2x3 = Parameter(name = 'NN2x3',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RNN2x3',
                  texname = '\\text{NN2x3}')

NN2x4 = Parameter(name = 'NN2x4',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RNN2x4',
                  texname = '\\text{NN2x4}')

NN3x1 = Parameter(name = 'NN3x1',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RNN3x1',
                  texname = '\\text{NN3x1}')

NN3x2 = Parameter(name = 'NN3x2',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RNN3x2',
                  texname = '\\text{NN3x2}')

NN3x3 = Parameter(name = 'NN3x3',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RNN3x3',
                  texname = '\\text{NN3x3}')

NN3x4 = Parameter(name = 'NN3x4',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RNN3x4',
                  texname = '\\text{NN3x4}')

NN4x1 = Parameter(name = 'NN4x1',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RNN4x1',
                  texname = '\\text{NN4x1}')

NN4x2 = Parameter(name = 'NN4x2',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RNN4x2',
                  texname = '\\text{NN4x2}')

NN4x3 = Parameter(name = 'NN4x3',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RNN4x3',
                  texname = '\\text{NN4x3}')

NN4x4 = Parameter(name = 'NN4x4',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RNN4x4',
                  texname = '\\text{NN4x4}')

Rd1x3 = Parameter(name = 'Rd1x3',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRd1x3',
                  texname = '\\text{Rd1x3}')

Rd1x6 = Parameter(name = 'Rd1x6',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRd1x6',
                  texname = '\\text{Rd1x6}')

Rd2x3 = Parameter(name = 'Rd2x3',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRd2x3',
                  texname = '\\text{Rd2x3}')

Rd2x6 = Parameter(name = 'Rd2x6',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRd2x6',
                  texname = '\\text{Rd2x6}')

Rd3x5 = Parameter(name = 'Rd3x5',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRd3x5',
                  texname = '\\text{Rd3x5}')

Rd4x4 = Parameter(name = 'Rd4x4',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRd4x4',
                  texname = '\\text{Rd4x4}')

Rd5x2 = Parameter(name = 'Rd5x2',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRd5x2',
                  texname = '\\text{Rd5x2}')

Rd6x1 = Parameter(name = 'Rd6x1',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRd6x1',
                  texname = '\\text{Rd6x1}')

Rl1x3 = Parameter(name = 'Rl1x3',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRl1x3',
                  texname = '\\text{Rl1x3}')

Rl1x6 = Parameter(name = 'Rl1x6',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRl1x6',
                  texname = '\\text{Rl1x6}')

Rl2x5 = Parameter(name = 'Rl2x5',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRl2x5',
                  texname = '\\text{Rl2x5}')

Rl3x4 = Parameter(name = 'Rl3x4',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRl3x4',
                  texname = '\\text{Rl3x4}')

Rl4x1 = Parameter(name = 'Rl4x1',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRl4x1',
                  texname = '\\text{Rl4x1}')

Rl5x2 = Parameter(name = 'Rl5x2',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRl5x2',
                  texname = '\\text{Rl5x2}')

Rl6x3 = Parameter(name = 'Rl6x3',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRl6x3',
                  texname = '\\text{Rl6x3}')

Rl6x6 = Parameter(name = 'Rl6x6',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRl6x6',
                  texname = '\\text{Rl6x6}')

Rn1x3 = Parameter(name = 'Rn1x3',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRn1x3',
                  texname = '\\text{Rn1x3}')

Rn2x2 = Parameter(name = 'Rn2x2',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRn2x2',
                  texname = '\\text{Rn2x2}')

Rn3x1 = Parameter(name = 'Rn3x1',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRn3x1',
                  texname = '\\text{Rn3x1}')

Ru1x3 = Parameter(name = 'Ru1x3',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRu1x3',
                  texname = '\\text{Ru1x3}')

Ru1x6 = Parameter(name = 'Ru1x6',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRu1x6',
                  texname = '\\text{Ru1x6}')

Ru2x3 = Parameter(name = 'Ru2x3',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRu2x3',
                  texname = '\\text{Ru2x3}')

Ru2x6 = Parameter(name = 'Ru2x6',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRu2x6',
                  texname = '\\text{Ru2x6}')

Ru3x5 = Parameter(name = 'Ru3x5',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRu3x5',
                  texname = '\\text{Ru3x5}')

Ru4x4 = Parameter(name = 'Ru4x4',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRu4x4',
                  texname = '\\text{Ru4x4}')

Ru5x1 = Parameter(name = 'Ru5x1',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRu5x1',
                  texname = '\\text{Ru5x1}')

Ru6x2 = Parameter(name = 'Ru6x2',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RRu6x2',
                  texname = '\\text{Ru6x2}')

UU1x1 = Parameter(name = 'UU1x1',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RUU1x1',
                  texname = '\\text{UU1x1}')

UU1x2 = Parameter(name = 'UU1x2',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RUU1x2',
                  texname = '\\text{UU1x2}')

UU2x1 = Parameter(name = 'UU2x1',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RUU2x1',
                  texname = '\\text{UU2x1}')

UU2x2 = Parameter(name = 'UU2x2',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RUU2x2',
                  texname = '\\text{UU2x2}')

VV1x1 = Parameter(name = 'VV1x1',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RVV1x1',
                  texname = '\\text{VV1x1}')

VV1x2 = Parameter(name = 'VV1x2',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RVV1x2',
                  texname = '\\text{VV1x2}')

VV2x1 = Parameter(name = 'VV2x1',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RVV2x1',
                  texname = '\\text{VV2x1}')

VV2x2 = Parameter(name = 'VV2x2',
                  nature = 'internal',
                  type = 'complex',
                  value = 'RVV2x2',
                  texname = '\\text{VV2x2}')

ee = Parameter(name = 'ee',
               nature = 'internal',
               type = 'real',
               value = '2*cmath.sqrt(1/aEWM1)*cmath.sqrt(cmath.pi)',
               texname = 'e')

G = Parameter(name = 'G',
              nature = 'internal',
              type = 'real',
              value = '2*cmath.sqrt(aS)*cmath.sqrt(cmath.pi)',
              texname = 'G')

gp = Parameter(name = 'gp',
               nature = 'internal',
               type = 'real',
               value = '1',
               texname = 'g\'')

gw = Parameter(name = 'gw',
               nature = 'internal',
               type = 'real',
               value = '1',
               texname = 'g_w')

LLLE1x2x1 = Parameter(name = 'LLLE1x2x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLLE1x2x1',
                      texname = '\\text{LLLE1x2x1}')

LLLE1x2x2 = Parameter(name = 'LLLE1x2x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLLE1x2x2',
                      texname = '\\text{LLLE1x2x2}')

LLLE1x2x3 = Parameter(name = 'LLLE1x2x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLLE1x2x3',
                      texname = '\\text{LLLE1x2x3}')

LLLE1x3x1 = Parameter(name = 'LLLE1x3x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLLE1x3x1',
                      texname = '\\text{LLLE1x3x1}')

LLLE1x3x2 = Parameter(name = 'LLLE1x3x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLLE1x3x2',
                      texname = '\\text{LLLE1x3x2}')

LLLE1x3x3 = Parameter(name = 'LLLE1x3x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLLE1x3x3',
                      texname = '\\text{LLLE1x3x3}')

LLLE2x1x1 = Parameter(name = 'LLLE2x1x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLLE2x1x1',
                      texname = '\\text{LLLE2x1x1}')

LLLE2x1x2 = Parameter(name = 'LLLE2x1x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLLE2x1x2',
                      texname = '\\text{LLLE2x1x2}')

LLLE2x1x3 = Parameter(name = 'LLLE2x1x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLLE2x1x3',
                      texname = '\\text{LLLE2x1x3}')

LLLE2x3x1 = Parameter(name = 'LLLE2x3x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLLE2x3x1',
                      texname = '\\text{LLLE2x3x1}')

LLLE2x3x2 = Parameter(name = 'LLLE2x3x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLLE2x3x2',
                      texname = '\\text{LLLE2x3x2}')

LLLE2x3x3 = Parameter(name = 'LLLE2x3x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLLE2x3x3',
                      texname = '\\text{LLLE2x3x3}')

LLLE3x1x1 = Parameter(name = 'LLLE3x1x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLLE3x1x1',
                      texname = '\\text{LLLE3x1x1}')

LLLE3x1x2 = Parameter(name = 'LLLE3x1x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLLE3x1x2',
                      texname = '\\text{LLLE3x1x2}')

LLLE3x1x3 = Parameter(name = 'LLLE3x1x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLLE3x1x3',
                      texname = '\\text{LLLE3x1x3}')

LLLE3x2x1 = Parameter(name = 'LLLE3x2x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLLE3x2x1',
                      texname = '\\text{LLLE3x2x1}')

LLLE3x2x2 = Parameter(name = 'LLLE3x2x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLLE3x2x2',
                      texname = '\\text{LLLE3x2x2}')

LLLE3x2x3 = Parameter(name = 'LLLE3x2x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLLE3x2x3',
                      texname = '\\text{LLLE3x2x3}')

LLQD1x1x1 = Parameter(name = 'LLQD1x1x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD1x1x1',
                      texname = '\\text{LLQD1x1x1}')

LLQD1x1x2 = Parameter(name = 'LLQD1x1x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD1x1x2',
                      texname = '\\text{LLQD1x1x2}')

LLQD1x1x3 = Parameter(name = 'LLQD1x1x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD1x1x3',
                      texname = '\\text{LLQD1x1x3}')

LLQD1x2x1 = Parameter(name = 'LLQD1x2x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD1x2x1',
                      texname = '\\text{LLQD1x2x1}')

LLQD1x2x2 = Parameter(name = 'LLQD1x2x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD1x2x2',
                      texname = '\\text{LLQD1x2x2}')

LLQD1x2x3 = Parameter(name = 'LLQD1x2x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD1x2x3',
                      texname = '\\text{LLQD1x2x3}')

LLQD1x3x1 = Parameter(name = 'LLQD1x3x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD1x3x1',
                      texname = '\\text{LLQD1x3x1}')

LLQD1x3x2 = Parameter(name = 'LLQD1x3x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD1x3x2',
                      texname = '\\text{LLQD1x3x2}')

LLQD1x3x3 = Parameter(name = 'LLQD1x3x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD1x3x3',
                      texname = '\\text{LLQD1x3x3}')

LLQD2x1x1 = Parameter(name = 'LLQD2x1x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD2x1x1',
                      texname = '\\text{LLQD2x1x1}')

LLQD2x1x2 = Parameter(name = 'LLQD2x1x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD2x1x2',
                      texname = '\\text{LLQD2x1x2}')

LLQD2x1x3 = Parameter(name = 'LLQD2x1x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD2x1x3',
                      texname = '\\text{LLQD2x1x3}')

LLQD2x2x1 = Parameter(name = 'LLQD2x2x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD2x2x1',
                      texname = '\\text{LLQD2x2x1}')

LLQD2x2x2 = Parameter(name = 'LLQD2x2x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD2x2x2',
                      texname = '\\text{LLQD2x2x2}')

LLQD2x2x3 = Parameter(name = 'LLQD2x2x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD2x2x3',
                      texname = '\\text{LLQD2x2x3}')

LLQD2x3x1 = Parameter(name = 'LLQD2x3x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD2x3x1',
                      texname = '\\text{LLQD2x3x1}')

LLQD2x3x2 = Parameter(name = 'LLQD2x3x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD2x3x2',
                      texname = '\\text{LLQD2x3x2}')

LLQD2x3x3 = Parameter(name = 'LLQD2x3x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD2x3x3',
                      texname = '\\text{LLQD2x3x3}')

LLQD3x1x1 = Parameter(name = 'LLQD3x1x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD3x1x1',
                      texname = '\\text{LLQD3x1x1}')

LLQD3x1x2 = Parameter(name = 'LLQD3x1x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD3x1x2',
                      texname = '\\text{LLQD3x1x2}')

LLQD3x1x3 = Parameter(name = 'LLQD3x1x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD3x1x3',
                      texname = '\\text{LLQD3x1x3}')

LLQD3x2x1 = Parameter(name = 'LLQD3x2x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD3x2x1',
                      texname = '\\text{LLQD3x2x1}')

LLQD3x2x2 = Parameter(name = 'LLQD3x2x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD3x2x2',
                      texname = '\\text{LLQD3x2x2}')

LLQD3x2x3 = Parameter(name = 'LLQD3x2x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD3x2x3',
                      texname = '\\text{LLQD3x2x3}')

LLQD3x3x1 = Parameter(name = 'LLQD3x3x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD3x3x1',
                      texname = '\\text{LLQD3x3x1}')

LLQD3x3x2 = Parameter(name = 'LLQD3x3x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD3x3x2',
                      texname = '\\text{LLQD3x3x2}')

LLQD3x3x3 = Parameter(name = 'LLQD3x3x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLQD3x3x3',
                      texname = '\\text{LLQD3x3x3}')

LUDD1x1x2 = Parameter(name = 'LUDD1x1x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLDD1x1x2',
                      texname = '\\text{LUDD1x1x2}')

LUDD1x1x3 = Parameter(name = 'LUDD1x1x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLDD1x1x3',
                      texname = '\\text{LUDD1x1x3}')

LUDD1x2x1 = Parameter(name = 'LUDD1x2x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLDD1x2x1',
                      texname = '\\text{LUDD1x2x1}')

LUDD1x2x3 = Parameter(name = 'LUDD1x2x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLDD1x2x3',
                      texname = '\\text{LUDD1x2x3}')

LUDD1x3x1 = Parameter(name = 'LUDD1x3x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLDD1x3x1',
                      texname = '\\text{LUDD1x3x1}')

LUDD1x3x2 = Parameter(name = 'LUDD1x3x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLDD1x3x2',
                      texname = '\\text{LUDD1x3x2}')

LUDD2x1x2 = Parameter(name = 'LUDD2x1x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLDD2x1x2',
                      texname = '\\text{LUDD2x1x2}')

LUDD2x1x3 = Parameter(name = 'LUDD2x1x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLDD2x1x3',
                      texname = '\\text{LUDD2x1x3}')

LUDD2x2x1 = Parameter(name = 'LUDD2x2x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLDD2x2x1',
                      texname = '\\text{LUDD2x2x1}')

LUDD2x2x3 = Parameter(name = 'LUDD2x2x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLDD2x2x3',
                      texname = '\\text{LUDD2x2x3}')

LUDD2x3x1 = Parameter(name = 'LUDD2x3x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLDD2x3x1',
                      texname = '\\text{LUDD2x3x1}')

LUDD2x3x2 = Parameter(name = 'LUDD2x3x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLDD2x3x2',
                      texname = '\\text{LUDD2x3x2}')

LUDD3x1x2 = Parameter(name = 'LUDD3x1x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLDD3x1x2',
                      texname = '\\text{LUDD3x1x2}')

LUDD3x1x3 = Parameter(name = 'LUDD3x1x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLDD3x1x3',
                      texname = '\\text{LUDD3x1x3}')

LUDD3x2x1 = Parameter(name = 'LUDD3x2x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLDD3x2x1',
                      texname = '\\text{LUDD3x2x1}')

LUDD3x2x3 = Parameter(name = 'LUDD3x2x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLDD3x2x3',
                      texname = '\\text{LUDD3x2x3}')

LUDD3x3x1 = Parameter(name = 'LUDD3x3x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLDD3x3x1',
                      texname = '\\text{LUDD3x3x1}')

LUDD3x3x2 = Parameter(name = 'LUDD3x3x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RLDD3x3x2',
                      texname = '\\text{LUDD3x3x2}')

td1x1 = Parameter(name = 'td1x1',
                  nature = 'internal',
                  type = 'complex',
                  value = 'Rtd1x1',
                  texname = '\\text{td1x1}')

td2x2 = Parameter(name = 'td2x2',
                  nature = 'internal',
                  type = 'complex',
                  value = 'Rtd2x2',
                  texname = '\\text{td2x2}')

td3x3 = Parameter(name = 'td3x3',
                  nature = 'internal',
                  type = 'complex',
                  value = 'Rtd3x3',
                  texname = '\\text{td3x3}')

te1x1 = Parameter(name = 'te1x1',
                  nature = 'internal',
                  type = 'complex',
                  value = 'Rte1x1',
                  texname = '\\text{te1x1}')

te2x2 = Parameter(name = 'te2x2',
                  nature = 'internal',
                  type = 'complex',
                  value = 'Rte2x2',
                  texname = '\\text{te2x2}')

te3x3 = Parameter(name = 'te3x3',
                  nature = 'internal',
                  type = 'complex',
                  value = 'Rte3x3',
                  texname = '\\text{te3x3}')

TLLE1x2x1 = Parameter(name = 'TLLE1x2x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTLE1x2x1',
                      texname = '\\text{TLLE1x2x1}')

TLLE1x2x2 = Parameter(name = 'TLLE1x2x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTLE1x2x2',
                      texname = '\\text{TLLE1x2x2}')

TLLE1x2x3 = Parameter(name = 'TLLE1x2x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTLE1x2x3',
                      texname = '\\text{TLLE1x2x3}')

TLLE1x3x1 = Parameter(name = 'TLLE1x3x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTLE1x3x1',
                      texname = '\\text{TLLE1x3x1}')

TLLE1x3x2 = Parameter(name = 'TLLE1x3x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTLE1x3x2',
                      texname = '\\text{TLLE1x3x2}')

TLLE1x3x3 = Parameter(name = 'TLLE1x3x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTLE1x3x3',
                      texname = '\\text{TLLE1x3x3}')

TLLE2x1x1 = Parameter(name = 'TLLE2x1x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTLE2x1x1',
                      texname = '\\text{TLLE2x1x1}')

TLLE2x1x2 = Parameter(name = 'TLLE2x1x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTLE2x1x2',
                      texname = '\\text{TLLE2x1x2}')

TLLE2x1x3 = Parameter(name = 'TLLE2x1x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTLE2x1x3',
                      texname = '\\text{TLLE2x1x3}')

TLLE2x3x1 = Parameter(name = 'TLLE2x3x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTLE2x3x1',
                      texname = '\\text{TLLE2x3x1}')

TLLE2x3x2 = Parameter(name = 'TLLE2x3x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTLE2x3x2',
                      texname = '\\text{TLLE2x3x2}')

TLLE2x3x3 = Parameter(name = 'TLLE2x3x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTLE2x3x3',
                      texname = '\\text{TLLE2x3x3}')

TLLE3x1x1 = Parameter(name = 'TLLE3x1x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTLE3x1x1',
                      texname = '\\text{TLLE3x1x1}')

TLLE3x1x2 = Parameter(name = 'TLLE3x1x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTLE3x1x2',
                      texname = '\\text{TLLE3x1x2}')

TLLE3x1x3 = Parameter(name = 'TLLE3x1x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTLE3x1x3',
                      texname = '\\text{TLLE3x1x3}')

TLLE3x2x1 = Parameter(name = 'TLLE3x2x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTLE3x2x1',
                      texname = '\\text{TLLE3x2x1}')

TLLE3x2x2 = Parameter(name = 'TLLE3x2x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTLE3x2x2',
                      texname = '\\text{TLLE3x2x2}')

TLLE3x2x3 = Parameter(name = 'TLLE3x2x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTLE3x2x3',
                      texname = '\\text{TLLE3x2x3}')

TLQD1x1x1 = Parameter(name = 'TLQD1x1x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD1x1x1',
                      texname = '\\text{TLQD1x1x1}')

TLQD1x1x2 = Parameter(name = 'TLQD1x1x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD1x1x2',
                      texname = '\\text{TLQD1x1x2}')

TLQD1x1x3 = Parameter(name = 'TLQD1x1x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD1x1x3',
                      texname = '\\text{TLQD1x1x3}')

TLQD1x2x1 = Parameter(name = 'TLQD1x2x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD1x2x1',
                      texname = '\\text{TLQD1x2x1}')

TLQD1x2x2 = Parameter(name = 'TLQD1x2x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD1x2x2',
                      texname = '\\text{TLQD1x2x2}')

TLQD1x2x3 = Parameter(name = 'TLQD1x2x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD1x2x3',
                      texname = '\\text{TLQD1x2x3}')

TLQD1x3x1 = Parameter(name = 'TLQD1x3x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD1x3x1',
                      texname = '\\text{TLQD1x3x1}')

TLQD1x3x2 = Parameter(name = 'TLQD1x3x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD1x3x2',
                      texname = '\\text{TLQD1x3x2}')

TLQD1x3x3 = Parameter(name = 'TLQD1x3x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD1x3x3',
                      texname = '\\text{TLQD1x3x3}')

TLQD2x1x1 = Parameter(name = 'TLQD2x1x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD2x1x1',
                      texname = '\\text{TLQD2x1x1}')

TLQD2x1x2 = Parameter(name = 'TLQD2x1x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD2x1x2',
                      texname = '\\text{TLQD2x1x2}')

TLQD2x1x3 = Parameter(name = 'TLQD2x1x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD2x1x3',
                      texname = '\\text{TLQD2x1x3}')

TLQD2x2x1 = Parameter(name = 'TLQD2x2x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD2x2x1',
                      texname = '\\text{TLQD2x2x1}')

TLQD2x2x2 = Parameter(name = 'TLQD2x2x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD2x2x2',
                      texname = '\\text{TLQD2x2x2}')

TLQD2x2x3 = Parameter(name = 'TLQD2x2x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD2x2x3',
                      texname = '\\text{TLQD2x2x3}')

TLQD2x3x1 = Parameter(name = 'TLQD2x3x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD2x3x1',
                      texname = '\\text{TLQD2x3x1}')

TLQD2x3x2 = Parameter(name = 'TLQD2x3x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD2x3x2',
                      texname = '\\text{TLQD2x3x2}')

TLQD2x3x3 = Parameter(name = 'TLQD2x3x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD2x3x3',
                      texname = '\\text{TLQD2x3x3}')

TLQD3x1x1 = Parameter(name = 'TLQD3x1x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD3x1x1',
                      texname = '\\text{TLQD3x1x1}')

TLQD3x1x2 = Parameter(name = 'TLQD3x1x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD3x1x2',
                      texname = '\\text{TLQD3x1x2}')

TLQD3x1x3 = Parameter(name = 'TLQD3x1x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD3x1x3',
                      texname = '\\text{TLQD3x1x3}')

TLQD3x2x1 = Parameter(name = 'TLQD3x2x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD3x2x1',
                      texname = '\\text{TLQD3x2x1}')

TLQD3x2x2 = Parameter(name = 'TLQD3x2x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD3x2x2',
                      texname = '\\text{TLQD3x2x2}')

TLQD3x2x3 = Parameter(name = 'TLQD3x2x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD3x2x3',
                      texname = '\\text{TLQD3x2x3}')

TLQD3x3x1 = Parameter(name = 'TLQD3x3x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD3x3x1',
                      texname = '\\text{TLQD3x3x1}')

TLQD3x3x2 = Parameter(name = 'TLQD3x3x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD3x3x2',
                      texname = '\\text{TLQD3x3x2}')

TLQD3x3x3 = Parameter(name = 'TLQD3x3x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTQD3x3x3',
                      texname = '\\text{TLQD3x3x3}')

tu1x1 = Parameter(name = 'tu1x1',
                  nature = 'internal',
                  type = 'complex',
                  value = 'Rtu1x1',
                  texname = '\\text{tu1x1}')

tu2x2 = Parameter(name = 'tu2x2',
                  nature = 'internal',
                  type = 'complex',
                  value = 'Rtu2x2',
                  texname = '\\text{tu2x2}')

tu3x3 = Parameter(name = 'tu3x3',
                  nature = 'internal',
                  type = 'complex',
                  value = 'Rtu3x3',
                  texname = '\\text{tu3x3}')

TUDD1x1x2 = Parameter(name = 'TUDD1x1x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTDD1x1x2',
                      texname = '\\text{TUDD1x1x2}')

TUDD1x1x3 = Parameter(name = 'TUDD1x1x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTDD1x1x3',
                      texname = '\\text{TUDD1x1x3}')

TUDD1x2x1 = Parameter(name = 'TUDD1x2x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTDD1x2x1',
                      texname = '\\text{TUDD1x2x1}')

TUDD1x2x3 = Parameter(name = 'TUDD1x2x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTDD1x2x3',
                      texname = '\\text{TUDD1x2x3}')

TUDD1x3x1 = Parameter(name = 'TUDD1x3x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTDD1x3x1',
                      texname = '\\text{TUDD1x3x1}')

TUDD1x3x2 = Parameter(name = 'TUDD1x3x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTDD1x3x2',
                      texname = '\\text{TUDD1x3x2}')

TUDD2x1x2 = Parameter(name = 'TUDD2x1x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTDD2x1x2',
                      texname = '\\text{TUDD2x1x2}')

TUDD2x1x3 = Parameter(name = 'TUDD2x1x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTDD2x1x3',
                      texname = '\\text{TUDD2x1x3}')

TUDD2x2x1 = Parameter(name = 'TUDD2x2x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTDD2x2x1',
                      texname = '\\text{TUDD2x2x1}')

TUDD2x2x3 = Parameter(name = 'TUDD2x2x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTDD2x2x3',
                      texname = '\\text{TUDD2x2x3}')

TUDD2x3x1 = Parameter(name = 'TUDD2x3x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTDD2x3x1',
                      texname = '\\text{TUDD2x3x1}')

TUDD2x3x2 = Parameter(name = 'TUDD2x3x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTDD2x3x2',
                      texname = '\\text{TUDD2x3x2}')

TUDD3x1x2 = Parameter(name = 'TUDD3x1x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTDD3x1x2',
                      texname = '\\text{TUDD3x1x2}')

TUDD3x1x3 = Parameter(name = 'TUDD3x1x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTDD3x1x3',
                      texname = '\\text{TUDD3x1x3}')

TUDD3x2x1 = Parameter(name = 'TUDD3x2x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTDD3x2x1',
                      texname = '\\text{TUDD3x2x1}')

TUDD3x2x3 = Parameter(name = 'TUDD3x2x3',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTDD3x2x3',
                      texname = '\\text{TUDD3x2x3}')

TUDD3x3x1 = Parameter(name = 'TUDD3x3x1',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTDD3x3x1',
                      texname = '\\text{TUDD3x3x1}')

TUDD3x3x2 = Parameter(name = 'TUDD3x3x2',
                      nature = 'internal',
                      type = 'complex',
                      value = 'RTDD3x3x2',
                      texname = '\\text{TUDD3x3x2}')

yd1x1 = Parameter(name = 'yd1x1',
                  nature = 'internal',
                  type = 'complex',
                  value = 'Ryd1x1',
                  texname = '\\text{yd1x1}')

yd2x2 = Parameter(name = 'yd2x2',
                  nature = 'internal',
                  type = 'complex',
                  value = 'Ryd2x2',
                  texname = '\\text{yd2x2}')

yd3x3 = Parameter(name = 'yd3x3',
                  nature = 'internal',
                  type = 'complex',
                  value = 'Ryd3x3',
                  texname = '\\text{yd3x3}')

ye1x1 = Parameter(name = 'ye1x1',
                  nature = 'internal',
                  type = 'complex',
                  value = 'Rye1x1',
                  texname = '\\text{ye1x1}')

ye2x2 = Parameter(name = 'ye2x2',
                  nature = 'internal',
                  type = 'complex',
                  value = 'Rye2x2',
                  texname = '\\text{ye2x2}')

ye3x3 = Parameter(name = 'ye3x3',
                  nature = 'internal',
                  type = 'complex',
                  value = 'Rye3x3',
                  texname = '\\text{ye3x3}')

yu1x1 = Parameter(name = 'yu1x1',
                  nature = 'internal',
                  type = 'complex',
                  value = 'Ryu1x1',
                  texname = '\\text{yu1x1}')

yu2x2 = Parameter(name = 'yu2x2',
                  nature = 'internal',
                  type = 'complex',
                  value = 'Ryu2x2',
                  texname = '\\text{yu2x2}')

yu3x3 = Parameter(name = 'yu3x3',
                  nature = 'internal',
                  type = 'complex',
                  value = 'Ryu3x3',
                  texname = '\\text{yu3x3}')

bb = Parameter(name = 'bb',
               nature = 'internal',
               type = 'complex',
               value = '((-mHd2 + mHu2 - MZ**2*cmath.cos(2*beta))*cmath.tan(2*beta))/2.',
               texname = 'b')

sw = Parameter(name = 'sw',
               nature = 'internal',
               type = 'real',
               value = 'cmath.sqrt(1 - cw**2)',
               texname = 's_w')

vev = Parameter(name = 'vev',
                nature = 'internal',
                type = 'real',
                value = '(2*cw*MZ*sw)/ee',
                texname = 'v')

vd = Parameter(name = 'vd',
               nature = 'internal',
               type = 'real',
               value = 'vev*cmath.cos(beta)',
               texname = 'v_d')

vu = Parameter(name = 'vu',
               nature = 'internal',
               type = 'real',
               value = 'vev*cmath.sin(beta)',
               texname = 'v_u')

I1x11 = Parameter(name = 'I1x11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'complexconjugate(yu1x1)',
                  texname = '\\text{I1x11}')

I1x22 = Parameter(name = 'I1x22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'complexconjugate(yu2x2)',
                  texname = '\\text{I1x22}')

I1x33 = Parameter(name = 'I1x33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'complexconjugate(yu3x3)',
                  texname = '\\text{I1x33}')

I10x11 = Parameter(name = 'I10x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*complexconjugate(Rd1x3)',
                   texname = '\\text{I10x11}')

I10x12 = Parameter(name = 'I10x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*complexconjugate(Rd1x3)',
                   texname = '\\text{I10x12}')

I10x21 = Parameter(name = 'I10x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*complexconjugate(Rd2x3)',
                   texname = '\\text{I10x21}')

I10x22 = Parameter(name = 'I10x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*complexconjugate(Rd2x3)',
                   texname = '\\text{I10x22}')

I10x55 = Parameter(name = 'I10x55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd5x2*complexconjugate(Rd5x2)',
                   texname = '\\text{I10x55}')

I10x66 = Parameter(name = 'I10x66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd6x1*complexconjugate(Rd6x1)',
                   texname = '\\text{I10x66}')

I100x131 = Parameter(name = 'I100x131',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x3x2)',
                     texname = '\\text{I100x131}')

I100x132 = Parameter(name = 'I100x132',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x3x2)',
                     texname = '\\text{I100x132}')

I100x133 = Parameter(name = 'I100x133',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x3x2)',
                     texname = '\\text{I100x133}')

I100x134 = Parameter(name = 'I100x134',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x3x2)',
                     texname = '\\text{I100x134}')

I100x141 = Parameter(name = 'I100x141',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x3x1)',
                     texname = '\\text{I100x141}')

I100x142 = Parameter(name = 'I100x142',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x3x1)',
                     texname = '\\text{I100x142}')

I100x143 = Parameter(name = 'I100x143',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x3x1)',
                     texname = '\\text{I100x143}')

I100x144 = Parameter(name = 'I100x144',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x3x1)',
                     texname = '\\text{I100x144}')

I100x231 = Parameter(name = 'I100x231',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x3x2)',
                     texname = '\\text{I100x231}')

I100x232 = Parameter(name = 'I100x232',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x3x2)',
                     texname = '\\text{I100x232}')

I100x233 = Parameter(name = 'I100x233',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x3x2)',
                     texname = '\\text{I100x233}')

I100x234 = Parameter(name = 'I100x234',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x3x2)',
                     texname = '\\text{I100x234}')

I100x241 = Parameter(name = 'I100x241',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x3x1)',
                     texname = '\\text{I100x241}')

I100x242 = Parameter(name = 'I100x242',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x3x1)',
                     texname = '\\text{I100x242}')

I100x243 = Parameter(name = 'I100x243',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x3x1)',
                     texname = '\\text{I100x243}')

I100x244 = Parameter(name = 'I100x244',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x3x1)',
                     texname = '\\text{I100x244}')

I100x311 = Parameter(name = 'I100x311',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x2x3)',
                     texname = '\\text{I100x311}')

I100x312 = Parameter(name = 'I100x312',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x2x3)',
                     texname = '\\text{I100x312}')

I100x313 = Parameter(name = 'I100x313',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x2x3)',
                     texname = '\\text{I100x313}')

I100x314 = Parameter(name = 'I100x314',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x2x3)',
                     texname = '\\text{I100x314}')

I100x321 = Parameter(name = 'I100x321',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x2x3)',
                     texname = '\\text{I100x321}')

I100x322 = Parameter(name = 'I100x322',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x2x3)',
                     texname = '\\text{I100x322}')

I100x323 = Parameter(name = 'I100x323',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x2x3)',
                     texname = '\\text{I100x323}')

I100x324 = Parameter(name = 'I100x324',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x2x3)',
                     texname = '\\text{I100x324}')

I100x341 = Parameter(name = 'I100x341',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x2x1)',
                     texname = '\\text{I100x341}')

I100x342 = Parameter(name = 'I100x342',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x2x1)',
                     texname = '\\text{I100x342}')

I100x343 = Parameter(name = 'I100x343',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x2x1)',
                     texname = '\\text{I100x343}')

I100x344 = Parameter(name = 'I100x344',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x2x1)',
                     texname = '\\text{I100x344}')

I100x411 = Parameter(name = 'I100x411',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x1x3)',
                     texname = '\\text{I100x411}')

I100x412 = Parameter(name = 'I100x412',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x1x3)',
                     texname = '\\text{I100x412}')

I100x413 = Parameter(name = 'I100x413',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x1x3)',
                     texname = '\\text{I100x413}')

I100x414 = Parameter(name = 'I100x414',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x1x3)',
                     texname = '\\text{I100x414}')

I100x421 = Parameter(name = 'I100x421',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x1x3)',
                     texname = '\\text{I100x421}')

I100x422 = Parameter(name = 'I100x422',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x1x3)',
                     texname = '\\text{I100x422}')

I100x423 = Parameter(name = 'I100x423',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x1x3)',
                     texname = '\\text{I100x423}')

I100x424 = Parameter(name = 'I100x424',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x1x3)',
                     texname = '\\text{I100x424}')

I100x431 = Parameter(name = 'I100x431',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x1x2)',
                     texname = '\\text{I100x431}')

I100x432 = Parameter(name = 'I100x432',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x1x2)',
                     texname = '\\text{I100x432}')

I100x433 = Parameter(name = 'I100x433',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x1x2)',
                     texname = '\\text{I100x433}')

I100x434 = Parameter(name = 'I100x434',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x1x2)',
                     texname = '\\text{I100x434}')

I101x131 = Parameter(name = 'I101x131',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x2x3)',
                     texname = '\\text{I101x131}')

I101x132 = Parameter(name = 'I101x132',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x2x3)',
                     texname = '\\text{I101x132}')

I101x133 = Parameter(name = 'I101x133',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x2x3)',
                     texname = '\\text{I101x133}')

I101x134 = Parameter(name = 'I101x134',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x2x3)',
                     texname = '\\text{I101x134}')

I101x141 = Parameter(name = 'I101x141',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x1x3)',
                     texname = '\\text{I101x141}')

I101x142 = Parameter(name = 'I101x142',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x1x3)',
                     texname = '\\text{I101x142}')

I101x143 = Parameter(name = 'I101x143',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x1x3)',
                     texname = '\\text{I101x143}')

I101x144 = Parameter(name = 'I101x144',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x1x3)',
                     texname = '\\text{I101x144}')

I101x231 = Parameter(name = 'I101x231',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x2x3)',
                     texname = '\\text{I101x231}')

I101x232 = Parameter(name = 'I101x232',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x2x3)',
                     texname = '\\text{I101x232}')

I101x233 = Parameter(name = 'I101x233',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x2x3)',
                     texname = '\\text{I101x233}')

I101x234 = Parameter(name = 'I101x234',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x2x3)',
                     texname = '\\text{I101x234}')

I101x241 = Parameter(name = 'I101x241',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x1x3)',
                     texname = '\\text{I101x241}')

I101x242 = Parameter(name = 'I101x242',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x1x3)',
                     texname = '\\text{I101x242}')

I101x243 = Parameter(name = 'I101x243',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x1x3)',
                     texname = '\\text{I101x243}')

I101x244 = Parameter(name = 'I101x244',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x1x3)',
                     texname = '\\text{I101x244}')

I101x311 = Parameter(name = 'I101x311',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x3x2)',
                     texname = '\\text{I101x311}')

I101x312 = Parameter(name = 'I101x312',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x3x2)',
                     texname = '\\text{I101x312}')

I101x313 = Parameter(name = 'I101x313',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x3x2)',
                     texname = '\\text{I101x313}')

I101x314 = Parameter(name = 'I101x314',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x3x2)',
                     texname = '\\text{I101x314}')

I101x321 = Parameter(name = 'I101x321',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x3x2)',
                     texname = '\\text{I101x321}')

I101x322 = Parameter(name = 'I101x322',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x3x2)',
                     texname = '\\text{I101x322}')

I101x323 = Parameter(name = 'I101x323',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x3x2)',
                     texname = '\\text{I101x323}')

I101x324 = Parameter(name = 'I101x324',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x3x2)',
                     texname = '\\text{I101x324}')

I101x341 = Parameter(name = 'I101x341',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x1x2)',
                     texname = '\\text{I101x341}')

I101x342 = Parameter(name = 'I101x342',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x1x2)',
                     texname = '\\text{I101x342}')

I101x343 = Parameter(name = 'I101x343',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x1x2)',
                     texname = '\\text{I101x343}')

I101x344 = Parameter(name = 'I101x344',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x1x2)',
                     texname = '\\text{I101x344}')

I101x411 = Parameter(name = 'I101x411',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x3x1)',
                     texname = '\\text{I101x411}')

I101x412 = Parameter(name = 'I101x412',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x3x1)',
                     texname = '\\text{I101x412}')

I101x413 = Parameter(name = 'I101x413',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x3x1)',
                     texname = '\\text{I101x413}')

I101x414 = Parameter(name = 'I101x414',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x3x1)',
                     texname = '\\text{I101x414}')

I101x421 = Parameter(name = 'I101x421',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x3x1)',
                     texname = '\\text{I101x421}')

I101x422 = Parameter(name = 'I101x422',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x3x1)',
                     texname = '\\text{I101x422}')

I101x423 = Parameter(name = 'I101x423',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x3x1)',
                     texname = '\\text{I101x423}')

I101x424 = Parameter(name = 'I101x424',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x3x1)',
                     texname = '\\text{I101x424}')

I101x431 = Parameter(name = 'I101x431',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)*complexconjugate(TUDD3x2x1)',
                     texname = '\\text{I101x431}')

I101x432 = Parameter(name = 'I101x432',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)*complexconjugate(TUDD3x2x1)',
                     texname = '\\text{I101x432}')

I101x433 = Parameter(name = 'I101x433',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)*complexconjugate(TUDD2x2x1)',
                     texname = '\\text{I101x433}')

I101x434 = Parameter(name = 'I101x434',
                     nature = 'internal',
                     type = 'complex',
                     value = 'complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)*complexconjugate(TUDD1x2x1)',
                     texname = '\\text{I101x434}')

I102x151 = Parameter(name = 'I102x151',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x2x3)*complexconjugate(Rd1x6)*complexconjugate(Rd5x2)*complexconjugate(Ru1x6)',
                     texname = '\\text{I102x151}')

I102x152 = Parameter(name = 'I102x152',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x2x3)*complexconjugate(Rd1x6)*complexconjugate(Rd5x2)*complexconjugate(Ru2x6)',
                     texname = '\\text{I102x152}')

I102x153 = Parameter(name = 'I102x153',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD2x2x3)*complexconjugate(Rd1x6)*complexconjugate(Rd5x2)*complexconjugate(Ru3x5)',
                     texname = '\\text{I102x153}')

I102x154 = Parameter(name = 'I102x154',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD1x2x3)*complexconjugate(Rd1x6)*complexconjugate(Rd5x2)*complexconjugate(Ru4x4)',
                     texname = '\\text{I102x154}')

I102x161 = Parameter(name = 'I102x161',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x1x3)*complexconjugate(Rd1x6)*complexconjugate(Rd6x1)*complexconjugate(Ru1x6)',
                     texname = '\\text{I102x161}')

I102x162 = Parameter(name = 'I102x162',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x1x3)*complexconjugate(Rd1x6)*complexconjugate(Rd6x1)*complexconjugate(Ru2x6)',
                     texname = '\\text{I102x162}')

I102x163 = Parameter(name = 'I102x163',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD2x1x3)*complexconjugate(Rd1x6)*complexconjugate(Rd6x1)*complexconjugate(Ru3x5)',
                     texname = '\\text{I102x163}')

I102x164 = Parameter(name = 'I102x164',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD1x1x3)*complexconjugate(Rd1x6)*complexconjugate(Rd6x1)*complexconjugate(Ru4x4)',
                     texname = '\\text{I102x164}')

I102x251 = Parameter(name = 'I102x251',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x2x3)*complexconjugate(Rd2x6)*complexconjugate(Rd5x2)*complexconjugate(Ru1x6)',
                     texname = '\\text{I102x251}')

I102x252 = Parameter(name = 'I102x252',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x2x3)*complexconjugate(Rd2x6)*complexconjugate(Rd5x2)*complexconjugate(Ru2x6)',
                     texname = '\\text{I102x252}')

I102x253 = Parameter(name = 'I102x253',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD2x2x3)*complexconjugate(Rd2x6)*complexconjugate(Rd5x2)*complexconjugate(Ru3x5)',
                     texname = '\\text{I102x253}')

I102x254 = Parameter(name = 'I102x254',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD1x2x3)*complexconjugate(Rd2x6)*complexconjugate(Rd5x2)*complexconjugate(Ru4x4)',
                     texname = '\\text{I102x254}')

I102x261 = Parameter(name = 'I102x261',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x1x3)*complexconjugate(Rd2x6)*complexconjugate(Rd6x1)*complexconjugate(Ru1x6)',
                     texname = '\\text{I102x261}')

I102x262 = Parameter(name = 'I102x262',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x1x3)*complexconjugate(Rd2x6)*complexconjugate(Rd6x1)*complexconjugate(Ru2x6)',
                     texname = '\\text{I102x262}')

I102x263 = Parameter(name = 'I102x263',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD2x1x3)*complexconjugate(Rd2x6)*complexconjugate(Rd6x1)*complexconjugate(Ru3x5)',
                     texname = '\\text{I102x263}')

I102x264 = Parameter(name = 'I102x264',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD1x1x3)*complexconjugate(Rd2x6)*complexconjugate(Rd6x1)*complexconjugate(Ru4x4)',
                     texname = '\\text{I102x264}')

I102x311 = Parameter(name = 'I102x311',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x3x2)*complexconjugate(Rd1x3)*complexconjugate(Rd3x5)*complexconjugate(Ru1x6)',
                     texname = '\\text{I102x311}')

I102x312 = Parameter(name = 'I102x312',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x3x2)*complexconjugate(Rd1x3)*complexconjugate(Rd3x5)*complexconjugate(Ru2x6)',
                     texname = '\\text{I102x312}')

I102x313 = Parameter(name = 'I102x313',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD2x3x2)*complexconjugate(Rd1x3)*complexconjugate(Rd3x5)*complexconjugate(Ru3x5)',
                     texname = '\\text{I102x313}')

I102x314 = Parameter(name = 'I102x314',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD1x3x2)*complexconjugate(Rd1x3)*complexconjugate(Rd3x5)*complexconjugate(Ru4x4)',
                     texname = '\\text{I102x314}')

I102x321 = Parameter(name = 'I102x321',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x3x2)*complexconjugate(Rd2x3)*complexconjugate(Rd3x5)*complexconjugate(Ru1x6)',
                     texname = '\\text{I102x321}')

I102x322 = Parameter(name = 'I102x322',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x3x2)*complexconjugate(Rd2x3)*complexconjugate(Rd3x5)*complexconjugate(Ru2x6)',
                     texname = '\\text{I102x322}')

I102x323 = Parameter(name = 'I102x323',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD2x3x2)*complexconjugate(Rd2x3)*complexconjugate(Rd3x5)*complexconjugate(Ru3x5)',
                     texname = '\\text{I102x323}')

I102x324 = Parameter(name = 'I102x324',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD1x3x2)*complexconjugate(Rd2x3)*complexconjugate(Rd3x5)*complexconjugate(Ru4x4)',
                     texname = '\\text{I102x324}')

I102x361 = Parameter(name = 'I102x361',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x1x2)*complexconjugate(Rd3x5)*complexconjugate(Rd6x1)*complexconjugate(Ru1x6)',
                     texname = '\\text{I102x361}')

I102x362 = Parameter(name = 'I102x362',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x1x2)*complexconjugate(Rd3x5)*complexconjugate(Rd6x1)*complexconjugate(Ru2x6)',
                     texname = '\\text{I102x362}')

I102x363 = Parameter(name = 'I102x363',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD2x1x2)*complexconjugate(Rd3x5)*complexconjugate(Rd6x1)*complexconjugate(Ru3x5)',
                     texname = '\\text{I102x363}')

I102x364 = Parameter(name = 'I102x364',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD1x1x2)*complexconjugate(Rd3x5)*complexconjugate(Rd6x1)*complexconjugate(Ru4x4)',
                     texname = '\\text{I102x364}')

I102x411 = Parameter(name = 'I102x411',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x3x1)*complexconjugate(Rd1x3)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)',
                     texname = '\\text{I102x411}')

I102x412 = Parameter(name = 'I102x412',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x3x1)*complexconjugate(Rd1x3)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)',
                     texname = '\\text{I102x412}')

I102x413 = Parameter(name = 'I102x413',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD2x3x1)*complexconjugate(Rd1x3)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)',
                     texname = '\\text{I102x413}')

I102x414 = Parameter(name = 'I102x414',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD1x3x1)*complexconjugate(Rd1x3)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)',
                     texname = '\\text{I102x414}')

I102x421 = Parameter(name = 'I102x421',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x3x1)*complexconjugate(Rd2x3)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)',
                     texname = '\\text{I102x421}')

I102x422 = Parameter(name = 'I102x422',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x3x1)*complexconjugate(Rd2x3)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)',
                     texname = '\\text{I102x422}')

I102x423 = Parameter(name = 'I102x423',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD2x3x1)*complexconjugate(Rd2x3)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)',
                     texname = '\\text{I102x423}')

I102x424 = Parameter(name = 'I102x424',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD1x3x1)*complexconjugate(Rd2x3)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)',
                     texname = '\\text{I102x424}')

I102x451 = Parameter(name = 'I102x451',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x2x1)*complexconjugate(Rd4x4)*complexconjugate(Rd5x2)*complexconjugate(Ru1x6)',
                     texname = '\\text{I102x451}')

I102x452 = Parameter(name = 'I102x452',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x2x1)*complexconjugate(Rd4x4)*complexconjugate(Rd5x2)*complexconjugate(Ru2x6)',
                     texname = '\\text{I102x452}')

I102x453 = Parameter(name = 'I102x453',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD2x2x1)*complexconjugate(Rd4x4)*complexconjugate(Rd5x2)*complexconjugate(Ru3x5)',
                     texname = '\\text{I102x453}')

I102x454 = Parameter(name = 'I102x454',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD1x2x1)*complexconjugate(Rd4x4)*complexconjugate(Rd5x2)*complexconjugate(Ru4x4)',
                     texname = '\\text{I102x454}')

I103x131 = Parameter(name = 'I103x131',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x3x2)*complexconjugate(Rd1x3)*complexconjugate(Rd3x5)*complexconjugate(Ru1x6)',
                     texname = '\\text{I103x131}')

I103x132 = Parameter(name = 'I103x132',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x3x2)*complexconjugate(Rd1x3)*complexconjugate(Rd3x5)*complexconjugate(Ru2x6)',
                     texname = '\\text{I103x132}')

I103x133 = Parameter(name = 'I103x133',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD2x3x2)*complexconjugate(Rd1x3)*complexconjugate(Rd3x5)*complexconjugate(Ru3x5)',
                     texname = '\\text{I103x133}')

I103x134 = Parameter(name = 'I103x134',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD1x3x2)*complexconjugate(Rd1x3)*complexconjugate(Rd3x5)*complexconjugate(Ru4x4)',
                     texname = '\\text{I103x134}')

I103x141 = Parameter(name = 'I103x141',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x3x1)*complexconjugate(Rd1x3)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)',
                     texname = '\\text{I103x141}')

I103x142 = Parameter(name = 'I103x142',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x3x1)*complexconjugate(Rd1x3)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)',
                     texname = '\\text{I103x142}')

I103x143 = Parameter(name = 'I103x143',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD2x3x1)*complexconjugate(Rd1x3)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)',
                     texname = '\\text{I103x143}')

I103x144 = Parameter(name = 'I103x144',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD1x3x1)*complexconjugate(Rd1x3)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)',
                     texname = '\\text{I103x144}')

I103x231 = Parameter(name = 'I103x231',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x3x2)*complexconjugate(Rd2x3)*complexconjugate(Rd3x5)*complexconjugate(Ru1x6)',
                     texname = '\\text{I103x231}')

I103x232 = Parameter(name = 'I103x232',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x3x2)*complexconjugate(Rd2x3)*complexconjugate(Rd3x5)*complexconjugate(Ru2x6)',
                     texname = '\\text{I103x232}')

I103x233 = Parameter(name = 'I103x233',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD2x3x2)*complexconjugate(Rd2x3)*complexconjugate(Rd3x5)*complexconjugate(Ru3x5)',
                     texname = '\\text{I103x233}')

I103x234 = Parameter(name = 'I103x234',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD1x3x2)*complexconjugate(Rd2x3)*complexconjugate(Rd3x5)*complexconjugate(Ru4x4)',
                     texname = '\\text{I103x234}')

I103x241 = Parameter(name = 'I103x241',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x3x1)*complexconjugate(Rd2x3)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)',
                     texname = '\\text{I103x241}')

I103x242 = Parameter(name = 'I103x242',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x3x1)*complexconjugate(Rd2x3)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)',
                     texname = '\\text{I103x242}')

I103x243 = Parameter(name = 'I103x243',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD2x3x1)*complexconjugate(Rd2x3)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)',
                     texname = '\\text{I103x243}')

I103x244 = Parameter(name = 'I103x244',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD1x3x1)*complexconjugate(Rd2x3)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)',
                     texname = '\\text{I103x244}')

I103x511 = Parameter(name = 'I103x511',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x2x3)*complexconjugate(Rd1x6)*complexconjugate(Rd5x2)*complexconjugate(Ru1x6)',
                     texname = '\\text{I103x511}')

I103x512 = Parameter(name = 'I103x512',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x2x3)*complexconjugate(Rd1x6)*complexconjugate(Rd5x2)*complexconjugate(Ru2x6)',
                     texname = '\\text{I103x512}')

I103x513 = Parameter(name = 'I103x513',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD2x2x3)*complexconjugate(Rd1x6)*complexconjugate(Rd5x2)*complexconjugate(Ru3x5)',
                     texname = '\\text{I103x513}')

I103x514 = Parameter(name = 'I103x514',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD1x2x3)*complexconjugate(Rd1x6)*complexconjugate(Rd5x2)*complexconjugate(Ru4x4)',
                     texname = '\\text{I103x514}')

I103x521 = Parameter(name = 'I103x521',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x2x3)*complexconjugate(Rd2x6)*complexconjugate(Rd5x2)*complexconjugate(Ru1x6)',
                     texname = '\\text{I103x521}')

I103x522 = Parameter(name = 'I103x522',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x2x3)*complexconjugate(Rd2x6)*complexconjugate(Rd5x2)*complexconjugate(Ru2x6)',
                     texname = '\\text{I103x522}')

I103x523 = Parameter(name = 'I103x523',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD2x2x3)*complexconjugate(Rd2x6)*complexconjugate(Rd5x2)*complexconjugate(Ru3x5)',
                     texname = '\\text{I103x523}')

I103x524 = Parameter(name = 'I103x524',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD1x2x3)*complexconjugate(Rd2x6)*complexconjugate(Rd5x2)*complexconjugate(Ru4x4)',
                     texname = '\\text{I103x524}')

I103x541 = Parameter(name = 'I103x541',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x2x1)*complexconjugate(Rd4x4)*complexconjugate(Rd5x2)*complexconjugate(Ru1x6)',
                     texname = '\\text{I103x541}')

I103x542 = Parameter(name = 'I103x542',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x2x1)*complexconjugate(Rd4x4)*complexconjugate(Rd5x2)*complexconjugate(Ru2x6)',
                     texname = '\\text{I103x542}')

I103x543 = Parameter(name = 'I103x543',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD2x2x1)*complexconjugate(Rd4x4)*complexconjugate(Rd5x2)*complexconjugate(Ru3x5)',
                     texname = '\\text{I103x543}')

I103x544 = Parameter(name = 'I103x544',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD1x2x1)*complexconjugate(Rd4x4)*complexconjugate(Rd5x2)*complexconjugate(Ru4x4)',
                     texname = '\\text{I103x544}')

I103x611 = Parameter(name = 'I103x611',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x1x3)*complexconjugate(Rd1x6)*complexconjugate(Rd6x1)*complexconjugate(Ru1x6)',
                     texname = '\\text{I103x611}')

I103x612 = Parameter(name = 'I103x612',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x1x3)*complexconjugate(Rd1x6)*complexconjugate(Rd6x1)*complexconjugate(Ru2x6)',
                     texname = '\\text{I103x612}')

I103x613 = Parameter(name = 'I103x613',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD2x1x3)*complexconjugate(Rd1x6)*complexconjugate(Rd6x1)*complexconjugate(Ru3x5)',
                     texname = '\\text{I103x613}')

I103x614 = Parameter(name = 'I103x614',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD1x1x3)*complexconjugate(Rd1x6)*complexconjugate(Rd6x1)*complexconjugate(Ru4x4)',
                     texname = '\\text{I103x614}')

I103x621 = Parameter(name = 'I103x621',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x1x3)*complexconjugate(Rd2x6)*complexconjugate(Rd6x1)*complexconjugate(Ru1x6)',
                     texname = '\\text{I103x621}')

I103x622 = Parameter(name = 'I103x622',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x1x3)*complexconjugate(Rd2x6)*complexconjugate(Rd6x1)*complexconjugate(Ru2x6)',
                     texname = '\\text{I103x622}')

I103x623 = Parameter(name = 'I103x623',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD2x1x3)*complexconjugate(Rd2x6)*complexconjugate(Rd6x1)*complexconjugate(Ru3x5)',
                     texname = '\\text{I103x623}')

I103x624 = Parameter(name = 'I103x624',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD1x1x3)*complexconjugate(Rd2x6)*complexconjugate(Rd6x1)*complexconjugate(Ru4x4)',
                     texname = '\\text{I103x624}')

I103x631 = Parameter(name = 'I103x631',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x1x2)*complexconjugate(Rd3x5)*complexconjugate(Rd6x1)*complexconjugate(Ru1x6)',
                     texname = '\\text{I103x631}')

I103x632 = Parameter(name = 'I103x632',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x1x2)*complexconjugate(Rd3x5)*complexconjugate(Rd6x1)*complexconjugate(Ru2x6)',
                     texname = '\\text{I103x632}')

I103x633 = Parameter(name = 'I103x633',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD2x1x2)*complexconjugate(Rd3x5)*complexconjugate(Rd6x1)*complexconjugate(Ru3x5)',
                     texname = '\\text{I103x633}')

I103x634 = Parameter(name = 'I103x634',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD1x1x2)*complexconjugate(Rd3x5)*complexconjugate(Rd6x1)*complexconjugate(Ru4x4)',
                     texname = '\\text{I103x634}')

I104x151 = Parameter(name = 'I104x151',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x3x2)*complexconjugate(Rd1x6)*complexconjugate(Rd5x2)*complexconjugate(Ru1x6)',
                     texname = '\\text{I104x151}')

I104x152 = Parameter(name = 'I104x152',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x3x2)*complexconjugate(Rd1x6)*complexconjugate(Rd5x2)*complexconjugate(Ru2x6)',
                     texname = '\\text{I104x152}')

I104x153 = Parameter(name = 'I104x153',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD2x3x2)*complexconjugate(Rd1x6)*complexconjugate(Rd5x2)*complexconjugate(Ru3x5)',
                     texname = '\\text{I104x153}')

I104x154 = Parameter(name = 'I104x154',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD1x3x2)*complexconjugate(Rd1x6)*complexconjugate(Rd5x2)*complexconjugate(Ru4x4)',
                     texname = '\\text{I104x154}')

I104x161 = Parameter(name = 'I104x161',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x3x1)*complexconjugate(Rd1x6)*complexconjugate(Rd6x1)*complexconjugate(Ru1x6)',
                     texname = '\\text{I104x161}')

I104x162 = Parameter(name = 'I104x162',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x3x1)*complexconjugate(Rd1x6)*complexconjugate(Rd6x1)*complexconjugate(Ru2x6)',
                     texname = '\\text{I104x162}')

I104x163 = Parameter(name = 'I104x163',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD2x3x1)*complexconjugate(Rd1x6)*complexconjugate(Rd6x1)*complexconjugate(Ru3x5)',
                     texname = '\\text{I104x163}')

I104x164 = Parameter(name = 'I104x164',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD1x3x1)*complexconjugate(Rd1x6)*complexconjugate(Rd6x1)*complexconjugate(Ru4x4)',
                     texname = '\\text{I104x164}')

I104x251 = Parameter(name = 'I104x251',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x3x2)*complexconjugate(Rd2x6)*complexconjugate(Rd5x2)*complexconjugate(Ru1x6)',
                     texname = '\\text{I104x251}')

I104x252 = Parameter(name = 'I104x252',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x3x2)*complexconjugate(Rd2x6)*complexconjugate(Rd5x2)*complexconjugate(Ru2x6)',
                     texname = '\\text{I104x252}')

I104x253 = Parameter(name = 'I104x253',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD2x3x2)*complexconjugate(Rd2x6)*complexconjugate(Rd5x2)*complexconjugate(Ru3x5)',
                     texname = '\\text{I104x253}')

I104x254 = Parameter(name = 'I104x254',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD1x3x2)*complexconjugate(Rd2x6)*complexconjugate(Rd5x2)*complexconjugate(Ru4x4)',
                     texname = '\\text{I104x254}')

I104x261 = Parameter(name = 'I104x261',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x3x1)*complexconjugate(Rd2x6)*complexconjugate(Rd6x1)*complexconjugate(Ru1x6)',
                     texname = '\\text{I104x261}')

I104x262 = Parameter(name = 'I104x262',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x3x1)*complexconjugate(Rd2x6)*complexconjugate(Rd6x1)*complexconjugate(Ru2x6)',
                     texname = '\\text{I104x262}')

I104x263 = Parameter(name = 'I104x263',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD2x3x1)*complexconjugate(Rd2x6)*complexconjugate(Rd6x1)*complexconjugate(Ru3x5)',
                     texname = '\\text{I104x263}')

I104x264 = Parameter(name = 'I104x264',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD1x3x1)*complexconjugate(Rd2x6)*complexconjugate(Rd6x1)*complexconjugate(Ru4x4)',
                     texname = '\\text{I104x264}')

I104x311 = Parameter(name = 'I104x311',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x2x3)*complexconjugate(Rd1x3)*complexconjugate(Rd3x5)*complexconjugate(Ru1x6)',
                     texname = '\\text{I104x311}')

I104x312 = Parameter(name = 'I104x312',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x2x3)*complexconjugate(Rd1x3)*complexconjugate(Rd3x5)*complexconjugate(Ru2x6)',
                     texname = '\\text{I104x312}')

I104x313 = Parameter(name = 'I104x313',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD2x2x3)*complexconjugate(Rd1x3)*complexconjugate(Rd3x5)*complexconjugate(Ru3x5)',
                     texname = '\\text{I104x313}')

I104x314 = Parameter(name = 'I104x314',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD1x2x3)*complexconjugate(Rd1x3)*complexconjugate(Rd3x5)*complexconjugate(Ru4x4)',
                     texname = '\\text{I104x314}')

I104x321 = Parameter(name = 'I104x321',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x2x3)*complexconjugate(Rd2x3)*complexconjugate(Rd3x5)*complexconjugate(Ru1x6)',
                     texname = '\\text{I104x321}')

I104x322 = Parameter(name = 'I104x322',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x2x3)*complexconjugate(Rd2x3)*complexconjugate(Rd3x5)*complexconjugate(Ru2x6)',
                     texname = '\\text{I104x322}')

I104x323 = Parameter(name = 'I104x323',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD2x2x3)*complexconjugate(Rd2x3)*complexconjugate(Rd3x5)*complexconjugate(Ru3x5)',
                     texname = '\\text{I104x323}')

I104x324 = Parameter(name = 'I104x324',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD1x2x3)*complexconjugate(Rd2x3)*complexconjugate(Rd3x5)*complexconjugate(Ru4x4)',
                     texname = '\\text{I104x324}')

I104x361 = Parameter(name = 'I104x361',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x2x1)*complexconjugate(Rd3x5)*complexconjugate(Rd6x1)*complexconjugate(Ru1x6)',
                     texname = '\\text{I104x361}')

I104x362 = Parameter(name = 'I104x362',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x2x1)*complexconjugate(Rd3x5)*complexconjugate(Rd6x1)*complexconjugate(Ru2x6)',
                     texname = '\\text{I104x362}')

I104x363 = Parameter(name = 'I104x363',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD2x2x1)*complexconjugate(Rd3x5)*complexconjugate(Rd6x1)*complexconjugate(Ru3x5)',
                     texname = '\\text{I104x363}')

I104x364 = Parameter(name = 'I104x364',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD1x2x1)*complexconjugate(Rd3x5)*complexconjugate(Rd6x1)*complexconjugate(Ru4x4)',
                     texname = '\\text{I104x364}')

I104x411 = Parameter(name = 'I104x411',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x1x3)*complexconjugate(Rd1x3)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)',
                     texname = '\\text{I104x411}')

I104x412 = Parameter(name = 'I104x412',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x1x3)*complexconjugate(Rd1x3)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)',
                     texname = '\\text{I104x412}')

I104x413 = Parameter(name = 'I104x413',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD2x1x3)*complexconjugate(Rd1x3)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)',
                     texname = '\\text{I104x413}')

I104x414 = Parameter(name = 'I104x414',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD1x1x3)*complexconjugate(Rd1x3)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)',
                     texname = '\\text{I104x414}')

I104x421 = Parameter(name = 'I104x421',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x1x3)*complexconjugate(Rd2x3)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)',
                     texname = '\\text{I104x421}')

I104x422 = Parameter(name = 'I104x422',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x1x3)*complexconjugate(Rd2x3)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)',
                     texname = '\\text{I104x422}')

I104x423 = Parameter(name = 'I104x423',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD2x1x3)*complexconjugate(Rd2x3)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)',
                     texname = '\\text{I104x423}')

I104x424 = Parameter(name = 'I104x424',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD1x1x3)*complexconjugate(Rd2x3)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)',
                     texname = '\\text{I104x424}')

I104x451 = Parameter(name = 'I104x451',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x1x2)*complexconjugate(Rd4x4)*complexconjugate(Rd5x2)*complexconjugate(Ru1x6)',
                     texname = '\\text{I104x451}')

I104x452 = Parameter(name = 'I104x452',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x1x2)*complexconjugate(Rd4x4)*complexconjugate(Rd5x2)*complexconjugate(Ru2x6)',
                     texname = '\\text{I104x452}')

I104x453 = Parameter(name = 'I104x453',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD2x1x2)*complexconjugate(Rd4x4)*complexconjugate(Rd5x2)*complexconjugate(Ru3x5)',
                     texname = '\\text{I104x453}')

I104x454 = Parameter(name = 'I104x454',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD1x1x2)*complexconjugate(Rd4x4)*complexconjugate(Rd5x2)*complexconjugate(Ru4x4)',
                     texname = '\\text{I104x454}')

I105x131 = Parameter(name = 'I105x131',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x2x3)*complexconjugate(Rd1x3)*complexconjugate(Rd3x5)*complexconjugate(Ru1x6)',
                     texname = '\\text{I105x131}')

I105x132 = Parameter(name = 'I105x132',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x2x3)*complexconjugate(Rd1x3)*complexconjugate(Rd3x5)*complexconjugate(Ru2x6)',
                     texname = '\\text{I105x132}')

I105x133 = Parameter(name = 'I105x133',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD2x2x3)*complexconjugate(Rd1x3)*complexconjugate(Rd3x5)*complexconjugate(Ru3x5)',
                     texname = '\\text{I105x133}')

I105x134 = Parameter(name = 'I105x134',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD1x2x3)*complexconjugate(Rd1x3)*complexconjugate(Rd3x5)*complexconjugate(Ru4x4)',
                     texname = '\\text{I105x134}')

I105x141 = Parameter(name = 'I105x141',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x1x3)*complexconjugate(Rd1x3)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)',
                     texname = '\\text{I105x141}')

I105x142 = Parameter(name = 'I105x142',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x1x3)*complexconjugate(Rd1x3)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)',
                     texname = '\\text{I105x142}')

I105x143 = Parameter(name = 'I105x143',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD2x1x3)*complexconjugate(Rd1x3)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)',
                     texname = '\\text{I105x143}')

I105x144 = Parameter(name = 'I105x144',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD1x1x3)*complexconjugate(Rd1x3)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)',
                     texname = '\\text{I105x144}')

I105x231 = Parameter(name = 'I105x231',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x2x3)*complexconjugate(Rd2x3)*complexconjugate(Rd3x5)*complexconjugate(Ru1x6)',
                     texname = '\\text{I105x231}')

I105x232 = Parameter(name = 'I105x232',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x2x3)*complexconjugate(Rd2x3)*complexconjugate(Rd3x5)*complexconjugate(Ru2x6)',
                     texname = '\\text{I105x232}')

I105x233 = Parameter(name = 'I105x233',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD2x2x3)*complexconjugate(Rd2x3)*complexconjugate(Rd3x5)*complexconjugate(Ru3x5)',
                     texname = '\\text{I105x233}')

I105x234 = Parameter(name = 'I105x234',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD1x2x3)*complexconjugate(Rd2x3)*complexconjugate(Rd3x5)*complexconjugate(Ru4x4)',
                     texname = '\\text{I105x234}')

I105x241 = Parameter(name = 'I105x241',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x1x3)*complexconjugate(Rd2x3)*complexconjugate(Rd4x4)*complexconjugate(Ru1x6)',
                     texname = '\\text{I105x241}')

I105x242 = Parameter(name = 'I105x242',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD3x1x3)*complexconjugate(Rd2x3)*complexconjugate(Rd4x4)*complexconjugate(Ru2x6)',
                     texname = '\\text{I105x242}')

I105x243 = Parameter(name = 'I105x243',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD2x1x3)*complexconjugate(Rd2x3)*complexconjugate(Rd4x4)*complexconjugate(Ru3x5)',
                     texname = '\\text{I105x243}')

I105x244 = Parameter(name = 'I105x244',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd3x3*complexconjugate(LUDD1x1x3)*complexconjugate(Rd2x3)*complexconjugate(Rd4x4)*complexconjugate(Ru4x4)',
                     texname = '\\text{I105x244}')

I105x511 = Parameter(name = 'I105x511',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x3x2)*complexconjugate(Rd1x6)*complexconjugate(Rd5x2)*complexconjugate(Ru1x6)',
                     texname = '\\text{I105x511}')

I105x512 = Parameter(name = 'I105x512',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x3x2)*complexconjugate(Rd1x6)*complexconjugate(Rd5x2)*complexconjugate(Ru2x6)',
                     texname = '\\text{I105x512}')

I105x513 = Parameter(name = 'I105x513',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD2x3x2)*complexconjugate(Rd1x6)*complexconjugate(Rd5x2)*complexconjugate(Ru3x5)',
                     texname = '\\text{I105x513}')

I105x514 = Parameter(name = 'I105x514',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD1x3x2)*complexconjugate(Rd1x6)*complexconjugate(Rd5x2)*complexconjugate(Ru4x4)',
                     texname = '\\text{I105x514}')

I105x521 = Parameter(name = 'I105x521',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x3x2)*complexconjugate(Rd2x6)*complexconjugate(Rd5x2)*complexconjugate(Ru1x6)',
                     texname = '\\text{I105x521}')

I105x522 = Parameter(name = 'I105x522',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x3x2)*complexconjugate(Rd2x6)*complexconjugate(Rd5x2)*complexconjugate(Ru2x6)',
                     texname = '\\text{I105x522}')

I105x523 = Parameter(name = 'I105x523',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD2x3x2)*complexconjugate(Rd2x6)*complexconjugate(Rd5x2)*complexconjugate(Ru3x5)',
                     texname = '\\text{I105x523}')

I105x524 = Parameter(name = 'I105x524',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD1x3x2)*complexconjugate(Rd2x6)*complexconjugate(Rd5x2)*complexconjugate(Ru4x4)',
                     texname = '\\text{I105x524}')

I105x541 = Parameter(name = 'I105x541',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x1x2)*complexconjugate(Rd4x4)*complexconjugate(Rd5x2)*complexconjugate(Ru1x6)',
                     texname = '\\text{I105x541}')

I105x542 = Parameter(name = 'I105x542',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD3x1x2)*complexconjugate(Rd4x4)*complexconjugate(Rd5x2)*complexconjugate(Ru2x6)',
                     texname = '\\text{I105x542}')

I105x543 = Parameter(name = 'I105x543',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD2x1x2)*complexconjugate(Rd4x4)*complexconjugate(Rd5x2)*complexconjugate(Ru3x5)',
                     texname = '\\text{I105x543}')

I105x544 = Parameter(name = 'I105x544',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd2x2*complexconjugate(LUDD1x1x2)*complexconjugate(Rd4x4)*complexconjugate(Rd5x2)*complexconjugate(Ru4x4)',
                     texname = '\\text{I105x544}')

I105x611 = Parameter(name = 'I105x611',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x3x1)*complexconjugate(Rd1x6)*complexconjugate(Rd6x1)*complexconjugate(Ru1x6)',
                     texname = '\\text{I105x611}')

I105x612 = Parameter(name = 'I105x612',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x3x1)*complexconjugate(Rd1x6)*complexconjugate(Rd6x1)*complexconjugate(Ru2x6)',
                     texname = '\\text{I105x612}')

I105x613 = Parameter(name = 'I105x613',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD2x3x1)*complexconjugate(Rd1x6)*complexconjugate(Rd6x1)*complexconjugate(Ru3x5)',
                     texname = '\\text{I105x613}')

I105x614 = Parameter(name = 'I105x614',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD1x3x1)*complexconjugate(Rd1x6)*complexconjugate(Rd6x1)*complexconjugate(Ru4x4)',
                     texname = '\\text{I105x614}')

I105x621 = Parameter(name = 'I105x621',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x3x1)*complexconjugate(Rd2x6)*complexconjugate(Rd6x1)*complexconjugate(Ru1x6)',
                     texname = '\\text{I105x621}')

I105x622 = Parameter(name = 'I105x622',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x3x1)*complexconjugate(Rd2x6)*complexconjugate(Rd6x1)*complexconjugate(Ru2x6)',
                     texname = '\\text{I105x622}')

I105x623 = Parameter(name = 'I105x623',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD2x3x1)*complexconjugate(Rd2x6)*complexconjugate(Rd6x1)*complexconjugate(Ru3x5)',
                     texname = '\\text{I105x623}')

I105x624 = Parameter(name = 'I105x624',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD1x3x1)*complexconjugate(Rd2x6)*complexconjugate(Rd6x1)*complexconjugate(Ru4x4)',
                     texname = '\\text{I105x624}')

I105x631 = Parameter(name = 'I105x631',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x2x1)*complexconjugate(Rd3x5)*complexconjugate(Rd6x1)*complexconjugate(Ru1x6)',
                     texname = '\\text{I105x631}')

I105x632 = Parameter(name = 'I105x632',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD3x2x1)*complexconjugate(Rd3x5)*complexconjugate(Rd6x1)*complexconjugate(Ru2x6)',
                     texname = '\\text{I105x632}')

I105x633 = Parameter(name = 'I105x633',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD2x2x1)*complexconjugate(Rd3x5)*complexconjugate(Rd6x1)*complexconjugate(Ru3x5)',
                     texname = '\\text{I105x633}')

I105x634 = Parameter(name = 'I105x634',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yd1x1*complexconjugate(LUDD1x2x1)*complexconjugate(Rd3x5)*complexconjugate(Rd6x1)*complexconjugate(Ru4x4)',
                     texname = '\\text{I105x634}')

I106x131 = Parameter(name = 'I106x131',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x2x3)*complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru1x3)',
                     texname = '\\text{I106x131}')

I106x132 = Parameter(name = 'I106x132',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x2x3)*complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru2x3)',
                     texname = '\\text{I106x132}')

I106x135 = Parameter(name = 'I106x135',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x2x3)*complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru5x1)',
                     texname = '\\text{I106x135}')

I106x136 = Parameter(name = 'I106x136',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x2x3)*complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru6x2)',
                     texname = '\\text{I106x136}')

I106x141 = Parameter(name = 'I106x141',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x1x3)*complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru1x3)',
                     texname = '\\text{I106x141}')

I106x142 = Parameter(name = 'I106x142',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x1x3)*complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru2x3)',
                     texname = '\\text{I106x142}')

I106x145 = Parameter(name = 'I106x145',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x1x3)*complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru5x1)',
                     texname = '\\text{I106x145}')

I106x146 = Parameter(name = 'I106x146',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x1x3)*complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru6x2)',
                     texname = '\\text{I106x146}')

I106x231 = Parameter(name = 'I106x231',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x2x3)*complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru1x3)',
                     texname = '\\text{I106x231}')

I106x232 = Parameter(name = 'I106x232',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x2x3)*complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru2x3)',
                     texname = '\\text{I106x232}')

I106x235 = Parameter(name = 'I106x235',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x2x3)*complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru5x1)',
                     texname = '\\text{I106x235}')

I106x236 = Parameter(name = 'I106x236',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x2x3)*complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru6x2)',
                     texname = '\\text{I106x236}')

I106x241 = Parameter(name = 'I106x241',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x1x3)*complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru1x3)',
                     texname = '\\text{I106x241}')

I106x242 = Parameter(name = 'I106x242',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x1x3)*complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru2x3)',
                     texname = '\\text{I106x242}')

I106x245 = Parameter(name = 'I106x245',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x1x3)*complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru5x1)',
                     texname = '\\text{I106x245}')

I106x246 = Parameter(name = 'I106x246',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x1x3)*complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru6x2)',
                     texname = '\\text{I106x246}')

I106x311 = Parameter(name = 'I106x311',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x3x2)*complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru1x3)',
                     texname = '\\text{I106x311}')

I106x312 = Parameter(name = 'I106x312',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x3x2)*complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru2x3)',
                     texname = '\\text{I106x312}')

I106x315 = Parameter(name = 'I106x315',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x3x2)*complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru5x1)',
                     texname = '\\text{I106x315}')

I106x316 = Parameter(name = 'I106x316',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x3x2)*complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru6x2)',
                     texname = '\\text{I106x316}')

I106x321 = Parameter(name = 'I106x321',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x3x2)*complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru1x3)',
                     texname = '\\text{I106x321}')

I106x322 = Parameter(name = 'I106x322',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x3x2)*complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru2x3)',
                     texname = '\\text{I106x322}')

I106x325 = Parameter(name = 'I106x325',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x3x2)*complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru5x1)',
                     texname = '\\text{I106x325}')

I106x326 = Parameter(name = 'I106x326',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x3x2)*complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru6x2)',
                     texname = '\\text{I106x326}')

I106x341 = Parameter(name = 'I106x341',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x1x2)*complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru1x3)',
                     texname = '\\text{I106x341}')

I106x342 = Parameter(name = 'I106x342',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x1x2)*complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru2x3)',
                     texname = '\\text{I106x342}')

I106x345 = Parameter(name = 'I106x345',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x1x2)*complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru5x1)',
                     texname = '\\text{I106x345}')

I106x346 = Parameter(name = 'I106x346',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x1x2)*complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru6x2)',
                     texname = '\\text{I106x346}')

I106x411 = Parameter(name = 'I106x411',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x3x1)*complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru1x3)',
                     texname = '\\text{I106x411}')

I106x412 = Parameter(name = 'I106x412',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x3x1)*complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru2x3)',
                     texname = '\\text{I106x412}')

I106x415 = Parameter(name = 'I106x415',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x3x1)*complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru5x1)',
                     texname = '\\text{I106x415}')

I106x416 = Parameter(name = 'I106x416',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x3x1)*complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru6x2)',
                     texname = '\\text{I106x416}')

I106x421 = Parameter(name = 'I106x421',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x3x1)*complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru1x3)',
                     texname = '\\text{I106x421}')

I106x422 = Parameter(name = 'I106x422',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x3x1)*complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru2x3)',
                     texname = '\\text{I106x422}')

I106x425 = Parameter(name = 'I106x425',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x3x1)*complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru5x1)',
                     texname = '\\text{I106x425}')

I106x426 = Parameter(name = 'I106x426',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x3x1)*complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru6x2)',
                     texname = '\\text{I106x426}')

I106x431 = Parameter(name = 'I106x431',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x2x1)*complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru1x3)',
                     texname = '\\text{I106x431}')

I106x432 = Parameter(name = 'I106x432',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x2x1)*complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru2x3)',
                     texname = '\\text{I106x432}')

I106x435 = Parameter(name = 'I106x435',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x2x1)*complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru5x1)',
                     texname = '\\text{I106x435}')

I106x436 = Parameter(name = 'I106x436',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x2x1)*complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru6x2)',
                     texname = '\\text{I106x436}')

I107x131 = Parameter(name = 'I107x131',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x3x2)*complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru1x3)',
                     texname = '\\text{I107x131}')

I107x132 = Parameter(name = 'I107x132',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x3x2)*complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru2x3)',
                     texname = '\\text{I107x132}')

I107x135 = Parameter(name = 'I107x135',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x3x2)*complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru5x1)',
                     texname = '\\text{I107x135}')

I107x136 = Parameter(name = 'I107x136',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x3x2)*complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru6x2)',
                     texname = '\\text{I107x136}')

I107x141 = Parameter(name = 'I107x141',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x3x1)*complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru1x3)',
                     texname = '\\text{I107x141}')

I107x142 = Parameter(name = 'I107x142',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x3x1)*complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru2x3)',
                     texname = '\\text{I107x142}')

I107x145 = Parameter(name = 'I107x145',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x3x1)*complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru5x1)',
                     texname = '\\text{I107x145}')

I107x146 = Parameter(name = 'I107x146',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x3x1)*complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru6x2)',
                     texname = '\\text{I107x146}')

I107x231 = Parameter(name = 'I107x231',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x3x2)*complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru1x3)',
                     texname = '\\text{I107x231}')

I107x232 = Parameter(name = 'I107x232',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x3x2)*complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru2x3)',
                     texname = '\\text{I107x232}')

I107x235 = Parameter(name = 'I107x235',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x3x2)*complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru5x1)',
                     texname = '\\text{I107x235}')

I107x236 = Parameter(name = 'I107x236',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x3x2)*complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru6x2)',
                     texname = '\\text{I107x236}')

I107x241 = Parameter(name = 'I107x241',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x3x1)*complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru1x3)',
                     texname = '\\text{I107x241}')

I107x242 = Parameter(name = 'I107x242',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x3x1)*complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru2x3)',
                     texname = '\\text{I107x242}')

I107x245 = Parameter(name = 'I107x245',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x3x1)*complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru5x1)',
                     texname = '\\text{I107x245}')

I107x246 = Parameter(name = 'I107x246',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x3x1)*complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru6x2)',
                     texname = '\\text{I107x246}')

I107x311 = Parameter(name = 'I107x311',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x2x3)*complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru1x3)',
                     texname = '\\text{I107x311}')

I107x312 = Parameter(name = 'I107x312',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x2x3)*complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru2x3)',
                     texname = '\\text{I107x312}')

I107x315 = Parameter(name = 'I107x315',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x2x3)*complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru5x1)',
                     texname = '\\text{I107x315}')

I107x316 = Parameter(name = 'I107x316',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x2x3)*complexconjugate(Rd1x6)*complexconjugate(Rd3x5)*complexconjugate(Ru6x2)',
                     texname = '\\text{I107x316}')

I107x321 = Parameter(name = 'I107x321',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x2x3)*complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru1x3)',
                     texname = '\\text{I107x321}')

I107x322 = Parameter(name = 'I107x322',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x2x3)*complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru2x3)',
                     texname = '\\text{I107x322}')

I107x325 = Parameter(name = 'I107x325',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x2x3)*complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru5x1)',
                     texname = '\\text{I107x325}')

I107x326 = Parameter(name = 'I107x326',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x2x3)*complexconjugate(Rd2x6)*complexconjugate(Rd3x5)*complexconjugate(Ru6x2)',
                     texname = '\\text{I107x326}')

I107x341 = Parameter(name = 'I107x341',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x2x1)*complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru1x3)',
                     texname = '\\text{I107x341}')

I107x342 = Parameter(name = 'I107x342',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x2x1)*complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru2x3)',
                     texname = '\\text{I107x342}')

I107x345 = Parameter(name = 'I107x345',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x2x1)*complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru5x1)',
                     texname = '\\text{I107x345}')

I107x346 = Parameter(name = 'I107x346',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x2x1)*complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru6x2)',
                     texname = '\\text{I107x346}')

I107x411 = Parameter(name = 'I107x411',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x1x3)*complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru1x3)',
                     texname = '\\text{I107x411}')

I107x412 = Parameter(name = 'I107x412',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x1x3)*complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru2x3)',
                     texname = '\\text{I107x412}')

I107x415 = Parameter(name = 'I107x415',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x1x3)*complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru5x1)',
                     texname = '\\text{I107x415}')

I107x416 = Parameter(name = 'I107x416',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x1x3)*complexconjugate(Rd1x6)*complexconjugate(Rd4x4)*complexconjugate(Ru6x2)',
                     texname = '\\text{I107x416}')

I107x421 = Parameter(name = 'I107x421',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x1x3)*complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru1x3)',
                     texname = '\\text{I107x421}')

I107x422 = Parameter(name = 'I107x422',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x1x3)*complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru2x3)',
                     texname = '\\text{I107x422}')

I107x425 = Parameter(name = 'I107x425',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x1x3)*complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru5x1)',
                     texname = '\\text{I107x425}')

I107x426 = Parameter(name = 'I107x426',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x1x3)*complexconjugate(Rd2x6)*complexconjugate(Rd4x4)*complexconjugate(Ru6x2)',
                     texname = '\\text{I107x426}')

I107x431 = Parameter(name = 'I107x431',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x1x2)*complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru1x3)',
                     texname = '\\text{I107x431}')

I107x432 = Parameter(name = 'I107x432',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu3x3*complexconjugate(LUDD3x1x2)*complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru2x3)',
                     texname = '\\text{I107x432}')

I107x435 = Parameter(name = 'I107x435',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu1x1*complexconjugate(LUDD1x1x2)*complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru5x1)',
                     texname = '\\text{I107x435}')

I107x436 = Parameter(name = 'I107x436',
                     nature = 'internal',
                     type = 'complex',
                     value = 'yu2x2*complexconjugate(LUDD2x1x2)*complexconjugate(Rd3x5)*complexconjugate(Rd4x4)*complexconjugate(Ru6x2)',
                     texname = '\\text{I107x436}')

I108x121 = Parameter(name = 'I108x121',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x2*Ru1x6',
                     texname = '\\text{I108x121}')

I108x122 = Parameter(name = 'I108x122',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x2*Ru2x6',
                     texname = '\\text{I108x122}')

I108x123 = Parameter(name = 'I108x123',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x2*Ru3x5',
                     texname = '\\text{I108x123}')

I108x124 = Parameter(name = 'I108x124',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x2*Ru4x4',
                     texname = '\\text{I108x124}')

I108x131 = Parameter(name = 'I108x131',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Ru1x6',
                     texname = '\\text{I108x131}')

I108x132 = Parameter(name = 'I108x132',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Ru2x6',
                     texname = '\\text{I108x132}')

I108x133 = Parameter(name = 'I108x133',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x3*Ru3x5',
                     texname = '\\text{I108x133}')

I108x134 = Parameter(name = 'I108x134',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x3*Ru4x4',
                     texname = '\\text{I108x134}')

I108x211 = Parameter(name = 'I108x211',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x1*Ru1x6',
                     texname = '\\text{I108x211}')

I108x212 = Parameter(name = 'I108x212',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x1*Ru2x6',
                     texname = '\\text{I108x212}')

I108x213 = Parameter(name = 'I108x213',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x1*Ru3x5',
                     texname = '\\text{I108x213}')

I108x214 = Parameter(name = 'I108x214',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x1*Ru4x4',
                     texname = '\\text{I108x214}')

I108x231 = Parameter(name = 'I108x231',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Ru1x6',
                     texname = '\\text{I108x231}')

I108x232 = Parameter(name = 'I108x232',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Ru2x6',
                     texname = '\\text{I108x232}')

I108x233 = Parameter(name = 'I108x233',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x3*Ru3x5',
                     texname = '\\text{I108x233}')

I108x234 = Parameter(name = 'I108x234',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x3*Ru4x4',
                     texname = '\\text{I108x234}')

I108x311 = Parameter(name = 'I108x311',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Ru1x6',
                     texname = '\\text{I108x311}')

I108x312 = Parameter(name = 'I108x312',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Ru2x6',
                     texname = '\\text{I108x312}')

I108x313 = Parameter(name = 'I108x313',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x1*Ru3x5',
                     texname = '\\text{I108x313}')

I108x314 = Parameter(name = 'I108x314',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x1*Ru4x4',
                     texname = '\\text{I108x314}')

I108x321 = Parameter(name = 'I108x321',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Ru1x6',
                     texname = '\\text{I108x321}')

I108x322 = Parameter(name = 'I108x322',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Ru2x6',
                     texname = '\\text{I108x322}')

I108x323 = Parameter(name = 'I108x323',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x2*Ru3x5',
                     texname = '\\text{I108x323}')

I108x324 = Parameter(name = 'I108x324',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x2*Ru4x4',
                     texname = '\\text{I108x324}')

I109x121 = Parameter(name = 'I109x121',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x1*Ru1x6',
                     texname = '\\text{I109x121}')

I109x122 = Parameter(name = 'I109x122',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x1*Ru2x6',
                     texname = '\\text{I109x122}')

I109x123 = Parameter(name = 'I109x123',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x1*Ru3x5',
                     texname = '\\text{I109x123}')

I109x124 = Parameter(name = 'I109x124',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x1*Ru4x4',
                     texname = '\\text{I109x124}')

I109x131 = Parameter(name = 'I109x131',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Ru1x6',
                     texname = '\\text{I109x131}')

I109x132 = Parameter(name = 'I109x132',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Ru2x6',
                     texname = '\\text{I109x132}')

I109x133 = Parameter(name = 'I109x133',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x1*Ru3x5',
                     texname = '\\text{I109x133}')

I109x134 = Parameter(name = 'I109x134',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x1*Ru4x4',
                     texname = '\\text{I109x134}')

I109x211 = Parameter(name = 'I109x211',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x2*Ru1x6',
                     texname = '\\text{I109x211}')

I109x212 = Parameter(name = 'I109x212',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x2*Ru2x6',
                     texname = '\\text{I109x212}')

I109x213 = Parameter(name = 'I109x213',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x2*Ru3x5',
                     texname = '\\text{I109x213}')

I109x214 = Parameter(name = 'I109x214',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x2*Ru4x4',
                     texname = '\\text{I109x214}')

I109x231 = Parameter(name = 'I109x231',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Ru1x6',
                     texname = '\\text{I109x231}')

I109x232 = Parameter(name = 'I109x232',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Ru2x6',
                     texname = '\\text{I109x232}')

I109x233 = Parameter(name = 'I109x233',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x2*Ru3x5',
                     texname = '\\text{I109x233}')

I109x234 = Parameter(name = 'I109x234',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x2*Ru4x4',
                     texname = '\\text{I109x234}')

I109x311 = Parameter(name = 'I109x311',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Ru1x6',
                     texname = '\\text{I109x311}')

I109x312 = Parameter(name = 'I109x312',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Ru2x6',
                     texname = '\\text{I109x312}')

I109x313 = Parameter(name = 'I109x313',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x3*Ru3x5',
                     texname = '\\text{I109x313}')

I109x314 = Parameter(name = 'I109x314',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x3*Ru4x4',
                     texname = '\\text{I109x314}')

I109x321 = Parameter(name = 'I109x321',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Ru1x6',
                     texname = '\\text{I109x321}')

I109x322 = Parameter(name = 'I109x322',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Ru2x6',
                     texname = '\\text{I109x322}')

I109x323 = Parameter(name = 'I109x323',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x3*Ru3x5',
                     texname = '\\text{I109x323}')

I109x324 = Parameter(name = 'I109x324',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x3*Ru4x4',
                     texname = '\\text{I109x324}')

I11x11 = Parameter(name = 'I11x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x6*complexconjugate(Rd1x6)',
                   texname = '\\text{I11x11}')

I11x12 = Parameter(name = 'I11x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x6*complexconjugate(Rd1x6)',
                   texname = '\\text{I11x12}')

I11x21 = Parameter(name = 'I11x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x6*complexconjugate(Rd2x6)',
                   texname = '\\text{I11x21}')

I11x22 = Parameter(name = 'I11x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x6*complexconjugate(Rd2x6)',
                   texname = '\\text{I11x22}')

I11x33 = Parameter(name = 'I11x33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd3x5*complexconjugate(Rd3x5)',
                   texname = '\\text{I11x33}')

I11x44 = Parameter(name = 'I11x44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd4x4*complexconjugate(Rd4x4)',
                   texname = '\\text{I11x44}')

I110x15 = Parameter(name = 'I110x15',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru5x1*complexconjugate(yu1x1)',
                    texname = '\\text{I110x15}')

I110x26 = Parameter(name = 'I110x26',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru6x2*complexconjugate(yu2x2)',
                    texname = '\\text{I110x26}')

I110x31 = Parameter(name = 'I110x31',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*complexconjugate(yu3x3)',
                    texname = '\\text{I110x31}')

I110x32 = Parameter(name = 'I110x32',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*complexconjugate(yu3x3)',
                    texname = '\\text{I110x32}')

I111x14 = Parameter(name = 'I111x14',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru4x4*yu1x1',
                    texname = '\\text{I111x14}')

I111x23 = Parameter(name = 'I111x23',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru3x5*yu2x2',
                    texname = '\\text{I111x23}')

I111x31 = Parameter(name = 'I111x31',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x6*yu3x3',
                    texname = '\\text{I111x31}')

I111x32 = Parameter(name = 'I111x32',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x6*yu3x3',
                    texname = '\\text{I111x32}')

I112x15 = Parameter(name = 'I112x15',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru5x1',
                    texname = '\\text{I112x15}')

I112x26 = Parameter(name = 'I112x26',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru6x2',
                    texname = '\\text{I112x26}')

I112x31 = Parameter(name = 'I112x31',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3',
                    texname = '\\text{I112x31}')

I112x32 = Parameter(name = 'I112x32',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3',
                    texname = '\\text{I112x32}')

I113x15 = Parameter(name = 'I113x15',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru5x1*complexconjugate(yd1x1)',
                    texname = '\\text{I113x15}')

I113x26 = Parameter(name = 'I113x26',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru6x2*complexconjugate(yd2x2)',
                    texname = '\\text{I113x26}')

I113x31 = Parameter(name = 'I113x31',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*complexconjugate(yd3x3)',
                    texname = '\\text{I113x31}')

I113x32 = Parameter(name = 'I113x32',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*complexconjugate(yd3x3)',
                    texname = '\\text{I113x32}')

I114x14 = Parameter(name = 'I114x14',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru4x4*yu1x1',
                    texname = '\\text{I114x14}')

I114x23 = Parameter(name = 'I114x23',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru3x5*yu2x2',
                    texname = '\\text{I114x23}')

I114x31 = Parameter(name = 'I114x31',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x6*yu3x3',
                    texname = '\\text{I114x31}')

I114x32 = Parameter(name = 'I114x32',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x6*yu3x3',
                    texname = '\\text{I114x32}')

I115x111 = Parameter(name = 'I115x111',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru1x3*complexconjugate(LLQD1x3x1)',
                     texname = '\\text{I115x111}')

I115x112 = Parameter(name = 'I115x112',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru2x3*complexconjugate(LLQD1x3x1)',
                     texname = '\\text{I115x112}')

I115x115 = Parameter(name = 'I115x115',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru5x1*complexconjugate(LLQD1x1x1)',
                     texname = '\\text{I115x115}')

I115x116 = Parameter(name = 'I115x116',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru6x2*complexconjugate(LLQD1x2x1)',
                     texname = '\\text{I115x116}')

I115x121 = Parameter(name = 'I115x121',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru1x3*complexconjugate(LLQD1x3x2)',
                     texname = '\\text{I115x121}')

I115x122 = Parameter(name = 'I115x122',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru2x3*complexconjugate(LLQD1x3x2)',
                     texname = '\\text{I115x122}')

I115x125 = Parameter(name = 'I115x125',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru5x1*complexconjugate(LLQD1x1x2)',
                     texname = '\\text{I115x125}')

I115x126 = Parameter(name = 'I115x126',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru6x2*complexconjugate(LLQD1x2x2)',
                     texname = '\\text{I115x126}')

I115x131 = Parameter(name = 'I115x131',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru1x3*complexconjugate(LLQD1x3x3)',
                     texname = '\\text{I115x131}')

I115x132 = Parameter(name = 'I115x132',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru2x3*complexconjugate(LLQD1x3x3)',
                     texname = '\\text{I115x132}')

I115x135 = Parameter(name = 'I115x135',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru5x1*complexconjugate(LLQD1x1x3)',
                     texname = '\\text{I115x135}')

I115x136 = Parameter(name = 'I115x136',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru6x2*complexconjugate(LLQD1x2x3)',
                     texname = '\\text{I115x136}')

I115x211 = Parameter(name = 'I115x211',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru1x3*complexconjugate(LLQD2x3x1)',
                     texname = '\\text{I115x211}')

I115x212 = Parameter(name = 'I115x212',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru2x3*complexconjugate(LLQD2x3x1)',
                     texname = '\\text{I115x212}')

I115x215 = Parameter(name = 'I115x215',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru5x1*complexconjugate(LLQD2x1x1)',
                     texname = '\\text{I115x215}')

I115x216 = Parameter(name = 'I115x216',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru6x2*complexconjugate(LLQD2x2x1)',
                     texname = '\\text{I115x216}')

I115x221 = Parameter(name = 'I115x221',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru1x3*complexconjugate(LLQD2x3x2)',
                     texname = '\\text{I115x221}')

I115x222 = Parameter(name = 'I115x222',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru2x3*complexconjugate(LLQD2x3x2)',
                     texname = '\\text{I115x222}')

I115x225 = Parameter(name = 'I115x225',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru5x1*complexconjugate(LLQD2x1x2)',
                     texname = '\\text{I115x225}')

I115x226 = Parameter(name = 'I115x226',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru6x2*complexconjugate(LLQD2x2x2)',
                     texname = '\\text{I115x226}')

I115x231 = Parameter(name = 'I115x231',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru1x3*complexconjugate(LLQD2x3x3)',
                     texname = '\\text{I115x231}')

I115x232 = Parameter(name = 'I115x232',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru2x3*complexconjugate(LLQD2x3x3)',
                     texname = '\\text{I115x232}')

I115x235 = Parameter(name = 'I115x235',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru5x1*complexconjugate(LLQD2x1x3)',
                     texname = '\\text{I115x235}')

I115x236 = Parameter(name = 'I115x236',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru6x2*complexconjugate(LLQD2x2x3)',
                     texname = '\\text{I115x236}')

I115x311 = Parameter(name = 'I115x311',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru1x3*complexconjugate(LLQD3x3x1)',
                     texname = '\\text{I115x311}')

I115x312 = Parameter(name = 'I115x312',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru2x3*complexconjugate(LLQD3x3x1)',
                     texname = '\\text{I115x312}')

I115x315 = Parameter(name = 'I115x315',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru5x1*complexconjugate(LLQD3x1x1)',
                     texname = '\\text{I115x315}')

I115x316 = Parameter(name = 'I115x316',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru6x2*complexconjugate(LLQD3x2x1)',
                     texname = '\\text{I115x316}')

I115x321 = Parameter(name = 'I115x321',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru1x3*complexconjugate(LLQD3x3x2)',
                     texname = '\\text{I115x321}')

I115x322 = Parameter(name = 'I115x322',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru2x3*complexconjugate(LLQD3x3x2)',
                     texname = '\\text{I115x322}')

I115x325 = Parameter(name = 'I115x325',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru5x1*complexconjugate(LLQD3x1x2)',
                     texname = '\\text{I115x325}')

I115x326 = Parameter(name = 'I115x326',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru6x2*complexconjugate(LLQD3x2x2)',
                     texname = '\\text{I115x326}')

I115x331 = Parameter(name = 'I115x331',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru1x3*complexconjugate(LLQD3x3x3)',
                     texname = '\\text{I115x331}')

I115x332 = Parameter(name = 'I115x332',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru2x3*complexconjugate(LLQD3x3x3)',
                     texname = '\\text{I115x332}')

I115x335 = Parameter(name = 'I115x335',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru5x1*complexconjugate(LLQD3x1x3)',
                     texname = '\\text{I115x335}')

I115x336 = Parameter(name = 'I115x336',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Ru6x2*complexconjugate(LLQD3x2x3)',
                     texname = '\\text{I115x336}')

I116x11 = Parameter(name = 'I116x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*complexconjugate(Rd1x3)',
                    texname = '\\text{I116x11}')

I116x12 = Parameter(name = 'I116x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*complexconjugate(Rd1x3)',
                    texname = '\\text{I116x12}')

I116x21 = Parameter(name = 'I116x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*complexconjugate(Rd2x3)',
                    texname = '\\text{I116x21}')

I116x22 = Parameter(name = 'I116x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*complexconjugate(Rd2x3)',
                    texname = '\\text{I116x22}')

I116x56 = Parameter(name = 'I116x56',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru6x2*complexconjugate(Rd5x2)',
                    texname = '\\text{I116x56}')

I116x65 = Parameter(name = 'I116x65',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru5x1*complexconjugate(Rd6x1)',
                    texname = '\\text{I116x65}')

I117x11 = Parameter(name = 'I117x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*complexconjugate(Rd1x6)*complexconjugate(yd3x3)',
                    texname = '\\text{I117x11}')

I117x12 = Parameter(name = 'I117x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*complexconjugate(Rd1x6)*complexconjugate(yd3x3)',
                    texname = '\\text{I117x12}')

I117x21 = Parameter(name = 'I117x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*complexconjugate(Rd2x6)*complexconjugate(yd3x3)',
                    texname = '\\text{I117x21}')

I117x22 = Parameter(name = 'I117x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*complexconjugate(Rd2x6)*complexconjugate(yd3x3)',
                    texname = '\\text{I117x22}')

I117x36 = Parameter(name = 'I117x36',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru6x2*complexconjugate(Rd3x5)*complexconjugate(yd2x2)',
                    texname = '\\text{I117x36}')

I117x45 = Parameter(name = 'I117x45',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru5x1*complexconjugate(Rd4x4)*complexconjugate(yd1x1)',
                    texname = '\\text{I117x45}')

I118x11 = Parameter(name = 'I118x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*complexconjugate(Rd1x6)*complexconjugate(td3x3)',
                    texname = '\\text{I118x11}')

I118x12 = Parameter(name = 'I118x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*complexconjugate(Rd1x6)*complexconjugate(td3x3)',
                    texname = '\\text{I118x12}')

I118x21 = Parameter(name = 'I118x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*complexconjugate(Rd2x6)*complexconjugate(td3x3)',
                    texname = '\\text{I118x21}')

I118x22 = Parameter(name = 'I118x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*complexconjugate(Rd2x6)*complexconjugate(td3x3)',
                    texname = '\\text{I118x22}')

I118x36 = Parameter(name = 'I118x36',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru6x2*complexconjugate(Rd3x5)*complexconjugate(td2x2)',
                    texname = '\\text{I118x36}')

I118x45 = Parameter(name = 'I118x45',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru5x1*complexconjugate(Rd4x4)*complexconjugate(td1x1)',
                    texname = '\\text{I118x45}')

I119x11 = Parameter(name = 'I119x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x6*tu3x3*complexconjugate(Rd1x3)',
                    texname = '\\text{I119x11}')

I119x12 = Parameter(name = 'I119x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x6*tu3x3*complexconjugate(Rd1x3)',
                    texname = '\\text{I119x12}')

I119x21 = Parameter(name = 'I119x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x6*tu3x3*complexconjugate(Rd2x3)',
                    texname = '\\text{I119x21}')

I119x22 = Parameter(name = 'I119x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x6*tu3x3*complexconjugate(Rd2x3)',
                    texname = '\\text{I119x22}')

I119x53 = Parameter(name = 'I119x53',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru3x5*tu2x2*complexconjugate(Rd5x2)',
                    texname = '\\text{I119x53}')

I119x64 = Parameter(name = 'I119x64',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru4x4*tu1x1*complexconjugate(Rd6x1)',
                    texname = '\\text{I119x64}')

I12x111 = Parameter(name = 'I12x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD1x1x3*Rd1x6',
                    texname = '\\text{I12x111}')

I12x112 = Parameter(name = 'I12x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD1x1x3*Rd2x6',
                    texname = '\\text{I12x112}')

I12x113 = Parameter(name = 'I12x113',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD1x1x2*Rd3x5',
                    texname = '\\text{I12x113}')

I12x121 = Parameter(name = 'I12x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD2x1x3*Rd1x6',
                    texname = '\\text{I12x121}')

I12x122 = Parameter(name = 'I12x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD2x1x3*Rd2x6',
                    texname = '\\text{I12x122}')

I12x123 = Parameter(name = 'I12x123',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD2x1x2*Rd3x5',
                    texname = '\\text{I12x123}')

I12x131 = Parameter(name = 'I12x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD3x1x3*Rd1x6',
                    texname = '\\text{I12x131}')

I12x132 = Parameter(name = 'I12x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD3x1x3*Rd2x6',
                    texname = '\\text{I12x132}')

I12x133 = Parameter(name = 'I12x133',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD3x1x2*Rd3x5',
                    texname = '\\text{I12x133}')

I12x211 = Parameter(name = 'I12x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD1x2x3*Rd1x6',
                    texname = '\\text{I12x211}')

I12x212 = Parameter(name = 'I12x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD1x2x3*Rd2x6',
                    texname = '\\text{I12x212}')

I12x214 = Parameter(name = 'I12x214',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD1x2x1*Rd4x4',
                    texname = '\\text{I12x214}')

I12x221 = Parameter(name = 'I12x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD2x2x3*Rd1x6',
                    texname = '\\text{I12x221}')

I12x222 = Parameter(name = 'I12x222',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD2x2x3*Rd2x6',
                    texname = '\\text{I12x222}')

I12x224 = Parameter(name = 'I12x224',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD2x2x1*Rd4x4',
                    texname = '\\text{I12x224}')

I12x231 = Parameter(name = 'I12x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD3x2x3*Rd1x6',
                    texname = '\\text{I12x231}')

I12x232 = Parameter(name = 'I12x232',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD3x2x3*Rd2x6',
                    texname = '\\text{I12x232}')

I12x234 = Parameter(name = 'I12x234',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD3x2x1*Rd4x4',
                    texname = '\\text{I12x234}')

I12x313 = Parameter(name = 'I12x313',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD1x3x2*Rd3x5',
                    texname = '\\text{I12x313}')

I12x314 = Parameter(name = 'I12x314',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD1x3x1*Rd4x4',
                    texname = '\\text{I12x314}')

I12x323 = Parameter(name = 'I12x323',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD2x3x2*Rd3x5',
                    texname = '\\text{I12x323}')

I12x324 = Parameter(name = 'I12x324',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD2x3x1*Rd4x4',
                    texname = '\\text{I12x324}')

I12x333 = Parameter(name = 'I12x333',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD3x3x2*Rd3x5',
                    texname = '\\text{I12x333}')

I12x334 = Parameter(name = 'I12x334',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD3x3x1*Rd4x4',
                    texname = '\\text{I12x334}')

I120x11 = Parameter(name = 'I120x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*yd3x3*complexconjugate(Rd1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I120x11}')

I120x12 = Parameter(name = 'I120x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*yd3x3*complexconjugate(Rd1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I120x12}')

I120x21 = Parameter(name = 'I120x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*yd3x3*complexconjugate(Rd2x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I120x21}')

I120x22 = Parameter(name = 'I120x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*yd3x3*complexconjugate(Rd2x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I120x22}')

I120x56 = Parameter(name = 'I120x56',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru6x2*yd2x2*complexconjugate(Rd5x2)*complexconjugate(yd2x2)',
                    texname = '\\text{I120x56}')

I120x65 = Parameter(name = 'I120x65',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru5x1*yd1x1*complexconjugate(Rd6x1)*complexconjugate(yd1x1)',
                    texname = '\\text{I120x65}')

I121x11 = Parameter(name = 'I121x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*yu3x3*complexconjugate(Rd1x3)*complexconjugate(yu3x3)',
                    texname = '\\text{I121x11}')

I121x12 = Parameter(name = 'I121x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*yu3x3*complexconjugate(Rd1x3)*complexconjugate(yu3x3)',
                    texname = '\\text{I121x12}')

I121x21 = Parameter(name = 'I121x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*yu3x3*complexconjugate(Rd2x3)*complexconjugate(yu3x3)',
                    texname = '\\text{I121x21}')

I121x22 = Parameter(name = 'I121x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*yu3x3*complexconjugate(Rd2x3)*complexconjugate(yu3x3)',
                    texname = '\\text{I121x22}')

I121x56 = Parameter(name = 'I121x56',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru6x2*yu2x2*complexconjugate(Rd5x2)*complexconjugate(yu2x2)',
                    texname = '\\text{I121x56}')

I121x65 = Parameter(name = 'I121x65',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru5x1*yu1x1*complexconjugate(Rd6x1)*complexconjugate(yu1x1)',
                    texname = '\\text{I121x65}')

I122x11 = Parameter(name = 'I122x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x6*yu3x3*complexconjugate(Rd1x3)',
                    texname = '\\text{I122x11}')

I122x12 = Parameter(name = 'I122x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x6*yu3x3*complexconjugate(Rd1x3)',
                    texname = '\\text{I122x12}')

I122x21 = Parameter(name = 'I122x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x6*yu3x3*complexconjugate(Rd2x3)',
                    texname = '\\text{I122x21}')

I122x22 = Parameter(name = 'I122x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x6*yu3x3*complexconjugate(Rd2x3)',
                    texname = '\\text{I122x22}')

I122x53 = Parameter(name = 'I122x53',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru3x5*yu2x2*complexconjugate(Rd5x2)',
                    texname = '\\text{I122x53}')

I122x64 = Parameter(name = 'I122x64',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru4x4*yu1x1*complexconjugate(Rd6x1)',
                    texname = '\\text{I122x64}')

I123x11 = Parameter(name = 'I123x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x6*yu3x3*complexconjugate(Rd1x6)*complexconjugate(yd3x3)',
                    texname = '\\text{I123x11}')

I123x12 = Parameter(name = 'I123x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x6*yu3x3*complexconjugate(Rd1x6)*complexconjugate(yd3x3)',
                    texname = '\\text{I123x12}')

I123x21 = Parameter(name = 'I123x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x6*yu3x3*complexconjugate(Rd2x6)*complexconjugate(yd3x3)',
                    texname = '\\text{I123x21}')

I123x22 = Parameter(name = 'I123x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x6*yu3x3*complexconjugate(Rd2x6)*complexconjugate(yd3x3)',
                    texname = '\\text{I123x22}')

I123x33 = Parameter(name = 'I123x33',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru3x5*yu2x2*complexconjugate(Rd3x5)*complexconjugate(yd2x2)',
                    texname = '\\text{I123x33}')

I123x44 = Parameter(name = 'I123x44',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru4x4*yu1x1*complexconjugate(Rd4x4)*complexconjugate(yd1x1)',
                    texname = '\\text{I123x44}')

I124x11 = Parameter(name = 'I124x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*complexconjugate(Ru1x3)',
                    texname = '\\text{I124x11}')

I124x12 = Parameter(name = 'I124x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*complexconjugate(Ru1x3)',
                    texname = '\\text{I124x12}')

I124x21 = Parameter(name = 'I124x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*complexconjugate(Ru2x3)',
                    texname = '\\text{I124x21}')

I124x22 = Parameter(name = 'I124x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*complexconjugate(Ru2x3)',
                    texname = '\\text{I124x22}')

I124x55 = Parameter(name = 'I124x55',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru5x1*complexconjugate(Ru5x1)',
                    texname = '\\text{I124x55}')

I124x66 = Parameter(name = 'I124x66',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru6x2*complexconjugate(Ru6x2)',
                    texname = '\\text{I124x66}')

I125x11 = Parameter(name = 'I125x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x6*complexconjugate(Ru1x6)',
                    texname = '\\text{I125x11}')

I125x12 = Parameter(name = 'I125x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x6*complexconjugate(Ru1x6)',
                    texname = '\\text{I125x12}')

I125x21 = Parameter(name = 'I125x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x6*complexconjugate(Ru2x6)',
                    texname = '\\text{I125x21}')

I125x22 = Parameter(name = 'I125x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x6*complexconjugate(Ru2x6)',
                    texname = '\\text{I125x22}')

I125x33 = Parameter(name = 'I125x33',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru3x5*complexconjugate(Ru3x5)',
                    texname = '\\text{I125x33}')

I125x44 = Parameter(name = 'I125x44',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru4x4*complexconjugate(Ru4x4)',
                    texname = '\\text{I125x44}')

I126x11 = Parameter(name = 'I126x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I126x11}')

I126x12 = Parameter(name = 'I126x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I126x12}')

I126x21 = Parameter(name = 'I126x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I126x21}')

I126x22 = Parameter(name = 'I126x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I126x22}')

I126x36 = Parameter(name = 'I126x36',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru6x2*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I126x36}')

I126x45 = Parameter(name = 'I126x45',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru5x1*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I126x45}')

I127x11 = Parameter(name = 'I127x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*complexconjugate(Ru1x6)*complexconjugate(tu3x3)',
                    texname = '\\text{I127x11}')

I127x12 = Parameter(name = 'I127x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*complexconjugate(Ru1x6)*complexconjugate(tu3x3)',
                    texname = '\\text{I127x12}')

I127x21 = Parameter(name = 'I127x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*complexconjugate(Ru2x6)*complexconjugate(tu3x3)',
                    texname = '\\text{I127x21}')

I127x22 = Parameter(name = 'I127x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*complexconjugate(Ru2x6)*complexconjugate(tu3x3)',
                    texname = '\\text{I127x22}')

I127x36 = Parameter(name = 'I127x36',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru6x2*complexconjugate(Ru3x5)*complexconjugate(tu2x2)',
                    texname = '\\text{I127x36}')

I127x45 = Parameter(name = 'I127x45',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru5x1*complexconjugate(Ru4x4)*complexconjugate(tu1x1)',
                    texname = '\\text{I127x45}')

I128x11 = Parameter(name = 'I128x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x6*tu3x3*complexconjugate(Ru1x3)',
                    texname = '\\text{I128x11}')

I128x12 = Parameter(name = 'I128x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x6*tu3x3*complexconjugate(Ru1x3)',
                    texname = '\\text{I128x12}')

I128x21 = Parameter(name = 'I128x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x6*tu3x3*complexconjugate(Ru2x3)',
                    texname = '\\text{I128x21}')

I128x22 = Parameter(name = 'I128x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x6*tu3x3*complexconjugate(Ru2x3)',
                    texname = '\\text{I128x22}')

I128x54 = Parameter(name = 'I128x54',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru4x4*tu1x1*complexconjugate(Ru5x1)',
                    texname = '\\text{I128x54}')

I128x63 = Parameter(name = 'I128x63',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru3x5*tu2x2*complexconjugate(Ru6x2)',
                    texname = '\\text{I128x63}')

I129x11 = Parameter(name = 'I129x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x6*yu3x3*complexconjugate(Ru1x3)',
                    texname = '\\text{I129x11}')

I129x12 = Parameter(name = 'I129x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x6*yu3x3*complexconjugate(Ru1x3)',
                    texname = '\\text{I129x12}')

I129x21 = Parameter(name = 'I129x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x6*yu3x3*complexconjugate(Ru2x3)',
                    texname = '\\text{I129x21}')

I129x22 = Parameter(name = 'I129x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x6*yu3x3*complexconjugate(Ru2x3)',
                    texname = '\\text{I129x22}')

I129x54 = Parameter(name = 'I129x54',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru4x4*yu1x1*complexconjugate(Ru5x1)',
                    texname = '\\text{I129x54}')

I129x63 = Parameter(name = 'I129x63',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru3x5*yu2x2*complexconjugate(Ru6x2)',
                    texname = '\\text{I129x63}')

I13x111 = Parameter(name = 'I13x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD1x3x1*Rd1x6',
                    texname = '\\text{I13x111}')

I13x112 = Parameter(name = 'I13x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD1x3x1*Rd2x6',
                    texname = '\\text{I13x112}')

I13x113 = Parameter(name = 'I13x113',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD1x2x1*Rd3x5',
                    texname = '\\text{I13x113}')

I13x121 = Parameter(name = 'I13x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD2x3x1*Rd1x6',
                    texname = '\\text{I13x121}')

I13x122 = Parameter(name = 'I13x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD2x3x1*Rd2x6',
                    texname = '\\text{I13x122}')

I13x123 = Parameter(name = 'I13x123',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD2x2x1*Rd3x5',
                    texname = '\\text{I13x123}')

I13x131 = Parameter(name = 'I13x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD3x3x1*Rd1x6',
                    texname = '\\text{I13x131}')

I13x132 = Parameter(name = 'I13x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD3x3x1*Rd2x6',
                    texname = '\\text{I13x132}')

I13x133 = Parameter(name = 'I13x133',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD3x2x1*Rd3x5',
                    texname = '\\text{I13x133}')

I13x211 = Parameter(name = 'I13x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD1x3x2*Rd1x6',
                    texname = '\\text{I13x211}')

I13x212 = Parameter(name = 'I13x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD1x3x2*Rd2x6',
                    texname = '\\text{I13x212}')

I13x214 = Parameter(name = 'I13x214',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD1x1x2*Rd4x4',
                    texname = '\\text{I13x214}')

I13x221 = Parameter(name = 'I13x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD2x3x2*Rd1x6',
                    texname = '\\text{I13x221}')

I13x222 = Parameter(name = 'I13x222',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD2x3x2*Rd2x6',
                    texname = '\\text{I13x222}')

I13x224 = Parameter(name = 'I13x224',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD2x1x2*Rd4x4',
                    texname = '\\text{I13x224}')

I13x231 = Parameter(name = 'I13x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD3x3x2*Rd1x6',
                    texname = '\\text{I13x231}')

I13x232 = Parameter(name = 'I13x232',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD3x3x2*Rd2x6',
                    texname = '\\text{I13x232}')

I13x234 = Parameter(name = 'I13x234',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD3x1x2*Rd4x4',
                    texname = '\\text{I13x234}')

I13x313 = Parameter(name = 'I13x313',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD1x2x3*Rd3x5',
                    texname = '\\text{I13x313}')

I13x314 = Parameter(name = 'I13x314',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD1x1x3*Rd4x4',
                    texname = '\\text{I13x314}')

I13x323 = Parameter(name = 'I13x323',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD2x2x3*Rd3x5',
                    texname = '\\text{I13x323}')

I13x324 = Parameter(name = 'I13x324',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD2x1x3*Rd4x4',
                    texname = '\\text{I13x324}')

I13x333 = Parameter(name = 'I13x333',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD3x2x3*Rd3x5',
                    texname = '\\text{I13x333}')

I13x334 = Parameter(name = 'I13x334',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LUDD3x1x3*Rd4x4',
                    texname = '\\text{I13x334}')

I130x11 = Parameter(name = 'I130x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*yu3x3*complexconjugate(Ru1x3)*complexconjugate(yu3x3)',
                    texname = '\\text{I130x11}')

I130x12 = Parameter(name = 'I130x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*yu3x3*complexconjugate(Ru1x3)*complexconjugate(yu3x3)',
                    texname = '\\text{I130x12}')

I130x21 = Parameter(name = 'I130x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*yu3x3*complexconjugate(Ru2x3)*complexconjugate(yu3x3)',
                    texname = '\\text{I130x21}')

I130x22 = Parameter(name = 'I130x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*yu3x3*complexconjugate(Ru2x3)*complexconjugate(yu3x3)',
                    texname = '\\text{I130x22}')

I130x55 = Parameter(name = 'I130x55',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru5x1*yu1x1*complexconjugate(Ru5x1)*complexconjugate(yu1x1)',
                    texname = '\\text{I130x55}')

I130x66 = Parameter(name = 'I130x66',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru6x2*yu2x2*complexconjugate(Ru6x2)*complexconjugate(yu2x2)',
                    texname = '\\text{I130x66}')

I131x11 = Parameter(name = 'I131x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x6*yu3x3*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I131x11}')

I131x12 = Parameter(name = 'I131x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x6*yu3x3*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I131x12}')

I131x21 = Parameter(name = 'I131x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x6*yu3x3*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I131x21}')

I131x22 = Parameter(name = 'I131x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x6*yu3x3*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I131x22}')

I131x33 = Parameter(name = 'I131x33',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru3x5*yu2x2*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I131x33}')

I131x44 = Parameter(name = 'I131x44',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru4x4*yu1x1*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I131x44}')

I132x131 = Parameter(name = 'I132x131',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd1x6*Rd3x5*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x131}')

I132x132 = Parameter(name = 'I132x132',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd1x6*Rd3x5*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x132}')

I132x135 = Parameter(name = 'I132x135',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x3*Rd1x6*Rd3x5*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I132x135}')

I132x136 = Parameter(name = 'I132x136',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x3*Rd1x6*Rd3x5*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I132x136}')

I132x141 = Parameter(name = 'I132x141',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd1x6*Rd4x4*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x141}')

I132x142 = Parameter(name = 'I132x142',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd1x6*Rd4x4*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x142}')

I132x145 = Parameter(name = 'I132x145',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x3*Rd1x6*Rd4x4*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I132x145}')

I132x146 = Parameter(name = 'I132x146',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x3*Rd1x6*Rd4x4*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I132x146}')

I132x231 = Parameter(name = 'I132x231',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd2x6*Rd3x5*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x231}')

I132x232 = Parameter(name = 'I132x232',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd2x6*Rd3x5*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x232}')

I132x235 = Parameter(name = 'I132x235',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x3*Rd2x6*Rd3x5*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I132x235}')

I132x236 = Parameter(name = 'I132x236',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x3*Rd2x6*Rd3x5*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I132x236}')

I132x241 = Parameter(name = 'I132x241',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd2x6*Rd4x4*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x241}')

I132x242 = Parameter(name = 'I132x242',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd2x6*Rd4x4*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x242}')

I132x245 = Parameter(name = 'I132x245',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x3*Rd2x6*Rd4x4*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I132x245}')

I132x246 = Parameter(name = 'I132x246',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x3*Rd2x6*Rd4x4*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I132x246}')

I132x311 = Parameter(name = 'I132x311',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd1x6*Rd3x5*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x311}')

I132x312 = Parameter(name = 'I132x312',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd1x6*Rd3x5*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x312}')

I132x315 = Parameter(name = 'I132x315',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x2*Rd1x6*Rd3x5*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I132x315}')

I132x316 = Parameter(name = 'I132x316',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x2*Rd1x6*Rd3x5*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I132x316}')

I132x321 = Parameter(name = 'I132x321',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd2x6*Rd3x5*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x321}')

I132x322 = Parameter(name = 'I132x322',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd2x6*Rd3x5*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x322}')

I132x325 = Parameter(name = 'I132x325',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x2*Rd2x6*Rd3x5*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I132x325}')

I132x326 = Parameter(name = 'I132x326',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x2*Rd2x6*Rd3x5*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I132x326}')

I132x341 = Parameter(name = 'I132x341',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x2*Rd3x5*Rd4x4*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x341}')

I132x342 = Parameter(name = 'I132x342',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x2*Rd3x5*Rd4x4*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x342}')

I132x345 = Parameter(name = 'I132x345',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x2*Rd3x5*Rd4x4*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I132x345}')

I132x346 = Parameter(name = 'I132x346',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x2*Rd3x5*Rd4x4*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I132x346}')

I132x411 = Parameter(name = 'I132x411',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd1x6*Rd4x4*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x411}')

I132x412 = Parameter(name = 'I132x412',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd1x6*Rd4x4*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x412}')

I132x415 = Parameter(name = 'I132x415',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x1*Rd1x6*Rd4x4*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I132x415}')

I132x416 = Parameter(name = 'I132x416',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x1*Rd1x6*Rd4x4*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I132x416}')

I132x421 = Parameter(name = 'I132x421',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd2x6*Rd4x4*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x421}')

I132x422 = Parameter(name = 'I132x422',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd2x6*Rd4x4*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x422}')

I132x425 = Parameter(name = 'I132x425',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x1*Rd2x6*Rd4x4*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I132x425}')

I132x426 = Parameter(name = 'I132x426',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x1*Rd2x6*Rd4x4*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I132x426}')

I132x431 = Parameter(name = 'I132x431',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x1*Rd3x5*Rd4x4*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x431}')

I132x432 = Parameter(name = 'I132x432',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x1*Rd3x5*Rd4x4*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I132x432}')

I132x435 = Parameter(name = 'I132x435',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x1*Rd3x5*Rd4x4*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I132x435}')

I132x436 = Parameter(name = 'I132x436',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x1*Rd3x5*Rd4x4*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I132x436}')

I133x131 = Parameter(name = 'I133x131',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd1x6*Rd3x5*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x131}')

I133x132 = Parameter(name = 'I133x132',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd1x6*Rd3x5*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x132}')

I133x135 = Parameter(name = 'I133x135',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x2*Rd1x6*Rd3x5*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I133x135}')

I133x136 = Parameter(name = 'I133x136',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x2*Rd1x6*Rd3x5*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I133x136}')

I133x141 = Parameter(name = 'I133x141',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd1x6*Rd4x4*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x141}')

I133x142 = Parameter(name = 'I133x142',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd1x6*Rd4x4*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x142}')

I133x145 = Parameter(name = 'I133x145',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x1*Rd1x6*Rd4x4*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I133x145}')

I133x146 = Parameter(name = 'I133x146',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x1*Rd1x6*Rd4x4*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I133x146}')

I133x231 = Parameter(name = 'I133x231',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd2x6*Rd3x5*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x231}')

I133x232 = Parameter(name = 'I133x232',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd2x6*Rd3x5*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x232}')

I133x235 = Parameter(name = 'I133x235',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x2*Rd2x6*Rd3x5*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I133x235}')

I133x236 = Parameter(name = 'I133x236',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x2*Rd2x6*Rd3x5*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I133x236}')

I133x241 = Parameter(name = 'I133x241',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd2x6*Rd4x4*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x241}')

I133x242 = Parameter(name = 'I133x242',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd2x6*Rd4x4*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x242}')

I133x245 = Parameter(name = 'I133x245',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x1*Rd2x6*Rd4x4*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I133x245}')

I133x246 = Parameter(name = 'I133x246',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x1*Rd2x6*Rd4x4*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I133x246}')

I133x311 = Parameter(name = 'I133x311',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd1x6*Rd3x5*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x311}')

I133x312 = Parameter(name = 'I133x312',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd1x6*Rd3x5*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x312}')

I133x315 = Parameter(name = 'I133x315',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x3*Rd1x6*Rd3x5*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I133x315}')

I133x316 = Parameter(name = 'I133x316',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x3*Rd1x6*Rd3x5*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I133x316}')

I133x321 = Parameter(name = 'I133x321',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd2x6*Rd3x5*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x321}')

I133x322 = Parameter(name = 'I133x322',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd2x6*Rd3x5*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x322}')

I133x325 = Parameter(name = 'I133x325',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x3*Rd2x6*Rd3x5*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I133x325}')

I133x326 = Parameter(name = 'I133x326',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x3*Rd2x6*Rd3x5*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I133x326}')

I133x341 = Parameter(name = 'I133x341',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x1*Rd3x5*Rd4x4*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x341}')

I133x342 = Parameter(name = 'I133x342',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x1*Rd3x5*Rd4x4*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x342}')

I133x345 = Parameter(name = 'I133x345',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x1*Rd3x5*Rd4x4*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I133x345}')

I133x346 = Parameter(name = 'I133x346',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x1*Rd3x5*Rd4x4*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I133x346}')

I133x411 = Parameter(name = 'I133x411',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd1x6*Rd4x4*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x411}')

I133x412 = Parameter(name = 'I133x412',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd1x6*Rd4x4*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x412}')

I133x415 = Parameter(name = 'I133x415',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x3*Rd1x6*Rd4x4*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I133x415}')

I133x416 = Parameter(name = 'I133x416',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x3*Rd1x6*Rd4x4*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I133x416}')

I133x421 = Parameter(name = 'I133x421',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd2x6*Rd4x4*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x421}')

I133x422 = Parameter(name = 'I133x422',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd2x6*Rd4x4*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x422}')

I133x425 = Parameter(name = 'I133x425',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x3*Rd2x6*Rd4x4*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I133x425}')

I133x426 = Parameter(name = 'I133x426',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x3*Rd2x6*Rd4x4*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I133x426}')

I133x431 = Parameter(name = 'I133x431',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x2*Rd3x5*Rd4x4*Ru1x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x431}')

I133x432 = Parameter(name = 'I133x432',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x2*Rd3x5*Rd4x4*Ru2x3*complexconjugate(yu3x3)',
                     texname = '\\text{I133x432}')

I133x435 = Parameter(name = 'I133x435',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x2*Rd3x5*Rd4x4*Ru5x1*complexconjugate(yu1x1)',
                     texname = '\\text{I133x435}')

I133x436 = Parameter(name = 'I133x436',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x2*Rd3x5*Rd4x4*Ru6x2*complexconjugate(yu2x2)',
                     texname = '\\text{I133x436}')

I134x151 = Parameter(name = 'I134x151',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd1x6*Rd5x2*Ru1x6*complexconjugate(yd2x2)',
                     texname = '\\text{I134x151}')

I134x152 = Parameter(name = 'I134x152',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd1x6*Rd5x2*Ru2x6*complexconjugate(yd2x2)',
                     texname = '\\text{I134x152}')

I134x153 = Parameter(name = 'I134x153',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x3*Rd1x6*Rd5x2*Ru3x5*complexconjugate(yd2x2)',
                     texname = '\\text{I134x153}')

I134x154 = Parameter(name = 'I134x154',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x3*Rd1x6*Rd5x2*Ru4x4*complexconjugate(yd2x2)',
                     texname = '\\text{I134x154}')

I134x161 = Parameter(name = 'I134x161',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd1x6*Rd6x1*Ru1x6*complexconjugate(yd1x1)',
                     texname = '\\text{I134x161}')

I134x162 = Parameter(name = 'I134x162',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd1x6*Rd6x1*Ru2x6*complexconjugate(yd1x1)',
                     texname = '\\text{I134x162}')

I134x163 = Parameter(name = 'I134x163',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x3*Rd1x6*Rd6x1*Ru3x5*complexconjugate(yd1x1)',
                     texname = '\\text{I134x163}')

I134x164 = Parameter(name = 'I134x164',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x3*Rd1x6*Rd6x1*Ru4x4*complexconjugate(yd1x1)',
                     texname = '\\text{I134x164}')

I134x251 = Parameter(name = 'I134x251',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd2x6*Rd5x2*Ru1x6*complexconjugate(yd2x2)',
                     texname = '\\text{I134x251}')

I134x252 = Parameter(name = 'I134x252',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd2x6*Rd5x2*Ru2x6*complexconjugate(yd2x2)',
                     texname = '\\text{I134x252}')

I134x253 = Parameter(name = 'I134x253',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x3*Rd2x6*Rd5x2*Ru3x5*complexconjugate(yd2x2)',
                     texname = '\\text{I134x253}')

I134x254 = Parameter(name = 'I134x254',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x3*Rd2x6*Rd5x2*Ru4x4*complexconjugate(yd2x2)',
                     texname = '\\text{I134x254}')

I134x261 = Parameter(name = 'I134x261',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd2x6*Rd6x1*Ru1x6*complexconjugate(yd1x1)',
                     texname = '\\text{I134x261}')

I134x262 = Parameter(name = 'I134x262',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd2x6*Rd6x1*Ru2x6*complexconjugate(yd1x1)',
                     texname = '\\text{I134x262}')

I134x263 = Parameter(name = 'I134x263',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x3*Rd2x6*Rd6x1*Ru3x5*complexconjugate(yd1x1)',
                     texname = '\\text{I134x263}')

I134x264 = Parameter(name = 'I134x264',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x3*Rd2x6*Rd6x1*Ru4x4*complexconjugate(yd1x1)',
                     texname = '\\text{I134x264}')

I134x311 = Parameter(name = 'I134x311',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd1x3*Rd3x5*Ru1x6*complexconjugate(yd3x3)',
                     texname = '\\text{I134x311}')

I134x312 = Parameter(name = 'I134x312',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd1x3*Rd3x5*Ru2x6*complexconjugate(yd3x3)',
                     texname = '\\text{I134x312}')

I134x313 = Parameter(name = 'I134x313',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x2*Rd1x3*Rd3x5*Ru3x5*complexconjugate(yd3x3)',
                     texname = '\\text{I134x313}')

I134x314 = Parameter(name = 'I134x314',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x2*Rd1x3*Rd3x5*Ru4x4*complexconjugate(yd3x3)',
                     texname = '\\text{I134x314}')

I134x321 = Parameter(name = 'I134x321',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd2x3*Rd3x5*Ru1x6*complexconjugate(yd3x3)',
                     texname = '\\text{I134x321}')

I134x322 = Parameter(name = 'I134x322',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd2x3*Rd3x5*Ru2x6*complexconjugate(yd3x3)',
                     texname = '\\text{I134x322}')

I134x323 = Parameter(name = 'I134x323',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x2*Rd2x3*Rd3x5*Ru3x5*complexconjugate(yd3x3)',
                     texname = '\\text{I134x323}')

I134x324 = Parameter(name = 'I134x324',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x2*Rd2x3*Rd3x5*Ru4x4*complexconjugate(yd3x3)',
                     texname = '\\text{I134x324}')

I134x361 = Parameter(name = 'I134x361',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x2*Rd3x5*Rd6x1*Ru1x6*complexconjugate(yd1x1)',
                     texname = '\\text{I134x361}')

I134x362 = Parameter(name = 'I134x362',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x2*Rd3x5*Rd6x1*Ru2x6*complexconjugate(yd1x1)',
                     texname = '\\text{I134x362}')

I134x363 = Parameter(name = 'I134x363',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x2*Rd3x5*Rd6x1*Ru3x5*complexconjugate(yd1x1)',
                     texname = '\\text{I134x363}')

I134x364 = Parameter(name = 'I134x364',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x2*Rd3x5*Rd6x1*Ru4x4*complexconjugate(yd1x1)',
                     texname = '\\text{I134x364}')

I134x411 = Parameter(name = 'I134x411',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd1x3*Rd4x4*Ru1x6*complexconjugate(yd3x3)',
                     texname = '\\text{I134x411}')

I134x412 = Parameter(name = 'I134x412',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd1x3*Rd4x4*Ru2x6*complexconjugate(yd3x3)',
                     texname = '\\text{I134x412}')

I134x413 = Parameter(name = 'I134x413',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x1*Rd1x3*Rd4x4*Ru3x5*complexconjugate(yd3x3)',
                     texname = '\\text{I134x413}')

I134x414 = Parameter(name = 'I134x414',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x1*Rd1x3*Rd4x4*Ru4x4*complexconjugate(yd3x3)',
                     texname = '\\text{I134x414}')

I134x421 = Parameter(name = 'I134x421',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd2x3*Rd4x4*Ru1x6*complexconjugate(yd3x3)',
                     texname = '\\text{I134x421}')

I134x422 = Parameter(name = 'I134x422',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd2x3*Rd4x4*Ru2x6*complexconjugate(yd3x3)',
                     texname = '\\text{I134x422}')

I134x423 = Parameter(name = 'I134x423',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x1*Rd2x3*Rd4x4*Ru3x5*complexconjugate(yd3x3)',
                     texname = '\\text{I134x423}')

I134x424 = Parameter(name = 'I134x424',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x1*Rd2x3*Rd4x4*Ru4x4*complexconjugate(yd3x3)',
                     texname = '\\text{I134x424}')

I134x451 = Parameter(name = 'I134x451',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x1*Rd4x4*Rd5x2*Ru1x6*complexconjugate(yd2x2)',
                     texname = '\\text{I134x451}')

I134x452 = Parameter(name = 'I134x452',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x1*Rd4x4*Rd5x2*Ru2x6*complexconjugate(yd2x2)',
                     texname = '\\text{I134x452}')

I134x453 = Parameter(name = 'I134x453',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x1*Rd4x4*Rd5x2*Ru3x5*complexconjugate(yd2x2)',
                     texname = '\\text{I134x453}')

I134x454 = Parameter(name = 'I134x454',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x1*Rd4x4*Rd5x2*Ru4x4*complexconjugate(yd2x2)',
                     texname = '\\text{I134x454}')

I135x151 = Parameter(name = 'I135x151',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd1x6*Rd5x2*Ru1x6*complexconjugate(yd2x2)',
                     texname = '\\text{I135x151}')

I135x152 = Parameter(name = 'I135x152',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd1x6*Rd5x2*Ru2x6*complexconjugate(yd2x2)',
                     texname = '\\text{I135x152}')

I135x153 = Parameter(name = 'I135x153',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x2*Rd1x6*Rd5x2*Ru3x5*complexconjugate(yd2x2)',
                     texname = '\\text{I135x153}')

I135x154 = Parameter(name = 'I135x154',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x2*Rd1x6*Rd5x2*Ru4x4*complexconjugate(yd2x2)',
                     texname = '\\text{I135x154}')

I135x161 = Parameter(name = 'I135x161',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd1x6*Rd6x1*Ru1x6*complexconjugate(yd1x1)',
                     texname = '\\text{I135x161}')

I135x162 = Parameter(name = 'I135x162',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd1x6*Rd6x1*Ru2x6*complexconjugate(yd1x1)',
                     texname = '\\text{I135x162}')

I135x163 = Parameter(name = 'I135x163',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x1*Rd1x6*Rd6x1*Ru3x5*complexconjugate(yd1x1)',
                     texname = '\\text{I135x163}')

I135x164 = Parameter(name = 'I135x164',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x1*Rd1x6*Rd6x1*Ru4x4*complexconjugate(yd1x1)',
                     texname = '\\text{I135x164}')

I135x251 = Parameter(name = 'I135x251',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd2x6*Rd5x2*Ru1x6*complexconjugate(yd2x2)',
                     texname = '\\text{I135x251}')

I135x252 = Parameter(name = 'I135x252',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd2x6*Rd5x2*Ru2x6*complexconjugate(yd2x2)',
                     texname = '\\text{I135x252}')

I135x253 = Parameter(name = 'I135x253',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x2*Rd2x6*Rd5x2*Ru3x5*complexconjugate(yd2x2)',
                     texname = '\\text{I135x253}')

I135x254 = Parameter(name = 'I135x254',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x2*Rd2x6*Rd5x2*Ru4x4*complexconjugate(yd2x2)',
                     texname = '\\text{I135x254}')

I135x261 = Parameter(name = 'I135x261',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd2x6*Rd6x1*Ru1x6*complexconjugate(yd1x1)',
                     texname = '\\text{I135x261}')

I135x262 = Parameter(name = 'I135x262',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd2x6*Rd6x1*Ru2x6*complexconjugate(yd1x1)',
                     texname = '\\text{I135x262}')

I135x263 = Parameter(name = 'I135x263',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x1*Rd2x6*Rd6x1*Ru3x5*complexconjugate(yd1x1)',
                     texname = '\\text{I135x263}')

I135x264 = Parameter(name = 'I135x264',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x1*Rd2x6*Rd6x1*Ru4x4*complexconjugate(yd1x1)',
                     texname = '\\text{I135x264}')

I135x311 = Parameter(name = 'I135x311',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd1x3*Rd3x5*Ru1x6*complexconjugate(yd3x3)',
                     texname = '\\text{I135x311}')

I135x312 = Parameter(name = 'I135x312',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd1x3*Rd3x5*Ru2x6*complexconjugate(yd3x3)',
                     texname = '\\text{I135x312}')

I135x313 = Parameter(name = 'I135x313',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x3*Rd1x3*Rd3x5*Ru3x5*complexconjugate(yd3x3)',
                     texname = '\\text{I135x313}')

I135x314 = Parameter(name = 'I135x314',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x3*Rd1x3*Rd3x5*Ru4x4*complexconjugate(yd3x3)',
                     texname = '\\text{I135x314}')

I135x321 = Parameter(name = 'I135x321',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd2x3*Rd3x5*Ru1x6*complexconjugate(yd3x3)',
                     texname = '\\text{I135x321}')

I135x322 = Parameter(name = 'I135x322',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd2x3*Rd3x5*Ru2x6*complexconjugate(yd3x3)',
                     texname = '\\text{I135x322}')

I135x323 = Parameter(name = 'I135x323',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x3*Rd2x3*Rd3x5*Ru3x5*complexconjugate(yd3x3)',
                     texname = '\\text{I135x323}')

I135x324 = Parameter(name = 'I135x324',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x3*Rd2x3*Rd3x5*Ru4x4*complexconjugate(yd3x3)',
                     texname = '\\text{I135x324}')

I135x361 = Parameter(name = 'I135x361',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x1*Rd3x5*Rd6x1*Ru1x6*complexconjugate(yd1x1)',
                     texname = '\\text{I135x361}')

I135x362 = Parameter(name = 'I135x362',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x1*Rd3x5*Rd6x1*Ru2x6*complexconjugate(yd1x1)',
                     texname = '\\text{I135x362}')

I135x363 = Parameter(name = 'I135x363',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x1*Rd3x5*Rd6x1*Ru3x5*complexconjugate(yd1x1)',
                     texname = '\\text{I135x363}')

I135x364 = Parameter(name = 'I135x364',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x1*Rd3x5*Rd6x1*Ru4x4*complexconjugate(yd1x1)',
                     texname = '\\text{I135x364}')

I135x411 = Parameter(name = 'I135x411',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd1x3*Rd4x4*Ru1x6*complexconjugate(yd3x3)',
                     texname = '\\text{I135x411}')

I135x412 = Parameter(name = 'I135x412',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd1x3*Rd4x4*Ru2x6*complexconjugate(yd3x3)',
                     texname = '\\text{I135x412}')

I135x413 = Parameter(name = 'I135x413',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x3*Rd1x3*Rd4x4*Ru3x5*complexconjugate(yd3x3)',
                     texname = '\\text{I135x413}')

I135x414 = Parameter(name = 'I135x414',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x3*Rd1x3*Rd4x4*Ru4x4*complexconjugate(yd3x3)',
                     texname = '\\text{I135x414}')

I135x421 = Parameter(name = 'I135x421',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd2x3*Rd4x4*Ru1x6*complexconjugate(yd3x3)',
                     texname = '\\text{I135x421}')

I135x422 = Parameter(name = 'I135x422',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd2x3*Rd4x4*Ru2x6*complexconjugate(yd3x3)',
                     texname = '\\text{I135x422}')

I135x423 = Parameter(name = 'I135x423',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x3*Rd2x3*Rd4x4*Ru3x5*complexconjugate(yd3x3)',
                     texname = '\\text{I135x423}')

I135x424 = Parameter(name = 'I135x424',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x3*Rd2x3*Rd4x4*Ru4x4*complexconjugate(yd3x3)',
                     texname = '\\text{I135x424}')

I135x451 = Parameter(name = 'I135x451',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x2*Rd4x4*Rd5x2*Ru1x6*complexconjugate(yd2x2)',
                     texname = '\\text{I135x451}')

I135x452 = Parameter(name = 'I135x452',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x2*Rd4x4*Rd5x2*Ru2x6*complexconjugate(yd2x2)',
                     texname = '\\text{I135x452}')

I135x453 = Parameter(name = 'I135x453',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x2*Rd4x4*Rd5x2*Ru3x5*complexconjugate(yd2x2)',
                     texname = '\\text{I135x453}')

I135x454 = Parameter(name = 'I135x454',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x2*Rd4x4*Rd5x2*Ru4x4*complexconjugate(yd2x2)',
                     texname = '\\text{I135x454}')

I136x131 = Parameter(name = 'I136x131',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd1x3*Rd3x5*Ru1x6*complexconjugate(yd3x3)',
                     texname = '\\text{I136x131}')

I136x132 = Parameter(name = 'I136x132',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd1x3*Rd3x5*Ru2x6*complexconjugate(yd3x3)',
                     texname = '\\text{I136x132}')

I136x133 = Parameter(name = 'I136x133',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x2*Rd1x3*Rd3x5*Ru3x5*complexconjugate(yd3x3)',
                     texname = '\\text{I136x133}')

I136x134 = Parameter(name = 'I136x134',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x2*Rd1x3*Rd3x5*Ru4x4*complexconjugate(yd3x3)',
                     texname = '\\text{I136x134}')

I136x141 = Parameter(name = 'I136x141',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd1x3*Rd4x4*Ru1x6*complexconjugate(yd3x3)',
                     texname = '\\text{I136x141}')

I136x142 = Parameter(name = 'I136x142',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd1x3*Rd4x4*Ru2x6*complexconjugate(yd3x3)',
                     texname = '\\text{I136x142}')

I136x143 = Parameter(name = 'I136x143',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x1*Rd1x3*Rd4x4*Ru3x5*complexconjugate(yd3x3)',
                     texname = '\\text{I136x143}')

I136x144 = Parameter(name = 'I136x144',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x1*Rd1x3*Rd4x4*Ru4x4*complexconjugate(yd3x3)',
                     texname = '\\text{I136x144}')

I136x231 = Parameter(name = 'I136x231',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd2x3*Rd3x5*Ru1x6*complexconjugate(yd3x3)',
                     texname = '\\text{I136x231}')

I136x232 = Parameter(name = 'I136x232',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd2x3*Rd3x5*Ru2x6*complexconjugate(yd3x3)',
                     texname = '\\text{I136x232}')

I136x233 = Parameter(name = 'I136x233',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x2*Rd2x3*Rd3x5*Ru3x5*complexconjugate(yd3x3)',
                     texname = '\\text{I136x233}')

I136x234 = Parameter(name = 'I136x234',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x2*Rd2x3*Rd3x5*Ru4x4*complexconjugate(yd3x3)',
                     texname = '\\text{I136x234}')

I136x241 = Parameter(name = 'I136x241',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd2x3*Rd4x4*Ru1x6*complexconjugate(yd3x3)',
                     texname = '\\text{I136x241}')

I136x242 = Parameter(name = 'I136x242',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd2x3*Rd4x4*Ru2x6*complexconjugate(yd3x3)',
                     texname = '\\text{I136x242}')

I136x243 = Parameter(name = 'I136x243',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x1*Rd2x3*Rd4x4*Ru3x5*complexconjugate(yd3x3)',
                     texname = '\\text{I136x243}')

I136x244 = Parameter(name = 'I136x244',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x1*Rd2x3*Rd4x4*Ru4x4*complexconjugate(yd3x3)',
                     texname = '\\text{I136x244}')

I136x511 = Parameter(name = 'I136x511',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd1x6*Rd5x2*Ru1x6*complexconjugate(yd2x2)',
                     texname = '\\text{I136x511}')

I136x512 = Parameter(name = 'I136x512',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd1x6*Rd5x2*Ru2x6*complexconjugate(yd2x2)',
                     texname = '\\text{I136x512}')

I136x513 = Parameter(name = 'I136x513',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x3*Rd1x6*Rd5x2*Ru3x5*complexconjugate(yd2x2)',
                     texname = '\\text{I136x513}')

I136x514 = Parameter(name = 'I136x514',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x3*Rd1x6*Rd5x2*Ru4x4*complexconjugate(yd2x2)',
                     texname = '\\text{I136x514}')

I136x521 = Parameter(name = 'I136x521',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd2x6*Rd5x2*Ru1x6*complexconjugate(yd2x2)',
                     texname = '\\text{I136x521}')

I136x522 = Parameter(name = 'I136x522',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd2x6*Rd5x2*Ru2x6*complexconjugate(yd2x2)',
                     texname = '\\text{I136x522}')

I136x523 = Parameter(name = 'I136x523',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x3*Rd2x6*Rd5x2*Ru3x5*complexconjugate(yd2x2)',
                     texname = '\\text{I136x523}')

I136x524 = Parameter(name = 'I136x524',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x3*Rd2x6*Rd5x2*Ru4x4*complexconjugate(yd2x2)',
                     texname = '\\text{I136x524}')

I136x541 = Parameter(name = 'I136x541',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x1*Rd4x4*Rd5x2*Ru1x6*complexconjugate(yd2x2)',
                     texname = '\\text{I136x541}')

I136x542 = Parameter(name = 'I136x542',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x1*Rd4x4*Rd5x2*Ru2x6*complexconjugate(yd2x2)',
                     texname = '\\text{I136x542}')

I136x543 = Parameter(name = 'I136x543',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x1*Rd4x4*Rd5x2*Ru3x5*complexconjugate(yd2x2)',
                     texname = '\\text{I136x543}')

I136x544 = Parameter(name = 'I136x544',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x1*Rd4x4*Rd5x2*Ru4x4*complexconjugate(yd2x2)',
                     texname = '\\text{I136x544}')

I136x611 = Parameter(name = 'I136x611',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd1x6*Rd6x1*Ru1x6*complexconjugate(yd1x1)',
                     texname = '\\text{I136x611}')

I136x612 = Parameter(name = 'I136x612',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd1x6*Rd6x1*Ru2x6*complexconjugate(yd1x1)',
                     texname = '\\text{I136x612}')

I136x613 = Parameter(name = 'I136x613',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x3*Rd1x6*Rd6x1*Ru3x5*complexconjugate(yd1x1)',
                     texname = '\\text{I136x613}')

I136x614 = Parameter(name = 'I136x614',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x3*Rd1x6*Rd6x1*Ru4x4*complexconjugate(yd1x1)',
                     texname = '\\text{I136x614}')

I136x621 = Parameter(name = 'I136x621',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd2x6*Rd6x1*Ru1x6*complexconjugate(yd1x1)',
                     texname = '\\text{I136x621}')

I136x622 = Parameter(name = 'I136x622',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd2x6*Rd6x1*Ru2x6*complexconjugate(yd1x1)',
                     texname = '\\text{I136x622}')

I136x623 = Parameter(name = 'I136x623',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x3*Rd2x6*Rd6x1*Ru3x5*complexconjugate(yd1x1)',
                     texname = '\\text{I136x623}')

I136x624 = Parameter(name = 'I136x624',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x3*Rd2x6*Rd6x1*Ru4x4*complexconjugate(yd1x1)',
                     texname = '\\text{I136x624}')

I136x631 = Parameter(name = 'I136x631',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x2*Rd3x5*Rd6x1*Ru1x6*complexconjugate(yd1x1)',
                     texname = '\\text{I136x631}')

I136x632 = Parameter(name = 'I136x632',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x2*Rd3x5*Rd6x1*Ru2x6*complexconjugate(yd1x1)',
                     texname = '\\text{I136x632}')

I136x633 = Parameter(name = 'I136x633',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x2*Rd3x5*Rd6x1*Ru3x5*complexconjugate(yd1x1)',
                     texname = '\\text{I136x633}')

I136x634 = Parameter(name = 'I136x634',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x2*Rd3x5*Rd6x1*Ru4x4*complexconjugate(yd1x1)',
                     texname = '\\text{I136x634}')

I137x131 = Parameter(name = 'I137x131',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd1x3*Rd3x5*Ru1x6*complexconjugate(yd3x3)',
                     texname = '\\text{I137x131}')

I137x132 = Parameter(name = 'I137x132',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd1x3*Rd3x5*Ru2x6*complexconjugate(yd3x3)',
                     texname = '\\text{I137x132}')

I137x133 = Parameter(name = 'I137x133',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x3*Rd1x3*Rd3x5*Ru3x5*complexconjugate(yd3x3)',
                     texname = '\\text{I137x133}')

I137x134 = Parameter(name = 'I137x134',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x3*Rd1x3*Rd3x5*Ru4x4*complexconjugate(yd3x3)',
                     texname = '\\text{I137x134}')

I137x141 = Parameter(name = 'I137x141',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd1x3*Rd4x4*Ru1x6*complexconjugate(yd3x3)',
                     texname = '\\text{I137x141}')

I137x142 = Parameter(name = 'I137x142',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd1x3*Rd4x4*Ru2x6*complexconjugate(yd3x3)',
                     texname = '\\text{I137x142}')

I137x143 = Parameter(name = 'I137x143',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x3*Rd1x3*Rd4x4*Ru3x5*complexconjugate(yd3x3)',
                     texname = '\\text{I137x143}')

I137x144 = Parameter(name = 'I137x144',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x3*Rd1x3*Rd4x4*Ru4x4*complexconjugate(yd3x3)',
                     texname = '\\text{I137x144}')

I137x231 = Parameter(name = 'I137x231',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd2x3*Rd3x5*Ru1x6*complexconjugate(yd3x3)',
                     texname = '\\text{I137x231}')

I137x232 = Parameter(name = 'I137x232',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x3*Rd2x3*Rd3x5*Ru2x6*complexconjugate(yd3x3)',
                     texname = '\\text{I137x232}')

I137x233 = Parameter(name = 'I137x233',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x3*Rd2x3*Rd3x5*Ru3x5*complexconjugate(yd3x3)',
                     texname = '\\text{I137x233}')

I137x234 = Parameter(name = 'I137x234',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x3*Rd2x3*Rd3x5*Ru4x4*complexconjugate(yd3x3)',
                     texname = '\\text{I137x234}')

I137x241 = Parameter(name = 'I137x241',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd2x3*Rd4x4*Ru1x6*complexconjugate(yd3x3)',
                     texname = '\\text{I137x241}')

I137x242 = Parameter(name = 'I137x242',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x3*Rd2x3*Rd4x4*Ru2x6*complexconjugate(yd3x3)',
                     texname = '\\text{I137x242}')

I137x243 = Parameter(name = 'I137x243',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x3*Rd2x3*Rd4x4*Ru3x5*complexconjugate(yd3x3)',
                     texname = '\\text{I137x243}')

I137x244 = Parameter(name = 'I137x244',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x3*Rd2x3*Rd4x4*Ru4x4*complexconjugate(yd3x3)',
                     texname = '\\text{I137x244}')

I137x511 = Parameter(name = 'I137x511',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd1x6*Rd5x2*Ru1x6*complexconjugate(yd2x2)',
                     texname = '\\text{I137x511}')

I137x512 = Parameter(name = 'I137x512',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd1x6*Rd5x2*Ru2x6*complexconjugate(yd2x2)',
                     texname = '\\text{I137x512}')

I137x513 = Parameter(name = 'I137x513',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x2*Rd1x6*Rd5x2*Ru3x5*complexconjugate(yd2x2)',
                     texname = '\\text{I137x513}')

I137x514 = Parameter(name = 'I137x514',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x2*Rd1x6*Rd5x2*Ru4x4*complexconjugate(yd2x2)',
                     texname = '\\text{I137x514}')

I137x521 = Parameter(name = 'I137x521',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd2x6*Rd5x2*Ru1x6*complexconjugate(yd2x2)',
                     texname = '\\text{I137x521}')

I137x522 = Parameter(name = 'I137x522',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x2*Rd2x6*Rd5x2*Ru2x6*complexconjugate(yd2x2)',
                     texname = '\\text{I137x522}')

I137x523 = Parameter(name = 'I137x523',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x2*Rd2x6*Rd5x2*Ru3x5*complexconjugate(yd2x2)',
                     texname = '\\text{I137x523}')

I137x524 = Parameter(name = 'I137x524',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x2*Rd2x6*Rd5x2*Ru4x4*complexconjugate(yd2x2)',
                     texname = '\\text{I137x524}')

I137x541 = Parameter(name = 'I137x541',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x2*Rd4x4*Rd5x2*Ru1x6*complexconjugate(yd2x2)',
                     texname = '\\text{I137x541}')

I137x542 = Parameter(name = 'I137x542',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x1x2*Rd4x4*Rd5x2*Ru2x6*complexconjugate(yd2x2)',
                     texname = '\\text{I137x542}')

I137x543 = Parameter(name = 'I137x543',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x1x2*Rd4x4*Rd5x2*Ru3x5*complexconjugate(yd2x2)',
                     texname = '\\text{I137x543}')

I137x544 = Parameter(name = 'I137x544',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x1x2*Rd4x4*Rd5x2*Ru4x4*complexconjugate(yd2x2)',
                     texname = '\\text{I137x544}')

I137x611 = Parameter(name = 'I137x611',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd1x6*Rd6x1*Ru1x6*complexconjugate(yd1x1)',
                     texname = '\\text{I137x611}')

I137x612 = Parameter(name = 'I137x612',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd1x6*Rd6x1*Ru2x6*complexconjugate(yd1x1)',
                     texname = '\\text{I137x612}')

I137x613 = Parameter(name = 'I137x613',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x1*Rd1x6*Rd6x1*Ru3x5*complexconjugate(yd1x1)',
                     texname = '\\text{I137x613}')

I137x614 = Parameter(name = 'I137x614',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x1*Rd1x6*Rd6x1*Ru4x4*complexconjugate(yd1x1)',
                     texname = '\\text{I137x614}')

I137x621 = Parameter(name = 'I137x621',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd2x6*Rd6x1*Ru1x6*complexconjugate(yd1x1)',
                     texname = '\\text{I137x621}')

I137x622 = Parameter(name = 'I137x622',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x3x1*Rd2x6*Rd6x1*Ru2x6*complexconjugate(yd1x1)',
                     texname = '\\text{I137x622}')

I137x623 = Parameter(name = 'I137x623',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x3x1*Rd2x6*Rd6x1*Ru3x5*complexconjugate(yd1x1)',
                     texname = '\\text{I137x623}')

I137x624 = Parameter(name = 'I137x624',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x3x1*Rd2x6*Rd6x1*Ru4x4*complexconjugate(yd1x1)',
                     texname = '\\text{I137x624}')

I137x631 = Parameter(name = 'I137x631',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x1*Rd3x5*Rd6x1*Ru1x6*complexconjugate(yd1x1)',
                     texname = '\\text{I137x631}')

I137x632 = Parameter(name = 'I137x632',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD3x2x1*Rd3x5*Rd6x1*Ru2x6*complexconjugate(yd1x1)',
                     texname = '\\text{I137x632}')

I137x633 = Parameter(name = 'I137x633',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD2x2x1*Rd3x5*Rd6x1*Ru3x5*complexconjugate(yd1x1)',
                     texname = '\\text{I137x633}')

I137x634 = Parameter(name = 'I137x634',
                     nature = 'internal',
                     type = 'complex',
                     value = 'LUDD1x2x1*Rd3x5*Rd6x1*Ru4x4*complexconjugate(yd1x1)',
                     texname = '\\text{I137x634}')

I138x131 = Parameter(name = 'I138x131',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd3x5*Ru1x6*TUDD3x3x2',
                     texname = '\\text{I138x131}')

I138x132 = Parameter(name = 'I138x132',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd3x5*Ru2x6*TUDD3x3x2',
                     texname = '\\text{I138x132}')

I138x133 = Parameter(name = 'I138x133',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd3x5*Ru3x5*TUDD2x3x2',
                     texname = '\\text{I138x133}')

I138x134 = Parameter(name = 'I138x134',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd3x5*Ru4x4*TUDD1x3x2',
                     texname = '\\text{I138x134}')

I138x141 = Parameter(name = 'I138x141',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd4x4*Ru1x6*TUDD3x3x1',
                     texname = '\\text{I138x141}')

I138x142 = Parameter(name = 'I138x142',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd4x4*Ru2x6*TUDD3x3x1',
                     texname = '\\text{I138x142}')

I138x143 = Parameter(name = 'I138x143',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd4x4*Ru3x5*TUDD2x3x1',
                     texname = '\\text{I138x143}')

I138x144 = Parameter(name = 'I138x144',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd4x4*Ru4x4*TUDD1x3x1',
                     texname = '\\text{I138x144}')

I138x231 = Parameter(name = 'I138x231',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd3x5*Ru1x6*TUDD3x3x2',
                     texname = '\\text{I138x231}')

I138x232 = Parameter(name = 'I138x232',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd3x5*Ru2x6*TUDD3x3x2',
                     texname = '\\text{I138x232}')

I138x233 = Parameter(name = 'I138x233',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd3x5*Ru3x5*TUDD2x3x2',
                     texname = '\\text{I138x233}')

I138x234 = Parameter(name = 'I138x234',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd3x5*Ru4x4*TUDD1x3x2',
                     texname = '\\text{I138x234}')

I138x241 = Parameter(name = 'I138x241',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd4x4*Ru1x6*TUDD3x3x1',
                     texname = '\\text{I138x241}')

I138x242 = Parameter(name = 'I138x242',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd4x4*Ru2x6*TUDD3x3x1',
                     texname = '\\text{I138x242}')

I138x243 = Parameter(name = 'I138x243',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd4x4*Ru3x5*TUDD2x3x1',
                     texname = '\\text{I138x243}')

I138x244 = Parameter(name = 'I138x244',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd4x4*Ru4x4*TUDD1x3x1',
                     texname = '\\text{I138x244}')

I138x311 = Parameter(name = 'I138x311',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd3x5*Ru1x6*TUDD3x2x3',
                     texname = '\\text{I138x311}')

I138x312 = Parameter(name = 'I138x312',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd3x5*Ru2x6*TUDD3x2x3',
                     texname = '\\text{I138x312}')

I138x313 = Parameter(name = 'I138x313',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd3x5*Ru3x5*TUDD2x2x3',
                     texname = '\\text{I138x313}')

I138x314 = Parameter(name = 'I138x314',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd3x5*Ru4x4*TUDD1x2x3',
                     texname = '\\text{I138x314}')

I138x321 = Parameter(name = 'I138x321',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd3x5*Ru1x6*TUDD3x2x3',
                     texname = '\\text{I138x321}')

I138x322 = Parameter(name = 'I138x322',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd3x5*Ru2x6*TUDD3x2x3',
                     texname = '\\text{I138x322}')

I138x323 = Parameter(name = 'I138x323',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd3x5*Ru3x5*TUDD2x2x3',
                     texname = '\\text{I138x323}')

I138x324 = Parameter(name = 'I138x324',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd3x5*Ru4x4*TUDD1x2x3',
                     texname = '\\text{I138x324}')

I138x341 = Parameter(name = 'I138x341',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd3x5*Rd4x4*Ru1x6*TUDD3x2x1',
                     texname = '\\text{I138x341}')

I138x342 = Parameter(name = 'I138x342',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd3x5*Rd4x4*Ru2x6*TUDD3x2x1',
                     texname = '\\text{I138x342}')

I138x343 = Parameter(name = 'I138x343',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd3x5*Rd4x4*Ru3x5*TUDD2x2x1',
                     texname = '\\text{I138x343}')

I138x344 = Parameter(name = 'I138x344',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd3x5*Rd4x4*Ru4x4*TUDD1x2x1',
                     texname = '\\text{I138x344}')

I138x411 = Parameter(name = 'I138x411',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd4x4*Ru1x6*TUDD3x1x3',
                     texname = '\\text{I138x411}')

I138x412 = Parameter(name = 'I138x412',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd4x4*Ru2x6*TUDD3x1x3',
                     texname = '\\text{I138x412}')

I138x413 = Parameter(name = 'I138x413',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd4x4*Ru3x5*TUDD2x1x3',
                     texname = '\\text{I138x413}')

I138x414 = Parameter(name = 'I138x414',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd4x4*Ru4x4*TUDD1x1x3',
                     texname = '\\text{I138x414}')

I138x421 = Parameter(name = 'I138x421',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd4x4*Ru1x6*TUDD3x1x3',
                     texname = '\\text{I138x421}')

I138x422 = Parameter(name = 'I138x422',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd4x4*Ru2x6*TUDD3x1x3',
                     texname = '\\text{I138x422}')

I138x423 = Parameter(name = 'I138x423',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd4x4*Ru3x5*TUDD2x1x3',
                     texname = '\\text{I138x423}')

I138x424 = Parameter(name = 'I138x424',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd4x4*Ru4x4*TUDD1x1x3',
                     texname = '\\text{I138x424}')

I138x431 = Parameter(name = 'I138x431',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd3x5*Rd4x4*Ru1x6*TUDD3x1x2',
                     texname = '\\text{I138x431}')

I138x432 = Parameter(name = 'I138x432',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd3x5*Rd4x4*Ru2x6*TUDD3x1x2',
                     texname = '\\text{I138x432}')

I138x433 = Parameter(name = 'I138x433',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd3x5*Rd4x4*Ru3x5*TUDD2x1x2',
                     texname = '\\text{I138x433}')

I138x434 = Parameter(name = 'I138x434',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd3x5*Rd4x4*Ru4x4*TUDD1x1x2',
                     texname = '\\text{I138x434}')

I139x131 = Parameter(name = 'I139x131',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd3x5*Ru1x6*TUDD3x2x3',
                     texname = '\\text{I139x131}')

I139x132 = Parameter(name = 'I139x132',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd3x5*Ru2x6*TUDD3x2x3',
                     texname = '\\text{I139x132}')

I139x133 = Parameter(name = 'I139x133',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd3x5*Ru3x5*TUDD2x2x3',
                     texname = '\\text{I139x133}')

I139x134 = Parameter(name = 'I139x134',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd3x5*Ru4x4*TUDD1x2x3',
                     texname = '\\text{I139x134}')

I139x141 = Parameter(name = 'I139x141',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd4x4*Ru1x6*TUDD3x1x3',
                     texname = '\\text{I139x141}')

I139x142 = Parameter(name = 'I139x142',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd4x4*Ru2x6*TUDD3x1x3',
                     texname = '\\text{I139x142}')

I139x143 = Parameter(name = 'I139x143',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd4x4*Ru3x5*TUDD2x1x3',
                     texname = '\\text{I139x143}')

I139x144 = Parameter(name = 'I139x144',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd4x4*Ru4x4*TUDD1x1x3',
                     texname = '\\text{I139x144}')

I139x231 = Parameter(name = 'I139x231',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd3x5*Ru1x6*TUDD3x2x3',
                     texname = '\\text{I139x231}')

I139x232 = Parameter(name = 'I139x232',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd3x5*Ru2x6*TUDD3x2x3',
                     texname = '\\text{I139x232}')

I139x233 = Parameter(name = 'I139x233',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd3x5*Ru3x5*TUDD2x2x3',
                     texname = '\\text{I139x233}')

I139x234 = Parameter(name = 'I139x234',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd3x5*Ru4x4*TUDD1x2x3',
                     texname = '\\text{I139x234}')

I139x241 = Parameter(name = 'I139x241',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd4x4*Ru1x6*TUDD3x1x3',
                     texname = '\\text{I139x241}')

I139x242 = Parameter(name = 'I139x242',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd4x4*Ru2x6*TUDD3x1x3',
                     texname = '\\text{I139x242}')

I139x243 = Parameter(name = 'I139x243',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd4x4*Ru3x5*TUDD2x1x3',
                     texname = '\\text{I139x243}')

I139x244 = Parameter(name = 'I139x244',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd4x4*Ru4x4*TUDD1x1x3',
                     texname = '\\text{I139x244}')

I139x311 = Parameter(name = 'I139x311',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd3x5*Ru1x6*TUDD3x3x2',
                     texname = '\\text{I139x311}')

I139x312 = Parameter(name = 'I139x312',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd3x5*Ru2x6*TUDD3x3x2',
                     texname = '\\text{I139x312}')

I139x313 = Parameter(name = 'I139x313',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd3x5*Ru3x5*TUDD2x3x2',
                     texname = '\\text{I139x313}')

I139x314 = Parameter(name = 'I139x314',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd3x5*Ru4x4*TUDD1x3x2',
                     texname = '\\text{I139x314}')

I139x321 = Parameter(name = 'I139x321',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd3x5*Ru1x6*TUDD3x3x2',
                     texname = '\\text{I139x321}')

I139x322 = Parameter(name = 'I139x322',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd3x5*Ru2x6*TUDD3x3x2',
                     texname = '\\text{I139x322}')

I139x323 = Parameter(name = 'I139x323',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd3x5*Ru3x5*TUDD2x3x2',
                     texname = '\\text{I139x323}')

I139x324 = Parameter(name = 'I139x324',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd3x5*Ru4x4*TUDD1x3x2',
                     texname = '\\text{I139x324}')

I139x341 = Parameter(name = 'I139x341',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd3x5*Rd4x4*Ru1x6*TUDD3x1x2',
                     texname = '\\text{I139x341}')

I139x342 = Parameter(name = 'I139x342',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd3x5*Rd4x4*Ru2x6*TUDD3x1x2',
                     texname = '\\text{I139x342}')

I139x343 = Parameter(name = 'I139x343',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd3x5*Rd4x4*Ru3x5*TUDD2x1x2',
                     texname = '\\text{I139x343}')

I139x344 = Parameter(name = 'I139x344',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd3x5*Rd4x4*Ru4x4*TUDD1x1x2',
                     texname = '\\text{I139x344}')

I139x411 = Parameter(name = 'I139x411',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd4x4*Ru1x6*TUDD3x3x1',
                     texname = '\\text{I139x411}')

I139x412 = Parameter(name = 'I139x412',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd4x4*Ru2x6*TUDD3x3x1',
                     texname = '\\text{I139x412}')

I139x413 = Parameter(name = 'I139x413',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd4x4*Ru3x5*TUDD2x3x1',
                     texname = '\\text{I139x413}')

I139x414 = Parameter(name = 'I139x414',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd1x6*Rd4x4*Ru4x4*TUDD1x3x1',
                     texname = '\\text{I139x414}')

I139x421 = Parameter(name = 'I139x421',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd4x4*Ru1x6*TUDD3x3x1',
                     texname = '\\text{I139x421}')

I139x422 = Parameter(name = 'I139x422',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd4x4*Ru2x6*TUDD3x3x1',
                     texname = '\\text{I139x422}')

I139x423 = Parameter(name = 'I139x423',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd4x4*Ru3x5*TUDD2x3x1',
                     texname = '\\text{I139x423}')

I139x424 = Parameter(name = 'I139x424',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd2x6*Rd4x4*Ru4x4*TUDD1x3x1',
                     texname = '\\text{I139x424}')

I139x431 = Parameter(name = 'I139x431',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd3x5*Rd4x4*Ru1x6*TUDD3x2x1',
                     texname = '\\text{I139x431}')

I139x432 = Parameter(name = 'I139x432',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd3x5*Rd4x4*Ru2x6*TUDD3x2x1',
                     texname = '\\text{I139x432}')

I139x433 = Parameter(name = 'I139x433',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd3x5*Rd4x4*Ru3x5*TUDD2x2x1',
                     texname = '\\text{I139x433}')

I139x434 = Parameter(name = 'I139x434',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rd3x5*Rd4x4*Ru4x4*TUDD1x2x1',
                     texname = '\\text{I139x434}')

I14x16 = Parameter(name = 'I14x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd6x1*complexconjugate(yd1x1)',
                   texname = '\\text{I14x16}')

I14x25 = Parameter(name = 'I14x25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd5x2*complexconjugate(yd2x2)',
                   texname = '\\text{I14x25}')

I14x31 = Parameter(name = 'I14x31',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*complexconjugate(yd3x3)',
                   texname = '\\text{I14x31}')

I14x32 = Parameter(name = 'I14x32',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*complexconjugate(yd3x3)',
                   texname = '\\text{I14x32}')

I140x111 = Parameter(name = 'I140x111',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru1x3*complexconjugate(Rd1x6)*complexconjugate(TLQD3x3x3)',
                     texname = '\\text{I140x111}')

I140x112 = Parameter(name = 'I140x112',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru2x3*complexconjugate(Rd1x6)*complexconjugate(TLQD3x3x3)',
                     texname = '\\text{I140x112}')

I140x115 = Parameter(name = 'I140x115',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru5x1*complexconjugate(Rd1x6)*complexconjugate(TLQD3x1x3)',
                     texname = '\\text{I140x115}')

I140x116 = Parameter(name = 'I140x116',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru6x2*complexconjugate(Rd1x6)*complexconjugate(TLQD3x2x3)',
                     texname = '\\text{I140x116}')

I140x141 = Parameter(name = 'I140x141',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru1x3*complexconjugate(Rd1x6)*complexconjugate(TLQD1x3x3)',
                     texname = '\\text{I140x141}')

I140x142 = Parameter(name = 'I140x142',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru2x3*complexconjugate(Rd1x6)*complexconjugate(TLQD1x3x3)',
                     texname = '\\text{I140x142}')

I140x145 = Parameter(name = 'I140x145',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru5x1*complexconjugate(Rd1x6)*complexconjugate(TLQD1x1x3)',
                     texname = '\\text{I140x145}')

I140x146 = Parameter(name = 'I140x146',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru6x2*complexconjugate(Rd1x6)*complexconjugate(TLQD1x2x3)',
                     texname = '\\text{I140x146}')

I140x151 = Parameter(name = 'I140x151',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru1x3*complexconjugate(Rd1x6)*complexconjugate(TLQD2x3x3)',
                     texname = '\\text{I140x151}')

I140x152 = Parameter(name = 'I140x152',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru2x3*complexconjugate(Rd1x6)*complexconjugate(TLQD2x3x3)',
                     texname = '\\text{I140x152}')

I140x155 = Parameter(name = 'I140x155',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru5x1*complexconjugate(Rd1x6)*complexconjugate(TLQD2x1x3)',
                     texname = '\\text{I140x155}')

I140x156 = Parameter(name = 'I140x156',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru6x2*complexconjugate(Rd1x6)*complexconjugate(TLQD2x2x3)',
                     texname = '\\text{I140x156}')

I140x161 = Parameter(name = 'I140x161',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru1x3*complexconjugate(Rd1x6)*complexconjugate(TLQD3x3x3)',
                     texname = '\\text{I140x161}')

I140x162 = Parameter(name = 'I140x162',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru2x3*complexconjugate(Rd1x6)*complexconjugate(TLQD3x3x3)',
                     texname = '\\text{I140x162}')

I140x165 = Parameter(name = 'I140x165',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru5x1*complexconjugate(Rd1x6)*complexconjugate(TLQD3x1x3)',
                     texname = '\\text{I140x165}')

I140x166 = Parameter(name = 'I140x166',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru6x2*complexconjugate(Rd1x6)*complexconjugate(TLQD3x2x3)',
                     texname = '\\text{I140x166}')

I140x211 = Parameter(name = 'I140x211',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru1x3*complexconjugate(Rd2x6)*complexconjugate(TLQD3x3x3)',
                     texname = '\\text{I140x211}')

I140x212 = Parameter(name = 'I140x212',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru2x3*complexconjugate(Rd2x6)*complexconjugate(TLQD3x3x3)',
                     texname = '\\text{I140x212}')

I140x215 = Parameter(name = 'I140x215',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru5x1*complexconjugate(Rd2x6)*complexconjugate(TLQD3x1x3)',
                     texname = '\\text{I140x215}')

I140x216 = Parameter(name = 'I140x216',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru6x2*complexconjugate(Rd2x6)*complexconjugate(TLQD3x2x3)',
                     texname = '\\text{I140x216}')

I140x241 = Parameter(name = 'I140x241',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru1x3*complexconjugate(Rd2x6)*complexconjugate(TLQD1x3x3)',
                     texname = '\\text{I140x241}')

I140x242 = Parameter(name = 'I140x242',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru2x3*complexconjugate(Rd2x6)*complexconjugate(TLQD1x3x3)',
                     texname = '\\text{I140x242}')

I140x245 = Parameter(name = 'I140x245',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru5x1*complexconjugate(Rd2x6)*complexconjugate(TLQD1x1x3)',
                     texname = '\\text{I140x245}')

I140x246 = Parameter(name = 'I140x246',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru6x2*complexconjugate(Rd2x6)*complexconjugate(TLQD1x2x3)',
                     texname = '\\text{I140x246}')

I140x251 = Parameter(name = 'I140x251',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru1x3*complexconjugate(Rd2x6)*complexconjugate(TLQD2x3x3)',
                     texname = '\\text{I140x251}')

I140x252 = Parameter(name = 'I140x252',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru2x3*complexconjugate(Rd2x6)*complexconjugate(TLQD2x3x3)',
                     texname = '\\text{I140x252}')

I140x255 = Parameter(name = 'I140x255',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru5x1*complexconjugate(Rd2x6)*complexconjugate(TLQD2x1x3)',
                     texname = '\\text{I140x255}')

I140x256 = Parameter(name = 'I140x256',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru6x2*complexconjugate(Rd2x6)*complexconjugate(TLQD2x2x3)',
                     texname = '\\text{I140x256}')

I140x261 = Parameter(name = 'I140x261',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru1x3*complexconjugate(Rd2x6)*complexconjugate(TLQD3x3x3)',
                     texname = '\\text{I140x261}')

I140x262 = Parameter(name = 'I140x262',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru2x3*complexconjugate(Rd2x6)*complexconjugate(TLQD3x3x3)',
                     texname = '\\text{I140x262}')

I140x265 = Parameter(name = 'I140x265',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru5x1*complexconjugate(Rd2x6)*complexconjugate(TLQD3x1x3)',
                     texname = '\\text{I140x265}')

I140x266 = Parameter(name = 'I140x266',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru6x2*complexconjugate(Rd2x6)*complexconjugate(TLQD3x2x3)',
                     texname = '\\text{I140x266}')

I140x311 = Parameter(name = 'I140x311',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru1x3*complexconjugate(Rd3x5)*complexconjugate(TLQD3x3x2)',
                     texname = '\\text{I140x311}')

I140x312 = Parameter(name = 'I140x312',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru2x3*complexconjugate(Rd3x5)*complexconjugate(TLQD3x3x2)',
                     texname = '\\text{I140x312}')

I140x315 = Parameter(name = 'I140x315',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru5x1*complexconjugate(Rd3x5)*complexconjugate(TLQD3x1x2)',
                     texname = '\\text{I140x315}')

I140x316 = Parameter(name = 'I140x316',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru6x2*complexconjugate(Rd3x5)*complexconjugate(TLQD3x2x2)',
                     texname = '\\text{I140x316}')

I140x341 = Parameter(name = 'I140x341',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru1x3*complexconjugate(Rd3x5)*complexconjugate(TLQD1x3x2)',
                     texname = '\\text{I140x341}')

I140x342 = Parameter(name = 'I140x342',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru2x3*complexconjugate(Rd3x5)*complexconjugate(TLQD1x3x2)',
                     texname = '\\text{I140x342}')

I140x345 = Parameter(name = 'I140x345',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru5x1*complexconjugate(Rd3x5)*complexconjugate(TLQD1x1x2)',
                     texname = '\\text{I140x345}')

I140x346 = Parameter(name = 'I140x346',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru6x2*complexconjugate(Rd3x5)*complexconjugate(TLQD1x2x2)',
                     texname = '\\text{I140x346}')

I140x351 = Parameter(name = 'I140x351',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru1x3*complexconjugate(Rd3x5)*complexconjugate(TLQD2x3x2)',
                     texname = '\\text{I140x351}')

I140x352 = Parameter(name = 'I140x352',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru2x3*complexconjugate(Rd3x5)*complexconjugate(TLQD2x3x2)',
                     texname = '\\text{I140x352}')

I140x355 = Parameter(name = 'I140x355',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru5x1*complexconjugate(Rd3x5)*complexconjugate(TLQD2x1x2)',
                     texname = '\\text{I140x355}')

I140x356 = Parameter(name = 'I140x356',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru6x2*complexconjugate(Rd3x5)*complexconjugate(TLQD2x2x2)',
                     texname = '\\text{I140x356}')

I140x361 = Parameter(name = 'I140x361',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru1x3*complexconjugate(Rd3x5)*complexconjugate(TLQD3x3x2)',
                     texname = '\\text{I140x361}')

I140x362 = Parameter(name = 'I140x362',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru2x3*complexconjugate(Rd3x5)*complexconjugate(TLQD3x3x2)',
                     texname = '\\text{I140x362}')

I140x365 = Parameter(name = 'I140x365',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru5x1*complexconjugate(Rd3x5)*complexconjugate(TLQD3x1x2)',
                     texname = '\\text{I140x365}')

I140x366 = Parameter(name = 'I140x366',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru6x2*complexconjugate(Rd3x5)*complexconjugate(TLQD3x2x2)',
                     texname = '\\text{I140x366}')

I140x411 = Parameter(name = 'I140x411',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru1x3*complexconjugate(Rd4x4)*complexconjugate(TLQD3x3x1)',
                     texname = '\\text{I140x411}')

I140x412 = Parameter(name = 'I140x412',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru2x3*complexconjugate(Rd4x4)*complexconjugate(TLQD3x3x1)',
                     texname = '\\text{I140x412}')

I140x415 = Parameter(name = 'I140x415',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru5x1*complexconjugate(Rd4x4)*complexconjugate(TLQD3x1x1)',
                     texname = '\\text{I140x415}')

I140x416 = Parameter(name = 'I140x416',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru6x2*complexconjugate(Rd4x4)*complexconjugate(TLQD3x2x1)',
                     texname = '\\text{I140x416}')

I140x441 = Parameter(name = 'I140x441',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru1x3*complexconjugate(Rd4x4)*complexconjugate(TLQD1x3x1)',
                     texname = '\\text{I140x441}')

I140x442 = Parameter(name = 'I140x442',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru2x3*complexconjugate(Rd4x4)*complexconjugate(TLQD1x3x1)',
                     texname = '\\text{I140x442}')

I140x445 = Parameter(name = 'I140x445',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru5x1*complexconjugate(Rd4x4)*complexconjugate(TLQD1x1x1)',
                     texname = '\\text{I140x445}')

I140x446 = Parameter(name = 'I140x446',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru6x2*complexconjugate(Rd4x4)*complexconjugate(TLQD1x2x1)',
                     texname = '\\text{I140x446}')

I140x451 = Parameter(name = 'I140x451',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru1x3*complexconjugate(Rd4x4)*complexconjugate(TLQD2x3x1)',
                     texname = '\\text{I140x451}')

I140x452 = Parameter(name = 'I140x452',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru2x3*complexconjugate(Rd4x4)*complexconjugate(TLQD2x3x1)',
                     texname = '\\text{I140x452}')

I140x455 = Parameter(name = 'I140x455',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru5x1*complexconjugate(Rd4x4)*complexconjugate(TLQD2x1x1)',
                     texname = '\\text{I140x455}')

I140x456 = Parameter(name = 'I140x456',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru6x2*complexconjugate(Rd4x4)*complexconjugate(TLQD2x2x1)',
                     texname = '\\text{I140x456}')

I140x461 = Parameter(name = 'I140x461',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru1x3*complexconjugate(Rd4x4)*complexconjugate(TLQD3x3x1)',
                     texname = '\\text{I140x461}')

I140x462 = Parameter(name = 'I140x462',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru2x3*complexconjugate(Rd4x4)*complexconjugate(TLQD3x3x1)',
                     texname = '\\text{I140x462}')

I140x465 = Parameter(name = 'I140x465',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru5x1*complexconjugate(Rd4x4)*complexconjugate(TLQD3x1x1)',
                     texname = '\\text{I140x465}')

I140x466 = Parameter(name = 'I140x466',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru6x2*complexconjugate(Rd4x4)*complexconjugate(TLQD3x2x1)',
                     texname = '\\text{I140x466}')

I141x111 = Parameter(name = 'I141x111',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru1x3*yd3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd1x3)',
                     texname = '\\text{I141x111}')

I141x112 = Parameter(name = 'I141x112',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru2x3*yd3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd1x3)',
                     texname = '\\text{I141x112}')

I141x115 = Parameter(name = 'I141x115',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru5x1*yd3x3*complexconjugate(LLQD3x1x3)*complexconjugate(Rd1x3)',
                     texname = '\\text{I141x115}')

I141x116 = Parameter(name = 'I141x116',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru6x2*yd3x3*complexconjugate(LLQD3x2x3)*complexconjugate(Rd1x3)',
                     texname = '\\text{I141x116}')

I141x141 = Parameter(name = 'I141x141',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru1x3*yd3x3*complexconjugate(LLQD1x3x3)*complexconjugate(Rd1x3)',
                     texname = '\\text{I141x141}')

I141x142 = Parameter(name = 'I141x142',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru2x3*yd3x3*complexconjugate(LLQD1x3x3)*complexconjugate(Rd1x3)',
                     texname = '\\text{I141x142}')

I141x145 = Parameter(name = 'I141x145',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru5x1*yd3x3*complexconjugate(LLQD1x1x3)*complexconjugate(Rd1x3)',
                     texname = '\\text{I141x145}')

I141x146 = Parameter(name = 'I141x146',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru6x2*yd3x3*complexconjugate(LLQD1x2x3)*complexconjugate(Rd1x3)',
                     texname = '\\text{I141x146}')

I141x151 = Parameter(name = 'I141x151',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru1x3*yd3x3*complexconjugate(LLQD2x3x3)*complexconjugate(Rd1x3)',
                     texname = '\\text{I141x151}')

I141x152 = Parameter(name = 'I141x152',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru2x3*yd3x3*complexconjugate(LLQD2x3x3)*complexconjugate(Rd1x3)',
                     texname = '\\text{I141x152}')

I141x155 = Parameter(name = 'I141x155',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru5x1*yd3x3*complexconjugate(LLQD2x1x3)*complexconjugate(Rd1x3)',
                     texname = '\\text{I141x155}')

I141x156 = Parameter(name = 'I141x156',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru6x2*yd3x3*complexconjugate(LLQD2x2x3)*complexconjugate(Rd1x3)',
                     texname = '\\text{I141x156}')

I141x161 = Parameter(name = 'I141x161',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru1x3*yd3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd1x3)',
                     texname = '\\text{I141x161}')

I141x162 = Parameter(name = 'I141x162',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru2x3*yd3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd1x3)',
                     texname = '\\text{I141x162}')

I141x165 = Parameter(name = 'I141x165',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru5x1*yd3x3*complexconjugate(LLQD3x1x3)*complexconjugate(Rd1x3)',
                     texname = '\\text{I141x165}')

I141x166 = Parameter(name = 'I141x166',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru6x2*yd3x3*complexconjugate(LLQD3x2x3)*complexconjugate(Rd1x3)',
                     texname = '\\text{I141x166}')

I141x211 = Parameter(name = 'I141x211',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru1x3*yd3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd2x3)',
                     texname = '\\text{I141x211}')

I141x212 = Parameter(name = 'I141x212',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru2x3*yd3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd2x3)',
                     texname = '\\text{I141x212}')

I141x215 = Parameter(name = 'I141x215',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru5x1*yd3x3*complexconjugate(LLQD3x1x3)*complexconjugate(Rd2x3)',
                     texname = '\\text{I141x215}')

I141x216 = Parameter(name = 'I141x216',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru6x2*yd3x3*complexconjugate(LLQD3x2x3)*complexconjugate(Rd2x3)',
                     texname = '\\text{I141x216}')

I141x241 = Parameter(name = 'I141x241',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru1x3*yd3x3*complexconjugate(LLQD1x3x3)*complexconjugate(Rd2x3)',
                     texname = '\\text{I141x241}')

I141x242 = Parameter(name = 'I141x242',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru2x3*yd3x3*complexconjugate(LLQD1x3x3)*complexconjugate(Rd2x3)',
                     texname = '\\text{I141x242}')

I141x245 = Parameter(name = 'I141x245',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru5x1*yd3x3*complexconjugate(LLQD1x1x3)*complexconjugate(Rd2x3)',
                     texname = '\\text{I141x245}')

I141x246 = Parameter(name = 'I141x246',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru6x2*yd3x3*complexconjugate(LLQD1x2x3)*complexconjugate(Rd2x3)',
                     texname = '\\text{I141x246}')

I141x251 = Parameter(name = 'I141x251',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru1x3*yd3x3*complexconjugate(LLQD2x3x3)*complexconjugate(Rd2x3)',
                     texname = '\\text{I141x251}')

I141x252 = Parameter(name = 'I141x252',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru2x3*yd3x3*complexconjugate(LLQD2x3x3)*complexconjugate(Rd2x3)',
                     texname = '\\text{I141x252}')

I141x255 = Parameter(name = 'I141x255',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru5x1*yd3x3*complexconjugate(LLQD2x1x3)*complexconjugate(Rd2x3)',
                     texname = '\\text{I141x255}')

I141x256 = Parameter(name = 'I141x256',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru6x2*yd3x3*complexconjugate(LLQD2x2x3)*complexconjugate(Rd2x3)',
                     texname = '\\text{I141x256}')

I141x261 = Parameter(name = 'I141x261',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru1x3*yd3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd2x3)',
                     texname = '\\text{I141x261}')

I141x262 = Parameter(name = 'I141x262',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru2x3*yd3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd2x3)',
                     texname = '\\text{I141x262}')

I141x265 = Parameter(name = 'I141x265',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru5x1*yd3x3*complexconjugate(LLQD3x1x3)*complexconjugate(Rd2x3)',
                     texname = '\\text{I141x265}')

I141x266 = Parameter(name = 'I141x266',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru6x2*yd3x3*complexconjugate(LLQD3x2x3)*complexconjugate(Rd2x3)',
                     texname = '\\text{I141x266}')

I141x511 = Parameter(name = 'I141x511',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru1x3*yd2x2*complexconjugate(LLQD3x3x2)*complexconjugate(Rd5x2)',
                     texname = '\\text{I141x511}')

I141x512 = Parameter(name = 'I141x512',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru2x3*yd2x2*complexconjugate(LLQD3x3x2)*complexconjugate(Rd5x2)',
                     texname = '\\text{I141x512}')

I141x515 = Parameter(name = 'I141x515',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru5x1*yd2x2*complexconjugate(LLQD3x1x2)*complexconjugate(Rd5x2)',
                     texname = '\\text{I141x515}')

I141x516 = Parameter(name = 'I141x516',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru6x2*yd2x2*complexconjugate(LLQD3x2x2)*complexconjugate(Rd5x2)',
                     texname = '\\text{I141x516}')

I141x541 = Parameter(name = 'I141x541',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru1x3*yd2x2*complexconjugate(LLQD1x3x2)*complexconjugate(Rd5x2)',
                     texname = '\\text{I141x541}')

I141x542 = Parameter(name = 'I141x542',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru2x3*yd2x2*complexconjugate(LLQD1x3x2)*complexconjugate(Rd5x2)',
                     texname = '\\text{I141x542}')

I141x545 = Parameter(name = 'I141x545',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru5x1*yd2x2*complexconjugate(LLQD1x1x2)*complexconjugate(Rd5x2)',
                     texname = '\\text{I141x545}')

I141x546 = Parameter(name = 'I141x546',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru6x2*yd2x2*complexconjugate(LLQD1x2x2)*complexconjugate(Rd5x2)',
                     texname = '\\text{I141x546}')

I141x551 = Parameter(name = 'I141x551',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru1x3*yd2x2*complexconjugate(LLQD2x3x2)*complexconjugate(Rd5x2)',
                     texname = '\\text{I141x551}')

I141x552 = Parameter(name = 'I141x552',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru2x3*yd2x2*complexconjugate(LLQD2x3x2)*complexconjugate(Rd5x2)',
                     texname = '\\text{I141x552}')

I141x555 = Parameter(name = 'I141x555',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru5x1*yd2x2*complexconjugate(LLQD2x1x2)*complexconjugate(Rd5x2)',
                     texname = '\\text{I141x555}')

I141x556 = Parameter(name = 'I141x556',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru6x2*yd2x2*complexconjugate(LLQD2x2x2)*complexconjugate(Rd5x2)',
                     texname = '\\text{I141x556}')

I141x561 = Parameter(name = 'I141x561',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru1x3*yd2x2*complexconjugate(LLQD3x3x2)*complexconjugate(Rd5x2)',
                     texname = '\\text{I141x561}')

I141x562 = Parameter(name = 'I141x562',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru2x3*yd2x2*complexconjugate(LLQD3x3x2)*complexconjugate(Rd5x2)',
                     texname = '\\text{I141x562}')

I141x565 = Parameter(name = 'I141x565',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru5x1*yd2x2*complexconjugate(LLQD3x1x2)*complexconjugate(Rd5x2)',
                     texname = '\\text{I141x565}')

I141x566 = Parameter(name = 'I141x566',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru6x2*yd2x2*complexconjugate(LLQD3x2x2)*complexconjugate(Rd5x2)',
                     texname = '\\text{I141x566}')

I141x611 = Parameter(name = 'I141x611',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru1x3*yd1x1*complexconjugate(LLQD3x3x1)*complexconjugate(Rd6x1)',
                     texname = '\\text{I141x611}')

I141x612 = Parameter(name = 'I141x612',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru2x3*yd1x1*complexconjugate(LLQD3x3x1)*complexconjugate(Rd6x1)',
                     texname = '\\text{I141x612}')

I141x615 = Parameter(name = 'I141x615',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru5x1*yd1x1*complexconjugate(LLQD3x1x1)*complexconjugate(Rd6x1)',
                     texname = '\\text{I141x615}')

I141x616 = Parameter(name = 'I141x616',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru6x2*yd1x1*complexconjugate(LLQD3x2x1)*complexconjugate(Rd6x1)',
                     texname = '\\text{I141x616}')

I141x641 = Parameter(name = 'I141x641',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru1x3*yd1x1*complexconjugate(LLQD1x3x1)*complexconjugate(Rd6x1)',
                     texname = '\\text{I141x641}')

I141x642 = Parameter(name = 'I141x642',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru2x3*yd1x1*complexconjugate(LLQD1x3x1)*complexconjugate(Rd6x1)',
                     texname = '\\text{I141x642}')

I141x645 = Parameter(name = 'I141x645',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru5x1*yd1x1*complexconjugate(LLQD1x1x1)*complexconjugate(Rd6x1)',
                     texname = '\\text{I141x645}')

I141x646 = Parameter(name = 'I141x646',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru6x2*yd1x1*complexconjugate(LLQD1x2x1)*complexconjugate(Rd6x1)',
                     texname = '\\text{I141x646}')

I141x651 = Parameter(name = 'I141x651',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru1x3*yd1x1*complexconjugate(LLQD2x3x1)*complexconjugate(Rd6x1)',
                     texname = '\\text{I141x651}')

I141x652 = Parameter(name = 'I141x652',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru2x3*yd1x1*complexconjugate(LLQD2x3x1)*complexconjugate(Rd6x1)',
                     texname = '\\text{I141x652}')

I141x655 = Parameter(name = 'I141x655',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru5x1*yd1x1*complexconjugate(LLQD2x1x1)*complexconjugate(Rd6x1)',
                     texname = '\\text{I141x655}')

I141x656 = Parameter(name = 'I141x656',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru6x2*yd1x1*complexconjugate(LLQD2x2x1)*complexconjugate(Rd6x1)',
                     texname = '\\text{I141x656}')

I141x661 = Parameter(name = 'I141x661',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru1x3*yd1x1*complexconjugate(LLQD3x3x1)*complexconjugate(Rd6x1)',
                     texname = '\\text{I141x661}')

I141x662 = Parameter(name = 'I141x662',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru2x3*yd1x1*complexconjugate(LLQD3x3x1)*complexconjugate(Rd6x1)',
                     texname = '\\text{I141x662}')

I141x665 = Parameter(name = 'I141x665',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru5x1*yd1x1*complexconjugate(LLQD3x1x1)*complexconjugate(Rd6x1)',
                     texname = '\\text{I141x665}')

I141x666 = Parameter(name = 'I141x666',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru6x2*yd1x1*complexconjugate(LLQD3x2x1)*complexconjugate(Rd6x1)',
                     texname = '\\text{I141x666}')

I142x111 = Parameter(name = 'I142x111',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x6*Ru1x3*ye3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I142x111}')

I142x112 = Parameter(name = 'I142x112',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x6*Ru2x3*ye3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I142x112}')

I142x115 = Parameter(name = 'I142x115',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x6*Ru5x1*ye3x3*complexconjugate(LLQD3x1x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I142x115}')

I142x116 = Parameter(name = 'I142x116',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x6*Ru6x2*ye3x3*complexconjugate(LLQD3x2x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I142x116}')

I142x121 = Parameter(name = 'I142x121',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl2x5*Ru1x3*ye2x2*complexconjugate(LLQD2x3x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I142x121}')

I142x122 = Parameter(name = 'I142x122',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl2x5*Ru2x3*ye2x2*complexconjugate(LLQD2x3x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I142x122}')

I142x125 = Parameter(name = 'I142x125',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl2x5*Ru5x1*ye2x2*complexconjugate(LLQD2x1x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I142x125}')

I142x126 = Parameter(name = 'I142x126',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl2x5*Ru6x2*ye2x2*complexconjugate(LLQD2x2x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I142x126}')

I142x131 = Parameter(name = 'I142x131',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl3x4*Ru1x3*ye1x1*complexconjugate(LLQD1x3x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I142x131}')

I142x132 = Parameter(name = 'I142x132',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl3x4*Ru2x3*ye1x1*complexconjugate(LLQD1x3x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I142x132}')

I142x135 = Parameter(name = 'I142x135',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl3x4*Ru5x1*ye1x1*complexconjugate(LLQD1x1x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I142x135}')

I142x136 = Parameter(name = 'I142x136',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl3x4*Ru6x2*ye1x1*complexconjugate(LLQD1x2x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I142x136}')

I142x161 = Parameter(name = 'I142x161',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x6*Ru1x3*ye3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I142x161}')

I142x162 = Parameter(name = 'I142x162',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x6*Ru2x3*ye3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I142x162}')

I142x165 = Parameter(name = 'I142x165',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x6*Ru5x1*ye3x3*complexconjugate(LLQD3x1x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I142x165}')

I142x166 = Parameter(name = 'I142x166',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x6*Ru6x2*ye3x3*complexconjugate(LLQD3x2x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I142x166}')

I142x211 = Parameter(name = 'I142x211',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x6*Ru1x3*ye3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I142x211}')

I142x212 = Parameter(name = 'I142x212',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x6*Ru2x3*ye3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I142x212}')

I142x215 = Parameter(name = 'I142x215',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x6*Ru5x1*ye3x3*complexconjugate(LLQD3x1x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I142x215}')

I142x216 = Parameter(name = 'I142x216',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x6*Ru6x2*ye3x3*complexconjugate(LLQD3x2x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I142x216}')

I142x221 = Parameter(name = 'I142x221',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl2x5*Ru1x3*ye2x2*complexconjugate(LLQD2x3x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I142x221}')

I142x222 = Parameter(name = 'I142x222',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl2x5*Ru2x3*ye2x2*complexconjugate(LLQD2x3x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I142x222}')

I142x225 = Parameter(name = 'I142x225',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl2x5*Ru5x1*ye2x2*complexconjugate(LLQD2x1x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I142x225}')

I142x226 = Parameter(name = 'I142x226',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl2x5*Ru6x2*ye2x2*complexconjugate(LLQD2x2x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I142x226}')

I142x231 = Parameter(name = 'I142x231',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl3x4*Ru1x3*ye1x1*complexconjugate(LLQD1x3x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I142x231}')

I142x232 = Parameter(name = 'I142x232',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl3x4*Ru2x3*ye1x1*complexconjugate(LLQD1x3x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I142x232}')

I142x235 = Parameter(name = 'I142x235',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl3x4*Ru5x1*ye1x1*complexconjugate(LLQD1x1x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I142x235}')

I142x236 = Parameter(name = 'I142x236',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl3x4*Ru6x2*ye1x1*complexconjugate(LLQD1x2x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I142x236}')

I142x261 = Parameter(name = 'I142x261',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x6*Ru1x3*ye3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I142x261}')

I142x262 = Parameter(name = 'I142x262',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x6*Ru2x3*ye3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I142x262}')

I142x265 = Parameter(name = 'I142x265',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x6*Ru5x1*ye3x3*complexconjugate(LLQD3x1x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I142x265}')

I142x266 = Parameter(name = 'I142x266',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x6*Ru6x2*ye3x3*complexconjugate(LLQD3x2x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I142x266}')

I142x311 = Parameter(name = 'I142x311',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x6*Ru1x3*ye3x3*complexconjugate(LLQD3x3x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I142x311}')

I142x312 = Parameter(name = 'I142x312',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x6*Ru2x3*ye3x3*complexconjugate(LLQD3x3x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I142x312}')

I142x315 = Parameter(name = 'I142x315',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x6*Ru5x1*ye3x3*complexconjugate(LLQD3x1x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I142x315}')

I142x316 = Parameter(name = 'I142x316',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x6*Ru6x2*ye3x3*complexconjugate(LLQD3x2x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I142x316}')

I142x321 = Parameter(name = 'I142x321',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl2x5*Ru1x3*ye2x2*complexconjugate(LLQD2x3x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I142x321}')

I142x322 = Parameter(name = 'I142x322',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl2x5*Ru2x3*ye2x2*complexconjugate(LLQD2x3x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I142x322}')

I142x325 = Parameter(name = 'I142x325',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl2x5*Ru5x1*ye2x2*complexconjugate(LLQD2x1x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I142x325}')

I142x326 = Parameter(name = 'I142x326',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl2x5*Ru6x2*ye2x2*complexconjugate(LLQD2x2x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I142x326}')

I142x331 = Parameter(name = 'I142x331',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl3x4*Ru1x3*ye1x1*complexconjugate(LLQD1x3x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I142x331}')

I142x332 = Parameter(name = 'I142x332',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl3x4*Ru2x3*ye1x1*complexconjugate(LLQD1x3x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I142x332}')

I142x335 = Parameter(name = 'I142x335',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl3x4*Ru5x1*ye1x1*complexconjugate(LLQD1x1x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I142x335}')

I142x336 = Parameter(name = 'I142x336',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl3x4*Ru6x2*ye1x1*complexconjugate(LLQD1x2x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I142x336}')

I142x361 = Parameter(name = 'I142x361',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x6*Ru1x3*ye3x3*complexconjugate(LLQD3x3x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I142x361}')

I142x362 = Parameter(name = 'I142x362',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x6*Ru2x3*ye3x3*complexconjugate(LLQD3x3x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I142x362}')

I142x365 = Parameter(name = 'I142x365',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x6*Ru5x1*ye3x3*complexconjugate(LLQD3x1x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I142x365}')

I142x366 = Parameter(name = 'I142x366',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x6*Ru6x2*ye3x3*complexconjugate(LLQD3x2x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I142x366}')

I142x411 = Parameter(name = 'I142x411',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x6*Ru1x3*ye3x3*complexconjugate(LLQD3x3x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I142x411}')

I142x412 = Parameter(name = 'I142x412',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x6*Ru2x3*ye3x3*complexconjugate(LLQD3x3x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I142x412}')

I142x415 = Parameter(name = 'I142x415',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x6*Ru5x1*ye3x3*complexconjugate(LLQD3x1x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I142x415}')

I142x416 = Parameter(name = 'I142x416',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x6*Ru6x2*ye3x3*complexconjugate(LLQD3x2x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I142x416}')

I142x421 = Parameter(name = 'I142x421',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl2x5*Ru1x3*ye2x2*complexconjugate(LLQD2x3x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I142x421}')

I142x422 = Parameter(name = 'I142x422',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl2x5*Ru2x3*ye2x2*complexconjugate(LLQD2x3x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I142x422}')

I142x425 = Parameter(name = 'I142x425',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl2x5*Ru5x1*ye2x2*complexconjugate(LLQD2x1x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I142x425}')

I142x426 = Parameter(name = 'I142x426',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl2x5*Ru6x2*ye2x2*complexconjugate(LLQD2x2x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I142x426}')

I142x431 = Parameter(name = 'I142x431',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl3x4*Ru1x3*ye1x1*complexconjugate(LLQD1x3x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I142x431}')

I142x432 = Parameter(name = 'I142x432',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl3x4*Ru2x3*ye1x1*complexconjugate(LLQD1x3x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I142x432}')

I142x435 = Parameter(name = 'I142x435',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl3x4*Ru5x1*ye1x1*complexconjugate(LLQD1x1x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I142x435}')

I142x436 = Parameter(name = 'I142x436',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl3x4*Ru6x2*ye1x1*complexconjugate(LLQD1x2x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I142x436}')

I142x461 = Parameter(name = 'I142x461',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x6*Ru1x3*ye3x3*complexconjugate(LLQD3x3x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I142x461}')

I142x462 = Parameter(name = 'I142x462',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x6*Ru2x3*ye3x3*complexconjugate(LLQD3x3x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I142x462}')

I142x465 = Parameter(name = 'I142x465',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x6*Ru5x1*ye3x3*complexconjugate(LLQD3x1x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I142x465}')

I142x466 = Parameter(name = 'I142x466',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x6*Ru6x2*ye3x3*complexconjugate(LLQD3x2x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I142x466}')

I143x111 = Parameter(name = 'I143x111',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru1x6*yu3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I143x111}')

I143x112 = Parameter(name = 'I143x112',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru2x6*yu3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I143x112}')

I143x113 = Parameter(name = 'I143x113',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru3x5*yu2x2*complexconjugate(LLQD3x2x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I143x113}')

I143x114 = Parameter(name = 'I143x114',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru4x4*yu1x1*complexconjugate(LLQD3x1x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I143x114}')

I143x141 = Parameter(name = 'I143x141',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru1x6*yu3x3*complexconjugate(LLQD1x3x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I143x141}')

I143x142 = Parameter(name = 'I143x142',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru2x6*yu3x3*complexconjugate(LLQD1x3x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I143x142}')

I143x143 = Parameter(name = 'I143x143',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru3x5*yu2x2*complexconjugate(LLQD1x2x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I143x143}')

I143x144 = Parameter(name = 'I143x144',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru4x4*yu1x1*complexconjugate(LLQD1x1x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I143x144}')

I143x151 = Parameter(name = 'I143x151',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru1x6*yu3x3*complexconjugate(LLQD2x3x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I143x151}')

I143x152 = Parameter(name = 'I143x152',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru2x6*yu3x3*complexconjugate(LLQD2x3x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I143x152}')

I143x153 = Parameter(name = 'I143x153',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru3x5*yu2x2*complexconjugate(LLQD2x2x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I143x153}')

I143x154 = Parameter(name = 'I143x154',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru4x4*yu1x1*complexconjugate(LLQD2x1x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I143x154}')

I143x161 = Parameter(name = 'I143x161',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru1x6*yu3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I143x161}')

I143x162 = Parameter(name = 'I143x162',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru2x6*yu3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I143x162}')

I143x163 = Parameter(name = 'I143x163',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru3x5*yu2x2*complexconjugate(LLQD3x2x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I143x163}')

I143x164 = Parameter(name = 'I143x164',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru4x4*yu1x1*complexconjugate(LLQD3x1x3)*complexconjugate(Rd1x6)',
                     texname = '\\text{I143x164}')

I143x211 = Parameter(name = 'I143x211',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru1x6*yu3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I143x211}')

I143x212 = Parameter(name = 'I143x212',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru2x6*yu3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I143x212}')

I143x213 = Parameter(name = 'I143x213',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru3x5*yu2x2*complexconjugate(LLQD3x2x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I143x213}')

I143x214 = Parameter(name = 'I143x214',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru4x4*yu1x1*complexconjugate(LLQD3x1x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I143x214}')

I143x241 = Parameter(name = 'I143x241',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru1x6*yu3x3*complexconjugate(LLQD1x3x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I143x241}')

I143x242 = Parameter(name = 'I143x242',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru2x6*yu3x3*complexconjugate(LLQD1x3x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I143x242}')

I143x243 = Parameter(name = 'I143x243',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru3x5*yu2x2*complexconjugate(LLQD1x2x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I143x243}')

I143x244 = Parameter(name = 'I143x244',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru4x4*yu1x1*complexconjugate(LLQD1x1x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I143x244}')

I143x251 = Parameter(name = 'I143x251',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru1x6*yu3x3*complexconjugate(LLQD2x3x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I143x251}')

I143x252 = Parameter(name = 'I143x252',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru2x6*yu3x3*complexconjugate(LLQD2x3x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I143x252}')

I143x253 = Parameter(name = 'I143x253',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru3x5*yu2x2*complexconjugate(LLQD2x2x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I143x253}')

I143x254 = Parameter(name = 'I143x254',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru4x4*yu1x1*complexconjugate(LLQD2x1x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I143x254}')

I143x261 = Parameter(name = 'I143x261',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru1x6*yu3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I143x261}')

I143x262 = Parameter(name = 'I143x262',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru2x6*yu3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I143x262}')

I143x263 = Parameter(name = 'I143x263',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru3x5*yu2x2*complexconjugate(LLQD3x2x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I143x263}')

I143x264 = Parameter(name = 'I143x264',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru4x4*yu1x1*complexconjugate(LLQD3x1x3)*complexconjugate(Rd2x6)',
                     texname = '\\text{I143x264}')

I143x311 = Parameter(name = 'I143x311',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru1x6*yu3x3*complexconjugate(LLQD3x3x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I143x311}')

I143x312 = Parameter(name = 'I143x312',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru2x6*yu3x3*complexconjugate(LLQD3x3x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I143x312}')

I143x313 = Parameter(name = 'I143x313',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru3x5*yu2x2*complexconjugate(LLQD3x2x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I143x313}')

I143x314 = Parameter(name = 'I143x314',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru4x4*yu1x1*complexconjugate(LLQD3x1x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I143x314}')

I143x341 = Parameter(name = 'I143x341',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru1x6*yu3x3*complexconjugate(LLQD1x3x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I143x341}')

I143x342 = Parameter(name = 'I143x342',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru2x6*yu3x3*complexconjugate(LLQD1x3x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I143x342}')

I143x343 = Parameter(name = 'I143x343',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru3x5*yu2x2*complexconjugate(LLQD1x2x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I143x343}')

I143x344 = Parameter(name = 'I143x344',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru4x4*yu1x1*complexconjugate(LLQD1x1x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I143x344}')

I143x351 = Parameter(name = 'I143x351',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru1x6*yu3x3*complexconjugate(LLQD2x3x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I143x351}')

I143x352 = Parameter(name = 'I143x352',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru2x6*yu3x3*complexconjugate(LLQD2x3x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I143x352}')

I143x353 = Parameter(name = 'I143x353',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru3x5*yu2x2*complexconjugate(LLQD2x2x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I143x353}')

I143x354 = Parameter(name = 'I143x354',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru4x4*yu1x1*complexconjugate(LLQD2x1x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I143x354}')

I143x361 = Parameter(name = 'I143x361',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru1x6*yu3x3*complexconjugate(LLQD3x3x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I143x361}')

I143x362 = Parameter(name = 'I143x362',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru2x6*yu3x3*complexconjugate(LLQD3x3x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I143x362}')

I143x363 = Parameter(name = 'I143x363',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru3x5*yu2x2*complexconjugate(LLQD3x2x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I143x363}')

I143x364 = Parameter(name = 'I143x364',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru4x4*yu1x1*complexconjugate(LLQD3x1x2)*complexconjugate(Rd3x5)',
                     texname = '\\text{I143x364}')

I143x411 = Parameter(name = 'I143x411',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru1x6*yu3x3*complexconjugate(LLQD3x3x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I143x411}')

I143x412 = Parameter(name = 'I143x412',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru2x6*yu3x3*complexconjugate(LLQD3x3x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I143x412}')

I143x413 = Parameter(name = 'I143x413',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru3x5*yu2x2*complexconjugate(LLQD3x2x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I143x413}')

I143x414 = Parameter(name = 'I143x414',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl1x3*Ru4x4*yu1x1*complexconjugate(LLQD3x1x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I143x414}')

I143x441 = Parameter(name = 'I143x441',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru1x6*yu3x3*complexconjugate(LLQD1x3x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I143x441}')

I143x442 = Parameter(name = 'I143x442',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru2x6*yu3x3*complexconjugate(LLQD1x3x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I143x442}')

I143x443 = Parameter(name = 'I143x443',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru3x5*yu2x2*complexconjugate(LLQD1x2x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I143x443}')

I143x444 = Parameter(name = 'I143x444',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl4x1*Ru4x4*yu1x1*complexconjugate(LLQD1x1x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I143x444}')

I143x451 = Parameter(name = 'I143x451',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru1x6*yu3x3*complexconjugate(LLQD2x3x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I143x451}')

I143x452 = Parameter(name = 'I143x452',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru2x6*yu3x3*complexconjugate(LLQD2x3x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I143x452}')

I143x453 = Parameter(name = 'I143x453',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru3x5*yu2x2*complexconjugate(LLQD2x2x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I143x453}')

I143x454 = Parameter(name = 'I143x454',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl5x2*Ru4x4*yu1x1*complexconjugate(LLQD2x1x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I143x454}')

I143x461 = Parameter(name = 'I143x461',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru1x6*yu3x3*complexconjugate(LLQD3x3x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I143x461}')

I143x462 = Parameter(name = 'I143x462',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru2x6*yu3x3*complexconjugate(LLQD3x3x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I143x462}')

I143x463 = Parameter(name = 'I143x463',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru3x5*yu2x2*complexconjugate(LLQD3x2x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I143x463}')

I143x464 = Parameter(name = 'I143x464',
                     nature = 'internal',
                     type = 'complex',
                     value = 'Rl6x3*Ru4x4*yu1x1*complexconjugate(LLQD3x1x1)*complexconjugate(Rd4x4)',
                     texname = '\\text{I143x464}')

I144x16 = Parameter(name = 'I144x16',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rd6x1)',
                    texname = '\\text{I144x16}')

I144x25 = Parameter(name = 'I144x25',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rd5x2)',
                    texname = '\\text{I144x25}')

I144x31 = Parameter(name = 'I144x31',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rd1x3)',
                    texname = '\\text{I144x31}')

I144x32 = Parameter(name = 'I144x32',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rd2x3)',
                    texname = '\\text{I144x32}')

I145x14 = Parameter(name = 'I145x14',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rd4x4)*complexconjugate(yd1x1)',
                    texname = '\\text{I145x14}')

I145x23 = Parameter(name = 'I145x23',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rd3x5)*complexconjugate(yd2x2)',
                    texname = '\\text{I145x23}')

I145x31 = Parameter(name = 'I145x31',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rd1x6)*complexconjugate(yd3x3)',
                    texname = '\\text{I145x31}')

I145x32 = Parameter(name = 'I145x32',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rd2x6)*complexconjugate(yd3x3)',
                    texname = '\\text{I145x32}')

I146x16 = Parameter(name = 'I146x16',
                    nature = 'internal',
                    type = 'complex',
                    value = 'yu1x1*complexconjugate(Rd6x1)',
                    texname = '\\text{I146x16}')

I146x25 = Parameter(name = 'I146x25',
                    nature = 'internal',
                    type = 'complex',
                    value = 'yu2x2*complexconjugate(Rd5x2)',
                    texname = '\\text{I146x25}')

I146x31 = Parameter(name = 'I146x31',
                    nature = 'internal',
                    type = 'complex',
                    value = 'yu3x3*complexconjugate(Rd1x3)',
                    texname = '\\text{I146x31}')

I146x32 = Parameter(name = 'I146x32',
                    nature = 'internal',
                    type = 'complex',
                    value = 'yu3x3*complexconjugate(Rd2x3)',
                    texname = '\\text{I146x32}')

I147x14 = Parameter(name = 'I147x14',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rl4x1)',
                    texname = '\\text{I147x14}')

I147x25 = Parameter(name = 'I147x25',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rl5x2)',
                    texname = '\\text{I147x25}')

I147x31 = Parameter(name = 'I147x31',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rl1x3)',
                    texname = '\\text{I147x31}')

I147x36 = Parameter(name = 'I147x36',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rl6x3)',
                    texname = '\\text{I147x36}')

I148x13 = Parameter(name = 'I148x13',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rl3x4)*complexconjugate(ye1x1)',
                    texname = '\\text{I148x13}')

I148x22 = Parameter(name = 'I148x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rl2x5)*complexconjugate(ye2x2)',
                    texname = '\\text{I148x22}')

I148x31 = Parameter(name = 'I148x31',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rl1x6)*complexconjugate(ye3x3)',
                    texname = '\\text{I148x31}')

I148x36 = Parameter(name = 'I148x36',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rl6x6)*complexconjugate(ye3x3)',
                    texname = '\\text{I148x36}')

I149x13 = Parameter(name = 'I149x13',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rn3x1)',
                    texname = '\\text{I149x13}')

I149x22 = Parameter(name = 'I149x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rn2x2)',
                    texname = '\\text{I149x22}')

I149x31 = Parameter(name = 'I149x31',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Rn1x3)',
                    texname = '\\text{I149x31}')

I15x14 = Parameter(name = 'I15x14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd4x4*yd1x1',
                   texname = '\\text{I15x14}')

I15x23 = Parameter(name = 'I15x23',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd3x5*yd2x2',
                   texname = '\\text{I15x23}')

I15x31 = Parameter(name = 'I15x31',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x6*yd3x3',
                   texname = '\\text{I15x31}')

I15x32 = Parameter(name = 'I15x32',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x6*yd3x3',
                   texname = '\\text{I15x32}')

I150x13 = Parameter(name = 'I150x13',
                    nature = 'internal',
                    type = 'complex',
                    value = 'ye1x1*complexconjugate(Rn3x1)',
                    texname = '\\text{I150x13}')

I150x22 = Parameter(name = 'I150x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'ye2x2*complexconjugate(Rn2x2)',
                    texname = '\\text{I150x22}')

I150x31 = Parameter(name = 'I150x31',
                    nature = 'internal',
                    type = 'complex',
                    value = 'ye3x3*complexconjugate(Rn1x3)',
                    texname = '\\text{I150x31}')

I151x15 = Parameter(name = 'I151x15',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Ru5x1)',
                    texname = '\\text{I151x15}')

I151x26 = Parameter(name = 'I151x26',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Ru6x2)',
                    texname = '\\text{I151x26}')

I151x31 = Parameter(name = 'I151x31',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Ru1x3)',
                    texname = '\\text{I151x31}')

I151x32 = Parameter(name = 'I151x32',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Ru2x3)',
                    texname = '\\text{I151x32}')

I152x14 = Parameter(name = 'I152x14',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I152x14}')

I152x23 = Parameter(name = 'I152x23',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I152x23}')

I152x31 = Parameter(name = 'I152x31',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I152x31}')

I152x32 = Parameter(name = 'I152x32',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I152x32}')

I153x15 = Parameter(name = 'I153x15',
                    nature = 'internal',
                    type = 'complex',
                    value = 'yd1x1*complexconjugate(Ru5x1)',
                    texname = '\\text{I153x15}')

I153x26 = Parameter(name = 'I153x26',
                    nature = 'internal',
                    type = 'complex',
                    value = 'yd2x2*complexconjugate(Ru6x2)',
                    texname = '\\text{I153x26}')

I153x31 = Parameter(name = 'I153x31',
                    nature = 'internal',
                    type = 'complex',
                    value = 'yd3x3*complexconjugate(Ru1x3)',
                    texname = '\\text{I153x31}')

I153x32 = Parameter(name = 'I153x32',
                    nature = 'internal',
                    type = 'complex',
                    value = 'yd3x3*complexconjugate(Ru2x3)',
                    texname = '\\text{I153x32}')

I154x11 = Parameter(name = 'I154x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*complexconjugate(Rd1x3)',
                    texname = '\\text{I154x11}')

I154x12 = Parameter(name = 'I154x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*complexconjugate(Rd1x3)',
                    texname = '\\text{I154x12}')

I154x21 = Parameter(name = 'I154x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*complexconjugate(Rd2x3)',
                    texname = '\\text{I154x21}')

I154x22 = Parameter(name = 'I154x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*complexconjugate(Rd2x3)',
                    texname = '\\text{I154x22}')

I154x56 = Parameter(name = 'I154x56',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru6x2*complexconjugate(Rd5x2)',
                    texname = '\\text{I154x56}')

I154x65 = Parameter(name = 'I154x65',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru5x1*complexconjugate(Rd6x1)',
                    texname = '\\text{I154x65}')

I155x11 = Parameter(name = 'I155x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn1x3*complexconjugate(Rl1x3)',
                    texname = '\\text{I155x11}')

I155x16 = Parameter(name = 'I155x16',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn1x3*complexconjugate(Rl6x3)',
                    texname = '\\text{I155x16}')

I155x25 = Parameter(name = 'I155x25',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn2x2*complexconjugate(Rl5x2)',
                    texname = '\\text{I155x25}')

I155x34 = Parameter(name = 'I155x34',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn3x1*complexconjugate(Rl4x1)',
                    texname = '\\text{I155x34}')

I156x11 = Parameter(name = 'I156x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*complexconjugate(Ru1x3)',
                    texname = '\\text{I156x11}')

I156x12 = Parameter(name = 'I156x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*complexconjugate(Ru2x3)',
                    texname = '\\text{I156x12}')

I156x21 = Parameter(name = 'I156x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*complexconjugate(Ru1x3)',
                    texname = '\\text{I156x21}')

I156x22 = Parameter(name = 'I156x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*complexconjugate(Ru2x3)',
                    texname = '\\text{I156x22}')

I156x56 = Parameter(name = 'I156x56',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*complexconjugate(Ru6x2)',
                    texname = '\\text{I156x56}')

I156x65 = Parameter(name = 'I156x65',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*complexconjugate(Ru5x1)',
                    texname = '\\text{I156x65}')

I157x11 = Parameter(name = 'I157x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*complexconjugate(Rn1x3)',
                    texname = '\\text{I157x11}')

I157x16 = Parameter(name = 'I157x16',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*complexconjugate(Rn1x3)',
                    texname = '\\text{I157x16}')

I157x25 = Parameter(name = 'I157x25',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*complexconjugate(Rn2x2)',
                    texname = '\\text{I157x25}')

I157x34 = Parameter(name = 'I157x34',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*complexconjugate(Rn3x1)',
                    texname = '\\text{I157x34}')

I158x11 = Parameter(name = 'I158x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*complexconjugate(Rd1x3)',
                    texname = '\\text{I158x11}')

I158x12 = Parameter(name = 'I158x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*complexconjugate(Rd1x3)',
                    texname = '\\text{I158x12}')

I158x21 = Parameter(name = 'I158x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*complexconjugate(Rd2x3)',
                    texname = '\\text{I158x21}')

I158x22 = Parameter(name = 'I158x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*complexconjugate(Rd2x3)',
                    texname = '\\text{I158x22}')

I158x55 = Parameter(name = 'I158x55',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*complexconjugate(Rd5x2)',
                    texname = '\\text{I158x55}')

I158x66 = Parameter(name = 'I158x66',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*complexconjugate(Rd6x1)',
                    texname = '\\text{I158x66}')

I159x11 = Parameter(name = 'I159x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*complexconjugate(Rl1x3)',
                    texname = '\\text{I159x11}')

I159x16 = Parameter(name = 'I159x16',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*complexconjugate(Rl1x3)',
                    texname = '\\text{I159x16}')

I159x44 = Parameter(name = 'I159x44',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*complexconjugate(Rl4x1)',
                    texname = '\\text{I159x44}')

I159x55 = Parameter(name = 'I159x55',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*complexconjugate(Rl5x2)',
                    texname = '\\text{I159x55}')

I159x61 = Parameter(name = 'I159x61',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*complexconjugate(Rl6x3)',
                    texname = '\\text{I159x61}')

I159x66 = Parameter(name = 'I159x66',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*complexconjugate(Rl6x3)',
                    texname = '\\text{I159x66}')

I16x16 = Parameter(name = 'I16x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd6x1',
                   texname = '\\text{I16x16}')

I16x25 = Parameter(name = 'I16x25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd5x2',
                   texname = '\\text{I16x25}')

I16x31 = Parameter(name = 'I16x31',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3',
                   texname = '\\text{I16x31}')

I16x32 = Parameter(name = 'I16x32',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3',
                   texname = '\\text{I16x32}')

I160x11 = Parameter(name = 'I160x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*complexconjugate(Ru1x3)',
                    texname = '\\text{I160x11}')

I160x12 = Parameter(name = 'I160x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*complexconjugate(Ru1x3)',
                    texname = '\\text{I160x12}')

I160x21 = Parameter(name = 'I160x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x3*complexconjugate(Ru2x3)',
                    texname = '\\text{I160x21}')

I160x22 = Parameter(name = 'I160x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x3*complexconjugate(Ru2x3)',
                    texname = '\\text{I160x22}')

I160x55 = Parameter(name = 'I160x55',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru5x1*complexconjugate(Ru5x1)',
                    texname = '\\text{I160x55}')

I160x66 = Parameter(name = 'I160x66',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru6x2*complexconjugate(Ru6x2)',
                    texname = '\\text{I160x66}')

I161x11 = Parameter(name = 'I161x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'ye1x1',
                    texname = '\\text{I161x11}')

I161x22 = Parameter(name = 'I161x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'ye2x2',
                    texname = '\\text{I161x22}')

I161x33 = Parameter(name = 'I161x33',
                    nature = 'internal',
                    type = 'complex',
                    value = 'ye3x3',
                    texname = '\\text{I161x33}')

I162x11 = Parameter(name = 'I162x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*complexconjugate(Rd1x6)',
                    texname = '\\text{I162x11}')

I162x12 = Parameter(name = 'I162x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*complexconjugate(Rd1x6)',
                    texname = '\\text{I162x12}')

I162x21 = Parameter(name = 'I162x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*complexconjugate(Rd2x6)',
                    texname = '\\text{I162x21}')

I162x22 = Parameter(name = 'I162x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*complexconjugate(Rd2x6)',
                    texname = '\\text{I162x22}')

I162x33 = Parameter(name = 'I162x33',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*complexconjugate(Rd3x5)',
                    texname = '\\text{I162x33}')

I162x44 = Parameter(name = 'I162x44',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*complexconjugate(Rd4x4)',
                    texname = '\\text{I162x44}')

I163x11 = Parameter(name = 'I163x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x6*complexconjugate(Rl1x6)',
                    texname = '\\text{I163x11}')

I163x16 = Parameter(name = 'I163x16',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x6*complexconjugate(Rl1x6)',
                    texname = '\\text{I163x16}')

I163x22 = Parameter(name = 'I163x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl2x5*complexconjugate(Rl2x5)',
                    texname = '\\text{I163x22}')

I163x33 = Parameter(name = 'I163x33',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl3x4*complexconjugate(Rl3x4)',
                    texname = '\\text{I163x33}')

I163x61 = Parameter(name = 'I163x61',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x6*complexconjugate(Rl6x6)',
                    texname = '\\text{I163x61}')

I163x66 = Parameter(name = 'I163x66',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x6*complexconjugate(Rl6x6)',
                    texname = '\\text{I163x66}')

I164x11 = Parameter(name = 'I164x11',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x6*complexconjugate(Ru1x6)',
                    texname = '\\text{I164x11}')

I164x12 = Parameter(name = 'I164x12',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x6*complexconjugate(Ru1x6)',
                    texname = '\\text{I164x12}')

I164x21 = Parameter(name = 'I164x21',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru1x6*complexconjugate(Ru2x6)',
                    texname = '\\text{I164x21}')

I164x22 = Parameter(name = 'I164x22',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru2x6*complexconjugate(Ru2x6)',
                    texname = '\\text{I164x22}')

I164x33 = Parameter(name = 'I164x33',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru3x5*complexconjugate(Ru3x5)',
                    texname = '\\text{I164x33}')

I164x44 = Parameter(name = 'I164x44',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Ru4x4*complexconjugate(Ru4x4)',
                    texname = '\\text{I164x44}')

I17x16 = Parameter(name = 'I17x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd6x1*complexconjugate(yu1x1)',
                   texname = '\\text{I17x16}')

I17x25 = Parameter(name = 'I17x25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd5x2*complexconjugate(yu2x2)',
                   texname = '\\text{I17x25}')

I17x31 = Parameter(name = 'I17x31',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*complexconjugate(yu3x3)',
                   texname = '\\text{I17x31}')

I17x32 = Parameter(name = 'I17x32',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*complexconjugate(yu3x3)',
                   texname = '\\text{I17x32}')

I18x14 = Parameter(name = 'I18x14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd4x4*yd1x1',
                   texname = '\\text{I18x14}')

I18x23 = Parameter(name = 'I18x23',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd3x5*yd2x2',
                   texname = '\\text{I18x23}')

I18x31 = Parameter(name = 'I18x31',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x6*yd3x3',
                   texname = '\\text{I18x31}')

I18x32 = Parameter(name = 'I18x32',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x6*yd3x3',
                   texname = '\\text{I18x32}')

I19x111 = Parameter(name = 'I19x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*complexconjugate(LLQD1x3x1)',
                    texname = '\\text{I19x111}')

I19x112 = Parameter(name = 'I19x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*complexconjugate(LLQD1x3x1)',
                    texname = '\\text{I19x112}')

I19x115 = Parameter(name = 'I19x115',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*complexconjugate(LLQD1x2x1)',
                    texname = '\\text{I19x115}')

I19x116 = Parameter(name = 'I19x116',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*complexconjugate(LLQD1x1x1)',
                    texname = '\\text{I19x116}')

I19x121 = Parameter(name = 'I19x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*complexconjugate(LLQD1x3x2)',
                    texname = '\\text{I19x121}')

I19x122 = Parameter(name = 'I19x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*complexconjugate(LLQD1x3x2)',
                    texname = '\\text{I19x122}')

I19x125 = Parameter(name = 'I19x125',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*complexconjugate(LLQD1x2x2)',
                    texname = '\\text{I19x125}')

I19x126 = Parameter(name = 'I19x126',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*complexconjugate(LLQD1x1x2)',
                    texname = '\\text{I19x126}')

I19x131 = Parameter(name = 'I19x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*complexconjugate(LLQD1x3x3)',
                    texname = '\\text{I19x131}')

I19x132 = Parameter(name = 'I19x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*complexconjugate(LLQD1x3x3)',
                    texname = '\\text{I19x132}')

I19x135 = Parameter(name = 'I19x135',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*complexconjugate(LLQD1x2x3)',
                    texname = '\\text{I19x135}')

I19x136 = Parameter(name = 'I19x136',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*complexconjugate(LLQD1x1x3)',
                    texname = '\\text{I19x136}')

I19x211 = Parameter(name = 'I19x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*complexconjugate(LLQD2x3x1)',
                    texname = '\\text{I19x211}')

I19x212 = Parameter(name = 'I19x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*complexconjugate(LLQD2x3x1)',
                    texname = '\\text{I19x212}')

I19x215 = Parameter(name = 'I19x215',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*complexconjugate(LLQD2x2x1)',
                    texname = '\\text{I19x215}')

I19x216 = Parameter(name = 'I19x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*complexconjugate(LLQD2x1x1)',
                    texname = '\\text{I19x216}')

I19x221 = Parameter(name = 'I19x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*complexconjugate(LLQD2x3x2)',
                    texname = '\\text{I19x221}')

I19x222 = Parameter(name = 'I19x222',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*complexconjugate(LLQD2x3x2)',
                    texname = '\\text{I19x222}')

I19x225 = Parameter(name = 'I19x225',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*complexconjugate(LLQD2x2x2)',
                    texname = '\\text{I19x225}')

I19x226 = Parameter(name = 'I19x226',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*complexconjugate(LLQD2x1x2)',
                    texname = '\\text{I19x226}')

I19x231 = Parameter(name = 'I19x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*complexconjugate(LLQD2x3x3)',
                    texname = '\\text{I19x231}')

I19x232 = Parameter(name = 'I19x232',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*complexconjugate(LLQD2x3x3)',
                    texname = '\\text{I19x232}')

I19x235 = Parameter(name = 'I19x235',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*complexconjugate(LLQD2x2x3)',
                    texname = '\\text{I19x235}')

I19x236 = Parameter(name = 'I19x236',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*complexconjugate(LLQD2x1x3)',
                    texname = '\\text{I19x236}')

I19x311 = Parameter(name = 'I19x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*complexconjugate(LLQD3x3x1)',
                    texname = '\\text{I19x311}')

I19x312 = Parameter(name = 'I19x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*complexconjugate(LLQD3x3x1)',
                    texname = '\\text{I19x312}')

I19x315 = Parameter(name = 'I19x315',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*complexconjugate(LLQD3x2x1)',
                    texname = '\\text{I19x315}')

I19x316 = Parameter(name = 'I19x316',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*complexconjugate(LLQD3x1x1)',
                    texname = '\\text{I19x316}')

I19x321 = Parameter(name = 'I19x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*complexconjugate(LLQD3x3x2)',
                    texname = '\\text{I19x321}')

I19x322 = Parameter(name = 'I19x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*complexconjugate(LLQD3x3x2)',
                    texname = '\\text{I19x322}')

I19x325 = Parameter(name = 'I19x325',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*complexconjugate(LLQD3x2x2)',
                    texname = '\\text{I19x325}')

I19x326 = Parameter(name = 'I19x326',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*complexconjugate(LLQD3x1x2)',
                    texname = '\\text{I19x326}')

I19x331 = Parameter(name = 'I19x331',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*complexconjugate(LLQD3x3x3)',
                    texname = '\\text{I19x331}')

I19x332 = Parameter(name = 'I19x332',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*complexconjugate(LLQD3x3x3)',
                    texname = '\\text{I19x332}')

I19x335 = Parameter(name = 'I19x335',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*complexconjugate(LLQD3x2x3)',
                    texname = '\\text{I19x335}')

I19x336 = Parameter(name = 'I19x336',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*complexconjugate(LLQD3x1x3)',
                    texname = '\\text{I19x336}')

I2x11 = Parameter(name = 'I2x11',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yd1x1',
                  texname = '\\text{I2x11}')

I2x22 = Parameter(name = 'I2x22',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yd2x2',
                  texname = '\\text{I2x22}')

I2x33 = Parameter(name = 'I2x33',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yd3x3',
                  texname = '\\text{I2x33}')

I20x111 = Parameter(name = 'I20x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x3*Rd1x6',
                    texname = '\\text{I20x111}')

I20x112 = Parameter(name = 'I20x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x3*Rd2x6',
                    texname = '\\text{I20x112}')

I20x113 = Parameter(name = 'I20x113',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x2*Rd3x5',
                    texname = '\\text{I20x113}')

I20x114 = Parameter(name = 'I20x114',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x1*Rd4x4',
                    texname = '\\text{I20x114}')

I20x121 = Parameter(name = 'I20x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x3*Rd1x6',
                    texname = '\\text{I20x121}')

I20x122 = Parameter(name = 'I20x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x3*Rd2x6',
                    texname = '\\text{I20x122}')

I20x123 = Parameter(name = 'I20x123',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x2*Rd3x5',
                    texname = '\\text{I20x123}')

I20x124 = Parameter(name = 'I20x124',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x1*Rd4x4',
                    texname = '\\text{I20x124}')

I20x131 = Parameter(name = 'I20x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd1x6',
                    texname = '\\text{I20x131}')

I20x132 = Parameter(name = 'I20x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd2x6',
                    texname = '\\text{I20x132}')

I20x133 = Parameter(name = 'I20x133',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x2*Rd3x5',
                    texname = '\\text{I20x133}')

I20x134 = Parameter(name = 'I20x134',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x1*Rd4x4',
                    texname = '\\text{I20x134}')

I20x211 = Parameter(name = 'I20x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x3*Rd1x6',
                    texname = '\\text{I20x211}')

I20x212 = Parameter(name = 'I20x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x3*Rd2x6',
                    texname = '\\text{I20x212}')

I20x213 = Parameter(name = 'I20x213',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x2*Rd3x5',
                    texname = '\\text{I20x213}')

I20x214 = Parameter(name = 'I20x214',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x1*Rd4x4',
                    texname = '\\text{I20x214}')

I20x221 = Parameter(name = 'I20x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x3*Rd1x6',
                    texname = '\\text{I20x221}')

I20x222 = Parameter(name = 'I20x222',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x3*Rd2x6',
                    texname = '\\text{I20x222}')

I20x223 = Parameter(name = 'I20x223',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x2*Rd3x5',
                    texname = '\\text{I20x223}')

I20x224 = Parameter(name = 'I20x224',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x1*Rd4x4',
                    texname = '\\text{I20x224}')

I20x231 = Parameter(name = 'I20x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd1x6',
                    texname = '\\text{I20x231}')

I20x232 = Parameter(name = 'I20x232',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd2x6',
                    texname = '\\text{I20x232}')

I20x233 = Parameter(name = 'I20x233',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x2*Rd3x5',
                    texname = '\\text{I20x233}')

I20x234 = Parameter(name = 'I20x234',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x1*Rd4x4',
                    texname = '\\text{I20x234}')

I20x311 = Parameter(name = 'I20x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd1x6',
                    texname = '\\text{I20x311}')

I20x312 = Parameter(name = 'I20x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd2x6',
                    texname = '\\text{I20x312}')

I20x313 = Parameter(name = 'I20x313',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x2*Rd3x5',
                    texname = '\\text{I20x313}')

I20x314 = Parameter(name = 'I20x314',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x1*Rd4x4',
                    texname = '\\text{I20x314}')

I20x321 = Parameter(name = 'I20x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd1x6',
                    texname = '\\text{I20x321}')

I20x322 = Parameter(name = 'I20x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd2x6',
                    texname = '\\text{I20x322}')

I20x323 = Parameter(name = 'I20x323',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x2*Rd3x5',
                    texname = '\\text{I20x323}')

I20x324 = Parameter(name = 'I20x324',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x1*Rd4x4',
                    texname = '\\text{I20x324}')

I20x331 = Parameter(name = 'I20x331',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd1x6',
                    texname = '\\text{I20x331}')

I20x332 = Parameter(name = 'I20x332',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd2x6',
                    texname = '\\text{I20x332}')

I20x333 = Parameter(name = 'I20x333',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*Rd3x5',
                    texname = '\\text{I20x333}')

I20x334 = Parameter(name = 'I20x334',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*Rd4x4',
                    texname = '\\text{I20x334}')

I21x111 = Parameter(name = 'I21x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x3*Rd1x6',
                    texname = '\\text{I21x111}')

I21x112 = Parameter(name = 'I21x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x3*Rd2x6',
                    texname = '\\text{I21x112}')

I21x113 = Parameter(name = 'I21x113',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x2*Rd3x5',
                    texname = '\\text{I21x113}')

I21x114 = Parameter(name = 'I21x114',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x1*Rd4x4',
                    texname = '\\text{I21x114}')

I21x121 = Parameter(name = 'I21x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x3*Rd1x6',
                    texname = '\\text{I21x121}')

I21x122 = Parameter(name = 'I21x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x3*Rd2x6',
                    texname = '\\text{I21x122}')

I21x123 = Parameter(name = 'I21x123',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x2*Rd3x5',
                    texname = '\\text{I21x123}')

I21x124 = Parameter(name = 'I21x124',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x1*Rd4x4',
                    texname = '\\text{I21x124}')

I21x131 = Parameter(name = 'I21x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd1x6',
                    texname = '\\text{I21x131}')

I21x132 = Parameter(name = 'I21x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd2x6',
                    texname = '\\text{I21x132}')

I21x133 = Parameter(name = 'I21x133',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x2*Rd3x5',
                    texname = '\\text{I21x133}')

I21x134 = Parameter(name = 'I21x134',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x1*Rd4x4',
                    texname = '\\text{I21x134}')

I21x211 = Parameter(name = 'I21x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x3*Rd1x6',
                    texname = '\\text{I21x211}')

I21x212 = Parameter(name = 'I21x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x3*Rd2x6',
                    texname = '\\text{I21x212}')

I21x213 = Parameter(name = 'I21x213',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x2*Rd3x5',
                    texname = '\\text{I21x213}')

I21x214 = Parameter(name = 'I21x214',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x1*Rd4x4',
                    texname = '\\text{I21x214}')

I21x221 = Parameter(name = 'I21x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x3*Rd1x6',
                    texname = '\\text{I21x221}')

I21x222 = Parameter(name = 'I21x222',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x3*Rd2x6',
                    texname = '\\text{I21x222}')

I21x223 = Parameter(name = 'I21x223',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x2*Rd3x5',
                    texname = '\\text{I21x223}')

I21x224 = Parameter(name = 'I21x224',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x1*Rd4x4',
                    texname = '\\text{I21x224}')

I21x231 = Parameter(name = 'I21x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd1x6',
                    texname = '\\text{I21x231}')

I21x232 = Parameter(name = 'I21x232',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd2x6',
                    texname = '\\text{I21x232}')

I21x233 = Parameter(name = 'I21x233',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x2*Rd3x5',
                    texname = '\\text{I21x233}')

I21x234 = Parameter(name = 'I21x234',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x1*Rd4x4',
                    texname = '\\text{I21x234}')

I21x311 = Parameter(name = 'I21x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd1x6',
                    texname = '\\text{I21x311}')

I21x312 = Parameter(name = 'I21x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd2x6',
                    texname = '\\text{I21x312}')

I21x313 = Parameter(name = 'I21x313',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x2*Rd3x5',
                    texname = '\\text{I21x313}')

I21x314 = Parameter(name = 'I21x314',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x1*Rd4x4',
                    texname = '\\text{I21x314}')

I21x321 = Parameter(name = 'I21x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd1x6',
                    texname = '\\text{I21x321}')

I21x322 = Parameter(name = 'I21x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd2x6',
                    texname = '\\text{I21x322}')

I21x323 = Parameter(name = 'I21x323',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x2*Rd3x5',
                    texname = '\\text{I21x323}')

I21x324 = Parameter(name = 'I21x324',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x1*Rd4x4',
                    texname = '\\text{I21x324}')

I21x331 = Parameter(name = 'I21x331',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd1x6',
                    texname = '\\text{I21x331}')

I21x332 = Parameter(name = 'I21x332',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd2x6',
                    texname = '\\text{I21x332}')

I21x333 = Parameter(name = 'I21x333',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*Rd3x5',
                    texname = '\\text{I21x333}')

I21x334 = Parameter(name = 'I21x334',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*Rd4x4',
                    texname = '\\text{I21x334}')

I22x11 = Parameter(name = 'I22x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*complexconjugate(Rd1x3)',
                   texname = '\\text{I22x11}')

I22x12 = Parameter(name = 'I22x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*complexconjugate(Rd1x3)',
                   texname = '\\text{I22x12}')

I22x21 = Parameter(name = 'I22x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*complexconjugate(Rd2x3)',
                   texname = '\\text{I22x21}')

I22x22 = Parameter(name = 'I22x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*complexconjugate(Rd2x3)',
                   texname = '\\text{I22x22}')

I22x55 = Parameter(name = 'I22x55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd5x2*complexconjugate(Rd5x2)',
                   texname = '\\text{I22x55}')

I22x66 = Parameter(name = 'I22x66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd6x1*complexconjugate(Rd6x1)',
                   texname = '\\text{I22x66}')

I23x11 = Parameter(name = 'I23x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x6*complexconjugate(Rd1x6)',
                   texname = '\\text{I23x11}')

I23x12 = Parameter(name = 'I23x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x6*complexconjugate(Rd1x6)',
                   texname = '\\text{I23x12}')

I23x21 = Parameter(name = 'I23x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x6*complexconjugate(Rd2x6)',
                   texname = '\\text{I23x21}')

I23x22 = Parameter(name = 'I23x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x6*complexconjugate(Rd2x6)',
                   texname = '\\text{I23x22}')

I23x33 = Parameter(name = 'I23x33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd3x5*complexconjugate(Rd3x5)',
                   texname = '\\text{I23x33}')

I23x44 = Parameter(name = 'I23x44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd4x4*complexconjugate(Rd4x4)',
                   texname = '\\text{I23x44}')

I24x11 = Parameter(name = 'I24x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*complexconjugate(Rd1x6)*complexconjugate(td3x3)',
                   texname = '\\text{I24x11}')

I24x12 = Parameter(name = 'I24x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*complexconjugate(Rd1x6)*complexconjugate(td3x3)',
                   texname = '\\text{I24x12}')

I24x21 = Parameter(name = 'I24x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*complexconjugate(Rd2x6)*complexconjugate(td3x3)',
                   texname = '\\text{I24x21}')

I24x22 = Parameter(name = 'I24x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*complexconjugate(Rd2x6)*complexconjugate(td3x3)',
                   texname = '\\text{I24x22}')

I24x35 = Parameter(name = 'I24x35',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd5x2*complexconjugate(Rd3x5)*complexconjugate(td2x2)',
                   texname = '\\text{I24x35}')

I24x46 = Parameter(name = 'I24x46',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd6x1*complexconjugate(Rd4x4)*complexconjugate(td1x1)',
                   texname = '\\text{I24x46}')

I25x11 = Parameter(name = 'I25x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*complexconjugate(Rd1x6)*complexconjugate(yd3x3)',
                   texname = '\\text{I25x11}')

I25x12 = Parameter(name = 'I25x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*complexconjugate(Rd1x6)*complexconjugate(yd3x3)',
                   texname = '\\text{I25x12}')

I25x21 = Parameter(name = 'I25x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*complexconjugate(Rd2x6)*complexconjugate(yd3x3)',
                   texname = '\\text{I25x21}')

I25x22 = Parameter(name = 'I25x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*complexconjugate(Rd2x6)*complexconjugate(yd3x3)',
                   texname = '\\text{I25x22}')

I25x35 = Parameter(name = 'I25x35',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd5x2*complexconjugate(Rd3x5)*complexconjugate(yd2x2)',
                   texname = '\\text{I25x35}')

I25x46 = Parameter(name = 'I25x46',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd6x1*complexconjugate(Rd4x4)*complexconjugate(yd1x1)',
                   texname = '\\text{I25x46}')

I26x11 = Parameter(name = 'I26x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x6*td3x3*complexconjugate(Rd1x3)',
                   texname = '\\text{I26x11}')

I26x12 = Parameter(name = 'I26x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x6*td3x3*complexconjugate(Rd1x3)',
                   texname = '\\text{I26x12}')

I26x21 = Parameter(name = 'I26x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x6*td3x3*complexconjugate(Rd2x3)',
                   texname = '\\text{I26x21}')

I26x22 = Parameter(name = 'I26x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x6*td3x3*complexconjugate(Rd2x3)',
                   texname = '\\text{I26x22}')

I26x53 = Parameter(name = 'I26x53',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd3x5*td2x2*complexconjugate(Rd5x2)',
                   texname = '\\text{I26x53}')

I26x64 = Parameter(name = 'I26x64',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd4x4*td1x1*complexconjugate(Rd6x1)',
                   texname = '\\text{I26x64}')

I27x11 = Parameter(name = 'I27x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*yd3x3*complexconjugate(Rd1x3)*complexconjugate(yd3x3)',
                   texname = '\\text{I27x11}')

I27x12 = Parameter(name = 'I27x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*yd3x3*complexconjugate(Rd1x3)*complexconjugate(yd3x3)',
                   texname = '\\text{I27x12}')

I27x21 = Parameter(name = 'I27x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*yd3x3*complexconjugate(Rd2x3)*complexconjugate(yd3x3)',
                   texname = '\\text{I27x21}')

I27x22 = Parameter(name = 'I27x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*yd3x3*complexconjugate(Rd2x3)*complexconjugate(yd3x3)',
                   texname = '\\text{I27x22}')

I27x55 = Parameter(name = 'I27x55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd5x2*yd2x2*complexconjugate(Rd5x2)*complexconjugate(yd2x2)',
                   texname = '\\text{I27x55}')

I27x66 = Parameter(name = 'I27x66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd6x1*yd1x1*complexconjugate(Rd6x1)*complexconjugate(yd1x1)',
                   texname = '\\text{I27x66}')

I28x11 = Parameter(name = 'I28x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x6*yd3x3*complexconjugate(Rd1x3)',
                   texname = '\\text{I28x11}')

I28x12 = Parameter(name = 'I28x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x6*yd3x3*complexconjugate(Rd1x3)',
                   texname = '\\text{I28x12}')

I28x21 = Parameter(name = 'I28x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x6*yd3x3*complexconjugate(Rd2x3)',
                   texname = '\\text{I28x21}')

I28x22 = Parameter(name = 'I28x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x6*yd3x3*complexconjugate(Rd2x3)',
                   texname = '\\text{I28x22}')

I28x53 = Parameter(name = 'I28x53',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd3x5*yd2x2*complexconjugate(Rd5x2)',
                   texname = '\\text{I28x53}')

I28x64 = Parameter(name = 'I28x64',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd4x4*yd1x1*complexconjugate(Rd6x1)',
                   texname = '\\text{I28x64}')

I29x11 = Parameter(name = 'I29x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x6*yd3x3*complexconjugate(Rd1x6)*complexconjugate(yd3x3)',
                   texname = '\\text{I29x11}')

I29x12 = Parameter(name = 'I29x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x6*yd3x3*complexconjugate(Rd1x6)*complexconjugate(yd3x3)',
                   texname = '\\text{I29x12}')

I29x21 = Parameter(name = 'I29x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x6*yd3x3*complexconjugate(Rd2x6)*complexconjugate(yd3x3)',
                   texname = '\\text{I29x21}')

I29x22 = Parameter(name = 'I29x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x6*yd3x3*complexconjugate(Rd2x6)*complexconjugate(yd3x3)',
                   texname = '\\text{I29x22}')

I29x33 = Parameter(name = 'I29x33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd3x5*yd2x2*complexconjugate(Rd3x5)*complexconjugate(yd2x2)',
                   texname = '\\text{I29x33}')

I29x44 = Parameter(name = 'I29x44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd4x4*yd1x1*complexconjugate(Rd4x4)*complexconjugate(yd1x1)',
                   texname = '\\text{I29x44}')

I3x111 = Parameter(name = 'I3x111',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD1x1x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I3x111}')

I3x112 = Parameter(name = 'I3x112',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD1x1x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I3x112}')

I3x113 = Parameter(name = 'I3x113',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD1x1x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I3x113}')

I3x121 = Parameter(name = 'I3x121',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD2x1x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I3x121}')

I3x122 = Parameter(name = 'I3x122',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD2x1x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I3x122}')

I3x123 = Parameter(name = 'I3x123',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD2x1x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I3x123}')

I3x131 = Parameter(name = 'I3x131',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD3x1x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I3x131}')

I3x132 = Parameter(name = 'I3x132',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD3x1x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I3x132}')

I3x133 = Parameter(name = 'I3x133',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD3x1x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I3x133}')

I3x211 = Parameter(name = 'I3x211',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD1x2x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I3x211}')

I3x212 = Parameter(name = 'I3x212',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD1x2x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I3x212}')

I3x214 = Parameter(name = 'I3x214',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD1x2x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I3x214}')

I3x221 = Parameter(name = 'I3x221',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD2x2x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I3x221}')

I3x222 = Parameter(name = 'I3x222',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD2x2x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I3x222}')

I3x224 = Parameter(name = 'I3x224',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD2x2x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I3x224}')

I3x231 = Parameter(name = 'I3x231',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD3x2x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I3x231}')

I3x232 = Parameter(name = 'I3x232',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD3x2x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I3x232}')

I3x234 = Parameter(name = 'I3x234',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD3x2x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I3x234}')

I3x313 = Parameter(name = 'I3x313',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD1x3x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I3x313}')

I3x314 = Parameter(name = 'I3x314',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD1x3x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I3x314}')

I3x323 = Parameter(name = 'I3x323',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD2x3x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I3x323}')

I3x324 = Parameter(name = 'I3x324',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD2x3x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I3x324}')

I3x333 = Parameter(name = 'I3x333',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD3x3x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I3x333}')

I3x334 = Parameter(name = 'I3x334',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD3x3x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I3x334}')

I30x11 = Parameter(name = 'I30x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(yd1x1)',
                   texname = '\\text{I30x11}')

I30x22 = Parameter(name = 'I30x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(yd2x2)',
                   texname = '\\text{I30x22}')

I30x33 = Parameter(name = 'I30x33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(yd3x3)',
                   texname = '\\text{I30x33}')

I31x11 = Parameter(name = 'I31x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yu1x1',
                   texname = '\\text{I31x11}')

I31x22 = Parameter(name = 'I31x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yu2x2',
                   texname = '\\text{I31x22}')

I31x33 = Parameter(name = 'I31x33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yu3x3',
                   texname = '\\text{I31x33}')

I32x11 = Parameter(name = 'I32x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(ye1x1)',
                   texname = '\\text{I32x11}')

I32x22 = Parameter(name = 'I32x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(ye2x2)',
                   texname = '\\text{I32x22}')

I32x33 = Parameter(name = 'I32x33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(ye3x3)',
                   texname = '\\text{I32x33}')

I33x13 = Parameter(name = 'I33x13',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(Rl3x4)*complexconjugate(ye1x1)',
                   texname = '\\text{I33x13}')

I33x22 = Parameter(name = 'I33x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(Rl2x5)*complexconjugate(ye2x2)',
                   texname = '\\text{I33x22}')

I33x31 = Parameter(name = 'I33x31',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(Rl1x6)*complexconjugate(ye3x3)',
                   texname = '\\text{I33x31}')

I33x36 = Parameter(name = 'I33x36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(Rl6x6)*complexconjugate(ye3x3)',
                   texname = '\\text{I33x36}')

I34x14 = Parameter(name = 'I34x14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'ye1x1*complexconjugate(Rl4x1)',
                   texname = '\\text{I34x14}')

I34x25 = Parameter(name = 'I34x25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'ye2x2*complexconjugate(Rl5x2)',
                   texname = '\\text{I34x25}')

I34x31 = Parameter(name = 'I34x31',
                   nature = 'internal',
                   type = 'complex',
                   value = 'ye3x3*complexconjugate(Rl1x3)',
                   texname = '\\text{I34x31}')

I34x36 = Parameter(name = 'I34x36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'ye3x3*complexconjugate(Rl6x3)',
                   texname = '\\text{I34x36}')

I35x111 = Parameter(name = 'I35x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x1*complexconjugate(Rl1x3)',
                    texname = '\\text{I35x111}')

I35x115 = Parameter(name = 'I35x115',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x2x1*complexconjugate(Rl5x2)',
                    texname = '\\text{I35x115}')

I35x116 = Parameter(name = 'I35x116',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x1*complexconjugate(Rl6x3)',
                    texname = '\\text{I35x116}')

I35x121 = Parameter(name = 'I35x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x1*complexconjugate(Rl1x3)',
                    texname = '\\text{I35x121}')

I35x124 = Parameter(name = 'I35x124',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x1x1*complexconjugate(Rl4x1)',
                    texname = '\\text{I35x124}')

I35x126 = Parameter(name = 'I35x126',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x1*complexconjugate(Rl6x3)',
                    texname = '\\text{I35x126}')

I35x134 = Parameter(name = 'I35x134',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x1x1*complexconjugate(Rl4x1)',
                    texname = '\\text{I35x134}')

I35x135 = Parameter(name = 'I35x135',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x2x1*complexconjugate(Rl5x2)',
                    texname = '\\text{I35x135}')

I35x211 = Parameter(name = 'I35x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x2*complexconjugate(Rl1x3)',
                    texname = '\\text{I35x211}')

I35x215 = Parameter(name = 'I35x215',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x2x2*complexconjugate(Rl5x2)',
                    texname = '\\text{I35x215}')

I35x216 = Parameter(name = 'I35x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x2*complexconjugate(Rl6x3)',
                    texname = '\\text{I35x216}')

I35x221 = Parameter(name = 'I35x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x2*complexconjugate(Rl1x3)',
                    texname = '\\text{I35x221}')

I35x224 = Parameter(name = 'I35x224',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x1x2*complexconjugate(Rl4x1)',
                    texname = '\\text{I35x224}')

I35x226 = Parameter(name = 'I35x226',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x2*complexconjugate(Rl6x3)',
                    texname = '\\text{I35x226}')

I35x234 = Parameter(name = 'I35x234',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x1x2*complexconjugate(Rl4x1)',
                    texname = '\\text{I35x234}')

I35x235 = Parameter(name = 'I35x235',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x2x2*complexconjugate(Rl5x2)',
                    texname = '\\text{I35x235}')

I35x311 = Parameter(name = 'I35x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x3*complexconjugate(Rl1x3)',
                    texname = '\\text{I35x311}')

I35x315 = Parameter(name = 'I35x315',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x2x3*complexconjugate(Rl5x2)',
                    texname = '\\text{I35x315}')

I35x316 = Parameter(name = 'I35x316',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x3*complexconjugate(Rl6x3)',
                    texname = '\\text{I35x316}')

I35x321 = Parameter(name = 'I35x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x3*complexconjugate(Rl1x3)',
                    texname = '\\text{I35x321}')

I35x324 = Parameter(name = 'I35x324',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x1x3*complexconjugate(Rl4x1)',
                    texname = '\\text{I35x324}')

I35x326 = Parameter(name = 'I35x326',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x3*complexconjugate(Rl6x3)',
                    texname = '\\text{I35x326}')

I35x334 = Parameter(name = 'I35x334',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x1x3*complexconjugate(Rl4x1)',
                    texname = '\\text{I35x334}')

I35x335 = Parameter(name = 'I35x335',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x2x3*complexconjugate(Rl5x2)',
                    texname = '\\text{I35x335}')

I36x111 = Parameter(name = 'I36x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x1*complexconjugate(Rl1x3)',
                    texname = '\\text{I36x111}')

I36x114 = Parameter(name = 'I36x114',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x1*complexconjugate(Rl4x1)',
                    texname = '\\text{I36x114}')

I36x115 = Parameter(name = 'I36x115',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x1*complexconjugate(Rl5x2)',
                    texname = '\\text{I36x115}')

I36x116 = Parameter(name = 'I36x116',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x1*complexconjugate(Rl6x3)',
                    texname = '\\text{I36x116}')

I36x121 = Parameter(name = 'I36x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x1*complexconjugate(Rl1x3)',
                    texname = '\\text{I36x121}')

I36x124 = Parameter(name = 'I36x124',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x1*complexconjugate(Rl4x1)',
                    texname = '\\text{I36x124}')

I36x125 = Parameter(name = 'I36x125',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x1*complexconjugate(Rl5x2)',
                    texname = '\\text{I36x125}')

I36x126 = Parameter(name = 'I36x126',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x1*complexconjugate(Rl6x3)',
                    texname = '\\text{I36x126}')

I36x131 = Parameter(name = 'I36x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*complexconjugate(Rl1x3)',
                    texname = '\\text{I36x131}')

I36x134 = Parameter(name = 'I36x134',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x1*complexconjugate(Rl4x1)',
                    texname = '\\text{I36x134}')

I36x135 = Parameter(name = 'I36x135',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x1*complexconjugate(Rl5x2)',
                    texname = '\\text{I36x135}')

I36x136 = Parameter(name = 'I36x136',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*complexconjugate(Rl6x3)',
                    texname = '\\text{I36x136}')

I36x211 = Parameter(name = 'I36x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x2*complexconjugate(Rl1x3)',
                    texname = '\\text{I36x211}')

I36x214 = Parameter(name = 'I36x214',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x2*complexconjugate(Rl4x1)',
                    texname = '\\text{I36x214}')

I36x215 = Parameter(name = 'I36x215',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x2*complexconjugate(Rl5x2)',
                    texname = '\\text{I36x215}')

I36x216 = Parameter(name = 'I36x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x2*complexconjugate(Rl6x3)',
                    texname = '\\text{I36x216}')

I36x221 = Parameter(name = 'I36x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x2*complexconjugate(Rl1x3)',
                    texname = '\\text{I36x221}')

I36x224 = Parameter(name = 'I36x224',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x2*complexconjugate(Rl4x1)',
                    texname = '\\text{I36x224}')

I36x225 = Parameter(name = 'I36x225',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x2*complexconjugate(Rl5x2)',
                    texname = '\\text{I36x225}')

I36x226 = Parameter(name = 'I36x226',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x2*complexconjugate(Rl6x3)',
                    texname = '\\text{I36x226}')

I36x231 = Parameter(name = 'I36x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*complexconjugate(Rl1x3)',
                    texname = '\\text{I36x231}')

I36x234 = Parameter(name = 'I36x234',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x2*complexconjugate(Rl4x1)',
                    texname = '\\text{I36x234}')

I36x235 = Parameter(name = 'I36x235',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x2*complexconjugate(Rl5x2)',
                    texname = '\\text{I36x235}')

I36x236 = Parameter(name = 'I36x236',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*complexconjugate(Rl6x3)',
                    texname = '\\text{I36x236}')

I36x311 = Parameter(name = 'I36x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*complexconjugate(Rl1x3)',
                    texname = '\\text{I36x311}')

I36x314 = Parameter(name = 'I36x314',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x3*complexconjugate(Rl4x1)',
                    texname = '\\text{I36x314}')

I36x315 = Parameter(name = 'I36x315',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x3*complexconjugate(Rl5x2)',
                    texname = '\\text{I36x315}')

I36x316 = Parameter(name = 'I36x316',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*complexconjugate(Rl6x3)',
                    texname = '\\text{I36x316}')

I36x321 = Parameter(name = 'I36x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*complexconjugate(Rl1x3)',
                    texname = '\\text{I36x321}')

I36x324 = Parameter(name = 'I36x324',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x3*complexconjugate(Rl4x1)',
                    texname = '\\text{I36x324}')

I36x325 = Parameter(name = 'I36x325',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x3*complexconjugate(Rl5x2)',
                    texname = '\\text{I36x325}')

I36x326 = Parameter(name = 'I36x326',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*complexconjugate(Rl6x3)',
                    texname = '\\text{I36x326}')

I36x331 = Parameter(name = 'I36x331',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*complexconjugate(Rl1x3)',
                    texname = '\\text{I36x331}')

I36x334 = Parameter(name = 'I36x334',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*complexconjugate(Rl4x1)',
                    texname = '\\text{I36x334}')

I36x335 = Parameter(name = 'I36x335',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*complexconjugate(Rl5x2)',
                    texname = '\\text{I36x335}')

I36x336 = Parameter(name = 'I36x336',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*complexconjugate(Rl6x3)',
                    texname = '\\text{I36x336}')

I37x121 = Parameter(name = 'I37x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE2x1x3)*complexconjugate(Rl1x6)',
                    texname = '\\text{I37x121}')

I37x122 = Parameter(name = 'I37x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE2x1x2)*complexconjugate(Rl2x5)',
                    texname = '\\text{I37x122}')

I37x123 = Parameter(name = 'I37x123',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE2x1x1)*complexconjugate(Rl3x4)',
                    texname = '\\text{I37x123}')

I37x126 = Parameter(name = 'I37x126',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE2x1x3)*complexconjugate(Rl6x6)',
                    texname = '\\text{I37x126}')

I37x131 = Parameter(name = 'I37x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE3x1x3)*complexconjugate(Rl1x6)',
                    texname = '\\text{I37x131}')

I37x132 = Parameter(name = 'I37x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE3x1x2)*complexconjugate(Rl2x5)',
                    texname = '\\text{I37x132}')

I37x133 = Parameter(name = 'I37x133',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE3x1x1)*complexconjugate(Rl3x4)',
                    texname = '\\text{I37x133}')

I37x136 = Parameter(name = 'I37x136',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE3x1x3)*complexconjugate(Rl6x6)',
                    texname = '\\text{I37x136}')

I37x211 = Parameter(name = 'I37x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE1x2x3)*complexconjugate(Rl1x6)',
                    texname = '\\text{I37x211}')

I37x212 = Parameter(name = 'I37x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE1x2x2)*complexconjugate(Rl2x5)',
                    texname = '\\text{I37x212}')

I37x213 = Parameter(name = 'I37x213',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE1x2x1)*complexconjugate(Rl3x4)',
                    texname = '\\text{I37x213}')

I37x216 = Parameter(name = 'I37x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE1x2x3)*complexconjugate(Rl6x6)',
                    texname = '\\text{I37x216}')

I37x231 = Parameter(name = 'I37x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE3x2x3)*complexconjugate(Rl1x6)',
                    texname = '\\text{I37x231}')

I37x232 = Parameter(name = 'I37x232',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE3x2x2)*complexconjugate(Rl2x5)',
                    texname = '\\text{I37x232}')

I37x233 = Parameter(name = 'I37x233',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE3x2x1)*complexconjugate(Rl3x4)',
                    texname = '\\text{I37x233}')

I37x236 = Parameter(name = 'I37x236',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE3x2x3)*complexconjugate(Rl6x6)',
                    texname = '\\text{I37x236}')

I37x311 = Parameter(name = 'I37x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE1x3x3)*complexconjugate(Rl1x6)',
                    texname = '\\text{I37x311}')

I37x312 = Parameter(name = 'I37x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE1x3x2)*complexconjugate(Rl2x5)',
                    texname = '\\text{I37x312}')

I37x313 = Parameter(name = 'I37x313',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE1x3x1)*complexconjugate(Rl3x4)',
                    texname = '\\text{I37x313}')

I37x316 = Parameter(name = 'I37x316',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE1x3x3)*complexconjugate(Rl6x6)',
                    texname = '\\text{I37x316}')

I37x321 = Parameter(name = 'I37x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE2x3x3)*complexconjugate(Rl1x6)',
                    texname = '\\text{I37x321}')

I37x322 = Parameter(name = 'I37x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE2x3x2)*complexconjugate(Rl2x5)',
                    texname = '\\text{I37x322}')

I37x323 = Parameter(name = 'I37x323',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE2x3x1)*complexconjugate(Rl3x4)',
                    texname = '\\text{I37x323}')

I37x326 = Parameter(name = 'I37x326',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LLLE2x3x3)*complexconjugate(Rl6x6)',
                    texname = '\\text{I37x326}')

I38x11 = Parameter(name = 'I38x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x3*complexconjugate(Rl1x3)',
                   texname = '\\text{I38x11}')

I38x16 = Parameter(name = 'I38x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x3*complexconjugate(Rl1x3)',
                   texname = '\\text{I38x16}')

I38x44 = Parameter(name = 'I38x44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl4x1*complexconjugate(Rl4x1)',
                   texname = '\\text{I38x44}')

I38x55 = Parameter(name = 'I38x55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl5x2*complexconjugate(Rl5x2)',
                   texname = '\\text{I38x55}')

I38x61 = Parameter(name = 'I38x61',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x3*complexconjugate(Rl6x3)',
                   texname = '\\text{I38x61}')

I38x66 = Parameter(name = 'I38x66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x3*complexconjugate(Rl6x3)',
                   texname = '\\text{I38x66}')

I39x11 = Parameter(name = 'I39x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x6*complexconjugate(Rl1x6)',
                   texname = '\\text{I39x11}')

I39x16 = Parameter(name = 'I39x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x6*complexconjugate(Rl1x6)',
                   texname = '\\text{I39x16}')

I39x22 = Parameter(name = 'I39x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl2x5*complexconjugate(Rl2x5)',
                   texname = '\\text{I39x22}')

I39x33 = Parameter(name = 'I39x33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl3x4*complexconjugate(Rl3x4)',
                   texname = '\\text{I39x33}')

I39x61 = Parameter(name = 'I39x61',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x6*complexconjugate(Rl6x6)',
                   texname = '\\text{I39x61}')

I39x66 = Parameter(name = 'I39x66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x6*complexconjugate(Rl6x6)',
                   texname = '\\text{I39x66}')

I4x111 = Parameter(name = 'I4x111',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD1x3x1)*complexconjugate(Rd1x6)',
                   texname = '\\text{I4x111}')

I4x112 = Parameter(name = 'I4x112',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD1x3x1)*complexconjugate(Rd2x6)',
                   texname = '\\text{I4x112}')

I4x113 = Parameter(name = 'I4x113',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD1x2x1)*complexconjugate(Rd3x5)',
                   texname = '\\text{I4x113}')

I4x121 = Parameter(name = 'I4x121',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD2x3x1)*complexconjugate(Rd1x6)',
                   texname = '\\text{I4x121}')

I4x122 = Parameter(name = 'I4x122',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD2x3x1)*complexconjugate(Rd2x6)',
                   texname = '\\text{I4x122}')

I4x123 = Parameter(name = 'I4x123',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD2x2x1)*complexconjugate(Rd3x5)',
                   texname = '\\text{I4x123}')

I4x131 = Parameter(name = 'I4x131',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD3x3x1)*complexconjugate(Rd1x6)',
                   texname = '\\text{I4x131}')

I4x132 = Parameter(name = 'I4x132',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD3x3x1)*complexconjugate(Rd2x6)',
                   texname = '\\text{I4x132}')

I4x133 = Parameter(name = 'I4x133',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD3x2x1)*complexconjugate(Rd3x5)',
                   texname = '\\text{I4x133}')

I4x211 = Parameter(name = 'I4x211',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD1x3x2)*complexconjugate(Rd1x6)',
                   texname = '\\text{I4x211}')

I4x212 = Parameter(name = 'I4x212',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD1x3x2)*complexconjugate(Rd2x6)',
                   texname = '\\text{I4x212}')

I4x214 = Parameter(name = 'I4x214',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD1x1x2)*complexconjugate(Rd4x4)',
                   texname = '\\text{I4x214}')

I4x221 = Parameter(name = 'I4x221',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD2x3x2)*complexconjugate(Rd1x6)',
                   texname = '\\text{I4x221}')

I4x222 = Parameter(name = 'I4x222',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD2x3x2)*complexconjugate(Rd2x6)',
                   texname = '\\text{I4x222}')

I4x224 = Parameter(name = 'I4x224',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD2x1x2)*complexconjugate(Rd4x4)',
                   texname = '\\text{I4x224}')

I4x231 = Parameter(name = 'I4x231',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD3x3x2)*complexconjugate(Rd1x6)',
                   texname = '\\text{I4x231}')

I4x232 = Parameter(name = 'I4x232',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD3x3x2)*complexconjugate(Rd2x6)',
                   texname = '\\text{I4x232}')

I4x234 = Parameter(name = 'I4x234',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD3x1x2)*complexconjugate(Rd4x4)',
                   texname = '\\text{I4x234}')

I4x313 = Parameter(name = 'I4x313',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD1x2x3)*complexconjugate(Rd3x5)',
                   texname = '\\text{I4x313}')

I4x314 = Parameter(name = 'I4x314',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD1x1x3)*complexconjugate(Rd4x4)',
                   texname = '\\text{I4x314}')

I4x323 = Parameter(name = 'I4x323',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD2x2x3)*complexconjugate(Rd3x5)',
                   texname = '\\text{I4x323}')

I4x324 = Parameter(name = 'I4x324',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD2x1x3)*complexconjugate(Rd4x4)',
                   texname = '\\text{I4x324}')

I4x333 = Parameter(name = 'I4x333',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD3x2x3)*complexconjugate(Rd3x5)',
                   texname = '\\text{I4x333}')

I4x334 = Parameter(name = 'I4x334',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LUDD3x1x3)*complexconjugate(Rd4x4)',
                   texname = '\\text{I4x334}')

I40x14 = Parameter(name = 'I40x14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl4x1*complexconjugate(ye1x1)',
                   texname = '\\text{I40x14}')

I40x25 = Parameter(name = 'I40x25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl5x2*complexconjugate(ye2x2)',
                   texname = '\\text{I40x25}')

I40x31 = Parameter(name = 'I40x31',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x3*complexconjugate(ye3x3)',
                   texname = '\\text{I40x31}')

I40x36 = Parameter(name = 'I40x36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x3*complexconjugate(ye3x3)',
                   texname = '\\text{I40x36}')

I41x13 = Parameter(name = 'I41x13',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl3x4*ye1x1',
                   texname = '\\text{I41x13}')

I41x22 = Parameter(name = 'I41x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl2x5*ye2x2',
                   texname = '\\text{I41x22}')

I41x31 = Parameter(name = 'I41x31',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x6*ye3x3',
                   texname = '\\text{I41x31}')

I41x36 = Parameter(name = 'I41x36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x6*ye3x3',
                   texname = '\\text{I41x36}')

I42x14 = Parameter(name = 'I42x14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl4x1',
                   texname = '\\text{I42x14}')

I42x25 = Parameter(name = 'I42x25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl5x2',
                   texname = '\\text{I42x25}')

I42x31 = Parameter(name = 'I42x31',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x3',
                   texname = '\\text{I42x31}')

I42x36 = Parameter(name = 'I42x36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x3',
                   texname = '\\text{I42x36}')

I43x13 = Parameter(name = 'I43x13',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl3x4*ye1x1',
                   texname = '\\text{I43x13}')

I43x22 = Parameter(name = 'I43x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl2x5*ye2x2',
                   texname = '\\text{I43x22}')

I43x31 = Parameter(name = 'I43x31',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x6*ye3x3',
                   texname = '\\text{I43x31}')

I43x36 = Parameter(name = 'I43x36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x6*ye3x3',
                   texname = '\\text{I43x36}')

I44x111 = Parameter(name = 'I44x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*complexconjugate(LLLE1x3x1)',
                    texname = '\\text{I44x111}')

I44x115 = Parameter(name = 'I44x115',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*complexconjugate(LLLE1x2x1)',
                    texname = '\\text{I44x115}')

I44x116 = Parameter(name = 'I44x116',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*complexconjugate(LLLE1x3x1)',
                    texname = '\\text{I44x116}')

I44x121 = Parameter(name = 'I44x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*complexconjugate(LLLE1x3x2)',
                    texname = '\\text{I44x121}')

I44x125 = Parameter(name = 'I44x125',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*complexconjugate(LLLE1x2x2)',
                    texname = '\\text{I44x125}')

I44x126 = Parameter(name = 'I44x126',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*complexconjugate(LLLE1x3x2)',
                    texname = '\\text{I44x126}')

I44x131 = Parameter(name = 'I44x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*complexconjugate(LLLE1x3x3)',
                    texname = '\\text{I44x131}')

I44x135 = Parameter(name = 'I44x135',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*complexconjugate(LLLE1x2x3)',
                    texname = '\\text{I44x135}')

I44x136 = Parameter(name = 'I44x136',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*complexconjugate(LLLE1x3x3)',
                    texname = '\\text{I44x136}')

I44x211 = Parameter(name = 'I44x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*complexconjugate(LLLE2x3x1)',
                    texname = '\\text{I44x211}')

I44x214 = Parameter(name = 'I44x214',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*complexconjugate(LLLE2x1x1)',
                    texname = '\\text{I44x214}')

I44x216 = Parameter(name = 'I44x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*complexconjugate(LLLE2x3x1)',
                    texname = '\\text{I44x216}')

I44x221 = Parameter(name = 'I44x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*complexconjugate(LLLE2x3x2)',
                    texname = '\\text{I44x221}')

I44x224 = Parameter(name = 'I44x224',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*complexconjugate(LLLE2x1x2)',
                    texname = '\\text{I44x224}')

I44x226 = Parameter(name = 'I44x226',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*complexconjugate(LLLE2x3x2)',
                    texname = '\\text{I44x226}')

I44x231 = Parameter(name = 'I44x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*complexconjugate(LLLE2x3x3)',
                    texname = '\\text{I44x231}')

I44x234 = Parameter(name = 'I44x234',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*complexconjugate(LLLE2x1x3)',
                    texname = '\\text{I44x234}')

I44x236 = Parameter(name = 'I44x236',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*complexconjugate(LLLE2x3x3)',
                    texname = '\\text{I44x236}')

I44x314 = Parameter(name = 'I44x314',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*complexconjugate(LLLE3x1x1)',
                    texname = '\\text{I44x314}')

I44x315 = Parameter(name = 'I44x315',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*complexconjugate(LLLE3x2x1)',
                    texname = '\\text{I44x315}')

I44x324 = Parameter(name = 'I44x324',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*complexconjugate(LLLE3x1x2)',
                    texname = '\\text{I44x324}')

I44x325 = Parameter(name = 'I44x325',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*complexconjugate(LLLE3x2x2)',
                    texname = '\\text{I44x325}')

I44x334 = Parameter(name = 'I44x334',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*complexconjugate(LLLE3x1x3)',
                    texname = '\\text{I44x334}')

I44x335 = Parameter(name = 'I44x335',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*complexconjugate(LLLE3x2x3)',
                    texname = '\\text{I44x335}')

I45x111 = Parameter(name = 'I45x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*complexconjugate(LLQD3x1x1)',
                    texname = '\\text{I45x111}')

I45x114 = Parameter(name = 'I45x114',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*complexconjugate(LLQD1x1x1)',
                    texname = '\\text{I45x114}')

I45x115 = Parameter(name = 'I45x115',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*complexconjugate(LLQD2x1x1)',
                    texname = '\\text{I45x115}')

I45x116 = Parameter(name = 'I45x116',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*complexconjugate(LLQD3x1x1)',
                    texname = '\\text{I45x116}')

I45x121 = Parameter(name = 'I45x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*complexconjugate(LLQD3x1x2)',
                    texname = '\\text{I45x121}')

I45x124 = Parameter(name = 'I45x124',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*complexconjugate(LLQD1x1x2)',
                    texname = '\\text{I45x124}')

I45x125 = Parameter(name = 'I45x125',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*complexconjugate(LLQD2x1x2)',
                    texname = '\\text{I45x125}')

I45x126 = Parameter(name = 'I45x126',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*complexconjugate(LLQD3x1x2)',
                    texname = '\\text{I45x126}')

I45x131 = Parameter(name = 'I45x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*complexconjugate(LLQD3x1x3)',
                    texname = '\\text{I45x131}')

I45x134 = Parameter(name = 'I45x134',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*complexconjugate(LLQD1x1x3)',
                    texname = '\\text{I45x134}')

I45x135 = Parameter(name = 'I45x135',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*complexconjugate(LLQD2x1x3)',
                    texname = '\\text{I45x135}')

I45x136 = Parameter(name = 'I45x136',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*complexconjugate(LLQD3x1x3)',
                    texname = '\\text{I45x136}')

I45x211 = Parameter(name = 'I45x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*complexconjugate(LLQD3x2x1)',
                    texname = '\\text{I45x211}')

I45x214 = Parameter(name = 'I45x214',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*complexconjugate(LLQD1x2x1)',
                    texname = '\\text{I45x214}')

I45x215 = Parameter(name = 'I45x215',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*complexconjugate(LLQD2x2x1)',
                    texname = '\\text{I45x215}')

I45x216 = Parameter(name = 'I45x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*complexconjugate(LLQD3x2x1)',
                    texname = '\\text{I45x216}')

I45x221 = Parameter(name = 'I45x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*complexconjugate(LLQD3x2x2)',
                    texname = '\\text{I45x221}')

I45x224 = Parameter(name = 'I45x224',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*complexconjugate(LLQD1x2x2)',
                    texname = '\\text{I45x224}')

I45x225 = Parameter(name = 'I45x225',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*complexconjugate(LLQD2x2x2)',
                    texname = '\\text{I45x225}')

I45x226 = Parameter(name = 'I45x226',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*complexconjugate(LLQD3x2x2)',
                    texname = '\\text{I45x226}')

I45x231 = Parameter(name = 'I45x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*complexconjugate(LLQD3x2x3)',
                    texname = '\\text{I45x231}')

I45x234 = Parameter(name = 'I45x234',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*complexconjugate(LLQD1x2x3)',
                    texname = '\\text{I45x234}')

I45x235 = Parameter(name = 'I45x235',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*complexconjugate(LLQD2x2x3)',
                    texname = '\\text{I45x235}')

I45x236 = Parameter(name = 'I45x236',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*complexconjugate(LLQD3x2x3)',
                    texname = '\\text{I45x236}')

I45x311 = Parameter(name = 'I45x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*complexconjugate(LLQD3x3x1)',
                    texname = '\\text{I45x311}')

I45x314 = Parameter(name = 'I45x314',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*complexconjugate(LLQD1x3x1)',
                    texname = '\\text{I45x314}')

I45x315 = Parameter(name = 'I45x315',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*complexconjugate(LLQD2x3x1)',
                    texname = '\\text{I45x315}')

I45x316 = Parameter(name = 'I45x316',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*complexconjugate(LLQD3x3x1)',
                    texname = '\\text{I45x316}')

I45x321 = Parameter(name = 'I45x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*complexconjugate(LLQD3x3x2)',
                    texname = '\\text{I45x321}')

I45x324 = Parameter(name = 'I45x324',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*complexconjugate(LLQD1x3x2)',
                    texname = '\\text{I45x324}')

I45x325 = Parameter(name = 'I45x325',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*complexconjugate(LLQD2x3x2)',
                    texname = '\\text{I45x325}')

I45x326 = Parameter(name = 'I45x326',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*complexconjugate(LLQD3x3x2)',
                    texname = '\\text{I45x326}')

I45x331 = Parameter(name = 'I45x331',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*complexconjugate(LLQD3x3x3)',
                    texname = '\\text{I45x331}')

I45x334 = Parameter(name = 'I45x334',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*complexconjugate(LLQD1x3x3)',
                    texname = '\\text{I45x334}')

I45x335 = Parameter(name = 'I45x335',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*complexconjugate(LLQD2x3x3)',
                    texname = '\\text{I45x335}')

I45x336 = Parameter(name = 'I45x336',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*complexconjugate(LLQD3x3x3)',
                    texname = '\\text{I45x336}')

I46x121 = Parameter(name = 'I46x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x1x3*Rl1x6',
                    texname = '\\text{I46x121}')

I46x122 = Parameter(name = 'I46x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x1x2*Rl2x5',
                    texname = '\\text{I46x122}')

I46x123 = Parameter(name = 'I46x123',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x1x1*Rl3x4',
                    texname = '\\text{I46x123}')

I46x126 = Parameter(name = 'I46x126',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x1x3*Rl6x6',
                    texname = '\\text{I46x126}')

I46x131 = Parameter(name = 'I46x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x1x3*Rl1x6',
                    texname = '\\text{I46x131}')

I46x132 = Parameter(name = 'I46x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x1x2*Rl2x5',
                    texname = '\\text{I46x132}')

I46x133 = Parameter(name = 'I46x133',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x1x1*Rl3x4',
                    texname = '\\text{I46x133}')

I46x136 = Parameter(name = 'I46x136',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x1x3*Rl6x6',
                    texname = '\\text{I46x136}')

I46x211 = Parameter(name = 'I46x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x2x3*Rl1x6',
                    texname = '\\text{I46x211}')

I46x212 = Parameter(name = 'I46x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x2x2*Rl2x5',
                    texname = '\\text{I46x212}')

I46x213 = Parameter(name = 'I46x213',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x2x1*Rl3x4',
                    texname = '\\text{I46x213}')

I46x216 = Parameter(name = 'I46x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x2x3*Rl6x6',
                    texname = '\\text{I46x216}')

I46x231 = Parameter(name = 'I46x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x2x3*Rl1x6',
                    texname = '\\text{I46x231}')

I46x232 = Parameter(name = 'I46x232',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x2x2*Rl2x5',
                    texname = '\\text{I46x232}')

I46x233 = Parameter(name = 'I46x233',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x2x1*Rl3x4',
                    texname = '\\text{I46x233}')

I46x236 = Parameter(name = 'I46x236',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x2x3*Rl6x6',
                    texname = '\\text{I46x236}')

I46x311 = Parameter(name = 'I46x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x3*Rl1x6',
                    texname = '\\text{I46x311}')

I46x312 = Parameter(name = 'I46x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x2*Rl2x5',
                    texname = '\\text{I46x312}')

I46x313 = Parameter(name = 'I46x313',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x1*Rl3x4',
                    texname = '\\text{I46x313}')

I46x316 = Parameter(name = 'I46x316',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x3*Rl6x6',
                    texname = '\\text{I46x316}')

I46x321 = Parameter(name = 'I46x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x3*Rl1x6',
                    texname = '\\text{I46x321}')

I46x322 = Parameter(name = 'I46x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x2*Rl2x5',
                    texname = '\\text{I46x322}')

I46x323 = Parameter(name = 'I46x323',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x1*Rl3x4',
                    texname = '\\text{I46x323}')

I46x326 = Parameter(name = 'I46x326',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x3*Rl6x6',
                    texname = '\\text{I46x326}')

I47x11 = Parameter(name = 'I47x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x3*complexconjugate(Rl1x3)',
                   texname = '\\text{I47x11}')

I47x16 = Parameter(name = 'I47x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x3*complexconjugate(Rl1x3)',
                   texname = '\\text{I47x16}')

I47x44 = Parameter(name = 'I47x44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl4x1*complexconjugate(Rl4x1)',
                   texname = '\\text{I47x44}')

I47x55 = Parameter(name = 'I47x55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl5x2*complexconjugate(Rl5x2)',
                   texname = '\\text{I47x55}')

I47x61 = Parameter(name = 'I47x61',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x3*complexconjugate(Rl6x3)',
                   texname = '\\text{I47x61}')

I47x66 = Parameter(name = 'I47x66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x3*complexconjugate(Rl6x3)',
                   texname = '\\text{I47x66}')

I48x11 = Parameter(name = 'I48x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x6*complexconjugate(Rl1x6)',
                   texname = '\\text{I48x11}')

I48x16 = Parameter(name = 'I48x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x6*complexconjugate(Rl1x6)',
                   texname = '\\text{I48x16}')

I48x22 = Parameter(name = 'I48x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl2x5*complexconjugate(Rl2x5)',
                   texname = '\\text{I48x22}')

I48x33 = Parameter(name = 'I48x33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl3x4*complexconjugate(Rl3x4)',
                   texname = '\\text{I48x33}')

I48x61 = Parameter(name = 'I48x61',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x6*complexconjugate(Rl6x6)',
                   texname = '\\text{I48x61}')

I48x66 = Parameter(name = 'I48x66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x6*complexconjugate(Rl6x6)',
                   texname = '\\text{I48x66}')

I49x11 = Parameter(name = 'I49x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x3*complexconjugate(Rl1x6)*complexconjugate(te3x3)',
                   texname = '\\text{I49x11}')

I49x16 = Parameter(name = 'I49x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x3*complexconjugate(Rl1x6)*complexconjugate(te3x3)',
                   texname = '\\text{I49x16}')

I49x25 = Parameter(name = 'I49x25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl5x2*complexconjugate(Rl2x5)*complexconjugate(te2x2)',
                   texname = '\\text{I49x25}')

I49x34 = Parameter(name = 'I49x34',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl4x1*complexconjugate(Rl3x4)*complexconjugate(te1x1)',
                   texname = '\\text{I49x34}')

I49x61 = Parameter(name = 'I49x61',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x3*complexconjugate(Rl6x6)*complexconjugate(te3x3)',
                   texname = '\\text{I49x61}')

I49x66 = Parameter(name = 'I49x66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x3*complexconjugate(Rl6x6)*complexconjugate(te3x3)',
                   texname = '\\text{I49x66}')

I5x14 = Parameter(name = 'I5x14',
                  nature = 'internal',
                  type = 'complex',
                  value = 'complexconjugate(Rd4x4)*complexconjugate(yd1x1)',
                  texname = '\\text{I5x14}')

I5x23 = Parameter(name = 'I5x23',
                  nature = 'internal',
                  type = 'complex',
                  value = 'complexconjugate(Rd3x5)*complexconjugate(yd2x2)',
                  texname = '\\text{I5x23}')

I5x31 = Parameter(name = 'I5x31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'complexconjugate(Rd1x6)*complexconjugate(yd3x3)',
                  texname = '\\text{I5x31}')

I5x32 = Parameter(name = 'I5x32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'complexconjugate(Rd2x6)*complexconjugate(yd3x3)',
                  texname = '\\text{I5x32}')

I50x11 = Parameter(name = 'I50x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x3*complexconjugate(Rl1x6)*complexconjugate(ye3x3)',
                   texname = '\\text{I50x11}')

I50x16 = Parameter(name = 'I50x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x3*complexconjugate(Rl1x6)*complexconjugate(ye3x3)',
                   texname = '\\text{I50x16}')

I50x25 = Parameter(name = 'I50x25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl5x2*complexconjugate(Rl2x5)*complexconjugate(ye2x2)',
                   texname = '\\text{I50x25}')

I50x34 = Parameter(name = 'I50x34',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl4x1*complexconjugate(Rl3x4)*complexconjugate(ye1x1)',
                   texname = '\\text{I50x34}')

I50x61 = Parameter(name = 'I50x61',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x3*complexconjugate(Rl6x6)*complexconjugate(ye3x3)',
                   texname = '\\text{I50x61}')

I50x66 = Parameter(name = 'I50x66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x3*complexconjugate(Rl6x6)*complexconjugate(ye3x3)',
                   texname = '\\text{I50x66}')

I51x11 = Parameter(name = 'I51x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x6*te3x3*complexconjugate(Rl1x3)',
                   texname = '\\text{I51x11}')

I51x16 = Parameter(name = 'I51x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x6*te3x3*complexconjugate(Rl1x3)',
                   texname = '\\text{I51x16}')

I51x43 = Parameter(name = 'I51x43',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl3x4*te1x1*complexconjugate(Rl4x1)',
                   texname = '\\text{I51x43}')

I51x52 = Parameter(name = 'I51x52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl2x5*te2x2*complexconjugate(Rl5x2)',
                   texname = '\\text{I51x52}')

I51x61 = Parameter(name = 'I51x61',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x6*te3x3*complexconjugate(Rl6x3)',
                   texname = '\\text{I51x61}')

I51x66 = Parameter(name = 'I51x66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x6*te3x3*complexconjugate(Rl6x3)',
                   texname = '\\text{I51x66}')

I52x11 = Parameter(name = 'I52x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x3*ye3x3*complexconjugate(Rl1x3)*complexconjugate(ye3x3)',
                   texname = '\\text{I52x11}')

I52x16 = Parameter(name = 'I52x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x3*ye3x3*complexconjugate(Rl1x3)*complexconjugate(ye3x3)',
                   texname = '\\text{I52x16}')

I52x44 = Parameter(name = 'I52x44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl4x1*ye1x1*complexconjugate(Rl4x1)*complexconjugate(ye1x1)',
                   texname = '\\text{I52x44}')

I52x55 = Parameter(name = 'I52x55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl5x2*ye2x2*complexconjugate(Rl5x2)*complexconjugate(ye2x2)',
                   texname = '\\text{I52x55}')

I52x61 = Parameter(name = 'I52x61',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x3*ye3x3*complexconjugate(Rl6x3)*complexconjugate(ye3x3)',
                   texname = '\\text{I52x61}')

I52x66 = Parameter(name = 'I52x66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x3*ye3x3*complexconjugate(Rl6x3)*complexconjugate(ye3x3)',
                   texname = '\\text{I52x66}')

I53x11 = Parameter(name = 'I53x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x6*ye3x3*complexconjugate(Rl1x3)',
                   texname = '\\text{I53x11}')

I53x16 = Parameter(name = 'I53x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x6*ye3x3*complexconjugate(Rl1x3)',
                   texname = '\\text{I53x16}')

I53x43 = Parameter(name = 'I53x43',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl3x4*ye1x1*complexconjugate(Rl4x1)',
                   texname = '\\text{I53x43}')

I53x52 = Parameter(name = 'I53x52',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl2x5*ye2x2*complexconjugate(Rl5x2)',
                   texname = '\\text{I53x52}')

I53x61 = Parameter(name = 'I53x61',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x6*ye3x3*complexconjugate(Rl6x3)',
                   texname = '\\text{I53x61}')

I53x66 = Parameter(name = 'I53x66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x6*ye3x3*complexconjugate(Rl6x3)',
                   texname = '\\text{I53x66}')

I54x11 = Parameter(name = 'I54x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x6*ye3x3*complexconjugate(Rl1x6)*complexconjugate(ye3x3)',
                   texname = '\\text{I54x11}')

I54x16 = Parameter(name = 'I54x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x6*ye3x3*complexconjugate(Rl1x6)*complexconjugate(ye3x3)',
                   texname = '\\text{I54x16}')

I54x22 = Parameter(name = 'I54x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl2x5*ye2x2*complexconjugate(Rl2x5)*complexconjugate(ye2x2)',
                   texname = '\\text{I54x22}')

I54x33 = Parameter(name = 'I54x33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl3x4*ye1x1*complexconjugate(Rl3x4)*complexconjugate(ye1x1)',
                   texname = '\\text{I54x33}')

I54x61 = Parameter(name = 'I54x61',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x6*ye3x3*complexconjugate(Rl6x6)*complexconjugate(ye3x3)',
                   texname = '\\text{I54x61}')

I54x66 = Parameter(name = 'I54x66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x6*ye3x3*complexconjugate(Rl6x6)*complexconjugate(ye3x3)',
                   texname = '\\text{I54x66}')

I55x111 = Parameter(name = 'I55x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x1x1*complexconjugate(Rn1x3)',
                    texname = '\\text{I55x111}')

I55x112 = Parameter(name = 'I55x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x1x1*complexconjugate(Rn2x2)',
                    texname = '\\text{I55x112}')

I55x121 = Parameter(name = 'I55x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x2x1*complexconjugate(Rn1x3)',
                    texname = '\\text{I55x121}')

I55x123 = Parameter(name = 'I55x123',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x2x1*complexconjugate(Rn3x1)',
                    texname = '\\text{I55x123}')

I55x132 = Parameter(name = 'I55x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x1*complexconjugate(Rn2x2)',
                    texname = '\\text{I55x132}')

I55x133 = Parameter(name = 'I55x133',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x1*complexconjugate(Rn3x1)',
                    texname = '\\text{I55x133}')

I55x211 = Parameter(name = 'I55x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x1x2*complexconjugate(Rn1x3)',
                    texname = '\\text{I55x211}')

I55x212 = Parameter(name = 'I55x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x1x2*complexconjugate(Rn2x2)',
                    texname = '\\text{I55x212}')

I55x221 = Parameter(name = 'I55x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x2x2*complexconjugate(Rn1x3)',
                    texname = '\\text{I55x221}')

I55x223 = Parameter(name = 'I55x223',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x2x2*complexconjugate(Rn3x1)',
                    texname = '\\text{I55x223}')

I55x232 = Parameter(name = 'I55x232',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x2*complexconjugate(Rn2x2)',
                    texname = '\\text{I55x232}')

I55x233 = Parameter(name = 'I55x233',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x2*complexconjugate(Rn3x1)',
                    texname = '\\text{I55x233}')

I55x311 = Parameter(name = 'I55x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x1x3*complexconjugate(Rn1x3)',
                    texname = '\\text{I55x311}')

I55x312 = Parameter(name = 'I55x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x1x3*complexconjugate(Rn2x2)',
                    texname = '\\text{I55x312}')

I55x321 = Parameter(name = 'I55x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x2x3*complexconjugate(Rn1x3)',
                    texname = '\\text{I55x321}')

I55x323 = Parameter(name = 'I55x323',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x2x3*complexconjugate(Rn3x1)',
                    texname = '\\text{I55x323}')

I55x332 = Parameter(name = 'I55x332',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x3*complexconjugate(Rn2x2)',
                    texname = '\\text{I55x332}')

I55x333 = Parameter(name = 'I55x333',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x3*complexconjugate(Rn3x1)',
                    texname = '\\text{I55x333}')

I56x111 = Parameter(name = 'I56x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x1*complexconjugate(Rn1x3)',
                    texname = '\\text{I56x111}')

I56x112 = Parameter(name = 'I56x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x1*complexconjugate(Rn2x2)',
                    texname = '\\text{I56x112}')

I56x113 = Parameter(name = 'I56x113',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x1*complexconjugate(Rn3x1)',
                    texname = '\\text{I56x113}')

I56x121 = Parameter(name = 'I56x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x1*complexconjugate(Rn1x3)',
                    texname = '\\text{I56x121}')

I56x122 = Parameter(name = 'I56x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x1*complexconjugate(Rn2x2)',
                    texname = '\\text{I56x122}')

I56x123 = Parameter(name = 'I56x123',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x1*complexconjugate(Rn3x1)',
                    texname = '\\text{I56x123}')

I56x131 = Parameter(name = 'I56x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*complexconjugate(Rn1x3)',
                    texname = '\\text{I56x131}')

I56x132 = Parameter(name = 'I56x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x1*complexconjugate(Rn2x2)',
                    texname = '\\text{I56x132}')

I56x133 = Parameter(name = 'I56x133',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x1*complexconjugate(Rn3x1)',
                    texname = '\\text{I56x133}')

I56x211 = Parameter(name = 'I56x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x2*complexconjugate(Rn1x3)',
                    texname = '\\text{I56x211}')

I56x212 = Parameter(name = 'I56x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x2*complexconjugate(Rn2x2)',
                    texname = '\\text{I56x212}')

I56x213 = Parameter(name = 'I56x213',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x2*complexconjugate(Rn3x1)',
                    texname = '\\text{I56x213}')

I56x221 = Parameter(name = 'I56x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x2*complexconjugate(Rn1x3)',
                    texname = '\\text{I56x221}')

I56x222 = Parameter(name = 'I56x222',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x2*complexconjugate(Rn2x2)',
                    texname = '\\text{I56x222}')

I56x223 = Parameter(name = 'I56x223',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x2*complexconjugate(Rn3x1)',
                    texname = '\\text{I56x223}')

I56x231 = Parameter(name = 'I56x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*complexconjugate(Rn1x3)',
                    texname = '\\text{I56x231}')

I56x232 = Parameter(name = 'I56x232',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x2*complexconjugate(Rn2x2)',
                    texname = '\\text{I56x232}')

I56x233 = Parameter(name = 'I56x233',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x2*complexconjugate(Rn3x1)',
                    texname = '\\text{I56x233}')

I56x311 = Parameter(name = 'I56x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*complexconjugate(Rn1x3)',
                    texname = '\\text{I56x311}')

I56x312 = Parameter(name = 'I56x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x3*complexconjugate(Rn2x2)',
                    texname = '\\text{I56x312}')

I56x313 = Parameter(name = 'I56x313',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x3*complexconjugate(Rn3x1)',
                    texname = '\\text{I56x313}')

I56x321 = Parameter(name = 'I56x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*complexconjugate(Rn1x3)',
                    texname = '\\text{I56x321}')

I56x322 = Parameter(name = 'I56x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x3*complexconjugate(Rn2x2)',
                    texname = '\\text{I56x322}')

I56x323 = Parameter(name = 'I56x323',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x3*complexconjugate(Rn3x1)',
                    texname = '\\text{I56x323}')

I56x331 = Parameter(name = 'I56x331',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*complexconjugate(Rn1x3)',
                    texname = '\\text{I56x331}')

I56x332 = Parameter(name = 'I56x332',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*complexconjugate(Rn2x2)',
                    texname = '\\text{I56x332}')

I56x333 = Parameter(name = 'I56x333',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*complexconjugate(Rn3x1)',
                    texname = '\\text{I56x333}')

I57x11 = Parameter(name = 'I57x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x3*complexconjugate(Rn1x3)',
                   texname = '\\text{I57x11}')

I57x16 = Parameter(name = 'I57x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x3*complexconjugate(Rn1x3)',
                   texname = '\\text{I57x16}')

I57x25 = Parameter(name = 'I57x25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl5x2*complexconjugate(Rn2x2)',
                   texname = '\\text{I57x25}')

I57x34 = Parameter(name = 'I57x34',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl4x1*complexconjugate(Rn3x1)',
                   texname = '\\text{I57x34}')

I58x11 = Parameter(name = 'I58x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x6*te3x3*complexconjugate(Rn1x3)',
                   texname = '\\text{I58x11}')

I58x16 = Parameter(name = 'I58x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x6*te3x3*complexconjugate(Rn1x3)',
                   texname = '\\text{I58x16}')

I58x22 = Parameter(name = 'I58x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl2x5*te2x2*complexconjugate(Rn2x2)',
                   texname = '\\text{I58x22}')

I58x33 = Parameter(name = 'I58x33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl3x4*te1x1*complexconjugate(Rn3x1)',
                   texname = '\\text{I58x33}')

I59x11 = Parameter(name = 'I59x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x3*ye3x3*complexconjugate(Rn1x3)*complexconjugate(ye3x3)',
                   texname = '\\text{I59x11}')

I59x16 = Parameter(name = 'I59x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x3*ye3x3*complexconjugate(Rn1x3)*complexconjugate(ye3x3)',
                   texname = '\\text{I59x16}')

I59x25 = Parameter(name = 'I59x25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl5x2*ye2x2*complexconjugate(Rn2x2)*complexconjugate(ye2x2)',
                   texname = '\\text{I59x25}')

I59x34 = Parameter(name = 'I59x34',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl4x1*ye1x1*complexconjugate(Rn3x1)*complexconjugate(ye1x1)',
                   texname = '\\text{I59x34}')

I6x16 = Parameter(name = 'I6x16',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yd1x1*complexconjugate(Rd6x1)',
                  texname = '\\text{I6x16}')

I6x25 = Parameter(name = 'I6x25',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yd2x2*complexconjugate(Rd5x2)',
                  texname = '\\text{I6x25}')

I6x31 = Parameter(name = 'I6x31',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yd3x3*complexconjugate(Rd1x3)',
                  texname = '\\text{I6x31}')

I6x32 = Parameter(name = 'I6x32',
                  nature = 'internal',
                  type = 'complex',
                  value = 'yd3x3*complexconjugate(Rd2x3)',
                  texname = '\\text{I6x32}')

I60x11 = Parameter(name = 'I60x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl1x6*ye3x3*complexconjugate(Rn1x3)',
                   texname = '\\text{I60x11}')

I60x16 = Parameter(name = 'I60x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl6x6*ye3x3*complexconjugate(Rn1x3)',
                   texname = '\\text{I60x16}')

I60x22 = Parameter(name = 'I60x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl2x5*ye2x2*complexconjugate(Rn2x2)',
                   texname = '\\text{I60x22}')

I60x33 = Parameter(name = 'I60x33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rl3x4*ye1x1*complexconjugate(Rn3x1)',
                   texname = '\\text{I60x33}')

I61x111 = Parameter(name = 'I61x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd1x3*complexconjugate(Rd1x3)*complexconjugate(Rn1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x111}')

I61x112 = Parameter(name = 'I61x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd2x3*complexconjugate(Rd1x3)*complexconjugate(Rn1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x112}')

I61x115 = Parameter(name = 'I61x115',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*Rd5x2*complexconjugate(Rd1x3)*complexconjugate(Rn1x3)*complexconjugate(yd2x2)',
                    texname = '\\text{I61x115}')

I61x116 = Parameter(name = 'I61x116',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*Rd6x1*complexconjugate(Rd1x3)*complexconjugate(Rn1x3)*complexconjugate(yd1x1)',
                    texname = '\\text{I61x116}')

I61x121 = Parameter(name = 'I61x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd1x3*complexconjugate(Rd2x3)*complexconjugate(Rn1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x121}')

I61x122 = Parameter(name = 'I61x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd2x3*complexconjugate(Rd2x3)*complexconjugate(Rn1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x122}')

I61x125 = Parameter(name = 'I61x125',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*Rd5x2*complexconjugate(Rd2x3)*complexconjugate(Rn1x3)*complexconjugate(yd2x2)',
                    texname = '\\text{I61x125}')

I61x126 = Parameter(name = 'I61x126',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*Rd6x1*complexconjugate(Rd2x3)*complexconjugate(Rn1x3)*complexconjugate(yd1x1)',
                    texname = '\\text{I61x126}')

I61x151 = Parameter(name = 'I61x151',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd1x3*complexconjugate(Rd5x2)*complexconjugate(Rn1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x151}')

I61x152 = Parameter(name = 'I61x152',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd2x3*complexconjugate(Rd5x2)*complexconjugate(Rn1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x152}')

I61x155 = Parameter(name = 'I61x155',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x2*Rd5x2*complexconjugate(Rd5x2)*complexconjugate(Rn1x3)*complexconjugate(yd2x2)',
                    texname = '\\text{I61x155}')

I61x156 = Parameter(name = 'I61x156',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x1*Rd6x1*complexconjugate(Rd5x2)*complexconjugate(Rn1x3)*complexconjugate(yd1x1)',
                    texname = '\\text{I61x156}')

I61x161 = Parameter(name = 'I61x161',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd1x3*complexconjugate(Rd6x1)*complexconjugate(Rn1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x161}')

I61x162 = Parameter(name = 'I61x162',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd2x3*complexconjugate(Rd6x1)*complexconjugate(Rn1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x162}')

I61x165 = Parameter(name = 'I61x165',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x2*Rd5x2*complexconjugate(Rd6x1)*complexconjugate(Rn1x3)*complexconjugate(yd2x2)',
                    texname = '\\text{I61x165}')

I61x166 = Parameter(name = 'I61x166',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x1*Rd6x1*complexconjugate(Rd6x1)*complexconjugate(Rn1x3)*complexconjugate(yd1x1)',
                    texname = '\\text{I61x166}')

I61x211 = Parameter(name = 'I61x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd1x3*complexconjugate(Rd1x3)*complexconjugate(Rn2x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x211}')

I61x212 = Parameter(name = 'I61x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd2x3*complexconjugate(Rd1x3)*complexconjugate(Rn2x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x212}')

I61x215 = Parameter(name = 'I61x215',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x2*Rd5x2*complexconjugate(Rd1x3)*complexconjugate(Rn2x2)*complexconjugate(yd2x2)',
                    texname = '\\text{I61x215}')

I61x216 = Parameter(name = 'I61x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x1*Rd6x1*complexconjugate(Rd1x3)*complexconjugate(Rn2x2)*complexconjugate(yd1x1)',
                    texname = '\\text{I61x216}')

I61x221 = Parameter(name = 'I61x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd1x3*complexconjugate(Rd2x3)*complexconjugate(Rn2x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x221}')

I61x222 = Parameter(name = 'I61x222',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd2x3*complexconjugate(Rd2x3)*complexconjugate(Rn2x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x222}')

I61x225 = Parameter(name = 'I61x225',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x2*Rd5x2*complexconjugate(Rd2x3)*complexconjugate(Rn2x2)*complexconjugate(yd2x2)',
                    texname = '\\text{I61x225}')

I61x226 = Parameter(name = 'I61x226',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x1*Rd6x1*complexconjugate(Rd2x3)*complexconjugate(Rn2x2)*complexconjugate(yd1x1)',
                    texname = '\\text{I61x226}')

I61x251 = Parameter(name = 'I61x251',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x3*Rd1x3*complexconjugate(Rd5x2)*complexconjugate(Rn2x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x251}')

I61x252 = Parameter(name = 'I61x252',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x3*Rd2x3*complexconjugate(Rd5x2)*complexconjugate(Rn2x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x252}')

I61x255 = Parameter(name = 'I61x255',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x2*Rd5x2*complexconjugate(Rd5x2)*complexconjugate(Rn2x2)*complexconjugate(yd2x2)',
                    texname = '\\text{I61x255}')

I61x256 = Parameter(name = 'I61x256',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x1*Rd6x1*complexconjugate(Rd5x2)*complexconjugate(Rn2x2)*complexconjugate(yd1x1)',
                    texname = '\\text{I61x256}')

I61x261 = Parameter(name = 'I61x261',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x3*Rd1x3*complexconjugate(Rd6x1)*complexconjugate(Rn2x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x261}')

I61x262 = Parameter(name = 'I61x262',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x3*Rd2x3*complexconjugate(Rd6x1)*complexconjugate(Rn2x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x262}')

I61x265 = Parameter(name = 'I61x265',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x2*Rd5x2*complexconjugate(Rd6x1)*complexconjugate(Rn2x2)*complexconjugate(yd2x2)',
                    texname = '\\text{I61x265}')

I61x266 = Parameter(name = 'I61x266',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x1*Rd6x1*complexconjugate(Rd6x1)*complexconjugate(Rn2x2)*complexconjugate(yd1x1)',
                    texname = '\\text{I61x266}')

I61x311 = Parameter(name = 'I61x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd1x3*complexconjugate(Rd1x3)*complexconjugate(Rn3x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x311}')

I61x312 = Parameter(name = 'I61x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd2x3*complexconjugate(Rd1x3)*complexconjugate(Rn3x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x312}')

I61x315 = Parameter(name = 'I61x315',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x2*Rd5x2*complexconjugate(Rd1x3)*complexconjugate(Rn3x1)*complexconjugate(yd2x2)',
                    texname = '\\text{I61x315}')

I61x316 = Parameter(name = 'I61x316',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x1*Rd6x1*complexconjugate(Rd1x3)*complexconjugate(Rn3x1)*complexconjugate(yd1x1)',
                    texname = '\\text{I61x316}')

I61x321 = Parameter(name = 'I61x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd1x3*complexconjugate(Rd2x3)*complexconjugate(Rn3x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x321}')

I61x322 = Parameter(name = 'I61x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd2x3*complexconjugate(Rd2x3)*complexconjugate(Rn3x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x322}')

I61x325 = Parameter(name = 'I61x325',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x2*Rd5x2*complexconjugate(Rd2x3)*complexconjugate(Rn3x1)*complexconjugate(yd2x2)',
                    texname = '\\text{I61x325}')

I61x326 = Parameter(name = 'I61x326',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x1*Rd6x1*complexconjugate(Rd2x3)*complexconjugate(Rn3x1)*complexconjugate(yd1x1)',
                    texname = '\\text{I61x326}')

I61x351 = Parameter(name = 'I61x351',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x3*Rd1x3*complexconjugate(Rd5x2)*complexconjugate(Rn3x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x351}')

I61x352 = Parameter(name = 'I61x352',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x3*Rd2x3*complexconjugate(Rd5x2)*complexconjugate(Rn3x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x352}')

I61x355 = Parameter(name = 'I61x355',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x2*Rd5x2*complexconjugate(Rd5x2)*complexconjugate(Rn3x1)*complexconjugate(yd2x2)',
                    texname = '\\text{I61x355}')

I61x356 = Parameter(name = 'I61x356',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x1*Rd6x1*complexconjugate(Rd5x2)*complexconjugate(Rn3x1)*complexconjugate(yd1x1)',
                    texname = '\\text{I61x356}')

I61x361 = Parameter(name = 'I61x361',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x3*Rd1x3*complexconjugate(Rd6x1)*complexconjugate(Rn3x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x361}')

I61x362 = Parameter(name = 'I61x362',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x3*Rd2x3*complexconjugate(Rd6x1)*complexconjugate(Rn3x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I61x362}')

I61x365 = Parameter(name = 'I61x365',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x2*Rd5x2*complexconjugate(Rd6x1)*complexconjugate(Rn3x1)*complexconjugate(yd2x2)',
                    texname = '\\text{I61x365}')

I61x366 = Parameter(name = 'I61x366',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x1*Rd6x1*complexconjugate(Rd6x1)*complexconjugate(Rn3x1)*complexconjugate(yd1x1)',
                    texname = '\\text{I61x366}')

I62x111 = Parameter(name = 'I62x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd1x6*complexconjugate(Rd1x6)*complexconjugate(Rn1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x111}')

I62x112 = Parameter(name = 'I62x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd2x6*complexconjugate(Rd1x6)*complexconjugate(Rn1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x112}')

I62x113 = Parameter(name = 'I62x113',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*Rd3x5*complexconjugate(Rd1x6)*complexconjugate(Rn1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x113}')

I62x114 = Parameter(name = 'I62x114',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*Rd4x4*complexconjugate(Rd1x6)*complexconjugate(Rn1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x114}')

I62x121 = Parameter(name = 'I62x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd1x6*complexconjugate(Rd2x6)*complexconjugate(Rn1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x121}')

I62x122 = Parameter(name = 'I62x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd2x6*complexconjugate(Rd2x6)*complexconjugate(Rn1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x122}')

I62x123 = Parameter(name = 'I62x123',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*Rd3x5*complexconjugate(Rd2x6)*complexconjugate(Rn1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x123}')

I62x124 = Parameter(name = 'I62x124',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*Rd4x4*complexconjugate(Rd2x6)*complexconjugate(Rn1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x124}')

I62x131 = Parameter(name = 'I62x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd1x6*complexconjugate(Rd3x5)*complexconjugate(Rn1x3)*complexconjugate(yd2x2)',
                    texname = '\\text{I62x131}')

I62x132 = Parameter(name = 'I62x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd2x6*complexconjugate(Rd3x5)*complexconjugate(Rn1x3)*complexconjugate(yd2x2)',
                    texname = '\\text{I62x132}')

I62x133 = Parameter(name = 'I62x133',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x2*Rd3x5*complexconjugate(Rd3x5)*complexconjugate(Rn1x3)*complexconjugate(yd2x2)',
                    texname = '\\text{I62x133}')

I62x134 = Parameter(name = 'I62x134',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x1*Rd4x4*complexconjugate(Rd3x5)*complexconjugate(Rn1x3)*complexconjugate(yd2x2)',
                    texname = '\\text{I62x134}')

I62x141 = Parameter(name = 'I62x141',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd1x6*complexconjugate(Rd4x4)*complexconjugate(Rn1x3)*complexconjugate(yd1x1)',
                    texname = '\\text{I62x141}')

I62x142 = Parameter(name = 'I62x142',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd2x6*complexconjugate(Rd4x4)*complexconjugate(Rn1x3)*complexconjugate(yd1x1)',
                    texname = '\\text{I62x142}')

I62x143 = Parameter(name = 'I62x143',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x2*Rd3x5*complexconjugate(Rd4x4)*complexconjugate(Rn1x3)*complexconjugate(yd1x1)',
                    texname = '\\text{I62x143}')

I62x144 = Parameter(name = 'I62x144',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x1*Rd4x4*complexconjugate(Rd4x4)*complexconjugate(Rn1x3)*complexconjugate(yd1x1)',
                    texname = '\\text{I62x144}')

I62x211 = Parameter(name = 'I62x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd1x6*complexconjugate(Rd1x6)*complexconjugate(Rn2x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x211}')

I62x212 = Parameter(name = 'I62x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd2x6*complexconjugate(Rd1x6)*complexconjugate(Rn2x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x212}')

I62x213 = Parameter(name = 'I62x213',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x2*Rd3x5*complexconjugate(Rd1x6)*complexconjugate(Rn2x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x213}')

I62x214 = Parameter(name = 'I62x214',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x1*Rd4x4*complexconjugate(Rd1x6)*complexconjugate(Rn2x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x214}')

I62x221 = Parameter(name = 'I62x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd1x6*complexconjugate(Rd2x6)*complexconjugate(Rn2x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x221}')

I62x222 = Parameter(name = 'I62x222',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd2x6*complexconjugate(Rd2x6)*complexconjugate(Rn2x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x222}')

I62x223 = Parameter(name = 'I62x223',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x2*Rd3x5*complexconjugate(Rd2x6)*complexconjugate(Rn2x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x223}')

I62x224 = Parameter(name = 'I62x224',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x1*Rd4x4*complexconjugate(Rd2x6)*complexconjugate(Rn2x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x224}')

I62x231 = Parameter(name = 'I62x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x3*Rd1x6*complexconjugate(Rd3x5)*complexconjugate(Rn2x2)*complexconjugate(yd2x2)',
                    texname = '\\text{I62x231}')

I62x232 = Parameter(name = 'I62x232',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x3*Rd2x6*complexconjugate(Rd3x5)*complexconjugate(Rn2x2)*complexconjugate(yd2x2)',
                    texname = '\\text{I62x232}')

I62x233 = Parameter(name = 'I62x233',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x2*Rd3x5*complexconjugate(Rd3x5)*complexconjugate(Rn2x2)*complexconjugate(yd2x2)',
                    texname = '\\text{I62x233}')

I62x234 = Parameter(name = 'I62x234',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x1*Rd4x4*complexconjugate(Rd3x5)*complexconjugate(Rn2x2)*complexconjugate(yd2x2)',
                    texname = '\\text{I62x234}')

I62x241 = Parameter(name = 'I62x241',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x3*Rd1x6*complexconjugate(Rd4x4)*complexconjugate(Rn2x2)*complexconjugate(yd1x1)',
                    texname = '\\text{I62x241}')

I62x242 = Parameter(name = 'I62x242',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x3*Rd2x6*complexconjugate(Rd4x4)*complexconjugate(Rn2x2)*complexconjugate(yd1x1)',
                    texname = '\\text{I62x242}')

I62x243 = Parameter(name = 'I62x243',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x2*Rd3x5*complexconjugate(Rd4x4)*complexconjugate(Rn2x2)*complexconjugate(yd1x1)',
                    texname = '\\text{I62x243}')

I62x244 = Parameter(name = 'I62x244',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x1*Rd4x4*complexconjugate(Rd4x4)*complexconjugate(Rn2x2)*complexconjugate(yd1x1)',
                    texname = '\\text{I62x244}')

I62x311 = Parameter(name = 'I62x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd1x6*complexconjugate(Rd1x6)*complexconjugate(Rn3x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x311}')

I62x312 = Parameter(name = 'I62x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd2x6*complexconjugate(Rd1x6)*complexconjugate(Rn3x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x312}')

I62x313 = Parameter(name = 'I62x313',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x2*Rd3x5*complexconjugate(Rd1x6)*complexconjugate(Rn3x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x313}')

I62x314 = Parameter(name = 'I62x314',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x1*Rd4x4*complexconjugate(Rd1x6)*complexconjugate(Rn3x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x314}')

I62x321 = Parameter(name = 'I62x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd1x6*complexconjugate(Rd2x6)*complexconjugate(Rn3x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x321}')

I62x322 = Parameter(name = 'I62x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd2x6*complexconjugate(Rd2x6)*complexconjugate(Rn3x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x322}')

I62x323 = Parameter(name = 'I62x323',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x2*Rd3x5*complexconjugate(Rd2x6)*complexconjugate(Rn3x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x323}')

I62x324 = Parameter(name = 'I62x324',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x1*Rd4x4*complexconjugate(Rd2x6)*complexconjugate(Rn3x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I62x324}')

I62x331 = Parameter(name = 'I62x331',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x3*Rd1x6*complexconjugate(Rd3x5)*complexconjugate(Rn3x1)*complexconjugate(yd2x2)',
                    texname = '\\text{I62x331}')

I62x332 = Parameter(name = 'I62x332',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x3*Rd2x6*complexconjugate(Rd3x5)*complexconjugate(Rn3x1)*complexconjugate(yd2x2)',
                    texname = '\\text{I62x332}')

I62x333 = Parameter(name = 'I62x333',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x2*Rd3x5*complexconjugate(Rd3x5)*complexconjugate(Rn3x1)*complexconjugate(yd2x2)',
                    texname = '\\text{I62x333}')

I62x334 = Parameter(name = 'I62x334',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x1*Rd4x4*complexconjugate(Rd3x5)*complexconjugate(Rn3x1)*complexconjugate(yd2x2)',
                    texname = '\\text{I62x334}')

I62x341 = Parameter(name = 'I62x341',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x3*Rd1x6*complexconjugate(Rd4x4)*complexconjugate(Rn3x1)*complexconjugate(yd1x1)',
                    texname = '\\text{I62x341}')

I62x342 = Parameter(name = 'I62x342',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x3*Rd2x6*complexconjugate(Rd4x4)*complexconjugate(Rn3x1)*complexconjugate(yd1x1)',
                    texname = '\\text{I62x342}')

I62x343 = Parameter(name = 'I62x343',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x2*Rd3x5*complexconjugate(Rd4x4)*complexconjugate(Rn3x1)*complexconjugate(yd1x1)',
                    texname = '\\text{I62x343}')

I62x344 = Parameter(name = 'I62x344',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x1*Rd4x4*complexconjugate(Rd4x4)*complexconjugate(Rn3x1)*complexconjugate(yd1x1)',
                    texname = '\\text{I62x344}')

I63x111 = Parameter(name = 'I63x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD3x3x3*complexconjugate(Rd1x3)*complexconjugate(Rn1x3)',
                    texname = '\\text{I63x111}')

I63x112 = Parameter(name = 'I63x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD3x3x3*complexconjugate(Rd1x3)*complexconjugate(Rn1x3)',
                    texname = '\\text{I63x112}')

I63x113 = Parameter(name = 'I63x113',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD3x3x2*complexconjugate(Rd1x3)*complexconjugate(Rn1x3)',
                    texname = '\\text{I63x113}')

I63x114 = Parameter(name = 'I63x114',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD3x3x1*complexconjugate(Rd1x3)*complexconjugate(Rn1x3)',
                    texname = '\\text{I63x114}')

I63x121 = Parameter(name = 'I63x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD3x3x3*complexconjugate(Rd2x3)*complexconjugate(Rn1x3)',
                    texname = '\\text{I63x121}')

I63x122 = Parameter(name = 'I63x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD3x3x3*complexconjugate(Rd2x3)*complexconjugate(Rn1x3)',
                    texname = '\\text{I63x122}')

I63x123 = Parameter(name = 'I63x123',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD3x3x2*complexconjugate(Rd2x3)*complexconjugate(Rn1x3)',
                    texname = '\\text{I63x123}')

I63x124 = Parameter(name = 'I63x124',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD3x3x1*complexconjugate(Rd2x3)*complexconjugate(Rn1x3)',
                    texname = '\\text{I63x124}')

I63x151 = Parameter(name = 'I63x151',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD3x2x3*complexconjugate(Rd5x2)*complexconjugate(Rn1x3)',
                    texname = '\\text{I63x151}')

I63x152 = Parameter(name = 'I63x152',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD3x2x3*complexconjugate(Rd5x2)*complexconjugate(Rn1x3)',
                    texname = '\\text{I63x152}')

I63x153 = Parameter(name = 'I63x153',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD3x2x2*complexconjugate(Rd5x2)*complexconjugate(Rn1x3)',
                    texname = '\\text{I63x153}')

I63x154 = Parameter(name = 'I63x154',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD3x2x1*complexconjugate(Rd5x2)*complexconjugate(Rn1x3)',
                    texname = '\\text{I63x154}')

I63x161 = Parameter(name = 'I63x161',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD3x1x3*complexconjugate(Rd6x1)*complexconjugate(Rn1x3)',
                    texname = '\\text{I63x161}')

I63x162 = Parameter(name = 'I63x162',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD3x1x3*complexconjugate(Rd6x1)*complexconjugate(Rn1x3)',
                    texname = '\\text{I63x162}')

I63x163 = Parameter(name = 'I63x163',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD3x1x2*complexconjugate(Rd6x1)*complexconjugate(Rn1x3)',
                    texname = '\\text{I63x163}')

I63x164 = Parameter(name = 'I63x164',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD3x1x1*complexconjugate(Rd6x1)*complexconjugate(Rn1x3)',
                    texname = '\\text{I63x164}')

I63x211 = Parameter(name = 'I63x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD2x3x3*complexconjugate(Rd1x3)*complexconjugate(Rn2x2)',
                    texname = '\\text{I63x211}')

I63x212 = Parameter(name = 'I63x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD2x3x3*complexconjugate(Rd1x3)*complexconjugate(Rn2x2)',
                    texname = '\\text{I63x212}')

I63x213 = Parameter(name = 'I63x213',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD2x3x2*complexconjugate(Rd1x3)*complexconjugate(Rn2x2)',
                    texname = '\\text{I63x213}')

I63x214 = Parameter(name = 'I63x214',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD2x3x1*complexconjugate(Rd1x3)*complexconjugate(Rn2x2)',
                    texname = '\\text{I63x214}')

I63x221 = Parameter(name = 'I63x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD2x3x3*complexconjugate(Rd2x3)*complexconjugate(Rn2x2)',
                    texname = '\\text{I63x221}')

I63x222 = Parameter(name = 'I63x222',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD2x3x3*complexconjugate(Rd2x3)*complexconjugate(Rn2x2)',
                    texname = '\\text{I63x222}')

I63x223 = Parameter(name = 'I63x223',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD2x3x2*complexconjugate(Rd2x3)*complexconjugate(Rn2x2)',
                    texname = '\\text{I63x223}')

I63x224 = Parameter(name = 'I63x224',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD2x3x1*complexconjugate(Rd2x3)*complexconjugate(Rn2x2)',
                    texname = '\\text{I63x224}')

I63x251 = Parameter(name = 'I63x251',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD2x2x3*complexconjugate(Rd5x2)*complexconjugate(Rn2x2)',
                    texname = '\\text{I63x251}')

I63x252 = Parameter(name = 'I63x252',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD2x2x3*complexconjugate(Rd5x2)*complexconjugate(Rn2x2)',
                    texname = '\\text{I63x252}')

I63x253 = Parameter(name = 'I63x253',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD2x2x2*complexconjugate(Rd5x2)*complexconjugate(Rn2x2)',
                    texname = '\\text{I63x253}')

I63x254 = Parameter(name = 'I63x254',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD2x2x1*complexconjugate(Rd5x2)*complexconjugate(Rn2x2)',
                    texname = '\\text{I63x254}')

I63x261 = Parameter(name = 'I63x261',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD2x1x3*complexconjugate(Rd6x1)*complexconjugate(Rn2x2)',
                    texname = '\\text{I63x261}')

I63x262 = Parameter(name = 'I63x262',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD2x1x3*complexconjugate(Rd6x1)*complexconjugate(Rn2x2)',
                    texname = '\\text{I63x262}')

I63x263 = Parameter(name = 'I63x263',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD2x1x2*complexconjugate(Rd6x1)*complexconjugate(Rn2x2)',
                    texname = '\\text{I63x263}')

I63x264 = Parameter(name = 'I63x264',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD2x1x1*complexconjugate(Rd6x1)*complexconjugate(Rn2x2)',
                    texname = '\\text{I63x264}')

I63x311 = Parameter(name = 'I63x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD1x3x3*complexconjugate(Rd1x3)*complexconjugate(Rn3x1)',
                    texname = '\\text{I63x311}')

I63x312 = Parameter(name = 'I63x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD1x3x3*complexconjugate(Rd1x3)*complexconjugate(Rn3x1)',
                    texname = '\\text{I63x312}')

I63x313 = Parameter(name = 'I63x313',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD1x3x2*complexconjugate(Rd1x3)*complexconjugate(Rn3x1)',
                    texname = '\\text{I63x313}')

I63x314 = Parameter(name = 'I63x314',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD1x3x1*complexconjugate(Rd1x3)*complexconjugate(Rn3x1)',
                    texname = '\\text{I63x314}')

I63x321 = Parameter(name = 'I63x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD1x3x3*complexconjugate(Rd2x3)*complexconjugate(Rn3x1)',
                    texname = '\\text{I63x321}')

I63x322 = Parameter(name = 'I63x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD1x3x3*complexconjugate(Rd2x3)*complexconjugate(Rn3x1)',
                    texname = '\\text{I63x322}')

I63x323 = Parameter(name = 'I63x323',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD1x3x2*complexconjugate(Rd2x3)*complexconjugate(Rn3x1)',
                    texname = '\\text{I63x323}')

I63x324 = Parameter(name = 'I63x324',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD1x3x1*complexconjugate(Rd2x3)*complexconjugate(Rn3x1)',
                    texname = '\\text{I63x324}')

I63x351 = Parameter(name = 'I63x351',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD1x2x3*complexconjugate(Rd5x2)*complexconjugate(Rn3x1)',
                    texname = '\\text{I63x351}')

I63x352 = Parameter(name = 'I63x352',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD1x2x3*complexconjugate(Rd5x2)*complexconjugate(Rn3x1)',
                    texname = '\\text{I63x352}')

I63x353 = Parameter(name = 'I63x353',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD1x2x2*complexconjugate(Rd5x2)*complexconjugate(Rn3x1)',
                    texname = '\\text{I63x353}')

I63x354 = Parameter(name = 'I63x354',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD1x2x1*complexconjugate(Rd5x2)*complexconjugate(Rn3x1)',
                    texname = '\\text{I63x354}')

I63x361 = Parameter(name = 'I63x361',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD1x1x3*complexconjugate(Rd6x1)*complexconjugate(Rn3x1)',
                    texname = '\\text{I63x361}')

I63x362 = Parameter(name = 'I63x362',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD1x1x3*complexconjugate(Rd6x1)*complexconjugate(Rn3x1)',
                    texname = '\\text{I63x362}')

I63x363 = Parameter(name = 'I63x363',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD1x1x2*complexconjugate(Rd6x1)*complexconjugate(Rn3x1)',
                    texname = '\\text{I63x363}')

I63x364 = Parameter(name = 'I63x364',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD1x1x1*complexconjugate(Rd6x1)*complexconjugate(Rn3x1)',
                    texname = '\\text{I63x364}')

I64x141 = Parameter(name = 'I64x141',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x1x3*Rl1x3*complexconjugate(Rl4x1)*complexconjugate(Rn1x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I64x141}')

I64x144 = Parameter(name = 'I64x144',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x1x1*Rl4x1*complexconjugate(Rl4x1)*complexconjugate(Rn1x3)*complexconjugate(ye1x1)',
                    texname = '\\text{I64x144}')

I64x145 = Parameter(name = 'I64x145',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x1x2*Rl5x2*complexconjugate(Rl4x1)*complexconjugate(Rn1x3)*complexconjugate(ye2x2)',
                    texname = '\\text{I64x145}')

I64x146 = Parameter(name = 'I64x146',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x1x3*Rl6x3*complexconjugate(Rl4x1)*complexconjugate(Rn1x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I64x146}')

I64x151 = Parameter(name = 'I64x151',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x2x3*Rl1x3*complexconjugate(Rl5x2)*complexconjugate(Rn1x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I64x151}')

I64x154 = Parameter(name = 'I64x154',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x2x1*Rl4x1*complexconjugate(Rl5x2)*complexconjugate(Rn1x3)*complexconjugate(ye1x1)',
                    texname = '\\text{I64x154}')

I64x155 = Parameter(name = 'I64x155',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x2x2*Rl5x2*complexconjugate(Rl5x2)*complexconjugate(Rn1x3)*complexconjugate(ye2x2)',
                    texname = '\\text{I64x155}')

I64x156 = Parameter(name = 'I64x156',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x2x3*Rl6x3*complexconjugate(Rl5x2)*complexconjugate(Rn1x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I64x156}')

I64x211 = Parameter(name = 'I64x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x3*Rl1x3*complexconjugate(Rl1x3)*complexconjugate(Rn2x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I64x211}')

I64x214 = Parameter(name = 'I64x214',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x1*Rl4x1*complexconjugate(Rl1x3)*complexconjugate(Rn2x2)*complexconjugate(ye1x1)',
                    texname = '\\text{I64x214}')

I64x215 = Parameter(name = 'I64x215',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x2*Rl5x2*complexconjugate(Rl1x3)*complexconjugate(Rn2x2)*complexconjugate(ye2x2)',
                    texname = '\\text{I64x215}')

I64x216 = Parameter(name = 'I64x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x3*Rl6x3*complexconjugate(Rl1x3)*complexconjugate(Rn2x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I64x216}')

I64x241 = Parameter(name = 'I64x241',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x1x3*Rl1x3*complexconjugate(Rl4x1)*complexconjugate(Rn2x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I64x241}')

I64x244 = Parameter(name = 'I64x244',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x1x1*Rl4x1*complexconjugate(Rl4x1)*complexconjugate(Rn2x2)*complexconjugate(ye1x1)',
                    texname = '\\text{I64x244}')

I64x245 = Parameter(name = 'I64x245',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x1x2*Rl5x2*complexconjugate(Rl4x1)*complexconjugate(Rn2x2)*complexconjugate(ye2x2)',
                    texname = '\\text{I64x245}')

I64x246 = Parameter(name = 'I64x246',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x1x3*Rl6x3*complexconjugate(Rl4x1)*complexconjugate(Rn2x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I64x246}')

I64x261 = Parameter(name = 'I64x261',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x3*Rl1x3*complexconjugate(Rl6x3)*complexconjugate(Rn2x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I64x261}')

I64x264 = Parameter(name = 'I64x264',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x1*Rl4x1*complexconjugate(Rl6x3)*complexconjugate(Rn2x2)*complexconjugate(ye1x1)',
                    texname = '\\text{I64x264}')

I64x265 = Parameter(name = 'I64x265',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x2*Rl5x2*complexconjugate(Rl6x3)*complexconjugate(Rn2x2)*complexconjugate(ye2x2)',
                    texname = '\\text{I64x265}')

I64x266 = Parameter(name = 'I64x266',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x3*Rl6x3*complexconjugate(Rl6x3)*complexconjugate(Rn2x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I64x266}')

I64x311 = Parameter(name = 'I64x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x3*Rl1x3*complexconjugate(Rl1x3)*complexconjugate(Rn3x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I64x311}')

I64x314 = Parameter(name = 'I64x314',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x1*Rl4x1*complexconjugate(Rl1x3)*complexconjugate(Rn3x1)*complexconjugate(ye1x1)',
                    texname = '\\text{I64x314}')

I64x315 = Parameter(name = 'I64x315',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x2*Rl5x2*complexconjugate(Rl1x3)*complexconjugate(Rn3x1)*complexconjugate(ye2x2)',
                    texname = '\\text{I64x315}')

I64x316 = Parameter(name = 'I64x316',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x3*Rl6x3*complexconjugate(Rl1x3)*complexconjugate(Rn3x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I64x316}')

I64x351 = Parameter(name = 'I64x351',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x2x3*Rl1x3*complexconjugate(Rl5x2)*complexconjugate(Rn3x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I64x351}')

I64x354 = Parameter(name = 'I64x354',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x2x1*Rl4x1*complexconjugate(Rl5x2)*complexconjugate(Rn3x1)*complexconjugate(ye1x1)',
                    texname = '\\text{I64x354}')

I64x355 = Parameter(name = 'I64x355',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x2x2*Rl5x2*complexconjugate(Rl5x2)*complexconjugate(Rn3x1)*complexconjugate(ye2x2)',
                    texname = '\\text{I64x355}')

I64x356 = Parameter(name = 'I64x356',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x2x3*Rl6x3*complexconjugate(Rl5x2)*complexconjugate(Rn3x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I64x356}')

I64x361 = Parameter(name = 'I64x361',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x3*Rl1x3*complexconjugate(Rl6x3)*complexconjugate(Rn3x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I64x361}')

I64x364 = Parameter(name = 'I64x364',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x1*Rl4x1*complexconjugate(Rl6x3)*complexconjugate(Rn3x1)*complexconjugate(ye1x1)',
                    texname = '\\text{I64x364}')

I64x365 = Parameter(name = 'I64x365',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x2*Rl5x2*complexconjugate(Rl6x3)*complexconjugate(Rn3x1)*complexconjugate(ye2x2)',
                    texname = '\\text{I64x365}')

I64x366 = Parameter(name = 'I64x366',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x3*Rl6x3*complexconjugate(Rl6x3)*complexconjugate(Rn3x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I64x366}')

I65x121 = Parameter(name = 'I65x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x2x3*Rl1x6*complexconjugate(Rl2x5)*complexconjugate(Rn1x3)*complexconjugate(ye2x2)',
                    texname = '\\text{I65x121}')

I65x122 = Parameter(name = 'I65x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x2x2*Rl2x5*complexconjugate(Rl2x5)*complexconjugate(Rn1x3)*complexconjugate(ye2x2)',
                    texname = '\\text{I65x122}')

I65x123 = Parameter(name = 'I65x123',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x2x1*Rl3x4*complexconjugate(Rl2x5)*complexconjugate(Rn1x3)*complexconjugate(ye2x2)',
                    texname = '\\text{I65x123}')

I65x126 = Parameter(name = 'I65x126',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x2x3*Rl6x6*complexconjugate(Rl2x5)*complexconjugate(Rn1x3)*complexconjugate(ye2x2)',
                    texname = '\\text{I65x126}')

I65x131 = Parameter(name = 'I65x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x1x3*Rl1x6*complexconjugate(Rl3x4)*complexconjugate(Rn1x3)*complexconjugate(ye1x1)',
                    texname = '\\text{I65x131}')

I65x132 = Parameter(name = 'I65x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x1x2*Rl2x5*complexconjugate(Rl3x4)*complexconjugate(Rn1x3)*complexconjugate(ye1x1)',
                    texname = '\\text{I65x132}')

I65x133 = Parameter(name = 'I65x133',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x1x1*Rl3x4*complexconjugate(Rl3x4)*complexconjugate(Rn1x3)*complexconjugate(ye1x1)',
                    texname = '\\text{I65x133}')

I65x136 = Parameter(name = 'I65x136',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE3x1x3*Rl6x6*complexconjugate(Rl3x4)*complexconjugate(Rn1x3)*complexconjugate(ye1x1)',
                    texname = '\\text{I65x136}')

I65x211 = Parameter(name = 'I65x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x3*Rl1x6*complexconjugate(Rl1x6)*complexconjugate(Rn2x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I65x211}')

I65x212 = Parameter(name = 'I65x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x2*Rl2x5*complexconjugate(Rl1x6)*complexconjugate(Rn2x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I65x212}')

I65x213 = Parameter(name = 'I65x213',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x1*Rl3x4*complexconjugate(Rl1x6)*complexconjugate(Rn2x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I65x213}')

I65x216 = Parameter(name = 'I65x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x3*Rl6x6*complexconjugate(Rl1x6)*complexconjugate(Rn2x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I65x216}')

I65x231 = Parameter(name = 'I65x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x1x3*Rl1x6*complexconjugate(Rl3x4)*complexconjugate(Rn2x2)*complexconjugate(ye1x1)',
                    texname = '\\text{I65x231}')

I65x232 = Parameter(name = 'I65x232',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x1x2*Rl2x5*complexconjugate(Rl3x4)*complexconjugate(Rn2x2)*complexconjugate(ye1x1)',
                    texname = '\\text{I65x232}')

I65x233 = Parameter(name = 'I65x233',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x1x1*Rl3x4*complexconjugate(Rl3x4)*complexconjugate(Rn2x2)*complexconjugate(ye1x1)',
                    texname = '\\text{I65x233}')

I65x236 = Parameter(name = 'I65x236',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x1x3*Rl6x6*complexconjugate(Rl3x4)*complexconjugate(Rn2x2)*complexconjugate(ye1x1)',
                    texname = '\\text{I65x236}')

I65x261 = Parameter(name = 'I65x261',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x3*Rl1x6*complexconjugate(Rl6x6)*complexconjugate(Rn2x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I65x261}')

I65x262 = Parameter(name = 'I65x262',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x2*Rl2x5*complexconjugate(Rl6x6)*complexconjugate(Rn2x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I65x262}')

I65x263 = Parameter(name = 'I65x263',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x1*Rl3x4*complexconjugate(Rl6x6)*complexconjugate(Rn2x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I65x263}')

I65x266 = Parameter(name = 'I65x266',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE2x3x3*Rl6x6*complexconjugate(Rl6x6)*complexconjugate(Rn2x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I65x266}')

I65x311 = Parameter(name = 'I65x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x3*Rl1x6*complexconjugate(Rl1x6)*complexconjugate(Rn3x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I65x311}')

I65x312 = Parameter(name = 'I65x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x2*Rl2x5*complexconjugate(Rl1x6)*complexconjugate(Rn3x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I65x312}')

I65x313 = Parameter(name = 'I65x313',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x1*Rl3x4*complexconjugate(Rl1x6)*complexconjugate(Rn3x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I65x313}')

I65x316 = Parameter(name = 'I65x316',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x3*Rl6x6*complexconjugate(Rl1x6)*complexconjugate(Rn3x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I65x316}')

I65x321 = Parameter(name = 'I65x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x2x3*Rl1x6*complexconjugate(Rl2x5)*complexconjugate(Rn3x1)*complexconjugate(ye2x2)',
                    texname = '\\text{I65x321}')

I65x322 = Parameter(name = 'I65x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x2x2*Rl2x5*complexconjugate(Rl2x5)*complexconjugate(Rn3x1)*complexconjugate(ye2x2)',
                    texname = '\\text{I65x322}')

I65x323 = Parameter(name = 'I65x323',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x2x1*Rl3x4*complexconjugate(Rl2x5)*complexconjugate(Rn3x1)*complexconjugate(ye2x2)',
                    texname = '\\text{I65x323}')

I65x326 = Parameter(name = 'I65x326',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x2x3*Rl6x6*complexconjugate(Rl2x5)*complexconjugate(Rn3x1)*complexconjugate(ye2x2)',
                    texname = '\\text{I65x326}')

I65x361 = Parameter(name = 'I65x361',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x3*Rl1x6*complexconjugate(Rl6x6)*complexconjugate(Rn3x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I65x361}')

I65x362 = Parameter(name = 'I65x362',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x2*Rl2x5*complexconjugate(Rl6x6)*complexconjugate(Rn3x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I65x362}')

I65x363 = Parameter(name = 'I65x363',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x1*Rl3x4*complexconjugate(Rl6x6)*complexconjugate(Rn3x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I65x363}')

I65x366 = Parameter(name = 'I65x366',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLLE1x3x3*Rl6x6*complexconjugate(Rl6x6)*complexconjugate(Rn3x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I65x366}')

I66x141 = Parameter(name = 'I66x141',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x6*TLLE3x1x3*complexconjugate(Rl4x1)*complexconjugate(Rn1x3)',
                    texname = '\\text{I66x141}')

I66x142 = Parameter(name = 'I66x142',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl2x5*TLLE3x1x2*complexconjugate(Rl4x1)*complexconjugate(Rn1x3)',
                    texname = '\\text{I66x142}')

I66x143 = Parameter(name = 'I66x143',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl3x4*TLLE3x1x1*complexconjugate(Rl4x1)*complexconjugate(Rn1x3)',
                    texname = '\\text{I66x143}')

I66x146 = Parameter(name = 'I66x146',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x6*TLLE3x1x3*complexconjugate(Rl4x1)*complexconjugate(Rn1x3)',
                    texname = '\\text{I66x146}')

I66x151 = Parameter(name = 'I66x151',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x6*TLLE3x2x3*complexconjugate(Rl5x2)*complexconjugate(Rn1x3)',
                    texname = '\\text{I66x151}')

I66x152 = Parameter(name = 'I66x152',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl2x5*TLLE3x2x2*complexconjugate(Rl5x2)*complexconjugate(Rn1x3)',
                    texname = '\\text{I66x152}')

I66x153 = Parameter(name = 'I66x153',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl3x4*TLLE3x2x1*complexconjugate(Rl5x2)*complexconjugate(Rn1x3)',
                    texname = '\\text{I66x153}')

I66x156 = Parameter(name = 'I66x156',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x6*TLLE3x2x3*complexconjugate(Rl5x2)*complexconjugate(Rn1x3)',
                    texname = '\\text{I66x156}')

I66x211 = Parameter(name = 'I66x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x6*TLLE2x3x3*complexconjugate(Rl1x3)*complexconjugate(Rn2x2)',
                    texname = '\\text{I66x211}')

I66x212 = Parameter(name = 'I66x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl2x5*TLLE2x3x2*complexconjugate(Rl1x3)*complexconjugate(Rn2x2)',
                    texname = '\\text{I66x212}')

I66x213 = Parameter(name = 'I66x213',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl3x4*TLLE2x3x1*complexconjugate(Rl1x3)*complexconjugate(Rn2x2)',
                    texname = '\\text{I66x213}')

I66x216 = Parameter(name = 'I66x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x6*TLLE2x3x3*complexconjugate(Rl1x3)*complexconjugate(Rn2x2)',
                    texname = '\\text{I66x216}')

I66x241 = Parameter(name = 'I66x241',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x6*TLLE2x1x3*complexconjugate(Rl4x1)*complexconjugate(Rn2x2)',
                    texname = '\\text{I66x241}')

I66x242 = Parameter(name = 'I66x242',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl2x5*TLLE2x1x2*complexconjugate(Rl4x1)*complexconjugate(Rn2x2)',
                    texname = '\\text{I66x242}')

I66x243 = Parameter(name = 'I66x243',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl3x4*TLLE2x1x1*complexconjugate(Rl4x1)*complexconjugate(Rn2x2)',
                    texname = '\\text{I66x243}')

I66x246 = Parameter(name = 'I66x246',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x6*TLLE2x1x3*complexconjugate(Rl4x1)*complexconjugate(Rn2x2)',
                    texname = '\\text{I66x246}')

I66x261 = Parameter(name = 'I66x261',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x6*TLLE2x3x3*complexconjugate(Rl6x3)*complexconjugate(Rn2x2)',
                    texname = '\\text{I66x261}')

I66x262 = Parameter(name = 'I66x262',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl2x5*TLLE2x3x2*complexconjugate(Rl6x3)*complexconjugate(Rn2x2)',
                    texname = '\\text{I66x262}')

I66x263 = Parameter(name = 'I66x263',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl3x4*TLLE2x3x1*complexconjugate(Rl6x3)*complexconjugate(Rn2x2)',
                    texname = '\\text{I66x263}')

I66x266 = Parameter(name = 'I66x266',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x6*TLLE2x3x3*complexconjugate(Rl6x3)*complexconjugate(Rn2x2)',
                    texname = '\\text{I66x266}')

I66x311 = Parameter(name = 'I66x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x6*TLLE1x3x3*complexconjugate(Rl1x3)*complexconjugate(Rn3x1)',
                    texname = '\\text{I66x311}')

I66x312 = Parameter(name = 'I66x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl2x5*TLLE1x3x2*complexconjugate(Rl1x3)*complexconjugate(Rn3x1)',
                    texname = '\\text{I66x312}')

I66x313 = Parameter(name = 'I66x313',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl3x4*TLLE1x3x1*complexconjugate(Rl1x3)*complexconjugate(Rn3x1)',
                    texname = '\\text{I66x313}')

I66x316 = Parameter(name = 'I66x316',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x6*TLLE1x3x3*complexconjugate(Rl1x3)*complexconjugate(Rn3x1)',
                    texname = '\\text{I66x316}')

I66x351 = Parameter(name = 'I66x351',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x6*TLLE1x2x3*complexconjugate(Rl5x2)*complexconjugate(Rn3x1)',
                    texname = '\\text{I66x351}')

I66x352 = Parameter(name = 'I66x352',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl2x5*TLLE1x2x2*complexconjugate(Rl5x2)*complexconjugate(Rn3x1)',
                    texname = '\\text{I66x352}')

I66x353 = Parameter(name = 'I66x353',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl3x4*TLLE1x2x1*complexconjugate(Rl5x2)*complexconjugate(Rn3x1)',
                    texname = '\\text{I66x353}')

I66x356 = Parameter(name = 'I66x356',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x6*TLLE1x2x3*complexconjugate(Rl5x2)*complexconjugate(Rn3x1)',
                    texname = '\\text{I66x356}')

I66x361 = Parameter(name = 'I66x361',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x6*TLLE1x3x3*complexconjugate(Rl6x3)*complexconjugate(Rn3x1)',
                    texname = '\\text{I66x361}')

I66x362 = Parameter(name = 'I66x362',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl2x5*TLLE1x3x2*complexconjugate(Rl6x3)*complexconjugate(Rn3x1)',
                    texname = '\\text{I66x362}')

I66x363 = Parameter(name = 'I66x363',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl3x4*TLLE1x3x1*complexconjugate(Rl6x3)*complexconjugate(Rn3x1)',
                    texname = '\\text{I66x363}')

I66x366 = Parameter(name = 'I66x366',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x6*TLLE1x3x3*complexconjugate(Rl6x3)*complexconjugate(Rn3x1)',
                    texname = '\\text{I66x366}')

I67x13 = Parameter(name = 'I67x13',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn3x1',
                   texname = '\\text{I67x13}')

I67x22 = Parameter(name = 'I67x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn2x2',
                   texname = '\\text{I67x22}')

I67x31 = Parameter(name = 'I67x31',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn1x3',
                   texname = '\\text{I67x31}')

I68x13 = Parameter(name = 'I68x13',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn3x1*complexconjugate(ye1x1)',
                   texname = '\\text{I68x13}')

I68x22 = Parameter(name = 'I68x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn2x2*complexconjugate(ye2x2)',
                   texname = '\\text{I68x22}')

I68x31 = Parameter(name = 'I68x31',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn1x3*complexconjugate(ye3x3)',
                   texname = '\\text{I68x31}')

I69x11 = Parameter(name = 'I69x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn1x3*complexconjugate(Rl1x3)',
                   texname = '\\text{I69x11}')

I69x16 = Parameter(name = 'I69x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn1x3*complexconjugate(Rl6x3)',
                   texname = '\\text{I69x16}')

I69x25 = Parameter(name = 'I69x25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn2x2*complexconjugate(Rl5x2)',
                   texname = '\\text{I69x25}')

I69x34 = Parameter(name = 'I69x34',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn3x1*complexconjugate(Rl4x1)',
                   texname = '\\text{I69x34}')

I7x111 = Parameter(name = 'I7x111',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD1x3x1*complexconjugate(Rd1x3)',
                   texname = '\\text{I7x111}')

I7x112 = Parameter(name = 'I7x112',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD1x3x1*complexconjugate(Rd2x3)',
                   texname = '\\text{I7x112}')

I7x115 = Parameter(name = 'I7x115',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD1x2x1*complexconjugate(Rd5x2)',
                   texname = '\\text{I7x115}')

I7x116 = Parameter(name = 'I7x116',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD1x1x1*complexconjugate(Rd6x1)',
                   texname = '\\text{I7x116}')

I7x121 = Parameter(name = 'I7x121',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD2x3x1*complexconjugate(Rd1x3)',
                   texname = '\\text{I7x121}')

I7x122 = Parameter(name = 'I7x122',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD2x3x1*complexconjugate(Rd2x3)',
                   texname = '\\text{I7x122}')

I7x125 = Parameter(name = 'I7x125',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD2x2x1*complexconjugate(Rd5x2)',
                   texname = '\\text{I7x125}')

I7x126 = Parameter(name = 'I7x126',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD2x1x1*complexconjugate(Rd6x1)',
                   texname = '\\text{I7x126}')

I7x131 = Parameter(name = 'I7x131',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD3x3x1*complexconjugate(Rd1x3)',
                   texname = '\\text{I7x131}')

I7x132 = Parameter(name = 'I7x132',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD3x3x1*complexconjugate(Rd2x3)',
                   texname = '\\text{I7x132}')

I7x135 = Parameter(name = 'I7x135',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD3x2x1*complexconjugate(Rd5x2)',
                   texname = '\\text{I7x135}')

I7x136 = Parameter(name = 'I7x136',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD3x1x1*complexconjugate(Rd6x1)',
                   texname = '\\text{I7x136}')

I7x211 = Parameter(name = 'I7x211',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD1x3x2*complexconjugate(Rd1x3)',
                   texname = '\\text{I7x211}')

I7x212 = Parameter(name = 'I7x212',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD1x3x2*complexconjugate(Rd2x3)',
                   texname = '\\text{I7x212}')

I7x215 = Parameter(name = 'I7x215',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD1x2x2*complexconjugate(Rd5x2)',
                   texname = '\\text{I7x215}')

I7x216 = Parameter(name = 'I7x216',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD1x1x2*complexconjugate(Rd6x1)',
                   texname = '\\text{I7x216}')

I7x221 = Parameter(name = 'I7x221',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD2x3x2*complexconjugate(Rd1x3)',
                   texname = '\\text{I7x221}')

I7x222 = Parameter(name = 'I7x222',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD2x3x2*complexconjugate(Rd2x3)',
                   texname = '\\text{I7x222}')

I7x225 = Parameter(name = 'I7x225',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD2x2x2*complexconjugate(Rd5x2)',
                   texname = '\\text{I7x225}')

I7x226 = Parameter(name = 'I7x226',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD2x1x2*complexconjugate(Rd6x1)',
                   texname = '\\text{I7x226}')

I7x231 = Parameter(name = 'I7x231',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD3x3x2*complexconjugate(Rd1x3)',
                   texname = '\\text{I7x231}')

I7x232 = Parameter(name = 'I7x232',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD3x3x2*complexconjugate(Rd2x3)',
                   texname = '\\text{I7x232}')

I7x235 = Parameter(name = 'I7x235',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD3x2x2*complexconjugate(Rd5x2)',
                   texname = '\\text{I7x235}')

I7x236 = Parameter(name = 'I7x236',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD3x1x2*complexconjugate(Rd6x1)',
                   texname = '\\text{I7x236}')

I7x311 = Parameter(name = 'I7x311',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD1x3x3*complexconjugate(Rd1x3)',
                   texname = '\\text{I7x311}')

I7x312 = Parameter(name = 'I7x312',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD1x3x3*complexconjugate(Rd2x3)',
                   texname = '\\text{I7x312}')

I7x315 = Parameter(name = 'I7x315',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD1x2x3*complexconjugate(Rd5x2)',
                   texname = '\\text{I7x315}')

I7x316 = Parameter(name = 'I7x316',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD1x1x3*complexconjugate(Rd6x1)',
                   texname = '\\text{I7x316}')

I7x321 = Parameter(name = 'I7x321',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD2x3x3*complexconjugate(Rd1x3)',
                   texname = '\\text{I7x321}')

I7x322 = Parameter(name = 'I7x322',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD2x3x3*complexconjugate(Rd2x3)',
                   texname = '\\text{I7x322}')

I7x325 = Parameter(name = 'I7x325',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD2x2x3*complexconjugate(Rd5x2)',
                   texname = '\\text{I7x325}')

I7x326 = Parameter(name = 'I7x326',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD2x1x3*complexconjugate(Rd6x1)',
                   texname = '\\text{I7x326}')

I7x331 = Parameter(name = 'I7x331',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD3x3x3*complexconjugate(Rd1x3)',
                   texname = '\\text{I7x331}')

I7x332 = Parameter(name = 'I7x332',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD3x3x3*complexconjugate(Rd2x3)',
                   texname = '\\text{I7x332}')

I7x335 = Parameter(name = 'I7x335',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD3x2x3*complexconjugate(Rd5x2)',
                   texname = '\\text{I7x335}')

I7x336 = Parameter(name = 'I7x336',
                   nature = 'internal',
                   type = 'complex',
                   value = 'LLQD3x1x3*complexconjugate(Rd6x1)',
                   texname = '\\text{I7x336}')

I70x11 = Parameter(name = 'I70x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn1x3*complexconjugate(Rl1x6)*complexconjugate(ye3x3)',
                   texname = '\\text{I70x11}')

I70x16 = Parameter(name = 'I70x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn1x3*complexconjugate(Rl6x6)*complexconjugate(ye3x3)',
                   texname = '\\text{I70x16}')

I70x22 = Parameter(name = 'I70x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn2x2*complexconjugate(Rl2x5)*complexconjugate(ye2x2)',
                   texname = '\\text{I70x22}')

I70x33 = Parameter(name = 'I70x33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn3x1*complexconjugate(Rl3x4)*complexconjugate(ye1x1)',
                   texname = '\\text{I70x33}')

I71x11 = Parameter(name = 'I71x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn1x3*complexconjugate(Rl1x6)*complexconjugate(te3x3)',
                   texname = '\\text{I71x11}')

I71x16 = Parameter(name = 'I71x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn1x3*complexconjugate(Rl6x6)*complexconjugate(te3x3)',
                   texname = '\\text{I71x16}')

I71x22 = Parameter(name = 'I71x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn2x2*complexconjugate(Rl2x5)*complexconjugate(te2x2)',
                   texname = '\\text{I71x22}')

I71x33 = Parameter(name = 'I71x33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn3x1*complexconjugate(Rl3x4)*complexconjugate(te1x1)',
                   texname = '\\text{I71x33}')

I72x11 = Parameter(name = 'I72x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn1x3*ye3x3*complexconjugate(Rl1x3)*complexconjugate(ye3x3)',
                   texname = '\\text{I72x11}')

I72x16 = Parameter(name = 'I72x16',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn1x3*ye3x3*complexconjugate(Rl6x3)*complexconjugate(ye3x3)',
                   texname = '\\text{I72x16}')

I72x25 = Parameter(name = 'I72x25',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn2x2*ye2x2*complexconjugate(Rl5x2)*complexconjugate(ye2x2)',
                   texname = '\\text{I72x25}')

I72x34 = Parameter(name = 'I72x34',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rn3x1*ye1x1*complexconjugate(Rl4x1)*complexconjugate(ye1x1)',
                   texname = '\\text{I72x34}')

I73x111 = Parameter(name = 'I73x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn1x3*complexconjugate(LLQD3x1x1)',
                    texname = '\\text{I73x111}')

I73x112 = Parameter(name = 'I73x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn2x2*complexconjugate(LLQD2x1x1)',
                    texname = '\\text{I73x112}')

I73x113 = Parameter(name = 'I73x113',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn3x1*complexconjugate(LLQD1x1x1)',
                    texname = '\\text{I73x113}')

I73x121 = Parameter(name = 'I73x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn1x3*complexconjugate(LLQD3x1x2)',
                    texname = '\\text{I73x121}')

I73x122 = Parameter(name = 'I73x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn2x2*complexconjugate(LLQD2x1x2)',
                    texname = '\\text{I73x122}')

I73x123 = Parameter(name = 'I73x123',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn3x1*complexconjugate(LLQD1x1x2)',
                    texname = '\\text{I73x123}')

I73x131 = Parameter(name = 'I73x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn1x3*complexconjugate(LLQD3x1x3)',
                    texname = '\\text{I73x131}')

I73x132 = Parameter(name = 'I73x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn2x2*complexconjugate(LLQD2x1x3)',
                    texname = '\\text{I73x132}')

I73x133 = Parameter(name = 'I73x133',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn3x1*complexconjugate(LLQD1x1x3)',
                    texname = '\\text{I73x133}')

I73x211 = Parameter(name = 'I73x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn1x3*complexconjugate(LLQD3x2x1)',
                    texname = '\\text{I73x211}')

I73x212 = Parameter(name = 'I73x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn2x2*complexconjugate(LLQD2x2x1)',
                    texname = '\\text{I73x212}')

I73x213 = Parameter(name = 'I73x213',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn3x1*complexconjugate(LLQD1x2x1)',
                    texname = '\\text{I73x213}')

I73x221 = Parameter(name = 'I73x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn1x3*complexconjugate(LLQD3x2x2)',
                    texname = '\\text{I73x221}')

I73x222 = Parameter(name = 'I73x222',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn2x2*complexconjugate(LLQD2x2x2)',
                    texname = '\\text{I73x222}')

I73x223 = Parameter(name = 'I73x223',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn3x1*complexconjugate(LLQD1x2x2)',
                    texname = '\\text{I73x223}')

I73x231 = Parameter(name = 'I73x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn1x3*complexconjugate(LLQD3x2x3)',
                    texname = '\\text{I73x231}')

I73x232 = Parameter(name = 'I73x232',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn2x2*complexconjugate(LLQD2x2x3)',
                    texname = '\\text{I73x232}')

I73x233 = Parameter(name = 'I73x233',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn3x1*complexconjugate(LLQD1x2x3)',
                    texname = '\\text{I73x233}')

I73x311 = Parameter(name = 'I73x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn1x3*complexconjugate(LLQD3x3x1)',
                    texname = '\\text{I73x311}')

I73x312 = Parameter(name = 'I73x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn2x2*complexconjugate(LLQD2x3x1)',
                    texname = '\\text{I73x312}')

I73x313 = Parameter(name = 'I73x313',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn3x1*complexconjugate(LLQD1x3x1)',
                    texname = '\\text{I73x313}')

I73x321 = Parameter(name = 'I73x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn1x3*complexconjugate(LLQD3x3x2)',
                    texname = '\\text{I73x321}')

I73x322 = Parameter(name = 'I73x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn2x2*complexconjugate(LLQD2x3x2)',
                    texname = '\\text{I73x322}')

I73x323 = Parameter(name = 'I73x323',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn3x1*complexconjugate(LLQD1x3x2)',
                    texname = '\\text{I73x323}')

I73x331 = Parameter(name = 'I73x331',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn1x3*complexconjugate(LLQD3x3x3)',
                    texname = '\\text{I73x331}')

I73x332 = Parameter(name = 'I73x332',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn2x2*complexconjugate(LLQD2x3x3)',
                    texname = '\\text{I73x332}')

I73x333 = Parameter(name = 'I73x333',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn3x1*complexconjugate(LLQD1x3x3)',
                    texname = '\\text{I73x333}')

I74x111 = Parameter(name = 'I74x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn1x3*complexconjugate(LLLE3x1x1)',
                    texname = '\\text{I74x111}')

I74x112 = Parameter(name = 'I74x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn2x2*complexconjugate(LLLE2x1x1)',
                    texname = '\\text{I74x112}')

I74x121 = Parameter(name = 'I74x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn1x3*complexconjugate(LLLE3x1x2)',
                    texname = '\\text{I74x121}')

I74x122 = Parameter(name = 'I74x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn2x2*complexconjugate(LLLE2x1x2)',
                    texname = '\\text{I74x122}')

I74x131 = Parameter(name = 'I74x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn1x3*complexconjugate(LLLE3x1x3)',
                    texname = '\\text{I74x131}')

I74x132 = Parameter(name = 'I74x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn2x2*complexconjugate(LLLE2x1x3)',
                    texname = '\\text{I74x132}')

I74x211 = Parameter(name = 'I74x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn1x3*complexconjugate(LLLE3x2x1)',
                    texname = '\\text{I74x211}')

I74x213 = Parameter(name = 'I74x213',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn3x1*complexconjugate(LLLE1x2x1)',
                    texname = '\\text{I74x213}')

I74x221 = Parameter(name = 'I74x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn1x3*complexconjugate(LLLE3x2x2)',
                    texname = '\\text{I74x221}')

I74x223 = Parameter(name = 'I74x223',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn3x1*complexconjugate(LLLE1x2x2)',
                    texname = '\\text{I74x223}')

I74x231 = Parameter(name = 'I74x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn1x3*complexconjugate(LLLE3x2x3)',
                    texname = '\\text{I74x231}')

I74x233 = Parameter(name = 'I74x233',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn3x1*complexconjugate(LLLE1x2x3)',
                    texname = '\\text{I74x233}')

I74x312 = Parameter(name = 'I74x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn2x2*complexconjugate(LLLE2x3x1)',
                    texname = '\\text{I74x312}')

I74x313 = Parameter(name = 'I74x313',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn3x1*complexconjugate(LLLE1x3x1)',
                    texname = '\\text{I74x313}')

I74x322 = Parameter(name = 'I74x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn2x2*complexconjugate(LLLE2x3x2)',
                    texname = '\\text{I74x322}')

I74x323 = Parameter(name = 'I74x323',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn3x1*complexconjugate(LLLE1x3x2)',
                    texname = '\\text{I74x323}')

I74x332 = Parameter(name = 'I74x332',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn2x2*complexconjugate(LLLE2x3x3)',
                    texname = '\\text{I74x332}')

I74x333 = Parameter(name = 'I74x333',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rn3x1*complexconjugate(LLLE1x3x3)',
                    texname = '\\text{I74x333}')

I75x111 = Parameter(name = 'I75x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn1x3*complexconjugate(Rd1x6)*complexconjugate(TLQD3x3x3)',
                    texname = '\\text{I75x111}')

I75x112 = Parameter(name = 'I75x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn1x3*complexconjugate(Rd1x6)*complexconjugate(TLQD3x3x3)',
                    texname = '\\text{I75x112}')

I75x115 = Parameter(name = 'I75x115',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn1x3*complexconjugate(Rd1x6)*complexconjugate(TLQD3x2x3)',
                    texname = '\\text{I75x115}')

I75x116 = Parameter(name = 'I75x116',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn1x3*complexconjugate(Rd1x6)*complexconjugate(TLQD3x1x3)',
                    texname = '\\text{I75x116}')

I75x121 = Parameter(name = 'I75x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn1x3*complexconjugate(Rd2x6)*complexconjugate(TLQD3x3x3)',
                    texname = '\\text{I75x121}')

I75x122 = Parameter(name = 'I75x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn1x3*complexconjugate(Rd2x6)*complexconjugate(TLQD3x3x3)',
                    texname = '\\text{I75x122}')

I75x125 = Parameter(name = 'I75x125',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn1x3*complexconjugate(Rd2x6)*complexconjugate(TLQD3x2x3)',
                    texname = '\\text{I75x125}')

I75x126 = Parameter(name = 'I75x126',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn1x3*complexconjugate(Rd2x6)*complexconjugate(TLQD3x1x3)',
                    texname = '\\text{I75x126}')

I75x131 = Parameter(name = 'I75x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn1x3*complexconjugate(Rd3x5)*complexconjugate(TLQD3x3x2)',
                    texname = '\\text{I75x131}')

I75x132 = Parameter(name = 'I75x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn1x3*complexconjugate(Rd3x5)*complexconjugate(TLQD3x3x2)',
                    texname = '\\text{I75x132}')

I75x135 = Parameter(name = 'I75x135',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn1x3*complexconjugate(Rd3x5)*complexconjugate(TLQD3x2x2)',
                    texname = '\\text{I75x135}')

I75x136 = Parameter(name = 'I75x136',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn1x3*complexconjugate(Rd3x5)*complexconjugate(TLQD3x1x2)',
                    texname = '\\text{I75x136}')

I75x141 = Parameter(name = 'I75x141',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn1x3*complexconjugate(Rd4x4)*complexconjugate(TLQD3x3x1)',
                    texname = '\\text{I75x141}')

I75x142 = Parameter(name = 'I75x142',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn1x3*complexconjugate(Rd4x4)*complexconjugate(TLQD3x3x1)',
                    texname = '\\text{I75x142}')

I75x145 = Parameter(name = 'I75x145',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn1x3*complexconjugate(Rd4x4)*complexconjugate(TLQD3x2x1)',
                    texname = '\\text{I75x145}')

I75x146 = Parameter(name = 'I75x146',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn1x3*complexconjugate(Rd4x4)*complexconjugate(TLQD3x1x1)',
                    texname = '\\text{I75x146}')

I75x211 = Parameter(name = 'I75x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn2x2*complexconjugate(Rd1x6)*complexconjugate(TLQD2x3x3)',
                    texname = '\\text{I75x211}')

I75x212 = Parameter(name = 'I75x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn2x2*complexconjugate(Rd1x6)*complexconjugate(TLQD2x3x3)',
                    texname = '\\text{I75x212}')

I75x215 = Parameter(name = 'I75x215',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn2x2*complexconjugate(Rd1x6)*complexconjugate(TLQD2x2x3)',
                    texname = '\\text{I75x215}')

I75x216 = Parameter(name = 'I75x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn2x2*complexconjugate(Rd1x6)*complexconjugate(TLQD2x1x3)',
                    texname = '\\text{I75x216}')

I75x221 = Parameter(name = 'I75x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn2x2*complexconjugate(Rd2x6)*complexconjugate(TLQD2x3x3)',
                    texname = '\\text{I75x221}')

I75x222 = Parameter(name = 'I75x222',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn2x2*complexconjugate(Rd2x6)*complexconjugate(TLQD2x3x3)',
                    texname = '\\text{I75x222}')

I75x225 = Parameter(name = 'I75x225',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn2x2*complexconjugate(Rd2x6)*complexconjugate(TLQD2x2x3)',
                    texname = '\\text{I75x225}')

I75x226 = Parameter(name = 'I75x226',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn2x2*complexconjugate(Rd2x6)*complexconjugate(TLQD2x1x3)',
                    texname = '\\text{I75x226}')

I75x231 = Parameter(name = 'I75x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn2x2*complexconjugate(Rd3x5)*complexconjugate(TLQD2x3x2)',
                    texname = '\\text{I75x231}')

I75x232 = Parameter(name = 'I75x232',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn2x2*complexconjugate(Rd3x5)*complexconjugate(TLQD2x3x2)',
                    texname = '\\text{I75x232}')

I75x235 = Parameter(name = 'I75x235',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn2x2*complexconjugate(Rd3x5)*complexconjugate(TLQD2x2x2)',
                    texname = '\\text{I75x235}')

I75x236 = Parameter(name = 'I75x236',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn2x2*complexconjugate(Rd3x5)*complexconjugate(TLQD2x1x2)',
                    texname = '\\text{I75x236}')

I75x241 = Parameter(name = 'I75x241',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn2x2*complexconjugate(Rd4x4)*complexconjugate(TLQD2x3x1)',
                    texname = '\\text{I75x241}')

I75x242 = Parameter(name = 'I75x242',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn2x2*complexconjugate(Rd4x4)*complexconjugate(TLQD2x3x1)',
                    texname = '\\text{I75x242}')

I75x245 = Parameter(name = 'I75x245',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn2x2*complexconjugate(Rd4x4)*complexconjugate(TLQD2x2x1)',
                    texname = '\\text{I75x245}')

I75x246 = Parameter(name = 'I75x246',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn2x2*complexconjugate(Rd4x4)*complexconjugate(TLQD2x1x1)',
                    texname = '\\text{I75x246}')

I75x311 = Parameter(name = 'I75x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn3x1*complexconjugate(Rd1x6)*complexconjugate(TLQD1x3x3)',
                    texname = '\\text{I75x311}')

I75x312 = Parameter(name = 'I75x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn3x1*complexconjugate(Rd1x6)*complexconjugate(TLQD1x3x3)',
                    texname = '\\text{I75x312}')

I75x315 = Parameter(name = 'I75x315',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn3x1*complexconjugate(Rd1x6)*complexconjugate(TLQD1x2x3)',
                    texname = '\\text{I75x315}')

I75x316 = Parameter(name = 'I75x316',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn3x1*complexconjugate(Rd1x6)*complexconjugate(TLQD1x1x3)',
                    texname = '\\text{I75x316}')

I75x321 = Parameter(name = 'I75x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn3x1*complexconjugate(Rd2x6)*complexconjugate(TLQD1x3x3)',
                    texname = '\\text{I75x321}')

I75x322 = Parameter(name = 'I75x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn3x1*complexconjugate(Rd2x6)*complexconjugate(TLQD1x3x3)',
                    texname = '\\text{I75x322}')

I75x325 = Parameter(name = 'I75x325',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn3x1*complexconjugate(Rd2x6)*complexconjugate(TLQD1x2x3)',
                    texname = '\\text{I75x325}')

I75x326 = Parameter(name = 'I75x326',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn3x1*complexconjugate(Rd2x6)*complexconjugate(TLQD1x1x3)',
                    texname = '\\text{I75x326}')

I75x331 = Parameter(name = 'I75x331',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn3x1*complexconjugate(Rd3x5)*complexconjugate(TLQD1x3x2)',
                    texname = '\\text{I75x331}')

I75x332 = Parameter(name = 'I75x332',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn3x1*complexconjugate(Rd3x5)*complexconjugate(TLQD1x3x2)',
                    texname = '\\text{I75x332}')

I75x335 = Parameter(name = 'I75x335',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn3x1*complexconjugate(Rd3x5)*complexconjugate(TLQD1x2x2)',
                    texname = '\\text{I75x335}')

I75x336 = Parameter(name = 'I75x336',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn3x1*complexconjugate(Rd3x5)*complexconjugate(TLQD1x1x2)',
                    texname = '\\text{I75x336}')

I75x341 = Parameter(name = 'I75x341',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn3x1*complexconjugate(Rd4x4)*complexconjugate(TLQD1x3x1)',
                    texname = '\\text{I75x341}')

I75x342 = Parameter(name = 'I75x342',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn3x1*complexconjugate(Rd4x4)*complexconjugate(TLQD1x3x1)',
                    texname = '\\text{I75x342}')

I75x345 = Parameter(name = 'I75x345',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn3x1*complexconjugate(Rd4x4)*complexconjugate(TLQD1x2x1)',
                    texname = '\\text{I75x345}')

I75x346 = Parameter(name = 'I75x346',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn3x1*complexconjugate(Rd4x4)*complexconjugate(TLQD1x1x1)',
                    texname = '\\text{I75x346}')

I76x111 = Parameter(name = 'I76x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn1x3*yd3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd1x3)',
                    texname = '\\text{I76x111}')

I76x112 = Parameter(name = 'I76x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn1x3*yd3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd1x3)',
                    texname = '\\text{I76x112}')

I76x115 = Parameter(name = 'I76x115',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn1x3*yd3x3*complexconjugate(LLQD3x2x3)*complexconjugate(Rd1x3)',
                    texname = '\\text{I76x115}')

I76x116 = Parameter(name = 'I76x116',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn1x3*yd3x3*complexconjugate(LLQD3x1x3)*complexconjugate(Rd1x3)',
                    texname = '\\text{I76x116}')

I76x121 = Parameter(name = 'I76x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn1x3*yd3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd2x3)',
                    texname = '\\text{I76x121}')

I76x122 = Parameter(name = 'I76x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn1x3*yd3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd2x3)',
                    texname = '\\text{I76x122}')

I76x125 = Parameter(name = 'I76x125',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn1x3*yd3x3*complexconjugate(LLQD3x2x3)*complexconjugate(Rd2x3)',
                    texname = '\\text{I76x125}')

I76x126 = Parameter(name = 'I76x126',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn1x3*yd3x3*complexconjugate(LLQD3x1x3)*complexconjugate(Rd2x3)',
                    texname = '\\text{I76x126}')

I76x151 = Parameter(name = 'I76x151',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn1x3*yd2x2*complexconjugate(LLQD3x3x2)*complexconjugate(Rd5x2)',
                    texname = '\\text{I76x151}')

I76x152 = Parameter(name = 'I76x152',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn1x3*yd2x2*complexconjugate(LLQD3x3x2)*complexconjugate(Rd5x2)',
                    texname = '\\text{I76x152}')

I76x155 = Parameter(name = 'I76x155',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn1x3*yd2x2*complexconjugate(LLQD3x2x2)*complexconjugate(Rd5x2)',
                    texname = '\\text{I76x155}')

I76x156 = Parameter(name = 'I76x156',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn1x3*yd2x2*complexconjugate(LLQD3x1x2)*complexconjugate(Rd5x2)',
                    texname = '\\text{I76x156}')

I76x161 = Parameter(name = 'I76x161',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn1x3*yd1x1*complexconjugate(LLQD3x3x1)*complexconjugate(Rd6x1)',
                    texname = '\\text{I76x161}')

I76x162 = Parameter(name = 'I76x162',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn1x3*yd1x1*complexconjugate(LLQD3x3x1)*complexconjugate(Rd6x1)',
                    texname = '\\text{I76x162}')

I76x165 = Parameter(name = 'I76x165',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn1x3*yd1x1*complexconjugate(LLQD3x2x1)*complexconjugate(Rd6x1)',
                    texname = '\\text{I76x165}')

I76x166 = Parameter(name = 'I76x166',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn1x3*yd1x1*complexconjugate(LLQD3x1x1)*complexconjugate(Rd6x1)',
                    texname = '\\text{I76x166}')

I76x211 = Parameter(name = 'I76x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn2x2*yd3x3*complexconjugate(LLQD2x3x3)*complexconjugate(Rd1x3)',
                    texname = '\\text{I76x211}')

I76x212 = Parameter(name = 'I76x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn2x2*yd3x3*complexconjugate(LLQD2x3x3)*complexconjugate(Rd1x3)',
                    texname = '\\text{I76x212}')

I76x215 = Parameter(name = 'I76x215',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn2x2*yd3x3*complexconjugate(LLQD2x2x3)*complexconjugate(Rd1x3)',
                    texname = '\\text{I76x215}')

I76x216 = Parameter(name = 'I76x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn2x2*yd3x3*complexconjugate(LLQD2x1x3)*complexconjugate(Rd1x3)',
                    texname = '\\text{I76x216}')

I76x221 = Parameter(name = 'I76x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn2x2*yd3x3*complexconjugate(LLQD2x3x3)*complexconjugate(Rd2x3)',
                    texname = '\\text{I76x221}')

I76x222 = Parameter(name = 'I76x222',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn2x2*yd3x3*complexconjugate(LLQD2x3x3)*complexconjugate(Rd2x3)',
                    texname = '\\text{I76x222}')

I76x225 = Parameter(name = 'I76x225',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn2x2*yd3x3*complexconjugate(LLQD2x2x3)*complexconjugate(Rd2x3)',
                    texname = '\\text{I76x225}')

I76x226 = Parameter(name = 'I76x226',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn2x2*yd3x3*complexconjugate(LLQD2x1x3)*complexconjugate(Rd2x3)',
                    texname = '\\text{I76x226}')

I76x251 = Parameter(name = 'I76x251',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn2x2*yd2x2*complexconjugate(LLQD2x3x2)*complexconjugate(Rd5x2)',
                    texname = '\\text{I76x251}')

I76x252 = Parameter(name = 'I76x252',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn2x2*yd2x2*complexconjugate(LLQD2x3x2)*complexconjugate(Rd5x2)',
                    texname = '\\text{I76x252}')

I76x255 = Parameter(name = 'I76x255',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn2x2*yd2x2*complexconjugate(LLQD2x2x2)*complexconjugate(Rd5x2)',
                    texname = '\\text{I76x255}')

I76x256 = Parameter(name = 'I76x256',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn2x2*yd2x2*complexconjugate(LLQD2x1x2)*complexconjugate(Rd5x2)',
                    texname = '\\text{I76x256}')

I76x261 = Parameter(name = 'I76x261',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn2x2*yd1x1*complexconjugate(LLQD2x3x1)*complexconjugate(Rd6x1)',
                    texname = '\\text{I76x261}')

I76x262 = Parameter(name = 'I76x262',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn2x2*yd1x1*complexconjugate(LLQD2x3x1)*complexconjugate(Rd6x1)',
                    texname = '\\text{I76x262}')

I76x265 = Parameter(name = 'I76x265',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn2x2*yd1x1*complexconjugate(LLQD2x2x1)*complexconjugate(Rd6x1)',
                    texname = '\\text{I76x265}')

I76x266 = Parameter(name = 'I76x266',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn2x2*yd1x1*complexconjugate(LLQD2x1x1)*complexconjugate(Rd6x1)',
                    texname = '\\text{I76x266}')

I76x311 = Parameter(name = 'I76x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn3x1*yd3x3*complexconjugate(LLQD1x3x3)*complexconjugate(Rd1x3)',
                    texname = '\\text{I76x311}')

I76x312 = Parameter(name = 'I76x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn3x1*yd3x3*complexconjugate(LLQD1x3x3)*complexconjugate(Rd1x3)',
                    texname = '\\text{I76x312}')

I76x315 = Parameter(name = 'I76x315',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn3x1*yd3x3*complexconjugate(LLQD1x2x3)*complexconjugate(Rd1x3)',
                    texname = '\\text{I76x315}')

I76x316 = Parameter(name = 'I76x316',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn3x1*yd3x3*complexconjugate(LLQD1x1x3)*complexconjugate(Rd1x3)',
                    texname = '\\text{I76x316}')

I76x321 = Parameter(name = 'I76x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn3x1*yd3x3*complexconjugate(LLQD1x3x3)*complexconjugate(Rd2x3)',
                    texname = '\\text{I76x321}')

I76x322 = Parameter(name = 'I76x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn3x1*yd3x3*complexconjugate(LLQD1x3x3)*complexconjugate(Rd2x3)',
                    texname = '\\text{I76x322}')

I76x325 = Parameter(name = 'I76x325',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn3x1*yd3x3*complexconjugate(LLQD1x2x3)*complexconjugate(Rd2x3)',
                    texname = '\\text{I76x325}')

I76x326 = Parameter(name = 'I76x326',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn3x1*yd3x3*complexconjugate(LLQD1x1x3)*complexconjugate(Rd2x3)',
                    texname = '\\text{I76x326}')

I76x351 = Parameter(name = 'I76x351',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn3x1*yd2x2*complexconjugate(LLQD1x3x2)*complexconjugate(Rd5x2)',
                    texname = '\\text{I76x351}')

I76x352 = Parameter(name = 'I76x352',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn3x1*yd2x2*complexconjugate(LLQD1x3x2)*complexconjugate(Rd5x2)',
                    texname = '\\text{I76x352}')

I76x355 = Parameter(name = 'I76x355',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn3x1*yd2x2*complexconjugate(LLQD1x2x2)*complexconjugate(Rd5x2)',
                    texname = '\\text{I76x355}')

I76x356 = Parameter(name = 'I76x356',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn3x1*yd2x2*complexconjugate(LLQD1x1x2)*complexconjugate(Rd5x2)',
                    texname = '\\text{I76x356}')

I76x361 = Parameter(name = 'I76x361',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x3*Rn3x1*yd1x1*complexconjugate(LLQD1x3x1)*complexconjugate(Rd6x1)',
                    texname = '\\text{I76x361}')

I76x362 = Parameter(name = 'I76x362',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x3*Rn3x1*yd1x1*complexconjugate(LLQD1x3x1)*complexconjugate(Rd6x1)',
                    texname = '\\text{I76x362}')

I76x365 = Parameter(name = 'I76x365',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd5x2*Rn3x1*yd1x1*complexconjugate(LLQD1x2x1)*complexconjugate(Rd6x1)',
                    texname = '\\text{I76x365}')

I76x366 = Parameter(name = 'I76x366',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd6x1*Rn3x1*yd1x1*complexconjugate(LLQD1x1x1)*complexconjugate(Rd6x1)',
                    texname = '\\text{I76x366}')

I77x111 = Parameter(name = 'I77x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*Rn1x3*yd3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd1x6)',
                    texname = '\\text{I77x111}')

I77x112 = Parameter(name = 'I77x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*Rn1x3*yd3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd1x6)',
                    texname = '\\text{I77x112}')

I77x113 = Parameter(name = 'I77x113',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*Rn1x3*yd2x2*complexconjugate(LLQD3x2x3)*complexconjugate(Rd1x6)',
                    texname = '\\text{I77x113}')

I77x114 = Parameter(name = 'I77x114',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*Rn1x3*yd1x1*complexconjugate(LLQD3x1x3)*complexconjugate(Rd1x6)',
                    texname = '\\text{I77x114}')

I77x121 = Parameter(name = 'I77x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*Rn1x3*yd3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd2x6)',
                    texname = '\\text{I77x121}')

I77x122 = Parameter(name = 'I77x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*Rn1x3*yd3x3*complexconjugate(LLQD3x3x3)*complexconjugate(Rd2x6)',
                    texname = '\\text{I77x122}')

I77x123 = Parameter(name = 'I77x123',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*Rn1x3*yd2x2*complexconjugate(LLQD3x2x3)*complexconjugate(Rd2x6)',
                    texname = '\\text{I77x123}')

I77x124 = Parameter(name = 'I77x124',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*Rn1x3*yd1x1*complexconjugate(LLQD3x1x3)*complexconjugate(Rd2x6)',
                    texname = '\\text{I77x124}')

I77x131 = Parameter(name = 'I77x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*Rn1x3*yd3x3*complexconjugate(LLQD3x3x2)*complexconjugate(Rd3x5)',
                    texname = '\\text{I77x131}')

I77x132 = Parameter(name = 'I77x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*Rn1x3*yd3x3*complexconjugate(LLQD3x3x2)*complexconjugate(Rd3x5)',
                    texname = '\\text{I77x132}')

I77x133 = Parameter(name = 'I77x133',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*Rn1x3*yd2x2*complexconjugate(LLQD3x2x2)*complexconjugate(Rd3x5)',
                    texname = '\\text{I77x133}')

I77x134 = Parameter(name = 'I77x134',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*Rn1x3*yd1x1*complexconjugate(LLQD3x1x2)*complexconjugate(Rd3x5)',
                    texname = '\\text{I77x134}')

I77x141 = Parameter(name = 'I77x141',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*Rn1x3*yd3x3*complexconjugate(LLQD3x3x1)*complexconjugate(Rd4x4)',
                    texname = '\\text{I77x141}')

I77x142 = Parameter(name = 'I77x142',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*Rn1x3*yd3x3*complexconjugate(LLQD3x3x1)*complexconjugate(Rd4x4)',
                    texname = '\\text{I77x142}')

I77x143 = Parameter(name = 'I77x143',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*Rn1x3*yd2x2*complexconjugate(LLQD3x2x1)*complexconjugate(Rd4x4)',
                    texname = '\\text{I77x143}')

I77x144 = Parameter(name = 'I77x144',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*Rn1x3*yd1x1*complexconjugate(LLQD3x1x1)*complexconjugate(Rd4x4)',
                    texname = '\\text{I77x144}')

I77x211 = Parameter(name = 'I77x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*Rn2x2*yd3x3*complexconjugate(LLQD2x3x3)*complexconjugate(Rd1x6)',
                    texname = '\\text{I77x211}')

I77x212 = Parameter(name = 'I77x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*Rn2x2*yd3x3*complexconjugate(LLQD2x3x3)*complexconjugate(Rd1x6)',
                    texname = '\\text{I77x212}')

I77x213 = Parameter(name = 'I77x213',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*Rn2x2*yd2x2*complexconjugate(LLQD2x2x3)*complexconjugate(Rd1x6)',
                    texname = '\\text{I77x213}')

I77x214 = Parameter(name = 'I77x214',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*Rn2x2*yd1x1*complexconjugate(LLQD2x1x3)*complexconjugate(Rd1x6)',
                    texname = '\\text{I77x214}')

I77x221 = Parameter(name = 'I77x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*Rn2x2*yd3x3*complexconjugate(LLQD2x3x3)*complexconjugate(Rd2x6)',
                    texname = '\\text{I77x221}')

I77x222 = Parameter(name = 'I77x222',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*Rn2x2*yd3x3*complexconjugate(LLQD2x3x3)*complexconjugate(Rd2x6)',
                    texname = '\\text{I77x222}')

I77x223 = Parameter(name = 'I77x223',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*Rn2x2*yd2x2*complexconjugate(LLQD2x2x3)*complexconjugate(Rd2x6)',
                    texname = '\\text{I77x223}')

I77x224 = Parameter(name = 'I77x224',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*Rn2x2*yd1x1*complexconjugate(LLQD2x1x3)*complexconjugate(Rd2x6)',
                    texname = '\\text{I77x224}')

I77x231 = Parameter(name = 'I77x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*Rn2x2*yd3x3*complexconjugate(LLQD2x3x2)*complexconjugate(Rd3x5)',
                    texname = '\\text{I77x231}')

I77x232 = Parameter(name = 'I77x232',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*Rn2x2*yd3x3*complexconjugate(LLQD2x3x2)*complexconjugate(Rd3x5)',
                    texname = '\\text{I77x232}')

I77x233 = Parameter(name = 'I77x233',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*Rn2x2*yd2x2*complexconjugate(LLQD2x2x2)*complexconjugate(Rd3x5)',
                    texname = '\\text{I77x233}')

I77x234 = Parameter(name = 'I77x234',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*Rn2x2*yd1x1*complexconjugate(LLQD2x1x2)*complexconjugate(Rd3x5)',
                    texname = '\\text{I77x234}')

I77x241 = Parameter(name = 'I77x241',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*Rn2x2*yd3x3*complexconjugate(LLQD2x3x1)*complexconjugate(Rd4x4)',
                    texname = '\\text{I77x241}')

I77x242 = Parameter(name = 'I77x242',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*Rn2x2*yd3x3*complexconjugate(LLQD2x3x1)*complexconjugate(Rd4x4)',
                    texname = '\\text{I77x242}')

I77x243 = Parameter(name = 'I77x243',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*Rn2x2*yd2x2*complexconjugate(LLQD2x2x1)*complexconjugate(Rd4x4)',
                    texname = '\\text{I77x243}')

I77x244 = Parameter(name = 'I77x244',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*Rn2x2*yd1x1*complexconjugate(LLQD2x1x1)*complexconjugate(Rd4x4)',
                    texname = '\\text{I77x244}')

I77x311 = Parameter(name = 'I77x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*Rn3x1*yd3x3*complexconjugate(LLQD1x3x3)*complexconjugate(Rd1x6)',
                    texname = '\\text{I77x311}')

I77x312 = Parameter(name = 'I77x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*Rn3x1*yd3x3*complexconjugate(LLQD1x3x3)*complexconjugate(Rd1x6)',
                    texname = '\\text{I77x312}')

I77x313 = Parameter(name = 'I77x313',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*Rn3x1*yd2x2*complexconjugate(LLQD1x2x3)*complexconjugate(Rd1x6)',
                    texname = '\\text{I77x313}')

I77x314 = Parameter(name = 'I77x314',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*Rn3x1*yd1x1*complexconjugate(LLQD1x1x3)*complexconjugate(Rd1x6)',
                    texname = '\\text{I77x314}')

I77x321 = Parameter(name = 'I77x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*Rn3x1*yd3x3*complexconjugate(LLQD1x3x3)*complexconjugate(Rd2x6)',
                    texname = '\\text{I77x321}')

I77x322 = Parameter(name = 'I77x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*Rn3x1*yd3x3*complexconjugate(LLQD1x3x3)*complexconjugate(Rd2x6)',
                    texname = '\\text{I77x322}')

I77x323 = Parameter(name = 'I77x323',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*Rn3x1*yd2x2*complexconjugate(LLQD1x2x3)*complexconjugate(Rd2x6)',
                    texname = '\\text{I77x323}')

I77x324 = Parameter(name = 'I77x324',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*Rn3x1*yd1x1*complexconjugate(LLQD1x1x3)*complexconjugate(Rd2x6)',
                    texname = '\\text{I77x324}')

I77x331 = Parameter(name = 'I77x331',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*Rn3x1*yd3x3*complexconjugate(LLQD1x3x2)*complexconjugate(Rd3x5)',
                    texname = '\\text{I77x331}')

I77x332 = Parameter(name = 'I77x332',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*Rn3x1*yd3x3*complexconjugate(LLQD1x3x2)*complexconjugate(Rd3x5)',
                    texname = '\\text{I77x332}')

I77x333 = Parameter(name = 'I77x333',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*Rn3x1*yd2x2*complexconjugate(LLQD1x2x2)*complexconjugate(Rd3x5)',
                    texname = '\\text{I77x333}')

I77x334 = Parameter(name = 'I77x334',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*Rn3x1*yd1x1*complexconjugate(LLQD1x1x2)*complexconjugate(Rd3x5)',
                    texname = '\\text{I77x334}')

I77x341 = Parameter(name = 'I77x341',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*Rn3x1*yd3x3*complexconjugate(LLQD1x3x1)*complexconjugate(Rd4x4)',
                    texname = '\\text{I77x341}')

I77x342 = Parameter(name = 'I77x342',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*Rn3x1*yd3x3*complexconjugate(LLQD1x3x1)*complexconjugate(Rd4x4)',
                    texname = '\\text{I77x342}')

I77x343 = Parameter(name = 'I77x343',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*Rn3x1*yd2x2*complexconjugate(LLQD1x2x1)*complexconjugate(Rd4x4)',
                    texname = '\\text{I77x343}')

I77x344 = Parameter(name = 'I77x344',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*Rn3x1*yd1x1*complexconjugate(LLQD1x1x1)*complexconjugate(Rd4x4)',
                    texname = '\\text{I77x344}')

I78x114 = Parameter(name = 'I78x114',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*Rn1x3*complexconjugate(Rl1x6)*complexconjugate(TLLE3x1x3)',
                    texname = '\\text{I78x114}')

I78x115 = Parameter(name = 'I78x115',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*Rn1x3*complexconjugate(Rl1x6)*complexconjugate(TLLE3x2x3)',
                    texname = '\\text{I78x115}')

I78x124 = Parameter(name = 'I78x124',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*Rn1x3*complexconjugate(Rl2x5)*complexconjugate(TLLE3x1x2)',
                    texname = '\\text{I78x124}')

I78x125 = Parameter(name = 'I78x125',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*Rn1x3*complexconjugate(Rl2x5)*complexconjugate(TLLE3x2x2)',
                    texname = '\\text{I78x125}')

I78x134 = Parameter(name = 'I78x134',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*Rn1x3*complexconjugate(Rl3x4)*complexconjugate(TLLE3x1x1)',
                    texname = '\\text{I78x134}')

I78x135 = Parameter(name = 'I78x135',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*Rn1x3*complexconjugate(Rl3x4)*complexconjugate(TLLE3x2x1)',
                    texname = '\\text{I78x135}')

I78x164 = Parameter(name = 'I78x164',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*Rn1x3*complexconjugate(Rl6x6)*complexconjugate(TLLE3x1x3)',
                    texname = '\\text{I78x164}')

I78x165 = Parameter(name = 'I78x165',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*Rn1x3*complexconjugate(Rl6x6)*complexconjugate(TLLE3x2x3)',
                    texname = '\\text{I78x165}')

I78x211 = Parameter(name = 'I78x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*Rn2x2*complexconjugate(Rl1x6)*complexconjugate(TLLE2x3x3)',
                    texname = '\\text{I78x211}')

I78x214 = Parameter(name = 'I78x214',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*Rn2x2*complexconjugate(Rl1x6)*complexconjugate(TLLE2x1x3)',
                    texname = '\\text{I78x214}')

I78x216 = Parameter(name = 'I78x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*Rn2x2*complexconjugate(Rl1x6)*complexconjugate(TLLE2x3x3)',
                    texname = '\\text{I78x216}')

I78x221 = Parameter(name = 'I78x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*Rn2x2*complexconjugate(Rl2x5)*complexconjugate(TLLE2x3x2)',
                    texname = '\\text{I78x221}')

I78x224 = Parameter(name = 'I78x224',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*Rn2x2*complexconjugate(Rl2x5)*complexconjugate(TLLE2x1x2)',
                    texname = '\\text{I78x224}')

I78x226 = Parameter(name = 'I78x226',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*Rn2x2*complexconjugate(Rl2x5)*complexconjugate(TLLE2x3x2)',
                    texname = '\\text{I78x226}')

I78x231 = Parameter(name = 'I78x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*Rn2x2*complexconjugate(Rl3x4)*complexconjugate(TLLE2x3x1)',
                    texname = '\\text{I78x231}')

I78x234 = Parameter(name = 'I78x234',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*Rn2x2*complexconjugate(Rl3x4)*complexconjugate(TLLE2x1x1)',
                    texname = '\\text{I78x234}')

I78x236 = Parameter(name = 'I78x236',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*Rn2x2*complexconjugate(Rl3x4)*complexconjugate(TLLE2x3x1)',
                    texname = '\\text{I78x236}')

I78x261 = Parameter(name = 'I78x261',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*Rn2x2*complexconjugate(Rl6x6)*complexconjugate(TLLE2x3x3)',
                    texname = '\\text{I78x261}')

I78x264 = Parameter(name = 'I78x264',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*Rn2x2*complexconjugate(Rl6x6)*complexconjugate(TLLE2x1x3)',
                    texname = '\\text{I78x264}')

I78x266 = Parameter(name = 'I78x266',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*Rn2x2*complexconjugate(Rl6x6)*complexconjugate(TLLE2x3x3)',
                    texname = '\\text{I78x266}')

I78x311 = Parameter(name = 'I78x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*Rn3x1*complexconjugate(Rl1x6)*complexconjugate(TLLE1x3x3)',
                    texname = '\\text{I78x311}')

I78x315 = Parameter(name = 'I78x315',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*Rn3x1*complexconjugate(Rl1x6)*complexconjugate(TLLE1x2x3)',
                    texname = '\\text{I78x315}')

I78x316 = Parameter(name = 'I78x316',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*Rn3x1*complexconjugate(Rl1x6)*complexconjugate(TLLE1x3x3)',
                    texname = '\\text{I78x316}')

I78x321 = Parameter(name = 'I78x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*Rn3x1*complexconjugate(Rl2x5)*complexconjugate(TLLE1x3x2)',
                    texname = '\\text{I78x321}')

I78x325 = Parameter(name = 'I78x325',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*Rn3x1*complexconjugate(Rl2x5)*complexconjugate(TLLE1x2x2)',
                    texname = '\\text{I78x325}')

I78x326 = Parameter(name = 'I78x326',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*Rn3x1*complexconjugate(Rl2x5)*complexconjugate(TLLE1x3x2)',
                    texname = '\\text{I78x326}')

I78x331 = Parameter(name = 'I78x331',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*Rn3x1*complexconjugate(Rl3x4)*complexconjugate(TLLE1x3x1)',
                    texname = '\\text{I78x331}')

I78x335 = Parameter(name = 'I78x335',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*Rn3x1*complexconjugate(Rl3x4)*complexconjugate(TLLE1x2x1)',
                    texname = '\\text{I78x335}')

I78x336 = Parameter(name = 'I78x336',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*Rn3x1*complexconjugate(Rl3x4)*complexconjugate(TLLE1x3x1)',
                    texname = '\\text{I78x336}')

I78x361 = Parameter(name = 'I78x361',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*Rn3x1*complexconjugate(Rl6x6)*complexconjugate(TLLE1x3x3)',
                    texname = '\\text{I78x361}')

I78x365 = Parameter(name = 'I78x365',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*Rn3x1*complexconjugate(Rl6x6)*complexconjugate(TLLE1x2x3)',
                    texname = '\\text{I78x365}')

I78x366 = Parameter(name = 'I78x366',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*Rn3x1*complexconjugate(Rl6x6)*complexconjugate(TLLE1x3x3)',
                    texname = '\\text{I78x366}')

I79x114 = Parameter(name = 'I79x114',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*Rn1x3*ye3x3*complexconjugate(LLLE3x1x3)*complexconjugate(Rl1x3)',
                    texname = '\\text{I79x114}')

I79x115 = Parameter(name = 'I79x115',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*Rn1x3*ye3x3*complexconjugate(LLLE3x2x3)*complexconjugate(Rl1x3)',
                    texname = '\\text{I79x115}')

I79x144 = Parameter(name = 'I79x144',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*Rn1x3*ye1x1*complexconjugate(LLLE3x1x1)*complexconjugate(Rl4x1)',
                    texname = '\\text{I79x144}')

I79x145 = Parameter(name = 'I79x145',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*Rn1x3*ye1x1*complexconjugate(LLLE3x2x1)*complexconjugate(Rl4x1)',
                    texname = '\\text{I79x145}')

I79x154 = Parameter(name = 'I79x154',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*Rn1x3*ye2x2*complexconjugate(LLLE3x1x2)*complexconjugate(Rl5x2)',
                    texname = '\\text{I79x154}')

I79x155 = Parameter(name = 'I79x155',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*Rn1x3*ye2x2*complexconjugate(LLLE3x2x2)*complexconjugate(Rl5x2)',
                    texname = '\\text{I79x155}')

I79x164 = Parameter(name = 'I79x164',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*Rn1x3*ye3x3*complexconjugate(LLLE3x1x3)*complexconjugate(Rl6x3)',
                    texname = '\\text{I79x164}')

I79x165 = Parameter(name = 'I79x165',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*Rn1x3*ye3x3*complexconjugate(LLLE3x2x3)*complexconjugate(Rl6x3)',
                    texname = '\\text{I79x165}')

I79x211 = Parameter(name = 'I79x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*Rn2x2*ye3x3*complexconjugate(LLLE2x3x3)*complexconjugate(Rl1x3)',
                    texname = '\\text{I79x211}')

I79x214 = Parameter(name = 'I79x214',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*Rn2x2*ye3x3*complexconjugate(LLLE2x1x3)*complexconjugate(Rl1x3)',
                    texname = '\\text{I79x214}')

I79x216 = Parameter(name = 'I79x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*Rn2x2*ye3x3*complexconjugate(LLLE2x3x3)*complexconjugate(Rl1x3)',
                    texname = '\\text{I79x216}')

I79x241 = Parameter(name = 'I79x241',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*Rn2x2*ye1x1*complexconjugate(LLLE2x3x1)*complexconjugate(Rl4x1)',
                    texname = '\\text{I79x241}')

I79x244 = Parameter(name = 'I79x244',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*Rn2x2*ye1x1*complexconjugate(LLLE2x1x1)*complexconjugate(Rl4x1)',
                    texname = '\\text{I79x244}')

I79x246 = Parameter(name = 'I79x246',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*Rn2x2*ye1x1*complexconjugate(LLLE2x3x1)*complexconjugate(Rl4x1)',
                    texname = '\\text{I79x246}')

I79x251 = Parameter(name = 'I79x251',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*Rn2x2*ye2x2*complexconjugate(LLLE2x3x2)*complexconjugate(Rl5x2)',
                    texname = '\\text{I79x251}')

I79x254 = Parameter(name = 'I79x254',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*Rn2x2*ye2x2*complexconjugate(LLLE2x1x2)*complexconjugate(Rl5x2)',
                    texname = '\\text{I79x254}')

I79x256 = Parameter(name = 'I79x256',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*Rn2x2*ye2x2*complexconjugate(LLLE2x3x2)*complexconjugate(Rl5x2)',
                    texname = '\\text{I79x256}')

I79x261 = Parameter(name = 'I79x261',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*Rn2x2*ye3x3*complexconjugate(LLLE2x3x3)*complexconjugate(Rl6x3)',
                    texname = '\\text{I79x261}')

I79x264 = Parameter(name = 'I79x264',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl4x1*Rn2x2*ye3x3*complexconjugate(LLLE2x1x3)*complexconjugate(Rl6x3)',
                    texname = '\\text{I79x264}')

I79x266 = Parameter(name = 'I79x266',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*Rn2x2*ye3x3*complexconjugate(LLLE2x3x3)*complexconjugate(Rl6x3)',
                    texname = '\\text{I79x266}')

I79x311 = Parameter(name = 'I79x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*Rn3x1*ye3x3*complexconjugate(LLLE1x3x3)*complexconjugate(Rl1x3)',
                    texname = '\\text{I79x311}')

I79x315 = Parameter(name = 'I79x315',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*Rn3x1*ye3x3*complexconjugate(LLLE1x2x3)*complexconjugate(Rl1x3)',
                    texname = '\\text{I79x315}')

I79x316 = Parameter(name = 'I79x316',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*Rn3x1*ye3x3*complexconjugate(LLLE1x3x3)*complexconjugate(Rl1x3)',
                    texname = '\\text{I79x316}')

I79x341 = Parameter(name = 'I79x341',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*Rn3x1*ye1x1*complexconjugate(LLLE1x3x1)*complexconjugate(Rl4x1)',
                    texname = '\\text{I79x341}')

I79x345 = Parameter(name = 'I79x345',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*Rn3x1*ye1x1*complexconjugate(LLLE1x2x1)*complexconjugate(Rl4x1)',
                    texname = '\\text{I79x345}')

I79x346 = Parameter(name = 'I79x346',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*Rn3x1*ye1x1*complexconjugate(LLLE1x3x1)*complexconjugate(Rl4x1)',
                    texname = '\\text{I79x346}')

I79x351 = Parameter(name = 'I79x351',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*Rn3x1*ye2x2*complexconjugate(LLLE1x3x2)*complexconjugate(Rl5x2)',
                    texname = '\\text{I79x351}')

I79x355 = Parameter(name = 'I79x355',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*Rn3x1*ye2x2*complexconjugate(LLLE1x2x2)*complexconjugate(Rl5x2)',
                    texname = '\\text{I79x355}')

I79x356 = Parameter(name = 'I79x356',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*Rn3x1*ye2x2*complexconjugate(LLLE1x3x2)*complexconjugate(Rl5x2)',
                    texname = '\\text{I79x356}')

I79x361 = Parameter(name = 'I79x361',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x3*Rn3x1*ye3x3*complexconjugate(LLLE1x3x3)*complexconjugate(Rl6x3)',
                    texname = '\\text{I79x361}')

I79x365 = Parameter(name = 'I79x365',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl5x2*Rn3x1*ye3x3*complexconjugate(LLLE1x2x3)*complexconjugate(Rl6x3)',
                    texname = '\\text{I79x365}')

I79x366 = Parameter(name = 'I79x366',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x3*Rn3x1*ye3x3*complexconjugate(LLLE1x3x3)*complexconjugate(Rl6x3)',
                    texname = '\\text{I79x366}')

I8x111 = Parameter(name = 'I8x111',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x1x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I8x111}')

I8x112 = Parameter(name = 'I8x112',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x1x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I8x112}')

I8x113 = Parameter(name = 'I8x113',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x1x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I8x113}')

I8x114 = Parameter(name = 'I8x114',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x1x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I8x114}')

I8x121 = Parameter(name = 'I8x121',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x1x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I8x121}')

I8x122 = Parameter(name = 'I8x122',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x1x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I8x122}')

I8x123 = Parameter(name = 'I8x123',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x1x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I8x123}')

I8x124 = Parameter(name = 'I8x124',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x1x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I8x124}')

I8x131 = Parameter(name = 'I8x131',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x1x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I8x131}')

I8x132 = Parameter(name = 'I8x132',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x1x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I8x132}')

I8x133 = Parameter(name = 'I8x133',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x1x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I8x133}')

I8x134 = Parameter(name = 'I8x134',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x1x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I8x134}')

I8x211 = Parameter(name = 'I8x211',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x2x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I8x211}')

I8x212 = Parameter(name = 'I8x212',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x2x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I8x212}')

I8x213 = Parameter(name = 'I8x213',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x2x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I8x213}')

I8x214 = Parameter(name = 'I8x214',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x2x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I8x214}')

I8x221 = Parameter(name = 'I8x221',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x2x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I8x221}')

I8x222 = Parameter(name = 'I8x222',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x2x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I8x222}')

I8x223 = Parameter(name = 'I8x223',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x2x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I8x223}')

I8x224 = Parameter(name = 'I8x224',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x2x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I8x224}')

I8x231 = Parameter(name = 'I8x231',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x2x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I8x231}')

I8x232 = Parameter(name = 'I8x232',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x2x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I8x232}')

I8x233 = Parameter(name = 'I8x233',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x2x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I8x233}')

I8x234 = Parameter(name = 'I8x234',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x2x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I8x234}')

I8x311 = Parameter(name = 'I8x311',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x3x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I8x311}')

I8x312 = Parameter(name = 'I8x312',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x3x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I8x312}')

I8x313 = Parameter(name = 'I8x313',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x3x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I8x313}')

I8x314 = Parameter(name = 'I8x314',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x3x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I8x314}')

I8x321 = Parameter(name = 'I8x321',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x3x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I8x321}')

I8x322 = Parameter(name = 'I8x322',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x3x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I8x322}')

I8x323 = Parameter(name = 'I8x323',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x3x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I8x323}')

I8x324 = Parameter(name = 'I8x324',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x3x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I8x324}')

I8x331 = Parameter(name = 'I8x331',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x3x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I8x331}')

I8x332 = Parameter(name = 'I8x332',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x3x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I8x332}')

I8x333 = Parameter(name = 'I8x333',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x3x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I8x333}')

I8x334 = Parameter(name = 'I8x334',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x3x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I8x334}')

I80x112 = Parameter(name = 'I80x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl2x5*Rn1x3*ye2x2*complexconjugate(LLLE3x2x3)*complexconjugate(Rl1x6)',
                    texname = '\\text{I80x112}')

I80x113 = Parameter(name = 'I80x113',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl3x4*Rn1x3*ye1x1*complexconjugate(LLLE3x1x3)*complexconjugate(Rl1x6)',
                    texname = '\\text{I80x113}')

I80x122 = Parameter(name = 'I80x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl2x5*Rn1x3*ye2x2*complexconjugate(LLLE3x2x2)*complexconjugate(Rl2x5)',
                    texname = '\\text{I80x122}')

I80x123 = Parameter(name = 'I80x123',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl3x4*Rn1x3*ye1x1*complexconjugate(LLLE3x1x2)*complexconjugate(Rl2x5)',
                    texname = '\\text{I80x123}')

I80x132 = Parameter(name = 'I80x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl2x5*Rn1x3*ye2x2*complexconjugate(LLLE3x2x1)*complexconjugate(Rl3x4)',
                    texname = '\\text{I80x132}')

I80x133 = Parameter(name = 'I80x133',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl3x4*Rn1x3*ye1x1*complexconjugate(LLLE3x1x1)*complexconjugate(Rl3x4)',
                    texname = '\\text{I80x133}')

I80x162 = Parameter(name = 'I80x162',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl2x5*Rn1x3*ye2x2*complexconjugate(LLLE3x2x3)*complexconjugate(Rl6x6)',
                    texname = '\\text{I80x162}')

I80x163 = Parameter(name = 'I80x163',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl3x4*Rn1x3*ye1x1*complexconjugate(LLLE3x1x3)*complexconjugate(Rl6x6)',
                    texname = '\\text{I80x163}')

I80x211 = Parameter(name = 'I80x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x6*Rn2x2*ye3x3*complexconjugate(LLLE2x3x3)*complexconjugate(Rl1x6)',
                    texname = '\\text{I80x211}')

I80x213 = Parameter(name = 'I80x213',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl3x4*Rn2x2*ye1x1*complexconjugate(LLLE2x1x3)*complexconjugate(Rl1x6)',
                    texname = '\\text{I80x213}')

I80x216 = Parameter(name = 'I80x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x6*Rn2x2*ye3x3*complexconjugate(LLLE2x3x3)*complexconjugate(Rl1x6)',
                    texname = '\\text{I80x216}')

I80x221 = Parameter(name = 'I80x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x6*Rn2x2*ye3x3*complexconjugate(LLLE2x3x2)*complexconjugate(Rl2x5)',
                    texname = '\\text{I80x221}')

I80x223 = Parameter(name = 'I80x223',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl3x4*Rn2x2*ye1x1*complexconjugate(LLLE2x1x2)*complexconjugate(Rl2x5)',
                    texname = '\\text{I80x223}')

I80x226 = Parameter(name = 'I80x226',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x6*Rn2x2*ye3x3*complexconjugate(LLLE2x3x2)*complexconjugate(Rl2x5)',
                    texname = '\\text{I80x226}')

I80x231 = Parameter(name = 'I80x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x6*Rn2x2*ye3x3*complexconjugate(LLLE2x3x1)*complexconjugate(Rl3x4)',
                    texname = '\\text{I80x231}')

I80x233 = Parameter(name = 'I80x233',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl3x4*Rn2x2*ye1x1*complexconjugate(LLLE2x1x1)*complexconjugate(Rl3x4)',
                    texname = '\\text{I80x233}')

I80x236 = Parameter(name = 'I80x236',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x6*Rn2x2*ye3x3*complexconjugate(LLLE2x3x1)*complexconjugate(Rl3x4)',
                    texname = '\\text{I80x236}')

I80x261 = Parameter(name = 'I80x261',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x6*Rn2x2*ye3x3*complexconjugate(LLLE2x3x3)*complexconjugate(Rl6x6)',
                    texname = '\\text{I80x261}')

I80x263 = Parameter(name = 'I80x263',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl3x4*Rn2x2*ye1x1*complexconjugate(LLLE2x1x3)*complexconjugate(Rl6x6)',
                    texname = '\\text{I80x263}')

I80x266 = Parameter(name = 'I80x266',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x6*Rn2x2*ye3x3*complexconjugate(LLLE2x3x3)*complexconjugate(Rl6x6)',
                    texname = '\\text{I80x266}')

I80x311 = Parameter(name = 'I80x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x6*Rn3x1*ye3x3*complexconjugate(LLLE1x3x3)*complexconjugate(Rl1x6)',
                    texname = '\\text{I80x311}')

I80x312 = Parameter(name = 'I80x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl2x5*Rn3x1*ye2x2*complexconjugate(LLLE1x2x3)*complexconjugate(Rl1x6)',
                    texname = '\\text{I80x312}')

I80x316 = Parameter(name = 'I80x316',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x6*Rn3x1*ye3x3*complexconjugate(LLLE1x3x3)*complexconjugate(Rl1x6)',
                    texname = '\\text{I80x316}')

I80x321 = Parameter(name = 'I80x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x6*Rn3x1*ye3x3*complexconjugate(LLLE1x3x2)*complexconjugate(Rl2x5)',
                    texname = '\\text{I80x321}')

I80x322 = Parameter(name = 'I80x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl2x5*Rn3x1*ye2x2*complexconjugate(LLLE1x2x2)*complexconjugate(Rl2x5)',
                    texname = '\\text{I80x322}')

I80x326 = Parameter(name = 'I80x326',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x6*Rn3x1*ye3x3*complexconjugate(LLLE1x3x2)*complexconjugate(Rl2x5)',
                    texname = '\\text{I80x326}')

I80x331 = Parameter(name = 'I80x331',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x6*Rn3x1*ye3x3*complexconjugate(LLLE1x3x1)*complexconjugate(Rl3x4)',
                    texname = '\\text{I80x331}')

I80x332 = Parameter(name = 'I80x332',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl2x5*Rn3x1*ye2x2*complexconjugate(LLLE1x2x1)*complexconjugate(Rl3x4)',
                    texname = '\\text{I80x332}')

I80x336 = Parameter(name = 'I80x336',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x6*Rn3x1*ye3x3*complexconjugate(LLLE1x3x1)*complexconjugate(Rl3x4)',
                    texname = '\\text{I80x336}')

I80x361 = Parameter(name = 'I80x361',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl1x6*Rn3x1*ye3x3*complexconjugate(LLLE1x3x3)*complexconjugate(Rl6x6)',
                    texname = '\\text{I80x361}')

I80x362 = Parameter(name = 'I80x362',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl2x5*Rn3x1*ye2x2*complexconjugate(LLLE1x2x3)*complexconjugate(Rl6x6)',
                    texname = '\\text{I80x362}')

I80x366 = Parameter(name = 'I80x366',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rl6x6*Rn3x1*ye3x3*complexconjugate(LLLE1x3x3)*complexconjugate(Rl6x6)',
                    texname = '\\text{I80x366}')

I81x121 = Parameter(name = 'I81x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x1x2)*complexconjugate(Ru1x6)',
                    texname = '\\text{I81x121}')

I81x122 = Parameter(name = 'I81x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x1x2)*complexconjugate(Ru2x6)',
                    texname = '\\text{I81x122}')

I81x123 = Parameter(name = 'I81x123',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD2x1x2)*complexconjugate(Ru3x5)',
                    texname = '\\text{I81x123}')

I81x124 = Parameter(name = 'I81x124',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD1x1x2)*complexconjugate(Ru4x4)',
                    texname = '\\text{I81x124}')

I81x131 = Parameter(name = 'I81x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x1x3)*complexconjugate(Ru1x6)',
                    texname = '\\text{I81x131}')

I81x132 = Parameter(name = 'I81x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x1x3)*complexconjugate(Ru2x6)',
                    texname = '\\text{I81x132}')

I81x133 = Parameter(name = 'I81x133',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD2x1x3)*complexconjugate(Ru3x5)',
                    texname = '\\text{I81x133}')

I81x134 = Parameter(name = 'I81x134',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD1x1x3)*complexconjugate(Ru4x4)',
                    texname = '\\text{I81x134}')

I81x211 = Parameter(name = 'I81x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x2x1)*complexconjugate(Ru1x6)',
                    texname = '\\text{I81x211}')

I81x212 = Parameter(name = 'I81x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x2x1)*complexconjugate(Ru2x6)',
                    texname = '\\text{I81x212}')

I81x213 = Parameter(name = 'I81x213',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD2x2x1)*complexconjugate(Ru3x5)',
                    texname = '\\text{I81x213}')

I81x214 = Parameter(name = 'I81x214',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD1x2x1)*complexconjugate(Ru4x4)',
                    texname = '\\text{I81x214}')

I81x231 = Parameter(name = 'I81x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x2x3)*complexconjugate(Ru1x6)',
                    texname = '\\text{I81x231}')

I81x232 = Parameter(name = 'I81x232',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x2x3)*complexconjugate(Ru2x6)',
                    texname = '\\text{I81x232}')

I81x233 = Parameter(name = 'I81x233',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD2x2x3)*complexconjugate(Ru3x5)',
                    texname = '\\text{I81x233}')

I81x234 = Parameter(name = 'I81x234',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD1x2x3)*complexconjugate(Ru4x4)',
                    texname = '\\text{I81x234}')

I81x311 = Parameter(name = 'I81x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x3x1)*complexconjugate(Ru1x6)',
                    texname = '\\text{I81x311}')

I81x312 = Parameter(name = 'I81x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x3x1)*complexconjugate(Ru2x6)',
                    texname = '\\text{I81x312}')

I81x313 = Parameter(name = 'I81x313',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD2x3x1)*complexconjugate(Ru3x5)',
                    texname = '\\text{I81x313}')

I81x314 = Parameter(name = 'I81x314',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD1x3x1)*complexconjugate(Ru4x4)',
                    texname = '\\text{I81x314}')

I81x321 = Parameter(name = 'I81x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x3x2)*complexconjugate(Ru1x6)',
                    texname = '\\text{I81x321}')

I81x322 = Parameter(name = 'I81x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x3x2)*complexconjugate(Ru2x6)',
                    texname = '\\text{I81x322}')

I81x323 = Parameter(name = 'I81x323',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD2x3x2)*complexconjugate(Ru3x5)',
                    texname = '\\text{I81x323}')

I81x324 = Parameter(name = 'I81x324',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD1x3x2)*complexconjugate(Ru4x4)',
                    texname = '\\text{I81x324}')

I82x121 = Parameter(name = 'I82x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x2x1)*complexconjugate(Ru1x6)',
                    texname = '\\text{I82x121}')

I82x122 = Parameter(name = 'I82x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x2x1)*complexconjugate(Ru2x6)',
                    texname = '\\text{I82x122}')

I82x123 = Parameter(name = 'I82x123',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD2x2x1)*complexconjugate(Ru3x5)',
                    texname = '\\text{I82x123}')

I82x124 = Parameter(name = 'I82x124',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD1x2x1)*complexconjugate(Ru4x4)',
                    texname = '\\text{I82x124}')

I82x131 = Parameter(name = 'I82x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x3x1)*complexconjugate(Ru1x6)',
                    texname = '\\text{I82x131}')

I82x132 = Parameter(name = 'I82x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x3x1)*complexconjugate(Ru2x6)',
                    texname = '\\text{I82x132}')

I82x133 = Parameter(name = 'I82x133',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD2x3x1)*complexconjugate(Ru3x5)',
                    texname = '\\text{I82x133}')

I82x134 = Parameter(name = 'I82x134',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD1x3x1)*complexconjugate(Ru4x4)',
                    texname = '\\text{I82x134}')

I82x211 = Parameter(name = 'I82x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x1x2)*complexconjugate(Ru1x6)',
                    texname = '\\text{I82x211}')

I82x212 = Parameter(name = 'I82x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x1x2)*complexconjugate(Ru2x6)',
                    texname = '\\text{I82x212}')

I82x213 = Parameter(name = 'I82x213',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD2x1x2)*complexconjugate(Ru3x5)',
                    texname = '\\text{I82x213}')

I82x214 = Parameter(name = 'I82x214',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD1x1x2)*complexconjugate(Ru4x4)',
                    texname = '\\text{I82x214}')

I82x231 = Parameter(name = 'I82x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x3x2)*complexconjugate(Ru1x6)',
                    texname = '\\text{I82x231}')

I82x232 = Parameter(name = 'I82x232',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x3x2)*complexconjugate(Ru2x6)',
                    texname = '\\text{I82x232}')

I82x233 = Parameter(name = 'I82x233',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD2x3x2)*complexconjugate(Ru3x5)',
                    texname = '\\text{I82x233}')

I82x234 = Parameter(name = 'I82x234',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD1x3x2)*complexconjugate(Ru4x4)',
                    texname = '\\text{I82x234}')

I82x311 = Parameter(name = 'I82x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x1x3)*complexconjugate(Ru1x6)',
                    texname = '\\text{I82x311}')

I82x312 = Parameter(name = 'I82x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x1x3)*complexconjugate(Ru2x6)',
                    texname = '\\text{I82x312}')

I82x313 = Parameter(name = 'I82x313',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD2x1x3)*complexconjugate(Ru3x5)',
                    texname = '\\text{I82x313}')

I82x314 = Parameter(name = 'I82x314',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD1x1x3)*complexconjugate(Ru4x4)',
                    texname = '\\text{I82x314}')

I82x321 = Parameter(name = 'I82x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x2x3)*complexconjugate(Ru1x6)',
                    texname = '\\text{I82x321}')

I82x322 = Parameter(name = 'I82x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD3x2x3)*complexconjugate(Ru2x6)',
                    texname = '\\text{I82x322}')

I82x323 = Parameter(name = 'I82x323',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD2x2x3)*complexconjugate(Ru3x5)',
                    texname = '\\text{I82x323}')

I82x324 = Parameter(name = 'I82x324',
                    nature = 'internal',
                    type = 'complex',
                    value = 'complexconjugate(LUDD1x2x3)*complexconjugate(Ru4x4)',
                    texname = '\\text{I82x324}')

I83x14 = Parameter(name = 'I83x14',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                   texname = '\\text{I83x14}')

I83x23 = Parameter(name = 'I83x23',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                   texname = '\\text{I83x23}')

I83x31 = Parameter(name = 'I83x31',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                   texname = '\\text{I83x31}')

I83x32 = Parameter(name = 'I83x32',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                   texname = '\\text{I83x32}')

I84x15 = Parameter(name = 'I84x15',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yu1x1*complexconjugate(Ru5x1)',
                   texname = '\\text{I84x15}')

I84x26 = Parameter(name = 'I84x26',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yu2x2*complexconjugate(Ru6x2)',
                   texname = '\\text{I84x26}')

I84x31 = Parameter(name = 'I84x31',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yu3x3*complexconjugate(Ru1x3)',
                   texname = '\\text{I84x31}')

I84x32 = Parameter(name = 'I84x32',
                   nature = 'internal',
                   type = 'complex',
                   value = 'yu3x3*complexconjugate(Ru2x3)',
                   texname = '\\text{I84x32}')

I85x111 = Parameter(name = 'I85x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x1*complexconjugate(Ru1x3)',
                    texname = '\\text{I85x111}')

I85x112 = Parameter(name = 'I85x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x1*complexconjugate(Ru2x3)',
                    texname = '\\text{I85x112}')

I85x115 = Parameter(name = 'I85x115',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x1*complexconjugate(Ru5x1)',
                    texname = '\\text{I85x115}')

I85x116 = Parameter(name = 'I85x116',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x1*complexconjugate(Ru6x2)',
                    texname = '\\text{I85x116}')

I85x121 = Parameter(name = 'I85x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x1*complexconjugate(Ru1x3)',
                    texname = '\\text{I85x121}')

I85x122 = Parameter(name = 'I85x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x1*complexconjugate(Ru2x3)',
                    texname = '\\text{I85x122}')

I85x125 = Parameter(name = 'I85x125',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x1*complexconjugate(Ru5x1)',
                    texname = '\\text{I85x125}')

I85x126 = Parameter(name = 'I85x126',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x1*complexconjugate(Ru6x2)',
                    texname = '\\text{I85x126}')

I85x131 = Parameter(name = 'I85x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*complexconjugate(Ru1x3)',
                    texname = '\\text{I85x131}')

I85x132 = Parameter(name = 'I85x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*complexconjugate(Ru2x3)',
                    texname = '\\text{I85x132}')

I85x135 = Parameter(name = 'I85x135',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x1*complexconjugate(Ru5x1)',
                    texname = '\\text{I85x135}')

I85x136 = Parameter(name = 'I85x136',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x1*complexconjugate(Ru6x2)',
                    texname = '\\text{I85x136}')

I85x211 = Parameter(name = 'I85x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x2*complexconjugate(Ru1x3)',
                    texname = '\\text{I85x211}')

I85x212 = Parameter(name = 'I85x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x2*complexconjugate(Ru2x3)',
                    texname = '\\text{I85x212}')

I85x215 = Parameter(name = 'I85x215',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x2*complexconjugate(Ru5x1)',
                    texname = '\\text{I85x215}')

I85x216 = Parameter(name = 'I85x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x2*complexconjugate(Ru6x2)',
                    texname = '\\text{I85x216}')

I85x221 = Parameter(name = 'I85x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x2*complexconjugate(Ru1x3)',
                    texname = '\\text{I85x221}')

I85x222 = Parameter(name = 'I85x222',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x2*complexconjugate(Ru2x3)',
                    texname = '\\text{I85x222}')

I85x225 = Parameter(name = 'I85x225',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x2*complexconjugate(Ru5x1)',
                    texname = '\\text{I85x225}')

I85x226 = Parameter(name = 'I85x226',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x2*complexconjugate(Ru6x2)',
                    texname = '\\text{I85x226}')

I85x231 = Parameter(name = 'I85x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*complexconjugate(Ru1x3)',
                    texname = '\\text{I85x231}')

I85x232 = Parameter(name = 'I85x232',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*complexconjugate(Ru2x3)',
                    texname = '\\text{I85x232}')

I85x235 = Parameter(name = 'I85x235',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x2*complexconjugate(Ru5x1)',
                    texname = '\\text{I85x235}')

I85x236 = Parameter(name = 'I85x236',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x2*complexconjugate(Ru6x2)',
                    texname = '\\text{I85x236}')

I85x311 = Parameter(name = 'I85x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*complexconjugate(Ru1x3)',
                    texname = '\\text{I85x311}')

I85x312 = Parameter(name = 'I85x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*complexconjugate(Ru2x3)',
                    texname = '\\text{I85x312}')

I85x315 = Parameter(name = 'I85x315',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x3*complexconjugate(Ru5x1)',
                    texname = '\\text{I85x315}')

I85x316 = Parameter(name = 'I85x316',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x3*complexconjugate(Ru6x2)',
                    texname = '\\text{I85x316}')

I85x321 = Parameter(name = 'I85x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*complexconjugate(Ru1x3)',
                    texname = '\\text{I85x321}')

I85x322 = Parameter(name = 'I85x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*complexconjugate(Ru2x3)',
                    texname = '\\text{I85x322}')

I85x325 = Parameter(name = 'I85x325',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x3*complexconjugate(Ru5x1)',
                    texname = '\\text{I85x325}')

I85x326 = Parameter(name = 'I85x326',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x3*complexconjugate(Ru6x2)',
                    texname = '\\text{I85x326}')

I85x331 = Parameter(name = 'I85x331',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*complexconjugate(Ru1x3)',
                    texname = '\\text{I85x331}')

I85x332 = Parameter(name = 'I85x332',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*complexconjugate(Ru2x3)',
                    texname = '\\text{I85x332}')

I85x335 = Parameter(name = 'I85x335',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*complexconjugate(Ru5x1)',
                    texname = '\\text{I85x335}')

I85x336 = Parameter(name = 'I85x336',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*complexconjugate(Ru6x2)',
                    texname = '\\text{I85x336}')

I86x11 = Parameter(name = 'I86x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Ru1x3*complexconjugate(Ru1x3)',
                   texname = '\\text{I86x11}')

I86x12 = Parameter(name = 'I86x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Ru2x3*complexconjugate(Ru1x3)',
                   texname = '\\text{I86x12}')

I86x21 = Parameter(name = 'I86x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Ru1x3*complexconjugate(Ru2x3)',
                   texname = '\\text{I86x21}')

I86x22 = Parameter(name = 'I86x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Ru2x3*complexconjugate(Ru2x3)',
                   texname = '\\text{I86x22}')

I86x55 = Parameter(name = 'I86x55',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Ru5x1*complexconjugate(Ru5x1)',
                   texname = '\\text{I86x55}')

I86x66 = Parameter(name = 'I86x66',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Ru6x2*complexconjugate(Ru6x2)',
                   texname = '\\text{I86x66}')

I87x11 = Parameter(name = 'I87x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Ru1x6*complexconjugate(Ru1x6)',
                   texname = '\\text{I87x11}')

I87x12 = Parameter(name = 'I87x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Ru2x6*complexconjugate(Ru1x6)',
                   texname = '\\text{I87x12}')

I87x21 = Parameter(name = 'I87x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Ru1x6*complexconjugate(Ru2x6)',
                   texname = '\\text{I87x21}')

I87x22 = Parameter(name = 'I87x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Ru2x6*complexconjugate(Ru2x6)',
                   texname = '\\text{I87x22}')

I87x33 = Parameter(name = 'I87x33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Ru3x5*complexconjugate(Ru3x5)',
                   texname = '\\text{I87x33}')

I87x44 = Parameter(name = 'I87x44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Ru4x4*complexconjugate(Ru4x4)',
                   texname = '\\text{I87x44}')

I88x11 = Parameter(name = 'I88x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*complexconjugate(Ru1x3)',
                   texname = '\\text{I88x11}')

I88x12 = Parameter(name = 'I88x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*complexconjugate(Ru2x3)',
                   texname = '\\text{I88x12}')

I88x21 = Parameter(name = 'I88x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*complexconjugate(Ru1x3)',
                   texname = '\\text{I88x21}')

I88x22 = Parameter(name = 'I88x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*complexconjugate(Ru2x3)',
                   texname = '\\text{I88x22}')

I88x56 = Parameter(name = 'I88x56',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd5x2*complexconjugate(Ru6x2)',
                   texname = '\\text{I88x56}')

I88x65 = Parameter(name = 'I88x65',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd6x1*complexconjugate(Ru5x1)',
                   texname = '\\text{I88x65}')

I89x11 = Parameter(name = 'I89x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*complexconjugate(Ru1x6)*complexconjugate(tu3x3)',
                   texname = '\\text{I89x11}')

I89x12 = Parameter(name = 'I89x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*complexconjugate(Ru2x6)*complexconjugate(tu3x3)',
                   texname = '\\text{I89x12}')

I89x21 = Parameter(name = 'I89x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*complexconjugate(Ru1x6)*complexconjugate(tu3x3)',
                   texname = '\\text{I89x21}')

I89x22 = Parameter(name = 'I89x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*complexconjugate(Ru2x6)*complexconjugate(tu3x3)',
                   texname = '\\text{I89x22}')

I89x53 = Parameter(name = 'I89x53',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd5x2*complexconjugate(Ru3x5)*complexconjugate(tu2x2)',
                   texname = '\\text{I89x53}')

I89x64 = Parameter(name = 'I89x64',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd6x1*complexconjugate(Ru4x4)*complexconjugate(tu1x1)',
                   texname = '\\text{I89x64}')

I9x111 = Parameter(name = 'I9x111',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x1x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I9x111}')

I9x112 = Parameter(name = 'I9x112',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x1x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I9x112}')

I9x113 = Parameter(name = 'I9x113',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x1x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I9x113}')

I9x114 = Parameter(name = 'I9x114',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x1x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I9x114}')

I9x121 = Parameter(name = 'I9x121',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x2x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I9x121}')

I9x122 = Parameter(name = 'I9x122',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x2x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I9x122}')

I9x123 = Parameter(name = 'I9x123',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x2x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I9x123}')

I9x124 = Parameter(name = 'I9x124',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x2x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I9x124}')

I9x131 = Parameter(name = 'I9x131',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x3x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I9x131}')

I9x132 = Parameter(name = 'I9x132',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x3x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I9x132}')

I9x133 = Parameter(name = 'I9x133',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x3x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I9x133}')

I9x134 = Parameter(name = 'I9x134',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD1x3x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I9x134}')

I9x211 = Parameter(name = 'I9x211',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x1x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I9x211}')

I9x212 = Parameter(name = 'I9x212',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x1x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I9x212}')

I9x213 = Parameter(name = 'I9x213',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x1x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I9x213}')

I9x214 = Parameter(name = 'I9x214',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x1x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I9x214}')

I9x221 = Parameter(name = 'I9x221',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x2x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I9x221}')

I9x222 = Parameter(name = 'I9x222',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x2x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I9x222}')

I9x223 = Parameter(name = 'I9x223',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x2x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I9x223}')

I9x224 = Parameter(name = 'I9x224',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x2x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I9x224}')

I9x231 = Parameter(name = 'I9x231',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x3x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I9x231}')

I9x232 = Parameter(name = 'I9x232',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x3x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I9x232}')

I9x233 = Parameter(name = 'I9x233',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x3x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I9x233}')

I9x234 = Parameter(name = 'I9x234',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD2x3x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I9x234}')

I9x311 = Parameter(name = 'I9x311',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x1x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I9x311}')

I9x312 = Parameter(name = 'I9x312',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x1x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I9x312}')

I9x313 = Parameter(name = 'I9x313',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x1x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I9x313}')

I9x314 = Parameter(name = 'I9x314',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x1x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I9x314}')

I9x321 = Parameter(name = 'I9x321',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x2x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I9x321}')

I9x322 = Parameter(name = 'I9x322',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x2x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I9x322}')

I9x323 = Parameter(name = 'I9x323',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x2x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I9x323}')

I9x324 = Parameter(name = 'I9x324',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x2x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I9x324}')

I9x331 = Parameter(name = 'I9x331',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x3x3)*complexconjugate(Rd1x6)',
                   texname = '\\text{I9x331}')

I9x332 = Parameter(name = 'I9x332',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x3x3)*complexconjugate(Rd2x6)',
                   texname = '\\text{I9x332}')

I9x333 = Parameter(name = 'I9x333',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x3x2)*complexconjugate(Rd3x5)',
                   texname = '\\text{I9x333}')

I9x334 = Parameter(name = 'I9x334',
                   nature = 'internal',
                   type = 'complex',
                   value = 'complexconjugate(LLQD3x3x1)*complexconjugate(Rd4x4)',
                   texname = '\\text{I9x334}')

I90x11 = Parameter(name = 'I90x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                   texname = '\\text{I90x11}')

I90x12 = Parameter(name = 'I90x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                   texname = '\\text{I90x12}')

I90x21 = Parameter(name = 'I90x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                   texname = '\\text{I90x21}')

I90x22 = Parameter(name = 'I90x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                   texname = '\\text{I90x22}')

I90x53 = Parameter(name = 'I90x53',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd5x2*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                   texname = '\\text{I90x53}')

I90x64 = Parameter(name = 'I90x64',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd6x1*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                   texname = '\\text{I90x64}')

I91x11 = Parameter(name = 'I91x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x6*td3x3*complexconjugate(Ru1x3)',
                   texname = '\\text{I91x11}')

I91x12 = Parameter(name = 'I91x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x6*td3x3*complexconjugate(Ru2x3)',
                   texname = '\\text{I91x12}')

I91x21 = Parameter(name = 'I91x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x6*td3x3*complexconjugate(Ru1x3)',
                   texname = '\\text{I91x21}')

I91x22 = Parameter(name = 'I91x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x6*td3x3*complexconjugate(Ru2x3)',
                   texname = '\\text{I91x22}')

I91x36 = Parameter(name = 'I91x36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd3x5*td2x2*complexconjugate(Ru6x2)',
                   texname = '\\text{I91x36}')

I91x45 = Parameter(name = 'I91x45',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd4x4*td1x1*complexconjugate(Ru5x1)',
                   texname = '\\text{I91x45}')

I92x11 = Parameter(name = 'I92x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x6*yd3x3*complexconjugate(Ru1x3)',
                   texname = '\\text{I92x11}')

I92x12 = Parameter(name = 'I92x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x6*yd3x3*complexconjugate(Ru2x3)',
                   texname = '\\text{I92x12}')

I92x21 = Parameter(name = 'I92x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x6*yd3x3*complexconjugate(Ru1x3)',
                   texname = '\\text{I92x21}')

I92x22 = Parameter(name = 'I92x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x6*yd3x3*complexconjugate(Ru2x3)',
                   texname = '\\text{I92x22}')

I92x36 = Parameter(name = 'I92x36',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd3x5*yd2x2*complexconjugate(Ru6x2)',
                   texname = '\\text{I92x36}')

I92x45 = Parameter(name = 'I92x45',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd4x4*yd1x1*complexconjugate(Ru5x1)',
                   texname = '\\text{I92x45}')

I93x11 = Parameter(name = 'I93x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*yd3x3*complexconjugate(Ru1x3)*complexconjugate(yd3x3)',
                   texname = '\\text{I93x11}')

I93x12 = Parameter(name = 'I93x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*yd3x3*complexconjugate(Ru2x3)*complexconjugate(yd3x3)',
                   texname = '\\text{I93x12}')

I93x21 = Parameter(name = 'I93x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*yd3x3*complexconjugate(Ru1x3)*complexconjugate(yd3x3)',
                   texname = '\\text{I93x21}')

I93x22 = Parameter(name = 'I93x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*yd3x3*complexconjugate(Ru2x3)*complexconjugate(yd3x3)',
                   texname = '\\text{I93x22}')

I93x56 = Parameter(name = 'I93x56',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd5x2*yd2x2*complexconjugate(Ru6x2)*complexconjugate(yd2x2)',
                   texname = '\\text{I93x56}')

I93x65 = Parameter(name = 'I93x65',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd6x1*yd1x1*complexconjugate(Ru5x1)*complexconjugate(yd1x1)',
                   texname = '\\text{I93x65}')

I94x11 = Parameter(name = 'I94x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x6*yd3x3*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                   texname = '\\text{I94x11}')

I94x12 = Parameter(name = 'I94x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x6*yd3x3*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                   texname = '\\text{I94x12}')

I94x21 = Parameter(name = 'I94x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x6*yd3x3*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                   texname = '\\text{I94x21}')

I94x22 = Parameter(name = 'I94x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x6*yd3x3*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                   texname = '\\text{I94x22}')

I94x33 = Parameter(name = 'I94x33',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd3x5*yd2x2*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                   texname = '\\text{I94x33}')

I94x44 = Parameter(name = 'I94x44',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd4x4*yd1x1*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                   texname = '\\text{I94x44}')

I95x11 = Parameter(name = 'I95x11',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*yu3x3*complexconjugate(Ru1x3)*complexconjugate(yu3x3)',
                   texname = '\\text{I95x11}')

I95x12 = Parameter(name = 'I95x12',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd1x3*yu3x3*complexconjugate(Ru2x3)*complexconjugate(yu3x3)',
                   texname = '\\text{I95x12}')

I95x21 = Parameter(name = 'I95x21',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*yu3x3*complexconjugate(Ru1x3)*complexconjugate(yu3x3)',
                   texname = '\\text{I95x21}')

I95x22 = Parameter(name = 'I95x22',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd2x3*yu3x3*complexconjugate(Ru2x3)*complexconjugate(yu3x3)',
                   texname = '\\text{I95x22}')

I95x56 = Parameter(name = 'I95x56',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd5x2*yu2x2*complexconjugate(Ru6x2)*complexconjugate(yu2x2)',
                   texname = '\\text{I95x56}')

I95x65 = Parameter(name = 'I95x65',
                   nature = 'internal',
                   type = 'complex',
                   value = 'Rd6x1*yu1x1*complexconjugate(Ru5x1)*complexconjugate(yu1x1)',
                   texname = '\\text{I95x65}')

I96x111 = Parameter(name = 'I96x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd1x3*complexconjugate(Rl1x3)*complexconjugate(Ru1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x111}')

I96x112 = Parameter(name = 'I96x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd1x3*complexconjugate(Rl1x3)*complexconjugate(Ru2x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x112}')

I96x115 = Parameter(name = 'I96x115',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd1x3*complexconjugate(Rl1x3)*complexconjugate(Ru5x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x115}')

I96x116 = Parameter(name = 'I96x116',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd1x3*complexconjugate(Rl1x3)*complexconjugate(Ru6x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x116}')

I96x141 = Parameter(name = 'I96x141',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd1x3*complexconjugate(Rl4x1)*complexconjugate(Ru1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x141}')

I96x142 = Parameter(name = 'I96x142',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd1x3*complexconjugate(Rl4x1)*complexconjugate(Ru2x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x142}')

I96x145 = Parameter(name = 'I96x145',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x3*Rd1x3*complexconjugate(Rl4x1)*complexconjugate(Ru5x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x145}')

I96x146 = Parameter(name = 'I96x146',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x3*Rd1x3*complexconjugate(Rl4x1)*complexconjugate(Ru6x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x146}')

I96x151 = Parameter(name = 'I96x151',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd1x3*complexconjugate(Rl5x2)*complexconjugate(Ru1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x151}')

I96x152 = Parameter(name = 'I96x152',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd1x3*complexconjugate(Rl5x2)*complexconjugate(Ru2x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x152}')

I96x155 = Parameter(name = 'I96x155',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x3*Rd1x3*complexconjugate(Rl5x2)*complexconjugate(Ru5x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x155}')

I96x156 = Parameter(name = 'I96x156',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x3*Rd1x3*complexconjugate(Rl5x2)*complexconjugate(Ru6x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x156}')

I96x161 = Parameter(name = 'I96x161',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd1x3*complexconjugate(Rl6x3)*complexconjugate(Ru1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x161}')

I96x162 = Parameter(name = 'I96x162',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd1x3*complexconjugate(Rl6x3)*complexconjugate(Ru2x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x162}')

I96x165 = Parameter(name = 'I96x165',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd1x3*complexconjugate(Rl6x3)*complexconjugate(Ru5x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x165}')

I96x166 = Parameter(name = 'I96x166',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd1x3*complexconjugate(Rl6x3)*complexconjugate(Ru6x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x166}')

I96x211 = Parameter(name = 'I96x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd2x3*complexconjugate(Rl1x3)*complexconjugate(Ru1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x211}')

I96x212 = Parameter(name = 'I96x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd2x3*complexconjugate(Rl1x3)*complexconjugate(Ru2x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x212}')

I96x215 = Parameter(name = 'I96x215',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd2x3*complexconjugate(Rl1x3)*complexconjugate(Ru5x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x215}')

I96x216 = Parameter(name = 'I96x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd2x3*complexconjugate(Rl1x3)*complexconjugate(Ru6x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x216}')

I96x241 = Parameter(name = 'I96x241',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd2x3*complexconjugate(Rl4x1)*complexconjugate(Ru1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x241}')

I96x242 = Parameter(name = 'I96x242',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd2x3*complexconjugate(Rl4x1)*complexconjugate(Ru2x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x242}')

I96x245 = Parameter(name = 'I96x245',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x3*Rd2x3*complexconjugate(Rl4x1)*complexconjugate(Ru5x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x245}')

I96x246 = Parameter(name = 'I96x246',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x3*Rd2x3*complexconjugate(Rl4x1)*complexconjugate(Ru6x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x246}')

I96x251 = Parameter(name = 'I96x251',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd2x3*complexconjugate(Rl5x2)*complexconjugate(Ru1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x251}')

I96x252 = Parameter(name = 'I96x252',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd2x3*complexconjugate(Rl5x2)*complexconjugate(Ru2x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x252}')

I96x255 = Parameter(name = 'I96x255',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x3*Rd2x3*complexconjugate(Rl5x2)*complexconjugate(Ru5x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x255}')

I96x256 = Parameter(name = 'I96x256',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x3*Rd2x3*complexconjugate(Rl5x2)*complexconjugate(Ru6x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x256}')

I96x261 = Parameter(name = 'I96x261',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd2x3*complexconjugate(Rl6x3)*complexconjugate(Ru1x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x261}')

I96x262 = Parameter(name = 'I96x262',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd2x3*complexconjugate(Rl6x3)*complexconjugate(Ru2x3)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x262}')

I96x265 = Parameter(name = 'I96x265',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd2x3*complexconjugate(Rl6x3)*complexconjugate(Ru5x1)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x265}')

I96x266 = Parameter(name = 'I96x266',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd2x3*complexconjugate(Rl6x3)*complexconjugate(Ru6x2)*complexconjugate(yd3x3)',
                    texname = '\\text{I96x266}')

I96x511 = Parameter(name = 'I96x511',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*Rd5x2*complexconjugate(Rl1x3)*complexconjugate(Ru1x3)*complexconjugate(yd2x2)',
                    texname = '\\text{I96x511}')

I96x512 = Parameter(name = 'I96x512',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*Rd5x2*complexconjugate(Rl1x3)*complexconjugate(Ru2x3)*complexconjugate(yd2x2)',
                    texname = '\\text{I96x512}')

I96x515 = Parameter(name = 'I96x515',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x2*Rd5x2*complexconjugate(Rl1x3)*complexconjugate(Ru5x1)*complexconjugate(yd2x2)',
                    texname = '\\text{I96x515}')

I96x516 = Parameter(name = 'I96x516',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x2*Rd5x2*complexconjugate(Rl1x3)*complexconjugate(Ru6x2)*complexconjugate(yd2x2)',
                    texname = '\\text{I96x516}')

I96x541 = Parameter(name = 'I96x541',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x2*Rd5x2*complexconjugate(Rl4x1)*complexconjugate(Ru1x3)*complexconjugate(yd2x2)',
                    texname = '\\text{I96x541}')

I96x542 = Parameter(name = 'I96x542',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x2*Rd5x2*complexconjugate(Rl4x1)*complexconjugate(Ru2x3)*complexconjugate(yd2x2)',
                    texname = '\\text{I96x542}')

I96x545 = Parameter(name = 'I96x545',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x2*Rd5x2*complexconjugate(Rl4x1)*complexconjugate(Ru5x1)*complexconjugate(yd2x2)',
                    texname = '\\text{I96x545}')

I96x546 = Parameter(name = 'I96x546',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x2*Rd5x2*complexconjugate(Rl4x1)*complexconjugate(Ru6x2)*complexconjugate(yd2x2)',
                    texname = '\\text{I96x546}')

I96x551 = Parameter(name = 'I96x551',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x2*Rd5x2*complexconjugate(Rl5x2)*complexconjugate(Ru1x3)*complexconjugate(yd2x2)',
                    texname = '\\text{I96x551}')

I96x552 = Parameter(name = 'I96x552',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x2*Rd5x2*complexconjugate(Rl5x2)*complexconjugate(Ru2x3)*complexconjugate(yd2x2)',
                    texname = '\\text{I96x552}')

I96x555 = Parameter(name = 'I96x555',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x2*Rd5x2*complexconjugate(Rl5x2)*complexconjugate(Ru5x1)*complexconjugate(yd2x2)',
                    texname = '\\text{I96x555}')

I96x556 = Parameter(name = 'I96x556',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x2*Rd5x2*complexconjugate(Rl5x2)*complexconjugate(Ru6x2)*complexconjugate(yd2x2)',
                    texname = '\\text{I96x556}')

I96x561 = Parameter(name = 'I96x561',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*Rd5x2*complexconjugate(Rl6x3)*complexconjugate(Ru1x3)*complexconjugate(yd2x2)',
                    texname = '\\text{I96x561}')

I96x562 = Parameter(name = 'I96x562',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*Rd5x2*complexconjugate(Rl6x3)*complexconjugate(Ru2x3)*complexconjugate(yd2x2)',
                    texname = '\\text{I96x562}')

I96x565 = Parameter(name = 'I96x565',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x2*Rd5x2*complexconjugate(Rl6x3)*complexconjugate(Ru5x1)*complexconjugate(yd2x2)',
                    texname = '\\text{I96x565}')

I96x566 = Parameter(name = 'I96x566',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x2*Rd5x2*complexconjugate(Rl6x3)*complexconjugate(Ru6x2)*complexconjugate(yd2x2)',
                    texname = '\\text{I96x566}')

I96x611 = Parameter(name = 'I96x611',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*Rd6x1*complexconjugate(Rl1x3)*complexconjugate(Ru1x3)*complexconjugate(yd1x1)',
                    texname = '\\text{I96x611}')

I96x612 = Parameter(name = 'I96x612',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*Rd6x1*complexconjugate(Rl1x3)*complexconjugate(Ru2x3)*complexconjugate(yd1x1)',
                    texname = '\\text{I96x612}')

I96x615 = Parameter(name = 'I96x615',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x1*Rd6x1*complexconjugate(Rl1x3)*complexconjugate(Ru5x1)*complexconjugate(yd1x1)',
                    texname = '\\text{I96x615}')

I96x616 = Parameter(name = 'I96x616',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x1*Rd6x1*complexconjugate(Rl1x3)*complexconjugate(Ru6x2)*complexconjugate(yd1x1)',
                    texname = '\\text{I96x616}')

I96x641 = Parameter(name = 'I96x641',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x1*Rd6x1*complexconjugate(Rl4x1)*complexconjugate(Ru1x3)*complexconjugate(yd1x1)',
                    texname = '\\text{I96x641}')

I96x642 = Parameter(name = 'I96x642',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x1*Rd6x1*complexconjugate(Rl4x1)*complexconjugate(Ru2x3)*complexconjugate(yd1x1)',
                    texname = '\\text{I96x642}')

I96x645 = Parameter(name = 'I96x645',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x1*Rd6x1*complexconjugate(Rl4x1)*complexconjugate(Ru5x1)*complexconjugate(yd1x1)',
                    texname = '\\text{I96x645}')

I96x646 = Parameter(name = 'I96x646',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x1*Rd6x1*complexconjugate(Rl4x1)*complexconjugate(Ru6x2)*complexconjugate(yd1x1)',
                    texname = '\\text{I96x646}')

I96x651 = Parameter(name = 'I96x651',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x1*Rd6x1*complexconjugate(Rl5x2)*complexconjugate(Ru1x3)*complexconjugate(yd1x1)',
                    texname = '\\text{I96x651}')

I96x652 = Parameter(name = 'I96x652',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x1*Rd6x1*complexconjugate(Rl5x2)*complexconjugate(Ru2x3)*complexconjugate(yd1x1)',
                    texname = '\\text{I96x652}')

I96x655 = Parameter(name = 'I96x655',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x1*Rd6x1*complexconjugate(Rl5x2)*complexconjugate(Ru5x1)*complexconjugate(yd1x1)',
                    texname = '\\text{I96x655}')

I96x656 = Parameter(name = 'I96x656',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x1*Rd6x1*complexconjugate(Rl5x2)*complexconjugate(Ru6x2)*complexconjugate(yd1x1)',
                    texname = '\\text{I96x656}')

I96x661 = Parameter(name = 'I96x661',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*Rd6x1*complexconjugate(Rl6x3)*complexconjugate(Ru1x3)*complexconjugate(yd1x1)',
                    texname = '\\text{I96x661}')

I96x662 = Parameter(name = 'I96x662',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*Rd6x1*complexconjugate(Rl6x3)*complexconjugate(Ru2x3)*complexconjugate(yd1x1)',
                    texname = '\\text{I96x662}')

I96x665 = Parameter(name = 'I96x665',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x1*Rd6x1*complexconjugate(Rl6x3)*complexconjugate(Ru5x1)*complexconjugate(yd1x1)',
                    texname = '\\text{I96x665}')

I96x666 = Parameter(name = 'I96x666',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x1*Rd6x1*complexconjugate(Rl6x3)*complexconjugate(Ru6x2)*complexconjugate(yd1x1)',
                    texname = '\\text{I96x666}')

I97x111 = Parameter(name = 'I97x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd1x6*complexconjugate(Rl1x6)*complexconjugate(Ru1x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x111}')

I97x112 = Parameter(name = 'I97x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd1x6*complexconjugate(Rl1x6)*complexconjugate(Ru2x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x112}')

I97x115 = Parameter(name = 'I97x115',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd1x6*complexconjugate(Rl1x6)*complexconjugate(Ru5x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x115}')

I97x116 = Parameter(name = 'I97x116',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd1x6*complexconjugate(Rl1x6)*complexconjugate(Ru6x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x116}')

I97x121 = Parameter(name = 'I97x121',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd1x6*complexconjugate(Rl2x5)*complexconjugate(Ru1x3)*complexconjugate(ye2x2)',
                    texname = '\\text{I97x121}')

I97x122 = Parameter(name = 'I97x122',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd1x6*complexconjugate(Rl2x5)*complexconjugate(Ru2x3)*complexconjugate(ye2x2)',
                    texname = '\\text{I97x122}')

I97x125 = Parameter(name = 'I97x125',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x3*Rd1x6*complexconjugate(Rl2x5)*complexconjugate(Ru5x1)*complexconjugate(ye2x2)',
                    texname = '\\text{I97x125}')

I97x126 = Parameter(name = 'I97x126',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x3*Rd1x6*complexconjugate(Rl2x5)*complexconjugate(Ru6x2)*complexconjugate(ye2x2)',
                    texname = '\\text{I97x126}')

I97x131 = Parameter(name = 'I97x131',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd1x6*complexconjugate(Rl3x4)*complexconjugate(Ru1x3)*complexconjugate(ye1x1)',
                    texname = '\\text{I97x131}')

I97x132 = Parameter(name = 'I97x132',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd1x6*complexconjugate(Rl3x4)*complexconjugate(Ru2x3)*complexconjugate(ye1x1)',
                    texname = '\\text{I97x132}')

I97x135 = Parameter(name = 'I97x135',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x3*Rd1x6*complexconjugate(Rl3x4)*complexconjugate(Ru5x1)*complexconjugate(ye1x1)',
                    texname = '\\text{I97x135}')

I97x136 = Parameter(name = 'I97x136',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x3*Rd1x6*complexconjugate(Rl3x4)*complexconjugate(Ru6x2)*complexconjugate(ye1x1)',
                    texname = '\\text{I97x136}')

I97x161 = Parameter(name = 'I97x161',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd1x6*complexconjugate(Rl6x6)*complexconjugate(Ru1x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x161}')

I97x162 = Parameter(name = 'I97x162',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd1x6*complexconjugate(Rl6x6)*complexconjugate(Ru2x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x162}')

I97x165 = Parameter(name = 'I97x165',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd1x6*complexconjugate(Rl6x6)*complexconjugate(Ru5x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x165}')

I97x166 = Parameter(name = 'I97x166',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd1x6*complexconjugate(Rl6x6)*complexconjugate(Ru6x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x166}')

I97x211 = Parameter(name = 'I97x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd2x6*complexconjugate(Rl1x6)*complexconjugate(Ru1x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x211}')

I97x212 = Parameter(name = 'I97x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd2x6*complexconjugate(Rl1x6)*complexconjugate(Ru2x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x212}')

I97x215 = Parameter(name = 'I97x215',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd2x6*complexconjugate(Rl1x6)*complexconjugate(Ru5x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x215}')

I97x216 = Parameter(name = 'I97x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd2x6*complexconjugate(Rl1x6)*complexconjugate(Ru6x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x216}')

I97x221 = Parameter(name = 'I97x221',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd2x6*complexconjugate(Rl2x5)*complexconjugate(Ru1x3)*complexconjugate(ye2x2)',
                    texname = '\\text{I97x221}')

I97x222 = Parameter(name = 'I97x222',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd2x6*complexconjugate(Rl2x5)*complexconjugate(Ru2x3)*complexconjugate(ye2x2)',
                    texname = '\\text{I97x222}')

I97x225 = Parameter(name = 'I97x225',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x3*Rd2x6*complexconjugate(Rl2x5)*complexconjugate(Ru5x1)*complexconjugate(ye2x2)',
                    texname = '\\text{I97x225}')

I97x226 = Parameter(name = 'I97x226',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x3*Rd2x6*complexconjugate(Rl2x5)*complexconjugate(Ru6x2)*complexconjugate(ye2x2)',
                    texname = '\\text{I97x226}')

I97x231 = Parameter(name = 'I97x231',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd2x6*complexconjugate(Rl3x4)*complexconjugate(Ru1x3)*complexconjugate(ye1x1)',
                    texname = '\\text{I97x231}')

I97x232 = Parameter(name = 'I97x232',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd2x6*complexconjugate(Rl3x4)*complexconjugate(Ru2x3)*complexconjugate(ye1x1)',
                    texname = '\\text{I97x232}')

I97x235 = Parameter(name = 'I97x235',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x3*Rd2x6*complexconjugate(Rl3x4)*complexconjugate(Ru5x1)*complexconjugate(ye1x1)',
                    texname = '\\text{I97x235}')

I97x236 = Parameter(name = 'I97x236',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x3*Rd2x6*complexconjugate(Rl3x4)*complexconjugate(Ru6x2)*complexconjugate(ye1x1)',
                    texname = '\\text{I97x236}')

I97x261 = Parameter(name = 'I97x261',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd2x6*complexconjugate(Rl6x6)*complexconjugate(Ru1x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x261}')

I97x262 = Parameter(name = 'I97x262',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd2x6*complexconjugate(Rl6x6)*complexconjugate(Ru2x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x262}')

I97x265 = Parameter(name = 'I97x265',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd2x6*complexconjugate(Rl6x6)*complexconjugate(Ru5x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x265}')

I97x266 = Parameter(name = 'I97x266',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd2x6*complexconjugate(Rl6x6)*complexconjugate(Ru6x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x266}')

I97x311 = Parameter(name = 'I97x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*Rd3x5*complexconjugate(Rl1x6)*complexconjugate(Ru1x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x311}')

I97x312 = Parameter(name = 'I97x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*Rd3x5*complexconjugate(Rl1x6)*complexconjugate(Ru2x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x312}')

I97x315 = Parameter(name = 'I97x315',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x2*Rd3x5*complexconjugate(Rl1x6)*complexconjugate(Ru5x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x315}')

I97x316 = Parameter(name = 'I97x316',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x2*Rd3x5*complexconjugate(Rl1x6)*complexconjugate(Ru6x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x316}')

I97x321 = Parameter(name = 'I97x321',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x2*Rd3x5*complexconjugate(Rl2x5)*complexconjugate(Ru1x3)*complexconjugate(ye2x2)',
                    texname = '\\text{I97x321}')

I97x322 = Parameter(name = 'I97x322',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x2*Rd3x5*complexconjugate(Rl2x5)*complexconjugate(Ru2x3)*complexconjugate(ye2x2)',
                    texname = '\\text{I97x322}')

I97x325 = Parameter(name = 'I97x325',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x2*Rd3x5*complexconjugate(Rl2x5)*complexconjugate(Ru5x1)*complexconjugate(ye2x2)',
                    texname = '\\text{I97x325}')

I97x326 = Parameter(name = 'I97x326',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x2*Rd3x5*complexconjugate(Rl2x5)*complexconjugate(Ru6x2)*complexconjugate(ye2x2)',
                    texname = '\\text{I97x326}')

I97x331 = Parameter(name = 'I97x331',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x2*Rd3x5*complexconjugate(Rl3x4)*complexconjugate(Ru1x3)*complexconjugate(ye1x1)',
                    texname = '\\text{I97x331}')

I97x332 = Parameter(name = 'I97x332',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x2*Rd3x5*complexconjugate(Rl3x4)*complexconjugate(Ru2x3)*complexconjugate(ye1x1)',
                    texname = '\\text{I97x332}')

I97x335 = Parameter(name = 'I97x335',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x2*Rd3x5*complexconjugate(Rl3x4)*complexconjugate(Ru5x1)*complexconjugate(ye1x1)',
                    texname = '\\text{I97x335}')

I97x336 = Parameter(name = 'I97x336',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x2*Rd3x5*complexconjugate(Rl3x4)*complexconjugate(Ru6x2)*complexconjugate(ye1x1)',
                    texname = '\\text{I97x336}')

I97x361 = Parameter(name = 'I97x361',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*Rd3x5*complexconjugate(Rl6x6)*complexconjugate(Ru1x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x361}')

I97x362 = Parameter(name = 'I97x362',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*Rd3x5*complexconjugate(Rl6x6)*complexconjugate(Ru2x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x362}')

I97x365 = Parameter(name = 'I97x365',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x2*Rd3x5*complexconjugate(Rl6x6)*complexconjugate(Ru5x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x365}')

I97x366 = Parameter(name = 'I97x366',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x2*Rd3x5*complexconjugate(Rl6x6)*complexconjugate(Ru6x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x366}')

I97x411 = Parameter(name = 'I97x411',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*Rd4x4*complexconjugate(Rl1x6)*complexconjugate(Ru1x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x411}')

I97x412 = Parameter(name = 'I97x412',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*Rd4x4*complexconjugate(Rl1x6)*complexconjugate(Ru2x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x412}')

I97x415 = Parameter(name = 'I97x415',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x1*Rd4x4*complexconjugate(Rl1x6)*complexconjugate(Ru5x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x415}')

I97x416 = Parameter(name = 'I97x416',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x1*Rd4x4*complexconjugate(Rl1x6)*complexconjugate(Ru6x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x416}')

I97x421 = Parameter(name = 'I97x421',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x1*Rd4x4*complexconjugate(Rl2x5)*complexconjugate(Ru1x3)*complexconjugate(ye2x2)',
                    texname = '\\text{I97x421}')

I97x422 = Parameter(name = 'I97x422',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x1*Rd4x4*complexconjugate(Rl2x5)*complexconjugate(Ru2x3)*complexconjugate(ye2x2)',
                    texname = '\\text{I97x422}')

I97x425 = Parameter(name = 'I97x425',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x1*Rd4x4*complexconjugate(Rl2x5)*complexconjugate(Ru5x1)*complexconjugate(ye2x2)',
                    texname = '\\text{I97x425}')

I97x426 = Parameter(name = 'I97x426',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x1*Rd4x4*complexconjugate(Rl2x5)*complexconjugate(Ru6x2)*complexconjugate(ye2x2)',
                    texname = '\\text{I97x426}')

I97x431 = Parameter(name = 'I97x431',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x1*Rd4x4*complexconjugate(Rl3x4)*complexconjugate(Ru1x3)*complexconjugate(ye1x1)',
                    texname = '\\text{I97x431}')

I97x432 = Parameter(name = 'I97x432',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x1*Rd4x4*complexconjugate(Rl3x4)*complexconjugate(Ru2x3)*complexconjugate(ye1x1)',
                    texname = '\\text{I97x432}')

I97x435 = Parameter(name = 'I97x435',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x1*Rd4x4*complexconjugate(Rl3x4)*complexconjugate(Ru5x1)*complexconjugate(ye1x1)',
                    texname = '\\text{I97x435}')

I97x436 = Parameter(name = 'I97x436',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x1*Rd4x4*complexconjugate(Rl3x4)*complexconjugate(Ru6x2)*complexconjugate(ye1x1)',
                    texname = '\\text{I97x436}')

I97x461 = Parameter(name = 'I97x461',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*Rd4x4*complexconjugate(Rl6x6)*complexconjugate(Ru1x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x461}')

I97x462 = Parameter(name = 'I97x462',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*Rd4x4*complexconjugate(Rl6x6)*complexconjugate(Ru2x3)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x462}')

I97x465 = Parameter(name = 'I97x465',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x1*Rd4x4*complexconjugate(Rl6x6)*complexconjugate(Ru5x1)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x465}')

I97x466 = Parameter(name = 'I97x466',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x1*Rd4x4*complexconjugate(Rl6x6)*complexconjugate(Ru6x2)*complexconjugate(ye3x3)',
                    texname = '\\text{I97x466}')

I98x111 = Parameter(name = 'I98x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd1x6*complexconjugate(Rl1x3)*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x111}')

I98x112 = Parameter(name = 'I98x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd1x6*complexconjugate(Rl1x3)*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x112}')

I98x113 = Parameter(name = 'I98x113',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd1x6*complexconjugate(Rl1x3)*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I98x113}')

I98x114 = Parameter(name = 'I98x114',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd1x6*complexconjugate(Rl1x3)*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I98x114}')

I98x141 = Parameter(name = 'I98x141',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd1x6*complexconjugate(Rl4x1)*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x141}')

I98x142 = Parameter(name = 'I98x142',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd1x6*complexconjugate(Rl4x1)*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x142}')

I98x143 = Parameter(name = 'I98x143',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x3*Rd1x6*complexconjugate(Rl4x1)*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I98x143}')

I98x144 = Parameter(name = 'I98x144',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x3*Rd1x6*complexconjugate(Rl4x1)*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I98x144}')

I98x151 = Parameter(name = 'I98x151',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd1x6*complexconjugate(Rl5x2)*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x151}')

I98x152 = Parameter(name = 'I98x152',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd1x6*complexconjugate(Rl5x2)*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x152}')

I98x153 = Parameter(name = 'I98x153',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x3*Rd1x6*complexconjugate(Rl5x2)*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I98x153}')

I98x154 = Parameter(name = 'I98x154',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x3*Rd1x6*complexconjugate(Rl5x2)*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I98x154}')

I98x161 = Parameter(name = 'I98x161',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd1x6*complexconjugate(Rl6x3)*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x161}')

I98x162 = Parameter(name = 'I98x162',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd1x6*complexconjugate(Rl6x3)*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x162}')

I98x163 = Parameter(name = 'I98x163',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd1x6*complexconjugate(Rl6x3)*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I98x163}')

I98x164 = Parameter(name = 'I98x164',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd1x6*complexconjugate(Rl6x3)*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I98x164}')

I98x211 = Parameter(name = 'I98x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd2x6*complexconjugate(Rl1x3)*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x211}')

I98x212 = Parameter(name = 'I98x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd2x6*complexconjugate(Rl1x3)*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x212}')

I98x213 = Parameter(name = 'I98x213',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd2x6*complexconjugate(Rl1x3)*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I98x213}')

I98x214 = Parameter(name = 'I98x214',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd2x6*complexconjugate(Rl1x3)*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I98x214}')

I98x241 = Parameter(name = 'I98x241',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd2x6*complexconjugate(Rl4x1)*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x241}')

I98x242 = Parameter(name = 'I98x242',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x3*Rd2x6*complexconjugate(Rl4x1)*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x242}')

I98x243 = Parameter(name = 'I98x243',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x3*Rd2x6*complexconjugate(Rl4x1)*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I98x243}')

I98x244 = Parameter(name = 'I98x244',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x3*Rd2x6*complexconjugate(Rl4x1)*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I98x244}')

I98x251 = Parameter(name = 'I98x251',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd2x6*complexconjugate(Rl5x2)*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x251}')

I98x252 = Parameter(name = 'I98x252',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x3*Rd2x6*complexconjugate(Rl5x2)*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x252}')

I98x253 = Parameter(name = 'I98x253',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x3*Rd2x6*complexconjugate(Rl5x2)*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I98x253}')

I98x254 = Parameter(name = 'I98x254',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x3*Rd2x6*complexconjugate(Rl5x2)*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I98x254}')

I98x261 = Parameter(name = 'I98x261',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd2x6*complexconjugate(Rl6x3)*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x261}')

I98x262 = Parameter(name = 'I98x262',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x3*Rd2x6*complexconjugate(Rl6x3)*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x262}')

I98x263 = Parameter(name = 'I98x263',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x3*Rd2x6*complexconjugate(Rl6x3)*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I98x263}')

I98x264 = Parameter(name = 'I98x264',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x3*Rd2x6*complexconjugate(Rl6x3)*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I98x264}')

I98x311 = Parameter(name = 'I98x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*Rd3x5*complexconjugate(Rl1x3)*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x311}')

I98x312 = Parameter(name = 'I98x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*Rd3x5*complexconjugate(Rl1x3)*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x312}')

I98x313 = Parameter(name = 'I98x313',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x2*Rd3x5*complexconjugate(Rl1x3)*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I98x313}')

I98x314 = Parameter(name = 'I98x314',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x2*Rd3x5*complexconjugate(Rl1x3)*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I98x314}')

I98x341 = Parameter(name = 'I98x341',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x2*Rd3x5*complexconjugate(Rl4x1)*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x341}')

I98x342 = Parameter(name = 'I98x342',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x2*Rd3x5*complexconjugate(Rl4x1)*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x342}')

I98x343 = Parameter(name = 'I98x343',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x2*Rd3x5*complexconjugate(Rl4x1)*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I98x343}')

I98x344 = Parameter(name = 'I98x344',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x2*Rd3x5*complexconjugate(Rl4x1)*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I98x344}')

I98x351 = Parameter(name = 'I98x351',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x2*Rd3x5*complexconjugate(Rl5x2)*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x351}')

I98x352 = Parameter(name = 'I98x352',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x2*Rd3x5*complexconjugate(Rl5x2)*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x352}')

I98x353 = Parameter(name = 'I98x353',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x2*Rd3x5*complexconjugate(Rl5x2)*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I98x353}')

I98x354 = Parameter(name = 'I98x354',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x2*Rd3x5*complexconjugate(Rl5x2)*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I98x354}')

I98x361 = Parameter(name = 'I98x361',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*Rd3x5*complexconjugate(Rl6x3)*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x361}')

I98x362 = Parameter(name = 'I98x362',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x2*Rd3x5*complexconjugate(Rl6x3)*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x362}')

I98x363 = Parameter(name = 'I98x363',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x2*Rd3x5*complexconjugate(Rl6x3)*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I98x363}')

I98x364 = Parameter(name = 'I98x364',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x2*Rd3x5*complexconjugate(Rl6x3)*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I98x364}')

I98x411 = Parameter(name = 'I98x411',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*Rd4x4*complexconjugate(Rl1x3)*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x411}')

I98x412 = Parameter(name = 'I98x412',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*Rd4x4*complexconjugate(Rl1x3)*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x412}')

I98x413 = Parameter(name = 'I98x413',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x1*Rd4x4*complexconjugate(Rl1x3)*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I98x413}')

I98x414 = Parameter(name = 'I98x414',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x1*Rd4x4*complexconjugate(Rl1x3)*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I98x414}')

I98x441 = Parameter(name = 'I98x441',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x1*Rd4x4*complexconjugate(Rl4x1)*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x441}')

I98x442 = Parameter(name = 'I98x442',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x3x1*Rd4x4*complexconjugate(Rl4x1)*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x442}')

I98x443 = Parameter(name = 'I98x443',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x2x1*Rd4x4*complexconjugate(Rl4x1)*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I98x443}')

I98x444 = Parameter(name = 'I98x444',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD1x1x1*Rd4x4*complexconjugate(Rl4x1)*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I98x444}')

I98x451 = Parameter(name = 'I98x451',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x1*Rd4x4*complexconjugate(Rl5x2)*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x451}')

I98x452 = Parameter(name = 'I98x452',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x3x1*Rd4x4*complexconjugate(Rl5x2)*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x452}')

I98x453 = Parameter(name = 'I98x453',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x2x1*Rd4x4*complexconjugate(Rl5x2)*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I98x453}')

I98x454 = Parameter(name = 'I98x454',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD2x1x1*Rd4x4*complexconjugate(Rl5x2)*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I98x454}')

I98x461 = Parameter(name = 'I98x461',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*Rd4x4*complexconjugate(Rl6x3)*complexconjugate(Ru1x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x461}')

I98x462 = Parameter(name = 'I98x462',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x3x1*Rd4x4*complexconjugate(Rl6x3)*complexconjugate(Ru2x6)*complexconjugate(yu3x3)',
                    texname = '\\text{I98x462}')

I98x463 = Parameter(name = 'I98x463',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x2x1*Rd4x4*complexconjugate(Rl6x3)*complexconjugate(Ru3x5)*complexconjugate(yu2x2)',
                    texname = '\\text{I98x463}')

I98x464 = Parameter(name = 'I98x464',
                    nature = 'internal',
                    type = 'complex',
                    value = 'LLQD3x1x1*Rd4x4*complexconjugate(Rl6x3)*complexconjugate(Ru4x4)*complexconjugate(yu1x1)',
                    texname = '\\text{I98x464}')

I99x111 = Parameter(name = 'I99x111',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD3x3x3*complexconjugate(Rl1x3)*complexconjugate(Ru1x3)',
                    texname = '\\text{I99x111}')

I99x112 = Parameter(name = 'I99x112',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD3x3x3*complexconjugate(Rl1x3)*complexconjugate(Ru2x3)',
                    texname = '\\text{I99x112}')

I99x115 = Parameter(name = 'I99x115',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD3x1x3*complexconjugate(Rl1x3)*complexconjugate(Ru5x1)',
                    texname = '\\text{I99x115}')

I99x116 = Parameter(name = 'I99x116',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD3x2x3*complexconjugate(Rl1x3)*complexconjugate(Ru6x2)',
                    texname = '\\text{I99x116}')

I99x141 = Parameter(name = 'I99x141',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD1x3x3*complexconjugate(Rl4x1)*complexconjugate(Ru1x3)',
                    texname = '\\text{I99x141}')

I99x142 = Parameter(name = 'I99x142',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD1x3x3*complexconjugate(Rl4x1)*complexconjugate(Ru2x3)',
                    texname = '\\text{I99x142}')

I99x145 = Parameter(name = 'I99x145',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD1x1x3*complexconjugate(Rl4x1)*complexconjugate(Ru5x1)',
                    texname = '\\text{I99x145}')

I99x146 = Parameter(name = 'I99x146',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD1x2x3*complexconjugate(Rl4x1)*complexconjugate(Ru6x2)',
                    texname = '\\text{I99x146}')

I99x151 = Parameter(name = 'I99x151',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD2x3x3*complexconjugate(Rl5x2)*complexconjugate(Ru1x3)',
                    texname = '\\text{I99x151}')

I99x152 = Parameter(name = 'I99x152',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD2x3x3*complexconjugate(Rl5x2)*complexconjugate(Ru2x3)',
                    texname = '\\text{I99x152}')

I99x155 = Parameter(name = 'I99x155',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD2x1x3*complexconjugate(Rl5x2)*complexconjugate(Ru5x1)',
                    texname = '\\text{I99x155}')

I99x156 = Parameter(name = 'I99x156',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD2x2x3*complexconjugate(Rl5x2)*complexconjugate(Ru6x2)',
                    texname = '\\text{I99x156}')

I99x161 = Parameter(name = 'I99x161',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD3x3x3*complexconjugate(Rl6x3)*complexconjugate(Ru1x3)',
                    texname = '\\text{I99x161}')

I99x162 = Parameter(name = 'I99x162',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD3x3x3*complexconjugate(Rl6x3)*complexconjugate(Ru2x3)',
                    texname = '\\text{I99x162}')

I99x165 = Parameter(name = 'I99x165',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD3x1x3*complexconjugate(Rl6x3)*complexconjugate(Ru5x1)',
                    texname = '\\text{I99x165}')

I99x166 = Parameter(name = 'I99x166',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd1x6*TLQD3x2x3*complexconjugate(Rl6x3)*complexconjugate(Ru6x2)',
                    texname = '\\text{I99x166}')

I99x211 = Parameter(name = 'I99x211',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD3x3x3*complexconjugate(Rl1x3)*complexconjugate(Ru1x3)',
                    texname = '\\text{I99x211}')

I99x212 = Parameter(name = 'I99x212',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD3x3x3*complexconjugate(Rl1x3)*complexconjugate(Ru2x3)',
                    texname = '\\text{I99x212}')

I99x215 = Parameter(name = 'I99x215',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD3x1x3*complexconjugate(Rl1x3)*complexconjugate(Ru5x1)',
                    texname = '\\text{I99x215}')

I99x216 = Parameter(name = 'I99x216',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD3x2x3*complexconjugate(Rl1x3)*complexconjugate(Ru6x2)',
                    texname = '\\text{I99x216}')

I99x241 = Parameter(name = 'I99x241',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD1x3x3*complexconjugate(Rl4x1)*complexconjugate(Ru1x3)',
                    texname = '\\text{I99x241}')

I99x242 = Parameter(name = 'I99x242',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD1x3x3*complexconjugate(Rl4x1)*complexconjugate(Ru2x3)',
                    texname = '\\text{I99x242}')

I99x245 = Parameter(name = 'I99x245',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD1x1x3*complexconjugate(Rl4x1)*complexconjugate(Ru5x1)',
                    texname = '\\text{I99x245}')

I99x246 = Parameter(name = 'I99x246',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD1x2x3*complexconjugate(Rl4x1)*complexconjugate(Ru6x2)',
                    texname = '\\text{I99x246}')

I99x251 = Parameter(name = 'I99x251',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD2x3x3*complexconjugate(Rl5x2)*complexconjugate(Ru1x3)',
                    texname = '\\text{I99x251}')

I99x252 = Parameter(name = 'I99x252',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD2x3x3*complexconjugate(Rl5x2)*complexconjugate(Ru2x3)',
                    texname = '\\text{I99x252}')

I99x255 = Parameter(name = 'I99x255',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD2x1x3*complexconjugate(Rl5x2)*complexconjugate(Ru5x1)',
                    texname = '\\text{I99x255}')

I99x256 = Parameter(name = 'I99x256',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD2x2x3*complexconjugate(Rl5x2)*complexconjugate(Ru6x2)',
                    texname = '\\text{I99x256}')

I99x261 = Parameter(name = 'I99x261',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD3x3x3*complexconjugate(Rl6x3)*complexconjugate(Ru1x3)',
                    texname = '\\text{I99x261}')

I99x262 = Parameter(name = 'I99x262',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD3x3x3*complexconjugate(Rl6x3)*complexconjugate(Ru2x3)',
                    texname = '\\text{I99x262}')

I99x265 = Parameter(name = 'I99x265',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD3x1x3*complexconjugate(Rl6x3)*complexconjugate(Ru5x1)',
                    texname = '\\text{I99x265}')

I99x266 = Parameter(name = 'I99x266',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd2x6*TLQD3x2x3*complexconjugate(Rl6x3)*complexconjugate(Ru6x2)',
                    texname = '\\text{I99x266}')

I99x311 = Parameter(name = 'I99x311',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD3x3x2*complexconjugate(Rl1x3)*complexconjugate(Ru1x3)',
                    texname = '\\text{I99x311}')

I99x312 = Parameter(name = 'I99x312',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD3x3x2*complexconjugate(Rl1x3)*complexconjugate(Ru2x3)',
                    texname = '\\text{I99x312}')

I99x315 = Parameter(name = 'I99x315',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD3x1x2*complexconjugate(Rl1x3)*complexconjugate(Ru5x1)',
                    texname = '\\text{I99x315}')

I99x316 = Parameter(name = 'I99x316',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD3x2x2*complexconjugate(Rl1x3)*complexconjugate(Ru6x2)',
                    texname = '\\text{I99x316}')

I99x341 = Parameter(name = 'I99x341',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD1x3x2*complexconjugate(Rl4x1)*complexconjugate(Ru1x3)',
                    texname = '\\text{I99x341}')

I99x342 = Parameter(name = 'I99x342',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD1x3x2*complexconjugate(Rl4x1)*complexconjugate(Ru2x3)',
                    texname = '\\text{I99x342}')

I99x345 = Parameter(name = 'I99x345',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD1x1x2*complexconjugate(Rl4x1)*complexconjugate(Ru5x1)',
                    texname = '\\text{I99x345}')

I99x346 = Parameter(name = 'I99x346',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD1x2x2*complexconjugate(Rl4x1)*complexconjugate(Ru6x2)',
                    texname = '\\text{I99x346}')

I99x351 = Parameter(name = 'I99x351',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD2x3x2*complexconjugate(Rl5x2)*complexconjugate(Ru1x3)',
                    texname = '\\text{I99x351}')

I99x352 = Parameter(name = 'I99x352',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD2x3x2*complexconjugate(Rl5x2)*complexconjugate(Ru2x3)',
                    texname = '\\text{I99x352}')

I99x355 = Parameter(name = 'I99x355',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD2x1x2*complexconjugate(Rl5x2)*complexconjugate(Ru5x1)',
                    texname = '\\text{I99x355}')

I99x356 = Parameter(name = 'I99x356',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD2x2x2*complexconjugate(Rl5x2)*complexconjugate(Ru6x2)',
                    texname = '\\text{I99x356}')

I99x361 = Parameter(name = 'I99x361',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD3x3x2*complexconjugate(Rl6x3)*complexconjugate(Ru1x3)',
                    texname = '\\text{I99x361}')

I99x362 = Parameter(name = 'I99x362',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD3x3x2*complexconjugate(Rl6x3)*complexconjugate(Ru2x3)',
                    texname = '\\text{I99x362}')

I99x365 = Parameter(name = 'I99x365',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD3x1x2*complexconjugate(Rl6x3)*complexconjugate(Ru5x1)',
                    texname = '\\text{I99x365}')

I99x366 = Parameter(name = 'I99x366',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd3x5*TLQD3x2x2*complexconjugate(Rl6x3)*complexconjugate(Ru6x2)',
                    texname = '\\text{I99x366}')

I99x411 = Parameter(name = 'I99x411',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD3x3x1*complexconjugate(Rl1x3)*complexconjugate(Ru1x3)',
                    texname = '\\text{I99x411}')

I99x412 = Parameter(name = 'I99x412',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD3x3x1*complexconjugate(Rl1x3)*complexconjugate(Ru2x3)',
                    texname = '\\text{I99x412}')

I99x415 = Parameter(name = 'I99x415',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD3x1x1*complexconjugate(Rl1x3)*complexconjugate(Ru5x1)',
                    texname = '\\text{I99x415}')

I99x416 = Parameter(name = 'I99x416',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD3x2x1*complexconjugate(Rl1x3)*complexconjugate(Ru6x2)',
                    texname = '\\text{I99x416}')

I99x441 = Parameter(name = 'I99x441',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD1x3x1*complexconjugate(Rl4x1)*complexconjugate(Ru1x3)',
                    texname = '\\text{I99x441}')

I99x442 = Parameter(name = 'I99x442',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD1x3x1*complexconjugate(Rl4x1)*complexconjugate(Ru2x3)',
                    texname = '\\text{I99x442}')

I99x445 = Parameter(name = 'I99x445',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD1x1x1*complexconjugate(Rl4x1)*complexconjugate(Ru5x1)',
                    texname = '\\text{I99x445}')

I99x446 = Parameter(name = 'I99x446',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD1x2x1*complexconjugate(Rl4x1)*complexconjugate(Ru6x2)',
                    texname = '\\text{I99x446}')

I99x451 = Parameter(name = 'I99x451',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD2x3x1*complexconjugate(Rl5x2)*complexconjugate(Ru1x3)',
                    texname = '\\text{I99x451}')

I99x452 = Parameter(name = 'I99x452',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD2x3x1*complexconjugate(Rl5x2)*complexconjugate(Ru2x3)',
                    texname = '\\text{I99x452}')

I99x455 = Parameter(name = 'I99x455',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD2x1x1*complexconjugate(Rl5x2)*complexconjugate(Ru5x1)',
                    texname = '\\text{I99x455}')

I99x456 = Parameter(name = 'I99x456',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD2x2x1*complexconjugate(Rl5x2)*complexconjugate(Ru6x2)',
                    texname = '\\text{I99x456}')

I99x461 = Parameter(name = 'I99x461',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD3x3x1*complexconjugate(Rl6x3)*complexconjugate(Ru1x3)',
                    texname = '\\text{I99x461}')

I99x462 = Parameter(name = 'I99x462',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD3x3x1*complexconjugate(Rl6x3)*complexconjugate(Ru2x3)',
                    texname = '\\text{I99x462}')

I99x465 = Parameter(name = 'I99x465',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD3x1x1*complexconjugate(Rl6x3)*complexconjugate(Ru5x1)',
                    texname = '\\text{I99x465}')

I99x466 = Parameter(name = 'I99x466',
                    nature = 'internal',
                    type = 'complex',
                    value = 'Rd4x4*TLQD3x2x1*complexconjugate(Rl6x3)*complexconjugate(Ru6x2)',
                    texname = '\\text{I99x466}')


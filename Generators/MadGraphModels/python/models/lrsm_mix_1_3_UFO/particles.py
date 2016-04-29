# This file was automatically created by FeynRules 2.3.1
# Mathematica version: 10.1.0  for Mac OS X x86 (64-bit) (March 24, 2015)
# Date: Wed 27 May 2015 19:49:11


from __future__ import division
from object_library import all_particles, Particle
import parameters as Param

import propagators as Prop

a = Particle(pdg_code = 22,
             name = 'a',
             antiname = 'a',
             spin = 3,
             color = 1,
             mass = Param.ZERO,
             width = Param.ZERO,
             texname = 'a',
             antitexname = 'a',
             charge = 0,
             GhostNumber = 0,
             LeptonNumber = 0,
             Y = 0)

Z = Particle(pdg_code = 23,
             name = 'Z',
             antiname = 'Z',
             spin = 3,
             color = 1,
             mass = Param.MZ,
             width = Param.WZ,
             texname = 'Z',
             antitexname = 'Z',
             charge = 0,
             GhostNumber = 0,
             LeptonNumber = 0,
             Y = 0)

W__plus__ = Particle(pdg_code = 24,
                     name = 'W+',
                     antiname = 'W-',
                     spin = 3,
                     color = 1,
                     mass = Param.MW,
                     width = Param.WW,
                     texname = 'W+',
                     antitexname = 'W-',
                     charge = 1,
                     GhostNumber = 0,
                     LeptonNumber = 0,
                     Y = 0)

W__minus__ = W__plus__.anti()

g = Particle(pdg_code = 21,
             name = 'g',
             antiname = 'g',
             spin = 3,
             color = 8,
             mass = Param.ZERO,
             width = Param.ZERO,
             texname = 'g',
             antitexname = 'g',
             charge = 0,
             GhostNumber = 0,
             LeptonNumber = 0,
             Y = 0)

W2__plus__ = Particle(pdg_code = 34,
                      name = 'W2+',
                      antiname = 'W2-',
                      spin = 3,
                      color = 1,
                      mass = Param.MW2,
                      width = Param.WW2,
                      texname = 'W2+',
                      antitexname = 'W2-',
                      charge = 1,
                      GhostNumber = 0,
                      LeptonNumber = 0,
                      Y = 0)

W2__minus__ = W2__plus__.anti()

Z2 = Particle(pdg_code = 32,
              name = 'Z2',
              antiname = 'Z2',
              spin = 3,
              color = 1,
              mass = Param.MZ2,
              width = Param.WZ2,
              texname = 'Z2',
              antitexname = 'Z2',
              charge = 0,
              GhostNumber = 0,
              LeptonNumber = 0,
              Y = 0)

e__minus__ = Particle(pdg_code = 11,
                      name = 'e-',
                      antiname = 'e+',
                      spin = 2,
                      color = 1,
                      mass = Param.Me,
                      width = Param.ZERO,
                      texname = 'e-',
                      antitexname = 'e+',
                      charge = -1,
                      GhostNumber = 0,
                      LeptonNumber = 1,
                      Y = 0)

e__plus__ = e__minus__.anti()

mu__minus__ = Particle(pdg_code = 13,
                       name = 'mu-',
                       antiname = 'mu+',
                       spin = 2,
                       color = 1,
                       mass = Param.Mmu,
                       width = Param.ZERO,
                       texname = 'mu-',
                       antitexname = 'mu+',
                       charge = -1,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

mu__plus__ = mu__minus__.anti()

ta__minus__ = Particle(pdg_code = 15,
                       name = 'ta-',
                       antiname = 'ta+',
                       spin = 2,
                       color = 1,
                       mass = Param.Mta,
                       width = Param.ZERO,
                       texname = 'ta-',
                       antitexname = 'ta+',
                       charge = -1,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

ta__plus__ = ta__minus__.anti()

u = Particle(pdg_code = 2,
             name = 'u',
             antiname = 'u~',
             spin = 2,
             color = 3,
             mass = Param.MU,
             width = Param.ZERO,
             texname = 'u',
             antitexname = 'u~',
             charge = 2/3,
             GhostNumber = 0,
             LeptonNumber = 0,
             Y = 0)

u__tilde__ = u.anti()

c = Particle(pdg_code = 4,
             name = 'c',
             antiname = 'c~',
             spin = 2,
             color = 3,
             mass = Param.MC,
             width = Param.ZERO,
             texname = 'c',
             antitexname = 'c~',
             charge = 2/3,
             GhostNumber = 0,
             LeptonNumber = 0,
             Y = 0)

c__tilde__ = c.anti()

t = Particle(pdg_code = 6,
             name = 't',
             antiname = 't~',
             spin = 2,
             color = 3,
             mass = Param.MT,
             width = Param.WT,
             texname = 't',
             antitexname = 't~',
             charge = 2/3,
             GhostNumber = 0,
             LeptonNumber = 0,
             Y = 0)

t__tilde__ = t.anti()

d = Particle(pdg_code = 1,
             name = 'd',
             antiname = 'd~',
             spin = 2,
             color = 3,
             mass = Param.MD,
             width = Param.ZERO,
             texname = 'd',
             antitexname = 'd~',
             charge = -1/3,
             GhostNumber = 0,
             LeptonNumber = 0,
             Y = 0)

d__tilde__ = d.anti()

s = Particle(pdg_code = 3,
             name = 's',
             antiname = 's~',
             spin = 2,
             color = 3,
             mass = Param.MS,
             width = Param.ZERO,
             texname = 's',
             antitexname = 's~',
             charge = -1/3,
             GhostNumber = 0,
             LeptonNumber = 0,
             Y = 0)

s__tilde__ = s.anti()

b = Particle(pdg_code = 5,
             name = 'b',
             antiname = 'b~',
             spin = 2,
             color = 3,
             mass = Param.MB,
             width = Param.ZERO,
             texname = 'b',
             antitexname = 'b~',
             charge = -1/3,
             GhostNumber = 0,
             LeptonNumber = 0,
             Y = 0)

b__tilde__ = b.anti()

v1 = Particle(pdg_code = 12,
              name = 'v1',
              antiname = 'v1',
              spin = 2,
              color = 1,
              mass = Param.Mv1,
              width = Param.ZERO,
              texname = 'v1',
              antitexname = 'v1',
              charge = 0,
              GhostNumber = 0,
              LeptonNumber = 0,
              Y = 0)

v2 = Particle(pdg_code = 14,
              name = 'v2',
              antiname = 'v2',
              spin = 2,
              color = 1,
              mass = Param.Mv2,
              width = Param.ZERO,
              texname = 'v2',
              antitexname = 'v2',
              charge = 0,
              GhostNumber = 0,
              LeptonNumber = 0,
              Y = 0)

v3 = Particle(pdg_code = 16,
              name = 'v3',
              antiname = 'v3',
              spin = 2,
              color = 1,
              mass = Param.Mv3,
              width = Param.ZERO,
              texname = 'v3',
              antitexname = 'v3',
              charge = 0,
              GhostNumber = 0,
              LeptonNumber = 0,
              Y = 0)

N1 = Particle(pdg_code = 9900012,
              name = 'N1',
              antiname = 'N1',
              spin = 2,
              color = 1,
              mass = Param.MN1,
              width = Param.WN1,
              texname = 'N1',
              antitexname = 'N1',
              charge = 0,
              GhostNumber = 0,
              LeptonNumber = 0,
              Y = 0)

N2 = Particle(pdg_code = 9900014,
              name = 'N2',
              antiname = 'N2',
              spin = 2,
              color = 1,
              mass = Param.MN2,
              width = Param.WN2,
              texname = 'N2',
              antitexname = 'N2',
              charge = 0,
              GhostNumber = 0,
              LeptonNumber = 0,
              Y = 0)

N3 = Particle(pdg_code = 9900016,
              name = 'N3',
              antiname = 'N3',
              spin = 2,
              color = 1,
              mass = Param.MN3,
              width = Param.WN3,
              texname = 'N3',
              antitexname = 'N3',
              charge = 0,
              GhostNumber = 0,
              LeptonNumber = 0,
              Y = 0)

H = Particle(pdg_code = 25,
             name = 'H',
             antiname = 'H',
             spin = 1,
             color = 1,
             mass = Param.MH,
             width = Param.WH,
             texname = 'H',
             antitexname = 'H',
             charge = 0,
             GhostNumber = 0,
             LeptonNumber = 0,
             Y = 0)

H01 = Particle(pdg_code = 35,
               name = 'H01',
               antiname = 'H01',
               spin = 1,
               color = 1,
               mass = Param.MH01,
               width = Param.WH01,
               texname = 'H01',
               antitexname = 'H01',
               charge = 0,
               GhostNumber = 0,
               LeptonNumber = 0,
               Y = 0)

H02 = Particle(pdg_code = 43,
               name = 'H02',
               antiname = 'H02',
               spin = 1,
               color = 1,
               mass = Param.MH02,
               width = Param.WH02,
               texname = 'H02',
               antitexname = 'H02',
               charge = 0,
               GhostNumber = 0,
               LeptonNumber = 0,
               Y = 0)

H03 = Particle(pdg_code = 44,
               name = 'H03',
               antiname = 'H03',
               spin = 1,
               color = 1,
               mass = Param.MH03,
               width = Param.WH03,
               texname = 'H03',
               antitexname = 'H03',
               charge = 0,
               GhostNumber = 0,
               LeptonNumber = 0,
               Y = 0)

HP1 = Particle(pdg_code = 37,
               name = 'HP1',
               antiname = 'HM1',
               spin = 1,
               color = 1,
               mass = Param.MHP1,
               width = Param.WHP1,
               texname = 'HP1',
               antitexname = 'HM1',
               charge = 1,
               GhostNumber = 0,
               LeptonNumber = 0,
               Y = 0)

HM1 = HP1.anti()

HP2 = Particle(pdg_code = 38,
               name = 'HP2',
               antiname = 'HM2',
               spin = 1,
               color = 1,
               mass = Param.MHP2,
               width = Param.WHP2,
               texname = 'HP2',
               antitexname = 'HM2',
               charge = 1,
               GhostNumber = 0,
               LeptonNumber = 0,
               Y = 0)

HM2 = HP2.anti()

HL__plus____plus__ = Particle(pdg_code = 61,
                              name = 'HL++',
                              antiname = 'HL--',
                              spin = 1,
                              color = 1,
                              mass = Param.MHPPL,
                              width = Param.WHPPL,
                              texname = 'HL++',
                              antitexname = 'HL--',
                              charge = 2,
                              GhostNumber = 0,
                              LeptonNumber = 0,
                              Y = 0)

HL__minus____minus__ = HL__plus____plus__.anti()

HR__plus____plus__ = Particle(pdg_code = 62,
                              name = 'HR++',
                              antiname = 'HR--',
                              spin = 1,
                              color = 1,
                              mass = Param.MHPPR,
                              width = Param.WHPPR,
                              texname = 'HR++',
                              antitexname = 'HR--',
                              charge = 2,
                              GhostNumber = 0,
                              LeptonNumber = 0,
                              Y = 0)

HR__minus____minus__ = HR__plus____plus__.anti()

A01 = Particle(pdg_code = 36,
               name = 'A01',
               antiname = 'A01',
               spin = 1,
               color = 1,
               mass = Param.MA01,
               width = Param.WA01,
               texname = 'A01',
               antitexname = 'A01',
               charge = 0,
               GhostNumber = 0,
               LeptonNumber = 0,
               Y = 0)

A02 = Particle(pdg_code = 45,
               name = 'A02',
               antiname = 'A02',
               spin = 1,
               color = 1,
               mass = Param.MA02,
               width = Param.WA02,
               texname = 'A02',
               antitexname = 'A02',
               charge = 0,
               GhostNumber = 0,
               LeptonNumber = 0,
               Y = 0)


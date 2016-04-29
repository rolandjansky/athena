# This file was automatically created by FeynRules 2.0.8
# Mathematica version: 9.0 for Mac OS X x86 (64-bit) (January 24, 2013)
# Date: Thu 16 Jan 2014 15:28:43


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

v1 = Particle(pdg_code = 9000012,
              name = 'v1',
              antiname = 'v1',
              spin = 2,
              color = 1,
              mass = Param.ZERO,
              width = Param.ZERO,
              texname = 'v1',
              antitexname = 'v1',
              charge = 0,
              GhostNumber = 0,
              LeptonNumber = 0,
              Y = 0)

v2 = Particle(pdg_code = 9000014,
              name = 'v2',
              antiname = 'v2',
              spin = 2,
              color = 1,
              mass = Param.ZERO,
              width = Param.ZERO,
              texname = 'v2',
              antitexname = 'v2',
              charge = 0,
              GhostNumber = 0,
              LeptonNumber = 0,
              Y = 0)

v3 = Particle(pdg_code = 9000016,
              name = 'v3',
              antiname = 'v3',
              spin = 2,
              color = 1,
              mass = Param.ZERO,
              width = Param.ZERO,
              texname = 'v3',
              antitexname = 'v3',
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
                       mass = Param.MM,
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
                       mass = Param.MTA,
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

H2 = Particle(pdg_code = 35,
              name = 'H2',
              antiname = 'H2',
              spin = 1,
              color = 1,
              mass = Param.MH2,
              width = Param.WH2,
              texname = 'H2',
              antitexname = 'H2',
              charge = 0,
              GhostNumber = 0,
              LeptonNumber = 0,
              Y = 0)

A0 = Particle(pdg_code = 36,
              name = 'A0',
              antiname = 'A0',
              spin = 1,
              color = 1,
              mass = Param.MA0,
              width = Param.WA0,
              texname = 'A0',
              antitexname = 'A0',
              charge = 0,
              GhostNumber = 0,
              LeptonNumber = 0,
              Y = 0)

HPlus = Particle(pdg_code = 37,
                 name = 'HPlus',
                 antiname = 'HMinus',
                 spin = 1,
                 color = 1,
                 mass = Param.MHP,
                 width = Param.WHP,
                 texname = 'HPlus',
                 antitexname = 'HMinus',
                 charge = 1,
                 GhostNumber = 0,
                 LeptonNumber = 0,
                 Y = 0)

HMinus = HPlus.anti()

H__plus____plus__ = Particle(pdg_code = 9900041,
                             name = 'H++',
                             antiname = 'H--',
                             spin = 1,
                             color = 1,
                             mass = Param.MH2P,
                             width = Param.WH2P,
                             texname = 'H++',
                             antitexname = 'H--',
                             charge = 2,
                             GhostNumber = 0,
                             LeptonNumber = 0,
                             Y = 0)

H__minus____minus__ = H__plus____plus__.anti()


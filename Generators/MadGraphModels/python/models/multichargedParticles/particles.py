# This file was automatically created by FeynRules 1.7.69
# Mathematica version: 8.0 for Mac OS X x86 (64-bit) (November 6, 2010)
# Date: Mon 1 Oct 2012 14:58:25


from __future__ import division
from object_library import all_particles, Particle
import parameters as Param

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

ghA = Particle(pdg_code = 9000001,
               name = 'ghA',
               antiname = 'ghA~',
               spin = -1,
               color = 1,
               mass = Param.ZERO,
               width = Param.ZERO,
               texname = 'ghA',
               antitexname = 'ghA~',
               charge = 0,
               GhostNumber = 1,
               LeptonNumber = 0,
               Y = 0)

ghA__tilde__ = ghA.anti()

ghZ = Particle(pdg_code = 9000002,
               name = 'ghZ',
               antiname = 'ghZ~',
               spin = -1,
               color = 1,
               mass = Param.MZ,
               width = Param.WZ,
               texname = 'ghZ',
               antitexname = 'ghZ~',
               charge = 0,
               GhostNumber = 1,
               LeptonNumber = 0,
               Y = 0)

ghZ__tilde__ = ghZ.anti()

ghWp = Particle(pdg_code = 9000003,
                name = 'ghWp',
                antiname = 'ghWp~',
                spin = -1,
                color = 1,
                mass = Param.MW,
                width = Param.WW,
                texname = 'ghWp',
                antitexname = 'ghWp~',
                charge = 1,
                GhostNumber = 1,
                LeptonNumber = 0,
                Y = 0)

ghWp__tilde__ = ghWp.anti()

ghWm = Particle(pdg_code = 9000004,
                name = 'ghWm',
                antiname = 'ghWm~',
                spin = -1,
                color = 1,
                mass = Param.MW,
                width = Param.WW,
                texname = 'ghWm',
                antitexname = 'ghWm~',
                charge = -1,
                GhostNumber = 1,
                LeptonNumber = 0,
                Y = 0)

ghWm__tilde__ = ghWm.anti()

ghG = Particle(pdg_code = 9000005,
               name = 'ghG',
               antiname = 'ghG~',
               spin = -1,
               color = 8,
               mass = Param.ZERO,
               width = Param.ZERO,
               texname = 'ghG',
               antitexname = 'ghG~',
               charge = 0,
               GhostNumber = 1,
               LeptonNumber = 0,
               Y = 0)

ghG__tilde__ = ghG.anti()

ve = Particle(pdg_code = 12,
              name = 've',
              antiname = 've~',
              spin = 2,
              color = 1,
              mass = Param.ZERO,
              width = Param.ZERO,
              texname = 've',
              antitexname = 've~',
              charge = 0,
              GhostNumber = 0,
              LeptonNumber = 1,
              Y = 0)

ve__tilde__ = ve.anti()

vm = Particle(pdg_code = 14,
              name = 'vm',
              antiname = 'vm~',
              spin = 2,
              color = 1,
              mass = Param.ZERO,
              width = Param.ZERO,
              texname = 'vm',
              antitexname = 'vm~',
              charge = 0,
              GhostNumber = 0,
              LeptonNumber = 1,
              Y = 0)

vm__tilde__ = vm.anti()

vt = Particle(pdg_code = 16,
              name = 'vt',
              antiname = 'vt~',
              spin = 2,
              color = 1,
              mass = Param.ZERO,
              width = Param.ZERO,
              texname = 'vt',
              antitexname = 'vt~',
              charge = 0,
              GhostNumber = 0,
              LeptonNumber = 1,
              Y = 0)

vt__tilde__ = vt.anti()

u = Particle(pdg_code = 2,
             name = 'u',
             antiname = 'u~',
             spin = 2,
             color = 3,
             mass = Param.ZERO,
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
             mass = Param.ZERO,
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
             mass = Param.ZERO,
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

G0 = Particle(pdg_code = 250,
              name = 'G0',
              antiname = 'G0',
              spin = 1,
              color = 1,
              mass = Param.MZ,
              width = Param.WZ,
              texname = 'G0',
              antitexname = 'G0',
              GoldstoneBoson = True,
              charge = 0,
              GhostNumber = 0,
              LeptonNumber = 0,
              Y = 0)

G__plus__ = Particle(pdg_code = 251,
                     name = 'G+',
                     antiname = 'G-',
                     spin = 1,
                     color = 1,
                     mass = Param.MW,
                     width = Param.WW,
                     texname = 'G+',
                     antitexname = 'G-',
                     GoldstoneBoson = True,
                     charge = 1,
                     GhostNumber = 0,
                     LeptonNumber = 0,
                     Y = 0)

G__minus__ = G__plus__.anti()

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
                       width = Param.WTau,
                       texname = 'ta-',
                       antitexname = 'ta+',
                       charge = -1,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

ta__plus__ = ta__minus__.anti()

qb0p3__minus__ = Particle(pdg_code = 10000030,
                       name = 'qb0p3',
                       antiname = 'qb0p3~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb0p3,
                       width = Param.ZERO,
                       texname = 'qb0p3+',
                       antitexname = 'qb0p3-',
                       charge = 0.3,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb0p3__plus__ = qb0p3__minus__.anti()

qb0p6__minus__ = Particle(pdg_code = 10000060,
                       name = 'qb0p6',
                       antiname = 'qb0p6~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb0p6,
                       width = Param.ZERO,
                       texname = 'qb0p6+',
                       antitexname = 'qb0p6-',
                       charge = 0.6,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb0p6__plus__ = qb0p6__minus__.anti()

qb1p5__minus__ = Particle(pdg_code = 10000150,
                       name = 'qb1p5',
                       antiname = 'qb1p5~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb1p5,
                       width = Param.ZERO,
                       texname = 'qb1p5+',
                       antitexname = 'qb1p5-',
                       charge = 1.5,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb1p5__plus__ = qb1p5__minus__.anti()

qb2p0__minus__ = Particle(pdg_code = 10000200,
                       name = 'qb2p0',
                       antiname = 'qb2p0~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb2p0,
                       width = Param.ZERO,
                       texname = 'qb2p0+',
                       antitexname = 'qb2p0-',
                       charge = 2.0,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb2p0__plus__ = qb2p0__minus__.anti()

qb2p5__minus__ = Particle(pdg_code = 10000250,
                       name = 'qb2p5',
                       antiname = 'qb2p5~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb2p5,
                       width = Param.ZERO,
                       texname = 'qb2p5+',
                       antitexname = 'qb2p5-',
                       charge = 2.5,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb2p5__plus__ = qb2p5__minus__.anti()

qb3p0__minus__ = Particle(pdg_code = 10000300,
                       name = 'qb3p0',
                       antiname = 'qb3p0~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb3p0,
                       width = Param.ZERO,
                       texname = 'qb3p0+',
                       antitexname = 'qb3p0-',
                       charge = 3.0,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb3p0__plus__ = qb3p0__minus__.anti()

qb3p5__minus__ = Particle(pdg_code = 10000350,
                       name = 'qb3p5',
                       antiname = 'qb3p5~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb3p5,
                       width = Param.ZERO,
                       texname = 'qb3p5+',
                       antitexname = 'qb3p5-',
                       charge = 3.5,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb3p5__plus__ = qb3p5__minus__.anti()

qb4p0__minus__ = Particle(pdg_code = 10000400,
                       name = 'qb4p0',
                       antiname = 'qb4p0~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb4p0,
                       width = Param.ZERO,
                       texname = 'qb4p0+',
                       antitexname = 'qb4p0-',
                       charge = 4.0,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb4p0__plus__ = qb4p0__minus__.anti()

qb4p5__minus__ = Particle(pdg_code = 10000450,
                       name = 'qb4p5',
                       antiname = 'qb4p5~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb4p5,
                       width = Param.ZERO,
                       texname = 'qb4p5+',
                       antitexname = 'qb4p5-',
                       charge = 4.5,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb4p5__plus__ = qb4p5__minus__.anti()

qb5p0__minus__ = Particle(pdg_code = 10000500,
                       name = 'qb5p0',
                       antiname = 'qb5p0~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb5p0,
                       width = Param.ZERO,
                       texname = 'qb5p0+',
                       antitexname = 'qb5p0-',
                       charge = 5.0,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb5p0__plus__ = qb5p0__minus__.anti()

qb5p5__minus__ = Particle(pdg_code = 10000550,
                       name = 'qb5p5',
                       antiname = 'qb5p5~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb5p5,
                       width = Param.ZERO,
                       texname = 'qb5p5+',
                       antitexname = 'qb5p5-',
                       charge = 5.5,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb5p5__plus__ = qb5p5__minus__.anti()

qb6p0__minus__ = Particle(pdg_code = 10000600,
                       name = 'qb6p0',
                       antiname = 'qb6p0~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb6p0,
                       width = Param.ZERO,
                       texname = 'qb6p0+',
                       antitexname = 'qb6p0-',
                       charge = 6.0,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb6p0__plus__ = qb6p0__minus__.anti()

qb6p5__minus__ = Particle(pdg_code = 10000650,
                       name = 'qb6p5',
                       antiname = 'qb6p5~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb6p5,
                       width = Param.ZERO,
                       texname = 'qb6p5+',
                       antitexname = 'qb6p5-',
                       charge = 6.5,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb6p5__plus__ = qb6p5__minus__.anti()

qb7p0__minus__ = Particle(pdg_code = 10000700,
                       name = 'qb7p0',
                       antiname = 'qb7p0~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb7p0,
                       width = Param.ZERO,
                       texname = 'qb7p0+',
                       antitexname = 'qb7p0-',
                       charge = 7.0,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb7p0__plus__ = qb7p0__minus__.anti()

qb7p5__minus__ = Particle(pdg_code = 10000750,
                       name = 'qb7p5',
                       antiname = 'qb7p5~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb7p5,
                       width = Param.ZERO,
                       texname = 'qb7p5+',
                       antitexname = 'qb7p5-',
                       charge = 7.5,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb7p5__plus__ = qb7p5__minus__.anti()

qb8p0__minus__ = Particle(pdg_code = 10000800,
                       name = 'qb8p0',
                       antiname = 'qb8p0~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb8p0,
                       width = Param.ZERO,
                       texname = 'qb8p0+',
                       antitexname = 'qb8p0-',
                       charge = 8.0,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb8p0__plus__ = qb8p0__minus__.anti()

qb8p5__minus__ = Particle(pdg_code = 10000850,
                       name = 'qb8p5',
                       antiname = 'qb8p5~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb8p5,
                       width = Param.ZERO,
                       texname = 'qb8p5+',
                       antitexname = 'qb8p5-',
                       charge = 8.5,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb8p5__plus__ = qb8p5__minus__.anti()

qb9p0__minus__ = Particle(pdg_code = 10000900,
                       name = 'qb9p0',
                       antiname = 'qb9p0~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb9p0,
                       width = Param.ZERO,
                       texname = 'qb9p0+',
                       antitexname = 'qb9p0-',
                       charge = 9.0,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb9p0__plus__ = qb9p0__minus__.anti()

qb9p5__minus__ = Particle(pdg_code = 10000950,
                       name = 'qb9p5',
                       antiname = 'qb9p5~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb9p5,
                       width = Param.ZERO,
                       texname = 'qb9p5+',
                       antitexname = 'qb9p5-',
                       charge = 9.5,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb9p5__plus__ = qb9p5__minus__.anti()

qb10p0__minus__ = Particle(pdg_code = 10001000,
                       name = 'qb10p0',
                       antiname = 'qb10p0~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb10p0,
                       width = Param.ZERO,
                       texname = 'qb10p0+',
                       antitexname = 'qb10p0-',
                       charge = 10.0,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb10p0__plus__ = qb10p0__minus__.anti()

qb20p0__minus__ = Particle(pdg_code = 10002000,
                       name = 'qb20p0',
                       antiname = 'qb20p0~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb20p0,
                       width = Param.ZERO,
                       texname = 'qb20p0+',
                       antitexname = 'qb20p0-',
                       charge = 20.0,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb20p0__plus__ = qb20p0__minus__.anti()

qb40p0__minus__ = Particle(pdg_code = 10004000,
                       name = 'qb40p0',
                       antiname = 'qb40p0~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb40p0,
                       width = Param.ZERO,
                       texname = 'qb40p0+',
                       antitexname = 'qb40p0-',
                       charge = 40.0,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb40p0__plus__ = qb40p0__minus__.anti()

qb60p0__minus__ = Particle(pdg_code = 10006000,
                       name = 'qb60p0',
                       antiname = 'qb60p0~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb60p0,
                       width = Param.ZERO,
                       texname = 'qb60p0+',
                       antitexname = 'qb60p0-',
                       charge = 60.0,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb60p0__plus__ = qb60p0__minus__.anti()

qb0p4__minus__ = Particle(pdg_code = 10000040,
                       name = 'qb0p4',
                       antiname = 'qb0p4~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb0p4,
                       width = Param.ZERO,
                       texname = 'qb0p4+',
                       antitexname = 'qb0p4-',
                       charge = 0.4,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb0p4__plus__ = qb0p4__minus__.anti()

qb0p7__minus__ = Particle(pdg_code = 10000070,
                       name = 'qb0p7',
                       antiname = 'qb0p7~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb0p7,
                       width = Param.ZERO,
                       texname = 'qb0p7+',
                       antitexname = 'qb0p7-',
                       charge = 0.7,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb0p7__plus__ = qb0p7__minus__.anti()

qb100p0__minus__ = Particle(pdg_code = 10010000,
                       name = 'qb100p0',
                       antiname = 'qb100p0~',
                       spin = 2,
                       color = 1,
                       mass = Param.MQb100p0,
                       width = Param.ZERO,
                       texname = 'qb100p0+',
                       antitexname = 'qb100p0-',
                       charge = 100.0,
                       GhostNumber = 0,
                       LeptonNumber = 1,
                       Y = 0)

qb100p0__plus__ = qb100p0__minus__.anti()


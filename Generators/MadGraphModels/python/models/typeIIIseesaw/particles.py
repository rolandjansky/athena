# This file was automatically created by FeynRules $Revision: 709 $
# Mathematica version: 8.0 for Linux x86 (64-bit) (February 23, 2011)
# Date: Tue 25 Oct 2011 09:50:07


from __future__ import division
from object_library import all_particles, Particle
import parameters as Param

v1 = Particle(pdg_code = 8000012,
              name = 'v1',
              antiname = 'v1',
              spin = 2,
              color = 1,
              mass = Param.ZERO,
              width = Param.ZERO,
              texname = 'v1',
              antitexname = 'v1',
              charge = 0,
              GhostNumber = 0)

v2 = Particle(pdg_code = 8000014,
              name = 'v2',
              antiname = 'v2',
              spin = 2,
              color = 1,
              mass = Param.ZERO,
              width = Param.ZERO,
              texname = 'v2',
              antitexname = 'v2',
              charge = 0,
              GhostNumber = 0)

v3 = Particle(pdg_code = 8000016,
              name = 'v3',
              antiname = 'v3',
              spin = 2,
              color = 1,
              mass = Param.ZERO,
              width = Param.ZERO,
              texname = 'v3',
              antitexname = 'v3',
              charge = 0,
              GhostNumber = 0)

tr0 = Particle(pdg_code = 8000018,
               name = 'tr0',
               antiname = 'tr0',
               spin = 2,
               color = 1,
               mass = Param.Mtr0,
               width = Param.Wtr0,
               texname = 'tr0',
               antitexname = 'tr0',
               charge = 0,
               GhostNumber = 0)

e__minus__ = Particle(pdg_code = 11,
                      name = 'e-',
                      antiname = 'e+',
                      spin = 2,
                      color = 1,
                      mass = Param.Me,
                      width = Param.ZERO,
                      texname = 'e-',
                      antitexname = 'e-',
                      charge = -1,
                      GhostNumber = 0)

e__plus__ = e__minus__.anti()

m__minus__ = Particle(pdg_code = 13,
                      name = 'm-',
                      antiname = 'm+',
                      spin = 2,
                      color = 1,
                      mass = Param.MM,
                      width = Param.ZERO,
                      texname = 'm-',
                      antitexname = 'm-',
                      charge = -1,
                      GhostNumber = 0)

m__plus__ = m__minus__.anti()

tt__minus__ = Particle(pdg_code = 15,
                       name = 'tt-',
                       antiname = 'tt+',
                       spin = 2,
                       color = 1,
                       mass = Param.MTA,
                       width = Param.Wtau,
                       texname = 'tt-',
                       antitexname = 'tt-',
                       charge = -1,
                       GhostNumber = 0)

tt__plus__ = tt__minus__.anti()

tr__minus__ = Particle(pdg_code = 8000020,
                       name = 'tr-',
                       antiname = 'tr+',
                       spin = 2,
                       color = 1,
                       mass = Param.Mtrch,
                       width = Param.Wtrch,
                       texname = 'tr-',
                       antitexname = 'tr-',
                       charge = -1,
                       GhostNumber = 0)

tr__plus__ = tr__minus__.anti()

u = Particle(pdg_code = 2,
             name = 'u',
             antiname = 'u~',
             spin = 2,
             color = 3,
             mass = Param.MU,
             width = Param.ZERO,
             texname = 'u',
             antitexname = 'u',
             charge = 2/3,
             GhostNumber = 0)

u__tilde__ = u.anti()

c = Particle(pdg_code = 4,
             name = 'c',
             antiname = 'c~',
             spin = 2,
             color = 3,
             mass = Param.MC,
             width = Param.WC,
             texname = 'c',
             antitexname = 'c',
             charge = 2/3,
             GhostNumber = 0)

c__tilde__ = c.anti()

t = Particle(pdg_code = 6,
             name = 't',
             antiname = 't~',
             spin = 2,
             color = 3,
             mass = Param.MT,
             width = Param.WT,
             texname = 't',
             antitexname = 't',
             charge = 2/3,
             GhostNumber = 0)

t__tilde__ = t.anti()

d = Particle(pdg_code = 1,
             name = 'd',
             antiname = 'd~',
             spin = 2,
             color = 3,
             mass = Param.MD,
             width = Param.ZERO,
             texname = 'd',
             antitexname = 'd',
             charge = -1/3,
             GhostNumber = 0)

d__tilde__ = d.anti()

s = Particle(pdg_code = 3,
             name = 's',
             antiname = 's~',
             spin = 2,
             color = 3,
             mass = Param.MS,
             width = Param.ZERO,
             texname = 's',
             antitexname = 's',
             charge = -1/3,
             GhostNumber = 0)

s__tilde__ = s.anti()

b = Particle(pdg_code = 5,
             name = 'b',
             antiname = 'b~',
             spin = 2,
             color = 3,
             mass = Param.MB,
             width = Param.WB,
             texname = 'b',
             antitexname = 'b',
             charge = -1/3,
             GhostNumber = 0)

b__tilde__ = b.anti()

ghA = Particle(pdg_code = 9000001,
               name = 'ghA',
               antiname = 'ghA~',
               spin = -1,
               color = 1,
               mass = Param.ZERO,
               width = Param.ZERO,
               texname = 'ghA',
               antitexname = 'ghA',
               charge = 0,
               GhostNumber = 1)

ghA__tilde__ = ghA.anti()

ghZ = Particle(pdg_code = 9000002,
               name = 'ghZ',
               antiname = 'ghZ~',
               spin = -1,
               color = 1,
               mass = Param.MZ,
               width = Param.ZERO,
               texname = 'ghZ',
               antitexname = 'ghZ',
               charge = 0,
               GhostNumber = 1)

ghZ__tilde__ = ghZ.anti()

ghWp = Particle(pdg_code = 9000003,
                name = 'ghWp',
                antiname = 'ghWp~',
                spin = -1,
                color = 1,
                mass = Param.MW,
                width = Param.ZERO,
                texname = 'ghWp',
                antitexname = 'ghWp',
                charge = 1,
                GhostNumber = 1)

ghWp__tilde__ = ghWp.anti()

ghWm = Particle(pdg_code = 9000004,
                name = 'ghWm',
                antiname = 'ghWm~',
                spin = -1,
                color = 1,
                mass = Param.MW,
                width = Param.ZERO,
                texname = 'ghWm',
                antitexname = 'ghWm',
                charge = -1,
                GhostNumber = 1)

ghWm__tilde__ = ghWm.anti()

ghG = Particle(pdg_code = 9000005,
               name = 'ghG',
               antiname = 'ghG~',
               spin = -1,
               color = 8,
               mass = Param.ZERO,
               width = Param.ZERO,
               texname = 'ghG',
               antitexname = 'ghG',
               charge = 0,
               GhostNumber = 1)

ghG__tilde__ = ghG.anti()

A = Particle(pdg_code = 22,
             name = 'A',
             antiname = 'A',
             spin = 3,
             color = 1,
             mass = Param.ZERO,
             width = Param.ZERO,
             texname = 'A',
             antitexname = 'A',
             charge = 0,
             GhostNumber = 0)

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
             GhostNumber = 0)

W__plus__ = Particle(pdg_code = 24,
                     name = 'W+',
                     antiname = 'W-',
                     spin = 3,
                     color = 1,
                     mass = Param.MW,
                     width = Param.WW,
                     texname = 'W+',
                     antitexname = 'W+',
                     charge = 1,
                     GhostNumber = 0)

W__minus__ = W__plus__.anti()

G = Particle(pdg_code = 21,
             name = 'G',
             antiname = 'G',
             spin = 3,
             color = 8,
             mass = Param.ZERO,
             width = Param.ZERO,
             texname = 'G',
             antitexname = 'G',
             charge = 0,
             GhostNumber = 0)

H = Particle(pdg_code = 25,
             name = 'H',
             antiname = 'H',
             spin = 1,
             color = 1,
             mass = Param.MH,
             width = Param.WH,
             texname = '\\phi',
             antitexname = '\\phi',
             charge = 0,
             GhostNumber = 0)

phi0 = Particle(pdg_code = 250,
                name = 'phi0',
                antiname = 'phi0',
                spin = 1,
                color = 1,
                mass = Param.MZ,
                width = Param.ZERO,
                texname = 'phi0',
                antitexname = 'phi0',
                GoldstoneBoson = True,
                charge = 0,
                GhostNumber = 0)

phi__plus__ = Particle(pdg_code = 251,
                       name = 'phi+',
                       antiname = 'phi-',
                       spin = 1,
                       color = 1,
                       mass = Param.MW,
                       width = Param.ZERO,
                       texname = '\\phi^+',
                       antitexname = '\\phi^+',
                       GoldstoneBoson = True,
                       charge = 1,
                       GhostNumber = 0)

phi__minus__ = phi__plus__.anti()


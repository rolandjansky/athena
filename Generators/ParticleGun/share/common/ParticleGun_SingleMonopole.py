
PDG = 4110000
loE = (float(monmass) + 10.)*1000.
hiE  = (float(monmass) + 6000.)*1000.
MeVmass=float(monmass)*1000.
#--------------------------------------------------------------
# Configuration for EvgenJobTransforms
#--------------------------------------------------------------
evgenConfig.description = "Single magnetic monopole generation for Mass=%s, Gcharge=%s in MC15" % (monmass,gcharge)
evgenConfig.keywords = ["exotic", "magneticMonopole", "singleParticle"]
evgenConfig.generators = ["ParticleGun"]
evgenConfig.contact = ["anlionti@cern.ch"]

evgenConfig.specialConfig = 'MASS=%s;GCHARGE=%s;preInclude=SimulationJobOptions/preInclude.Monopole.py' % (monmass,gcharge)



#--------------------------------------------------------------
# Configuration for ParticleGun
#--------------------------------------------------------------
include("ParticleGun/ParticleGun_Common.py")

import ParticleGun as PG
PG.MASSES[4110000] = float(MeVmass)
genSeq.ParticleGun.sampler.pid = (-PDG, PDG)
genSeq.ParticleGun.sampler.mom = PG.EEtaMPhiSampler(energy=[loE,hiE], eta=[-2,2])


#--------------------------------------------------------------
# Edit PDGTABLE.MeV with monopole mass
#--------------------------------------------------------------
ALINE1="M 4110000                          %s.E+03       +0.0E+00 -0.0E+00 Monopole        0" % (monmass)
ALINE2="W 4110000                          0.E+00         +0.0E+00 -0.0E+00 Monopole        0"

import os
import sys

pdgmod = os.path.isfile('PDGTABLE.MeV')
if pdgmod is True:
    os.remove('PDGTABLE.MeV')
os.system('get_files -data PDGTABLE.MeV')
f=open('PDGTABLE.MeV','a')
f.writelines(str(ALINE1))
f.writelines('\n')
f.writelines(str(ALINE2))
f.writelines('\n')
f.close()

del ALINE1
del ALINE2

#--------------------------------------------------------------
# Edit G4particle_whitelist.txt with monopole
#--------------------------------------------------------------

ALINE1="4110000   mm  %s.E+03 (Mev/c) lepton %s" % (monmass,gcharge)
ALINE2="-4110000  mmbar  %s.E+03 (Mev/c) lepton -%s" % (monmass,gcharge)

import os
import sys

pdgmod = os.path.isfile('G4particle_whitelist.txt')
if pdgmod is True:
    os.remove('G4particle_whitelist.txt')
os.system('get_files -data G4particle_whitelist.txt')
f=open('G4particle_whitelist.txt','a')
f.writelines(str(ALINE1))
f.writelines('\n')
f.writelines(str(ALINE2))
f.writelines('\n')
f.close()

del ALINE1
del ALINE2

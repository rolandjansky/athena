
PDG = 10000000 + int(float(charge)*100.0)
loE = (float(mass) + 10.)*1000.
hiE  = (float(mass) + 6000.)*1000.	
MeVmass=float(mass)*1000.
#--------------------------------------------------------------
# Configuration for EvgenJobTransforms
#--------------------------------------------------------------
evgenConfig.description = "Single HECO generation for Mass=%s, Charge=%s in MC15" % (mass,charge)
evgenConfig.keywords = ["exotic", "singleParticle","highElectricChargeObject"]
evgenConfig.generators = ["ParticleGun"]
evgenConfig.contact = ["anlionti@cern.ch"]

evgenConfig.specialConfig = 'MASS=%s;CHARGE=%s;preInclude=SimulationJobOptions/preInclude.Qball.py' % (mass,charge)



#--------------------------------------------------------------
# Configuration for ParticleGun
#--------------------------------------------------------------
include("ParticleGun/ParticleGun_Common.py")

import ParticleGun as PG
PG.MASSES[PDG] = float(MeVmass)
genSeq.ParticleGun.sampler.pid = (-PDG, PDG)
genSeq.ParticleGun.sampler.mom = PG.EEtaMPhiSampler(energy=[loE,hiE], eta=[-2,2])


#--------------------------------------------------------------
# Edit PDGTABLE.MeV with monopole mass
#--------------------------------------------------------------
ALINE1="M %s                          %s.E+03       +0.0E+00 -0.0E+00 Monopole        0" % (PDG,mass)
ALINE2="W %s                          0.E+00         +0.0E+00 -0.0E+00 Monopole        0" % (PDG)

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

ALINE1="%s   qb  %s.E+03 (Mev/c) lepton %s" % (PDG,mass,charge)
ALINE2="-%s  qbbar  %s.E+03 (Mev/c) lepton -%s" % (PDG,mass,charge)

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

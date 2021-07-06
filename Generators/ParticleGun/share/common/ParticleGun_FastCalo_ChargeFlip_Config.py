#! -*- python -*-
evgenConfig.description = "Single particle gun for FastCaloSim event generation"
evgenConfig.keywords = ["singleParticle",]
evgenConfig.generators = ["ParticleGun"]
evgenConfig.contact = ["david.sosa@cern.ch"]

import ParticleGun as PG
import ROOT

class MyParticleSampler(PG.ParticleSampler):
    def __init__(self,energy,eta,pid,shift_z=0):
        self.pid = pid
        self.shift_z = shift_z
        pdg_table = ROOT.TDatabasePDG.Instance()
        mass = pdg_table.GetParticle(self.pid()).Mass()
        self.mom1 = PG.EEtaMPhiSampler(energy=energy,eta=eta,mass=mass)

    def shoot(self):
        pid = self.pid()
        
        shift_z = self.shift_z

        mom = self.mom1.shoot()
        pos_temp = mom.Vect().Unit()
        
        # Would it hit the barrel, or the endcap?
        if abs(pos_temp.Z())/3550.<pos_temp.Perp()/1148.: # Hit the barrel!
            pos_temp *= 1148./pos_temp.Perp()
        else: # Hit the endcap!
            pos_temp *= 3550./abs(pos_temp.Z())

        # Shift position of vector in the Z direction
        pos_temp_2 = ROOT.TVector3()
        pos_temp_2.SetXYZ(pos_temp.X(), pos_temp.Y(), pos_temp.Z()+shift_z)
        pos_temp_2 *= 1. / pos_temp_2.Mag(); # reduce magnitude of vector

        # recalculate; Would it hit the barrel, or the endcap?
        if abs(pos_temp_2.Z())/3550.<pos_temp_2.Perp()/1148.:
            pos_temp_2 *= 1148./pos_temp_2.Perp()
        else:
            pos_temp_2 *= 3550./abs(pos_temp_2.Z())

        pos = ROOT.TLorentzVector(pos_temp_2.X(),pos_temp_2.Y(),pos_temp_2.Z(), pos_temp_2.Mag())
        
        #print "pid ",pid
        
        return [ PG.SampledParticle( pid , mom , pos ) ]

myE = float(runArgs.jobConfig[0].split('_E')[1].split('_')[0])
myZV = float(runArgs.jobConfig[0].split('_')[-1].split('.py')[0].replace("m","-"))

myPDGID = runArgs.jobConfig[0].split('_pid')[1].split('_')[0].replace('n','-')
myPDGID = int(float(myPDGID.replace('p','')))

eta_li = []

if "disj" in runArgs.jobConfig[0]:
    myLowEta1  = 0.01*float(runArgs.jobConfig[0].split('eta_')[1].split('_')[0].replace('m','-'))
    myLowEta2  = 0.01*float(runArgs.jobConfig[0].split('eta_')[1].split('_')[1].replace('m','-'))
    myHighEta1 = 0.01*float(runArgs.jobConfig[0].split('eta_')[1].split('_')[2].replace('m','-'))
    myHighEta2 = 0.01*float(runArgs.jobConfig[0].split('eta_')[1].split('_')[3].replace('m','-'))
    eta_li.extend([myLowEta1,myLowEta2,myHighEta1,myHighEta2])

else:
    myLowEta  = 0.01*float(runArgs.jobConfig[0].split('eta')[1].split('_')[0].replace('m','-'))
    myHighEta = 0.01*float(runArgs.jobConfig[0].split('eta')[1].split('_')[1].replace('m','-'))
    eta_li.extend([myLowEta,myHighEta])


print "================ SETTTINGS ================="
print ("energy = ", myE)
print ("eta = ", eta_li)
print ("pid = ", myPDGID)
print ("shift_z = ", myZV)
print "============================================"

genSeq += PG.ParticleGun()
genSeq.ParticleGun.sampler = MyParticleSampler(energy=myE,eta=eta_li,pid=(myPDGID,myPDGID),shift_z=myZV) #unmixed

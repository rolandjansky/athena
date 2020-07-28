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

        # Define geometry
        barrelR1 = 1148.0
        barrelR2 = 120.0
        barrelR3 = 41.0
        endcapZ1 = 3550.0
        endcapZ2 = 4587.0

        # Would it hit the barrel, or the endcap?
        tanTheta = pos_temp.Perp() / abs( pos_temp.Z() );
        if   tanTheta > barrelR1 / endcapZ1:
            pos_temp *= barrelR1 / pos_temp.Perp()
        elif tanTheta > barrelR2 / endcapZ1:
            pos_temp *= endcapZ1 / abs( pos_temp.Z() )
        elif tanTheta > barrelR2 / endcapZ2:
            pos_temp *= barrelR2 / pos_temp.Perp()
        elif tanTheta > barrelR3 / endcapZ2:
            pos_temp *= endcapZ2 / abs( pos_temp.Z() )
        else:
            pos_temp *= barrelR3 / pos_temp.Perp()

        # Shift position of vector in the Z direction
        pos_temp_2 = ROOT.TVector3()
        pos_temp_2.SetXYZ(pos_temp.X(), pos_temp.Y(), pos_temp.Z()+shift_z)
        pos_temp_2 *= 1. / pos_temp_2.Mag(); # reduce magnitude of vector

        # recalculate; Would it hit the barrel, or the endcap?
        tanTheta_2 = pos_temp_2.Perp() / abs( pos_temp_2.Z() );
        if   tanTheta_2 > barrelR1 / endcapZ1:
            pos_temp_2 *= barrelR1 / pos_temp_2.Perp()
        elif tanTheta_2 > barrelR2 / endcapZ1:
            pos_temp_2 *= endcapZ1 / abs( pos_temp_2.Z() )
        elif tanTheta_2 > barrelR2 / endcapZ2:
            pos_temp_2 *= barrelR2 / pos_temp_2.Perp()
        elif tanTheta_2 > barrelR3 / endcapZ2:
            pos_temp_2 *= endcapZ2 / abs( pos_temp_2.Z() )
        else:
            pos_temp_2 *= barrelR3 / pos_temp_2.Perp()

        pos = ROOT.TLorentzVector(pos_temp_2.X(),pos_temp_2.Y(),pos_temp_2.Z(), pos_temp_2.Mag())

        #print "pid ",pid

        return [ PG.SampledParticle( pid , mom , pos ) ]

E_li = []
myLowE = float(runArgs.jobConfig[0].split('_E')[1].split('_')[0])
myHighE = float(runArgs.jobConfig[0].split('_E')[1].split('_')[1])
E_li.extend([myLowE,myHighE])

myZV = float(runArgs.jobConfig[0].split('_')[-1].split('.py')[0].replace("m","-"))
myPDGID = int(float(runArgs.jobConfig[0].split('_pid')[1].split('_')[0].replace('m','-')))

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
print ("energy = ", E_li)
print ("eta = ", eta_li)
print ("pid = ", myPDGID)
print ("shift_z = ", myZV)
print "============================================"

genSeq += PG.ParticleGun()
print "E_li = ", E_li, ", eta_li = ", eta_li, ", pid = ", myPDGID, ", myZV = ", myZV
genSeq.ParticleGun.sampler = MyParticleSampler(energy=E_li,eta=eta_li,pid=myPDGID,shift_z=myZV)


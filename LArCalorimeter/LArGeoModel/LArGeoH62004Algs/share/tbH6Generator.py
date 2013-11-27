#--- ParticleGenerator -------------------------------------------------
# PDGCodes: e-=11, e+=-11, mu-=13, mu+=-13, pi+=211, pi-=-211, 
#           pi0=111, gamma=22, geantino=999
# Energy and momentum now in MeV!!!!

#from G4AtlasApps import AtlasG4Eng
from AthenaCommon import Logging
from G4AtlasApps.SimFlags import jobproperties

log = Logging.logging.getLogger('LArGeoH62004Algs')

include("GeneratorUtils/StdEvgenSetup.py")
import ParticleGun as PG

# - Particle type (PDG code)
try: 
        ParticleGeneratorPDG=' constant '+ParticlePDG
        particl = int(ParticlePDG)
except:
    log.warning('tbH6Generator: You are running geantino')
    ParticleGeneratorPDG=' constant 999'
    particl = 999
# - Energy 
try:
        ParticleGeneratorEnergy=' constant '+ repr(Energy)
        energy = float(Energy)
except:
        log.warning('tbH6Generator: Unknown energy. Set 200 GeV')
        ParticleGeneratorEnergy=' constant 200000'
        energy = 200000.

# Creating a ParticleGeneratorOrdersList
#ParticleGeneratorOrdersList=[]

# Fixed values
ParticleGeneratorOrdersList += ["vertz: constant -21600."]
ParticleGeneratorOrdersList += ["targetz: constant 9367.5"]
# Particle and energy
ParticleGeneratorOrdersList += ['energy: '+ParticleGeneratorEnergy]
ParticleGeneratorOrdersList += ['pdgcode: '+ParticleGeneratorPDG]
# X coordinate + smearing
if jobproperties.SimFlags.LArTB_H1XSmear.get_Value() != 0.:
   if energy == 40000:
      ParticleGeneratorOrdersList += ["targetx: gaussian 0. "+str(jobproperties.SimFlags.LArTB_H1XSmear.get_Value())]
   else:
      ParticleGeneratorOrdersList += ["targetx: flat -"+str(jobproperties.SimFlags.LArTB_H1XSmear.get_Value())+"  "+str(jobproperties.SimFlags.LArTB_H1XSmear.get_Value())]
   ParticleGeneratorOrdersList += ["vertx: targetx"]
else:
   ParticleGeneratorOrdersList += ["targetx: constant 0."]
   ParticleGeneratorOrdersList += ["vertx: constant 0."]

# Y coordinate + smearing
# adding beam measured Y
if(abs(particl) == 999):
    yreco = 1.07 * jobproperties.SimFlags.LArTB_H1TableYPos.get_Value()
else:
    if(abs(particl) != 11 or energy < 120000):
        yreco = 1.04 * jobproperties.SimFlags.LArTB_H1TableYPos.get_Value()
    else:
        yreco = 1.07 * jobproperties.SimFlags.LArTB_H1TableYPos.get_Value()
yreco =  30967.3 * yreco / 30000.   
if jobproperties.SimFlags.LArTB_H1YSmear.get_Value() != 0.:
   if energy == 40000:
      ParticleGeneratorOrdersList += ["targety: gaussian "+str(yreco)+"  "+str(jobproperties.SimFlags.LArTB_H1YSmear.get_Value())]
   else:
      ParticleGeneratorOrdersList += ["targety: flat "+str(yreco - jobproperties.SimFlags.LArTB_H1YSmear.get_Value())+"  "+str(yreco + jobproperties.SimFlags.LArTB_H1YSmear.get_Value())]
   ParticleGeneratorOrdersList += ["verty: targety - "+str(yreco)]
else:
   ParticleGeneratorOrdersList += ["targety: constant "+str(yreco)]
   ParticleGeneratorOrdersList += ["verty: constant 0."]
      
#ParticleGeneratorOrdersList.sort()
print ParticleGeneratorOrdersList
#jobproperties.SimFlags.ParticleGeneratorOrders = ParticleGeneratorOrdersList
import AthenaCommon.AtlasUnixGeneratorJob
#from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
#topSeq += ParticleGenerator()
#topSeq.ParticleGenerator.orders = sorted(ParticleGeneratorOrdersList)
pg = PG.ParticleGun()
ps = PG.ConstEnergySampler(pid=particl, energy=energy, eta=PG.UniformSampler(-2.0, 2.0))
pg.samplers.append(ps)
topSeq += pg
from TruthExamples.TruthExamplesConf import WriteHepMC
topSeq += WriteHepMC()
#=======================================================================

evgenConfig.description = "Single muons with inv pt from 1 to 1000 GeV, |eta|<3 and special vertez"
evgenConfig.keywords = ["singleparticle", "mu"]

## Common setup for ParticleGenerator
from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
topAlg += ParticleGenerator()
evgenConfig.generators = ["ParticleGenerator"]

#multi-particle event
nparticles = 10

topAlg.ParticleGenerator.orders = []
for ipart in xrange(nparticles) :
    topAlg.ParticleGenerator.orders += [
        "PDGcode[%d]: sequence -13 13" % ipart,
        "invpt[%d]: flat 1.0e-6 1.3e-3" % ipart,
        "eta[%d]: flat -3.0 3.0" % ipart,
        "phi[%d]: flat -3.14159 3.14159" % ipart,
        "targetip[%d]: flat -2.2 2.2" % ipart,
        "vertZ[%d]: flat -120.0 120.0" % ipart
        ]

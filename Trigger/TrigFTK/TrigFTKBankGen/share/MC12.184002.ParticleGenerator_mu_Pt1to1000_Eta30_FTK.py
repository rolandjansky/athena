evgenConfig.description = "Single muons with inv pt from 1 to 1000 GeV, |eta|<3 and special vertez"
evgenConfig.keywords = ["singleparticle", "mu"]

## Common setup for ParticleGenerator
from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
topAlg += ParticleGenerator()
evgenConfig.generators = ["ParticleGenerator"]


topAlg.ParticleGenerator.orders = [
"PDGcode: sequence -13 13" ,
  "invpt: flat 1.0e-6 1.3e-3",
  "eta: flat -3.0 3.0",
  "phi: flat -3.14159 3.14159",
  "targetip: flat -2.2 2.2",
  "vertZ: flat -120.0 120.0"
  ]

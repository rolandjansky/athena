print 'SINGLE PARTICLE GENERATOR'
## Run ParticleGenerator
import AthenaCommon.AtlasUnixGeneratorJob
spgorders = ['pdgcode: sequence 211 -211',
             'vertX: constant 0.0',
             'vertY: constant 0.0',
             'vertZ: constant 0.0',
             't: constant 0.0',
             'eta: constant 0.2',
             'phi: flat  0 6.28318',
             'pt: constant 50000']
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()
from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
topSeq += ParticleGenerator()
topSeq.ParticleGenerator.orders = sorted(spgorders)

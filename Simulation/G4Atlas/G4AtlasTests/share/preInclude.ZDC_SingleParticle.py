print 'SINGLE PARTICLE GENERATOR'
## Run ParticleGenerator
# One neutron and one photon per side
import AthenaCommon.AtlasUnixGeneratorJob
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
import ParticleGun as PG
pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = PG.CyclicSeqSampler([2112, 22, 2112, 22])
esampler = PG.CyclicSeqSampler([1360000, 500000, 1360000, 500000])
thsampler = PG.CyclicSeqSampler([0, 0, PG.PI, PG.PI])
pg.sampler.mom = PG.EThetaMPhiSampler(energy=esampler, theta=thsampler)
job += pg

include("G4AtlasApps/fragment.SimCopyWeights.py")

print "As this is a single particle generator job turn off VertexPositioner"
simFlags.EventFilter.get_Value()['VertexPositioner'] = False
simFlags.EventFilter.get_Value()['BeamEffectTransformation'] = False
simFlags.EventFilter.get_Value()['PrimaryEventRotations'] = False
simFlags.VertexFromCondDB= False

try:
    include('ForwardTransportSvc/preInclude.ForwardTransportFlags_3.5TeV_0000.55m_nominal_v01.py')
except:
    print "forwardTransportFlags not available in this release."

include('ForwardTransportSvc/ForwardTransportSvcConfig.ZDC.py')

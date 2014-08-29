print 'SINGLE PARTICLE GENERATOR'

import AthenaCommon.AtlasUnixGeneratorJob
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
import ParticleGun as PG
pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = 2212
pg.sampler.mom = PG.EEtaMPhiSampler(energy=3500000, eta=10)
job += pg

include("G4AtlasApps/fragment.SimCopyWeights.py")

print "As this is a single particle generator job turn off VertexPositioner (EventFilter already off when ZDCon)"
simFlags.MagneticField.set_Off()
simFlags.EventFilter.get_Value()['VertexPositioner'] = False
simFlags.EventFilter.get_Value()['BeamEffectTransformation'] = False
simFlags.EventFilter.get_Value()['PrimaryEventRotations'] = False
simFlags.VertexFromCondDB = False

try:
    include("ForwardTransportSvc/preInclude.ForwardTransportFlags_3.5TeV_0090.00m_nominal_v02.py")
except:
    print "forwardTransportFlags not available in this release. The job will only work if this release uses TwissFiles-00-00-03 or earlier."

include('ForwardTransportSvc/ForwardTransportSvcConfig.ALFA.py')

try:
    from ForwardTransportSvc.ForwardTransportFlags import forwardTransportFlags
except:
    #Back-compatibility hack for releases where forwardTransportFlags are not available.
    # use Twiss Files for 7 TeV to match the CoM energy used above.
    ForwardTransSvc.TwissFile1 = os.getenv('TwissFilesPATH') + '/3.5TeV/0090.00m/nominal/v02/c1_90alfa2.twiss'
    ForwardTransSvc.TwissFile2 = os.getenv('TwissFilesPATH') + '/3.5TeV/0090.00m/nominal/v02/c4_90alfa.twiss'

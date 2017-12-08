## job options file to steer the cluster example

#########################
if not 'EVTMAX' in dir():
    EVTMAX=5
if not 'INPUT' in dir():
    INPUT = [ # a "reference" file from FCT
        'root://eosatlas//eos/atlas/user/b/binet/reffiles/14.1.0.x'\
        '/AllBasicSamples.AOD.pool.root'
        ]
    #INPUT = ['AOD.pool.root']
if not 'TUPLEOUT' in dir():
    TUPLEOUT = 'pyathena_pyclusters.root'
    pass
#########################

## setup application for reading POOL files
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = INPUT

## events to process
theApp.EvtMax = EVTMAX

## setup the histo svc
##  - register an output stream id 'py'
##  - associate an output file
svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output = [
    "py DATAFILE='%s' TYP='ROOT' OPT='RECREATE'" % TUPLEOUT,
    ]

## alg sequence creation
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

## schedule the algorithm
from PyAthenaTests.Lib import ClusterExAlg
job += ClusterExAlg()

## ease interactive prompt, if any
##  => a no-op if the event loop is actually a batch-like
#import AthenaPython.Helpers as IPyAthena
#IPyAthena.setupIPyAthena()

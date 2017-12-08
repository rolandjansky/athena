from glob import glob
#_cbntfiles = glob('/tmp/binet/data/cbnt_aod.aan.*.root')
#_cbntfiles = glob('/tmp/binet/data/NTUP_TOP.*.root*')
#_cbntfiles = glob('/tmp/binet/data/mini*.root*')
#_cbntfiles = glob('/tmp/binet/data/ntuple*.root*')
_cbntfiles = [
    'root://eosatlas//eos/atlas/user/b/binet/utests/utests/filter-d3pd/ntuple.0.root',
    'root://eosatlas//eos/atlas/user/b/binet/utests/utests/filter-d3pd/ntuple.1.root',
    ]
import AthenaRootComps.ReadAthenaRoot
svcMgr.EventSelector.InputCollections = _cbntfiles
svcMgr.EventSelector.TupleName = "CollectionTree"
svcMgr.EventSelector.TupleName = "egamma"

from AthenaPython import PyAthena
StatusCode = PyAthena.StatusCode

class MyAlg( PyAthena.Alg ):

    def __init__(self, name='MyAlg', **kw):
        kw['name'] = name
        self.activeBranches = kw.get(
            'activeBranches',
            ['RunNumber',
             'EventNumber',
             'el_n',
             'el_eta',
             'el_jetcone_dr',
             ])
        super(MyAlg, self).__init__(**kw)
        return

    def initialize(self):
        self.evtstore = PyAthena.py_svc('StoreGateSvc')
        
        return StatusCode.Success

    def execute(self):
        self.msg.info('running execute...')
        #keys = self.evtstore.keys()
        for br in self.activeBranches:
            try:
                o = self.evtstore[br]
                if hasattr(o, 'at'):
                    o = list(o)
                    for i,v in enumerate(o):
                        if hasattr(v, 'at') and not isinstance(v, (basestring,)):
                            o[i] = list(v)
                self.msg.info('%s: %r', br, o)
            except Exception, err:
                self.msg.info(' --> err for [%s]: %s' % (br, err))
                pass
        return StatusCode.Success

    def finalize(self):
        return StatusCode.Success

    pass # MyAlg

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job += MyAlg()

activeBranches = [ 'nMC' ] # only 1 branch
activeBranches = [ '*' ]   # all branches
activeBranches = []        # no branch

svcMgr.EventSelector.ActiveBranches = activeBranches
job.MyAlg.ActiveBranches            = activeBranches

if not 'EVTMAX' in dir():
    EVTMAX=-1
theApp.EvtMax = EVTMAX

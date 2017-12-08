from glob import glob
_cbntfiles = glob('/tmp/binet/data/cbnt_aod.aan.*.root')
_cbntfiles = glob('/tmp/binet/data/NTUP_TOP.*.root*')
_cbntfiles = glob('/tmp/binet/data/mini*.root*')
_cbntfiles = glob('/tmp/binet/data/ntuple*.root*')
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
        self.activeBranches = kw.get('activeBranches',
                                     ['EventNumber',
                                      'RunNumber',
                                      ])
        super(MyAlg, self).__init__(**kw)
        return

    def initialize(self):

        self.thSvc = PyAthena.py_svc('THistSvc', iface='ITHistSvc')
        if not self.thSvc:
            self.msg.error("Could not retrieve THistSvc !")
            return StatusCode.Failure

        self.tree = self.thSvc.get('/temp/TTreeStream/egamma',
                                   klass='TTree')
        if not self.tree:
            self.msg.error('could not retrieve tree from THistSvc')
            return StatusCode.Failure

        return StatusCode.Success

    def execute(self):
        self.msg.info('running execute...')
        for br in self.activeBranches:
            b = getattr(self.tree, br)
            if hasattr(b, 'at'):
                b = list(b)
            self.msg.info('branch [%s]: %r', br, b)
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

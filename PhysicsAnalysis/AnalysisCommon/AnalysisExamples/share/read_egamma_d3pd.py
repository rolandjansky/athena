_d3pd_files = [
    'root://castoratlas//castor/cern.ch/user/b/binet/utests/utests/filter-d3pd/ntuple.0.root',
    #'ntuple.0.root',
    ]
import AthenaRootComps.ReadAthenaRoot
svcMgr.EventSelector.InputCollections = _d3pd_files
svcMgr.EventSelector.TupleName = "egamma"

#activeBranches = [ 'nMC' ] # only 1 branch
activeBranches = [ '*' ]   # all branches
activeBranches = []        # no branch
svcMgr.EventSelector.ActiveBranches = activeBranches

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job += CfgMgr.AthEx__ReadD3pdNtuple(
    TreeName = '/temp/TTreeStream/egamma',
    OutputLevel = Lvl.DEBUG
    )

if not 'EVTMAX' in dir():
    EVTMAX=10
theApp.EvtMax = EVTMAX


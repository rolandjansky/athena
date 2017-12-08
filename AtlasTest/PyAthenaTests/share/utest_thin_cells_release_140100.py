############################
if not 'INPUT' in dir():
    INPUT=[
        'root://eosatlas//eos/atlas/user/b/binet/reffiles/14.1.0.x'\
        '/AllBasicSamples.ESD.pool.root'
        ]
    pass
if not 'OUTPUT' in dir():
    OUTPUT = 'thinned-cells.pool'
    pass
############################

PoolESDInput = INPUT
PoolESDOutput= 'dummy.esd.pool'

# ESD reprocessing
include ('RecExCommon/esdtoesd.py')
svcMgr.GeoModelSvc.AtlasVersion = 'ATLAS-CSC-05-01-00'

include('PyAthenaTests/pythin_jobOptions.py')

job.Thinner.coll_type='CaloCellContainer'
job.Thinner.coll_name='AllCalo'
job.Thinner._filter_cnt = 0
def filter_fct(x):
    self = PyAthena.algs.Thinner
    cnt = self._filter_cnt
    dec = cnt%2==0
    self._filter_cnt += 1
    return dec
job.Thinner.filter_fct = filter_fct

job.OutStream.TakeItemsFromInput = True
job.OutStream.ForceRead = True

# make sure our thinned stream is the only one
del job.StreamESD
del job.StreamESD_FH

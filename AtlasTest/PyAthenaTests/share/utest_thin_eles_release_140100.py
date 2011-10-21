############################
if not 'INPUT' in dir():
    INPUT=[
        'root://eosatlas//eos/atlas/user/b/binet/reffiles/14.1.0.x'\
        '/AllBasicSamples.AOD.pool.root'
        ]
    pass
if not 'OUTPUT' in dir():
    OUTPUT = 'thinned-eles.pool'
    pass
############################

PoolAODInput = INPUT
PoolAODOutput= 'dummy.aod.pool'

# AOD reprocessing
include ('RecExCommon/aodtoaod.py')
svcMgr.GeoModelSvc.AtlasVersion = 'ATLAS-CSC-05-01-00'

include('PyAthenaTests/pythin_jobOptions.py')
del job.StreamAOD
del job.StreamAOD_FH
del job.Thinner

job.insert(-1, Thinner('thin_eles',
                        coll_type='ElectronContainer',
                        coll_name='ElectronAODCollection'))
job.OutStream.TakeItemsFromInput = False
job.OutStream.ForceRead = False
job.OutStream.ItemList = [ 'EventInfo#*',
                           'ElectronContainer#*',]

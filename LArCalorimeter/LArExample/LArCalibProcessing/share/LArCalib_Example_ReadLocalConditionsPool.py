###################################################################
#
# jobOptions to read condition data from a local POOL file(s)
#
###################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArAthenaPool/LArAthenaPool_joboptions.py" )
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py") 

CondProxyProvider = Service("CondProxyProvider")
ProxyProviderSvc  = Service("ProxyProviderSvc")
ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]

EventPersistencySvc = Service( "EventPersistencySvc" )
EventPersistencySvc.CnvServices += [ "AthenaPoolCnvSvc" ]

#
# sigle POOL file should be defined as:
# PoolFileName = "filename.pool.root"
#
if 'PoolFileName' in dir() :
	CondProxyProvider.InputCollections = [ PoolFileName ]

#
# multiple POOL files should be defined as:
# PoolFileList = [ "file1.pool.root", "file2.pool.root", ... ] 
#
if 'PoolFileList' in dir() :
	if ( len(PoolFileList)>0 ) :
		CondProxyProvider.InputCollections += PoolFileList

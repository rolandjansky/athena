## @brief Switch database to using FRONTIER, but with a fallback
# to DBRelease if FRONTIER_SERVER is undefined (e.g., on HPC)

# Move from RecJobTransforms to PyJobTransforms to enable use
# in simulation ATN and KV jobs
# $Id: UseFrontier.py 605683 2014-07-09 17:22:17Z graemes $

if(os.environ.get('FRONTIER_SERVER')):
    print 'UseFrontier.py: Enabling FRONTIER DB access'
    from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
    ServiceMgr+=DBReplicaSvc(COOLSQLiteVetoPattern="DBRelease")
else:
    print 'UseFrontier.py: Using default DB access'
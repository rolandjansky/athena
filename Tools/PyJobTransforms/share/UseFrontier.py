## @brief Switch database to using FRONTIER, but with a fallback
# to DBRelease if FRONTIER_SERVER is undefined (e.g., on HPC)

# Move from RecJobTransforms to PyJobTransforms to enable use
# in simulation ATN and KV jobs

if(os.environ.get('FRONTIER_SERVER')):
    print('UseFrontier.py: Enabling FRONTIER DB access')
    from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
    ServiceMgr+=DBReplicaSvc(COOLSQLiteVetoPattern="DBRelease")
else:
    print('UseFrontier.py: Using default DB access')

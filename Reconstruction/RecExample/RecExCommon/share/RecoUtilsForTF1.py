#######################
## Reco utils for TF1##
#######################

if hasattr(svcMgr, 'AthenaPoolCnvSvc'):
    from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
    # Switch on splitting by setting default SplitLevel to 99
    svcMgr.AthenaPoolCnvSvc.PoolAttributes = [ "DEFAULT_SPLITLEVEL ='99'" ]
    # Increase default BasketSize to 32K, ROOT default (but overwritten by POOL)
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DEFAULT_BUFFERSIZE = '32000'" ]
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolAODOutput() + "'; COMPRESSION_LEVEL = '5'" ]
    # Set auto_flush level, for testing we should run with 10, 20, 50, 100, 200, 500
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolAODOutput() + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '10'" ]

    # Turn off auto_flush for DataHeader container to avoid basket optimization
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "ContainerName = 'TTree=POOLContainer'; TREE_AUTO_FLUSH = '0'" ]
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "ContainerName = 'POOLContainer(DataHeaderForm)'; BRANCH_BASKET_SIZE = '256000'" ]

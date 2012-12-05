# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from DataQualityUtils.DQWebDisplayConfig import DQWebDisplayConfig

dqconfig = DQWebDisplayConfig()
dqconfig.config        = "TCT"
dqconfig.hcfg          = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Collisions/collisions_run.1.41.hcfg"
dqconfig.hcfg_min10    = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Collisions/collisions_minutes10.1.9.hcfg"
dqconfig.hcfg_min30    = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Collisions/collisions_minutes30.1.5.hcfg"
dqconfig.hanResultsDir = "/afs/cern.ch/atlas/offline/external/FullChainTest/tier0/dqm/han_results"
dqconfig.htmlDir       = "/afs/cern.ch/atlas/offline/external/FullChainTest/tier0/dqm/www"
dqconfig.htmlWeb       = "http://atlas-project-fullchaintest.web.cern.ch/atlas-project-FullChainTest/tier0/dqm/www"
dqconfig.runlist       = "runlist_TCT.xml"
dqconfig.indexFile     = "results_TCT.html"
dqconfig.lockFile      = "DQWebDisplay_TCT.lock"
dqconfig.dbConnection  = "sqlite://;schema=MyCOOL_histo.db;dbname=OFLP200"
dqconfig.dqmfOfl       = "/GLOBAL/DETSTATUS/DQMFOFL"
dqconfig.dbConnectionHisto  = "sqlite://;schema=MyCOOL_histo.db;dbname=OFLP200"
dqconfig.dqmfOflHisto       = "/GLOBAL/DETSTATUS/DQMFOFLH"
dqconfig.dbTagName          = "DetStatusDQMFOFL-TCT"

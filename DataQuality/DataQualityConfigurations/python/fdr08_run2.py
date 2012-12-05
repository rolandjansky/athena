# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from DataQualityUtils.DQWebDisplayConfig import DQWebDisplayConfig

dqconfig = DQWebDisplayConfig()
dqconfig.config         = "FDR2c"
dqconfig.hcfg           = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Collisions/collisions_run.1.36.hcfg"
dqconfig.hcfg_min10     = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Collisions/collisions_minutes10.1.9.hcfg"
dqconfig.hcfg_min30     = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Collisions/collisions_minutes30.1.5.hcfg"
dqconfig.server         = "voatlas12.cern.ch"
dqconfig.histogramCache = "/afs/cern.ch/user/a/atlasdqm/w1/histogram_web_display_cache"
dqconfig.hanResultsDir  = "/dqmdisk0/han_results/tier0/NotUsed"
dqconfig.htmlDir        = "/dqmdisk0/www/tier0/FDR2c"
dqconfig.htmlWeb        = "http://atlasdqm.cern.ch/tier0/FDR2c"
dqconfig.runlist        = "runlist_FDR2c.xml"
dqconfig.indexFile      = "results_FDR2c.html"
dqconfig.lockFile       = "DQWebDisplay_FDR2c.lock"
dqconfig.dbConnection       = "oracle://ATLAS_COOLWRITE;schema=ATLAS_COOLOFL_GLOBAL;dbname=OFLP200;"
dqconfig.dqmfOfl            = "/GLOBAL/DETSTATUS/DQMFOFL"
dqconfig.dbConnectionHisto  = "oracle://ATLAS_COOLWRITE;schema=ATLAS_COOLOFL_GLOBAL;dbname=OFLP200;"
dqconfig.dqmfOflHisto       = "/GLOBAL/DETSTATUS/DQMFOFLH"
dqconfig.dbTagName          = "DetStatusDQMFOFLH-FDR2-02"
dqconfig.auth       = "/afs/cern.ch/atlas/project/tzero/var"

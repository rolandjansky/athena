# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from DataQualityUtils.DQWebDisplayConfig import DQWebDisplayConfig

dqconfig = DQWebDisplayConfig()
dqconfig.config         = "data08"
dqconfig.hcfg           = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Collisions/collisions_run.1.63.hcfg"
dqconfig.hcfg_min10     = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Collisions/collisions_minutes10.1.18.hcfg"
dqconfig.hcfg_min30     = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Collisions/collisions_minutes30.1.6.hcfg"
dqconfig.server         = "voatlas12.cern.ch"
dqconfig.histogramCache = "/afs/cern.ch/user/a/atlasdqm/w1/histogram_web_display_cache"
dqconfig.hanResultsDir  = "/dqmdisk0/han_results/tier0/all"
dqconfig.htmlDir        = "/dqmdisk0/www/tier0/data08"
dqconfig.htmlWeb        = "http://atlasdqm.cern.ch/tier0/data08"
dqconfig.runlist        = "runlist_data08.xml"
dqconfig.indexFile      = "results_data08.html"
dqconfig.lockFile       = "DQWebDisplay_data08.lock"
#dqconfig.dbConnection       = "oracle://ATLAS_COOLWRITE;schema=ATLAS_COOLOFL_GLOBAL;dbname=COMP200;"
#dqconfig.dqmfOfl            = "/GLOBAL/DETSTATUS/DQMFOFL"
#dqconfig.dbConnectionHisto  = "oracle://ATLAS_COOLWRITE;schema=ATLAS_COOLOFL_GLOBAL;dbname=COMP200;"
#dqconfig.dqmfOflHisto       = "/GLOBAL/DETSTATUS/DQMFOFLH"
#dqconfig.dbTagName          = "HEAD"

#authentication file at Tier0 (uncomment this option at Tier0 only)
#dqconfig.auth       = "/afs/cern.ch/atlas/project/tzero/var"

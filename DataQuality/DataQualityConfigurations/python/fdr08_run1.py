# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from DataQualityUtils.DQWebDisplayConfig import DQWebDisplayConfig

dqconfig = DQWebDisplayConfig()
dqconfig.config         = "FDR1"
dqconfig.hcfg          = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/FDR1/fdr_run.1.7.hcfg"
dqconfig.hcfg_min10    = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/FDR1/fdr_minutes10.1.4.hcfg"
dqconfig.hcfg_min30    = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/FDR1/fdr_minutes30.1.3.hcfg"
dqconfig.hanResultsDir = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/han_results/tier0"
dqconfig.htmlDir       = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/www/tier0"
dqconfig.htmlWeb       = "http://atlasdqm.web.cern.ch/atlasdqm/tier0"
dqconfig.runlist       = "runlist_FDR1.xml"
dqconfig.indexFile     = "../results_FDR1.html"
dqconfig.lockFile      = "DQWebDisplay_FDR1.lock"

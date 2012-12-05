# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from DataQualityUtils.DQWebDisplayConfig import DQWebDisplayConfig

dqconfig = DQWebDisplayConfig()
dqconfig.config         = "RTT"
dqconfig.hcfg          = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Cosmics/cosmics_run.1.13.hcfg"
dqconfig.hcfg_min10    = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Cosmics/cosmics_minutes10.1.3.hcfg"
dqconfig.hcfg_min30    = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Cosmics/cosmics_minutes30.1.4.hcfg"
dqconfig.hanResultsDir = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/han_results/RTT"
dqconfig.htmlDir       = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/www/RTT"
dqconfig.htmlWeb       = "http://atlasdqm.web.cern.ch/atlasdqm/RTT"
dqconfig.runlist       = "runlist_RTT.xml"
dqconfig.indexFile     = "results_RTT.html"
dqconfig.lockFile      = "DQWebDisplay_RTT.lock"

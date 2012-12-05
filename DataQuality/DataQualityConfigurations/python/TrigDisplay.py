# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from DataQualityUtils.DQWebDisplayConfig import DQWebDisplayConfig

dqconfig = DQWebDisplayConfig()
dqconfig.config         = "Trigger Reprocessing"
dqconfig.hcfg           = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Trigger/trigger_run.current.hcfg"
dqconfig.hcfg_min10     = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Trigger/trigger_minutes10.current.hcfg"
dqconfig.hcfg_min30     = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Trigger/trigger_minutes30.current.hcfg"
dqconfig.hanResultsDir  = "/afs/cern.ch/atlas/project/trigger/trigcaf/han_results"
dqconfig.doHandi        = False
dqconfig.htmlDir        = "/afs/cern.ch/atlas/project/trigger/trigcaf/www"
dqconfig.htmlWeb        = "http://atlasdqm.cern.ch/tier0/trigger"
dqconfig.runlist        = "runlist_trigger.xml"
dqconfig.indexFile      = "results_trigger.html"
dqconfig.lockFile       = "DQWebDisplay_trigger.lock"

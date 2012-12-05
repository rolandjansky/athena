# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DataQualityUtils.DQWebDisplayConfig import DQWebDisplayConfig

dqconfig                = DQWebDisplayConfig()
dqconfig.config         = "TEST"

# The "current" files are symbolic links that will always point to the revision that is currently used by Tier 0.
# You can change these settings in your local working copy of this file, but please do not commit the change to SVN.
hcfg_dir                = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/HeavyIons"
revision                = "current"

dqconfig.hcfg           = "%s/heavyions_run.%s.hcfg"       % (hcfg_dir, revision)
dqconfig.hcfg_min10     = "%s/heavyions_minutes10.%s.hcfg" % (hcfg_dir, revision)
dqconfig.hcfg_min30     = "%s/heavyions_minutes30.%s.hcfg" % (hcfg_dir, revision)

dqconfig.histogramCache = "/afs/cern.ch/user/a/atlasdqm/w1/histogram_web_display_cache"
dqconfig.hanResultsDir  = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/han_results/test"
dqconfig.htmlDir        = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/www/test"
dqconfig.htmlWeb        = "http://atlasdqm.web.cern.ch/atlasdqm/test"
dqconfig.runlist        = "runlist_TEST.xml"
dqconfig.indexFile      = "results_TEST.html"
dqconfig.lockFile       = "DQWebDisplay_TEST.lock"
dqconfig.doHandi        = False

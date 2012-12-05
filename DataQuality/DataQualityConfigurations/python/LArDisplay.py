# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DataQualityUtils.DQWebDisplayConfig import DQWebDisplayConfig

dqconfig                = DQWebDisplayConfig()
dqconfig.config         = "larcomm"

# The "current" files are symbolic links that will always point to the revision that is currently used by Tier 0.
# You can change these settings in your local working copy of this file, but please do not commit the change to SVN.
hcfg_dir                = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Collisions"
revision                = "current"

dqconfig.hcfg           = "%s/collisions_run.%s.hcfg"       % (hcfg_dir, revision)
dqconfig.hcfg_min10     = "%s/collisions_minutes10.%s.hcfg" % (hcfg_dir, revision)
dqconfig.hcfg_min30     = "%s/collisions_minutes30.%s.hcfg" % (hcfg_dir, revision)

dqconfig.histogramCache = "/afs/cern.ch/user/a/atlasdqm/dqmdisk1/histogram_web_display_cache"
dqconfig.hanResultsDir  = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/han_results/larcomm"
dqconfig.htmlDir        = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/www/larcomm"
dqconfig.htmlWeb        = "http://cern.ch/atlasdqm/larcomm"
dqconfig.runlist        = "runlist_larcomm.xml"
dqconfig.indexFile      = "results_larcomm.html"
dqconfig.lockFile       = "DQWebDisplay_larcomm.lock"
dqconfig.doHandi        = False

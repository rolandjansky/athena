# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DataQualityConfigurations.data09_900GeV import *

dqconfig.config        = "TCTReleaseHAN"
dqconfig.hcfg          = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/tct_reference_run.hcfg"
dqconfig.hcfg_min10    = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/tct_reference_minutes10.hcfg"
dqconfig.hcfg_min30    = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/tct_reference_minutes30.hcfg"
dqconfig.hanResultsDir = "/afs/cern.ch/atlas/offline/external/FullChainTest/tier0/dqm/han_results"
dqconfig.htmlDir       = "/afs/cern.ch/atlas/offline/external/FullChainTest/tier0/dqm/wwwtemp"
dqconfig.htmlWeb       = "http://atlas-project-fullchaintest.web.cern.ch/atlas-project-FullChainTest/tier0/dqm/wwwtemp"
dqconfig.runlist       = "runlist_TCT.xml"
dqconfig.indexFile     = "results_TCT.html"
dqconfig.lockFile      = ""
dqconfig.dbConnection  = "sqlite://;schema=MyCOOL_histo.db;dbname=COMP200"
dqconfig.dqmfOfl       = "/GLOBAL/DETSTATUS/DQMFOFL"
dqconfig.dbConnectionHisto  = "sqlite://;schema=MyCOOL_histo.db;dbname=COMP200"
dqconfig.dqmfOflHisto       = "/GLOBAL/DETSTATUS/DQMFOFLH"
dqconfig.dbTagName          = "DetStatusDQMFOFLH-TCT"
dqconfig.doHandi       = True

dqconfig.auth           = ""
dqconfig.server         = ""
dqconfig.histogramCache = ""
dqconfig.webHandoffDir  = ""

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DataQualityUtils.DQWebDisplayConfig import DQWebDisplayConfig

dqconfig = DQWebDisplayConfig()
dqconfig.config        = "TCTYesterdaysHAN"
#dqconfig.config        = "TCTDQWebDisplayCosmics"
## /afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Cosmics/
#dqconfig.hcfg_min30    = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Cosmics/cosmics_minutes30.1.9.hcfg"
#dqconfig.hcfg          = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Cosmics/cosmics_run.1.80.hcfg"
#dqconfig.hcfg_min10    = "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Cosmics/cosmics_minutes10.1.12.hcfg"
dqconfig.hcfg_min30    = "cosmics_prevminutes30.latest.hcfg"
dqconfig.hcfg          = "cosmics_prevrun.latest.hcfg"
dqconfig.hcfg_min10    = "cosmics_prevminutes10.latest.hcfg"
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


# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DataQualityUtils.DQWebDisplayConfig import DQWebDisplayConfig
import os

isprod = os.environ.get('DQPRODUCTION') == '1'

dqconfig = DQWebDisplayConfig()
dqconfig.config         = "Cosmics"
dqconfig.hcfg           = os.environ.get('DQC_HCFG_COSMICS_RUN', "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Cosmics/cosmics_run.current.hcfg")
dqconfig.hcfg_min10     = os.environ.get('DQC_HCFG_COSMICS_MINUTES10', "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Cosmics/cosmics_minutes10.current.hcfg")
dqconfig.hcfg_min30     = os.environ.get('DQC_HCFG_COSMICS_MINUTES30', "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Cosmics/cosmics_minutes30.current.hcfg")
dqconfig.server         = ["atlasdqm.cern.ch"] if isprod else []
dqconfig.histogramCache = "/afs/cern.ch/user/a/atlasdqm/w1/histogram_web_display_cache" if isprod else ''
dqconfig.hanResultsDir  = "/dqmdisk0/han_results/tier0/Cosmics08" if isprod else '/afs/cern.ch/user/a/atlasdqm/dqmdisk/han_results/test'
dqconfig.doHandi        = False
dqconfig.htmlDir        = "/dqmdisk0/www/tier0/Cosmics08" if isprod else '/afs/cern.ch/user/a/atlasdqm/dqmdisk/www/test'
dqconfig.htmlWeb        = "http://atlasdqm.cern.ch/tier0/Cosmics08"
dqconfig.runlist        = "runlist_Cosmics08.xml"
dqconfig.indexFile      = "results_Cosmics08.html"
dqconfig.lockFile       = "DQWebDisplay_Cosmics08.lock"
dqconfig.webHandoffDir  = '/afs/cern.ch/user/a/atlasdqm/maxidisk/webHandoff' if isprod else ''

dqconfig.filemap        = { 'RPCDQMFOFFLINE.db': '/afs/cern.ch/user/m/muoncali/w0/RPC/DQAresults',
                            'RPCConditionDB.db': '/afs/cern.ch/user/m/muoncali/w0/RPC/DQAresults',
                            'MDTDQMFOFFLINE_DEAD.db': '/afs/cern.ch/user/m/muoncali/w0/RPC/DQAresults',
                            'MDTDQMFOFFLINE_NOISY.db': '/afs/cern.ch/user/m/muoncali/w0/RPC/DQAresults',
}

dqconfig.dbConnection  = "oracle://ATLAS_COOLWRITE;schema=ATLAS_COOLOFL_GLOBAL;dbname=CONDBR2;"
dqconfig.dqmfOfl       = "/GLOBAL/DETSTATUS/DQMFOFL"
dqconfig.dbTagName    = "DetStatusDQMFOFL-%(stream)s-pass1"
dqconfig.dbTagNameESn       = "DetStatusDQMFOFL-%(stream)s-ES%(procpass)s"
dqconfig.shiftOfl       = "/GLOBAL/DETSTATUS/SHIFTOFL"

dqconfig.auth       = "/afs/cern.ch/atlas/project/tzero/var"

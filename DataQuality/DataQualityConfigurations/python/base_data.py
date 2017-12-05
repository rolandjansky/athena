# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from DataQualityUtils.DQWebDisplayConfig import DQWebDisplayConfig
import os

dqconfig = DQWebDisplayConfig()
dqconfig.config         = "Collisions Data"
dqconfig.hcfg           = os.environ.get('DQC_HCFG_COLLISIONS_RUN', "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Collisions/collisions_run.current.hcfg")
dqconfig.hcfg_min10     = os.environ.get('DQC_HCFG_COLLISIONS_MINUTES10', "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Collisions/collisions_minutes10.current.hcfg")
dqconfig.hcfg_min30     = os.environ.get('DQC_HCFG_COLLISIONS_MINUTES30', "/afs/cern.ch/user/a/atlasdqm/dqmdisk/tier0/han_config/Collisions/collisions_minutes30.current.hcfg")
dqconfig.server         = ["aiatlas039.cern.ch", "aiatlas035.cern.ch", "aiatlas133.cern.ch"]
dqconfig.eosResultsDir  = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/data-dqm/han_results/tier0/collisions/"
dqconfig.histogramCache = "/afs/cern.ch/user/a/atlasdqm/w1/histogram_web_display_cache"
dqconfig.hanResultsDir  = "/dqmdisk0/han_results/tier0/collisions"
dqconfig.doHandi        = False
dqconfig.htmlDir        = "/dqmdisk0/www/tier0/collisions"
dqconfig.htmlWeb        = "http://atlasdqm.cern.ch/tier0/collisions"
dqconfig.runlist        = "runlist_collisions.xml"
dqconfig.indexFile      = "results_collisions.html"
dqconfig.lockFile       = "DQWebDisplay_collisions.lock"
dqconfig.webHandoffDir  = '/afs/cern.ch/user/a/atlasdqm/maxidisk/webHandoff'

dqconfig.filemap        = { 'RPCDQMFOFFLINE.db': '/afs/cern.ch/user/m/muoncali/w0/RPC/DQAresults',
                            'RPCConditionDB.db': '/afs/cern.ch/user/m/muoncali/w0/RPC/DQAresults',
                            'MDTDQMFOFFLINE_DEAD.db': '/afs/cern.ch/user/m/muoncali/w0/RPC/DQAresults',
                            'MDTDQMFOFFLINE_NOISY.db': '/afs/cern.ch/user/m/muoncali/w0/RPC/DQAresults',
                            'zrate.csv': 'root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/data-dqm/zlumi',
                            'zlumi.root': 'root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/data-dqm/zlumi',
                            'zlumi.root_lumicomp.eps': 'root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/data-dqm/zlumi',
                            }

dqconfig.dbConnection  = "oracle://ATLAS_COOLWRITE;schema=ATLAS_COOLOFL_GLOBAL;dbname=CONDBR2;"
dqconfig.dqmfOfl       = "/GLOBAL/DETSTATUS/DQMFOFL"
dqconfig.dbTagName    = "DetStatusDQMFOFL-%(stream)s-pass1"
dqconfig.dbTagNameESn       = "DetStatusDQMFOFL-%(stream)s-ES%(procpass)s"
dqconfig.shiftOfl       = "/GLOBAL/DETSTATUS/SHIFTOFL"

#authentication file at Tier0 (uncomment this option at Tier0 only)
dqconfig.auth       = "/afs/cern.ch/atlas/project/tzero/var"

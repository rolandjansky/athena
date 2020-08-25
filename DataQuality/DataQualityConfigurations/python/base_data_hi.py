# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


from __future__ import print_function

from DataQualityUtils.DQWebDisplayConfig import DQWebDisplayConfig
import os
from ._resolve_data_path import resolve_data_path

hcfg_dir = resolve_data_path('DataQualityConfigurations')
if hcfg_dir:
    print ("Found DataQualityConfigurations data directory %s, using it" % hcfg_dir)
else:
    hcfg_dir = os.getcwd()
    print ("DataQualityConfigurations data directory not found, attempting to use $PWD instead.")
print ("Looking for heavyions_*.hcfg files in %s" % (hcfg_dir))

isprod = os.environ.get('DQPRODUCTION') == '1'

dqconfig = DQWebDisplayConfig()
dqconfig.config         = "Heavy Ion Data"
dqconfig.hcfg           = os.environ.get('DQC_HCFG_HEAVYIONS_RUN', "%s/heavyions_run.hcfg" % hcfg_dir)
dqconfig.hcfg_min10     = os.environ.get('DQC_HCFG_HEAVYIONS_MINUTES10', "%s/heavyions_minutes10.hcfg" % hcfg_dir)
dqconfig.hcfg_min30     = os.environ.get('DQC_HCFG_HEAVYIONS_MINUTES30', "%s/heavyions_minutes30.hcfg" % hcfg_dir)
dqconfig.server         = os.environ.get('DQC_SERVERS', "aiatlas009.cern.ch,aiatlas016.cern.ch,aiatlas039.cern.ch").split(',') if isprod else []
dqconfig.eosResultsDir  = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/data-dqm/han_results/tier0/collisions/" if isprod else ""
dqconfig.histogramCache = "/afs/cern.ch/user/a/atlasdqm/w1/histogram_web_display_cache" if isprod else ''
dqconfig.hanResultsDir  = "/dqmdisk0/han_results/tier0/collisions" if isprod else '/afs/cern.ch/user/a/atlasdqm/dqmdisk/han_results/test'
dqconfig.doHandi        = False
dqconfig.htmlDir        = "/dqmdisk0/www/tier0/collisions" if isprod else '/afs/cern.ch/user/a/atlasdqm/dqmdisk/www/test'
dqconfig.htmlWeb        = "http://atlasdqm.cern.ch/tier0/collisions"
dqconfig.runlist        = "runlist_collisions.xml"
dqconfig.indexFile      = "results_collisions.html"
dqconfig.lockFile       = "DQWebDisplay_collisions.lock"
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

#authentication file at Tier0 (uncomment this option at Tier0 only)
dqconfig.auth       = "/afs/cern.ch/atlas/project/tzero/var"

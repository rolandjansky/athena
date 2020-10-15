# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DataQualityUtils.DQWebDisplayConfig import DQWebDisplayConfig
import os
from ._resolve_data_path import resolve_data_path

hcfg_dir = resolve_data_path('DataQualityConfigurations')
if hcfg_dir:
    print "Found DataQualityConfigurations data directory %s, using it" % hcfg_dir
else:
    hcfg_dir = os.getcwd()
    print "DataQualityConfigurations data directory not found, attempting to use $PWD instead."
print "Looking for collisions_*.hcfg files in %s" % (hcfg_dir)

isprod = os.environ.get('DQPRODUCTION') == '1'

dqconfig = DQWebDisplayConfig()
dqconfig.config         = "Beam"
dqconfig.hcfg           = os.environ.get('DQC_HCFG_COLLISIONS_RUN', "%s/collisions_run.hcfg" % hcfg_dir)
dqconfig.hcfg_min10     = os.environ.get('DQC_HCFG_COLLISIONS_MINUTES10', "%s/collisions_minutes10.hcfg" % hcfg_dir)
dqconfig.hcfg_min30     = os.environ.get('DQC_HCFG_COLLISIONS_MINUTES30', "%s/collisions_minutes30.hcfg" % hcfg_dir)
dqconfig.server         = os.environ.get('DQC_SERVERS', "aiatlas009.cern.ch,aiatlas016.cern.ch,aiatlas039.cern.ch").split(',') if isprod else []
dqconfig.eosResultsDir  = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/data-dqm/han_results/tier0/collisions/" if isprod else ""
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

#dqconfig.dbConnectionHisto = "oracle://ATLAS_COOLWRITE;schema=ATLAS_COOLOFL_GLOBAL;dbname=COMP200;"
#dqconfig.dqmfOflHisto      = "/GLOBAL/DETSTATUS/DQMFOFLH"
#dqconfig.dbTagNameH    = "DetStatusDQMFOFLH-IDCosmics08-pass1"

dqconfig.auth       = "/afs/cern.ch/atlas/project/tzero/var"

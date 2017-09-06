#--------------------------------------------------------------
# Full job is a list of algorithms
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
alg = AlgSequence()

#--------------------------------------------------------------
# Enable monitoring
#--------------------------------------------------------------
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doSemiDetailedMonitoring = True

from AthenaCommon.AppMgr import ToolSvc
from TrigFTKBankGen.TrigFTKBankGenConf import *

#--------------------------------------------------------------
# initialize algo(s)
#--------------------------------------------------------------
CachedBankGen = FTKCachedBankGenAlgo("CachedBankGen")

#--------------------------------------------------------------
# 
#--------------------------------------------------------------
from AthenaCommon.AppMgr import theApp
# Number of events to be processed
theApp.EvtMax = 1

from AthenaCommon.AppMgr import ServiceMgr

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output += ["outputHisto DATAFILE='outputHisto.root' OPT='RECREATE'"]

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
CachedBankGen.OutputLevel = DEBUG


#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------

ConfigDir = "/nfs/dust/atlas/user/britzger/19.3.0/Trigger/TrigFTK/TrigFTKSim/config/"
CachedBankGen.ssmapAM_path = ConfigDir + "ss_file/raw_30x32x72Ibl.ss"

CachedBankGen.curreg = 24  ## is indeed specified by the inputbank_path

CachedBankGen.maxpatt = 4096*4096

CachedBankGen.infile = "/afs/desy.de/user/b/britzger/dust/19.3.0/test.root"
CachedBankGen.ccacheOut = "test.ccache.root"
CachedBankGen.pcacheOut = "test.pcache.root"
CachedBankGen.nSub = 1
CachedBankGen.iSub = 0

alg += CachedBankGen
#==============================================================
#
# End of job options file
#
###############################################################

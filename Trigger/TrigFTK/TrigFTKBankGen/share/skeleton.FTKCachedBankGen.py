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
theApp.EvtMax = 1 # Number of events to be processed

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

# --- 'fixed arguments'

# --- mandatory runArgs
runArgsFromTrfMandatory = ['pmap_path', 'rmap_path', 'ssmapTSP_path', 'ssmapAM_path',
                           'curreg','cursubreg','infile','outfile','IBLMode','hwmodeid_tsp']


# --- Check that the arguments in the mandatory list are set
for mandArg in runArgsFromTrfMandatory:
    print "Seeking mandArg: " + mandArg
    if not hasattr(runArgs, mandArg):
        raise RuntimeError, 'Failed to find mandatory runtime argument for transform: ' + mandArg
    else:
        attr = getattr(runArgs, mandArg)
        print "Setting mandArg: " + mandArg
        setattr(CachedBankGen, mandArg, getattr(runArgs, mandArg))


# --- optional arguments
TSPBankGen.maxpatt = -1

runArgsFromTrfOptional = { 'maxpatt': -1 # IBLMode, hwmodeid_ccache, hwmodeid_pcache
                           }
skipArgs = []

# --- Set the PattBankGenRoot properties
for runArgName in runArgsFromTrfOptional.keys():
	print 'Setting FTKCachedBankGen argument'+runArgName
	if runArgName in skipArgs:
		print 'Skipping ' + runArgName
		continue
	if hasattr(runArgs, runArgName):
		print "if " + runArgName
		setattr(CachedBankGen, runArgName, getattr(runArgs, runArgName))
	else:
		print "else " + runArgName
		setattr(CachedBankGen, runArgName, runArgsFromTrfOptional[runArgName])


#--------------------------------------------------------------
# Add algo
#--------------------------------------------------------------
alg += CachedBankGen

#--------------------------------------------------------------


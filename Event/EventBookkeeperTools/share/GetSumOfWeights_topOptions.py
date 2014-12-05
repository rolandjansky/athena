#-----------------------------------------------------------------------------
# Job options to run (or submit to a grid job) in order to get one output
# root file that contains a histogram with the sum-of-weights and number of
# events that were originally processed, e.g., by the DerivationFramework job.
#-----------------------------------------------------------------------------


# Define which events to process (default: all)
theApp.SkipEvents = vars().get('SKIPEVT', 0)
theApp.EvtMax     = vars().get('EVTMAX', -1)


# Read the provided input files
inFileName = vars().get("FNAME", "/afs/cern.ch/atlas/project/PAT/xAODs/r5591/mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e1727_s1933_s1911_r5591/AOD.01494882._111853.pool.root.1")
inFileList = [ inFileName ]
from glob import glob
if vars().has_key('FDIR'):
    print "Using FDIR=", FDIR
    inFileList = glob( FDIR+"/*.root*")
    pass
import AthenaPoolCnvSvc.ReadAthenaPool
print "Using inFileList=", inFileList
svcMgr.EventSelector.InputCollections = inFileList


# Change the event printout interval, if you want to
svcMgr += CfgMgr.AthenaEventLoopMgr(EventPrintoutInterval=200)


# ====================================================================
# Define your output root file holding the histograms using MultipleStreamManager
# ====================================================================
rootStreamName = "EventBookkeeperStream"
rootFileName   = "EventBookkeeper.root"
rootDirName    = "/Hists"
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
EBKStream = MSMgr.NewRootStream( rootStreamName, rootFileName )


# Fetch the AthAlgSeq, i.e., one of the existing master sequences where one should attach all algorithms
algSeq = CfgMgr.AthSequencer("AthAlgSeq")

# create the alg that creates the histogram
algSeq += CfgMgr.GetSumOfWeightsAlg( "GetSumOfWeightsDefaultAlg",
                                     #OutputLevel    = DEBUG,
                                     RootStreamName = rootStreamName,
                                     RootDirName    = rootDirName
                                     )

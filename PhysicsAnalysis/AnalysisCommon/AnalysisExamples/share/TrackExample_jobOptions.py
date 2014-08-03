# Author: Ketevi A. Assamagan
# CERN, February 13, 2005

# Set up the reading of an xAOD file using RecExCommon:
FNAME = "/afs/cern.ch/atlas/project/PAT/data/xAOD/" \
    "valid2.147807.PowhegPythia8_AU2CT10_Zmumu.digit." \
    "AOD.e2657_s1933_s1964_r5493.pool.root"
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Create and configure the example algorithm:
from AnalysisExamples.AnalysisExamplesConf import TrackExample
trackExample = TrackExample()

# The stream/file to write to
trackExample.RootStreamName = "/AANT"
trackExample.RootDirName    = "TrackParticle"

# Track isolation cone
trackExample.TrackIsolationCone = 16

# Event weight
trackExample.EventWeight = 1.0

# Track Momentum cut
trackExample.TrackMomentumCut = 1.0 * GeV

# Message output level for this algorithm
trackExample.OutputLevel = DEBUG

# Print the algorithm's properties:
print trackExample

# Add the algorithm to the sequence:
topSequence += trackExample

# Number of Events to process
theApp.EvtMax = 10

#########################################
#Setup TTree registration Service
# save ROOT histograms and NTuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
#Root Ntuple output file and name
ServiceMgr.THistSvc.Output += [
    "AANT DATAFILE='TrackExample.hist.root' OPT='RECREATE'"
    ] 

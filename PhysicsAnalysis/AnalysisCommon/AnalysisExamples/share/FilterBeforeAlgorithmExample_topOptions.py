# Author: Ketevi A. Assamagan
# CERN, February 13, 2005

#get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr

# Detector Description if needed
DetDescrVersion="ATLAS-CSC-01-02-00" # default - uncomment to set the geometry
include ("RecExCommon/RecExCommon_flags.py")
#DetFlags.ID_setOff()
#DetFlags.Calo_setOff()
#DetFlags.Muon_setOff()
include( "RecExCommon/AllDet_detDescr.py" )

# The ESD or AOD input data
# note that here, you can use the ESD or the AOD as input
# since the collections of TrackParticles are both in the ESD and in the AOD
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ "AOD.pool.root" ] 

include( "TrackIsolationTools/TrackIsolationTool_jobOptions.py" )

# use sequencer for filtering
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AlgSequence import AthSequencer
Sequencer = AthSequencer( "MySequencer" )
Sequencer.StopOverride = False
topSequence += Sequencer

##### Define the filter algorithm
from AnalysisExamples.AnalysisExamplesConf import FilterExample
FilterExample = FilterExample(
                 Jets      = "Cone7H1TopoParticleJets",
                 Electrons = "ElectronAODCollection",
                 Muons     = "StacoMuonCollection",
                 Photons   = "PhotonAODCollection",
                 TauJets   = "TauRecContainer",
                 MissingET = "MET_Final",
                 OutputLevel = DEBUG
                )
print FilterExample
Sequencer += FilterExample
######## End Filter algorithm

######## Now define the algorithms to run after the filter passed
from AnalysisExamples.AnalysisExamplesConf import TrackExample
TrackExample = TrackExample()
TrackExample.TrackIsolationTool = TrackIsolationTool
TrackExample.TrackParticleContainerName = "TrackParticleCandidate"
TrackExample.TrackIsolationCone = 0.4
TrackExample.EventWeight = 1.0
TrackExample.TrackMomentumCut = 0.0*GeV
TrackExample.OutputLevel = INFO
print TrackExample
Sequencer += TrackExample

############# End of the algorithms to run

# Output level and Number of Events to process
ServiceMgr.MessageSvc.OutputLevel = INFO 
theApp.EvtMax = 500

#########################################
#Setup TTree registration Service
# save ROOT histograms and NTuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
#Root Ntuple output file and name
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='TrackExample.hist.root' OPT='RECREATE'"] 

# For interactive analysis in athena
#import PyAnalysisCore.InitPyAnalysisCore


# Author: Ketevi A. Assamagan
# BNL, November 20, 2004

# Read in the AOD from POOL - AOD.pool.root
# this file is produced by running the AOD builders of the RDO raw data

# import the analysis flags
from AnalysisExamples.AnalysisFlags import AnalysisFlags

# back navigation and interactive session flags
AnalysisFlags.DoNavigation      = False
AnalysisFlags.DoInteractive     = True

include( "AnalysisExamples/AnalysisCommon_jobOptions.py")

AnalysisFlags.Print()

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr
# The AOD input file
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = ["AOD.pool.root"]

# The AOD input file for large statistics
#include ( "aodinput.py" )

###########################################
# setup TTree registration Service
# save ROOT histograms and NTuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='MiscellaneousExamples.root' OPT='RECREATE'"]
from AnalysisTools.AnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream = AANTupleStream()

AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ]
AANTupleStream.OutputName = 'MisellaneousExamples.root'
AANTupleStream.WriteInputDataHeader = True
AANTupleStream.OutputLevel = WARNING

# The user analysis algorithm to be executed
from AnalysisExamples.AnalysisExamplesConf import MiscellaneousExamples 
topSequence += MiscellaneousExamples()

MiscellaneousExamples = MiscellaneousExamples()
# The user analysis cuts - default values to changed by the user as needed

# set the names of the particle containers to be retrieved from StoreGate
# must be the same name used to build the AOD container
# do not touch these if you do know what you are doing

MiscellaneousExamples.JetContainerName = "Cone4H1TowerJets"
MiscellaneousExamples.ElectronContainerName = "ElectronAODCollection"
MiscellaneousExamples.MuonContainerName = "StacoMuonCollection"
MiscellaneousExamples.EventWeight = 1.0
MiscellaneousExamples.JetPtCut = 20.0*GeV
MiscellaneousExamples.DeltaMjj = 25.0*GeV
MiscellaneousExamples.OutputLevel = INFO

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = ERROR

# Number of Events to process
theApp.EvtMax = 200

# For interactive analysis in athena
if AnalysisFlags.DoInteractive:
   include( "PyAnalysisCore/InitPyAnalysisCore.py" )


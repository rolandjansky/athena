# Author: Shuwei YE
# BNL, September 2009
# modified by Vivek Jain, Oct. 30, 2009
# Run this through AnalysisMaster.py
#
# get a handle on the ServiceManager which holds all the services 
from AthenaCommon.AppMgr import ServiceMgr

# the POOL converters
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py" )

# The input file already specified near the top of this file
ServiceMgr.EventSelector.InputCollections = jp.AthenaCommonFlags.FilesInput()
#
ServiceMgr.EventSelector.BackNavigation = False 

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AnalysisExamples.AnalysisExamplesConf import VFitZmmOnAOD
topSequence += VFitZmmOnAOD()

# The user analysis cuts - default values to changed by the user as needed

# set the names of the particle containers to be retrieved from StoreGate
# must be the same name used to build the AOD container
# do not touch these if you do know what you are doing

# Muon Parameters
topSequence.VFitZmmOnAOD.MuonContainer = "MuidMuonCollection"
topSequence.VFitZmmOnAOD.MuonEtCut  = 6.0*GeV
topSequence.VFitZmmOnAOD.MuonEtaCut = 2.5

# message level for VFitZmmOnAOD
topSequence.VFitZmmOnAOD.OutputLevel = INFO

# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='VFitZmmOnAOD.root' OPT='RECREATE'"]
from AnalysisTools.AnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
topSequence.AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ]
topSequence.AANTupleStream.OutputName = 'VFitZmmOnAOD.root'
topSequence.AANTupleStream.WriteInputDataHeader = True
topSequence.AANTupleStream.OutputLevel = WARNING
# Root Ntuple output file and name
#theApp.HistogramPersistency = "ROOT"
#theApp.HistogramPersistencySvc.OutputFile = "VFitZmmOnAOD.root"

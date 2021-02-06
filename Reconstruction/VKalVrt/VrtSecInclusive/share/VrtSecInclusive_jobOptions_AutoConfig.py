# This uses AutoConfiguration
# Vivek Jain - Mar 2010
# Modified: VJ, Dec 2011 - Use AthAlgorithm instead of CBNT_AthenaAwareBase


# to run on data at BNL may need to set HOTDISK externally

from AthenaCommon.AthenaCommonFlags import jobproperties as jp

#jp.AthenaCommonFlags.FilesInput = ['/usatlas/u/vj/vj_bnl_local/datafile/mc10_7TeV.105001.pythia_minbias.recon.AOD.e574_s932_s946_r1649_tid191045_00/AOD.191045._001937.pool.root.1']

#jp.AthenaCommonFlags.FilesInput = [ '/afs/cern.ch/atlas/maxidisk/d49/AOD.191045._001937.pool.root.1']
jp.AthenaCommonFlags.FilesInput = [
'/afs/cern.ch/work/v/vkost/vkdata/valid2.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.AOD.e3099_s1982_s1964_r6024_tid04652558_00/AOD.04652558._000029.pool.root.1',
]
################################################################
# more single input files to test on
#
#jp.AthenaCommonFlags.FilesInput = [ '/usatlas/u/vj/MyTest/15.5.2/run/InDetRecAOD_new_Z400.root']
#
########## if running on multiple files
#
#DATAPATH = '/usatlas/u/vj/vj_bnl_local/datafile/mc10_7TeV.105001.pythia_minbias.recon.AOD.e574_s932_s946_r1649_tid191045_00/'

#from glob import glob
#INPUT = glob(DATAPATH + 'AOD*.root*')
##INPUT = glob(DATAPATH + 'ESD*.root*')
#print INPUT
#jp.AthenaCommonFlags.FilesInput = INPUT

# from david R.
from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsSummarySvc
InDetSCT_ConditionsSummarySvc = SCT_ConditionsSummarySvc(name = "InDetSCT_ConditionsSummarySvc")
ServiceMgr += InDetSCT_ConditionsSummarySvc

from RecExConfig.RecFlags import rec

# get inputFileSummary - will use it to extract info for MC/DATA
from RecExConfig.InputFilePeeker import inputFileSummary

# import the data types 
import EventKernel.ParticleDataType

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr

include ("RecExCond/RecExCommon_flags.py")

include( "RecExCond/AllDet_detDescr.py" )
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

# write out a summary of the time spent
theApp.AuditAlgorithms=True
theAuditorSvc.Auditors  += [ "ChronoAuditor"]


#----------------------------------------------------------
#  VrtSecInclusive creates also TrackSummary tool.
#   TrackSummary tool creates InDetExtrapolator and AtlasMagneticFieldTool
#
from VrtSecInclusive.VrtSecInclusive import VrtSecInclusive
topSequence += VrtSecInclusive()

#topSequence.VrtSecInclusive.OutputLevel = DEBUG
topSequence.VrtSecInclusive.OutputLevel = INFO
#topSequence.VrtSecInclusive.CutBLayHits = 1
#topSequence.VrtSecInclusive.CutPixelHits = 1
topSequence.VrtSecInclusive.CutSctHits = 1
topSequence.VrtSecInclusive.TrkA0ErrCut = 200000
topSequence.VrtSecInclusive.TrkZErrCut = 200000
topSequence.VrtSecInclusive.a0TrkPVDstMinCut = 5.0
topSequence.VrtSecInclusive.TrkPtCut = 300
topSequence.VrtSecInclusive.SelVrtChi2Cut=4.5
topSequence.VrtSecInclusive.CutSharedHits=5
topSequence.VrtSecInclusive.TrkChi2Cut=5.0
topSequence.VrtSecInclusive.TruthTrkLen=1
topSequence.VrtSecInclusive.DoSAloneTRT=False
topSequence.VrtSecInclusive.DoTruth = False
# following is when there is no GEN_AOD in input file,
# e.g., when I run on output of InDetRecExample or on a ESD file
# when running on AOD output of InDetRecEx, explicitly uncomment the next line and comment out rec.readESD
#     topSequence.VrtSecInclusive.MCEventContainer = "TruthEvent"

if rec.readESD():
    topSequence.VrtSecInclusive.MCEventContainer = "TruthEvent"

if 'IS_SIMULATION' in inputFileSummary['evt_type']:
    topSequence.VrtSecInclusive.DoTruth=True


from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
InclusiveVxFitterTool = Trk__TrkVKalVrtFitter(name                = "InclusiveVxFitter",
	                                      Extrapolator        = ToolSvc.AtlasExtrapolator,
	                                      IterationNumber     = 30
					     )
ToolSvc +=  InclusiveVxFitterTool;
InclusiveVxFitterTool.OutputLevel = INFO
topSequence.VrtSecInclusive.VertexFitterTool=InclusiveVxFitterTool
topSequence.VrtSecInclusive.Extrapolator = ToolSvc.AtlasExtrapolator

print VrtSecInclusive

# The input file
ServiceMgr.EventSelector.InputCollections = jp.AthenaCommonFlags.FilesInput()


##########################################
# setup TTree registration Service
# save ROOT histograms and NTuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='vert.root' OPT='RECREATE'"]
from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
topSequence.AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ]
topSequence.AANTupleStream.OutputName = 'vert.root'
topSequence.AANTupleStream.WriteInputDataHeader = True
topSequence.AANTupleStream.OutputLevel = INFO

# Number of Events to process
theApp.EvtMax = -1
#theApp.EvtMax = 2
#ServiceMgr.EventSelector.SkipEvents = 2
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 9999999 # all messages 

## uncomment following lines to dump truth info

#from TruthExamples.TruthExamplesConf import DumpMC
#topSequence += DumpMC()
# fix this by hand for running on AOD or ESD
#DumpMC.McEventKey = "GEN_AOD"
#DumpMC.McEventKey = "TruthEvent"

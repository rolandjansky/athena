# For some files, AutoConfig may not work. this one should work

# from david cote/tadashi
     
from RecExConfig.RecFlags import rec
     
rec.AutoConfiguration.set_Value_and_Lock([])
     
# import the data types
import EventKernel.ParticleDataType
    
# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr
     
#from AtlasGeoModel import GeoModelInit
#from AtlasGeoModel import SetGeometryVersion
     
DetDescrVersion = "ATLAS-GEO-16-00-00"
     
#from AthenaCommon.DetFlags      import DetFlags
include ("RecExCommon/RecExCommon_flags.py")
#DetFlags.ID_setOn()
# --- and switch off all the rest
#DetFlags.Calo_setOff()
#DetFlags.Muon_setOff()
     
include( "RecExCond/AllDet_detDescr.py" )
include( "IOVDbSvc/IOVRecExCommon.py" ) 
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
     
#GeoModelSvc = Service( "GeoModelSvc" )
#GeoModelSvc.AtlasVersion = "ATLAS-GEO-02-01-00"
#GeoModelSvc.IgnoreTagDifference = False
     
#print GeoModelSvc

from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
     
# the Dlls
#include ( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
#include ( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py" )
#include ( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py" )
#include ( "EventAthenaPool/EventAthenaPool_joboptions.py" )
     
# write out a summary of the time spent
#theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  += [ "NameAuditor"]
#theAuditorSvc.Auditors  += [ "ChronoAuditor", "MemStatAuditor"]
     
#----------------------------------------------------------
#  VrtSecInclusive creates also TrackSummary tool.
#   TrackSummary tool creates InDetExtrapolator and AtlasMagneticFieldTool
#
from VrtSecInclusive.VrtSecInclusive import VrtSecInclusive
topSequence += VrtSecInclusive()
#topSequence.VrtSecInclusive.OutputLevel = VERBOSE
topSequence.VrtSecInclusive.OutputLevel = INFO
#topSequence.VrtSecInclusive.CutBLayHits = 1
#topSequence.VrtSecInclusive.CutPixelHits = 1
topSequence.VrtSecInclusive.CutSctHits = 1
topSequence.VrtSecInclusive.DoTruth = True
topSequence.VrtSecInclusive.TrkA0ErrCut = 200000
topSequence.VrtSecInclusive.TrkZErrCut = 200000
topSequence.VrtSecInclusive.a0TrkPVDstMinCut = 5.0
topSequence.VrtSecInclusive.TrkPtCut = 300
topSequence.VrtSecInclusive.SelVrtChi2Cut=4.5
topSequence.VrtSecInclusive.TrkChi2Cut=5.0
topSequence.VrtSecInclusive.TruthTrkLen=1
topSequence.VrtSecInclusive.CutSharedHits=5
topSequence.VrtSecInclusive.DoSAloneTRT=False

# next line needed if running on ESD or output of InDetRecEx , otherwise GEN_AOD
#topSequence.VrtSecInclusive.MCEventContainer = "TruthEvent"
topSequence.VrtSecInclusive.MCEventContainer = "GEN_AOD"
#

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
	
# The AOD input file
ServiceMgr.EventSelector.InputCollections = ["AOD.pool.root"]

### Following lines is to read AOD files without listing all of them individually
###
#DATAPATH = '/direct/usatlas+u/vj/vj_tracking/datafile/single_pion_task25576/'
#DATAPATH = '/direct/usatlas+u/vj/vj_tracking/datafile/minbias_task25553/'
#DATAPATH = '/direct/usatlas+u/vj/vj_tracking/datafile/minbias_task26982/'
#DATAPATH = '/direct/usatlas+u/vj/vj_tracking/datafile/mc09_7TeV.105004.pythia_ddiff.recon.ESD.e514_s764_s767_r1229_tid126162_00/'
	#DATAPATH = '/direct/usatlas+u/vj/vj_tracking/datafile/mc09_7TeV.105003.pythia_sdiff.recon.ESD.e514_s764_s767_r1229_tid126163_00/'
from glob import glob
#INPUT = glob(DATAPATH + 'ESD*.root*')
#print INPUT
#ServiceMgr.EventSelector.InputCollections = INPUT
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
#ServiceMgr.EventSelector.SkipEvents = 2
ServiceMgr.MessageSvc.OutputLevel = INFO
	
## uncomment following lines to dump truth info

#from TruthExamples.TruthExamplesConf import DumpMC
#topSequence += DumpMC()
#DumpMC.McEventKey = "GEN_AOD"

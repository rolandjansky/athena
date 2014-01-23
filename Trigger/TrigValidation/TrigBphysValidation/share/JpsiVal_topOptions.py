# Author: Ketevi A. Assamagan
# BNL, June 12, 2004
# Author: Vivek Jain
# Oct 30, 2009
# ------------------------------------------------------------

# THIS SCRIPT MUST BE RUN FROM THE ====ANALYSISMASTER.PY=====

# i.e., put the name of this file in AnalysisMaster.py, and do,
# athena AnalysisMaster.py
# ------------------------------------------------------------

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AppMgr import ServiceMgr
# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool
#EventSelector.BackNavigation = True

# this line is needed for reading AOD files made with pre-14.1.X releases
# If you leave it uncommented, you can still read post-14.1.X releases

#include ("JetRec/ParticleJetCompatibility.py")

# Get the selection, overlap checking and overlap removal tools  
#include ( "UserAnalysisUtils/UserAnalysisSelectionTool_jobOptions.py" )
#include ( "UserAnalysisUtils/UserAnalysisPreparationTool_jobOptions.py" )
#include ( "UserAnalysisUtils/UserAnalysisOverlapCheckingTool_jobOptions.py" )
#include ( "UserAnalysisUtils/UserAnalysisOverlapRemovalTool_jobOptions.py" )

# The input file already specified near the top of this file
ServiceMgr.EventSelector.InputCollections = jp.AthenaCommonFlags.FilesInput()


from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
topSequence = AlgSequence()


#======================================================================================
# L u m i B l o c k  j o b  o p t i o n s 
#=========================================
# add LumiBlockMetaDataTool to ToolSvc and configure
#from LumiBlockComps.LumiBlockCompsConf import LumiBlockMetaDataTool
#ToolSvc += LumiBlockMetaDataTool( "LumiBlockMetaDataTool" )
#LumiBlockMetaDataTool.calcLumi = False # False by default
#LumiBlockMetaDataTool.storeXMLFiles = True
#LumiBlockMetaDataTool.applyDQCuts = True
#LumiBlockMetaDataTool.OutputLevel = INFO

# add ToolSvc.LumiBlockMetaDataTool to MetaDataSvc
#from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import MetaDataSvc
#svcMgr += MetaDataSvc( "MetaDataSvc" )
#svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.LumiBlockMetaDataTool ]

# Configure the goodrunslist selector tool
#from GoodRunsLists.GoodRunsListsConf import *
#ToolSvc += GoodRunsListSelectorTool() 
#GoodRunsListSelectorTool.OutputLevel = INFO
#GoodRunsListSelectorTool.GoodRunsListVec = [ 'data11_7TeV.periodAllYear_DetStatus-v18-pro08-05_CoolRunQuery-00-03-98_Muon.xml' ]  # <<<<--- Edit this line!
#GoodRunsListSelectorTool.PassThrough = False

## This Athena job consists of algorithms that loop over events;
## here, the (default) top sequence is used:
#job = AlgSequence()
#seq = AthSequencer("AthFilterSeq")
#topSequence += seq

#from GoodRunsListsUser.GoodRunsListsUserConf import *
#seq += GRLTriggerSelectorAlg('GRLTriggerAlg1')
### In the next line, pick up correct name from inside xml file!
#seq.GRLTriggerAlg1.GoodRunsListArray = ['Muon']        

#======================================================================================


from TrigBphysValidation.TrigBphysValidationConf import JpsiVal
topSequence += JpsiVal()


############# The properties of the AnalysisSkeleton Algorithm
#topSequence.AnalysisSkeleton.AnalysisSelectionTool       = ToolSvc.UserAnalysisSelectionTool
#topSequence.AnalysisSkeleton.AnalysisPreparationTool     = ToolSvc.UserAnalysisPreparationTool
#topSequence.AnalysisSkeleton.AnalysisOverlapCheckingTool = ToolSvc.UserAnalysisOverlapCheckingTool
#topSequence.AnalysisSkeleton.AnalysisOverlapRemovalTool  = ToolSvc.UserAnalysisOverlapRemovalTool

#IsAtlfast = False

topSequence.JpsiVal.OutputLevel = DEBUG
# track algo 1=SiTrack, 2=IdScan
topSequence.JpsiVal.TrackAlgo=2
#topSequence.JpsiVal.TriggerName="L1_MU0"
topSequence.JpsiVal.TriggerName="L1_MU4"
# to run onA OD
topSequence.JpsiVal.McEventContainer="TruthEvent"

#if 'IS_SIMULATION' in inputFileSummary['evt_type']:
#   topSequence.JpsiVal.DoTruth                = True

print JpsiVal

# Change the selections if necesary
# Please read jO files in UserAnalysisUtils/ for other options
#
#ToolSvc.UserAnalysisSelectionTool.IsAtlfastData = IsAtlfast
##topSequence.AnalysisSkeleton.AnalysisSelectionTool.ElectronIsEMFlag="Loose"
#ToolSvc.UserAnalysisSelectionTool.MuonPt=6.0*GeV
#ToolSvc.UserAnalysisSelectionTool.JetPt=20.0*GeV
#ToolSvc.UserAnalysisSelectionTool.ElectronEtaWindowCut=False
#ToolSvc.UserAnalysisSelectionTool.ElectronEtaWindowCenter=1.445
#ToolSvc.UserAnalysisSelectionTool.ElectronEtaWindow=0.075


##########################################
##
## Set up the trigger part
##
##########################################

#topSequence.JpsiVal.DoTrigger = True
#if topSequence.JpsiVal.DoTrigger:
# set up trigger config service
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg =  TriggerConfigGetter()

## Done with trigger setup

##########################################
# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='JpsiVal.aan.root' OPT='RECREATE'" ]
from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream = AANTupleStream()
AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ]
AANTupleStream.OutputName = 'JpsiVal.aan.root'
AANTupleStream.WriteInputDataHeader = True
AANTupleStream.OutputLevel = WARNING


###################### For interactive analysis
#include ("PyAnalysisCore/InitPyAnalysisCore.py")

from GaudiSvc.GaudiSvcConf import AuditorSvc
ServiceMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE

#### test MC dump ###

#from TruthExamples.TruthExamplesConf import DumpMC
#topSequence += DumpMC()
#DumpMC.McEventKey = "GEN_AOD"

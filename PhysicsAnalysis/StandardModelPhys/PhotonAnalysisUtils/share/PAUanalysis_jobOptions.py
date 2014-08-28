from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.GlobalFlags import globalflags
import AthenaPoolCnvSvc.ReadAthenaPool
import subprocess
import sys
import commands
PAULog = logging.getLogger( "PAULog" ) # We should use this to breadcast messages

#-------------------------------------------------------------------------
# Input Dataset
#-------------------------------------------------------------------------
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()
#-------------------------------------------------------------------------


#-------------------------------------------------------------------------
# All of the configuration for PAU should be done through this structure - allows us to auto-produce
# PAU NTuples using official transforms
#-------------------------------------------------------------------------
from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import PAUflags
#-------------------------------------------------------------------------

if PAUflags.DumpStoreGate():
	StoreGateSvc.OutputLevel = INFO
	StoreGateSvc.Dump = True  #true will dump data store contents


from RecExConfig.AutoConfiguration import ConfigureFromListOfKeys
ConfigureFromListOfKeys (['ProjectName',
			  'BeamType',
			  'RealOrSim',
			  'DoTruth',
			  'LumiFlags',
			  'FieldAndGeo',
			  'ConditionsTag',
			  'InputType'])

PAUflags.UseRealData = (globalflags.DataSource=='data') # Real data or MC?

if PAUflags.UseRealData():
	PAUflags.ProductionRelease = 15.5
	from AthenaCommon.GlobalFlags import GlobalFlags
	GlobalFlags.DataSource.set_data()
	recoveryAlreadyDone = True


# The names of the trigger lines that are run are setup in CommonJobOptions.py
# based on the value of this variable
# Use 'R15' for AODs reconstructed with Athena 15.x, 'R14' for older versions
if ( PAUflags.ProductionRelease.StoredValue < 15.2 ) :
      TriggerNames = 'R14'
else :
      TriggerNames = 'R15'



from RecExConfig.RecFlags  import rec
PAUdoAOD = rec.readAOD()
PAUdoESD = rec.readESD()
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doForwardDet.set_Value_and_Lock(False)
include ("RecExCommon/RecExCommon_topOptions.py")

######################################################################################################################
# set containers names

if ( PAUdoESD ) :
	CaloCells = "AllCalo"
	MCEventCollection = "TruthEvent"
	MCParticleContainer = "MyConv"+MCEventCollection

if not 'PhotonContainer' in dir() :
	PhotonContainer = "PhotonAODCollection"
if not 'ElectronContainer' in dir() :
	ElectronContainer = "ElectronAODCollection"
if not 'PhotonDetailContainer' in dir() :
	PhotonDetailContainer = "egDetailAOD"
if not 'ElectronDetailContainer' in dir() :
	ElectronDetailContainer = "egDetailAOD"

if not 'PhotonClusterCollection' in dir() :
	PhotonClusterCollection = "egClusterCollection"

if not 'MuonContainer' in dir() :
	MuonContainer = "StacoMuonCollection"

if not 'TruthJetsCollection' in dir() :
	if ( PAUflags.ProductionRelease.StoredValue < 15.3 ) :
		TruthJetsCollection = "Cone4TruthJets"
	else :
		TruthJetsCollection = "AntiKt4TruthJets"

#the following are set above, for ESD:
if not 'CaloCells' in dir() :
	CaloCells = "AODCellContainer"
if not 'MCEventCollection' in dir() :
	MCEventCollection = "GEN_AOD"
if not 'MCParticleContainer' in dir() :
	MCParticleContainer = "SpclMC"

# the following two will be changed in case of photon recovery rerunning...
PhotonContainerMerged       = PhotonContainer
PhotonsDetailsContainerName = PhotonDetailContainer
PhotonDetailContainerMerged = PhotonDetailContainer
PhotonVxCandContainerMerged = "ConversionCandidate"
PhotonClusterContainerMerged = PhotonClusterCollection
	
# -------------------------------------------------- PhotonRecoveryTool --------------------------------------------------
if ( PAUflags.ProductionRelease.StoredValue < 15.5 ) :
	recoveryAlreadyDone = False
	if not 'usePhotonRecoveryTool' in dir() :
		usePhotonRecoveryTool = True
	if not 'usePAUconversionFlagTool' in dir():
		usePAUconversionFlagTool = False
else :
	recoveryAlreadyDone = True
	print "#######################################################################################"
	print "## recoveryAlreadyDone ==> Turning off PAUconversionFlagTool and PhotonRecoveryTool  ##"
	print "#######################################################################################"
	usePAUconversionFlagTool = False
	usePhotonRecoveryTool = False

if ( usePhotonRecoveryTool==True and usePAUconversionFlagTool==True ):
	print "#################################################################################################"
	print "##    You have to choose between  PhotonRecoveryTool  and  PAUconversionFlagTool  (not both!)  ##"
	print "#################################################################################################"
	sys.exit(1)

if not PAUflags.DoReRunEgamma.statusOn:
	if ( PAUflags.ProductionRelease.StoredValue < 15.2 and usePhotonRecoveryTool==True ) :
		PAUflags.DoReRunEgamma = True
	else :
		PAUflags.DoReRunEgamma = False  

if (usePhotonRecoveryTool==True):
	PhotonContainerMerged = "PhotonAODCollection"
	PhotonsDetailsContainerName = "AllPhotonsDetails"
	if not 'doPhotonRecoveryAlg'  in dir():
		doPhotonRecoveryAlg = True
	print "#######################################################################################"
	if (PAUflags.DoReRunEgamma() == True):
		print "## egamma WILL be rerun                                                              ##"
	else:
		print "## egamma WILL NOT be rerun                                                          ##"
	print "#######################################################################################"
else :
	PhotonContainerMerged = PhotonContainer
	PhotonsDetailsContainerName = PhotonDetailContainer

######################################################################################################################

#from CaloClusterCorrection.CaloClusterCorrectionFlags import caloClusterCorrectionFlags
#caloClusterCorrectionFlags.CaloSwWhichCorrection = 'v5_1_calh'
#caloClusterCorrectionFlags.DefaultSource         = 'jo' 


# If user did not provide any GRL xml file, switching GRLSelection off
PAUflags.UseGRLSelection = PAUflags.GoodRunsListVec.statusOn

if PAUflags.UseGRLSelection() :
	PAULog.info("'UseGRLSelection' was selected: Checking if file(s) in GoodRunsListVec are present...")
	import os.path
	for XMLFile in PAUflags.GoodRunsListVec():
		if (os.path.isfile(XMLFile)):
			PAULog.info("'"+XMLFile+"' exists in "+os.path.abspath("."))
		else:
			PAULog.warning(XMLFile+" DOES NOT exists in "+os.path.abspath("."))
			PAULog.warning("##################################################")
			PAULog.warning("Disabling GRL selection!")
			PAULog.warning("##################################################")
			PAUflags.UseGRLSelection = False
			break
	if PAUflags.UseGRLSelection():
		from GoodRunsLists.GoodRunsListsConf import *
		ToolSvc += GoodRunsListSelectorTool()
		GoodRunsListSelectorTool.GoodRunsListVec = PAUflags.GoodRunsListVec()
		GoodRunsListSelectorTool.OutputLevel     = INFO

include( "RecExCond/AllDet_detDescr.py" )
#ServiceMgr.GeoModelSvc.OutputLevel = INFO

#-------------------------------------------------------------------------
# Message Service
#-------------------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = WARNING


#-------------------------------------------------------------------------
# Algorithms
#-------------------------------------------------------------------------

#-------------------------------
#instructions to compute DeltaT Calo and DeltaT MBTS : needed to be
#after the MC/data flag, after the file definition and after geometry definition

include("TileConditions/TileConditions_jobOptions.py")
include ("LArCellRec/LArCollisionTime_jobOptions.py")
from PrimaryDPDMaker.PrimaryDPDMakerConf import CaloTimeFilterTool
ToolSvc += CaloTimeFilterTool() 
CaloTimeFilterTool.timeDiffCut               = 5
CaloTimeFilterTool.MinCellsPerSide           = 2

from PrimaryDPDMaker.PrimaryDPDMakerConf import MBTSTimeFilterTool
ToolSvc += MBTSTimeFilterTool()      
MBTSTimeFilterTool.ChargeThreshold   = 60./222.
MBTSTimeFilterTool.MinHitsPerSide    = 2
MBTSTimeFilterTool.MaxTimeDifference = 10
#-------------------------------

from AthenaCommon.AlgSequence import AlgSequence,AthSequencer
mainJob = AlgSequence()
mainJob += AthSequencer("ModSequence1")
theJob = mainJob.ModSequence1

if PAUflags.RunEgammaFilter():
	from PrimaryDPDMaker.EGammaCountingFilter import EGammaCountingFilter
	theJob += EGammaCountingFilter("EMClusterCommStream_EGammaCountingFilter")
	theJob.EMClusterCommStream_EGammaCountingFilter.useElectronCollection  = True
	theJob.EMClusterCommStream_EGammaCountingFilter.electronCollectionType = "ElectronContainer"
	theJob.EMClusterCommStream_EGammaCountingFilter.electronCollectionName = ElectronContainer
	theJob.EMClusterCommStream_EGammaCountingFilter.usePhotonCollection  = True
	theJob.EMClusterCommStream_EGammaCountingFilter.photonCollectionType = "PhotonContainer"
	theJob.EMClusterCommStream_EGammaCountingFilter.photonCollectionName = PhotonContainer
	theJob.EMClusterCommStream_EGammaCountingFilter.useEMClusterCollection = False
	theJob.EMClusterCommStream_EGammaCountingFilter.OutputLevel = DEBUG

if PAUflags.UseGRLSelection() :
        from GoodRunsListsUser.GoodRunsListsUserConf import *
        from AthenaCommon.AlgSequence import AlgSequence,AthSequencer
        theJob += GRLTriggerSelectorAlg('GRLTriggerAlg1')
        print "#######################################################################################"
        print "Lumi block selection will be attempted, if XML file(s) are present..."
        print "#######################################################################################" 

if PAUflags.DoPhotonPostProcessingReRunPID():
	doPhotonRecoveryAlg = False
	doReRunEgamma = False
	include('PhotonAnalysisAlgs/PhotonPostProcessing_ReRunPID_jobOptions.py')

### WARNING: the block below probably won't work anymore!
if (usePhotonRecoveryTool==True):
	# Recovery algorithm
	doReRunEgamma=PAUflags.DoReRunEgamma()
	include('PhotonAnalysisAlgs/PhotonRecovery_jobOptions.py')
	PhotonConversionRecoveryTool.newSchemeRecovery = not(PAUflags.ProductionRelease.StoredValue < 15.2)
        #PhotonConversionRecoveryTool.OutputLevel = DEBUG

	theJob += paa
	
	if (not(PAUflags.DoReRunEgamma())):
		include('PhotonAnalysisAlgs/PhotonPostProcessing_jobOptions.py')
	else :
		PhotonContainer             = PhotonContainer+"Recovered"
		PhotonDetailContainerMerged = PhotonDetailContainer+"Reco"
		PhotonsDetailsContainerName = PhotonDetailContainer+"Recovered"
		PhotonVxCandContainerMerged = "ConversionCandidateRecovered"
		PhotonContainerMerged       = PhotonContainer
		PhotonClusterContainerMerged= PhotonClusterContainer+"Recovered"
### end WARNING

if PAUflags.UsePAUGetter():
	from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import jobproperties
	
	PAUflags.UsePAUconversionFlagTool      = usePAUconversionFlagTool
	PAUflags.PhotonContainer               = PhotonContainer
	PAUflags.PhotonDetailContainerMerged   = PhotonDetailContainerMerged
	PAUflags.PhotonsDetailsContainerName   = PhotonsDetailsContainerName
	PAUflags.PhotonContainerMerged         = PhotonContainerMerged
	PAUflags.PhotonVxCandContainerMerged   = PhotonVxCandContainerMerged
	PAUflags.PhotonClusterContainerMerged  = PhotonClusterContainerMerged
	PAUflags.AlgSequence                   = "ModSequence1"
	
	from PhotonAnalysisUtils.PhotonAnalysisUtilsGetter import PhotonAnalysisUtilsGetter
	pau = PhotonAnalysisUtilsGetter()
	
else:
	include("PhotonAnalysisUtils/CommonJobOptions.py")


# the following is to guarantee access to Jet collections with new names:
include("JetRec/ParticleJetCompatibility.py")

# the following is just for tests...
#from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUprobeStoreGate
#probeSG = PAUprobeStoreGate()
#probeSG.OutputLevel = INFO
#theJob += probeSG

##########################################
# setup TTree registration svc
# save ROOT histograms and Tuple

#rtf=popen2.Popen3("find ../.. -name PT2dist.root",0)
#rtf.tochild.close()
#output = rtf.fromchild
#for rf in output:
#   rootf=rf[:-1]
#   break

if ( PAUflags.DumpNtuple() or PAUflags.DumpHistograms()) :
	
	from GaudiSvc.GaudiSvcConf import THistSvc
	ServiceMgr += THistSvc()
	ServiceMgr.THistSvc.Output = ["AANT DATAFILE='"+PAUflags.FileName()+"' OPT='RECREATE'"]

	if ( PAUflags.DumpCollectionTree() ) :
	
		CBNTAthenaAware = True
		include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
		include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")
	
		from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
	
		CollectionTreeStream = AANTupleStream( 'CollectionTreeStream' )
		topSequence += CollectionTreeStream
		topSequence.CollectionTreeStream.StreamName = "AANT" 
		topSequence.CollectionTreeStream.TreeName = "CollectionTree"
		topSequence.CollectionTreeStream.ExtraRefNames = [ "StreamESD","Stream1" ]
                #dont use, filename taken from THistSvc: topSequence.CollectionTreeStream.OutputName = 'AnalysisSkeleton.aan.root'
		topSequence.CollectionTreeStream.WriteInputDataHeader = True
		topSequence.CollectionTreeStream.OutputLevel = INFO


###################### For interactive analysis
#include ("PyAnalysisCore/InitPyAnalysisCore.py")



###########################################   TRIGGER   ###################################################
if PAUflags.UseTrigger():

    ## set up trigger decision tool
    #from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    #tdt = Trig__TrigDecisionTool("TrigDecisionTool")
    #ToolSvc += tdt

    # To read files with trigger config stored as in-file meta-data,
    # i.e. 13.0.40 and above: ds
    # To read AOD produced with 13.0.30 you need to change ds to aod: 
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    TriggerFlags.configurationSourceList = ['ds']
    
    # set up trigger config service
    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    cfg =  TriggerConfigGetter()


#-------------------------------------------------------------------------
# Finish up
#-------------------------------------------------------------------------

doAudit = False
if doAudit:
    theApp.AuditAlgorithms=True
    theApp.Dlls += [ "GaudiAud" ]
    
    from AthenaCommon.ConfigurableDb import getConfigurable
    theAuditorSvc += getConfigurable("ChronoAuditor")()
    
    AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
    AthenaPoolCnvSvc.UseDetailChronoStat = TRUE

if PAUflags.PrintJobOptions():
	print ToolSvc
	print mainJob


from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
import popen2
import sys

from CaloClusterCorrection.CaloClusterCorrectionFlags import jobproperties
jobproperties.CaloClusterCorrectionFlags.CaloSwWhichCorrection = 'v5_1_calh'
jobproperties.CaloClusterCorrectionFlags.DefaultSource = 'jo' 

#-------------------------------------------------------------------------
# Options to turn on/off certain algorithms
#-------------------------------------------------------------------------

UseTrigger = False

recoveryAlreadyDone = False

usePAUconversionFlagTool =False
usePhotonRecoveryTool = True

if ( recoveryAlreadyDone==True ) :
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

runWithoutRecovery = not ( usePAUconversionFlagTool or usePhotonRecoveryTool )

#-------------------------------------------------------------------------

if ( (usePAUconversionFlagTool==False and usePhotonRecoveryTool==False and runWithoutRecovery==False) or (usePAUconversionFlagTool==True and usePhotonRecoveryTool==True ) or (runWithoutRecovery==True and usePhotonRecoveryTool==True) or (runWithoutRecovery==True and usePAUconversionFlagTool==True)):
	print "\n##################################################################################################\n##   You have to choose to use either PAUconversionFlagTool OR PhotonRecoveryTool OR No Recovery   ##\n##################################################################################################\n"
	sys.exit(1)
	
if (usePhotonRecoveryTool==True):
	LoopOverElectronContainer = False
	PhotonsDetailsContainerName = "AllPhotonsDetails"
	if not 'doPhotonRecoveryAlg'  in dir():
		doPhotonRecoveryAlg    = True
	if not 'doReRunEgamma'        in dir():
		doReRunEgamma          = False 

if (usePAUconversionFlagTool==True):
	LoopOverElectronContainer = True
	PhotonContainerMerged = "PhotonAODCollection"
	PhotonsDetailsContainerName = "egDetailAOD"

if (runWithoutRecovery==True):
	LoopOverElectronContainer = False
	if (recoveryAlreadyDone==False):
		PhotonContainerMerged = "PhotonAODCollection"
		PhotonsDetailsContainerName = "egDetailAOD"
	else:
		PhotonContainerMerged = "AllPhotons"
		PhotonsDetailsContainerName = "AllPhotonsDetails"
		
if not 'aantfile' in dir():
	aantfile  = "aant.root"

#-------------------------------------------------------------------------
# Input Dataset
#-------------------------------------------------------------------------
#theApp.EvtMax=-1
#theApp.EvtMax=250
#ServiceMgr.EventSelector.SkipEvents=25

ServiceMgr.EventSelector.InputCollections = [ "AOD.pool.root" ]

doAOD = False
doESD = False
doDPD = False
if ServiceMgr.EventSelector.InputCollections[0].rfind('DPD') != -1:
	doDPD = True
elif ServiceMgr.EventSelector.InputCollections[0].rfind('ESD') != -1:
	doESD = True
else:
	doAOD = True
	

#-------------------------------------------------------------------------
# Geometry and Detector Description
#-------------------------------------------------------------------------
#DetDescrVersion = 'ATLAS-CSC-02-01-00'  # for release 14
#DetDescrVersion = 'ATLAS-CSC-01-02-00'  # for release 14 H->gg
#DetDescrVersion = 'ATLAS-GEOM-01-00-00'  # for release 14 20GeV single photon
DetDescrVersion = 'ATLAS-GEO-02-01-00'  # for release 14 Et7-80 single photon sample
#DetDescrVersion="ATLAS-CSC-05-00-00"

StoreGateSvc.Dump = True  #true will dump data store contents

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

from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()



if (usePhotonRecoveryTool==True):
	# Recovery algorithm
	include('PhotonAnalysisAlgs/PhotonRecovery_jobOptions.py')
	#PhotonConversionRecoveryTool.OutputLevel = DEBUG

	## Ugly hack.  For some reason, there are other algorithms ending up in the job that I don't think we want,
	## and it's unclear to me both how they got there and how to get turn them off.  This seems to work well though.
	for i in theJob:
		theJob.remove(i)
	    
	theJob += paa
	
	include('PhotonAnalysisAlgs/PhotonPostProcessing_jobOptions.py')


CBNTAthenaAware = True
include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")

include("CommonJobOptions-15.2.0.py")


##########################################
# setup TTree registration svc
# save ROOT histograms and Tuple

#rtf=popen2.Popen3("find ../.. -name PT2dist.root",0)
#rtf.tochild.close()
#output = rtf.fromchild
#for rf in output:
#   rootf=rf[:-1]
#   break

#from GaudiSvc.GaudiSvcConf import THistSvc
#ServiceMgr += THistSvc()
#ServiceMgr.THistSvc.Input += ["RefFile DATAFILE='"+rootf+"' OPT='OLD'"]
#ServiceMgr.THistSvc.Output = ["AANT DATAFILE='AnalysisSkeleton.aan.root' OPT='RECREATE'"]

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Input += ["RefFile DATAFILE='PT2dist.root' OPT='OLD'"]
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='AnalysisSkeleton.aan.root' OPT='RECREATE'"]




# uncomment the following if you need HggUserData in separate file
# then update UDStream options accordingly!
#ServiceMgr.THistSvc.Output += ["Hgg DATAFILE='HggUD.aan.root' OPT='RECREATE'"]

from AnalysisTools.AthAnalysisToolsConf import AANTupleStream

CollectionTreeStream = AANTupleStream( 'CollectionTreeStream' )
topSequence += CollectionTreeStream
topSequence.CollectionTreeStream.ExtraRefNames = [ "StreamESD","Stream1" ]
topSequence.CollectionTreeStream.OutputName = 'AnalysisSkeleton.aan.root'
topSequence.CollectionTreeStream.WriteInputDataHeader = True
topSequence.CollectionTreeStream.OutputLevel = INFO

PAUStream = AANTupleStream( 'PAUStream' )
topSequence += PAUStream
topSequence.PAUStream.StreamName = "AANT" # must be coherent with : PAUanalysis.PAUStreamName
topSequence.PAUStream.TreeName   = "PAUReco"  # must be coherent with : PAUanalysis.PAUTreeName
topSequence.PAUStream.OutputName = 'AnalysisSkeleton.aan.root'
topSequence.PAUStream.WriteInputDataHeader = True
topSequence.PAUStream.OutputLevel = INFO

UDStream = AANTupleStream( 'UDStream' )
topSequence += UDStream
topSequence.UDStream.StreamName = "AANT" # must be coherent with : PAUanalysis.PAUStreamName
topSequence.UDStream.TreeName   = "HggUserData"  # must be coherent with : PAUanalysis.PAUTreeName
topSequence.UDStream.OutputName = 'AnalysisSkeleton.aan.root'
topSequence.UDStream.WriteInputDataHeader = True
topSequence.UDStream.OutputLevel = INFO

###################### For interactive analysis
#include ("PyAnalysisCore/InitPyAnalysisCore.py")



###########################################   TRIGGER   ###################################################
if (UseTrigger==True):

    ## set up trigger decision tool
    from TrigDecision.TrigDecisionConfig import TrigDecisionTool
    tdt = TrigDecisionTool()
    ToolSvc += tdt

    # might be needed for TriggerConfigGetter...
    from RecExConfig.RecFlags  import rec
    rec.readRDO=False
    rec.readAOD=True
    rec.doWriteAOD=False
    rec.doWriteESD=False

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
    
print ToolSvc
print theJob

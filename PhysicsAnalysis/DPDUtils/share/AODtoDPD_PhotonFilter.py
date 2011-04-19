# This is the job options that does AOD based skimming and thinning
###############################################################
#
# Job options file
#
# Authors: Davide Costanzo, Stathes Paganis, Sven Menke
# Anastopoulos Christos 
#==============================================================
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ToolSvc

svcMgr.EventSelector.InputCollections =[ "AOD.pool.root" ]

DetDescrVersion = 'ATLAS-GEO-02-01-00'

#Needed in some cases 
##### Det Description ##############################
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-02-01-00')
include("RecExCond/AllDet_detDescr.py")
############################     
DetDescrVersion = 'ATLAS-GEO-02-01-00'

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = -1

#
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3

from RecExConfig.RecFlags import rec
rec.readRDO=False
rec.readAOD=True
rec.doWriteESD=False
rec.doWriteAOD=False
rec.doWriteTAG=False

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList=['ds']

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter() 

###############################
# Load thinning service
###############################

from AthenaServices.Configurables import ThinningSvc
svcMgr += ThinningSvc()
# the output stream(s) to apply thinning on
svcMgr.ThinningSvc.Streams += ['StreamDPD']

include( "PyAnalysisCore/InitPyAnalysisCore.py")
include( "DPDUtils/ThinningSvcWrapper_jobOptions.py" )

#==============================================================
#
# End of job options file
#
#==============================================================

###############################################################
#--------------------------------------------------------------
#---   Secondary Write portion  ----- Don't change it !!!
#--------------------------------------------------------------
###############################################################

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import AthenaPoolOutputStreamTool
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
StreamDPD = MSMgr.NewStream("StreamDPD","SkimmedThin.AOD.pool.root")

from DPDUtils.DPDUtilsConf import  DPDPhotonFilter
filAlg=DPDPhotonFilter("DPDPhotonFilter")

filAlg.UseTriggerSelection=False
filAlg.TriggerChain=("EF_g20")
#filAlg.TriggerChain=("EF_2g20,EF_g20")
filAlg.PhotEtCut=20000
filAlg.PhotEtaCut=2.5
#filAlg.TriggerSignatures = [ "g20_loose" , "g20i" ,"2g20" ,"g25" ,"g105" , "g10" ]
#filAlg.TrigDecisionTool = "TrigDecisionTool"
topSequence+=filAlg
print topSequence

StreamDPD.AcceptAlgs(["DPDPhotonFilter"])

#Write everything in the AOD
StreamDPD.Stream.TakeItemsFromInput=True
StreamDPD.Stream_FH.TakeItemsFromInput=True



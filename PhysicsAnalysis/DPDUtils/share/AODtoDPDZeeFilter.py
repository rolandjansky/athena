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

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

###############################
# Load thinning service
###############################

from AthenaServices.Configurables import ThinningSvc
svcMgr += ThinningSvc()
# the output stream(s) to apply thinning on
svcMgr.ThinningSvc.Streams += ['StreamDPD']

#Needed in some cases 
##### Det Description ##############################
#from AthenaCommon.GlobalFlags import globalflags
#globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-02-01-00')
#include("RecExCond/AllDet_detDescr.py")
############################     

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
svcMgr.EventSelector.InputCollections =[ "AOD.pool.root" ]
#svcMgr.EventSelector.InputCollections =inFileName

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

from DPDUtils.DPDUtilsConf import  ZeeFilter
filAlg=ZeeFilter("ZeeFilter")
topSequence+=filAlg
print topSequence

StreamDPD.AcceptAlgs(["ZeeFilter"])

#Metadata
StreamDPD.AddMetaDataItem("IOVMetaDataContainer#*")
include('LumiBlockComps/CreateLumiBlocks_jobOptions.py') 
StreamDPD.AddMetaDataItem("LumiBlockCollection#*")

#Event Level
StreamDPD.AddItem('EventInfo#*')
StreamDPD.AddItem ('TrackRecordCollection#*')
#Electrons
StreamDPD.AddItem ('ElectronContainer#ElectronAODCollection')
#egamma
StreamDPD.AddItem ('egDetailContainer#egDetailAOD')
StreamDPD.AddItem('CaloClusterContainer#egClusterCollection')
#Missing Et 
StreamDPD.AddItem ('MissingET#MET_RefFinal')
StreamDPD.AddItem ('MissingET#MET_LocHadObj')
#Muons
StreamDPD.AddItem ('Analysis::MuonContainer#StacoMuonCollection')
#Jet
StreamDPD.AddItem ('JetKeyDescriptor#JetKeyMap')
StreamDPD.AddItem ('JetCollection#Kt4H1TopoJets')
StreamDPD.AddItem ('CaloClusterContainer#CaloCalTopoCluster')
#Tracks
StreamDPD.AddItem ('Rec::TrackParticleContainer#TrackParticleCandidate')
StreamDPD.AddItem ('Rec::TrackParticleContainer#StacoTrackParticles')

#Truth
StreamDPD.AddItem ('TruthParticleContainer#SpclMC')
StreamDPD.AddItem ('McEventCollection#GEN_AOD')

StreamDPD.ForceRead=TRUE;  #force read of output data objs


#Write everything in the AOD
#StreamDPD.Stream.TakeItemsFromInput=True
#StreamDPD.Stream_FH.TakeItemsFromInput=True






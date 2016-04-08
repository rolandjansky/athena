###############################################################
#
# Main Job options file
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

########Detector Description########################################
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-02-01-00')
include("RecExCond/AllDet_detDescr.py")
#####################################################################

# Configure IP TOOL
from AthenaCommon.AppMgr import ToolSvc

# ATLAS extrapolator
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
theAtlasExtrapolator=AtlasExtrapolator()
ToolSvc+=theAtlasExtrapolator

from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__KalmanVertexUpdator
myVertexUpdator = Trk__KalmanVertexUpdator()
ToolSvc += myVertexUpdator

from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
myLinearizedTrackFactory = Trk__FullLinearizedTrackFactory(name="myLinearizedTrackFactory",Extrapolator =theAtlasExtrapolator)
ToolSvc += myLinearizedTrackFactory			    


from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
TrackToVertexIPEstimator = Trk__TrackToVertexIPEstimator(name="TrackToVertexIPEstimator",
                                                         Extrapolator = theAtlasExtrapolator,
                                                         LinearizedTrackFactory=myLinearizedTrackFactory)

ToolSvc += TrackToVertexIPEstimator
######################################################################

#Number of Events
theApp.EvtMax = 100
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 5
svcMgr.EventSelector.InputCollections =[ "AOD.pool.root" ]

from HSG2DPDUtils.HSG2DPDUtilsConf import H4lAddUserData
topSequence += H4lAddUserData( "H4lAddUserData" )

include( "PyAnalysisCore/InitPyAnalysisCore.py")

#--------------------------------------------------------------
#---   Secondary Write portion  ----- Don't change it !!!
#--------------------------------------------------------------

#### Multiple Stream Manager #########################
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import AthenaPoolOutputStreamTool
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
StreamDPD = MSMgr.NewStream("StreamDPD","H4lAddUserData.pool.root")

#################Add Lumi##############################
include('LumiBlockComps/CreateLumiBlocks_jobOptions.py') 
StreamDPD.AddMetaDataItem("LumiBlockCollection#*")


###### Keep all Collections #########################
StreamDPDForceRead=True
StreamDPD.Stream.TakeItemsFromInput=True
####################################################

#---------------------------------------------------------------
# UserDataSvc
#---------------------------------------------------------------
from AthenaServices.TheUserDataSvc import TheUserDataSvc
svcMgr += TheUserDataSvc("UserDataSvc")
svcMgr.UserDataSvc.OutputStream=StreamDPD.Stream


#==============================================================
#
# End of job options file
#
###############################################################

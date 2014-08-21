# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: Sebastian Boeser
# UCL, July 21, 2008

######
# Reading in AOD
######
# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import theApp

# Create one ONCRPC Server
from JiveXML.JiveXMLConf import JiveXML__ONCRPCServerSvc
ServiceMgr += JiveXML__ONCRPCServerSvc(OutputLevel=VERBOSE)
theApp.CreateSvc += [ "JiveXML::ONCRPCServerSvc" ]

# And another that streams to the "external" first server
from JiveXML.JiveXMLConf import JiveXML__ExternalONCRPCServerSvc
ExternalSvc = JiveXML__ExternalONCRPCServerSvc(OutputLevel=VERBOSE)
ServiceMgr += ExternalSvc
theApp.CreateSvc += [ "JiveXML::ExternalONCRPCServerSvc" ]

# and the POOL converters
import AthenaPoolCnvSvc.ReadAthenaPool
#include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py" )
include ( "RecAthenaPool/RecAthenaPool_joboptions.py")

# and add the AOD input file
for i in range(0,50):
  ServiceMgr.EventSelector.InputCollections += [
   "AOD.pool.root"
  ]

# Number of Events to process
if not 'EvtMax' in dir():
  EvtMax = -1

theApp.EvtMax = EvtMax

########
# The basic JiveXML and EventData2XML options
########

###import and create top sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#Add the JiveXML algorithm
from JiveXML.JiveXMLConf import JiveXML__AlgoJiveXML
topSequence += JiveXML__AlgoJiveXML(name="AlgoJiveXML")

#Set the EventData2XML properties
import os
topSequence.AlgoJiveXML.AtlasRelease = os.environ["AtlasVersion"]
topSequence.AlgoJiveXML.OnlineMode = False
topSequence.AlgoJiveXML.WriteToFile = False
topSequence.AlgoJiveXML.OutputLevel = INFO

from GaudiKernel.GaudiHandles import PrivateToolHandleArray                                      
from GaudiKernel.GaudiHandles import PublicToolHandleArray                                      
from GaudiKernel.GaudiHandles import PublicToolHandle

#Also stream to our new server and the "external" server
from JiveXML.JiveXMLConf import JiveXML__StreamToServerTool
topSequence.AlgoJiveXML.StreamTools += PrivateToolHandleArray( [
  "JiveXML::StreamToServerTool/StreamToServerTool"
])
secondStream = JiveXML__StreamToServerTool("StreamToExternalServerTool",OutputLevel=VERBOSE,StreamName = "copy")
#Have the second stream tool stream to the external server
from GaudiKernel.GaudiHandles import ServiceHandle
#secondStream.ServerService = ServiceHandle("JiveXML::ExternalONCRPCServerSvc/ExternalONCRPCServerSvc")
secondStream.ServerService = ServiceHandle(ExternalSvc)
#Then add it to the list of stream tools
topSequence.AlgoJiveXML.StreamTools += [secondStream]

###Add retrievers using data types
#topSequence.AlgoJiveXML.DataTypes += ['JiveXML::AODCaloClusterRetriever/AODCaloClusterRetriever']
#                                                                                                   
##Add retrievers using ToolHandleArray with Configurables                                         
#from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__AODCaloClusterRetriever                 
#from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__TrackParticleRetriever                  
#from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__PhotonRetriever                  
#from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__ElectronRetriever                  
#from AnalysisJiveXML.AnalysisJiveXMLConf import JiveXML__MuonRetriever                  
#
#ToolSvc += JiveXML__TrackParticleRetriever("TrackParticleRetriever")
#ToolSvc += JiveXML__PhotonRetriever("PhotonRetriever", OutputLevel=INFO),
#ToolSvc += JiveXML__ElectronRetriever("ElectronRetriever", OutputLevel=INFO),
#
#topSequence.AlgoJiveXML.DataRetrievers += [
#   ToolSvc.TrackParticleRetriever,
#   ToolSvc.ElectronRetriever,
#]
#
##Add using ToolHandles with names                                                 
#topSequence.AlgoJiveXML.DataRetrievers += [ PublicToolHandle("JiveXML::PhotonRetriever/PhotonRetriever") ]
#ToolSvc.PhotonRetriever.OutputLevel=INFO
#
##Add retrievers using ToolHandleArray with names                                                 
#topSequence.AlgoJiveXML.DataRetrievers += PublicToolHandleArray( [                                 
#  "JiveXML::MuonRetriever/MuonRetriever"
#] )
#topSequence.AlgoJiveXML.DataRetrievers["MuonRetriever"].OutputLevel=INFO
#                                                                                                 
#                                                                                                   
##Set options for Retrievers                                                                      
##topSequence.AlgoJiveXML.DataRetrievers['AODCaloClusterRetriever'].OutputLevel=INFO                                   
#ToolSvc += JiveXML__AODCaloClusterRetriever('AODCaloClusterRetriever')
#ToolSvc.AODCaloClusterRetriever.OutputLevel=INFO
##topSequence.AlgoJiveXML.DataRetrievers['MuonRetriever'].OutputLevel=INFO                                             
##topSequence.AlgoJiveXML.DataRetrievers['ElectronRetriever'].OutputLevel=INFO                                             
##topSequence.AlgoJiveXML.DataRetrievers['PhotonRetriever'].OutputLevel=INFO                                             
##topSequence.AlgoJiveXML.DataRetrievers['TauJetRetriever'].OutputLevel=INFO                                             
##topSequence.AlgoJiveXML.DataRetrievers['TrackParticleRetriever'].OutputLevel=INFO                                    
##topSequence.AlgoJiveXML.DataRetrievers['TrackParticleRetriever'].DoWriteHLT=True                         


######
# Message output control
######
# thresholds: VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL, ALWAYS
## the overall level
ServiceMgr.MessageSvc.OutputLevel = ERROR
## Don't want to see every storeGate access
ServiceMgr.StoreGateSvc.OutputLevel = ERROR
## This one will give us event numbers
ServiceMgr.EventSelector.OutputLevel= INFO

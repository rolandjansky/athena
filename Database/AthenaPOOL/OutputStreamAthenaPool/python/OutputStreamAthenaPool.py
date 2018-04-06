# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file OutputStreamAthenaPool.py
## @brief Helper methods to create output streams
## @author Peter van Gemmeren <gemmeren@bnl.gov>
## $Id: OutputStreamAthenaPool.py,v 1.10 2009-04-27 18:19:34 gemmeren Exp $
###############################################################

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaServices.AthenaServicesConf import AthenaOutputStream
from AthenaServices.AthenaServicesConf import AthenaOutputStreamTool

def createOutputStream( streamName, fileName = "", asAlg = False, noTag = True ):
   # define athena output stream
   writingTool = AthenaOutputStreamTool( streamName + "Tool" )
   outputStream = AthenaOutputStream(
      streamName,
      WritingTool = writingTool,
      ItemList    = [ "EventInfo#*" ]
      )
   #outputStream.ItemList += [ "xAOD::EventInfo#*" ]
   outputStream.MetadataStore = svcMgr.MetaDataStore
   outputStream.MetadataItemList = [ "EventStreamInfo#" + streamName, "IOVMetaDataContainer#*" ]

   from AthenaCommon.AlgSequence import AlgSequence
   topSequence = AlgSequence()
   
   doTag = not noTag
   if doTag:
      outputStream.ItemList += [ "AthenaAttributeList#SimpleTag" ]

      if ('EventInfoTagBuilder/EventInfoTagBuilder' not in topSequence.getProperties()['Members']):
         key = "SimpleTag"
         # Tell tool to pick it up
         outputStream.WritingTool.AttributeListKey=key
         # build eventinfo attribute list
         from OutputStreamAthenaPoolConf import EventInfoAttListTool
         svcMgr.ToolSvc += EventInfoAttListTool()
         from OutputStreamAthenaPoolConf import EventInfoTagBuilder
         EventInfoTagBuilder   = EventInfoTagBuilder(AttributeList=key)
         topSequence += EventInfoTagBuilder

   # decide where to put outputstream in sequencing
   if asAlg:
      topSequence += outputStream
   else:
      theApp.OutStreamType = "AthenaOutputStream"
      theApp.addOutputStream( outputStream )

   if fileName != "":
      outputStream.OutputFile = fileName
      from OutputStreamAthenaPoolConf import MakeEventStreamInfo 
      streamInfoTool = MakeEventStreamInfo( streamName + "_MakeEventStreamInfo" )
      streamInfoTool.Key = streamName
      outputStream.HelperTools = [ streamInfoTool ]

   return outputStream

def createOutputConditionStream( streamName, fileName = "" ):
   from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
   conditionStream = OutputConditionsAlg(
      streamName,
      outputFile = fileName,
      WriteIOV = False
      )
   return conditionStream

## backward compat
AthenaPoolOutputStream          = createOutputStream
AthenaPoolOutputConditionStream = createOutputConditionStream

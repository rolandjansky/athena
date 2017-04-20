# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file OutputStreamAthenaPool.py
## @brief Helper methods to create output streams
## @author Peter van Gemmeren <gemmeren@bnl.gov>
## $Id: OutputStreamAthenaPool.py,v 1.10 2009-04-27 18:19:34 gemmeren Exp $
###############################################################

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaServices.AthenaServicesConf import AthenaOutputStream
from OutputStreamAthenaPoolConf import AthenaPoolOutputStreamTool

def createOutputStream( streamName, fileName = "", asAlg = False, noTag = False ):
   # define athena output stream
   writingTool = AthenaPoolOutputStreamTool( streamName + "Tool" )
   writingTool.DataHeaderSatellites = [ "basic/:EventInfo#*" ]
   outputStream = AthenaOutputStream(
      streamName,
      WritingTool = writingTool,
      ItemList    = [ "EventInfo#*, xAOD::EventInfo#*, AthenaAttributeList#SimpleTag" ]
      )
   outputStream.MetadataStore = svcMgr.MetaDataStore
   outputStream.MetadataItemList = [ "EventStreamInfo#" + streamName, "IOVMetaDataContainer#*" ]

   from AthenaCommon.AlgSequence import AlgSequence
   topSequence = AlgSequence()

   if not noTag:
      key = "SimpleTag"
      # Tell tool to pick it up
      outputStream.WritingTool.AttributeListKey=key
      # build eventinfo attribute list
      from OutputStreamAthenaPoolConf import EventInfoAttListTool
      svcMgr.ToolSvc += EventInfoAttListTool()

      from OutputStreamAthenaPoolConf import EventInfoTagBuilder
      EventInfoTagBuilder   = EventInfoTagBuilder(AttributeList=key)
      if ('EventInfoTagBuilder/EventInfoTagBuilder' not in topSequence.getProperties()['Members']):
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

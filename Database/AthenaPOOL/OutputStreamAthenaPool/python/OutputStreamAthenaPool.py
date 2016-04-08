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

def createOutputStream( streamName, fileName = "", asAlg = False ):
   writingTool = AthenaPoolOutputStreamTool( streamName + "Tool" )
   writingTool.DataHeaderSatellites = [ "basic/:EventInfo#*" ]
   outputStream = AthenaOutputStream(
      streamName,
      WritingTool = writingTool,
      ItemList    = [ "EventInfo#*" ]
      )
   outputStream.MetadataStore = svcMgr.MetaDataStore
   outputStream.MetadataItemList = [ "EventStreamInfo#" + streamName, "IOVMetaDataContainer#*" ]
   if asAlg:
      from AthenaCommon.AlgSequence import AlgSequence
      topSequence = AlgSequence()
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

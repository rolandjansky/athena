# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @file OutputStreamAthenaPool.py
## @brief Helper methods to create output streams
## @author Peter van Gemmeren <gemmeren@bnl.gov>
###############################################################

from __future__ import print_function

from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaServices.AthenaServicesConf import AthenaOutputStream
from AthenaServices.AthenaServicesConf import AthenaOutputStreamTool

_trigNavThinningSvcs = {}
def registerTrigNavThinningSvc (streamName, svc):
   _trigNavThinningSvcs[streamName] = svc
   return

def createOutputStream( streamName, fileName = "", asAlg = False, noTag = False,
                        eventInfoKey = "EventInfo", decisionFilter="",
                        trigNavThinningSvc = None ):
   if trigNavThinningSvc is None:
      trigNavThinningSvc = _trigNavThinningSvcs.get (streamName, None)


   # define athena output stream
   writingTool = AthenaOutputStreamTool( streamName + "Tool" )
   outputStream = AthenaOutputStream(
      streamName,
      WritingTool = writingTool,
      ItemList    = [ "EventInfo#*" ]
      )
   outputStream.ExtraOutputs += [("DataHeader", "StoreGateSvc+" + streamName)]
   #outputStream.ItemList += [ "xAOD::EventInfo#*" ]
   outputStream.MetadataStore = svcMgr.MetaDataStore
   outputStream.MetadataItemList = [
        "EventStreamInfo#" + streamName,
        "IOVMetaDataContainer#*",
   ]

   ## get a handle on the default top-level algorithm sequence
   from AthenaCommon.AlgSequence import AlgSequence
   topSequence = AlgSequence()
   from AthenaCommon.AlgSequence import AthSequencer
   outSequence = AthSequencer("AthOutSeq")

   doTag = not noTag
   if doTag:
      if ('EventInfoTagBuilder/EventInfoTagBuilder' not in topSequence.getProperties()['Members']):
         key = "SimpleTag"
         # Tell tool to pick it up
         outputStream.WritingTool.AttributeListKey=key
         # build eventinfo attribute list
         from .OutputStreamAthenaPoolConf import EventInfoAttListTool, EventInfoTagBuilder
         EventInfoTagBuilder = EventInfoTagBuilder(AttributeList=key, EventInfoKey=eventInfoKey, FilterString=decisionFilter,
                                                   Tool=EventInfoAttListTool())
         from AthenaCommon.GlobalFlags  import globalflags
         if globalflags.InputFormat() == 'bytestream':
            #No event-tag input in bytestream
            EventInfoTagBuilder.PropagateInput=False
         topSequence += EventInfoTagBuilder

   # decide where to put outputstream in sequencing
   if asAlg:
      outSequence += outputStream
   else:
      outSequence += outputStream

   if fileName != "":
      outputStream.OutputFile = fileName
      from .OutputStreamAthenaPoolConf import MakeEventStreamInfo 
      streamInfoTool = MakeEventStreamInfo( streamName + "_MakeEventStreamInfo" )
      streamInfoTool.Key = streamName
      streamInfoTool.EventInfoKey = eventInfoKey
      # for xAOD access, add EventFormat to all POOL output streams
      # Key to use for event format on this stream
      event_format_key = 'EventFormat{}'.format(streamName)
      event_format_tool = CfgMgr.xAODMaker__EventFormatStreamHelperTool(
         "{}_MakeEventFormat".format(streamName),
         Key=event_format_key,
      )
      outputStream.MetadataItemList += ["xAOD::EventFormat#{}".format(event_format_key)]
      outputStream.HelperTools = [ streamInfoTool, event_format_tool]


   # Support for MT thinning.
   from AthenaServices.AthenaServicesConf import Athena__ThinningCacheTool
   tct = Athena__ThinningCacheTool ('ThinningCacheTool_' + streamName,
                                    StreamName = streamName)
   if trigNavThinningSvc is not None:
      tct.TrigNavigationThinningSvc = trigNavThinningSvc
   outputStream.HelperTools += [tct]


   # Set the list of transient items based on what we know is in the transient
   # store.  The output algorithm will then declare input dependencies
   # for objects which are both listed here and in the ItemList.
   # (We do it like this because ItemList is typically configured to include
   # everything which might possibly be output.  If this gets cleaned up,
   # then we can remove this.)
   # Some builds don't include RecExConfig, so don't crash in that case.
   # FIXME: Rather than using ObjKeyStore, we could scan all algorithms
   # and look for write handles.
   try:
      tlist = []
      from RecExConfig.ObjKeyStore import objKeyStore
      for typ, klist in objKeyStore['transient'].getProperties().items():
         for k in klist:
            tlist.append (typ + '#' + k)
      outputStream.TransientItems += tlist
   except ImportError:
      pass

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


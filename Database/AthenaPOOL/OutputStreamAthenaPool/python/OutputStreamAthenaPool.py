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

   ## get a handle on the default top-level algorithm sequence
   from AthenaCommon.AlgSequence import AlgSequence
   topSequence = AlgSequence()
   from AthenaCommon.AlgSequence import AthSequencer
   outSequence = AthSequencer("AthOutSeq")

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
      outSequence += outputStream
   else:
      outSequence += outputStream

   if fileName != "":
      outputStream.OutputFile = fileName
      from OutputStreamAthenaPoolConf import MakeEventStreamInfo 
      streamInfoTool = MakeEventStreamInfo( streamName + "_MakeEventStreamInfo" )
      streamInfoTool.Key = streamName
      outputStream.HelperTools = [ streamInfoTool ]

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

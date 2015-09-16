# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file OutputStreamAthenaPool.py
## @brief Helper methods to create output streams
## @author Peter van Gemmeren <gemmeren@bnl.gov>
## $Id: OutputStreamAthenaPool.py,v 1.10 2009-04-27 18:19:34 gemmeren Exp $
###############################################################

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from OutputStreamAthenaPoolConf import AthenaPoolOutputStreamTool

def createOutputStream( streamName, fileName = "", asAlg = False ):
   writingTool = AthenaPoolOutputStreamTool( streamName + "Tool" )
   writingTool.DataHeaderSatellites = [ "basic/:EventInfo#*" ]
   outputStream = AthenaPoolOutputStreamProtect(
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
      from AthenaServices.AthenaServicesConf import AthenaOutputStream
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

from AthenaCommon import CfgMgr
class AthenaPoolOutputStreamProtect(CfgMgr.AthenaOutputStream):
   def __init__(self, name='Stream1', **kw):
      kw['name'] = kw.get('name', name)
      super(AthenaPoolOutputStreamProtect, self).__init__(**kw)
      return
   def _set_output_file(self, fname):
      self._properties['OutputFile'].__set__(self, fname)
      from AthenaServices.AthenaServicesConf import AthenaOutputStream
      AthenaOutputStream("%s_FH" % (self._name,)).OutputFile = fname
      return

   def _get_output_file(self):
      return self._properties['OutputFile'].__get__(self)

   OutputFile = property(_get_output_file, _set_output_file, "fwd doc...")
   pass

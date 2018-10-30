# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator, ConfigurationError
from AthenaCommon.Logging import logging

def OutputStreamCfg(configFlags, streamName, ItemList=[] ):
   from OutputStreamAthenaPoolConf import MakeEventStreamInfo
   from AthenaServices.AthenaServicesConf import AthenaOutputStream
   from AthenaServices.AthenaServicesConf import AthenaOutputStreamTool

   flagName="Output.%sFileName" % streamName
   if configFlags.hasFlag(flagName):
      fileName=configFlags._get(flagName)
   else:
      fileName="my%s.pool.root" % streamName
      msg = logging.getLogger('OutputStreamCfg')
      msg.info("No file name predefined for stream %s. Using %s" % (streamName, fileName))      

   if fileName in configFlags.Input.Files:
      raise ConfigurationError("Same name for input and output file %s" % fileName)


   outputAlgName="OutputStream"+streamName

   result=ComponentAccumulator(sequenceName="AthOutSeq")
   # define athena output stream
   writingTool = AthenaOutputStreamTool( streamName + "Tool" )
   streamInfoTool = MakeEventStreamInfo( streamName + "_MakeEventStreamInfo" )
   streamInfoTool.Key = streamName
   outputStream = AthenaOutputStream(
      outputAlgName,
      WritingTool = writingTool,
      ItemList    = [ "xAOD::EventInfo#*" ]+ItemList, 
      OutputFile = fileName,
      HelperTools = [ streamInfoTool ],
      )
   #outputStream.MetadataStore = svcMgr.MetaDataStore
   #outputStream.MetadataItemList = [ "EventStreamInfo#" + streamName, "IOVMetaDataContainer#*" ]


   result.addEventAlgo(outputStream)
   return result


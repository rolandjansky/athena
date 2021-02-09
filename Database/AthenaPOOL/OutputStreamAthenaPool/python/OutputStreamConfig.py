# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator, ConfigurationError
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import logging

def OutputStreamCfg(configFlags, streamName, ItemList=[], MetadataItemList=[],
                    disableEventTag=False, trigNavThinningSvc=None):
   MakeEventStreamInfo=CompFactory.MakeEventStreamInfo
   AthenaOutputStream=CompFactory.AthenaOutputStream
   AthenaOutputStreamTool=CompFactory.AthenaOutputStreamTool
   StoreGateSvc=CompFactory.StoreGateSvc

   msg = logging.getLogger('OutputStreamCfg')
   flagName="Output.%sFileName" % streamName
   if configFlags.hasFlag(flagName):
      fileName=configFlags._get(flagName)
   else:
      fileName="my%s.pool.root" % streamName
      msg.info("No file name predefined for stream %s. Using %s", streamName, fileName)

   if fileName in configFlags.Input.Files:
      raise ConfigurationError("Same name for input and output file %s" % fileName)


   outputAlgName="OutputStream"+streamName

   result=ComponentAccumulator(sequence=CompFactory.AthSequencer('AthOutSeq',StopOverride=True))
   # define athena output stream
   writingTool = AthenaOutputStreamTool( "Stream" + streamName + "Tool" )
   streamInfoTool = MakeEventStreamInfo( "Stream" + streamName + "_MakeEventStreamInfo" )
   streamInfoTool.Key = "Stream" + streamName

   # Support for MT thinning.
   ThinningCacheTool = CompFactory.getComp ('Athena::ThinningCacheTool') # AthenaServices
   tct = ThinningCacheTool ('ThinningCacheTool_' + streamName,
                            StreamName = streamName)
   if trigNavThinningSvc is not None:
      tct.TrigNavigationThinningSvc = trigNavThinningSvc

   outputStream = AthenaOutputStream(
      outputAlgName,
      WritingTool = writingTool,
      ItemList    = [ "xAOD::EventInfo#EventInfo", "xAOD::EventAuxInfo#EventInfoAux."  ]+ItemList, 
      MetadataItemList = MetadataItemList,
      OutputFile = fileName,
      HelperTools = [ streamInfoTool, tct ],
      )
   outputStream.ExtraOutputs += [("DataHeader", "StoreGateSvc+" + streamName)]
   result.addService(StoreGateSvc("MetaDataStore"))
   outputStream.MetadataStore = result.getService("MetaDataStore")
   outputStream.MetadataItemList += [
        "EventStreamInfo#Stream" + streamName,
        "IOVMetaDataContainer#*",
   ]

   # Make EventFormat object
   event_format_key = 'EventFormat{}'.format(streamName)
   event_format_tool = CompFactory.xAODMaker.EventFormatStreamHelperTool(
      '{}_MakeEventFormat'.format(event_format_key),
      Key=event_format_key,
   )
   outputStream.HelperTools.append(event_format_tool)
   msg.info("Creating event format for this stream")

   # Simplifies naming 
   outputStream.MetadataItemList.append(
      "xAOD::EventFormat#{}".format(event_format_key)
   )

   # setup FileMetaData
   file_metadata_key = "FileMetaData"
   outputStream.HelperTools.append(
        CompFactory.xAODMaker.FileMetaDataCreatorTool(
            name='{}_FileMetaDataCreatorTool'.format(streamName),
            OutputKey=file_metadata_key,
            StreamName=streamName,
        )
   )
   outputStream.MetadataItemList += [
        "xAOD::FileMetaData#{}".format(file_metadata_key),
        "xAOD::FileMetaDataAuxInfo#{}Aux.".format(file_metadata_key),
   ]

   # Event Tag
   if not disableEventTag:
      key = "SimpleTag"
      outputStream.WritingTool.AttributeListKey=key
      # build eventinfo attribute list
      EventInfoAttListTool, EventInfoTagBuilder=CompFactory.getComps("EventInfoAttListTool","EventInfoTagBuilder",)
      tagBuilder = EventInfoTagBuilder(AttributeList=key,
                                       Tool=EventInfoAttListTool())
      result.addEventAlgo(tagBuilder)

   # For xAOD output
   if "xAOD" in streamName:
      outputStream.WritingTool.SubLevelBranchName = "<key>"

      AthenaPoolCnvSvc=CompFactory.AthenaPoolCnvSvc
      poolcnvsvc = AthenaPoolCnvSvc()
      result.addService(poolcnvsvc)
      poolcnvsvc.PoolAttributes += [ "DatabaseName = '" + fileName + "'; COMPRESSION_LEVEL = '5'" ]
      poolcnvsvc.PoolAttributes += [ "DatabaseName = '" + fileName + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '-10000000'" ]
      poolcnvsvc.PoolAttributes += [ "DatabaseName = '" + fileName + "'; ContainerName = 'TTree=CollectionTree'; CONTAINER_SPLITLEVEL = '1'" ]
      poolcnvsvc.PoolAttributes += [ "DatabaseName = '" + fileName + "'; ContainerName = 'TTree=Aux.'; CONTAINER_SPLITLEVEL = '1'"]

   result.addEventAlgo(outputStream)
   return result


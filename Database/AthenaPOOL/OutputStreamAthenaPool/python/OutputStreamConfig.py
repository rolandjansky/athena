# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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
      msg.info("No file name predefined for stream %s. Using %s", streamName, fileName)

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
      ItemList    = [ "xAOD::EventInfo#EventInfo", "xAOD::EventAuxInfo#EventInfoAux."  ]+ItemList, 
      OutputFile = fileName,
      HelperTools = [ streamInfoTool ],
      )
   #outputStream.MetadataStore = svcMgr.MetaDataStore
   #outputStream.MetadataItemList = [ "EventStreamInfo#" + streamName, "IOVMetaDataContainer#*" ]

   # For xAOD output
   if streamName=="xAOD":
      from xAODEventFormatCnv.xAODEventFormatCnvConf import xAODMaker__EventFormatSvc
      # Simplifies naming 
      result.addService(xAODMaker__EventFormatSvc())
      outputStream.MetadataItemList.append( "xAOD::EventFormat#EventFormat" )

      from xAODMetaDataCnv.xAODMetaDataCnvConf import xAODMaker__FileMetaDataMarkUpTool
      streamMarkUpTool = xAODMaker__FileMetaDataMarkUpTool( streamName + "_FileMetaDataMarkUpTool" )
      streamMarkUpTool.Key = streamName
      outputStream.HelperTools += [ streamMarkUpTool ]
      outputStream.WritingTool.SubLevelBranchName = "<key>"

      from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
      poolcnvsvc = AthenaPoolCnvSvc()
      result.addService(poolcnvsvc)
      poolcnvsvc.PoolAttributes += [ "DatabaseName = '" + fileName + "'; COMPRESSION_LEVEL = '5'" ]
      poolcnvsvc.PoolAttributes += [ "DatabaseName = '" + fileName + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '-10000000'" ]
      poolcnvsvc.PoolAttributes += [ "DatabaseName = '" + fileName + "'; ContainerName = 'TTree=CollectionTree'; CONTAINER_SPLITLEVEL = '1'" ]
      poolcnvsvc.PoolAttributes += [ "DatabaseName = '" + fileName + "'; ContainerName = 'TTree=Aux.'; CONTAINER_SPLITLEVEL = '1'"]

   result.addEventAlgo(outputStream)
   return result


# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator, ConfigurationError
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from AthenaCommon.Logging import logging


def OutputStreamCfg(configFlags, streamName, ItemList=[], MetadataItemList=[],
                    disableEventTag=False, trigNavThinningSvc=None, AcceptAlgs=[]):
   eventInfoKey = "EventInfo"
   if configFlags.Common.ProductionStep == ProductionStep.PileUpPresampling:
      eventInfoKey = configFlags.Overlay.BkgPrefix + "EventInfo"

   msg = logging.getLogger("OutputStreamCfg")
   outputStreamName = f"Stream{streamName}"
   flagName = f"Output.{streamName}FileName"
   if configFlags.hasFlag(flagName):
      fileName = configFlags._get(flagName)
   else:
      fileName = f"my{streamName}.pool.root"
      msg.info("No file name predefined for stream %s. Using %s", streamName, fileName)

   if fileName in configFlags.Input.Files:
      raise ConfigurationError("Same name for input and output file %s" % fileName)

   result = ComponentAccumulator(sequence = CompFactory.AthSequencer("AthOutSeq", StopOverride=True))
   # define athena output stream
   writingTool = CompFactory.AthenaOutputStreamTool(f"Stream{streamName}Tool",
                                                    DataHeaderKey=outputStreamName)

   # In DAOD production the EventInfo is prepared specially by the SlimmingHelper to ensure it is written in AuxDyn form
   # So for derivations the ItemList from the SlimmingHelper alone is used without the extra EventInfo items
   finalItemList = []
   if ("DAOD_" or "D2AOD_") in streamName:
      finalItemList = ItemList
   else:
      finalItemList = [f"xAOD::EventInfo#{eventInfoKey}", f"xAOD::EventAuxInfo#{eventInfoKey}Aux."] + ItemList 

   outputStream = CompFactory.AthenaOutputStream(
      f"OutputStream{streamName}",
      StreamName=outputStreamName,
      WritingTool=writingTool,
      ItemList = finalItemList,
      MetadataItemList = MetadataItemList,
      OutputFile=fileName,
   )
   outputStream.AcceptAlgs += AcceptAlgs
   outputStream.ExtraOutputs += [("DataHeader", f"StoreGateSvc+{outputStreamName}")]
   result.addService(CompFactory.StoreGateSvc("MetaDataStore"))
   outputStream.MetadataStore = result.getService("MetaDataStore")
   outputStream.MetadataItemList += [
      f"EventStreamInfo#{outputStreamName}",
      "IOVMetaDataContainer#*",
   ]

   streamInfoTool = CompFactory.MakeEventStreamInfo(f"Stream{streamName}_MakeEventStreamInfo")
   streamInfoTool.Key = outputStreamName
   streamInfoTool.DataHeaderKey = outputStreamName
   streamInfoTool.EventInfoKey = eventInfoKey
   outputStream.HelperTools.append(streamInfoTool)

   # Make EventFormat object
   from xAODEventFormatCnv.EventFormatConfig import EventFormatCfg
   result.merge(
      EventFormatCfg(
         flags=configFlags, stream=outputStream, streamName=outputStreamName
      )
   )

   # Setup FileMetaData
   from xAODMetaDataCnv.FileMetaDataConfig import FileMetaDataCfg
   result.merge(FileMetaDataCfg(configFlags,
                                stream=outputStream,
                                streamName=outputStreamName))

   # Setup additional MetaData

   # ======================================================
   # TO-DO:
   # ======================================================
   # For the time being we're adding common MetaData items
   # and configure the necessary tools/services en masse.
   # Ideally, we should introduce a self-sufficienct config
   # for each item and merge them here.
   # ======================================================
   if any([ x in streamName for x in ['AOD','ESD'] ]):
      # LumiBlockMetaDataTool seems to cause crashes in MP derivation jobs
      # As done in RecExCommon_topOptions.py use the algorithm in MP jobs
      # This needs to be checked and confirmed...
      mdToolNames = ['BookkeeperTool']
      if configFlags.Concurrency.NumProcs > 0:
          result.addEventAlgo(CompFactory.CreateLumiBlockCollectionFromFile())
      else:
          mdToolNames.append('LumiBlockMetaDataTool')
      outputStream.MetadataItemList += ['xAOD::TriggerMenuContainer#*'
                                       ,'xAOD::TriggerMenuAuxContainer#*'
                                       ,'xAOD::TriggerMenuJsonContainer#*'
                                       ,'xAOD::TriggerMenuJsonAuxContainer#*'
                                       ,'xAOD::LumiBlockRangeContainer#*'
                                       ,'xAOD::LumiBlockRangeAuxContainer#*'
                                       ,'xAOD::CutBookkeeperContainer#*'
                                       ,'xAOD::CutBookkeeperAuxContainer#*'
                                       ,'ByteStreamMetadataContainer#*'
                                       ,'xAOD::TruthMetaDataContainer#TruthMetaData'
                                       ,'xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux.']

      from AthenaServices.MetaDataSvcConfig import MetaDataSvcCfg
      result.merge(MetaDataSvcCfg(configFlags,
                                  tools = [CompFactory.xAODMaker.TriggerMenuMetaDataTool('TriggerMenuMetaDataTool')],
                                  toolNames = mdToolNames))

   # Support for MT thinning.
   thinningCacheTool = CompFactory.Athena.ThinningCacheTool(f"ThinningCacheTool_Stream{streamName}",
                                                            StreamName=outputStreamName)
   if trigNavThinningSvc is not None:
      thinningCacheTool.TrigNavigationThinningSvc = trigNavThinningSvc
   outputStream.HelperTools.append(thinningCacheTool)

   # Event Tag
   if not disableEventTag:
      key = "SimpleTag"
      outputStream.WritingTool.AttributeListKey=key
      # build eventinfo attribute list
      tagBuilder = CompFactory.EventInfoTagBuilder(AttributeList=key,
                                                   Tool=CompFactory.EventInfoAttListTool(),
                                                   EventInfoKey=eventInfoKey)
      result.addEventAlgo(tagBuilder)

   # For xAOD output
   if "AOD" in streamName:
      outputStream.WritingTool.SubLevelBranchName = "<key>"

   result.addEventAlgo(outputStream, domain='IO')
   return result

def addToESD(configFlags, itemOrList, **kwargs):
   """
   Adds items to ESD stream

   The argument can be either list of items or just one item
   if further customisations are needed for output stream they can be passed via kwargs

   returns CA to be merged i.e.: result.merge(addToESD(flags, "xAOD::CoolObject"))
   """
   if not configFlags.Output.doWriteESD:
      return ComponentAccumulator()
   items = [itemOrList] if isinstance(itemOrList, str) else itemOrList
   return OutputStreamCfg(configFlags, "ESD", ItemList=items, **kwargs)

def addToAOD(configFlags, itemOrList, **kwargs):
   """
   Adds items to AOD stream

   @see add addToESD
   """
   if not configFlags.Output.doWriteAOD:
      return ComponentAccumulator()
   items = [itemOrList] if isinstance(itemOrList, str) else itemOrList
   return OutputStreamCfg(configFlags, "AOD", ItemList=items, **kwargs)

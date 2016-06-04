__doc__ = "Add containers to ESD/AOD ItemList using the definitions from CaloRingerKeys"

from AthenaCommon.Logging import logging

mlog = logging.getLogger( 'CaloRingerOutputItemList_jobOptions.py' )
mlog.info('Entering')

class IncludeError ( ImportError ):
  pass

from CaloRingerAlgs.CaloRingerFlags import jobproperties
if not jobproperties.CaloRingerFlags.doWriteRingsToFile():
  raise IncludeError(("Shouldn't be calling " \
    "CaloRingerOutputItemList_joboptions.py with doWriteRingsToFile " \
    "set to false."))

from RecExConfig.RecFlags import rec

from CaloRingerAlgs.CaloRingerAlgorithmBuilder \
    import CaloRingerAlgorithmBuilder

from CaloRingerAlgs.CaloRingerMetaDataBuilder \
    import CaloRingerMetaDataBuilder

from CaloRingerAlgs.CaloRingerKeys import outputCaloRingsType, \
                                          CaloRingerKeysDict, \
                                          outputRingSetConfType, \
                                          outputRingSetConfAuxType, \
                                          outputRingSetType, \
                                          outputRingSetAuxType, \
                                          outputCaloRingsType, \
                                          outputCaloRingsAuxType

# Get instance to the builder:
CRBuilder = CaloRingerAlgorithmBuilder()
# Get instance to the metadata builder:
CRMetaBuilder = CaloRingerMetaDataBuilder()

# AOD list, also added to the ESD
CaloRingerAODList = []
CaloRingerAODMetaDataList = []

def addOutputToList(streamList, localCType, localCKey, auxOption):
  streamList.append('%s#%s' % (localCType, localCKey ) )
  if 'xAOD::' in localCType:
    auxType = localCType.replace('Container', 'AuxContainer')
    auxKey =  localCKey + 'Aux.'
    streamList.append( '%s#%s%s' % (auxType, auxKey, auxOption) )

def addRingerInputMetaToList(streamList, containerType):
  # NOTE: Keys defined at: http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Reconstruction/RecExample/RecExConfig/python/InputFilePeeker.py
  __addRingerInputToList(streamList, containerType, 'metadata_itemsDic') # beware the absence of the 't'

def addRingerInputEventToList(streamList, containerType):
  __addRingerInputToList(streamList, containerType, 'eventdata_itemsDic')  # beware the absence of the 't'

def __addRingerInputToList(streamList, containerType, storeTypeStr):
  from RecExConfig.InputFilePeeker import inputFileSummary
  metaItemDict = inputFileSummary.get( storeTypeStr )
  try:
    keys = metaItemDict.get( containerType )
    if keys:
      for key in keys:
        mlog.debug('Appending container %s#%s in input file to output file', \
            containerType, \
            key)
        streamList.append( '%s#%s' % (containerType, key) )
  except KeyError:
    pass

# Add itens into lists
if rec.doWriteAOD() or rec.doWriteESD():
  if CRBuilder.usable():
    for cType, cKeys in CRBuilder.output().items():
      for cKey in cKeys:
        for dictKey, value in CaloRingerKeysDict.outputs.items():
          if value[0] == cType:
            auxOption = CaloRingerKeysDict.outputs[dictKey][2]
            addOutputToList(CaloRingerAODList, cType, cKey, auxOption)
            mlog.debug("Added container with type/key %s/%s to StoreGateSvc", cType, cKey)
            break
  elif rec.readESD() or rec.readAOD(): # In this case, we assume that the
                                       # joboption was added to add input file ringer containers 
                                       # to the output file.
    # add the event store information:
    addRingerInputEventToList( CaloRingerAODList, outputCaloRingsType() )
    addRingerInputEventToList( CaloRingerAODList, outputCaloRingsAuxType() )
    addRingerInputEventToList( CaloRingerAODList, outputRingSetType() )
    addRingerInputEventToList( CaloRingerAODList, outputRingSetAuxType() )
 
  if CRMetaBuilder.usable() and jobproperties.CaloRingerFlags.buildCaloRingsOn():
    for cType, cKeys in CRMetaBuilder.output().items():
      for cKey in cKeys:
        for dictKey, value in CaloRingerKeysDict.outputsMetaData.items():
          if value[0] == cType:
            auxOption = CaloRingerKeysDict.outputsMetaData[dictKey][2]
            addOutputToList(CaloRingerAODMetaDataList, cType, cKey, auxOption)
            mlog.debug("Added container with type/key %s/%s to MetaDataStore", cType, cKey)
            break
        else:
          raise ValueError("Could not find type/key %s/%s in dictionaries" % (cType, cKey) )
    # add the meta data information:
    addRingerInputMetaToList( CaloRingerAODMetaDataList, outputRingSetConfType() )
    addRingerInputMetaToList( CaloRingerAODMetaDataList, outputRingSetConfAuxType() )
  elif rec.readESD() or rec.readAOD():
    # add the meta data information:
    addRingerInputMetaToList( CaloRingerAODMetaDataList, outputRingSetConfType() )
    addRingerInputMetaToList( CaloRingerAODMetaDataList, outputRingSetConfAuxType() )


# List for ESD: same as AOD
CaloRingerESDList = list(CaloRingerAODList)
CaloRingerESDMetaDataList = list(CaloRingerAODMetaDataList)

# This will be done here manually for avoid changing Reco joboptions for now:
from AthenaCommon.KeyStore import CfgItemList
localESDList = CfgItemList('CaloRingerESD', items = CaloRingerESDList)
localAODList = CfgItemList('CaloRingerAOD', items = CaloRingerAODList)

localESDMetaDataList = CfgItemList('CaloRingerConfESD', items = CaloRingerESDMetaDataList)
localAODMetaDataList = CfgItemList('CaloRingerConfAOD', items = CaloRingerAODMetaDataList)

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
from AthenaCommon.Resilience import treatException

def moveAlgToEnd(stream):
  from AthenaCommon.AlgSequence import AlgSequence, dumpSequence
  topSequence = AlgSequence()
  #dumpSequence( topSequence )
  if stream in topSequence:
    delattr(topSequence, stream.getName())
  #else:
  #  raise LookupError(stream.getName())
  topSequence += stream
  #mlog.info("Printing topSequece after changing stream to "
  #  "last position: ")
  #dumpSequence(topSequence)

# Add ESD outputs if list is not empty and we are writing an ESD:
if rec.doWriteESD and \
    (bool(localESDList) or bool(localESDMetaDataList)):
  mlog.debug("Attempting to add Ringer outputs to StreamESD")
  try:
    # NOTE: Call it to retrieve full containers list
    StreamESD.ItemList += localESDList()
    StreamESD.MetadataItemList += localESDMetaDataList()
  except NameError:
    mlog.warning("Couldn't get StreamESD, we will create instead.")
    StreamESD = AthenaPoolOutputStream ( "StreamESD" )
    StreamESD.ItemList += localESDList()
    StreamESD.MetadataItemList += localESDMetaDataList()
    StreamESD.OutputFile = athenaCommonFlags.PoolESDOutput()
  # We need to be sure that Stream will be the last executing Algorithm, so
  # that the objects are saved in its latest form:
  try:
    moveAlgToEnd(StreamESD)
    # On ESD, we also need to move this alg to run afterwards StreamESD creation
    from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import MakeInputDataHeader
    makeInputDataHeaderESD = MakeInputDataHeader("MakeInputDataHeaderESD",
                                                 StreamName = "StreamESD")
    moveAlgToEnd(makeInputDataHeaderESD)
  except LookupError, e:
    treatException(("Could not find stream '%s' at topSequence."   \
      " CaloRings wont probably be at OutputFile, if existent.") % \
      str(e))
  mlog.debug("StreamESD.ItemList: \n%r " , StreamESD.ItemList)
  mlog.debug("StreamESD.MetadataItemList: \n%r " , StreamESD.MetadataItemList)

# Add AOD outputs if list is not empty and we are writing an AOD:
if rec.doWriteAOD and \
    (bool(localAODList) or bool(localAODMetaDataList)):
  mlog.debug("Attempting to add Ringer outputs to StreamAOD")
  try:
    # NOTE: Call it to retrieve full containers list
    StreamAOD.ItemList += localAODList()
    StreamAOD.MetadataItemList += localAODMetaDataList()
  except NameError:
    mlog.warning("Couldn't get StreamAOD, we will create instead.")
    StreamAOD = AthenaPoolOutputStream ( "StreamAOD" )
    StreamAOD.ItemList += localAODList()
    StreamAOD.MetadataItemList += localAODMetaDataList()
    StreamAOD.OutputFile = athenaCommonFlags.PoolAODOutput()
  try:
    moveAlgToEnd(StreamAOD)
  except LookupError as e:
    treatException(("Could not find stream '%s' at topSequence."   \
      " CaloRings wont probably be at OutputFile, if existent.") % \
      str(e))
  mlog.debug("StreamAOD.ItemList: \n%r " , StreamAOD.ItemList)
  mlog.debug("StreamAOD.MetadataItemList: \n%r " , StreamAOD.MetadataItemList)

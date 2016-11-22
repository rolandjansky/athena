__doc__ = "Add containers to ESD/AOD ItemList using the definitions from CaloRingerKeys"

from AthenaCommon.Logging import logging

mlog = logging.getLogger( 'CaloRingerOutputItemList_jobOptions.py' )
mlog.info('Entering')

class IncludeError ( ImportError ):
  pass

from CaloRingerAlgs.CaloRingerFlags import caloRingerFlags
if not caloRingerFlags.doWriteRingsToFile():
  raise IncludeError(("Shouldn't be calling " \
    "CaloRingerOutputItemList_joboptions.py with doWriteRingsToFile " \
    "set to false."))

from RecExConfig.RecFlags import rec

from CaloRingerAlgs.CaloRingerAlgorithmBuilder import CaloRingerAlgorithmBuilder

from CaloRingerAlgs.CaloRingerMetaDataBuilder import CaloRingerMetaDataBuilder

from CaloRingerAlgs.CaloRingerKeys import (outputCaloRingsType, CaloRingerKeysDict, outputRingSetConfType
                                          ,outputRingSetConfAuxType, outputRingSetType, outputRingSetAuxType
                                          ,outputCaloRingsType, outputCaloRingsAuxType)

# AOD list, also added to the ESD
caloRingerAODList = []

# The MetaData items we need to be streamed
caloRingerMetaDataList = []

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
if ( rec.doWriteAOD() or rec.doWriteESD() ) and caloRingerFlags.doWriteRingsToFile():
  # Get instance to the builder:
  CRBuilder = CaloRingerAlgorithmBuilder()
  # Get instance to the metadata builder:
  CRMetaBuilder = CaloRingerMetaDataBuilder()

  if CRBuilder.usable():
    for cType, cKeys in CRBuilder.output().items():
      for cKey in cKeys:
        for dictKey, value in CaloRingerKeysDict.outputs.items():
          if value[0] == cType:
            auxOption = CaloRingerKeysDict.outputs[dictKey][2]
            addOutputToList(caloRingerAODList, cType, cKey, auxOption)
            mlog.debug("Added container with type/key %s/%s to StoreGateSvc", cType, cKey)
            break
  elif rec.readESD() or rec.readAOD(): # In this case, we assume that the
                                       # joboption was added to add input file ringer containers 
                                       # to the output file.
    # add the event store information:
    addRingerInputEventToList( caloRingerAODList, outputCaloRingsType() )
    addRingerInputEventToList( caloRingerAODList, outputCaloRingsAuxType() )
    addRingerInputEventToList( caloRingerAODList, outputRingSetType() )
    addRingerInputEventToList( caloRingerAODList, outputRingSetAuxType() )
 
  if CRMetaBuilder.usable() and caloRingerFlags.buildCaloRingsOn():
    for cType, cKeys in CRMetaBuilder.output().items():
      for cKey in cKeys:
        for dictKey, value in CaloRingerKeysDict.outputsMetaData.items():
          if value[0] == cType:
            auxOption = CaloRingerKeysDict.outputsMetaData[dictKey][2]
            addOutputToList(caloRingerMetaDataList, cType, cKey, auxOption)
            mlog.debug("Added container with type/key %s/%s to MetaDataStore", cType, cKey)
            break
        else:
          raise ValueError("Could not find type/key %s/%s in dictionaries" % (cType, cKey) )
    # add the meta data information:
    addRingerInputMetaToList( caloRingerMetaDataList, outputRingSetConfType() )
    addRingerInputMetaToList( caloRingerMetaDataList, outputRingSetConfAuxType() )
  elif rec.readESD() or rec.readAOD():
    # add the meta data information:
    addRingerInputMetaToList( caloRingerMetaDataList, outputRingSetConfType() )
    addRingerInputMetaToList( caloRingerMetaDataList, outputRingSetConfAuxType() )


# List for ESD: same as AOD
caloRingerESDList = list(caloRingerAODList)


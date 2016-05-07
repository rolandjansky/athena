__doc__ = "Add containers to ESD/AOD ItemList using the definitions from egammaKeys"

from egammaRec.egammaRecFlags import jobproperties
from egammaRec import egammaKeys
from egammaRec.egammaKeys import egammaKeysDict
from AthenaCommon.Logging import logging
from RecExConfig.RecFlags import rec
logEgammaOutputItemList_jobOptions = logging.getLogger( 'py:egammaOutputItemList_jobOptions' )

def getItem(cType, cKey):
  "getItem(cType, cKey) -> Return item to be added to the output list: <cType>#<cKey>"
  return '%s#%s' % (cType, cKey)

def getAuxItem(cType, cKey, auxOption=''):
  "getAuxItem(cType, cKey, auxOption='') -> <cType>#<cKey>Aux.<auxOption>"
  auxType = cType.replace('Container', 'AuxContainer')
  auxKey =  cKey + 'Aux.'
  return '%s#%s%s' % (auxType, auxKey, auxOption)

def addContainer(outputList, cType, cKey):
  "addContainer(outputList, cType, cKey) -> Add container to outputList"
  # Skip containers if already in outputList
  item = getItem(cType, cKey)
  if item not in outputList:
    outputList.append( item )
  
def addAuxContainer(outputList, cType, cKey, auxOption=''):
  "addAux(outputList, cType, cKey, auxOption='') -> Add aux container to outputList"
  item = getAuxItem(cType, cKey, auxOption)
  if item not in outputList:
    outputList.append( item )


# List for of keys to be written to AOD. 
# All egammaKeys.outputs but EgammaRec and TopoSeededCellLink
AOD_outputs = [i for i,j in egammaKeysDict.outputs.items() \
  if i not in ('EgammaRec', 'TopoSeededCellLink','FwdClusterCellLink')]

# Define egammaAODList in the proper format (<type>#<key><option>),
# including aux containers
egammaAODList = []
for _ in AOD_outputs:
  cType, cKey, auxOption = egammaKeysDict.outputs[_]

  # Skip truth if doTruth = False
  if not rec.doTruth() and 'Truth' in cKey:
    continue

  # Skip Trk::Tracks in xAOD
  if egammaKeys.outputTrackType() in cType:
    continue
  
  addContainer(egammaAODList, cType, cKey)
  # Add aux containers for xAOD containers
  if 'xAOD::' in cType:
    addAuxContainer(egammaAODList, cType, cKey, auxOption)

# List for ESD: same as AOD but for tracks and links from topo-seeded clusters to cells 
egammaESDList = list(egammaAODList)
egammaESDList.append( getItem(egammaKeys.outputTrackType(), egammaKeys.outputTrackKey()) )
egammaESDList.append( getItem(egammaKeys.outputTopoSeededCellLinkType(), egammaKeys.outputTopoSeededCellLinkKey()) )
egammaESDList.append( getItem(egammaKeys.outputFwdClusterCellLinkType(), egammaKeys.outputFwdClusterCellLinkKey()) )

# Remove auxOption from TopoSeeded and Forward Clusters ==> keep cellLink 
for index, item in enumerate(egammaESDList):
  if egammaKeys.outputTopoSeededClusterKey() + 'Aux' in item:
    egammaESDList[index] = getAuxItem( egammaKeys.outputTopoSeededClusterType(), egammaKeys.outputTopoSeededClusterKey() )
  
  if egammaKeys.outputFwdClusterKey() + 'Aux' in item:
    egammaESDList[index] = getAuxItem( egammaKeys.outputFwdClusterType(), egammaKeys.outputFwdClusterKey() )


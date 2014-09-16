__doc__ = "Add containers to ESD/AOD ItemList using the definitions from egammaKeys"

from egammaRec.egammaRecFlags import jobproperties
from egammaRec import egammaKeys
from AthenaCommon.Logging import logging
from RecExConfig.RecFlags import rec
logEgammaOutputItemList_jobOptions = logging.getLogger( 'py:egammaOutputItemList_jobOptions' )

# List for of keys to be written to AOD. All egammaKeys.outputs but egammaRecContainer
AOD_outputs = [i for i,j in egammaKeys.outputs.items() if j[0] != 'egammaRecContainer']

# Define egammaAODList in the proper format (<type>#<key><option>),
# including aux containers
egammaAODList = []
for _ in AOD_outputs:
  cType, cKey, auxOption = egammaKeys.outputs[_]

  # Skip truth if doTruth = False
  if not rec.doTruth() and 'Truth' in cKey:
    continue

  #Skip Tracks in xAOD
  if egammaKeys.outputTrackType() in cType:
    continue

  egammaAODList.append( "%s#%s" % (cType, cKey) )
  # Add aux containers for xAOD containers
  if 'xAOD::' in cType:
    auxType = cType.replace('Container', 'AuxContainer')
    auxKey =  cKey + "Aux."  
    egammaAODList.append( "%s#%s%s" % (auxType, auxKey, auxOption) )


# List for ESD: same as AOD
egammaESDList = list(egammaAODList)
egammaESDList.append( "%s#%s" %  (egammaKeys.outputTrackType(), egammaKeys.outputTrackKey()) )

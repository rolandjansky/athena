from CaloRingerAlgs.CaloRingerKeys import (
    CaloRingerKeysDict,
)

from CaloRingerAlgs.CaloRingerFlags import caloRingerFlags
from RecExConfig.RecFlags import rec
__doc__ = "Add containers to ESD/AOD ItemList using the definitions from CaloRingerKeys"

from AthenaCommon.Logging import logging

mlog = logging.getLogger('CaloRingerOutputItemList_jobOptions.py')
mlog.info('Entering')


class IncludeError (ImportError):
    pass


if not caloRingerFlags.doWriteRingsToFile():
    raise IncludeError(("Shouldn't be calling "
                        "CaloRingerOutputItemList_joboptions.py with doWriteRingsToFile "
                        "set to false."))


# Avoid duplication
caloRingerAODList = []


def addOutputToList(streamList, localCType, localCKey, auxOption):
    streamList.append('%s#%s' % (localCType, localCKey))
    if 'xAOD::' in localCType:
        auxType = localCType.replace('Container', 'AuxContainer')
        streamList.append('%s#%sAux.%s' % (auxType, localCKey, auxOption))


# Add itens into lists
if (rec.doWriteAOD() or rec.doWriteESD()) and caloRingerFlags.doWriteRingsToFile():
    # For now keep it like Run-2 Electron only
    caloRingerWriteOutputs = ["ElectronRingSets", "ElectronCaloRings"]
    for dictKey, value in CaloRingerKeysDict.outputs.items():
        if dictKey in caloRingerWriteOutputs:
            cType, cKey, auxOption = value
            addOutputToList(caloRingerAODList, cType, cKey, auxOption)
            mlog.debug(
                "Added container with type/key %s/%s to StoreGateSvc", cType, cKey)


# List for ESD: same as AOD
caloRingerESDList = list(caloRingerAODList)

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """CaloRingerKeys.py contains the input and output types used by
CaloRinger Algorithm and Tools. It is inpired in egammaKeys.py by Bruno
Lenzi"""
__author__ = "Werner S. Freund"

# Temporary solution just to emulate what was being done in egammaGetter.py
from egammaRec.egammaKeys import egammaKeysDict

def outputRingSetType():
  return 'xAOD::RingSetContainer'

def outputCaloRingsType():
  return 'xAOD::CaloRingsContainer'

def outputRingSetConfType():
  return 'xAOD::RingSetConfContainer'

def outputRingSetAuxType():
  return 'xAOD::RingSetAuxContainer'

def outputCaloRingsAuxType():
  return 'xAOD::CaloRingsAuxContainer'

def outputRingSetConfAuxType():
  return 'xAOD::RingSetConfAuxContainer'

class CaloRingerKeysDict:
  
  # Copy dictionary value, key key with same name:
  inputs = dict(
    Electron  =       egammaKeysDict.outputs.get ( "Electron" ),
    Photon    =       egammaKeysDict.outputs.get ( "Photon"   ), 
    )

  # Define new outputs here:
  _output_values = [
      [outputRingSetType(),   'ElectronRingSets',      ''],
      [outputRingSetType(),   'ElectronAsymRingSets',  ''],
      [outputCaloRingsType(), 'ElectronCaloRings',     ''],
      [outputCaloRingsType(), 'ElectronCaloAsymRings', ''],
      [outputRingSetType(),   'PhotonRingSets',        ''],
      [outputRingSetType(),   'PhotonAsymRingSets',    ''],
      [outputCaloRingsType(), 'PhotonCaloRings',       ''],
      [outputCaloRingsType(), 'PhotonCaloAsymRings',   ''],
      ]

  # This will keep dict key equal to Container key:
  outputs = {value[1]: value for value in _output_values}
  
  # Keys in this dictionary must be different from _output_values 
  # for CaloRingerOutputItemList be able to distinguish if an output comes
  # from outputs or outputsMetaData
  _outputMetaData_values = [
          [outputRingSetConfType(),   'ElectronRingSetsConf'  , ''],
          [outputRingSetConfType(),   'PhotonRingSetsConf'    , ''],
          ]

  # This will keep dict key equal to Container key:
  outputsMetaData = {value[1]: value for value in _outputMetaData_values}

###################################################################
# Create methods to return the types and keys in inputs and outputs
# e.g.:
#
# from CaloRingerAlgs import CaloRingerKeys
# CaloRingerKeys.outputElectronKey()
#
for i, j in CaloRingerKeysDict.inputs.items():
  exec "def input%sType(): return '%s'" % (i, j[0])
  exec "def input%sKey(): return '%s'" % (i, j[1])
#
for i, j in CaloRingerKeysDict.outputs.items():
  exec "def output%sType(): return '%s'" % (i, j[0])
  exec "def output%sKey(): return '%s'" % (i, j[1])
#
for i, j in CaloRingerKeysDict.outputsMetaData.items():
  exec "def output%sType(): return '%s'" % (i, j[0])
  exec "def output%sKey(): return '%s'" % (i, j[1])
###################################################################


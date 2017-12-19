# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import CaloRingerToolsConf

from egammaRec.Factories import PublicToolFactory, FcnWrapper
from GaudiKernel.GaudiHandles import PublicToolHandle, PublicToolHandleArray

from CaloRingerAlgs import CaloRingerKeys
from CaloRingerAlgs.CaloRingerFlags import caloRingerFlags

def getElectronCaloRingsBuilder():
  "Return the electrons CaloRings builder "
  if caloRingerFlags.buildElectronCaloRings():
    from CaloRingerTools.CaloRingerBuilderFactories import ElectronCaloRingsBuilder, \
                                                           ElectronCaloAsymRingsBuilder
    if caloRingerFlags.useAsymBuilder():
      return ElectronCaloAsymRingsBuilder()
    else:
      return ElectronCaloRingsBuilder()
  else:
    return PublicToolHandle('')

def getPhotonCaloRingsBuilder():
  "Return the photon CaloRings builder "
  if caloRingerFlags.buildPhotonCaloRings():
    from CaloRingerTools.CaloRingerBuilderFactories import PhotonCaloRingsBuilder, \
                                                           PhotonCaloAsymRingsBuilder
    if caloRingerFlags.useAsymBuilder():
      return PhotonCaloAsymRingsBuilder()
    else:
      return PhotonCaloRingsBuilder()
  else:
    return PublicToolHandle('')

def getCaloRingerElectronSelectorsBuilder():
  "Return the Electron Selectors"
  if caloRingerFlags.doElectronIdentification():
    from CaloRingerTools.CaloRingerSelectorsBuilders import CaloRingerElectronSelectorsBuilder
    return CaloRingerElectronSelectorsBuilder().getAllSelectorHandles()
  else:
    return PublicToolHandleArray([])

def getCaloRingerPhotonSelectorsBuilder():
  "Return the Photon Selectors"
  #if caloRingerFlags.doPhotonIdentification:
  #  from CaloRingerTools.CaloRingerSelectorsBuilders import CaloRingerPhotonSelectorsBuilder
  #  return CaloRingerPhotonSelectorBuilder().getAllSelectorHandles()
  #else:
  return PublicToolHandleArray([])

from CaloRingerAlgs.CaloRingerAlgorithmBuilder import checkBuildElectronCaloRings
from CaloRingerAlgs.CaloRingerAlgorithmBuilder import checkDoElectronIdentification
CaloRingerElectronsReaderTool = PublicToolFactory(CaloRingerToolsConf.Ringer__CaloRingerElectronsReader,
                                            name = "CaloRingerElectronsReaderTool",
                                            inputKey = CaloRingerKeys.inputElectronKey(),
                                            crBuilder = FcnWrapper(getElectronCaloRingsBuilder),
                                            builderAvailable = FcnWrapper(checkBuildElectronCaloRings),
                                            ElectronSelectors = FcnWrapper(getCaloRingerElectronSelectorsBuilder),
                                            selectorsAvailable = FcnWrapper(checkDoElectronIdentification),
                                           )

from CaloRingerAlgs.CaloRingerAlgorithmBuilder import checkBuildPhotonCaloRings
from CaloRingerAlgs.CaloRingerAlgorithmBuilder import checkDoPhotonIdentification
CaloRingerPhotonsReaderTool = PublicToolFactory(CaloRingerToolsConf.Ringer__CaloRingerPhotonsReader,
                                          name = "CaloRingerPhotonsReaderTool",
                                          inputKey = CaloRingerKeys.inputPhotonKey(),
                                          crBuilder = FcnWrapper(getPhotonCaloRingsBuilder),
                                          builderAvailable = FcnWrapper(checkBuildPhotonCaloRings),
                                          PhotonSelectors = FcnWrapper(getCaloRingerPhotonSelectorsBuilder),
                                          #selectorsAvailable = FcnWrapper(checkDoPhotonIdentification)
                                         )

# Noise retrievers build rings on random calorimeter positions to simulate
# noise conditions affecting the CaloRings. Their instatiations have the shape
# used to build the rings for the original particles

# TODO Add logic whether to build one random ring per event, or to build a grid.
#CaloRingerElectronsNoiseReaderTool = ToolFactory(CaloRingerToolsConf.Ringer__CaloRingerNoiseReader,
#    name = "CaloRingerElectronsNoiseReaderTool",
#    crBuilder = FcnWrapper(ElectronsCaloRingsBuilder))

#CaloRingerPhotonsNoiseReaderTool = ToolFactory(CaloRingerToolsConf.Ringer__CaloRingerNoiseReader,
#    name = "CaloRingerPhotonsNoiseReaderTool",
#    crBuilder = FcnWrapper(PhotonsCaloRingsBuilder))



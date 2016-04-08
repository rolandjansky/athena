# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import CaloRingerToolsConf

from egammaRec.Factories import ToolFactory
from egammaRec import egammaKeys

from CaloRingerAlgs import CaloRingerKeys

def configureEgammaCaloRings(crBuilder):
  crBuilder.RingSetContainerName = 'EgammaRingSets'
  crBuilder.CaloRingsContainerName = 'EgammaCaloRings'
  crBuilder.EtaWidth = [0.025, 0.003125, 0.025, 0.05, 0.1, 0.1, 0.2]
  crBuilder.PhiWidth = [0.098174770424681, 0.098174770424681, 0.024543692606170, 
      0.024543692606170, 0.098174770424681, 0.098174770424681, 0.098174770424681]
  crBuilder.NRings = [8, 64, 8, 8, 4, 4, 4]
  crBuilder.CellMaxDEtaDist = .2
  crBuilder.CellMaxDPhiDist = .2
  try:
    import cppyy
  except ImportError:
    import PyCintex as cppyy
  from ROOT import CaloCell_ID
  # Changes in the layers may break the algorithm because it may not be able to
  # recognize whether a layer is correspondent to EM1, EM2 and so on. If it is
  # needed to update this, please enter in touch with (Werner S. Freund)
  # <wsfreund@cern.ch>

  # TODO Remove TileGap
  crBuilder.Layers = [CaloCell_ID.PreSamplerB,CaloCell_ID.PreSamplerE,
                      CaloCell_ID.EMB1, CaloCell_ID.EME1,
                      CaloCell_ID.EMB2, CaloCell_ID.EME2,
                      CaloCell_ID.EMB3, CaloCell_ID.EME3,
                      CaloCell_ID.HEC0, CaloCell_ID.TileBar0, CaloCell_ID.TileGap3, CaloCell_ID.TileExt0,
                      CaloCell_ID.HEC1, CaloCell_ID.HEC2, CaloCell_ID.TileBar1, CaloCell_ID.TileGap1, CaloCell_ID.TileExt1,
                      CaloCell_ID.HEC3, CaloCell_ID.TileBar2, CaloCell_ID.TileGap2, CaloCell_ID.TileExt2]
  # FIXME recheck amount of rings
  crBuilder.RingSetNLayers = [2, 2, 2, 2, 4, 5, 4]
  crBuilder.useShowerShapeBarycenter = False
  crBuilder.CellsContainerName = egammaKeys.caloCellKey()
  
def configureElectronCaloRings(crBuilder):
  configureEgammaCaloRings(crBuilder)
  crBuilder.CaloRingsContainerName = CaloRingerKeys.outputElectronCaloRingsKey()
  crBuilder.RingSetContainerName = CaloRingerKeys.outputElectronRingSetsKey()


def configureElectronCaloAsymRings(crBuilder):
  configureEgammaCaloRings(crBuilder)
  crBuilder.DoEtaAxesDivision = True 
  crBuilder.DoPhiAxesDivision = True
  # FIXME This should be encapsulated somewhere else...
  crBuilder.NRings = resizeNRingsAsymRings(crBuilder, crBuilder.DoEtaAxesDivision, crBuilder.DoPhiAxesDivision)
  crBuilder.CaloRingsContainerName = CaloRingerKeys.outputElectronCaloAsymRingsKey()
  crBuilder.RingSetContainerName = CaloRingerKeys.outputElectronAsymRingSetsKey()

def configurePhotonCaloRings(crBuilder):
  configureEgammaCaloRings(crBuilder)
  crBuilder.CaloRingsContainerName = CaloRingerKeys.outputPhotonCaloRingsKey()
  crBuilder.RingSetContainerName = CaloRingerKeys.outputPhotonRingSetsKey()

def configurePhotonCaloAsymRings(crBuilder):
  configureEgammaCaloRings(crBuilder)
  crBuilder.DoEtaAxesDivision = True  
  crBuilder.DoPhiAxesDivision = True 
  crBuilder.NRings = resizeNRingsAsymRings(crBuilder, crBuilder.DoEtaAxesDivision, crBuilder.DoPhiAxesDivision)
  crBuilder.CaloRingsContainerName = CaloRingerKeys.outputPhotonCaloAsymRingsKey()
  crBuilder.RingSetContainerName = CaloRingerKeys.outputPhotonAsymRingSetsKey()

def resizeNRingsAsymRings(crBuilder,doEtaAxesDivision, doPhiAxesDivision):
  if ((doEtaAxesDivision == True) and (doPhiAxesDivision == True)):
    return [(rings-1)*4+1 for rings in crBuilder.NRings]
  else:
    return [(rings-1)*2+1 for rings in crBuilder.NRings]

ElectronCaloRingsBuilder = ToolFactory(CaloRingerToolsConf.Ringer__CaloRingsBuilder,
  name = "ElectronCaloRingsBuilder",
  postInit = [configureElectronCaloRings])

ElectronCaloAsymRingsBuilder = ToolFactory(CaloRingerToolsConf.Ringer__CaloAsymRingsBuilder,
  name = "ElectronCaloAsymRingsBuilder",
  postInit = [configureElectronCaloAsymRings])

PhotonCaloRingsBuilder = ToolFactory(CaloRingerToolsConf.Ringer__CaloRingsBuilder,
  name = "PhotonCaloRingsBuilder",
  postInit = [configurePhotonCaloRings])

PhotonCaloAsymRingsBuilder = ToolFactory(CaloRingerToolsConf.Ringer__CaloAsymRingsBuilder,
  name = "PhotonCaloAsymRingsBuilder",
  postInit = [configurePhotonCaloAsymRings])

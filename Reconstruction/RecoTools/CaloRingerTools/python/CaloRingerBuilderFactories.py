# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import CaloRingerToolsConf

from egammaRec.Factories import PublicToolFactory
from egammaRec import egammaKeys

from CaloRingerAlgs import CaloRingerKeys

class ConfigureEgammaCaloRings( object ):
  """
  Base class for Egamma CaloRings builders
  """

  def __init__( self, minEnergy = None ):
    self.RingSetContainerName = 'EgammaRingSets'
    self.CaloRingsContainerName = 'EgammaCaloRings'
    self.EtaWidth = [0.025, 0.003125, 0.025, 0.05, 0.1, 0.1, 0.2]
    self.PhiWidth = [0.098174770424681, 0.098174770424681, 0.024543692606170, 
        0.024543692606170, 0.098174770424681, 0.098174770424681, 0.098174770424681]
    self.NRings = [8, 64, 8, 8, 4, 4, 4]
    self.CellMaxDEtaDist = .2
    self.CellMaxDPhiDist = .2
    from ROOT import CaloCell_ID
    # Changes in the layers may break the algorithm because it may not be able to
    # recognize whether a layer is correspondent to EM1, EM2 and so on. If it is
    # needed to update this, please enter in touch with (Werner S. Freund)
    # <wsfreund@cern.ch>

    # TODO Remove TileGap
    self.Layers = [CaloCell_ID.PreSamplerB, CaloCell_ID.PreSamplerE,
                   CaloCell_ID.EMB1,        CaloCell_ID.EME1,
                   CaloCell_ID.EMB2,        CaloCell_ID.EME2,
                   CaloCell_ID.EMB3,        CaloCell_ID.EME3,
                   CaloCell_ID.HEC0,        CaloCell_ID.TileBar0,    CaloCell_ID.TileGap3, CaloCell_ID.TileExt0,
                   CaloCell_ID.HEC1,        CaloCell_ID.HEC2,        CaloCell_ID.TileBar1, CaloCell_ID.TileGap1, CaloCell_ID.TileExt1,
                   CaloCell_ID.HEC3,        CaloCell_ID.TileBar2,    CaloCell_ID.TileGap2, CaloCell_ID.TileExt2]
    # FIXME recheck amount of rings
    self.RingSetNLayers = [2, 2, 2, 2, 4, 5, 4]
    self.useShowerShapeBarycenter = False
    self.CellsContainerName = egammaKeys.caloCellKey()

  def __call__(self, crBuilder):
    for item, val in self.__dict__.iteritems():
      setattr(crBuilder, item, val)

class ConfigureElectronCaloRings( ConfigureEgammaCaloRings ):
  """
  Configure the ElectronCaloRingsBuilder default values
  """
  def __init__(self):
    ConfigureEgammaCaloRings.__init__(self)
    self.CaloRingsContainerName = CaloRingerKeys.outputElectronCaloRingsKey()
    self.RingSetContainerName = CaloRingerKeys.outputElectronRingSetsKey()

  def __call__( self, crBuilder):
    ConfigureEgammaCaloRings.__call__( self, crBuilder )
    from CaloRingerAlgs.CaloRingerFlags import caloRingerFlags
    minEnergy = caloRingerFlags.minElectronEnergy()
    if minEnergy is not None:
      from AthenaCommon.SystemOfUnits import GeV
      crBuilder.MinPartEnergy = minEnergy * GeV


class AsymRings( object ):
  """
  Base class for configuring rings used for studying asymetric energy
  deposition
  """

  def __init__(self, doEtaAxesDivision = True, doPhiAxesDivision = True):
    self.doEtaAxesDivision = doEtaAxesDivision
    self.doPhiAxesDivision = doPhiAxesDivision

  def resizeNRingsAsymRings(self, obj):
    if ((obj.doEtaAxesDivision == True) and (obj.doPhiAxesDivision == True)):
      obj.NRings = [(rings-1)*4+1 for rings in obj.NRings]
    else:
      obj.NRings = [(rings-1)*2+1 for rings in obj.NRings]

  def __call__(self, crBuilder):
    for item, val in self.__dict__.iteritems():
      setattr(crBuilder, item, val)
    self.resizeNRingsAsymRings( crBuilder )

class ConfigurePhotonCaloRings( ConfigureEgammaCaloRings ):
  """
  Configure the PhotonCaloRingsBuilder default values
  """

  def __init__(self):
    ConfigureEgammaCaloRings.__init__(self)
    self.CaloRingsContainerName = CaloRingerKeys.outputPhotonCaloRingsKey()
    self.RingSetContainerName = CaloRingerKeys.outputPhotonRingSetsKey()

  def __call__( self, crBuilder):
    ConfigureEgammaCaloRings.__call__( self, crBuilder )
    from CaloRingerAlgs.CaloRingerFlags import caloRingerFlags
    minEnergy = caloRingerFlags.minPhotonEnergy()
    if minEnergy is not None:
      from AthenaCommon.SystemOfUnits import GeV
      crBuilder.MinPartEnergy = minEnergy * GeV

ElectronCaloRingsBuilder = PublicToolFactory(CaloRingerToolsConf.Ringer__CaloRingsBuilder,
  name = "ElectronCaloRingsBuilder",
  postInit = [ConfigureElectronCaloRings().__call__])

ElectronCaloAsymRingsBuilder = PublicToolFactory(CaloRingerToolsConf.Ringer__CaloAsymRingsBuilder,
  name = "ElectronCaloAsymRingsBuilder",
  postInit = [ ConfigureElectronCaloRings().__call__
             , AsymRings(doEtaAxesDivision = True, doPhiAxesDivision = True).__call__ ])

PhotonCaloRingsBuilder = PublicToolFactory(CaloRingerToolsConf.Ringer__CaloRingsBuilder,
  name = "PhotonCaloRingsBuilder",
  postInit = [ConfigurePhotonCaloRings().__call__])

PhotonCaloAsymRingsBuilder = PublicToolFactory(CaloRingerToolsConf.Ringer__CaloAsymRingsBuilder,
  name = "PhotonCaloAsymRingsBuilder",
  postInit = [ConfigurePhotonCaloRings().__call__
             , AsymRings(doEtaAxesDivision = True, doPhiAxesDivision = True).__call__])

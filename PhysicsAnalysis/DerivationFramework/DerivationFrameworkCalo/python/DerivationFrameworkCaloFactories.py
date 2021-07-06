# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaborationa

# Declares a GainDecorator factory. To add it to your decoration:
# from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getExtraContent
# GainDecoratorTool = GainDecorator()
# MySlimmingHelper.ExtraVariables.extend( getGainDecorations(GainDecoratorTool) )

# Currently it adds:
# ['Electrons.E_Lr0_HiG', 'Electrons.E_Lr0_MedG', 'Electrons.E_Lr0_LowG', 'Electrons.E_Lr1_HiG', 'Electrons.E_Lr1_MedG', 'Electrons.E_Lr1_LowG', 'Electrons.E_Lr2_HiG', 'Electrons.E_Lr2_MedG', 'Electrons.E_Lr2_LowG', 'Electrons.E_Lr3_HiG', 'Electrons.E_Lr3_MedG', 'Electrons.E_Lr3_LowG', 'Electrons.nCells_Lr0_HiG', 'Electrons.nCells_Lr0_MedG', 'Electrons.nCells_Lr0_LowG', 'Electrons.nCells_Lr1_HiG', 'Electrons.nCells_Lr1_MedG', 'Electrons.nCells_Lr1_LowG', 'Electrons.nCells_Lr2_HiG', 'Electrons.nCells_Lr2_MedG', 'Electrons.nCells_Lr2_LowG', 'Electrons.nCells_Lr3_HiG', 'Electrons.nCells_Lr3_MedG', 'Electrons.nCells_Lr3_LowG', 'Photons.E_Lr0_HiG', 'Photons.E_Lr0_MedG', 'Photons.E_Lr0_LowG', 'Photons.E_Lr1_HiG', 'Photons.E_Lr1_MedG', 'Photons.E_Lr1_LowG', 'Photons.E_Lr2_HiG', 'Photons.E_Lr2_MedG', 'Photons.E_Lr2_LowG', 'Photons.E_Lr3_HiG', 'Photons.E_Lr3_MedG', 'Photons.E_Lr3_LowG', 'Photons.nCells_Lr0_HiG', 'Photons.nCells_Lr0_MedG', 'Photons.nCells_Lr0_LowG', 'Photons.nCells_Lr1_HiG', 'Photons.nCells_Lr1_MedG', 'Photons.nCells_Lr1_LowG', 'Photons.nCells_Lr2_HiG', 'Photons.nCells_Lr2_MedG', 'Photons.nCells_Lr2_LowG', 'Photons.nCells_Lr3_HiG', 'Photons.nCells_Lr3_MedG', 'Photons.nCells_Lr3_LowG']

from egammaRec.Factories import ToolFactory, getPropertyValue
from egammaRec import egammaKeys
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__GainDecorator, DerivationFramework__ClusterEnergyPerLayerDecorator
from CaloClusterCorrection.CaloClusterCorrectionConf import CaloFillRectangularCluster as CFRC
from GaudiKernel.Constants import DEBUG

GainDecorator = ToolFactory( DerivationFramework__GainDecorator, 
  SGKey_electrons = egammaKeys.outputElectronKey(),
  SGKey_photons = egammaKeys.outputPhotonKey(), OutputLevel=DEBUG )

# Define defaults for CaloFillRectangularCluster and ClusterEnergyPerLayerDecorator
# will be overridden later depending on the number of cells neta X nphi
CaloFillRectangularCluster = ToolFactory(CFRC,
  cells_name = egammaKeys.caloCellKey(), fill_cluster = True )

ClusterEnergyPerLayerDecorator = ToolFactory(
  DerivationFramework__ClusterEnergyPerLayerDecorator,
  CaloFillRectangularClusterTool = CaloFillRectangularCluster,
  SGKey_electrons = egammaKeys.outputElectronKey(),
  SGKey_photons = egammaKeys.outputPhotonKey(), 
  SGKey_caloCells = egammaKeys.caloCellKey() )

def getGainLayerNames( obj ):
  """getGainLayerNames( obj ) -> return a list of names of the decorations added to the
  egamma object, given the GainDecorator object"""
  return [getPropertyValue(obj, "decoration_pattern").format(info=info, layer=layer, gain=gain) \
    for info in ["E", "nCells"] \
    for layer in getPropertyValue(obj, "layers") \
    for gain in getPropertyValue(obj, "gain_names").values() ]

def getGainDecorations( obj, 
  collections=[egammaKeys.outputElectronKey(), egammaKeys.outputPhotonKey()],
  info = ["E", "nCells"] ):
  """getGainDecorations( obj, collections=["Electrons", "Photons"] ) -> 
  Return a list with the 'ExtraContent' to be added to the decorations to save the gain
  information per layer"""
  return ["{part}.{info}".format(part=part, info=info) for part in collections \
    for info in getGainLayerNames(obj) ]

def getClusterEnergyPerLayerDecorator( neta, nphi, prefix = '', **kw ):
  """Return a factory for ClusterEnergyPerLayerDecorator given neta, nphi"""
  return ClusterEnergyPerLayerDecorator.copyPublic(
    name = "%sClusterEnergyPerLayerDecorator_%sx%s" % (prefix, neta, nphi),
    CaloFillRectangularClusterTool = CaloFillRectangularCluster.copy(
      name = "%sCaloFillRectangularCluster_%sx%s" % (prefix, neta, nphi), 
      eta_size = neta,
      phi_size = nphi),
    neta = neta,
    nphi = nphi,
    **kw)

def getClusterEnergyPerLayerDecorations( obj ):
  """getClusterEnergyPerLayerDecorations( obj ) -> return a list of names of the
  decorations added to the egamma object, given the ClusterEnergyPerLayerDecorations
  object (e.g. Photons.E7x11_Lr0, ...)"""
  properties = 'SGKey_photons', 'SGKey_electrons'
  collections = filter(bool, (getPropertyValue(obj, x) for x in properties) )
  return ['{part}.E{neta}x{nphi}_Lr{layer}'.format(part=part, 
    neta=getPropertyValue(obj, 'neta'),
    nphi=getPropertyValue(obj, 'nphi'),
    layer=layer) \
    for layer in getPropertyValue(obj, "layers") \
    for part in collections]

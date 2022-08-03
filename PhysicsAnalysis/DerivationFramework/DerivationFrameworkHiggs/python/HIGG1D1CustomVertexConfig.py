# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory



def PrimaryVertexRefittingToolCfg(ConfigFlags, **kwargs):

    """ PV refitting tool """

    acc = ComponentAccumulator() 

    from TrkConfig.TrkVertexFitterUtilsConfig import TrackToVertexIPEstimatorCfg
    trkToIP = acc.popToolsAndMerge( TrackToVertexIPEstimatorCfg(ConfigFlags,**kwargs) )
    kwargs.setdefault( "TrackToVertexIPEstimator", trkToIP )
    acc.setPrivateTools( CompFactory.Analysis.PrimaryVertexRefitter( **kwargs) ) 

    return acc

def ZeeVertexRefittingToolCfg(ConfigFlags, **kwargs):
    
    """ PV refitting after removing Z->ee tracks, for vertex studies """

    acc = ComponentAccumulator()

    import AthenaCommon.SystemOfUnits as Units

    pvRefitter = acc.popToolsAndMerge( PrimaryVertexRefittingToolCfg(ConfigFlags, **kwargs) )
    acc.setPrivateTools( CompFactory.DerivationFramework.ZeeVertexRefittingTool( name = "HIGG1D1_ZeeVertexRefitterTool",
                                              ObjectRequirements="(Electrons.DFCommonElectronsLHMedium) && (Electrons.pt > 19.*GeV)",
                                              LowMassCut=50*Units.GeV,
                                              RefittedPVContainerName="ZeeRefittedPrimaryVertices",                                    
                                              ElectronContainerName="Electrons",
                                              PVContainerName="PrimaryVertices",
                                              MCSamples = [361106],
                                              PrimaryVertexRefitterTool = pvRefitter ) ) 
    
    return acc
  


def ZeeVertexRefitterCfg(ConfigFlags):

    """ PV refitting after removing Z->ee tracks, for vertex studies """


    # Creates a vertex container (ZeeRefittedPrimaryVertices) where the type=1 vertex is refitted
    # after removing tracks that are associated with Z->ee decay candidates
    # Tool runs only for data and Zee MC samples (must be defined in the MCSamples list)

    acc = ComponentAccumulator()
    CommonAugmentation = CompFactory.DerivationFramework.CommonAugmentation
    ZeeVertexRefittingTool = acc.popToolsAndMerge(ZeeVertexRefittingToolCfg(ConfigFlags))
    acc.addPublicTool(ZeeVertexRefittingTool)
    acc.addEventAlgo(CommonAugmentation("ZeeVertexRefitKernel",AugmentationTools=[ZeeVertexRefittingTool])) 
    return acc


def PhotonPointingToolCfg(ConfigFlags):
    acc = ComponentAccumulator()
    acc.setPrivateTools( CompFactory.CP.PhotonPointingTool( isSimulation = ConfigFlags.Input.isMC  ) )
    return acc

def PhotonVertexSelectionToolCfg(ConfigFlags, **kwargs):
    acc = ComponentAccumulator()
    if "PhotonPointingTool" not in kwargs:
        photonPointingTool = acc.popToolsAndMerge( PhotonPointingToolCfg(ConfigFlags) )
        kwargs.setdefault("PhotonPointingTool", photonPointingTool)
    acc.setPrivateTools( CompFactory.CP.PhotonVertexSelectionTool( **kwargs ) )
    return acc

def DiphotonVertexDecoratorCfg(ConfigFlags, **kwargs):
    acc = ComponentAccumulator()
    if "PhotonVertexSelectionTool" not in kwargs:
        photonVertexSelectionTool = acc.popToolsAndMerge( PhotonVertexSelectionToolCfg(ConfigFlags) )
        kwargs.setdefault("PhotonVertexSelectionTool", photonVertexSelectionTool)
    acc.setPrivateTools( CompFactory.DerivationFramework.DiphotonVertexDecorator( **kwargs ) )
    return acc

def DiPhotonVertexCfg(ConfigFlags):
    """ Diphoton vertex decoration tool """
    # Decorator creates a shallow copy of PrimaryVertices (HggPrimaryVertices) for diphoton events
    # Must be created before the jetalg in the sequence as it is input to the modified PFlow jets
    acc = ComponentAccumulator()
    DiphotonVertexDecorator =  acc.popToolsAndMerge( DiphotonVertexDecoratorCfg(ConfigFlags) )
    acc.addPublicTool(DiphotonVertexDecorator)
    CommonAugmentation = CompFactory.DerivationFramework.CommonAugmentation
    acc.addEventAlgo(CommonAugmentation("DiphotonVertexKernel",AugmentationTools=[DiphotonVertexDecorator]))
    return acc

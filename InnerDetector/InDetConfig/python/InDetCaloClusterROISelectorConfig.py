# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetCaloClusterROISelector package
from AthenaConfiguration.ComponentFactory import CompFactory
import AthenaCommon.SystemOfUnits as Units
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def CaloDepthEntranceCfg(flags, name="CaloDepthToolEntrance", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("DepthChoice", "entrance")
    acc.setPrivateTools(CompFactory.CaloDepthTool(name, **kwargs))
    return acc

def CaloSurfaceBuilderEntranceCfg(flags, name="CaloSurfaceBuilderEntrance", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("CaloDepthTool", acc.popToolsAndMerge(CaloDepthEntranceCfg(flags)))
    acc.setPrivateTools( CompFactory.CaloSurfaceBuilder(name, **kwargs) )
    return acc

def CaloClusterROIPhiRZContainerMakerCfg(ConfigFlags, name="CaloClusterROIPhiRZContainerMaker", **kwargs):
    from egammaAlgs.egammaTopoClusterCopierConfig import egammaTopoClusterCopierCfg
    result = egammaTopoClusterCopierCfg(ConfigFlags)

    if "CaloSurfaceBuilder" not in kwargs :
        kwargs.setdefault("CaloSurfaceBuilder", result.popToolsAndMerge( CaloSurfaceBuilderEntranceCfg(ConfigFlags) ))

    kwargs.setdefault("InputClusterContainerName",  ConfigFlags.Egamma.Keys.Internal.EgammaTopoClusters)
    kwargs.setdefault("EMEnergyOnly", True)

    OutputROIContainerName=[]
    minPt=[]
    phiWidth=[]
    if False : # Set to True if TRT_SeededTrackFinder_ATL.SearchInCaloROI is true @TODO introduce flag or remove support since this does not seem to work?
        OutputROIContainerName.append('InDetCaloClusterROIPhiRZ3GeV')
        minPt.append(ConfigFlags.InDet.Tracking.ActivePass.minSecondaryPt)
        phiWidth.append(0.3) # must be equal or larger than phiWidth of its clients: TRT_SeededTrackFinder_ATL (phiWidth)

    if ConfigFlags.InDet.Tracking.ActivePass.RoISeededBackTracking :
        # TRT_TrackSegmentsFinder
        # TRT_SeededTrackFinder
        pt_cut = ConfigFlags.InDet.Tracking.ActivePass.minRoIClusterEt
        OutputROIContainerName.append('InDetCaloClusterROIPhiRZ%.0fGeVUnordered' % (pt_cut/Units.GeV))
        minPt.append(pt_cut)
        phiWidth.append(0.)  # no phi ordering, no Roi duplication close to +- pi

    if ConfigFlags.InDet.Tracking.doCaloSeededBrem :
        OutputROIContainerName.append('InDetCaloClusterROIPhiRZ0GeV')
        minPt.append(0)
        phiWidth.append(ConfigFlags.InDet.Tracking.ActivePass.phiWidthBrem) # must be equal or larger than phiWidth of its clients: InDetSiTrackMaker (phiWidt)

    if ConfigFlags.InDet.Tracking.doCaloSeededAmbi and ConfigFlags.Detector.EnableCalo :
        OutputROIContainerName.append('InDetCaloClusterROIPhiRZ10GeV')
        minPt.append(10000)
        phiWidth.append(0.05) # must be equal or larger than phiWidth of its clients: InDetAmbiTrackSelectionTool

    if ConfigFlags.InDet.Tracking.doBremRecovery and ConfigFlags.InDet.Tracking.doCaloSeededBrem and ConfigFlags.Detector.EnableCalo :
        OutputROIContainerName.append('InDetCaloClusterROIPhiRZ5GeV')
        minPt.append(5000)
        phiWidth.append(0.075) # must be equal or larger than phiWidth of its clients: InDetNNScoringTool (phiWidthEM)

    kwargs.setdefault("OutputROIContainerName", OutputROIContainerName)
    kwargs.setdefault("minPt", minPt)
    kwargs.setdefault("phiWidth", phiWidth)

    if "egammaCaloClusterSelector" not in kwargs:
        from egammaCaloTools.egammaCaloToolsConfig import egammaCaloClusterSelectorCfg
        kwargs["egammaCaloClusterSelector"] = result.popToolsAndMerge(egammaCaloClusterSelectorCfg(ConfigFlags))
    result.addEventAlgo(CompFactory.InDet.CaloClusterROIPhiRZContainerMaker(name, **kwargs), primary=True)
    return result

def ITkCaloClusterROIPhiRZContainerMakerCfg(ConfigFlags, name="CaloClusterROIPhiRZContainerMaker", **kwargs):

    from egammaAlgs.egammaTopoClusterCopierConfig import egammaTopoClusterCopierCfg
    result = egammaTopoClusterCopierCfg(ConfigFlags)

    if "CaloSurfaceBuilder" not in kwargs :
        kwargs.setdefault("CaloSurfaceBuilder", result.popToolsAndMerge( CaloSurfaceBuilderEntranceCfg(ConfigFlags) ))

    kwargs.setdefault("InputClusterContainerName",  ConfigFlags.Egamma.Keys.Internal.EgammaTopoClusters)
    kwargs.setdefault("EMEnergyOnly", True)

    OutputROIContainerName=[]
    minPt=[]
    phiWidth=[]

    if ConfigFlags.ITk.Tracking.doCaloSeededBrem :
        OutputROIContainerName.append('ITkCaloClusterROIPhiRZ0GeV')
        minPt.append(0)
        phiWidth.append(ConfigFlags.ITk.Tracking.ActivePass.phiWidthBrem[0]) # value from central eta bin
        # must be equal or larger than phiWidth of its clients: InDetSiTrackMaker (phiWidth)

    if ConfigFlags.ITk.Tracking.doCaloSeededAmbi :
        OutputROIContainerName.append('ITkCaloClusterROIPhiRZ10GeV')
        minPt.append(10000)
        phiWidth.append(0.05) # must be equal or larger than phiWidth of its clients: InDetAmbiTrackSelectionTool

    if ConfigFlags.ITk.Tracking.doConversionFinding:
        OutputROIContainerName.append('ITkCaloClusterROIPhiRZ15GeVUnordered')
        minPt.append(15000)
        phiWidth.append(0.) # no phi ordering, no Roi duplication close to +- pi

    kwargs.setdefault("OutputROIContainerName", OutputROIContainerName)
    kwargs.setdefault("minPt", minPt)
    kwargs.setdefault("phiWidth", phiWidth)

    if "egammaCaloClusterSelector" not in kwargs:
        from egammaCaloTools.egammaCaloToolsConfig import egammaCaloClusterSelectorCfg
        kwargs["egammaCaloClusterSelector"] = result.popToolsAndMerge(egammaCaloClusterSelectorCfg(ConfigFlags))

    result.addEventAlgo(CompFactory.InDet.CaloClusterROIPhiRZContainerMaker(name, **kwargs), primary=True)
    return result

def HadCaloClusterROIPhiRZContainerMakerCfg(ConfigFlags, name="HadCaloClusterROIPhiRZContainerMaker", **kwargs):
    from egammaAlgs.egammaTopoClusterCopierConfig import egammaTopoClusterCopierCfg
    result = egammaTopoClusterCopierCfg(ConfigFlags)

    kwargs.setdefault("InputClusterContainerName",  "CaloCalTopoClusters")

    if "CaloSurfaceBuilder" not in kwargs :
        kwargs.setdefault("CaloSurfaceBuilder", result.popToolsAndMerge( CaloSurfaceBuilderEntranceCfg(ConfigFlags) ))

    OutputROIContainerName=[]
    minPt=[]
    phiWidth=[]

    if ConfigFlags.InDet.Tracking.doHadCaloSeededSSS and ConfigFlags.Detector.EnableCalo:
        OutputROIContainerName.append("InDetHadCaloClusterROIPhiRZ")
        minPt.append(0)
        phiWidth.append(ConfigFlags.InDet.Tracking.ActivePass.phiWidthBrem) # must be equal or larger than phiWidth of its clients: InDetSiTrackMaker (phiWidth)

    if ConfigFlags.InDet.Tracking.doCaloSeededAmbi and ConfigFlags.Detector.EnableCalo :
        OutputROIContainerName.append("InDetHadCaloClusterROIPhiRZBjet")
        minPt.append(0)
        phiWidth.append(0.05) # must be equal or larger than phiWidth of its clients: InDetAmbiTrackSelectionTool

    kwargs.setdefault("OutputROIContainerName", OutputROIContainerName)
    kwargs.setdefault("minPt", minPt)
    kwargs.setdefault("phiWidth", phiWidth)

    if "egammaCaloClusterSelector" not in kwargs:
        from egammaCaloTools.egammaCaloToolsConfig import egammaHadCaloClusterSelectorCfg
        kwargs["egammaCaloClusterSelector"] = result.popToolsAndMerge(egammaHadCaloClusterSelectorCfg(ConfigFlags))
    result.addEventAlgo(CompFactory.InDet.CaloClusterROIPhiRZContainerMaker(name, **kwargs), primary=True)
    return result

def ITkHadCaloClusterROIPhiRZContainerMakerCfg(ConfigFlags, name="HadCaloClusterROIPhiRZContainerMaker", **kwargs):
    from egammaAlgs.egammaTopoClusterCopierConfig import egammaTopoClusterCopierCfg
    result = egammaTopoClusterCopierCfg(ConfigFlags)

    kwargs.setdefault("InputClusterContainerName",  "CaloCalTopoClusters")

    if "CaloSurfaceBuilder" not in kwargs :
        kwargs.setdefault("CaloSurfaceBuilder", result.popToolsAndMerge( CaloSurfaceBuilderEntranceCfg(ConfigFlags) ))

    OutputROIContainerName=[]
    minPt=[]
    phiWidth=[]

    if ConfigFlags.ITk.Tracking.doHadCaloSeededSSS :
        OutputROIContainerName.append("ITkHadCaloClusterROIPhiRZ")
        minPt.append(0)
        phiWidth.append(ConfigFlags.ITk.Tracking.ActivePass.phiWidthBrem[0]) # value from central eta bin
        # must be equal or larger than phiWidth of its clients: InDetSiTrackMaker (phiWidth)

    if ConfigFlags.ITk.Tracking.doCaloSeededAmbi :
        OutputROIContainerName.append("ITkHadCaloClusterROIPhiRZBjet")
        minPt.append(0)
        phiWidth.append(0.05) # must be equal or larger than phiWidth of its clients: InDetAmbiTrackSelectionTool

    kwargs.setdefault("OutputROIContainerName", OutputROIContainerName)
    kwargs.setdefault("minPt", minPt)
    kwargs.setdefault("phiWidth", phiWidth)

    if "egammaCaloClusterSelector" not in kwargs:
        from egammaCaloTools.egammaCaloToolsConfig import egammaHadCaloClusterSelectorCfg
        kwargs["egammaCaloClusterSelector"] = result.popToolsAndMerge(egammaHadCaloClusterSelectorCfg(ConfigFlags))

    result.addEventAlgo(CompFactory.InDet.CaloClusterROIPhiRZContainerMaker(name, **kwargs), primary=True)
    return result

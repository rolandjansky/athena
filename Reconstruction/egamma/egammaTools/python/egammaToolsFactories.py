# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = """ToolFactories to instantiate
all egammaTools with default configuration"""
__author__ = "Bruno Lenzi"


from ElectronPhotonSelectorTools.EgammaPIDdefs import egammaPID
from egammaTools import egammaToolsConf
from egammaRec.Factories import ToolFactory
from egammaRec import egammaKeys
from egammaCaloTools.egammaCaloToolsFactories import (
  egammaShowerShape, egammaIso)
from CaloIdentifier import SUBCALO
from AthenaConfiguration.AllConfigFlags import ConfigFlags


_clusterTypes = dict(
    Ele35='ele35', Ele55='ele55', Ele37='ele37',
    Gam35='gam35_unconv', Gam55='gam55_unconv', Gam37='gam37_unconv',
    Econv35='gam35_conv', Econv55='gam55_conv', Econv37='gam37_conv'
)


# Configure corrections for superclusters.
def configureSuperClusterCorrections(swTool):
    """Add attributes ClusterCorrectionToolsXX to egammaSwTool
       object for corrections for superclusters."""
    from CaloClusterCorrection.CaloSwCorrections import make_CaloSwCorrections
    from CaloRec.CaloRecMakers import _process_tools

    for attrName, clName in _clusterTypes.items():
        n = 'ClusterCorrectionToolsSuperCluster' + attrName
        if not hasattr(swTool, n) or getattr(swTool, n):
            continue

        setattr(swTool, n, _process_tools(
            swTool,
            make_CaloSwCorrections(
                clName,
                suffix='EGSuperCluster',
                version=ConfigFlags.Egamma.Calib.SuperClusterCorrectionVersion,
                cells_name=egammaKeys.caloCellKey())))


egammaSwSuperClusterTool = ToolFactory(
    egammaToolsConf.egammaSwTool,
    postInit=[configureSuperClusterCorrections])

EMFourMomBuilder = ToolFactory(egammaToolsConf.EMFourMomBuilder)


class EMPIDBuilderPhotonBase (egammaToolsConf.EMPIDBuilder):
    __slots__ = ()

    def __init__(self, name="EMPIDBuilderPhotonBase"):
        egammaToolsConf.EMPIDBuilder.__init__(self, name)
        # photon Selectors
        from ElectronPhotonSelectorTools.ConfiguredAsgPhotonIsEMSelectors \
            import ConfiguredAsgPhotonIsEMSelector
        LoosePhotonSelector = ConfiguredAsgPhotonIsEMSelector(
            "LoosePhotonSelector", egammaPID.PhotonIDLoose)
        TightPhotonSelector = ConfiguredAsgPhotonIsEMSelector(
            "TightPhotonSelector", egammaPID.PhotonIDTight)

        self.photonIsEMselectors = [LoosePhotonSelector, TightPhotonSelector]
        self.photonIsEMselectorResultNames = ["Loose", "Tight"]
        self.LuminosityTool = None


PhotonPIDBuilder = ToolFactory(
    EMPIDBuilderPhotonBase,
    name="PhotonPIDBuilder")


EMShowerBuilder = ToolFactory(
    egammaToolsConf.EMShowerBuilder,
    CellsName=egammaKeys.caloCellKey(),
    CaloNums=[SUBCALO.LAREM, SUBCALO.LARHEC, SUBCALO.TILE],
    ShowerShapeTool=egammaShowerShape,
    HadronicLeakageTool=egammaIso,
    Print=False)

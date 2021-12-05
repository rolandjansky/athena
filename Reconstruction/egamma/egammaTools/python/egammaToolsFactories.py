# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """ToolFactories to instantiate
all egammaTools with default configuration"""
__author__ = "Bruno Lenzi"


from .EMPIDBuilderBase import EMPIDBuilderPhotonBase
from ElectronPhotonSelectorTools import ElectronPhotonSelectorToolsConf


from egammaTools import egammaToolsConf
from egammaRec.Factories import ToolFactory
from egammaRec import egammaKeys
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

EGammaAmbiguityTool = ToolFactory(
    ElectronPhotonSelectorToolsConf.EGammaAmbiguityTool)

EMFourMomBuilder = ToolFactory(egammaToolsConf.EMFourMomBuilder)

# Photon Selectors
PhotonPIDBuilder = ToolFactory(
    EMPIDBuilderPhotonBase,
    name="PhotonPIDBuilder")

# -------------------------

# Import the factories that are not defined here
from .EMShowerBuilder import EMShowerBuilder            # noqa: F401

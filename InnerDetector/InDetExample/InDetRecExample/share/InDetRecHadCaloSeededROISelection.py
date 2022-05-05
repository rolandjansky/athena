# ------------------------------------------------------------
#
# ----------- loading the selection tools for Calo Seeded Brem
#
# ------------------------------------------------------------

#We are building ROIs corresponding (ideally) to high pt Bs

#
# --- load the tool to check the energy deposits and select clusters
#
from egammaRec.Factories import ToolFactory
from egammaCaloTools import egammaCaloToolsConf
from InDetRecExample.InDetJobProperties import InDetFlags

egammaCaloClusterHadROISelector = ToolFactory( egammaCaloToolsConf.egammaCaloClusterSelector,
                                               name = 'caloClusterHadROISelector',
                                               egammaCheckEnergyDepositTool = "",
                                               ClusterEtCut = 150e3
                                              )

def HadCaloClusterROIPhiRZContainerMaker(name="HadCaloClusterROIPhiRZContainerMaker", **kwargs):

    # if "CaloSurfaceBuilder" not in kwargs :
    #     from CaloTrackingGeometry.CaloSurfaceBuilderBase import CaloSurfaceBuilderFactory
    #     kwargs.setdefault("CaloSurfaceBuilder", CaloSurfaceBuilderFactory( '' ) )

    kwargs.setdefault("InputClusterContainerName",  "CaloCalTopoClusters")

    OutputROIContainerName=[]
    minPt=[]
    phiWidth=[]

    if InDetFlags.doHadCaloSeededSSS() :
        OutputROIContainerName.append("InDetHadCaloClusterROIPhiRZ")
        minPt.append(0)
        phiWidth.append(0.3) # must be equal or larger than phiWidth of its clients: InDetSiTrackMaker (phiWidthBrem())

    if InDetFlags.doCaloSeededAmbi() :
        OutputROIContainerName.append("InDetHadCaloClusterROIPhiRZBjet")
        minPt.append(0)
        phiWidth.append(0.05) # must be equal or larger than phiWidth of its clients: InDetAmbiTrackSelectionTool

    kwargs.setdefault("OutputROIContainerName", OutputROIContainerName)
    kwargs.setdefault("minPt", minPt)
    kwargs.setdefault("phiWidth", phiWidth)

    if "egammaCaloClusterSelector" not in kwargs:
        kwargs["egammaCaloClusterSelector"] = egammaCaloClusterHadROISelector()
    from InDetCaloClusterROISelector.InDetCaloClusterROISelectorConf import InDet__CaloClusterROIPhiRZContainerMaker
    return InDet__CaloClusterROIPhiRZContainerMaker(name, **kwargs)


if InDetFlags.doCaloSeededAmbi() :
    topSequence += HadCaloClusterROIPhiRZContainerMaker()


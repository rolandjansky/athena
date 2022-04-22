# ------------------------------------------------------------
#
# ----------- loading the selection tools for Calo Seeded Brem
#
# ------------------------------------------------------------

#
# --- load the tool to check the energy deposits and select clusters
#
from InDetCaloClusterROISelector.InDetCaloClusterROISelectorConf import (
    InDet__CaloClusterROI_Selector)
from InDetCaloClusterROIBuilder.InDetCaloClusterROIBuilderConf import (
    InDet__CaloClusterROI_Builder)
from InDetRecExample.InDetKeys import InDetKeys
from egammaRec.Factories import ToolFactory
from egammaCaloTools.egammaCaloToolsFactories import (
    egammaCheckEnergyDepositTool)
from egammaCaloTools import egammaCaloToolsConf
from InDetRecExample.InDetJobProperties import InDetFlags

egammaCaloClusterROISelector = ToolFactory(
    egammaCaloToolsConf.egammaCaloClusterSelector,
    name='caloClusterROISelector',
    egammaCheckEnergyDepositTool=egammaCheckEnergyDepositTool,
    EMEtCut=2250.,
    EMEtSplittingFraction=0.7,
    EMFCut=0.7,
    RetaCut=0.65,
    HadLeakCut=0.15
)
#
# --- get the builder tool
#
InDetCaloClusterROIBuilder = InDet__CaloClusterROI_Builder(
    name="InDetCaloClusterROIBuilder",
    EMEnergyOnly=True)

if (InDetFlags.doPrintConfigurables()):
    printfunc(InDetCaloClusterROIBuilder)

#
# --- now load the algorithm
#
InDetCaloClusterROISelector = InDet__CaloClusterROI_Selector(
    name="InDetCaloClusterROISelector",
    InputClusterContainerName=InDetKeys.CaloClusterContainer(),
    # "InDetCaloClusterROIs"
    OutputClusterContainerName=InDetKeys.CaloClusterROIContainer(),
    CaloClusterROIBuilder=InDetCaloClusterROIBuilder,
    egammaCaloClusterSelector=egammaCaloClusterROISelector()
)


def CaloClusterROIPhiRZContainerMaker(name="CaloClusterROIPhiRZContainerMaker", **kwargs):

    # if "CaloSurfaceBuilder" not in kwargs :
    #    from CaloTrackingGeometry.CaloSurfaceBuilderBase import CaloSurfaceBuilderFactory
    #    kwargs.setdefault("CaloSurfaceBuilder", CaloSurfaceBuilderFactory( '' ) )

    kwargs.setdefault("InputClusterContainerName",  InDetKeys.CaloClusterContainer())
    kwargs.setdefault("EMEnergyOnly", True)

    OutputROIContainerName=[]
    minPt=[]
    phiWidth=[]
    if InDetFlags.doCaloSeededBrem() :
        OutputROIContainerName.append('InDetCaloClusterROIPhiRZ0GeV')
        minPt.append(0)
        phiWidth.append(0.3) # must be equal or larger than phiWidth of its clients: InDetSiTrackMaker (NewTrackingCuts.phiWidthBrem())

    if False : # Set to True if TRT_SeededTrackFinder_ATL.SearchInCaloROI is true @TODO introduce flag or remove support since this does not seem to work?
        OutputROIContainerName.append('InDetCaloClusterROIPhiR12GeV')
        minPt.append(12000.)  # NewTrackingCuts.minSecondaryPt(); TRT_SeededTrackFinder_ATL
        phiWidth.append(0.3) # must be equal or larger than phiWidth of its clients: TRT_SeededTrackFinder_ATL (phiWidth)

    if InDetFlags.doCaloSeededAmbi() :
        OutputROIContainerName.append('InDetCaloClusterROIPhiRZ10GeV')
        minPt.append(10000)
        phiWidth.append(0.05) # must be equal or larger than phiWidth of its clients: InDetAmbiTrackSelectionTool

    kwargs.setdefault("OutputROIContainerName", OutputROIContainerName)
    kwargs.setdefault("minPt", minPt)
    kwargs.setdefault("phiWidth", phiWidth)

    if "egammaCaloClusterSelector" not in kwargs:
        kwargs["egammaCaloClusterSelector"] = egammaCaloClusterROISelector()

    from InDetCaloClusterROISelector.InDetCaloClusterROISelectorConf import InDet__CaloClusterROIPhiRZContainerMaker
    return InDet__CaloClusterROIPhiRZContainerMaker(name, **kwargs)


topSequence += InDetCaloClusterROISelector
topSequence += CaloClusterROIPhiRZContainerMaker()

if (InDetFlags.doPrintConfigurables()):
    printfunc(InDetCaloClusterROISelector)

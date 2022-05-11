# ------------------------------------------------------------
#
# ----------- loading the selection tools for Calo Seeded Brem
#
# ------------------------------------------------------------

#
# --- load the tool to check the energy deposits and select clusters
#
from InDetRecExample.InDetKeys import InDetKeys
from egammaRec.Factories import ToolFactory
from egammaCaloTools.egammaCaloToolsFactories import (
    egammaCheckEnergyDepositTool)
from egammaCaloTools import egammaCaloToolsConf
from InDetRecExample.InDetJobProperties import InDetFlags
import AthenaCommon.SystemOfUnits as Units

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


def CaloClusterROIPhiRZContainerMaker(name="CaloClusterROIPhiRZContainerMaker", tracking_cuts_list = [ConfiguredNewTrackingCuts("Offline")], **kwargs):
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

    unordered_pt=[]
    unordered_pt_id=[]
    for tracking_cuts in tracking_cuts_list :
        # TRT_TrackSegmentsFinder
        # TRT_SeededTrackFinder
        if InDetFlags.doCaloSeededTRTSegments() or tracking_cuts.RoISeededBackTracking():
            pt_cut = tracking_cuts.minRoIClusterEt()
            if pt_cut not in unordered_pt :
                unordered_pt.append(pt_cut)
                an_id = '%.0f' % (unordered_pt[-1]/Units.GeV)
                if an_id in unordered_pt_id :
                    raise Exception('Calo ROIPhiRZContainer Id not unique for minRoIClusterEt ' % unordered_pt[-1])
                unordered_pt_id.append(an_id)

    for a_pt in unordered_pt :
        OutputROIContainerName.append('InDetCaloClusterROIPhiRZ%.0fGeVUnordered' % (a_pt/Units.GeV))
        minPt.append(a_pt)
        phiWidth.append(0.)  # no phi ordering, no Roi duplication close to +- pi

    if InDetFlags.doCaloSeededAmbi() :
        OutputROIContainerName.append('InDetCaloClusterROIPhiRZ10GeV')
        minPt.append(10000)
        phiWidth.append(0.05) # must be equal or larger than phiWidth of its clients: InDetAmbiTrackSelectionTool

    if InDetFlags.doBremRecovery() and InDetFlags.doCaloSeededBrem() :
        OutputROIContainerName.append('InDetCaloClusterROIPhiRZ5GeV')
        minPt.append(5000)
        phiWidth.append(0.075) # must be equal or larger than phiWidth of its clients: InDetNNScoringTool (phiWidthEM)

    kwargs.setdefault("OutputROIContainerName", OutputROIContainerName)
    kwargs.setdefault("minPt", minPt)
    kwargs.setdefault("phiWidth", phiWidth)

    if "egammaCaloClusterSelector" not in kwargs:
        kwargs["egammaCaloClusterSelector"] = egammaCaloClusterROISelector()

    from InDetCaloClusterROISelector.InDetCaloClusterROISelectorConf import InDet__CaloClusterROIPhiRZContainerMaker
    return InDet__CaloClusterROIPhiRZContainerMaker(name, **kwargs)


tracking_cuts=[]
if "InDetNewTrackingCuts" in dir() :
    tracking_cuts.append( InDetNewTrackingCuts )
else :
    from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
    tracking_cuts.append( ConfiguredNewTrackingCuts("Offline") )

topSequence += CaloClusterROIPhiRZContainerMaker(tracking_cuts_list = tracking_cuts )

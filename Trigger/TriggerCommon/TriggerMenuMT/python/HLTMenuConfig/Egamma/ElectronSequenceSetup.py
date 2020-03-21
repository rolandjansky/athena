#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

import AthenaCommon.CfgMgr as CfgMgr
from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
# ATR-20453
# Until such time as FS and RoI collections do not interfere, a hacky fix
#from AthenaCommon.CFElements import parOR, seqAND
from AthenaCommon.CFElements import seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm, ViewCreatorInitialROITool
from TrigEDMConfig.TriggerEDMRun3 import recordable

def fastElectronSequence(ConfigFlags):
    """ second step:  tracking....."""
    
  
    from TrigInDetConfig.InDetSetup import makeInDetAlgs
    RoIs = "EMIDRoIs" # contract with the fastCalo
    viewAlgs = makeInDetAlgs(whichSignature = "Electron", separateTrackParticleCreator="_Electron", rois = RoIs)

    # A simple algorithm to confirm that data has been inherited from parent view
    # Required to satisfy data dependencies
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import CaloMenuDefs  
    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("electronViewDataVerifier")
    ViewVerify.DataObjects = [('xAOD::TrigEMClusterContainer','StoreGateSvc+'+ CaloMenuDefs.L2CaloClusters)]
    viewAlgs.append(ViewVerify)
    
    TrackParticlesName = ""
    for viewAlg in viewAlgs:
        if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
            TrackParticlesName = viewAlg.TrackParticlesName
      
    from TrigEgammaHypo.TrigL2ElectronFexMTConfig import L2ElectronFex_1
    theElectronFex= L2ElectronFex_1()
    theElectronFex.TrigEMClusterName = CaloMenuDefs.L2CaloClusters
    theElectronFex.TrackParticlesName = TrackParticlesName
    theElectronFex.ElectronsName=recordable("HLT_L2Electrons")

    # EVCreator:
    l2ElectronViewsMaker = EventViewCreatorAlgorithm("IMl2Electron")
    l2ElectronViewsMaker.RoIsLink = "initialRoI"
    l2ElectronViewsMaker.RoITool = ViewCreatorInitialROITool()
    l2ElectronViewsMaker.InViewRoIs = RoIs
    l2ElectronViewsMaker.Views = "EMElectronViews"
    l2ElectronViewsMaker.ViewFallThrough = True
    l2ElectronViewsMaker.RequireParentView = True

    theElectronFex.RoIs = l2ElectronViewsMaker.InViewRoIs
    # ATR-20453
    # Until such time as FS and RoI collections do not interfere, a hacky fix
    #electronInViewAlgs = parOR("electronInViewAlgs", viewAlgs + [ theElectronFex ])
    electronInViewAlgs = seqAND("electronInViewAlgs", viewAlgs + [ theElectronFex ])
    l2ElectronViewsMaker.ViewNodeName = "electronInViewAlgs"

    electronAthSequence = seqAND("electronAthSequence", [l2ElectronViewsMaker, electronInViewAlgs ] )
    return (electronAthSequence, l2ElectronViewsMaker, theElectronFex.ElectronsName)


def fastElectronMenuSequence():
    """ Creates 2nd step Electron  MENU sequence"""
    # retrievee the reco seuqence+IM
    (electronAthSequence, l2ElectronViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(fastElectronSequence, ConfigFlags)

    # make the Hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2ElectronHypoAlgMT
    theElectronHypo = TrigL2ElectronHypoAlgMT()
    theElectronHypo.Electrons = sequenceOut

    theElectronHypo.RunInView=True

    from TrigEgammaHypo.TrigL2ElectronHypoTool import TrigL2ElectronHypoToolFromDict

    return  MenuSequence( Maker       = l2ElectronViewsMaker,                                        
                          Sequence    = electronAthSequence,
                          Hypo        = theElectronHypo,
                          HypoToolGen = TrigL2ElectronHypoToolFromDict )


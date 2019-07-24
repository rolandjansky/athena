#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

import AthenaCommon.CfgMgr as CfgMgr
from AthenaConfiguration.AllConfigFlags import ConfigFlags

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from TrigEDMConfig.TriggerEDMRun3 import recordable

def fastElectronSequence(ConfigFlags):
    """ second step:  tracking....."""
    
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs
    (viewAlgs, eventAlgs) = makeInDetAlgs(whichSignature = "Electron", separateTrackParticleCreator="_Electron")


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
    l2ElectronViewsMaker = EventViewCreatorAlgorithm("l2ElectronViewsMaker")
    l2ElectronViewsMaker.RoIsLink = "roi" # -||-
    l2ElectronViewsMaker.InViewRoIs = "EMIDRoIs" # contract with the fastCalo
    l2ElectronViewsMaker.Views = "EMElectronViews"
    l2ElectronViewsMaker.ViewFallThrough = True
    l2ElectronViewsMaker.RequireParentView = True


    for viewAlg in viewAlgs:
        if "RoIs" in viewAlg.properties():
            viewAlg.RoIs = l2ElectronViewsMaker.InViewRoIs
        if "roiCollectionName" in viewAlg.properties():
            viewAlg.roiCollectionName = l2ElectronViewsMaker.InViewRoIs

    theElectronFex.RoIs = l2ElectronViewsMaker.InViewRoIs    
    electronInViewAlgs = parOR("electronInViewAlgs", viewAlgs + [ theElectronFex ])
    l2ElectronViewsMaker.ViewNodeName = "electronInViewAlgs"

    electronAthSequence = seqAND("electronAthSequence", eventAlgs + [l2ElectronViewsMaker, electronInViewAlgs ] )
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


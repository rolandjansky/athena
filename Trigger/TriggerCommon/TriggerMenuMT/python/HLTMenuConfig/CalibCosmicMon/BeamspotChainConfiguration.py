# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.CalibCosmicMon.BeamspotChainConfiguration")

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TrigStreamerHypo.TrigStreamerHypoConfigMT import StreamerHypoToolMTgenerator
from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlgMT
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from DecisionHandling.DecisionHandlingConf import InputMakerForRoI
from AthenaCommon.CFElements import seqAND


#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO, 
# so let's make them functions already now
#----------------------------------------------------------------

 

def allTE_trkfast_Cfg( flags ):
        return allTE_trkfast()

def allTE_trkfast():

        inputMakerAlg = InputMakerForRoI("IM_beamspotInputMaker", mergeOutputs=False)
        inputMakerAlg.RoIs="beamspotInputRoIs"
        from TrigInDetConfig.InDetSetup import makeInDetAlgs
        viewAlgs = makeInDetAlgs(whichSignature='FS', rois=inputMakerAlg.RoIs)
        from TrigT2BeamSpot.T2VertexBeamSpotConfig import T2VertexBeamSpot_activeAllTE
        T2VertexBeamSpot_activeAllTE.TrackCollections = ["TrigFastTrackFinder_Tracks_FS"]
        #beamspotSequence = seqAND("beamspotSequence",viewAlgs+[inputMakerAlg,T2VertexBeamSpot_activeAllTE()])
        beamspotSequence = seqAND("beamspotSequence",[inputMakerAlg]+viewAlgs+[T2VertexBeamSpot_activeAllTE()])

        #hypo
        beamspotHypoAlg = TrigStreamerHypoAlgMT("BeamspotHypoAlg")
        beamspotHypoAlg.RuntimeValidation = False #Needed to avoid the ERROR ! Decision has no 'feature' ElementLink
        beamspotHypoToolGen= StreamerHypoToolMTgenerator

        return  MenuSequence( Sequence    = beamspotSequence,
                          Maker       = inputMakerAlg,
                          Hypo        = beamspotHypoAlg,
                          HypoToolGen = beamspotHypoToolGen )

# Class to configure chain
#----------------------------------------------------------------
class BeamspotChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)


    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):                            
        chainSteps = []
        log.debug("Assembling chain for " + self.chainName)
        # --------------------
        # define here the names of the steps and obtain the chainStep configuration 
        # --------------------
        stepDictionary = {
            "allTE_trkfast":[self.getAllTEStep()]
            #"activeTE_trkfast":[self.activeTE_trkfast()],
            #"trkFS_trkfast":[self.trkFS_trkfast()],
        }

        #key = self.chainPart['EFrecoAlg']
        key = self.chainPart['addInfo'][0] + "_" + self.chainPart['l2IDAlg'][0]#TODO: hardcoded index
        steps=stepDictionary[key]
        for step in steps:
            chainSteps+=[step]
            
        myChain = self.buildChain(chainSteps)
        return myChain

   
    # --------------------
    # Configuration of costmonitor
    # --------------------
    def getAllTEStep(self):
        return self.getStep(1,"allTE_trkfast",[allTE_trkfast_Cfg])

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from TriggerMenuMT.HLT.Menu.ChainConfigurationBase import ChainConfigurationBase
from TrigStreamerHypo.TrigStreamerHypoConfig import StreamerHypoToolGenerator
from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlg
from TriggerMenuMT.HLT.Menu.MenuComponents import MenuSequence
from AthenaCommon.CFElements import seqAND, parOR
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorInitialROITool

#----------------------------------------------------------------

# fragments generating configuration will be functions in New JO,
# so let's make them functions already now
#----------------------------------------------------------------

def trkFS_trkfast_Cfg( flags ):
        return allTE_trkfast( signature="FS" )

def allTE_trkfast_Cfg( flags ):
        return allTE_trkfast( signature="BeamSpot" )

def allTE_trkfast( signature="FS" ):
        inputMakerAlg = EventViewCreatorAlgorithm("IM_beamspot_"+signature)
        inputMakerAlg.ViewFallThrough = True
        inputMakerAlg.RoIsLink = "initialRoI"
        inputMakerAlg.RoITool = ViewCreatorInitialROITool()
        inputMakerAlg.InViewRoIs = "beamspotViewRoI_"+signature
        inputMakerAlg.Views      = "beamspotViewRoI_"+signature

        from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTracking
        from TrigT2BeamSpot.T2VertexBeamSpotConfig import T2VertexBeamSpot_activeAllTE

        #Load signature configuration (containing cut values, names of collections, etc)
        from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
        IDTrigConfig = getInDetTrigConfig( signature )

        if(signature == "FS"):
            IDTrigConfig = getInDetTrigConfig("beamSpotFS")

        viewAlgs, viewVerify  = makeInDetTrigFastTracking( config = IDTrigConfig,  rois=inputMakerAlg.InViewRoIs)

        vertexAlg = T2VertexBeamSpot_activeAllTE( "vertex_"+signature )
        vertexAlg.TrackCollection = IDTrigConfig.trkTracks_FTF()

        viewVerify.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+beamspotViewRoI_'+signature ),
                                   ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                                   ( 'TagInfo' , 'DetectorStore+ProcessingTags' )]

        # Make sure this is still available at whole-event level
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence.SGInputLoader.Load += [( 'TagInfo' , 'DetectorStore+ProcessingTags' )]

        beamspotSequence = parOR( "beamspotSequence_"+signature, viewAlgs+[vertexAlg] )
        inputMakerAlg.ViewNodeName = beamspotSequence.name()
        beamspotViewsSequence = seqAND( "beamspotViewsSequence"+signature, [ inputMakerAlg, beamspotSequence ])


        #hypo
        beamspotHypoAlg = TrigStreamerHypoAlg("BeamspotHypoAlg_"+signature)
        beamspotHypoAlg.RuntimeValidation = False #Needed to avoid the ERROR ! Decision has no 'feature' ElementLink
        beamspotHypoToolGen= StreamerHypoToolGenerator


        return  MenuSequence( Sequence    = beamspotViewsSequence,
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
    def assembleChainImpl(self):
        chainSteps = []
        log.debug("Assembling chain for %s", self.chainName)

        stepDictionary = self.getStepDictionary()

        #key = self.chainPart['EFrecoAlg']
        key = self.chainPart['addInfo'][0] + "_" + self.chainPart['l2IDAlg'][0]#TODO: hardcoded index
        steps=stepDictionary[key]
        for step in steps:
            chainstep = getattr(self, step)()
            chainSteps+=[chainstep]

        myChain = self.buildChain(chainSteps)
        return myChain

    def getStepDictionary(self):
        # --------------------
        # define here the names of the steps and obtain the chainStep configuration
        # --------------------
        stepDictionary = {
            "allTE_trkfast":['getAllTEStep'],
            #"activeTE_trkfast":[self.activeTE_trkfast()],
            "trkFS_trkfast":['getTrkFSStep']
        }
        return stepDictionary

    # --------------------
    # Configuration of costmonitor (costmonitor ?? but isn't this is the actua chain configuration ??)
    # --------------------
    def getTrkFSStep(self):
        return self.getStep(1,"trkFS_trkfast",[trkFS_trkfast_Cfg])


    # --------------------
    # Configuration of costmonitor (costmonitor ?? but isn't this is the actua chain configuration ??)
    # --------------------
    def getAllTEStep(self):
        return self.getStep(1,"allTE_trkfast",[allTE_trkfast_Cfg])

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.UnconventionalTracking.UnconventionalTrackingDef")

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import seqAND
from ..CommonSequences.FullScanDefs import caloFSRoI

def unconventionalTrackingChainParts(chainParts):
    unconvtrkChainParts = []
    for p in chainParts:
        if p['trigType'] == 'unconvtrk':
            unconvtrkChainParts.append(p)
    return unconvtrkChainParts


#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class UnconventionalTrackingChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)

    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):                            
        log.debug("Assembling chain %s", self.chainName)

        chainSteps = []

        stepDictionary = self.getStepDictionary()

        key = self.chainPart['extra']

        steps=stepDictionary[key]

        for step_level in steps:
            for step in step_level:
                chainstep = getattr(self, step)()
                chainSteps+=[chainstep]
    
        myChain = self.buildChain(chainSteps)
        return myChain

    def getStepDictionary(self):


        # --------------------
        # define here the names of the steps and obtain the chainStep configuration
        # each value is a list [ L2, EF ] where L2 = [list of L2 steps] and EF = [ EF steps]
        # this way, Bphys (or others) can insert steps at the end of L2 and end of EF after
        # the muon steps are defined
        # note that bphys chains are by default noL2Comb, even though this is not in the name
        # --------------------

        stepDictionary = {
            "isohpttrack" : [['getIsoHPtTrackTrigger']]
        }

        return stepDictionary




        # --------------------
    def getIsoHPtTrackTrigger(self):
        return self.getStep(3,'IsoHPtTrackTrigger',[IsoHPtTrackTriggerCfg])





def IsoHPtTrackTriggerSequence(ConfigFlags):


    from TrigT2CaloCommon.CaloDef import clusterFSInputMaker
    InputMakerAlg= clusterFSInputMaker()        

   
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( 'jet' )




    from TrigEDMConfig.TriggerEDMRun3 import recordable
    from TrigInDetConfig.InDetSetup import makeInDetAlgsNoView
    TrkInputNoViewAlg = makeInDetAlgsNoView( config = IDTrigConfig, rois=caloFSRoI)

    

    from TrigInDetConfig.TrigInDetPriVtxConfig import makeVertices
    
    verticesname = recordable("HLT_IDVertex_FS")
    vtxAlgs = makeVertices( "jet", IDTrigConfig.FT.tracksFTF( doRecord = IDTrigConfig.isRecordable ) , verticesname, IDTrigConfig )
    prmVtx = vtxAlgs[-1]


    TrkSeq = seqAND("UncTrkrecoSeq", [InputMakerAlg,TrkInputNoViewAlg, prmVtx])
    sequenceOut = IDTrigConfig.FT.tracksFTF( doRecord = IDTrigConfig.isRecordable ) 


    # from .JetTrackingConfig import JetTrackingSequence
    # (jettrkseq, trkcolls) = RecoFragmentsPool.retrieve( JetTrackingSequence, configFlags, trkopt=jetRecoDict["ftf"], RoIs=RoIs)
    # recoSeq += jettrkseq



    return (TrkSeq, InputMakerAlg, sequenceOut)





def IsoHPtTrackTriggerMenuSequence():
        


        from TrigLongLivedParticlesHypo.TrigIsoHPtTrackTriggerHypoTool import TrigIsoHPtTrackTriggerHypoToolFromDict
        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import (TrigIsoHPtTrackTriggerHypoAlgMT)

        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        ( TrkSeq, InputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(IsoHPtTrackTriggerSequence,ConfigFlags)
        
        theIsoHPtTrackTriggerHypo = TrigIsoHPtTrackTriggerHypoAlgMT("L2IsoHPtTrack")
        theIsoHPtTrackTriggerHypo.trackKey =  sequenceOut



        log.info("Building the Step dictinary for IsoHPt!")
        return MenuSequence( Sequence    = TrkSeq,
                            Maker       = InputMakerAlg,
                            Hypo        = theIsoHPtTrackTriggerHypo,
                            HypoToolGen = TrigIsoHPtTrackTriggerHypoToolFromDict, 
                            )

def IsoHPtTrackTriggerCfg(flags):
    return IsoHPtTrackTriggerMenuSequence()

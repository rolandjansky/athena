
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import copy
from AthenaCommon.Logging import logging
log = logging.getLogger('MenuComponents')

from DecisionHandling.DecisionHandlingConf import RoRSeqFilter
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponentsNaming import CFNaming
from AthenaCommon.CFElements import parOR, seqAND


class Node(object):
    """base class representing one Alg + inputs + outputs, to be used to Draw dot diagrams and connect objects"""
    def __init__(self, Alg):
        self.name = ("%sNode")%( Alg.name())
        self.Alg=Alg
        self.inputs=[]
        self.outputs=[]

    def addOutput(self, name):
        self.outputs.append(name)

    def addInput(self, name):
        self.inputs.append(name)

    def getOutputList(self):
        return self.outputs

    def getInputList(self):
        return self.inputs

    def __repr__(self):
        return "Node::%s  [%s] -> [%s]"%(self.Alg.name(), ' '.join(map(str, self.getInputList())), ' '.join(map(str, self.getOutputList())))



class AlgNode(Node):
    """Node class that connects inputs and outputs to basic alg. properties """
    def __init__(self, Alg, inputProp, outputProp):
        Node.__init__(self, Alg)
        self.outputProp=outputProp
        self.inputProp=inputProp

    def addDefaultOutput(self):
        if self.outputProp != '':
            self.addOutput(("%s_%s"%(self.Alg.name(),self.outputProp)))

    def setPar(self, prop, name):
        cval = self.Alg.getProperties()[prop]
        if type(cval) is list:
            cval.append(name)
            return setattr(self.Alg, prop, cval)
        else: 
            return setattr(self.Alg, prop, name)

    def resetPar(self, prop):
        cval = self.Alg.getProperties()[prop]
        if type(cval) is list:
            return setattr(self.Alg, prop, [])
        else: 
            return setattr(self.Alg, prop, "")

    def getPar(self, prop):
        if hasattr(self.Alg, prop):
            return getattr(self.Alg, prop)
        else:
            return self.Alg.getDefaultProperty(prop)


    def resetOutput(self):
        self.resetPar(self.outputProp)

    def resetInput(self):
        self.resetPar(self.inputProp)
        
    def addOutput(self, name):
        outputs = self.readOutputList()
        if name in outputs:
            log.debug("Warning, output %s already set in %s, DH not added",name, self.name)
        else:
            if self.outputProp != '':
                self.setPar(self.outputProp,name)
            else:
                log.error("no OutputProp set")
        Node.addOutput(self, name)


    def readOutputList(self):
        outputs = []
        cval = self.getPar(self.outputProp)
        if cval == '':
            return outputs
        if type(cval) is list:
            outputs.extend(cval)
        else:
            outputs.append(cval)
        return outputs

    def addInput(self, name):
        inputs = self.readInputList()
        if name in inputs:
            log.debug("Warning, input %s already set in %s, DH not added",name, self.name)
        else:
            if self.inputProp != '':
                self.setPar(self.inputProp,name)
            else:
                log.error("no InputProp set")
        Node.addInput(self, name)


    def readInputList(self):
        inputs = []
        cval = self.getPar(self.inputProp)
        if cval =='':
            return inputs
        if type(cval) is list:
            inputs.extend(cval)
        else:
            inputs.append(cval)
        return inputs

    def __repr__(self):
        return "Alg::%s  [%s] -> [%s]"%(self.Alg.name(), ' '.join(map(str, self.getInputList())), ' '.join(map(str, self.getOutputList())))


class HypoToolConf(object):
    """ Class to group info on hypotools"""
    def __init__(self, hypoToolGen):
        self.hypoToolGen = hypoToolGen
        self.name=hypoToolGen.__name__ if hypoToolGen else "None"
        self.conf=''

    def setConf( self, chainDict):
        if type(chainDict) is not dict:
            raise RuntimeError("Configuring hypo with %s, not good anymore, use chainDict" % str(chainDict) )
        self.chainDict = chainDict

    # def setName(self, name):
    #     self.name=name

    # def setConf(self, conf):
    #     self.conf=conf

    def create(self):
        """creates instance of the hypo tool"""
        return self.hypoToolGen( self.chainDict )


class HypoAlgNode(AlgNode):
    """Node for HypoAlgs"""
    def __init__(self, Alg):
        assert isHypoBase(Alg), "Error in creating HypoAlgNode from Alg "  + Alg.name()
        AlgNode.__init__(self, Alg, 'HypoInputDecisions', 'HypoOutputDecisions')
        self.tools = []
        self.previous=[]

    def addHypoTool (self, hypoToolConf):
        if hypoToolConf.chainDict['chainName'] not in self.tools:
            ## HypoTools are private, so need to be created when added to the Alg
            ## this incantation may seem strange, however it is the only one which works
            ## trying tool = hypoToolConf.create() and then assignement does not work! will be no problem in run3 config
            tools = self.Alg.HypoTools
            self.Alg.HypoTools = tools+[hypoToolConf.create()]
            self.tools.append( self.Alg.HypoTools[-1].getName() ) # should not be needed anymore
        else:
            raise RuntimeError("The hypo tool of name "+ hypoToolConf.chainDict['chainName'] +" already present")


    def setPreviousDecision(self,prev):
        self.previous.append(prev)
        return self.addInput(prev)

    def resetDecisions(self):
        self.previous = []
        self.resetOutput()
        self.resetInput()

    def __repr__(self):
        return "HypoAlg::%s  [%s] -> [%s], previous = [%s], HypoTools=[%s]"%(self.Alg.name(),' '.join(map(str, self.getInputList())),
                                                                                 ' '.join(map(str, self.getOutputList())),
                                                                                 ' '.join(map(str, self.previous)),
                                                                                 ' '.join(map(str, self.tools)))



class SequenceFilterNode(AlgNode):
    """Node for any kind of sequence filter"""
    def __init__(self, Alg, inputProp, outputProp):
        AlgNode.__init__(self,  Alg, inputProp, outputProp)

    def setChains(self, name):
        return self.setPar("Chains", name)

    def getChains(self):
        return self.getPar("Chains")

    def __repr__(self):
        return "SequenceFilter::%s  [%s] -> [%s], chains=%s"%(self.Alg.name(),' '.join(map(str, self.getInputList())),' '.join(map(str, self.getOutputList())), self.getChains())


class RoRSequenceFilterNode(SequenceFilterNode):
    def __init__(self, name):
        Alg= RoRSeqFilter(name)
        SequenceFilterNode.__init__(self,  Alg, 'Input', 'Output')



class InputMakerNode(AlgNode):
    def __init__(self, Alg):
        assert isInputMakerBase(Alg), "Error in creating InputMakerNode from Alg "  + Alg.name()
        AlgNode.__init__(self,  Alg, 'InputMakerInputDecisions', 'InputMakerOutputDecisions')


from DecisionHandling.DecisionHandlingConf import ComboHypo
class ComboMaker(AlgNode):
    def __init__(self, name):
        Alg = ComboHypo(name)
        log.debug("Making combo Alg %s", name)
        AlgNode.__init__(self,  Alg, 'HypoInputDecisions', 'HypoOutputDecisions')
        self.prop="MultiplicitiesMap"


    def addChain(self, chain):
        log.debug("ComboMaker %s adding chain %s", self.Alg.name(),chain)
        from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName
        dictDecoding = DictFromChainName.DictFromChainName()
        allMultis = [int(mult) for mult in dictDecoding.getChainMultFromName(chain)]       
       
        newdict = {chain : allMultis}

        cval = self.Alg.getProperties()[self.prop]  # check necessary to see if chain was added already?
        if type(cval) is dict:
            if chain in cval.keys():
                log.error("ERROR in cofiguration: ComboAlg %s has already been configured for chain %s", self.Alg.name(), chain)
            else:
                cval[chain]=allMultis
        else:
            cval=newdict

        setattr(self.Alg, self.prop, cval)
        log.debug("ComboAlg %s has now these chains chain %s", self.Alg.name(), self.getPar(self.prop))


    def getChains(self):
        cval = self.Alg.getProperties()[self.prop]
        return cval
        


#########################################################
# USEFULL TOOLS
#########################################################

def isHypoBase(alg):
    if  'HypoInputDecisions'  in alg.__class__.__dict__:
        return True
    prop = alg.__class__.__dict__.get('_properties')
    return  ('HypoInputDecisions'  in prop)

def isInputMakerBase(alg):
    return  ('InputMakerInputDecisions'  in alg.__class__.__dict__)

def isFilterAlg(alg):
    return isinstance(alg, RoRSeqFilter)



##########################################################
# NOW sequences and chains
##########################################################

class MenuSequence(object):
    """ Class to group reco sequences with the Hypo"""
    def __init__(self, Sequence, Maker,  Hypo, HypoToolGen, CA=None ):
        assert Maker.name().startswith("IM"), "The input maker {} name needs to start with letter: IM".format(Maker.name())
        self.name = CFNaming.menuSequenceName(Hypo.name())
        self.sequence     = Node( Alg=Sequence)
        self._maker       = InputMakerNode( Alg = Maker )
        self.hypoToolConf = HypoToolConf( HypoToolGen )
        self._hypo        = HypoAlgNode( Alg = Hypo )
        self.inputs=[]
        self.outputs=[]
        self.seed=''
        self.reuse = False # flag to draw dot diagrmas
        self.ca = CA



    def replaceHypoForCombo(self, HypoAlg):
        log.debug("set new Hypo %s for combo sequence %s ", HypoAlg.name(), self.name)
        self._hypo = HypoAlgNode( Alg=HypoAlg )

    def replaceHypoForDuplication(self, HypoAlg):
        log.debug("set new Hypo %s for duplicated sequence %s, resetting decisions ", HypoAlg.name(), self.name)        
        self._hypo = HypoAlgNode( Alg=HypoAlg )
        # do we need this?
        #       self._hypo.resetInput()
        #       self._hypo.resetOutput()

    def resetConnections(self):
        self.inputs=[]
        self.outputs=[]
        
    @property
    def maker(self):
        if self.ca is not None:
            makerAlg = self.ca.getEventAlgo(self._maker.Alg.name())
            self._maker.Alg = makerAlg
            # return InputMakerNode(Alg=makerAlg)
        return self._maker

    @property
    def hypo(self):
        if self.ca is not None:
            hypoAlg = self.ca.getEventAlgo(self._hypo.Alg.name())
            self._hypo.Alg = hypoAlg
            # return HypoAlgNode(Alg=hypoAlg)
        return self._hypo

    def connectToFilter(self, outfilter):
        """ Sets the input and output of the hypo, and links to the input maker """

        #### Connect filter to the InputMaker
        self.maker.addInput(outfilter)
        input_maker_output = CFNaming.inputMakerOutName(self.maker.Alg.name(),outfilter)
        self.maker.addOutput(input_maker_output)

        #### Add input/output Decision to Hypo
        self.hypo.setPreviousDecision( input_maker_output)
        hypo_output = CFNaming.hypoAlgOutName(self.hypo.Alg.name(), input_maker_output)
        if len(self.hypo.getOutputList()):
            log.error("Hypo " + self.hypo.name() +" has already an output configured: you may want to duplicate the Hypo!")
        self.hypo.addOutput(hypo_output)

        # needed for drawing
        self.inputs.append(outfilter)
        self.outputs.append(hypo_output)


        log.debug("MenuSequence.connectToFilter: connecting InputMaker and HypoAlg, adding: \n\
        InputMaker::%s.output=%s, \n\
        HypoAlg::%s.previousDecision=%s, \n\
        HypoAlg::%s.output=%s",\
                self.maker.Alg.name(), input_maker_output, self.hypo.Alg.name(), input_maker_output, self.hypo.Alg.name(), hypo_output )

    def __repr__(self):
        hypotool = self.hypoToolConf.name 
        return "MenuSequence::%s \n Hypo::%s \n Maker::%s \n Sequence::%s \n HypoTool::%s\n"\
        %(self.name, self.hypo.Alg.name(), self.maker.Alg.name(), self.sequence.Alg.name(), hypotool)


#################################################
#### CONFIGURATION FOR L1DECODER
#################################################
## It might be moved somewhere in the cofiguration later one
# This is map between the L1 items and the name of teh Decisions in the L1Decoder unpacking tools
# to be moved to L1Decoder package
def DoMapSeedToL1Decoder(seed):
    mapSeedToL1Decoder = {  "EM" : "L1EM",
                            "MU" : "L1MU",
                            "J"  : "L1J",
                            "TAU": "L1TAU",
                            "XE" : "L1MET",
                            "XS" : "L1MET",
                            "TE" : "L1MET"}

    # remove actual threshold value from L1 seed string
    for thresholdType, l1Collection in mapSeedToL1Decoder.iteritems():
        if seed.startswith( thresholdType ):
            return l1Collection
    log.error("Threshold "+ seed + " not mapped to any Decision objects! Available are: " + str(mapSeedToL1Decoder.values()))


#################################################

#from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import getAllThresholdsFromItem, getUniqueThresholdsFromItem

class Chain(object):
    """Basic class to define the trigger menu """
    __slots__='name','steps','vseeds','group_seed'
    def __init__(self, name, ChainSteps, L1Thresholds):
        """
        Construct the Chain from the steps
        Out of all arguments the ChainSteps & L1Thresholds are most relevant, the chain name is used in debug messages
        """
        self.name = name
        self.steps=ChainSteps
        self.vseeds=L1Thresholds

     

        # group_seed is used to set the seed type (EM, MU,JET), removing the actual threshold
        # in practice it is the L1Decoder Decision output
        self.group_seed = [DoMapSeedToL1Decoder(stri) for stri in self.vseeds]
        self.setSeedsToSequences() # save seed of each menuseq

        isCombo=False
        for step in self.steps:
            if step.isCombo:
                isCombo=True

        log.debug("Made %s Chain %s with seeds: %s ", "combo" if isCombo else "", name, self.vseeds)
                
    def setSeedsToSequences(self):
        """ Set the L1 seeds to the menu sequences """

        # check if the number of seeds is enought for all the seuqences:
        max_seq= max(len(step.sequences) for step in self.steps)            
        tot_seed=len(self.vseeds)
        if max_seq==tot_seed:
            for step in self.steps:
                if len(step.sequences) == 0:
                    continue
                if len(step.sequences) != tot_seed:
                    log.warning ("setSeedsToSequences: Chain %s, step %s has %d sequences but %d seeds ", self.name, step.name, len(step.sequences), tot_seed)
                    continue
                nseq=0
                for seed in self.vseeds:
                    seq=step.sequences[nseq]
                    seq.seed ="L1"+filter(lambda x: x.isalpha(), seed)
                    log.debug( "setSeedsToSequences: Chain %s adding seed %s to sequence %d in step %s", self.name, seq.seed, nseq, step.name )
                    nseq+=1
        else:
            log.error("setSeedsToSequences: found %d sequences in the chain %s and %d seeds %s. What to do??", max_seq, self.name, tot_seed, str(self.vseeds))


            
    def decodeHypoToolConfs(self, allChainDicts):
        """ This is extrapolating the hypotool configuration from the (combined) chain name"""
        import copy
        from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import getChainDictFromChainName
        chainDict = getChainDictFromChainName(self.name, allChainDicts)

        for step in self.steps:
            if len(step.sequences) == 0:
                continue

            if len(chainDict['chainParts']) != len(step.sequences):              
                log.error("Error in step %s for chain %s: found %d chain parts and %d sequences", step.name, self.name, len(chainDict['chainParts']), len(step.sequences))
                log.error(chainDict['chainParts'])

            for seq, chainDictPart in zip(step.sequences, chainDict['chainParts']):
                if seq.ca is not None: # The CA merging took care of everything
                    continue

                onePartChainDict = copy.deepcopy( chainDict )
                onePartChainDict['chainParts'] = [ chainDictPart ]

                seq.hypoToolConf.setConf( onePartChainDict )
                seq.hypo.addHypoTool(seq.hypoToolConf) #this creates the HypoTools

                
    def __repr__(self):
        return "--- Chain %s --- \n + Seeds: %s \n + Steps: \n %s \n"%(\
                    self.name, ' '.join(map(str, self.vseeds)), '\n '.join(map(str, self.steps)))



class CFSequence(object):
    """Class to describe the ChainStep + filter 
    One Filter can be included in many CF sequences, so the CF sequence is identified by the step name and the connections
    """
    def __init__(self, ChainStep, FilterAlg, connections):

        self.filter = FilterAlg
        self.step = ChainStep
        self.connections=connections
        log.debug("CFSequence: creating new one with Filter %s, step %s and %d connections: %s",self.filter.Alg.name(), self.step.name, len(self.connections), self.connections)
        self.connect()
        self.setDecisions()

    def setDecisions(self):
        self.decisions=[]
        for sequence in self.step.sequences:                
            self.decisions.extend(sequence.outputs)
        if not len(self.step.sequences):
            self.decisions.extend(self.filter.getOutputList())

       
    def connect(self):
        """Connect filter to ChainStep (all its sequences)
        if a ChainStep contains the same sequence multiple times (for multi-object chains),
        the filter is connected only once (to avoid multiple DH links)
        """
        log.debug("CFSequence: Connect Filter %s with %d menuSequences of step %s", self.filter.Alg.name(), len(self.step.sequences), self.step.name)
        if len(self.connections) == 0:
            log.error("ERROR, no filter outputs are set!")
            #raise("CFSequence: Invalid Filter Configuration")
       
        if len(self.step.sequences):
            # check whether the number of filter outputs are the same as the number of sequences in the step
            if len(self.connections) != len(self.step.sequences):
                log.error("Found %d filter outputs and %d MenuSequences in Step %s", len(self.filter.getOutputList()), len(self.step.sequences), self.step.name)
                #raise("CFSequence: Invalid Filter Configuration")
            nseq=0
            for seq in self.step.sequences:
                filter_out = self.connections[nseq]
                log.debug("CFSequence: Found input %s to sequence::%s from Filter::%s (from seed %s)", filter_out, seq.name, self.filter.Alg.name(), seq.seed)
                seq.connectToFilter( filter_out )
                nseq+=1
        
            if self.step.isCombo:
                self.connectCombo()
            
        else:
          log.debug("This CFSequence has no sequences: outputs are the Filter outputs")




    def connectCombo(self):
        """ reset sequence outputs, they will be replaced by new combo outputs"""
        
        for seq in self.step.sequences:
            seq.outputs=[]

        for seq in self.step.sequences:
            combo_input=seq.hypo.getOutputList()[0]
            self.step.combo.addInput(combo_input)
            log.debug("Adding inputs %s to combo %s", combo_input, self.step.combo.Alg.name())
            # inputs are the output decisions of the hypos of the sequences
            # outputs are the modified name of input deciions that need to be connected to the next filter
            combo_output=CFNaming.comboHypoOutputName (combo_input)
            self.step.combo.addOutput(combo_output)
            seq.outputs.append(combo_output)
            log.debug("CFSequence: connectCombo: Adding outputs %s to combo %s", combo_output, self.step.combo.Alg.name())


    def __repr__(self):
        return "--- CFSequence ---\n + Filter: %s \n +  %s \n + decisions: %s\n"%(\
                    self.filter.Alg.name(), self.step, self.decisions)



class ChainStep(object):
    """Class to describe one step of a chain; if multiplicity is greater than 1, the step is combo/combined"""
    def __init__(self, name,  Sequences=[], multiplicity=1):
        self.name = name
        self.sequences=[]
        self.multiplicity = multiplicity
        self.isCombo=multiplicity>1       
        self.combo=None
        if self.isCombo:
            self.makeCombo(Sequences)
        else:
            self.sequences = Sequences


    def makeCombo(self, Sequences):
        if len(Sequences)==0:
            return
        
        # For combo sequences, duplicate the sequence, the Hypo with differnt names and create the ComboHypoAlg
        self.combo = ComboMaker(CFNaming.comboHypoName(self.name))
        duplicatedHypos = []
        for sequence in Sequences:
            oldhypo=sequence.hypo.Alg            
            duplicatedHypos.append(oldhypo.name())
            ncopy=duplicatedHypos.count(oldhypo.name())

            new_sequence=copy.deepcopy(sequence)
            new_sequence.name = CFNaming.comboSequenceCopyName(sequence.name,ncopy, self.name)

            newHypoAlgName = CFNaming.comboHypoCopyName(oldhypo.name(),ncopy, self.name)
            new_hypoAlg=oldhypo.clone(newHypoAlgName) # need to reset decisions?
            new_sequence.replaceHypoForCombo(new_hypoAlg)
            self.sequences.append(new_sequence)

    def __repr__(self):
        return "--- ChainStep %s ---\n + isCombo: %d, multiplicity= %d \n +  %s \n "%(self.name, self.isCombo,self.multiplicity, ' '.join(map(str, self.sequences) ))
    



# this is fragment for New JO


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
class InEventReco( ComponentAccumulator ):
    """ Class to handle in-event reco """
    def __init__(self, name, inputMaker=None):
        super( InEventReco, self ).__init__()
        self.name = name
        from AthenaCommon.CFElements import parOR, seqAND
        self.mainSeq = seqAND( name )
        self.addSequence( self.mainSeq )

        # Details below to be checked
        self.inputMakerAlg = inputMaker

        # Avoid registering a duplicate
        self.addEventAlgo( self.inputMakerAlg, self.mainSeq.name() )
        self.recoSeq = parOR( "InputSeq_"+self.inputMakerAlg.name())
        self.addSequence( self.recoSeq, self.mainSeq.name() )
    pass

    def mergeReco( self, ca ):
        """ Merged CA movnig reconstruction algorithms into the right sequence """
        return self.merge( ca, sequenceName=self.recoSeq.getName() )

    def addRecoAlg( self, alg ):
        """Reconstruction alg to be run per event"""
        log.warning( "InViewReco.addRecoAlgo: consider using mergeReco that takes care of the CA accumulation and moving algorithms" )
        self.addEventAlgo( alg, self.recoSeq.name() )

    def addHypoAlg(self, alg):
        self.addEventAlgo( alg, self.mainSeq.name() )

    def sequence( self ):
        return self.mainSeq

    def inputMaker( self ):
        return self.inputMakerAlg



class InViewReco( ComponentAccumulator ):
    """ Class to handle in-view reco, sets up the View maker if not provided and exposes InputMaker so that more inputs to it can be added in the process of assembling the menu """
    def __init__(self, name, viewMaker=None):
        super( InViewReco, self ).__init__()
        self.name = name
        from AthenaCommon.CFElements import parOR, seqAND
        self.mainSeq = seqAND( name )
        self.addSequence( self.mainSeq )

        from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
        if viewMaker:
            self.viewMakerAlg = viewMaker
        else:
            self.viewMakerAlg = EventViewCreatorAlgorithm("IM"+name,
                                                          ViewFallThrough = True,
                                                          RoIsLink        = 'initialRoI', # -||-
                                                          InViewRoIs      = name+'RoIs',
                                                          Views           = name+'Views',
                                                          ViewNodeName    = name+"InView")

        self.addEventAlgo( self.viewMakerAlg, self.mainSeq.name() )
        self.viewsSeq = parOR( self.viewMakerAlg.ViewNodeName )
        self.addSequence( self.viewsSeq, self.mainSeq.name() )

    def addInputFromFilter(self, filterAlg ):
        assert len(filterAlg.Output) == 1, "Can only oprate on filter algs with one configured output, use addInput to setup specific inputs"
        self.addInput( filterAlg.Output[0], "Reco_"+( filterAlg.Output[0].replace("Filtered_", "") ) )

    def addInput(self, inKey, outKey ):
        """Adds input (DecisionsContainer) from which the views should be created """
        self.viewMakerAlg.InputMakerInputDecisions += [ inKey ]
        self.viewMakerAlg.InputMakerOutputDecisions += [ outKey ]

    def mergeReco( self, ca ):
        """ Merged CA movnig reconstruction algorithms into the right sequence """
        return self.merge( ca, sequenceName=self.viewsSeq.getName() )

    def addRecoAlg( self, alg ):
        """Reconstruction alg to be run per view"""
        log.warning( "InViewReco.addRecoAlgo: consider using mergeReco that takes care of the CA accumulation and moving algorithms" )
        self.addEventAlgo( alg, self.viewsSeq.name() )

    def addHypoAlg(self, alg):
        self.addEventAlgo( alg, self.mainSeq.name() )

    def sequence( self ):
        return self.mainSeq

    def inputMaker( self ):
        return self.viewMakerAlg

#
class RecoFragmentsPool(object):
    """ Class to host all the reco fragments that need to be reused """
    fragments = {}
    @classmethod
    def retrieve( cls,  creator, flags, **kwargs ):
        """ create, or return created earlier reco fragment

        Reco fragment is uniquelly identified by the function and set og **kwargs.
        The flags are not part of unique identifier as creation of new reco fragments should not be caused by difference in the unrelated flags.
        TODO, if that code survives migration to New JO we need to handle the case when the creator is an inner function
        """
        requestHash = hash( ( creator, tuple(kwargs.keys()), tuple(kwargs.values()) ) )
        if requestHash not in cls.fragments:
            recoFragment = creator( flags, **kwargs )
            cls.fragments[requestHash] = recoFragment
            log.debug( "created reconstruction fragment using function: %s", creator.func_name )
            return recoFragment
        else:
            log.debug( "reconstruction fragment that would be created from %s is taken from the cache", creator.func_name )
            return cls.fragments[requestHash]


def getChainStepName(chainName, stepNumber):
    return '{}_step{}'.format(chainName, stepNumber)

def createStepView(stepName):
    stepReco = parOR(CFNaming.stepRecoName(stepName))
    stepView = seqAND(CFNaming.stepViewName(stepName), [stepReco])
    return stepReco, stepView

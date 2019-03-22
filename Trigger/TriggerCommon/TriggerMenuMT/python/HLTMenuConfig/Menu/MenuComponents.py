# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import sys, re, copy
from AthenaCommon.Logging import logging
from AthenaCommon.Constants import VERBOSE,DEBUG
log = logging.getLogger('MenuComponents')
log.setLevel( VERBOSE )
logLevel=DEBUG

from DecisionHandling.DecisionHandlingConf import RoRSeqFilter
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponentsNaming import CFNaming


class Node():
    """ base class representing one Alg + inputs + outputs, to be used to Draw dot diagrams and connect objects"""
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

    def __str__(self):
        return "Node::%s  [%s] -> [%s]"%(self.Alg.name(), ' '.join(map(str, self.getInputList())), ' '.join(map(str, self.getOutputList())))



class AlgNode(Node):
    """Node class that connects inputs and outputs to basic alg. properties """
    def __init__(self, Alg, inputProp, outputProp):
        Node.__init__(self, Alg)
        self.outputProp=outputProp
        self.inputProp=inputProp

    def addDefaultOutput(self):
        if self.outputProp is not '':
            self.addOutput(("%s_%s"%(self.Alg.name(),self.outputProp)))

    def setPar(self, prop, name):
        cval = self.Alg.getProperties()[prop]
        if type(cval) is list:
            cval.append(name)
            return setattr(self.Alg, prop, cval)
        else: 
            return setattr(self.Alg, prop, name)


    def getPar(self, prop):
        if hasattr(self.Alg, prop):
            return getattr(self.Alg, prop)
        else:
            return self.Alg.getDefaultProperty(prop)


    def addOutput(self, name):
        outputs = self.readOutputList()
        if name in outputs:
            log.debug("Warning, %s already set in %s, DH not added",name, self.name)
        else:
            if self.outputProp is not '':
                self.setPar(self.outputProp,name)
            else:
                sys.exit("no OutputProp set")
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
            log.debug("Warning, %s already set in %s, DH not added",name, self.name)
        else:
            if self.inputProp is not '':
                self.setPar(self.inputProp,name)
            else:
                sys.exit("no InputProp set")
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

    def __str__(self):
        return "Alg::%s  [%s] -> [%s]"%(self.Alg.name(), ' '.join(map(str, self.getInputList())), ' '.join(map(str, self.getOutputList())))


class HypoToolConf():
    """ Class to group info on hypotools"""
    def __init__(self, hypoToolGen):
        self.hypoToolGen = hypoToolGen
        self.name=''
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
        print "here", hypoToolConf.chainDict
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

    def __str__(self):
        return "HypoAlg::%s  [%s] -> [%s], previous = [%s], HypoTools=[%s]"%(self.Alg.name(),' '.join(map(str, self.getInputList())),
                                                                                 ' '.join(map(str, self.getOutputList())),
                                                                                 ' '.join(map(str, self.previous)),
                                                                                 ' '.join(map(str, self.tools)))



class SequenceFilterNode(AlgNode):
    """Node for any kind of sequence filter"""
    def __init__(self, Alg, inputProp, outputProp):
        AlgNode.__init__(self,  Alg, inputProp, outputProp)

    def setChains(self, name):
        log.debug("Adding Chain %s to filter %s"%(name, self.Alg.name()))
        return self.setPar("Chains", name)

    def getChains(self):
        return self.getPar("Chains")

    def __str__(self):
        return "SequenceFilter::%s  [%s] -> [%s], chains=%s"%(self.Alg.name(),' '.join(map(str, self.getInputList())),' '.join(map(str, self.getOutputList())), self.getChains())


class RoRSequenceFilterNode(SequenceFilterNode):
    def __init__(self, name):
        Alg= RoRSeqFilter(name, OutputLevel = logLevel)
        SequenceFilterNode.__init__(self,  Alg, 'Input', 'Output')



class InputMakerNode(AlgNode):
    def __init__(self, Alg):
        assert isInputMakerBase(Alg), "Error in creating InputMakerNode from Alg "  + Alg.name()
        AlgNode.__init__(self,  Alg, 'InputMakerInputDecisions', 'InputMakerOutputDecisions')
        Alg.OutputLevel = logLevel


from DecisionHandling.DecisionHandlingConf import ComboHypo
class ComboMaker(AlgNode):
    def __init__(self, name):
        Alg = ComboHypo(name)
        log.debug("Making combo Alg %s"%name)
        AlgNode.__init__(self,  Alg, 'HypoInputDecisions', 'HypoOutputDecisions')
        Alg.OutputLevel = logLevel
        self.prop="MultiplicitiesMap"


    def addChain(self, chain):
        log.debug("ComboMaker %s adding chain %s"%(self.Alg.name(),chain))
        from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName
        dictDecoding = DictFromChainName.DictFromChainName()
        allMultis = dictDecoding.getChainMultFromName(chain)
        print "chain ", chain
        print "WOOF allMultis", allMultis
        newdict = {chain : allMultis}

        for i in range(1, len(allMultis)):
            cval = self.Alg.getProperties()[self.prop]  # check necessary to see if chain was added already?
            if type(cval) is dict:
                ##cval[chain] = allMultis
                if chain in cval.keys():
                    cval[chain].append(allMultis[i])
                else:
                    cval[chain]=[allMultis[i]]
            else:
                cval=newdict
            setattr(self.Alg, self.prop, cval)



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


class MenuSequence():
    """ Class to group reco sequences with the Hypo"""
    def __init__(self, Sequence, Maker,  Hypo, HypoToolGen, CA=None ):
        self.name = CFNaming.menuSequenceName(Hypo.name())
        self.sequence     = Node( Alg=Sequence)
        self.maker        = InputMakerNode( Alg = Maker )
        self.hypoToolConf = HypoToolConf( HypoToolGen ) if HypoToolGen else None
        self.hypo         = HypoAlgNode( Alg = Hypo )
        self.inputs=[]
        self.outputs=[]
        self.seed=''
        self.reuse = False # flag to draw dot diagrmas
        self.ca = CA

    def replaceHypoForCombo(self, HypoAlg):
        log.debug("set new Hypo %s for combo sequence %s "%(HypoAlg.name(), self.name))
        self.hypo= HypoAlgNode( Alg=HypoAlg )

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
            sys.exit("ERROR, in chain configuration")
        self.hypo.addOutput(hypo_output)

        # needed for drawing
        self.inputs.append(outfilter)
        self.outputs.append(hypo_output)


        log.debug("MenuSequence.connectToFilter: connecting InputMaker and HypoAlg, adding: \n\
        InputMaker::%s.output=%s, \n\
        HypoAlg::%s.previousDecision=%s, \n\
        HypoAlg::%s.output=%s",\
                self.maker.Alg.name(), input_maker_output, self.hypo.Alg.name(), input_maker_output, self.hypo.Alg.name(), hypo_output )

    def __str__(self):
        return "MenuSequence::%s \n Hypo::%s \n Maker::%s \n Sequence::%s"%(self.name, self.hypo, self.maker, self.sequence)


#################################################
#### CONFIGURATION FOR L1DECODER
#################################################
## It might be moved somewhere in the cofiguration later one
# This is amp between the L1 items and the name of teh Decisions in the L1Decoder unpacking tools
def DoMapSeedToL1Decoder(seed):
    mapSeedToL1Decoder = {  "EM" : "L1EM",
                            "MU" : "L1MU",
                            "J"  : "L1J",
                            "TAU": "L1TAU",
                            "XE" : "L1MET",
                            "XS" : "L1MET",
                            "TE" : "L1MET"}

    # remove actual threshold value from L1 seed string
    stripSeed  = filter(lambda x: x.isalpha(), seed)
    if stripSeed not in mapSeedToL1Decoder:
        log.error("Seed "+ seed + " not mapped to any Decision objects! Available are: " + str(mapSeedToL1Decoder.values()))
        sys.exit("ERROR, in chain configuration")
    return (mapSeedToL1Decoder[stripSeed])

#################################################

class Chain:
    """Basic class to define the trigger menu """
    def __init__(self, name, Seed, ChainSteps=[]):
        self.name = name
        self.steps=ChainSteps
        self.seed=Seed
        self.vseeds=[]
        vseeds = Seed.strip().split("_")
        vseeds.pop(0) #remove first L1 string
        # split multi seeds
        for seed in vseeds:
            split=re.findall(r"(\d+)?([A-Z]+\d+)", seed)
            mult,single = split[0]
            if not mult: 
                mult=1
            else: 
                mult=int(mult)
#            print mult,single
            for m in range(0,mult): 
                self.vseeds.append(single)

        # group_seed is used to se tthe seed type (EM, MU,JET), removing the actual threshold
        # in practice it is the L1Decoder Decision output
        self.group_seed = [DoMapSeedToL1Decoder(stri) for stri in self.vseeds]
        self.setSeedsToSequences() # save seed of each menuseq
        log.debug("Chain " + name + " with seeds: %s "%str( self.vseeds))

        for step in self.steps:
            if step.isCombo:
                step.combo.addChain(self.name)

    def setSeedsToSequences(self):
        # set the seed to the menusequences
        sequences1=self.steps[0].sequences
        tot_seq=len(sequences1)
        tot_seed=len(self.vseeds)
        if tot_seq==tot_seed:
            nseq=0
            for seed in self.vseeds:
                for step in self.steps:
                    seq=step.sequences[nseq]
                    seq.seed ="L1"+filter(lambda x: x.isalpha(), seed)
                    log.debug( "Chain %s adding seed %s to sequence %d in step %s"%(self.name, seq.seed, nseq, step.name))
                nseq+=1

        else:
            log.error("found %d sequences in this chain and %d seeds. What to do??", tot_seq, tot_seed)
            sys.exit("ERROR, in chain configuration")

    def decodeHypoToolConfs(self, allChainDicts):
        """ This is extrapolating the hypotool configuration from the (combined) chain name"""
        import copy
        from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import getChainDictFromChainName
        chainDict = getChainDictFromChainName(self.name, allChainDicts)

        print "got here", chainDict
        for step in self.steps:
            if len(chainDict['chainParts']) != len(step.sequences):
                log.error("Error in step %s: found %d chain parts and %d sequences"%(step.name, len(chainDict['chainParts']), len(step.sequences)))
                sys.exit("ERROR, in chain configuration")

            for seq, chainDictPart in zip(step.sequences, chainDict['chainParts']):
                if seq.ca is not None: # The CA merging took care of everything
                    continue
                onePartChainDict = copy.deepcopy( chainDict )
                onePartChainDict['chainParts'] = [ chainDictPart ]

                seq.hypoToolConf.setConf( onePartChainDict )
                seq.hypo.addHypoTool(seq.hypoToolConf)


class CFSequence():
    """Class to describe the ChainStep + filter

    """
    def __init__(self, ChainStep, FilterAlg):
        self.filter = FilterAlg
        self.step = ChainStep
        self.connect()

    def connect(self):
        """Connect filter to ChainStep (all its sequences)
        if a ChainStep contains the same sequence multiple times (for multi-object chains),
        the filter is connected only once (to avoid multiple DH links)
        """

        log.debug("CFSequence: Connect Filter %s with menuSequences of step %s"%(self.filter.Alg.name(), self.step.name))
        filter_output = self.filter.getOutputList()
        if len(filter_output) == 0:
            log.error("ERROR, no filter outputs are set!")
            sys.exit("ERROR, no filter outputs are set!")

        # check whether the number of filter outputs are the same as the number of sequences in the step
        if len(filter_output) != len(self.step.sequences):
            log.error("Found %d filter outputs and %d MenuSequences in Step %s"%( len(self.filter.getOutputList()), len(self.step.sequences), self.step.name))
            sys.exit("ERROR: Found %d filter outputs differnt from %d MenuSequences in Step %s"%( len(self.filter.getOutputList()), len(self.step.sequences), self.step.name))


        nseq=0
        for seq in self.step.sequences:
            filter_out = filter_output[nseq]
            log.debug("Found input %s to sequence::%s from Filter::%s (from seed %s)", filter_out, seq.name, self.filter.Alg.name(), seq.seed)
            seq.connectToFilter( filter_out )
            #seq.connectToFilter(self.filter, filter_out )
            nseq+=1


        if self.step.isCombo: 
            self.connectCombo()

    def connectCombo(self):
        # reset sequence output, they will b ereplaced by new combo outputs
        for seq in self.step.sequences:
            seq.outputs=[]

        for seq in self.step.sequences:
            combo_input=seq.hypo.getOutputList()[0]
            self.step.combo.addInput(combo_input)
            log.debug("Adding inputs %s to combo %s"%(combo_input, self.step.combo.Alg.name()))
            # inputs are the output decisions of the hypos of the sequences
            # outputs are the modified name of input deciions that need to be connected to the next filter
            combo_output="combo_%s"%combo_input
            self.step.combo.addOutput(combo_output)
            seq.outputs.append(combo_output)
            log.debug("Adding outputs %s to combo %s"%(combo_output, self.step.combo.Alg.name()))


    def __str__(self):
        return "--- CFSequence ---\n + Filter: %s \n +  %s \n "%(\
            self.filter, self.step )


class ChainStep:
    """Class to describe one step of a chain; if more than one menuSequence, then the step is combo"""
    def __init__(self, name,  Sequences=[]):
        self.name = name
        self.sequences=[]
        self.isCombo=len(Sequences)>1
        self.combo=None
        if self.isCombo:
            self.makeCombo(Sequences)
        else:
            self.sequences = Sequences

    def makeCombo(self, Sequences):
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
            new_hypoAlg=oldhypo.clone(newHypoAlgName)
            new_sequence.replaceHypoForCombo(new_hypoAlg)
            self.sequences.append(new_sequence)

    def __str__(self):
        return "--- ChainStep %s ---\n + isCombo: %d \n +  %s \n "%(self.name, self.isCombo, ' '.join(map(str, self.sequences) ))



# this is fragment for New JO


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
class InEventReco( ComponentAccumulator ):
    """ Class to handle in-event reco """
    pass


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
            from AthenaCommon.Constants import DEBUG
            self.viewMakerAlg = EventViewCreatorAlgorithm(name+'ViewsMaker',
                                                          ViewFallThrough = True,
                                                          RoIsLink        = 'initialRoI', # -||-
                                                          InViewRoIs      = name+'RoIs',
                                                          Views           = name+'Views',
                                                          ViewNodeName    = name+"InView",
                                                          OutputLevel=DEBUG)

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


class RecoFragmentsPool:
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
            log.debug( "created reconstruction fragment using function: %s" % creator.func_name )
            return recoFragment
        else:
            log.debug( "reconstruction fragment that would be created from %s is taken from the cache" % creator.func_name )
            return cls.fragments[requestHash]


def getChainStepName(chainName, stepNumber):
    return '{}_step{}'.format(chainName, stepNumber)

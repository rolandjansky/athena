
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )

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
            log.debug("Output DH not added in %s: %s already set!", self.name, name)
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
            log.debug("Input DH not added in %s: %s already set!", self.name, name)
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
    """ Class to group info on hypotools for ChainDict"""
    def __init__(self, hypoToolGen):
        self.hypoToolGen = hypoToolGen
        self.name=hypoToolGen.__name__ if hypoToolGen else "None"
       

    def setConf( self, chainDict):
        if type(chainDict) is not dict:
            raise RuntimeError("Configuring hypo with %s, not good anymore, use chainDict" % str(chainDict) )
        self.chainDict = chainDict


    def create(self):
        """creates instance of the hypo tool"""
        return self.hypoToolGen( self.chainDict )


class HypoAlgNode(AlgNode):
    """Node for HypoAlgs"""
    initialOutput= 'StoreGateSvc+UNSPECIFIED_OUTPUT'
    def __init__(self, Alg):
        assert isHypoBase(Alg), "Error in creating HypoAlgNode from Alg "  + Alg.name()
        AlgNode.__init__(self, Alg, 'HypoInputDecisions', 'HypoOutputDecisions')
        self.tools = []
        self.previous=[]

    def addOutput(self, name):
        outputs = self.readOutputList()        
        if name in outputs:
            log.debug("Output DH not added in %s: %s already set!", self.name, name)
        elif self.initialOutput in outputs:
            AlgNode.addOutput(self, name)
        else:
            log.error("Hypo " + self.name +" has already %s as configured output: you may want to duplicate the Hypo!" + outputs[0])




    def addHypoTool (self, hypoToolConf):
        log.debug("   Adding HypoTool %s to %s", hypoToolConf.chainDict['chainName'], self.Alg.name())
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
        input_maker_output = CFNaming.inputMakerOutName(self.Alg.name(),"out")
        self.addOutput(input_maker_output)


from DecisionHandling.DecisionHandlingConf import ComboHypo
class ComboMaker(AlgNode):
    def __init__(self, name, multiplicity):
        #Alg = RecoFragmentsPool.retrieve( self.create, name )
        log.debug("ComboMaker init: Alg %s", name)
        Alg = ComboHypo(name)
        AlgNode.__init__(self,  Alg, 'HypoInputDecisions', 'HypoOutputDecisions')
        self.prop="MultiplicitiesMap"
        self.mult=list(multiplicity)

    def create (self, name):
        log.debug("ComboMaker.create %s",name)
        return ComboHypo(name)
        
    def addChain(self, chainDict):
        chainName = chainDict['chainName']
        log.debug("ComboMaker %s adding chain %s", self.Alg.name(),chainName)
        allMultis = self.mult
        newdict = {chainName : allMultis}

        cval = self.Alg.getProperties()[self.prop]  # check necessary to see if chain was added already?
        if type(cval) is dict:
            if chainName in cval.keys():
                log.error("ERROR in cofiguration: ComboAlg %s has already been configured for chain %s", self.Alg.name(), chainName)
            else:
                cval[chainName]=allMultis
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
    """ By construction it has one Hypo Only; behaviour changed to support muFastOvlpRmSequence() which has two, but this will change"""
    
    def __init__(self, Sequence, Maker,  Hypo, HypoToolGen, CA=None ):
        assert Maker.name().startswith("IM"), "The input maker {} name needs to start with letter: IM".format(Maker.name())
        self.sequence     = Node( Alg=Sequence)
        self._maker       = InputMakerNode( Alg = Maker )
        self.seed=''
        self.reuse = False # flag to draw dot diagrmas
        self.ca = CA
        self.connect(Hypo, HypoToolGen)
        

    @property
    def maker(self):
        if self.ca is not None:
            makerAlg = self.ca.getEventAlgo(self._maker.Alg.name())
            self._maker.Alg = makerAlg
        return self._maker

    @property
    def hypo(self):
        if self.ca is not None:
            hypoAlg = self.ca.getEventAlgo(self._hypo.Alg.name())
            self._hypo.Alg = hypoAlg
        return self._hypo

    def connectToFilter(self, outfilter):
        """ Connect filter to the InputMaker"""
        self.maker.addInput(outfilter)
      

    def connect(self, Hypo, HypoToolGen):
        """ Sets the input and output of the hypo, and links to the input maker """
        input_maker_output= self.maker.readOutputList()[0] # only one since it's merged

         #### Add input/output Decision to Hypo
        if type(Hypo) is list:
            self.name=[]
            self.hypoToolConf=[]
            self._hypo=[]
            hypo_input_total=[]
            hypo_output_total=[]
            hypo_input = input_maker_output
            for hypo_alg, hptool in zip(Hypo, HypoToolGen):
              self.name.append( CFNaming.menuSequenceName(hypo_alg.name()) )
              self.hypoToolConf.append( HypoToolConf( hptool ) )

              hypo_input_total.append(hypo_input)
              hypo_output = CFNaming.hypoAlgOutName(hypo_alg.name())
              hypo_output_total.append(hypo_output)

              hypo_node = HypoAlgNode( Alg = hypo_alg )
              hypo_node.addOutput(hypo_output)
              hypo_node.setPreviousDecision(hypo_input)
              
              self._hypo.append( hypo_node )
              hypo_input = hypo_output
            log.warning("Sequence %s has more than one Hypo; correct your sequence for next develpments", self.name)
        else:
           self.name = CFNaming.menuSequenceName(Hypo.name())
           self.hypoToolConf = HypoToolConf( HypoToolGen )
           self._hypo = HypoAlgNode( Alg = Hypo )
           hypo_output = CFNaming.hypoAlgOutName(Hypo.name())
           self._hypo.addOutput(hypo_output)
           self._hypo.setPreviousDecision( input_maker_output)

  

        log.debug("MenuSequence.connect: connecting InputMaker and HypoAlg and OverlapRemoverAlg, adding: \n\
        InputMaker::%s.output=%s",\
                        self.maker.Alg.name(), input_maker_output)
        if type(self._hypo) is list:
           for hp, hp_in, hp_out in zip( self._hypo, hypo_input_total, hypo_output_total):
              log.debug("HypoAlg::%s.previousDecision=%s, \n\
                         HypoAlg::%s.output=%s",\
                              hp.Alg.name(), hp_in, hp.Alg.name(), hp_out)
        else:
           log.debug("HypoAlg::%s.previousDecision=%s, \n\
                      HypoAlg::%s.output=%s",\
                           self.hypo.Alg.name(), input_maker_output, self.hypo.Alg.name(), self.hypo.readOutputList()[0])


    def __repr__(self):
        if type(self._hypo) is list:
           hyponame=[]
           hypotool=[]
           for hp, hptool in zip(self._hypo, self.hypoToolConf):
              hyponame.append( hp.Alg.name() )
              hypotool.append( hptool.name )
           return "MenuSequence::%s \n Hypo::%s \n Maker::%s \n Sequence::%s \n HypoTool::%s"\
           %(self.name, hyponame, self.maker.Alg.name(), self.sequence.Alg.name(), hypotool)
        else:
           hyponame = self._hypo.Alg.name()
           hypotool = self.hypoToolConf.name
           return "MenuSequence::%s \n Hypo::%s \n Maker::%s \n Sequence::%s \n HypoTool::%s\n"\
           %(self.name, hyponame, self.maker.Alg.name(), self.sequence.Alg.name(), hypotool)




#################################################

#from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import getAllThresholdsFromItem, getUniqueThresholdsFromItem

       
class Chain(object):
    """Basic class to define the trigger menu """
    __slots__='name','steps','vseeds','L1decisions'
    def __init__(self, name, ChainSteps, L1Thresholds):
        """
        Construct the Chain from the steps
        Out of all arguments the ChainSteps & L1Thresholds are most relevant, the chain name is used in debug messages
        """
        self.name = name
        self.steps=ChainSteps
        self.vseeds=L1Thresholds

        from L1Decoder.L1DecoderConfig import mapThresholdToL1DecisionCollection
        # L1decisions are used to set the seed type (EM, MU,JET), removing the actual threshold
        # in practice it is the L1Decoder Decision output
        self.L1decisions = [ mapThresholdToL1DecisionCollection(stri) for stri in L1Thresholds]
        log.debug("L1Decisions: %s", ' '.join(self.L1decisions))
        
        self.setSeedsToSequences()
        isCombo=False
        #TO DO: check that all the steps are combo
        for step in self.steps:
            if step.isCombo:
                isCombo=True

        log.debug("Made %s Chain %s with seeds: %s ", "combo" if isCombo else "", name, self.L1decisions)


    def checkMultiplicity(self):
        if len(self.steps) == 0:
            return 0
        mult=[sum(step.multiplicity) for step in self.steps] # on mult per step
        not_empty_mult = [m for m in mult if m!=0]
        if len(not_empty_mult) == 0: #empty chain?
            log.error("checkMultiplicity: Chain %s has all steps with multiplicity =0: what to do?", self.name)
            return 0
        if not_empty_mult.count(not_empty_mult[0]) != len(not_empty_mult):
            log.error("checkMultiplicity: Chain %s has steps with differnt multiplicities: %s", self.name, ' '.join(mult))
            return 0

        if not_empty_mult[0] != len(self.vseeds):
            log.error("checkMultiplicity: Chain %s has %d multiplicity per step, and %d L1Decisions", self.name, mult, len(self.vseeds))
            return 0
        return not_empty_mult[0]

        
       
    def setSeedsToSequences(self):
        """ Set the L1 seeds (L1Decisions) to the menu sequences """
        if len(self.steps) == 0:
            return

        # TODO: check if the number of seeds is sufficient for all the seuqences, no action of no steps are configured
        for step in self.steps:
            for seed, seq in zip(self.L1decisions, step.sequences):
                seq.seed= seed
                log.debug( "setSeedsToSequences: Chain %s adding seed %s to sequence in step %s", self.name, seq.seed, step.name )                                 

    def getChainLegs(self):
        """ This is extrapolating the chain legs"""
        from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainInDict
        listOfChainDictsLegs = splitChainInDict(self.name)
        legs = [part['chainName'] for part in listOfChainDictsLegs]
        return legs
                

    def decodeHypoToolConfs(self):
        """ This is extrapolating the hypotool configuration from the chain name"""
        log.debug("decodeHypoToolConfs for chain %s", self.name)
        from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainInDict
        listOfChainDictsLegs = splitChainInDict(self.name)
        for step in self.steps:
            if len(step.sequences) == 0:
                continue

            step_mult = [str(m) for m in step.multiplicity]
            menu_mult = [part['chainParts'][0]['multiplicity'] for part in listOfChainDictsLegs ] 
            if step_mult != menu_mult:
                # need to agree on the procedure: if the jet code changes the chainparts accordingly, this will never happen
                log.warning("Got multiplicty %s from chain parts, but have %s legs. This is expected now for jets, so this tmp fix is added:", menu_mult, step_mult)
                chainDict = listOfChainDictsLegs[0]
                chainDict['chainName']= self.name # rename the chaindict to remove the leg name
                for seq in step.sequences:
                    seq.hypoToolConf.setConf( chainDict )
                    seq.hypo.addHypoTool(seq.hypoToolConf) #this creates the HypoTools                    
                continue

            # add one hypotool per sequence and chain part
            for seq, onePartChainDict in zip(step.sequences, listOfChainDictsLegs):
                if seq.ca is not None: # The CA merging took care of everything
                    continue
                if type(seq.hypoToolConf) is list:
                    log.warning ("This sequence %s has %d multiple HypoTools ",seq.sequence.name, len(seq.hypoToolConf))
                    for hp, hptoolConf in zip( seq.hypo, seq.hypoToolConf ):
                        hptoolConf.setConf( onePartChainDict )
                        hp.addHypoTool(hptoolConf) #this creates the HypoTools
                else:
                    seq.hypoToolConf.setConf( onePartChainDict )
                    seq.hypo.addHypoTool(seq.hypoToolConf) #this creates the HypoTools
                    

    def __repr__(self):
        return "--- Chain %s --- \n + Seeds: %s \n + Steps: \n %s \n"%(\
                    self.name, ' '.join(map(str, self.L1decisions)), '\n '.join(map(str, self.steps)))




class CFSequence(object):
    """Class to describe the flow of decisions through ChainStep + filter with their connections (input, output)
    A Filter can have more than one input/output if used in different chains, so this class stores and manages all of them (when doing the connect)
    """
    def __init__(self, ChainStep, FilterAlg):
        self.filter = FilterAlg
        self.step = ChainStep
        if self.step.isCombo:
            self.connectCombo()
        self.setDecisions()
        log.debug("CFSequence.__init: created %s ",self)

    def setDecisions(self):
        """ Set the output decision of this CFSequence as the hypo outputdecision; In case of combo, takes the Combo outputs"""
        self.decisions=[]
        if not len(self.step.sequences):
            self.decisions.extend(self.filter.readOutputList())
        else:
            if self.step.isCombo:
                self.decisions.extend(self.step.combo.getOutputList())
            else:
                for sequence in self.step.sequences:
                    hp=sequence.hypo
                    if type(hp) is list:
                        for hypo in hp:
                            self.decisions.append(hypo.readOutputList()[0])
                    else:
                        self.decisions.append(hp.readOutputList()[0])

        log.debug("CFSequence: set out decisions: %s", self.decisions)


    def connect(self, connections):
        """Connect filter to ChainStep (and all its sequences) through these connections (which are sets of filter outputs)
        if a ChainStep contains the same sequence multiple times (for multi-object chains),
        the filter is connected only once (to avoid multiple DH links)
        """
        log.debug("CFSequence: connect Filter %s with %d menuSequences of step %s, using %d connections", self.filter.Alg.name(), len(self.step.sequences), self.step.name, len(connections))
        if len(connections) == 0:
            log.error("ERROR, no filter outputs are set!")
            #raise("CFSequence: Invalid Filter Configuration")

        if len(self.step.sequences):
            # check whether the number of filter outputs are the same as the number of sequences in the step
            if len(connections) != len(self.step.sequences):
                log.error("Found %d connections and %d MenuSequences in Step %s", len(connections), len(self.step.sequences), self.step.name)
                #raise("CFSequence: Invalid Filter Configuration")
            nseq=0
            for seq in self.step.sequences:
                filter_out = connections[nseq]
                log.debug("CFSequence: Found input %s to sequence::%s from Filter::%s (from seed %s)", filter_out, seq.name, self.filter.Alg.name(), seq.seed)
                seq.connectToFilter( filter_out )
                nseq+=1
        else:
          log.debug("This CFSequence has no sequences: outputs are the Filter outputs")



    def connectCombo(self):
        """ connect Combo to Hypos"""
        for seq in self.step.sequences:
            if type(seq.hypo) is list:
               combo_input=seq.hypo[-1].readOutputList()[0] # last one?
            else:
               combo_input=seq.hypo.readOutputList()[0]
            self.step.combo.addInput(combo_input)
            log.debug("CFSequence.connectCombo: adding input to  %s: %s",  self.step.combo.Alg.name(), combo_input)
            # inputs are the output decisions of the hypos of the sequences
            combo_output=CFNaming.comboHypoOutputName (self.step.combo.Alg.name(), combo_input)
            self.step.combo.addOutput(combo_output)
            log.debug("CFSequence.connectCombo: adding output to  %s: %s",  self.step.combo.Alg.name(), combo_output)



    def __repr__(self):
        return "--- CFSequence ---\n + Filter: %s \n + decisions: %s\n +  %s \n"%(\
                    self.filter.Alg.name(), self.decisions, self.step)



class StepComp(object):
    """ Class to build hte ChainStep, for including empty sequences"""
    def __init__(self, sequence, multiplicity,empty):
        self.sequence=sequence
        self.multiplicity=multiplicity
        self.empty=empty

class ChainStep(object):
    """Class to describe one step of a chain; if multiplicity is greater than 1, the step is combo/combined.  Set one multiplicity value per sequence"""
    def __init__(self, name,  Sequences=[], multiplicity=[1]):
       
        self.name = name
        self.sequences=Sequences
        self.multiplicity = multiplicity
        self.isCombo=sum(multiplicity)>1
        self.combo=None
        if self.isCombo:
            self.makeCombo(Sequences )
        self.decisions = []

    def makeCombo(self, Sequences):
        if len(Sequences)==0:
            return
        hashableMult = tuple(self.multiplicity)
        self.combo =  RecoFragmentsPool.retrieve(createComboAlg, None, name=CFNaming.comboHypoName(self.name), multiplicity=hashableMult)


    def __repr__(self):
        return "--- ChainStep %s ---\n + isCombo = %d, multiplicity = %d \n + MenuSequences = %s"%(self.name, self.isCombo,sum(self.multiplicity), ' '.join(map(str, [seq.name for seq in self.sequences]) ))


def createComboAlg(dummyFlags, name, multiplicity):
    return ComboMaker(name, multiplicity)


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

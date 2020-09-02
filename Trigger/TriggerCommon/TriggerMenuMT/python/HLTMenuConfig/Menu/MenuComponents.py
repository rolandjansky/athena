# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )
from collections import MutableSequence
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponentsNaming import CFNaming
from AthenaCommon.CFElements import parOR, seqAND, compName, getProp
from DecisionHandling.DecisionHandlingConfig import ComboHypoCfg
from AthenaConfiguration.ComponentFactory import CompFactory
RoRSeqFilter=CompFactory.RoRSeqFilter

class Node(object):
    """base class representing one Alg + inputs + outputs, to be used to Draw dot diagrams and connect objects"""
    def __init__(self, Alg):
        self.name = ("%sNode")%( Alg.name )
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
        return "Node::%s  [%s] -> [%s]"%(self.Alg.name, ' '.join(map(str, self.getInputList())), ' '.join(map(str, self.getOutputList())))



class AlgNode(Node):
    """Node class that connects inputs and outputs to basic alg. properties """
    def __init__(self, Alg, inputProp, outputProp):
        Node.__init__(self, Alg)
        self.outputProp=outputProp
        self.inputProp=inputProp

    def addDefaultOutput(self):
        if self.outputProp != '':
            self.addOutput(("%s_%s"%(self.Alg.getName(),self.outputProp)))

    def setPar(self, propname, value):
        cval = getProp( self.Alg, propname)
        if isinstance(cval, MutableSequence):
            cval.append(value)
            return setattr(self.Alg, propname, cval)
        else:
            return setattr(self.Alg, propname, value)

    def resetPar(self, prop):
        cval = getProp(self.Alg, prop)
        if isinstance(cval, MutableSequence):
            return setattr(self.Alg, prop, [])
        else:
            return setattr(self.Alg, prop, "")

    def getPar(self, prop):
        return getProp(self.Alg, prop)


    def resetOutput(self):
        self.resetPar(self.outputProp)

    def resetInput(self):
        self.resetPar(self.inputProp)

    def addOutput(self, name):
        outputs = self.readOutputList()
        if name in outputs:
            log.debug("Output DH not added in %s: %s already set!", self.Alg.getName(), name)
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
        if isinstance(cval, MutableSequence):
            outputs.extend(cval)
        else:
            outputs.append(cval)
        return outputs

    def addInput(self, name):
        inputs = self.readInputList()
        if name in inputs:
            log.debug("Input DH not added in %s: %s already set!", self.Alg.getName(), name)
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
        if isinstance(cval, MutableSequence):
            inputs.extend(cval)
        else:
            inputs.append(cval)
        return inputs

    def __repr__(self):
        return "Alg::%s  [%s] -> [%s]"%(self.Alg.getName(), ' '.join(map(str, self.getInputList())), ' '.join(map(str, self.getOutputList())))


def algColor(alg):
    """ Set given color to Alg type"""
    if isComboHypoAlg(alg):
        return "darkorange"
    if isHypoBase(alg):
        return "darkorchid1"
    if isInputMakerBase(alg):
        return "cyan3"
    if isFilterAlg(alg):
        return "chartreuse3"
    return "cadetblue1"


class HypoToolConf(object):
    """ Class to group info on hypotools for ChainDict"""
    def __init__(self, hypoToolGen):
        self.hypoToolGen = hypoToolGen
        self.name=hypoToolGen.__name__


    def setConf( self, chainDict):
        if type(chainDict) is not dict:
            raise RuntimeError("Configuring hypo with %s, not good anymore, use chainDict" % str(chainDict) )
        self.chainDict = chainDict


    def create(self):
        """creates instance of the hypo tool"""
        return self.hypoToolGen( self.chainDict )
    
    
    def confAndCreate(self, chainDict):
        """sets the configuration and creates instance of the hypo tool"""
        self.setConf(chainDict)
        return self.create()


class HypoAlgNode(AlgNode):
    """Node for HypoAlgs"""
    initialOutput= 'StoreGateSvc+UNSPECIFIED_OUTPUT'
    def __init__(self, Alg):
        assert isHypoBase(Alg), "Error in creating HypoAlgNode from Alg "  + compName(Alg)
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
        log.debug("   Adding HypoTool %s to %s", hypoToolConf.chainDict['chainName'], compName(self.Alg))
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
        return "HypoAlg::%s  [%s] -> [%s], previous = [%s], HypoTools=[%s]"%(compName(self.Alg),' '.join(map(str, self.getInputList())),
                                                                                 ' '.join(map(str, self.getOutputList())),
                                                                                 ' '.join(map(str, self.previous)),
                                                                                 ' '.join(map(str, self.tools)))


class SequenceFilterNode(AlgNode):
    """Node for any kind of sequence filter"""
    def __init__(self, Alg, inputProp, outputProp):
        AlgNode.__init__(self,  Alg, inputProp, outputProp)

    def addChain(self, name):
        return self.setPar("Chains", name)

    def getChains(self):
        return self.getPar("Chains")

    def __repr__(self):
        return "SequenceFilter::%s  [%s] -> [%s], chains=%s"%(compName(self.Alg),' '.join(map(str, self.getInputList())),' '.join(map(str, self.getOutputList())), self.getChains())


class RoRSequenceFilterNode(SequenceFilterNode):
    def __init__(self, name):
        Alg= RoRSeqFilter(name)
        SequenceFilterNode.__init__(self,  Alg, 'Input', 'Output')



class InputMakerNode(AlgNode):
    def __init__(self, Alg):
        assert isInputMakerBase(Alg), "Error in creating InputMakerNode from Alg "  + compName(Alg)
        AlgNode.__init__(self,  Alg, 'InputMakerInputDecisions', 'InputMakerOutputDecisions')
        input_maker_output = CFNaming.inputMakerOutName(compName(self.Alg))
        self.addOutput(input_maker_output)


class ComboMaker(AlgNode):
    def __init__(self, name, multiplicity, comboHypoCfg):
        self.prop="MultiplicitiesMap"
        self.mult=list(multiplicity)
        self.comboHypoCfg = comboHypoCfg
        Alg = RecoFragmentsPool.retrieve( self.create, name )
        log.debug("ComboMaker init: Alg %s", name)
        AlgNode.__init__(self,  Alg, 'HypoInputDecisions', 'HypoOutputDecisions')

    def create (self, name):
        log.debug("ComboMaker.create %s",name)
        return self.comboHypoCfg(name)

    def addChain(self, chainDict):
        chainName = chainDict['chainName']
        log.debug("ComboMaker %s adding chain %s", compName(self.Alg), chainName)
        allMultis = self.mult
        newdict = {chainName : allMultis}

        cval = self.Alg.getProperties()[self.prop]  # check necessary to see if chain was added already?
        if type(cval) is dict:
            if chainName in cval.keys():
                log.error("ERROR in cofiguration: ComboAlg %s has already been configured for chain %s", compName(self.Alg), chainName)
            else:
                cval[chainName]=allMultis
        else:
            cval=newdict

        setattr(self.Alg, self.prop, cval)


    def getChains(self):
        cval = self.Alg.getProperties()[self.prop]
        return cval.keys()


    def createComboHypoTools(self, chainDict, comboToolConfs):
        """Created the ComboHypoTools"""
        if not len(comboToolConfs):
            return
        confs = [ HypoToolConf( tool ) for tool in comboToolConfs ]
        log.debug("ComboMaker.createComboHypoTools for chain %s, Alg %s with %d tools", chainDict["chainName"],self.Alg.getName(), len(comboToolConfs))        
        for conf in confs:
            tools = self.Alg.ComboHypoTools
            self.Alg.ComboHypoTools = tools + [ conf.confAndCreate( chainDict ) ]


#########################################################
# USEFULL TOOLS
#########################################################

def isHypoBase(alg):
    if  'HypoInputDecisions'  in alg.__class__.__dict__:
        return True
    prop = alg.__class__.__dict__.get('_properties')
    if type(prop) is dict:
        return  ('HypoInputDecisions'  in prop)
    else:
        return False

def isInputMakerBase(alg):
    return  ('InputMakerInputDecisions'  in alg.__class__.__dict__)

def isFilterAlg(alg):
    return isinstance(alg, RoRSeqFilter)

def isComboHypoAlg(alg):
    return  ('MultiplicitiesMap'  in alg.__class__.__dict__)




##########################################################
# Now sequences and chains
##########################################################

class EmptyMenuSequence(object):
    """ Class to emulate reco sequences with no Hypo"""
    """ By construction it has no Hypo;"""
    
    def __init__(self, the_name):
        self._name = the_name
        Maker = CompFactory.InputMakerForRoI("IM"+the_name)
        Maker.RoITool = CompFactory.ViewCreatorInitialROITool()
        self._maker       = InputMakerNode( Alg = Maker )
        self._seed=''
        self._sequence    = Node( Alg = seqAND(the_name, [Maker]))
        log.debug("Made EmptySequence %s",the_name)

    @property
    def sequence(self):
        return self._sequence

    @property
    def seed(self):
        return self._seed

    @property
    def name(self):
        return self._name

    def getOutputList(self):
        return self._maker.readOutputList() # Only one since it's merged

    def connectToFilter(self, outfilter):
        """ Connect filter to the InputMaker"""
        self._maker.addInput(outfilter)

    def createHypoTools(self, chainDict):
        log.debug("This sequence is empty. No Hypo to conficure")

    def addToSequencer(self, recoSeq_list, hypo_list):
        recoSeq_list.add(self.sequence.Alg)                    
        
    def buildDFDot(self, cfseq_algs, all_hypos, isCombo, last_step_hypo_nodes, file):
        cfseq_algs.append(self._maker)
        cfseq_algs.append(self.sequence )

        file.write("    %s[fillcolor=%s]\n"%(self._maker.Alg.getName(), algColor(self._maker.Alg)))
        file.write("    %s[fillcolor=%s]\n"%(self.sequence.Alg.getName(), algColor(self.sequence.Alg)))
 
        return cfseq_algs, all_hypos, last_step_hypo_nodes

    def getTools(self):
        log.debug("No tools for empty menu sequences")

    def setSeed( self, seed ):
        self._seed = seed

    def __repr__(self):
        return "MenuSequence::%s \n Hypo::%s \n Maker::%s \n Sequence::%s \n HypoTool::%s\n"\
            %(self.name, "Empty", self._maker.Alg.getName(), self.sequence.Alg.getName(), "None")

class MenuSequence(object):
    """ Class to group reco sequences with the Hypo"""
    """ By construction it has one Hypo Only; behaviour changed to support muFastOvlpRmSequence() which has two, but this will change"""

    def __init__(self, Sequence, Maker,  Hypo, HypoToolGen):
        assert compName(Maker).startswith("IM"), "The input maker {} name needs to start with letter: IM".format(compName(Maker))
        self._sequence     = Node( Alg=Sequence)
        self._maker       = InputMakerNode( Alg = Maker )
        self._seed=''

        if isinstance(Hypo, list): # we will remove support for this and will issue error
            log.warning("Sequence %s has more than one Hypo; correct your sequence in the next development cycle", self.name)
            assert len(Hypo) == len(HypoToolGen), "The number of hypo algs {} and hypo tools {} not the same".format( len(Hypo), len(HypoToolGen) )

        input_maker_output= self.maker.readOutputList()[0] # only one since it's merged

        #### Add input/output Decision to Hypo, handle first somewhat ill-defined case
        if isinstance(Hypo, list):
            self._hypo         = [ HypoAlgNode( Alg = alg ) for alg in Hypo ]
            self._hypoToolConf = [ HypoToolConf( tool ) for tool in HypoToolGen ]
            self._name         = [ CFNaming.menuSequenceName(compName(alg)) for alg in self._hypo ]

            hypo_input = input_maker_output
            for hypo_alg_node in self._hypo:
                hypo_output = CFNaming.hypoAlgOutName(compName(hypo_alg_node.Alg))
                hypo_alg_node.addOutput(  hypo_output )
                hypo_alg_node.setPreviousDecision( hypo_input )
                hypo_input = hypo_output

        else:
            self._name = CFNaming.menuSequenceName(compName(Hypo))
            self._hypoToolConf = HypoToolConf( HypoToolGen )
            self._hypo = HypoAlgNode( Alg = Hypo )
            hypo_output = CFNaming.hypoAlgOutName(compName(Hypo))
            self._hypo.addOutput(hypo_output)
            self._hypo.setPreviousDecision( input_maker_output)

        log.debug("MenuSequence.connect: connecting InputMaker and HypoAlg, adding: \n\
        InputMaker::%s.output=%s",\
                        compName(self.maker.Alg), input_maker_output)

        if type(self._hypo) is list:
            hypo_input_total = []
            [ hypo_input_total.extend( alg_node.Alg.getInputList() )  for alg_node in self._hypo ]
            hypo_output_total = []
            [ hypo_output_total.extend( alg_node.Alg.getOutputList() )  for alg_node in self._hypo ]

            for hp, hp_in, hp_out in zip( self._hypo, hypo_input_total, hypo_output_total):
                log.debug("HypoAlg::%s.HypoInputDecisions=%s, \n\
                HypoAlg::%s.HypoOutputDecisions=%s",\
                          compName(hp.Alg), hp_in, compName(hp.Alg), hp_out)
        else:
           log.debug("HypoAlg::%s.HypoInputDecisions=%s, \n \
           HypoAlg::%s.HypoOutputDecisions=%s",\
                           compName(self.hypo.Alg), self.hypo.readInputList()[0], compName(self.hypo.Alg), self.hypo.readOutputList()[0])


    @property
    def seed(self):
        return self._seed


    @property
    def name(self):
        return self._name


    @property
    def sequence(self):
        return self._sequence


    @property
    def maker(self):
        return self._maker

    @property
    def hypo(self):
        return self._hypo


    def getOutputList(self):
        outputlist = []
        if type(self._hypo) is list:
            for hypo in self._hypo:
                outputlist.append(hypo.readOutputList()[0])
        else:
            outputlist.append(self._hypo.readOutputList()[0])
        return outputlist


    def connectToFilter(self, outfilter):
        """ Connect filter to the InputMaker"""
        self._maker.addInput(outfilter)
      

    def connect(self, Hypo, HypoToolGen):
        """ Sets the input and output of the hypo, and links to the input maker """
        input_maker_output= self._maker.readOutputList()[0] # only one since it's merged

         #### Add input/output Decision to Hypo
        if type(Hypo) is list:
            self.name=[]
            self.hypoToolConf=[]
            self._hypo=[]
            hypo_input_total=[]
            hypo_output_total=[]
            hypo_input = input_maker_output
            for hypo_alg, hptool in zip(Hypo, HypoToolGen):
              self.name.append( CFNaming.menuSequenceName(compName(hypo_alg)) )
              self.hypoToolConf.append( HypoToolConf( hptool ) )

              hypo_input_total.append(hypo_input)
              hypo_output = CFNaming.hypoAlgOutName(compName(hypo_alg))
              hypo_output_total.append(hypo_output)

              hypo_node = HypoAlgNode( Alg = hypo_alg )
              hypo_node.addOutput(hypo_output)
              hypo_node.setPreviousDecision(hypo_input)
              
              self._hypo.append( hypo_node )
              hypo_input = hypo_output
            log.warning("Sequence %s has more than one Hypo; correct your sequence for next develpments", self.name)
        else:
           self.name = CFNaming.menuSequenceName(compName(Hypo))
           self.hypoToolConf = HypoToolConf( HypoToolGen )
           self._hypo = HypoAlgNode( Alg = Hypo )
           hypo_output = CFNaming.hypoAlgOutName(compName(Hypo))
           self._hypo.addOutput(hypo_output)
           self._hypo.setPreviousDecision( input_maker_output)


        log.debug("MenuSequence.connect: connecting InputMaker and HypoAlg and OverlapRemoverAlg, adding: \n\
        InputMaker::%s.output=%s",\
                        compName(self._maker.Alg), input_maker_output)
        if type(self._hypo) is list:
           for hp, hp_in, hp_out in zip( self._hypo, hypo_input_total, hypo_output_total):
              log.debug("HypoAlg::%s.previousDecision=%s, \n\
                         HypoAlg::%s.output=%s",\
                              compName(hp.Alg), hp_in, compName(hp.Alg), hp_out)
        else:
           log.debug("HypoAlg::%s.previousDecision=%s, \n\
                      HypoAlg::%s.output=%s",\
                           compName(self._hypo.Alg), input_maker_output, compName(self._hypo.Alg), self._hypo.readOutputList()[0])


    def createHypoTools(self, chainDict):
        if type(self._hypoToolConf) is list:
            log.warning ("This sequence %s has %d multiple HypoTools ",self.sequence.name, len(self.hypoToolConf))
            for hypo, hypoToolConf in zip(self._hypo, self._hypoToolConf):
                hypoToolConf.setConf( chainDict )
                hypo.addHypoTool(self._hypoToolConf)
        else:
            self._hypoToolConf.setConf( chainDict )
            self._hypo.addHypoTool(self._hypoToolConf) #this creates the HypoTools  


    def addToSequencer(self, recoSeq_list, hypo_list):
        recoSeq_list.add(self.sequence.Alg)
        hypo_list.add(self._hypo.Alg)
            

    def buildDFDot(self, cfseq_algs, all_hypos, isCombo, last_step_hypo_nodes, file):
        cfseq_algs.append(self._maker)
        cfseq_algs.append(self.sequence )

        file.write("    %s[fillcolor=%s]\n"%(self._maker.Alg.getName(), algColor(self._maker.Alg)))
        file.write("    %s[fillcolor=%s]\n"%(self.sequence.Alg.getName(), algColor(self.sequence.Alg)))
        
        if type(self._hypo) is list:
            for hp in self._hypo:
                cfseq_algs.append(hp)
                file.write("    %s[color=%s]\n"%(hp.Alg.getName(), algColor(hp.Alg)))
                all_hypos.append(hp)
        else:
            cfseq_algs.append(self._hypo)
            file.write("    %s[color=%s]\n"%(self._hypo.Alg.getName(), algColor(self._hypo.Alg)))
            all_hypos.append(self._hypo)
            if not isCombo:
                if type(self._hypo) is list:
                    last_step_hypo_nodes.append(self._hypo[-1])
                else:
                    last_step_hypo_nodes.append(self._hypo)

        return cfseq_algs, all_hypos, last_step_hypo_nodes


    def getTools(self):
        if type(self._hypo) is list:
            return self._hypo[0].tools
        else:
            return self._hypo.tools

    def setSeed( self, seed ):
        self._seed = seed

    def __repr__(self):
        if type(self._hypo) is list:
           hyponame=[]
           hypotool=[]
           for hp, hptool in zip(self._hypo, self._hypoToolConf):
              hyponame.append( hp.Alg.getName() )
              hypotool.append( hptool.name )
           return "MenuSequence::%s \n Hypo::%s \n Maker::%s \n Sequence::%s \n HypoTool::%s"\
           %(self.name, hyponame, self._maker.Alg.getName(), self.sequence.Alg.getName(), hypotool)
        else:
           hyponame = self._hypo.Alg.getName()
           hypotool = self._hypoToolConf.name
           return "MenuSequence::%s \n Hypo::%s \n Maker::%s \n Sequence::%s \n HypoTool::%s\n"\
           %(self.name, hyponame, self._maker.Alg.getName(), self.sequence.Alg.getName(), hypotool)


class CAMenuSequence(MenuSequence):
    ''' MenuSequence with Compoment Accumulator '''

    def __init__(self, Sequence, Maker,  Hypo, HypoToolGen, CA):
        self.ca = CA
        MenuSequence.__init__(self, Sequence, Maker,  Hypo, HypoToolGen)

    @property
    def sequence(self):
        makerAlg = self.ca.getEventAlgo(self._maker.Alg.name)
        self._maker.Alg = makerAlg
        return self._sequence

    @property
    def maker(self):
        makerAlg = self.ca.getEventAlgo(self._maker.Alg.name)
        self._maker.Alg = makerAlg
        return self._maker

    @property
    def hypo(self):
        hypoAlg = self.ca.getEventAlgo(self._hypo.Alg.name)
        self._hypo.Alg = hypoAlg
        return self._hypo


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

    def numberAllSteps(self):
        if len(self.steps)==0:
            return
        else:
            import re
            for stepID,step in enumerate(self.steps):
                step_name = step.name
                if re.search('^Step[0-9]_',step_name):
                    step_name = step_name[6:]
                step.name = 'Step%d_'%(stepID+1)+step_name
        return

    def insertEmptySteps(self, chainDict, empty_step_name, n_new_steps, start_position):
        #start position indexed from 0. if start position is 3 and length is 2, it works like:
        # [old1,old2,old3,old4,old5,old6] ==> [old1,old2,old3,empty1,empty2,old4,old5,old6]
        import re

        if len(self.steps) == 0 :
            log.error("I can't insert empty steps because the chain doesn't have any steps yet!")

        if len(self.steps) < start_position :
            log.error("I can't insert empty steps at step %d because the chain doesn't have that many steps!", start_position)

        
        chain_steps_pre_split = self.steps[:start_position]
        chain_steps_post_split = self.steps[start_position:]

        next_step_name = ''
        prev_step_name = ''
        if start_position == 0:
            next_step_name = chain_steps_post_split[0].name
            if re.search('^Step[0-9]_',next_step_name):
                next_step_name = next_step_name[6:]
            prev_step_name = 'empty_'+str(len(self.L1decisions))+'L1in'
        else:
            if len(chain_steps_post_split) == 0:
                log.error("Adding empty steps to the end of a chain - why would you do this?")
            else:
                prev_step_name = chain_steps_pre_split[-1].name
                next_step_name = chain_steps_post_split[0].name

        steps_to_add = []
        for stepID in range(1,n_new_steps+1):
            new_step_name =  prev_step_name+'_'+empty_step_name+'%d_'%stepID+next_step_name

            log.debug("Configuring empty step " + new_step_name)
            steps_to_add += [ChainStep(new_step_name, [], [], [chainDict], comboHypoCfg=ComboHypoCfg)]
        
        self.steps = chain_steps_pre_split + steps_to_add + chain_steps_post_split

        return

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
                    seq.setSeed( seed )
                    log.debug( "setSeedsToSequences: Chain %s adding seed %s to sequence in step %s", self.name, seed, step.name )

    def getChainLegs(self):
        """ This is extrapolating the chain legs from the chain dictionary"""
        from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainInDict
        listOfChainDictsLegs = splitChainInDict(self.name)
        legs = [part['chainName'] for part in listOfChainDictsLegs]      
        return legs


    def createHypoTools(self):
        """ This is extrapolating the hypotool configuration from the chain name"""
        log.debug("createHypoTools for chain %s", self.name)
        from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainInDict

        # this spliting is only needed for chains which don't yet attach
        # the dictionaries to the chain steps. It should be removed
        # later once that migration is done.
        listOfChainDictsLegs = splitChainInDict(self.name)
        for step in self.steps:
            log.debug("createHypoTools for Step %s", step.name)
            if len(step.sequences) == 0:
                continue
            
            if sum(step.multiplicity) >1 and not step.isCombo:
                log.error("This should be an error, because step mult > 1 (%d), but step is not combo", sum(step.multiplicity))

            if len(step.chainDicts) > 0:
                # new way to configure hypo tools, works if the chain dictionaries have been attached to the steps
                log.debug('%s in new hypo tool creation method, step mult= %d, isCombo=%d', self.name, sum(step.multiplicity), step.isCombo)
                log.debug("N(seq)=%d, N(chainDicts)=%d", len(step.sequences), len(step.chainDicts))
                assert len(step.sequences)==len(step.chainDicts), "createHypoTools only makes sense if number of sequences == number of chain dicts"
                for seq, onePartChainDict in zip(step.sequences, step.chainDicts):
                    log.debug('    seq: %s, onePartChainDict:', seq.name)
                    log.debug('    ' + str(onePartChainDict))
                    seq.createHypoTools( onePartChainDict )              

            else:
                # legacy way, to be removed once all signatures pass the chainDicts to the steps
                step_mult = [str(m) for m in step.multiplicity]
                log.debug('%s in old hypo tool creation method', self.name)
                menu_mult = [ part['chainParts'][0]['multiplicity'] for part in listOfChainDictsLegs ]
                if step_mult != menu_mult:
                    # Probably this shouldn't happen, but it currently does
                    log.warning("Got multiplicty %s from chain parts, but have %s multiplicity in this step. This is expected only for jet chains, but it has happened for %s, using the first chain dict", menu_mult, step.multiplicity, self.name)
                    firstChainDict = listOfChainDictsLegs[0]
                    firstChainDict['chainName']= self.name # rename the chaindict to remove the leg name
                    for seq in step.sequences:
                        seq.createHypoTools( firstChainDict )
                else:
                    # add one hypotool per sequence and chain part
                    for seq, onePartChainDict in zip(step.sequences, listOfChainDictsLegs):
                        seq.createHypoTools( onePartChainDict )

            step.createComboHypoTools(self.name) 


    def __repr__(self):
        return "-*- Chain %s -*- \n + Seeds: %s \n + Steps: \n %s \n"%(\
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
        # empty steps:
        if not len(self.step.sequences):
            self.decisions.extend(self.filter.getOutputList())
        else:
            if self.step.isCombo:
                self.decisions.extend(self.step.combo.getOutputList())
            else:
                for sequence in self.step.sequences:
                    sequence_outputs=sequence.getOutputList()
                    for output in sequence_outputs:
                        self.decisions.append(output)

        log.debug("CFSequence: set out decisions: %s", self.decisions)


    def connect(self, connections):
        """Connect filter to ChainStep (and all its sequences) through these connections (which are sets of filter outputs)
        if a ChainStep contains the same sequence multiple times (for multi-object chains),
        the filter is connected only once (to avoid multiple DH links)
        """
        log.debug("CFSequence: connect Filter %s with %d menuSequences of step %s, using %d connections", compName(self.filter.Alg), len(self.step.sequences), self.step.name, len(connections))
        if len(connections) == 0:
            log.error("ERROR, no filter outputs are set!")

        if len(self.step.sequences):
            # check whether the number of filter outputs are the same as the number of sequences in the step
            if len(connections) != len(self.step.sequences):
                log.error("Found %d connections and %d MenuSequences in Step %s", len(connections), len(self.step.sequences), self.step.name)

            nseq=0
            for seq in self.step.sequences:
                filter_out = connections[nseq]
                log.debug("CFSequence: Found input %s to sequence::%s from Filter::%s (from seed %s)", filter_out, seq.name, compName(self.filter.Alg), seq.seed)
                seq.connectToFilter( filter_out )
                nseq+=1
        else:
          log.debug("This CFSequence has no sequences: outputs are the Filter outputs, which are %d", len(self.decisions))


    def connectCombo(self):
        """ connect Combo to Hypos"""
        for seq in self.step.sequences:
            if type(seq.getOutputList()) is list:
               combo_input=seq.getOutputList()[-1] # last one?
            else:
               combo_input=seq.getOutputList()[0]
            self.step.combo.addInput(combo_input)
            log.debug("CFSequence.connectCombo: adding input to  %s: %s",  self.step.combo.Alg.getName(), combo_input)
            # inputs are the output decisions of the hypos of the sequences
            combo_output=CFNaming.comboHypoOutputName (self.step.combo.Alg.getName(), combo_input)
            self.step.combo.addOutput(combo_output)
            log.debug("CFSequence.connectCombo: adding output to  %s: %s",  self.step.combo.Alg.getName(), combo_output)


    def __repr__(self):
        return "--- CFSequence ---\n + Filter: %s \n + decisions: %s\n +  %s \n"%(\
                    self.filter.Alg.getName(), self.decisions, self.step)



class StepComponent(object):
    """ Class to build hte ChainStep, for including empty sequences"""
    def __init__(self, sequence, multiplicity,empty):
        self.sequence=sequence
        self.multiplicity=multiplicity
        self.empty=empty

class ChainStep(object):
    """Class to describe one step of a chain; if multiplicity is greater than 1, the step is combo/combined.  Set one multiplicity value per sequence"""
    def __init__(self, name,  Sequences=[], multiplicity=[1], chainDicts=[], comboHypoCfg=ComboHypoCfg, comboToolConfs=[]):

        # include cases of emtpy steps with multiplicity = [] or multiplicity=[0,0,0///]
        if sum(multiplicity)==0:
            multiplicity=[]

        # sanity check on inputs
        if len(Sequences) != len(multiplicity):
            raise RuntimeError("Tried to configure a ChainStep %s with %i Sequences and %i multiplicities. These lists must have the same size" % (name, len(Sequences), len(multiplicity)) )

        self.name = name
        self.sequences=Sequences
        self.multiplicity = multiplicity
        self.comboHypoCfg=comboHypoCfg
        self.comboToolConfs=comboToolConfs
        self.isCombo=sum(multiplicity)>1
        self.combo=None
        self.chainDicts = chainDicts
        if self.isCombo:
            self.makeCombo()

    def addComboHypoTools(self,  tools):
        self.comboToolConfs.append(tools)

    def makeCombo(self):
        if len(self.sequences)==0:
            return
        hashableMult = tuple(self.multiplicity)
        self.combo =  RecoFragmentsPool.retrieve(createComboAlg, None, name=CFNaming.comboHypoName(self.name), multiplicity=hashableMult, comboHypoCfg=self.comboHypoCfg)

    def createComboHypoTools(self, chainName):
        if self.isCombo:
            from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
            chainDict = TriggerConfigHLT.getChainDictFromChainName(chainName)
            self.combo.createComboHypoTools(chainDict, self.comboToolConfs)

    def getChainNames(self):
        if not self.isCombo:
            return [seq.getTools() for seq in self.sequences]
        else:
            return list(self.combo.getChains())
        
    def __repr__(self):
        if len(self.sequences) == 0:
            return "--- ChainStep %s ---\n is Empty, ChainDict = %s "%(self.name,  ' '.join(map(str, [dic['chainName'] for dic in self.chainDicts])) )
        if not self.isCombo:
            return "--- ChainStep %s ---\n , multiplicity = %d  ChainDict = %s \n + MenuSequences = %s "%(self.name,  sum(self.multiplicity), ' '.join(map(str, [dic['chainName'] for dic in self.chainDicts])), ' '.join(map(str, [seq.name for seq in self.sequences]) ))
        else:
            if self.combo:
                calg = self.combo.Alg.name()
            else:
                calg = 'NONE'
            return "--- ChainStep %s ---\n + isCombo, multiplicity = %d  ChainDict = %s \n + MenuSequences = %s  \n + ComboHypo = %s,  ComboHypoTools = %s" %\
                   (self.name,  sum(self.multiplicity),
                    ' '.join(map(str, [dic['chainName'] for dic in self.chainDicts])),
                    ' '.join(map(str, [seq.name for seq in self.sequences]) ),
                    calg,
                    ' '.join(map(str, [tool.__name__ for tool in self.comboToolConfs])))


def createComboAlg(dummyFlags, name, multiplicity, comboHypoCfg):
    return ComboMaker(name, multiplicity, comboHypoCfg)


# this is fragment for New JO


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
class InEventReco( ComponentAccumulator ):
    """ Class to handle in-event reco """
    def __init__(self, name, inputMaker=None):
        super( InEventReco, self ).__init__()
        self.name = name
        self.mainSeq = seqAND( name )
        self.addSequence( self.mainSeq )

        # Details below to be checked
        self.inputMakerAlg = inputMaker

        # Avoid registering a duplicate
        self.addEventAlgo( self.inputMakerAlg, self.mainSeq.name )
        self.recoSeq = parOR( "InputSeq_"+self.inputMakerAlg.name )
        self.addSequence( self.recoSeq, self.mainSeq.name )
    pass

    def mergeReco( self, ca ):
        """ Merged CA movnig reconstruction algorithms into the right sequence """
        return self.merge( ca, sequenceName=self.recoSeq.getName() )

    def addHypoAlg(self, alg):
        self.addEventAlgo( alg, self.mainSeq.name )

    def sequence( self ):
        return self.mainSeq

    def inputMaker( self ):
        return self.inputMakerAlg



class InViewReco( ComponentAccumulator ):
    """ Class to handle in-view reco, sets up the View maker if not provided and exposes InputMaker so that more inputs to it can be added in the process of assembling the menu """
    def __init__(self, name, viewMaker=None, roisKey=None):
        super( InViewReco, self ).__init__()
        self.name = name
        self.mainSeq = seqAND( name )
        self.addSequence( self.mainSeq )

        ViewCreatorInitialROITool=CompFactory.ViewCreatorInitialROITool

        if viewMaker:
            self.viewMakerAlg = viewMaker
        else:
            self.viewMakerAlg = CompFactory.EventViewCreatorAlgorithm("IM"+name,
                                                          ViewFallThrough = True,
                                                          RoIsLink        = 'initialRoI',
                                                          RoITool         = ViewCreatorInitialROITool(),
                                                          InViewRoIs      = roisKey if roisKey else name+'RoIs',
                                                          Views           = name+'Views',
                                                          ViewNodeName    = name+"InView")

        self.addEventAlgo( self.viewMakerAlg, self.mainSeq.name )
        self.viewsSeq = parOR( self.viewMakerAlg.ViewNodeName )
        self.addSequence( self.viewsSeq, self.mainSeq.name )

    def addInputFromFilter(self, filterAlg ):
        assert len(filterAlg.Output) == 1, "Can only oprate on filter algs with one configured output, use addInput to setup specific inputs"
        self.addInput( filterAlg.Output[0], "Reco_"+( filterAlg.Output[0].replace("Filtered_", "") ) )

    def addInput(self, inKey, outKey ):
        """Adds input (DecisionsContainer) from which the views should be created """
        self.viewMakerAlg.InputMakerInputDecisions += [ inKey ]
        self.viewMakerAlg.InputMakerOutputDecisions = outKey

    def mergeReco( self, ca ):
        """ Merged CA movnig reconstruction algorithms into the right sequence """
        return self.merge( ca, sequenceName=self.viewsSeq.getName() )

    def addHypoAlg(self, alg):
        self.addEventAlgo( alg, self.mainSeq.name )

    def sequence( self ):
        return self.mainSeq

    def inputMaker( self ):
        return self.viewMakerAlg


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
            return recoFragment
        else:
            return cls.fragments[requestHash]


def getChainStepName(chainName, stepNumber):
    return '{}_step{}'.format(chainName, stepNumber)

def createStepView(stepName):
    stepReco = parOR(CFNaming.stepRecoName(stepName))
    stepView = seqAND(CFNaming.stepViewName(stepName), [stepReco])
    return stepReco, stepView

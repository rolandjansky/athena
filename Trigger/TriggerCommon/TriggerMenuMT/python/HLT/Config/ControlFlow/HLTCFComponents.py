# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLT.Config.MenuComponents import AlgNode
from TriggerMenuMT.HLT.Config.ControlFlow.MenuComponentsNaming import CFNaming

from AthenaCommon.CFElements import compName

from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )

from AthenaConfiguration.ComponentFactory import CompFactory
RoRSeqFilter = CompFactory.RoRSeqFilter
PassFilter   = CompFactory.PassFilter


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

class SequenceFilterNode(AlgNode):
    """Node for any kind of sequence filter"""
    def __init__(self, Alg, inputProp, outputProp):
        AlgNode.__init__(self,  Alg, inputProp, outputProp)

    def addChain(self, name, input_name):
        return

    def getChains(self):
        return []

    def getChainsPerInput(self):
        return [[]]

    def __repr__(self):
        return "SequenceFilter::%s  [%s] -> [%s], chains=%s"%(compName(self.Alg),' '.join(map(str, self.getInputList())),' '.join(map(str, self.getOutputList())), self.getChains())


class RoRSequenceFilterNode(SequenceFilterNode):
    def __init__(self, name):
        Alg= RoRSeqFilter(name)
        SequenceFilterNode.__init__(self,  Alg, 'Input', 'Output')

    def addChain(self, name, input_name):
        input_index = self.readInputList().index(input_name)
        chains_in_input = self.getPar("ChainsPerInput")
        if len(chains_in_input) == input_index:
            chains_in_input.append([name])
        elif len(chains_in_input) > input_index:
            chains_in_input[input_index].append(name)
        else:
            log.error("Error: why requiring input %i when size is %i ?" , input_index , len(chains_in_input))
            raise RuntimeError("Error: why requiring input %i when size is %i " , input_index , len(chains_in_input))
            
        self.Alg.ChainsPerInput= chains_in_input
        return self.setPar("Chains", name) # still neded?
        
    def getChains(self):
        return self.getPar("Chains")

    def getChainsPerInput(self):
        return self.getPar("ChainsPerInput")


from AthenaCommon.AlgSequence import AthSequencer
class PassFilterNode(SequenceFilterNode):
    """ PassFilter is a Filter node without inputs/outputs, so OutputProp=InputProp=empty"""
    def __init__(self, name):        
        Alg=AthSequencer( "PassSequence" )
        Alg.IgnoreFilterPassed=True   # always pass     
        SequenceFilterNode.__init__(self,  Alg, '', '')
        

#########################################################
# CFSequence class
#########################################################
class CFSequence(object):
    """Class to describe the flow of decisions through ChainStep + filter with their connections (input, output)
    A Filter can have more than one input/output if used in different chains, so this class stores and manages all of them (when doing the connect)
    """
    def __init__(self, ChainStep, FilterAlg):
        self.filter = FilterAlg
        self.step = ChainStep
        self.connectCombo()
        self.setDecisions()
        log.debug("CFSequence.__init: created %s ",self)

    def setDecisions(self):
        """ Set the output decision of this CFSequence as the hypo outputdecision; In case of combo, takes the Combo outputs"""
        self.decisions=[]
        # empty steps:
        if self.step.combo is None:
            self.decisions.extend(self.filter.getOutputList())
        else:
            self.decisions.extend(self.step.combo.getOutputList())            

        log.debug("CFSequence: set out decisions: %s", self.decisions)


    def connect(self, connections):
        """Connect filter to ChainStep (and all its sequences) through these connections (which are sets of filter outputs)
        if a ChainStep contains the same sequence multiple times (for multi-object chains),
        the filter is connected only once (to avoid multiple DH links)
        """
        log.debug("CFSequence: connect Filter %s with %d menuSequences of step %s, using %d connections", compName(self.filter.Alg), len(self.step.sequences), self.step.name, len(connections))
        log.debug("   --- sequences: ")
        for seq in self.step.sequences:
            log.debug(seq)
        if len(connections) == 0:
            log.error("ERROR, no filter outputs are set!")

        if len(self.step.sequences):
            # check whether the number of filter outputs are the same as the number of sequences in the step
            if len(connections) != len(self.step.sequences):
                log.error("Found %d connections and %d MenuSequences in Step %s", len(connections), len(self.step.sequences), self.step.name)
                raise Exception("[CFSequence] Connections and sequences do not match, this must be fixed!")
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
        if self.step.combo is None:
            return

        for seq in self.step.sequences:            
            combo_input=seq.getOutputList()[0]
            self.step.combo.addInput(combo_input)
            inputs = self.step.combo.readInputList()
            legindex = inputs.index(combo_input)
            log.debug("CFSequence.connectCombo: adding input to  %s: %s",  self.step.combo.Alg.getName(), combo_input)
            # inputs are the output decisions of the hypos of the sequences
            combo_output=CFNaming.comboHypoOutputName (self.step.combo.Alg.getName(), legindex)            
            self.step.combo.addOutput(combo_output)
            log.debug("CFSequence.connectCombo: adding output to  %s: %s",  self.step.combo.Alg.getName(), combo_output)


    def __repr__(self):
        return "--- CFSequence ---\n + Filter: %s \n + decisions: %s\n +  %s \n"%(\
                    self.filter.Alg.getName(), self.decisions, self.step)




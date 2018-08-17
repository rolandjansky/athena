# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

#-----------------------------------------------------
# Hlt configuration base classes
#-----------------------------------------------------

#from AthenaCommon.Configurable import *
from TriggerJobOpts.TriggerFlags import *
from TriggerMenu.menu.ChainDef import ChainDef

log = logging.getLogger( 'TriggerMenu.menu.HltConfig.py' )

class HltSeq:
    def __init__(self, inputTEs, algos, outputTE, topo_start_from=None):
        self.inputTEs = inputTEs
        if type(inputTEs)==type(''): self.inputTEs = [inputTEs]
        self.algos = algos
        self.outputTE = outputTE
        self.topo_start_from = topo_start_from

class HltChainDef:
    """signatures: dictionary from step
    """
    def __init__(self, sig_id,
                 chain_name, chain_counter,                
                 prescale,
                 pass_through,
                 rerun, 
                 ):

        self.sig_id = "HLT_"+sig_id
        self.chain_name = chain_name
        self.chain_counter = chain_counter
        self.prescale = prescale
        self.pass_through = pass_through
        self.rerun = rerun

        self.signatures = []
        self.sequences = []
        self.sequencesPerSignature = {}
        self.combined_chains = [] # optional
        self.defineSequences()
        self.defineSignatures()
        self.defineCombinedChain()
        if len(self.combined_chains)>0: self.mergeCombinedChains()

    def getClassName(self):
        return self.__class__.__name__
        
    def defineSequences(self):
        self.sequences = []

    def defineSignatures(self):
        self.signatures = []

    def defineCombinedChain(self):
        self.combined_chains = []
        pass

    def addSequence(self, inputTEs, algos, outputTE, topo_start_from=None):
        self.sequences.append(HltSeq(inputTEs, algos, outputTE, topo_start_from))

    def addSignature(self, TElist=[]):
        self.signatures.append(TElist)
        
    def mergeCombinedChains(self):
        for (i, c) in enumerate(self.combined_chains):
            for seq in c.sequences:
                seq.inputTEs = map(lambda x: c.renameTE(x), seq.inputTEs)
                seq.outputTE = c.renameTE(seq.outputTE)
            self.sequences.extend(c.sequences)
        n = max(map(lambda x: len(x.signatures), self.combined_chains))
        for i in range(1, n+1):
            tes = []
            for c in self.combined_chains:
                if len(c.signatures)>=i:
                    for te in c.signatures[i-1]:
                        tes.append(c.renameTE(te))
            self.addSignature(tes)
        pass
    
    
    def generateChainDef(self):
        chainDef = ChainDef(chain_name=self.chain_name, lower_chain_name=self.lower_chain_name, level=self.level)
          
        log.debug("HltConfig: all sequences %s" % self.sequencesPerSignature)
        log.debug("HltConfig: all signatures %s" % self.signatures)
        log.debug("HltConfig: self.sequecnes %s" % self.sequences)
        for sequence in self.sequences:
            chainDef.addSequence(filter(lambda x: x!=None, sequence.algos), map(lambda x: self.renameTE(x), sequence.inputTEs), self.renameTE(sequence.outputTE))

        for (isig, sig) in enumerate(self.signatures):
            tes = []
            for te in sig:
                #print "HltConfig te",te
                out = self.renameTE(te)
                tes.append(out)                
            if len(tes)>0: 
                chainDef.addSignature(isig+1+self.signatureCounterOffset,tes)  #addHLTSignature(tes, isig+1)
        #print "HltConfig : DONE ==================="
        return chainDef




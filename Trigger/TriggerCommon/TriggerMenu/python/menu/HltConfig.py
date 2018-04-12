# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#-----------------------------------------------------
# Hlt configuration base classes
#-----------------------------------------------------

from TriggerMenu.menu.ChainDef import ChainDef
from AthenaCommon.Logging import logging  # loads logger

log = logging.getLogger( 'TriggerMenu.menu.HltConfig.py' )
# log.setLevel(logging.DEBUG)

class HltSeq:
    def __init__(self, inputTEs, algos, outputTE, topo_start_from=None):
        self.inputTEs = inputTEs
        if type(inputTEs)==type(''): self.inputTEs = [inputTEs]
        self.algos = algos
        self.outputTE = outputTE
        self.topo_start_from = topo_start_from

def mergeRemovingOverlap(x, y, r=''):
    if r=='':
        r = x[x.rfind('_'):]
        if y.startswith(r): return x.replace(r, '')+y
        else: return x+y
    else:
        return x.replace(r, '')+y

class HltChainDef:
    """signatures: dictionary from step
    """

    def __init__(self, sig_id,
                 chain_name, chain_counter, level,
                 lower_chain_name, inputTEs,
                 prescale,
                 pass_through,
                 rerun, 
                 signatureCounterOffset):


        self.sig_id = "HLT_"+sig_id
        self.chain_name = chain_name
        self.chain_counter = chain_counter
        self.level = level
        self.lower_chain_name = lower_chain_name
        self.prescale = prescale
        self.pass_through = pass_through
        self.rerun = rerun
        self.signatureCounterOffset = signatureCounterOffset
        #
        self.inputTEs = inputTEs
        # self.suffix = config.suffix#
        self.signatures = []
        self.sequences = []
        self.sequencesPerSignature = {}
        self.combined_chains = [] # optional
        #self.physics_streams = []
        #self.calib_streams = []
        #self.groups = []
        self.TErenamingMap = {}
        #
        self.defineSequences()
        self.defineSignatures()
        #self.defineStreamGroupTriggerType()
        self.defineTErenaming()
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

    #def defineStreamGroupTriggerType(self):
    #    self.physics_streams = []
    #    self.calib_streams = []
    #    self.groups = []
    #    self.trigger_type = []
        
    def defineTErenaming(self):
        self.TErenamingMap = {}

    def renameTE(self, te_name):
        try:
            return self.TErenamingMap[te_name]
        except:
            return te_name

    def addSequence(self, inputTEs, algos, outputTE, topo_start_from=None):
        self.sequences.append(HltSeq(inputTEs, algos, outputTE, topo_start_from))

    def addSignature(self, TElist=[]):
        self.signatures.append(TElist)
        
    def findSeqForOutputTE(self, te):
        # for a in self.sequences: print a.outputTE
        for x in self.sequences:
            if x.outputTE==te: return x
        return None

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
    
    # def generateSequences(self):
    #     from AthenaCommon.Configurable import ConfigurableAlgorithm

    #     def findSeqWithOutput(output, seqList):
    #         """Sequential search for sequence with given output"""
    #         for s in seqList:
    #             if s.output==output: return s
    #         return None

    #     def generate(te,sigID):
    #         seq = self.findSeqForOutputTE(te) #seq in HltConfig
    #         if seq==None: return
    #         for TE in seq.inputTEs: # descend downd the chain, if no input TE in sequence proceed
    #             generate(TE,sigID)
    #         out = self.renameTE(seq.outputTE)
    #         # print 'generate TE for ', te, ' renamed to ', out, '<-', seq.outputTE
    #         if out != '':
    #             # seqOut = findSeqWithOutput(out, triggerPythonConfig.theSeqLists) # is none if TPC.theSeqLists is empty or doesnt contain sequence with te out "out"
    #             # if seqOut!=None:
    #             #     algs0 = seqOut.algs #all algorithms associated to the sequence
    #             #     algs1 = []
    #             #     for a in seq.algos: 
    #             #         if issubclass(type(a), ConfigurableAlgorithm):
    #             #             algs1.append(a.getFullName())
    #             #         elif type(a) == type(''):
    #             #             algs1.append(a)
    #             #         else:
    #             #             log.warning('Not sure what this algorithm in TE=%s (All algo=%s) : %s' % \
    #             #                         (te, str(seq.algos), str(a)))
    #             #     if algs0 != algs1: # if sequences with same TEout name in HltConfig and TPC don't contain same algos throw a couple of warnings
    #             #         allOut = filter(lambda x: x.output==out, triggerPythonConfig.theSeqLists)
    #             #         log.warning('Sequence producing TE %s already exists with different algos (x%d)' % \
    #             #                     (out, len(allOut)))
    #             #         log.warning('  Algo1 = %s' % str(algs1))
    #             #         for (iii, xxx) in enumerate(allOut):
    #             #             log.warning('  Existing algos%d=%s' % (iii, str(xxx.algs)))
    #             #else:
    #             if len(seq.algos)==0:
    #                 mlog.info( 'TE %s has zero algorithms' % out)
    #             if len(seq.algos)!=0:
    #                 seq.algos = filter(lambda x: x!=None, seq.algos)
    #                 inTEs = map(lambda x: self.renameTE(x), seq.inputTEs)                        
    #                 self.sequencesPerSignature[sigID] += [[seq.algos, inTEs, out]]

        # if self.level=='L2' and TriggerFlags.doLVL2() or \
        #        self.level=='EF': # and TriggerFlags.doEF():
        #     for sig in self.signatures:
        #         if len(sig)==0: continue
        #         sigID = "_".join(sig)
        #         self.sequencesPerSignature[sigID] = []
        #         for te in sig:
        #             generate(te,sigID)

        # pass
    
    def generateChainDef(self):
        #chain = HLTChain(self.chain_name, str(self.chain_counter),
        #                 self.lower_chain_name, self.level, str(self.prescale), str(self.pass_through),
        #                 str(self.rerun))
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

class L2EFChainDef(HltChainDef):
    def __init__(self, sig_id, 
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter, 
                 l2_inputTEs, prescale=1, pass_through=0, rerun=-1, signatureCounterOffset=0):
        self.l2chain = HltChainDef(sig_id, l2_chain_name, l2_chain_counter, 'L2', l2_lower_chain_name, l2_inputTEs, prescale, pass_through, rerun, signatureCounterOffset)
        self.sig_id = sig_id
        self.l2_lower_chain_name = l2_lower_chain_name
        self.l2_chain_name = l2_chain_name
        self.ef_chain_name = ef_chain_name
        self.inputTEs = l2_inputTEs
        self.signatureCounterOffset = signatureCounterOffset
        #self.physics_streams = []
        #self.calib_streams = []
        #self.L2physics_streams = []
        #self.L2calib_streams = []
        #self.EFphysics_streams = []
        #self.EFcalib_streams = []
        #self.groups = []
        #self.trigger_type = []
        
        self.efchain = HltChainDef(sig_id, ef_chain_name, ef_chain_counter,'EF', l2_chain_name, [], prescale, pass_through, rerun, signatureCounterOffset )
        #
        #print 'define sequences'
        self.defineSequences()
        #print 'define signatures'
        #print 'self class', self.__class__.__name__
        self.defineSignatures()
        #print 'bootstrap'
        # bootstrap for EF chain
        ef_inputTEs = []
        if len(self.l2chain.signatures)>0:
            for te in self.l2chain.signatures[-1]:
                ef_inputTEs.append(self.renameTE(te))
        self.efchain.inputTEs = ef_inputTEs
        self.l2chain.sequences, self.l2chain.signatures = [], []
        self.efchain.sequences, self.efchain.signatures = [], []
        self.defineSequences()
        self.defineSignatures()
        #
        #self.defineStreamGroupTriggerType()
        self.defineTErenaming()
        # self.defineCombinedChain()
        self.setConfigToL2EF()
        pass

    def addL2Sequence(self, inputTEs, algos, outputTE, topo_start_from=None):
        self.l2chain.addSequence(inputTEs, algos, outputTE, topo_start_from)

    def addEFSequence(self, inputTEs, algos, outputTE, topo_start_from=None):
        self.efchain.addSequence(inputTEs, algos, outputTE, topo_start_from)
        
    def addL2Signature(self, tes):
        self.l2chain.addSignature(tes)

    def addEFSignature(self, tes):
        self.efchain.addSignature(tes)

    def setConfigToL2EF(self):
        def setConfToChain(chain, level):
            #chain.physics_streams = list(self.physics_streams)
            #chain.calib_streams = list(self.calib_streams)
            #if level=='L2':
            #    chain.physics_streams += self.L2physics_streams
            #    chain.calib_streams += self.L2calib_streams
            #elif level=='EF':
            #    chain.physics_streams += self.EFphysics_streams
            #    chain.calib_streams += self.EFcalib_streams
            #chain.groups = self.groups
            #chain.trigger_type = self.trigger_type
            chain.TErenamingMap = self.TErenamingMap
        setConfToChain(self.l2chain, 'L2')
        setConfToChain(self.efchain, 'EF')

    def generateChainDef(self):
        if self.l2chain and self.l2chain.chain_name!='':
            # L2 configuration must be there
            theChainDef = self.l2chain.generateChainDef()
        #if self.efchain and self.efchain.chain_name!='':
        #    theChainDef = self.efchain.generateChainDef()
            
        return theChainDef

    def generateHLTChainDef(self):
        if self.l2chain and self.l2chain.chain_name!='':
            # L2 configuration must be there
            theChainDefL2 = self.l2chain.generateChainDef()
        if self.efchain and self.efchain.chain_name!='':
             theChainDefEF = self.efchain.generateChainDef()        

        theChainDef = theChainDefL2
        theChainDef.chain_name = "HLT_"+self.sig_id
        theChainDef.level = "HLT"
        
        
        if len(theChainDefL2.signatureList) > 0:
            signatureCounter = theChainDefL2.signatureList[-1]["signature_counter"]
        else:
            signatureCounter = 0
        for signatureEF in theChainDefEF.signatureList:
            signatureCounter += 1
            signatureEF["signature_counter"] = signatureCounter
            theChainDef.signatureList.append(signatureEF)

        for sequenceEF in theChainDefEF.sequenceList:
            theChainDef.sequenceList.append(sequenceEF)
         
        return theChainDef


    def createL2Chain(self):
        pass
    def createEFChain(self):
        pass

def chainConfig(cls, config, inputTEs):
    c = HltChainDef('', '', 0, '', '', inputTEs, config)
    return c

def checkHypoType(configurable, class_name):
    if configurable==None:
        # bad, object is null
        pass
    elif configurable.getType() != class_name:
        # bad, type inconsistent
        pass
    else:
        return True
    return False

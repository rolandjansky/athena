# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#-----------------------------------------------------
# Hlt configuration base classes
#-----------------------------------------------------

from AthenaCommon.Configurable import *
from TriggerMenuPython.TriggerPythonConfig import *
from TriggerJobOpts.TriggerFlags import *

log = logging.getLogger( 'HltConfig.py' )
# log.setLevel(logging.DEBUG)

class HltSeq:
    def __init__(self, inputTEs, algos, outputTE):
        self.inputTEs = inputTEs
        if type(inputTEs)==type(''): self.inputTEs = [inputTEs]
        self.algos = algos
        self.outputTE = outputTE

_AllChains = {
    'ChainName': 'ChainObject',
    
    }

def findChain(chain_name):
    if chain_name in _AllChains:
        return _AllChains[chain_name]
    else:
        log.info( 'Chain %s not created yet' % chain_name)
        return None


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
                 lower_chain_name, inputTEs, config,
                 prescale,
                 pass_through,
                 rerun):
        self.sig_id = sig_id
        self.chain_name = chain_name
        self.chain_counter = chain_counter
        self.level = level
        self.lower_chain_name = lower_chain_name
        self.prescale = prescale
        self.pass_through = pass_through
        self.rerun = rerun
        #
        self.inputTEs = inputTEs
        # self.suffix = config.suffix
        self.config = config
        #
        self.signatures = []
        self.sequences = []
        self.combined_chains = [] # optional
        self.physics_streams = []
        self.calib_streams = []
        self.groups = []
        self.TErenamingMap = {}
        #
        self.defineSequences(config)
        self.defineSignatures()
        self.defineStreamGroupTriggerType()
        self.defineTErenaming()
        self.defineCombinedChain(config)
        if len(self.combined_chains)>0: self.mergeCombinedChains()
        global _AllChains
        if self.chain_name not in _AllChains.keys():
            _AllChains[self.chain_name] = self

    def getClassName(self):
        return self.__class__.__name__
        
    def defineSequences(self, config):
        self.sequences = []

    def defineSignatures(self):
        self.signatures = []

    def defineCombinedChain(self, combined_config):
        self.combined_chains = []
        pass

    def defineStreamGroupTriggerType(self):
        self.physics_streams = []
        self.calib_streams = []
        self.groups = []
        self.trigger_type = []
        
    def defineTErenaming(self):
        self.TErenamingMap = {}

    def renameTE(self, te_name):
        try:
            return self.TErenamingMap[te_name]
        except:
            return te_name

    def addSequence(self, inputTEs, algos, outputTE):
        self.sequences.append(HltSeq(inputTEs, algos, outputTE))

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
    
    def generateSequence(self, triggerPythonConfig):
        from AthenaCommon.Configurable import ConfigurableAlgorithm

        def findSeqWithOutput(output, seqList):
            """Sequential search for sequence with given output"""
            for s in seqList:
                if s.output==output: return s
            return None

        def generate(te):
            log = logging.getLogger( 'HltConfig.py' )
            seq = self.findSeqForOutputTE(te)
            if seq==None: return
            for TE in seq.inputTEs:
                generate(TE)
            out = self.renameTE(seq.outputTE)
            ##print 'generate TE for ', te, ' renamed to ', out, '<-', seq.outputTE
            if out != '':
                seqOut = findSeqWithOutput(out, triggerPythonConfig.theSeqLists)
                if seqOut!=None:
                    algs0 = seqOut.algs
                    algs1 = []
                    for a in seq.algos:
                       # print "(BETTA) type: ", type(a),a                        
                        if issubclass(type(a), ConfigurableAlgorithm):
                            algs1.append(a.getFullName())
                        elif type(a) == type(''):
                            algs1.append(a)
                        else:
                            log.warning('Not sure what this algorithm in TE=%s (All algo=%s) : %s' % \
                                        (te, str(seq.algos), str(a)))
                    if algs0 != algs1:
                        allOut = filter(lambda x: x.output==out, triggerPythonConfig.theSeqLists)
                        log.warning('Sequence producing TE %s already exists with different algos (x%d)' % \
                                    (out, len(allOut)))
                        log.warning('  Algo1 = %s' % str(algs1))
                        for (iii, xxx) in enumerate(allOut):
                            log.warning('  Existing algos%d=%s' % (iii, str(xxx.algs)))
                else:
                    if len(seq.algos)==0:
                        log.info( 'TE %s has zero algorithms' % out)
                    if len(seq.algos)!=0:
                        seq.algos = filter(lambda x: x!=None, seq.algos)
                        inTEs = map(lambda x: self.renameTE(x), seq.inputTEs)                        
                        triggerPythonConfig.addSequence(inTEs, seq.algos, out)
        if self.level=='L2' and TriggerFlags.doLVL2() or \
               self.level=='EF': # and TriggerFlags.doEF():
            for sig in self.signatures:
                if len(sig)==0: continue
                for te in sig:
                    generate(te)
        pass
    
    def generateMenu(self, triggerPythonConfig):
        chain = HLTChain(self.chain_name, str(self.chain_counter),
                         self.lower_chain_name, self.level, str(self.prescale), str(self.pass_through),
                         str(self.rerun))
        for s in self.physics_streams:
            chain.addStreamTag(s, 'physics')
        for s in self.calib_streams:
            chain.addStreamTag(s, 'calibration')
        chain.addGroup(self.groups)
        self.generateSequence(triggerPythonConfig)
        for (isig, sig) in enumerate(self.signatures):
            tes = []
            for te in sig:
                out = self.renameTE(te)
                tes.append(out)
            if len(tes)>0: chain.addHLTSignature(tes, isig+1)
        triggerPythonConfig.registerHLTChain(self.sig_id, chain)
        # print 'Registering chain %s with sig %s' % (chain.chain_name, self.sig_id)
        pass

class L2EFChainDef(HltChainDef):
    def __init__(self, sig_id, 
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter, 
                 l2_inputTEs, config, prescale=1, pass_through=0, rerun=-1):
        self.l2chain = HltChainDef(sig_id, l2_chain_name, l2_chain_counter,
                                   'L2', l2_lower_chain_name, l2_inputTEs,
                                   config.L2Config, prescale, pass_through, rerun)

        self.sig_id = sig_id
        self.l2_lower_chain_name = l2_lower_chain_name
        self.l2_chain_name = l2_chain_name
        self.ef_chain_name = ef_chain_name
        self.inputTEs = l2_inputTEs
        self.config = config
        self.physics_streams = []
        self.calib_streams = []
        self.L2physics_streams = []
        self.L2calib_streams = []
        self.EFphysics_streams = []
        self.EFcalib_streams = []
        self.groups = []
        self.trigger_type = []
        
##         print 'l2 sig: ', self.l2chain.signatures
##         print 'l2 seq: ', self.l2chain.sequences
        self.efchain = HltChainDef(sig_id, ef_chain_name, ef_chain_counter,
                                   'EF', l2_chain_name, [],
                                   config.EFConfig, prescale, pass_through, rerun)
        #
        #print 'define sequences'
        self.defineSequences(config)
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
        self.defineSequences(config)
        self.defineSignatures()
        #
        self.defineStreamGroupTriggerType()
        self.defineTErenaming()
        # self.defineCombinedChain()
        self.setConfigToL2EF()
        pass

    def addL2Sequence(self, inputTEs, algos, outputTE):
        self.l2chain.addSequence(inputTEs, algos, outputTE)

    def addEFSequence(self, inputTEs, algos, outputTE):
        self.efchain.addSequence(inputTEs, algos, outputTE)
        
    def addL2Signature(self, tes):
        self.l2chain.addSignature(tes)
    def addEFSignature(self, tes):
        self.efchain.addSignature(tes)

    def setConfigToL2EF(self):
        def setConfToChain(chain, level):
            chain.physics_streams = list(self.physics_streams)
            chain.calib_streams = list(self.calib_streams)
            if level=='L2':
                chain.physics_streams += self.L2physics_streams
                chain.calib_streams += self.L2calib_streams
            elif level=='EF':
                chain.physics_streams += self.EFphysics_streams
                chain.calib_streams += self.EFcalib_streams
            chain.groups = self.groups
            chain.trigger_type = self.trigger_type
            chain.TErenamingMap = self.TErenamingMap
        setConfToChain(self.l2chain, 'L2')
        setConfToChain(self.efchain, 'EF')

    def generateMenu(self, triggerPythonConfig):
        if self.l2chain and self.l2chain.chain_name!='':
            # L2 configuration must be there
            self.l2chain.generateMenu(triggerPythonConfig)
        if self.efchain and self.efchain.chain_name!='':
            self.efchain.generateMenu(triggerPythonConfig)

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

del log

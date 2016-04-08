# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerPythonConfig import HLTChain, TriggerPythonConfig

# here be dragons 
def __impl__replace_if_default(self, var, replacement):
    assert hasattr(self, var),  "%s has no attribute '%s'" % (self.__class__,var)
    if getattr(self.__class__, var) == getattr(self, var): setattr(self, var, replacement)
    return self

def __impl__warn_if_default(self, var, hint):
    assert getattr(self.__class__, var) != getattr(self, var), "impossible to derive value of property '%s' for '%s', hint: %s "% (var, self.__class__, hint)
    return self



def FixedClassFactory(className,argMap):
    """
    Create class with fixed attribute set.
    I.e. if it is attemted to be initialized with other attributes than allowed it will
    complain. In addition the default values made at the creation are available.
    """
    tmpClass = type(className,(),argMap)
    def init(self,**args):
        for     tmpKey,tmpVal in args.iteritems():
            if hasattr(self,tmpKey):
                setattr(self,tmpKey,tmpVal)
            else:
                raise AttributeError,"%s has no attribute '%s'" \
                      % (self.__class__,tmpKey)

    tmpClass.__init__ = init
    tmpClass.replace_if_default = __impl__replace_if_default
    tmpClass.warn_if_default    = __impl__warn_if_default
    return tmpClass

def ExtendableClassFactory(className,argMap):
    """
    Create class with extendable attribute set.
    I.e. if it is attempted to be initialized with other attributes than allowed WONT
    complain. In addition the default values are set to be None, so it is clear they were the dynamicaly added.
    """    
    tmpClass = type(className,(),argMap)
    def init(self,**args):
        for tmpKey,tmpVal in args.iteritems():
            if hasattr(self,tmpKey):
                setattr(self,tmpKey,tmpVal)
            else:
                setattr(self,tmpKey,tmpVal)

    def __update(self, dict):
        for k,v in dict.iteritems():
            assert k not in self.__dict__, "The object with the key %s is already in the store: %s" % (k, self.__dict__[k].__class__.__name__)                
            setattr(self, k, v)
        return self
   
    tmpClass.__init__ = init
    tmpClass.replace_if_default = __impl__replace_if_default
    tmpClass.warn_if_default    = __impl__warn_if_default
    tmpClass.update             = __update
    return tmpClass


Chain = FixedClassFactory('Chain', {'counter':-1, 'name':None, 'seed':None})

DeduceChainConfig = Chain()

FreeStore = ExtendableClassFactory('FreeStore', {})


        
class ChainTemplate:
    def __init__(self, signature, l2, ef):
        """
        Construct basic chains (L2 and EF) configuration. As much as possible is deduced from two objects (l2&ef).        
        """

        self.sig_id = signature
        self.__last_te = ''
        
        # deduction of chain names and seeds,counters, basic checks
        assert l2 != None or ef != None, "Can not construct signature %s with no chains at L2 and EF" % signature
        
        if l2:
            l2.replace_if_default('name', 'L2_%s' % signature)
            l2.warn_if_default('seed', 'The chain %s should have L1 seeding item defined.' % l2.name )
            l2.warn_if_default('counter', 'The chain at L2 should have counter')
        if ef:
            if ef == DeduceChainConfig:
                ef = Chain()
            if l2:                
                ef.replace_if_default('counter', l2.counter).replace_if_default('name', 'EF_%s' % l2.name.replace('L2_', '', 1)).replace_if_default('seed', l2.name)
            else:
                ef.replace_if_default('name', 'EF_%s' % signature).warn_if_default('counter', 'L2 chain is undefined so chain counter must be given at EF')


        # chain objects creation and insertion into menu
        if l2:
            self.l2 = HLTChain(chain_name=l2.name, chain_counter=str(l2.counter), lower_chain_name=l2.seed, level='L2')
            TriggerPythonConfig.currentTriggerConfig().registerHLTChain(signature, self.l2)
        else:
            self.l2 = None
            
        if ef:
            self.ef = HLTChain(chain_name=ef.name, chain_counter=str(ef.counter), lower_chain_name=ef.seed, level='EF')
            TriggerPythonConfig.currentTriggerConfig().registerHLTChain(signature, self.ef)
        else:
            self.ef = None

            
    def addSequence(self, input, algorithms, output):
        if output not in TriggerPythonConfig.currentTriggerConfig().seqDict().iterkeys():
            TriggerPythonConfig.currentTriggerConfig().addSequence(input, algorithms, output)
        self.__last_te = output
        return self

    def continueSequence(self, algorithms, output):
        if output not in TriggerPythonConfig.currentTriggerConfig().seqDict().iterkeys():
            TriggerPythonConfig.currentTriggerConfig().addSequence(self.__last_te, algorithms, output)
        self.__last_te = output
        return self    

    def addSignature(self, level=None, mult=1, sigcounter=-1):
        if not level: # try to deduce level form the name  
            if self.__last_te.startswith('L2_'):
                level = 'L2'
            elif self.__last_te.startswith('EF_'):
                level = 'EF'
            else:
                assert False, 'The trigger level can not be deduced from the: %s' % self.__last_te
                
        if level == 'L2':
            if sigcounter>0:
                self.l2.addHLTSignature(self.__last_te if mult==1 else [self.__last_te]*mult, sigcounter=sigcounter)
            else:
                self.l2.addHLTSignature(self.__last_te if mult==1 else [self.__last_te]*mult)
        else:
            if sigcounter>0:
                self.ef.addHLTSignature(self.__last_te if mult==1 else [self.__last_te]*mult, sigcounter=sigcounter)
            else:
                self.ef.addHLTSignature(self.__last_te if mult==1 else [self.__last_te]*mult)
        return self

    def addStreams(self, *args):
        if self.l2:
            [self.l2.addStreamTag(s) for s in args]
        if self.ef:
            [self.ef.addStreamTag(s) for s in args]
        return self

    def addGroups(self, *args):
        if self.l2:
            self.l2.groups = list(set(self.l2.groups).union(args))
            #[self.l2.addGroup(g) for g in args]
        if self.ef:
            self.ef.groups = list(set(self.ef.groups).union(args))
            #[self.ef.addGroup(g) for g in args]
        return self

    def removeGroups(self, *args):
        if self.l2:
            self.l2.groups = list(set(self.l2.groups)-set(args))
        if self.ef:
            self.ef.groups = list(set(self.ef.groups)-set(args))

        return self
    
    def merge(self, efnames=None, l2names=None, onlyEF=False):
        """
        Merge chains into composite one. I.e. EF_j10 and 2 x EF_j20 into EF_2j20_j10.

        
        Normally the EF chain names are given and rest is figured out by the code.
        But this can be also done only for L2. Or only for EF.
        """
        tpc = TriggerPythonConfig.currentTriggerConfig()
        def find_ingredients(names):
            chains = [ tpc.getHLTChain(i) for i in names ]
            if not all(chains):
                missing = list(set(names)-set([ch.chain_name for ch in chains if ch ]))
                raise AttributeError, 'When merging chains: %s these were missing: %s' %\
                      (str(names), str(missing))            
            return chains
            
        if efnames != None:
            ef_chains = find_ingredients(efnames)
            self.ef.mergeAndAppendChains(ef_chains)
            
            if not onlyEF:
                l2names = [ch.lower_chain_name for ch in ef_chains if ch.lower_chain_name != '' ]
            
        if l2names != None:
            l2_chains = find_ingredients(l2names)
            self.l2.mergeAndAppendChains(l2_chains)
        return self

    
    # this is fake, and once fully migrated to this system should be removed
    def generateMenu(self, tpc):
        pass


class CombinedChainTemplate(ChainTemplate):
    def __init__(self, signature, l2, ef=DeduceChainConfig,
                 l2merge=None, efmerge=None,
                 streams=[], groups=[]):
        ChainTemplate.__init__(self, signature, l2, ef)
        self.merge(efmerge, l2merge)
        for x in streams:
            self.addStreams(x)
        for x in groups:
            self.addGroups(x)
        
# test        
if __name__ == '__main__': # selftest
    s1 = FreeStore(menu='Physics', responsible='expert')
    print dir(s1)
    s2 = FreeStore(chain="L2_mu6", counter=345, seed="L1_MU6")
    print dir(s2)
    
    s3 = FreeStore(l2=FreeStore(mufast='mufast_hypo', mucomb='mucomb_hypo'),
               ef=FreeStore(msonly='MSOnlyHypo', combined='FullTrack_hypo'))
    #print s3
    s4 = FreeStore().update({'a': 4, 'b': 'another value'})
    print s4

    class MuonChain(ChainTemplate):
        def __init__(self, sig, l2, ef, fexes, hypos):
            ChainTemplate.__init__(self, sig, l2, ef)
            # actual chain making goes here
            



    fexes = FreeStore(mufast='mufast', mucomb='mucomb', seg='segment_finder', standalone='standalone', combined='combinedTrackMaker' )
    print fexes
    l2hypos = FreeStore(fast4='mufashypo_4GeV', comb4='mucomb_4GeV')
    mu0 = MuonChain(sig='mu4', l2=Chain(counter=234,seed='L2_MU0'),
                    ef=Chain(),
                    fexes=fexes,
                    hypos=FreeStore(mufast=l2hypos.fast4, mucomb=l2hypos.comb4))
    
    efonly = ChainTemplate('mu_nseeded', l2=None, ef=Chain(counter=456, seed=''))

    l2only = ChainTemplate('calib',  l2=Chain(counter=456, seed=''), ef=None).addStreams('calbration')

    

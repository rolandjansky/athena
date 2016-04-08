# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging        

from TriggerMenuPython.TriggerPythonConfig import *
from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags
from TriggerMenuPython.Combined import CombinedChain2
from TriggerMenuPython.ChainTemplate import *

from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Jet, TrigSiTrack_Muon
from TrigIDSCAN.TrigIDSCAN_Config   import TrigIDSCAN_Jet

from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxConfig import L2HistoPrmVtx_SiTrack
from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxConfig import L2HistoPrmVtx_IDScan

from TrigBjetHypo.TrigBjetFexConfig  import *
from TrigBjetHypo.TrigBjetHypoConfig import *

from TrigBjetHypo.TrigLeptonJetFexConfig  import L2LeptonJetFex
from TrigBjetHypo.TrigLeptonJetHypoConfig import L2LeptonJetHypo

from TrigmuComb.TrigmuCombConfig import *
from TrigmuFast.TrigmuFastConfig import *

from InDetTrigRecExample.EFInDetConfig import *
from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxConfig import EFHistoPrmVtx_Jet

from TrigBjetHypo.TrigLeptonJetFexConfig  import EFLeptonJetFex
from TrigBjetHypo.TrigLeptonJetHypoConfig import EFLeptonJetHypo

bjet_groups = ['RATE:MultiBjet','BW:Bjets']

class MultiBjet :
    def __init__(self, sig_id):

        mlog = logging.getLogger( 'MultiBjet.py' )
        self.sig_id = sig_id
        
        self.__supported = [
            #lower threshold for 1E30
            '2b10_3L1J10','2b10_4L1J5',
            '3b10_4L1J10','3b10_4L1J5',
            # new naming scheme
            '2b15_3L1J15', '3b15_L1J15',
            '2b15_3b10_4L1J10',
            ]

        if sig_id not in self.__supported:
            mlog.error(sig_id+" not supported "+str(self.__supported))
            raise Exception("ERROR: "+sig_id+" not supported "+str(self.__supported) )
        
        self.l2_ingredients = {
            '2b10_3L1J10' : [ 'L2_b10', 'L2_b10'],
            '2b10_4L1J5'  : [ 'L2_b10', 'L2_b10'],
             '3b10_4L1J10' : [ 'L2_b10', 'L2_b10', 'L2_b10'],
            '3b10_4L1J5'  : [ 'L2_b10', 'L2_b10', 'L2_b10'],
            '2b15_3L1J15' : [ 'L2_b15', 'L2_b15'],
            '2b15_3b10_4L1J10' : [ 'L2_b15', 'L2_b15', 'L2_b10'],
            }

        self.l2_chains = {
            '2b10_3L1J10' : HLTChain(chain_name='L2_2b10_3L1J10',chain_counter='1008',lower_chain_name='L1_3J10',level='L2',prescale='1',pass_through='1'),
            '2b10_4L1J5'  : HLTChain(chain_name='L2_2b10_4L1J5',chain_counter='1009',lower_chain_name='L1_4J5',level='L2',prescale='1',pass_through='1'),
            '3b10_4L1J5'  : HLTChain(chain_name='L2_3b10_4L1J5',chain_counter='1010',lower_chain_name='L1_4J5',level='L2',prescale='1',pass_through='1'),
            '3b10_4L1J10' : HLTChain(chain_name='L2_3b10_4L1J10',chain_counter='1011',lower_chain_name='L1_4J10',level='L2',prescale='1',pass_through='1'),
            '2b15_3L1J15' : HLTChain(chain_name='L2_2b15_3L1J15',chain_counter='667',lower_chain_name='L1_3J15',level='L2',prescale='1',pass_through='1'),
            '2b15_3b10_4L1J10' : HLTChain(chain_name='L2_2b15_3b10_4L1J10',chain_counter='258',lower_chain_name='L1_4J10',level='L2',prescale='1',pass_through='1'),
            
            }

        self.ef_ingredients = {
            '2b10_3L1J10' : [ 'EF_b10', 'EF_b10'],
            '2b10_4L1J5'  : [ 'EF_b10', 'EF_b10'],
            '3b10_4L1J10' : [ 'EF_b10', 'EF_b10', 'EF_b10'],
            '3b10_4L1J5'  : [ 'EF_b10', 'EF_b10', 'EF_b10'],
            '2b15_3L1J15' : [ 'EF_b15', 'EF_b15'],
            '2b15_3b10_4L1J10' : [ 'EF_b15', 'EF_b15', 'EF_b10'],
            }

        self.ef_chains = {
            '2b10_3L1J10' : HLTChain(chain_name='EF_2b10_3L1J10',chain_counter='1008',lower_chain_name='L2_2b10_3L1J10',level='EF',prescale='1',pass_through='1'),
            '2b10_4L1J5'  : HLTChain(chain_name='EF_2b10_4L1J5',chain_counter='1009',lower_chain_name='L2_2b10_4L1J5',level='EF',prescale='1',pass_through='1'),
            '3b10_4L1J5'  : HLTChain(chain_name='EF_3b10_4L1J5',chain_counter='1010',lower_chain_name='L2_3b10_4L1J5',level='EF',prescale='1',pass_through='1'),
            '3b10_4L1J10' : HLTChain(chain_name='EF_3b10_4L1J10',chain_counter='1011',lower_chain_name='L2_3b10_4L1J10',level='EF',prescale='1',pass_through='1'),
            '2b15_3L1J15' : HLTChain(chain_name='EF_2b15_3L1J15',chain_counter='667',lower_chain_name='L2_2b15_3L1J15',level='EF',prescale='1',pass_through='1'),
            '2b15_3b10_4L1J10' : HLTChain(chain_name='EF_2b15_3b10_4L1J10',chain_counter='258',lower_chain_name='L2_2b15_3b10_4L1J10',level='EF',prescale='1',pass_through='1'),
            }
            

    def generateMenu(self, triggerPythonConfig) :


        l2parts = [triggerPythonConfig.getHLTChain(i) for i in self.l2_ingredients[self.sig_id]] 
        efparts = [triggerPythonConfig.getHLTChain(i) for i in self.ef_ingredients[self.sig_id]]

        l2chain = self.l2_chains[self.sig_id]
        l2chain.addStreamTag('jets')  
        l2chain.addGroup(bjet_groups)


        if TriggerFlags.doLVL2():
            l2chain.mergeAndAppendChains(l2parts)

        efchain = self.ef_chains[self.sig_id]
        efchain.addStreamTag('jets')
        efchain.addGroup(bjet_groups)

        if TriggerFlags.doEF():
            efchain.mergeAndAppendChains(efparts)         

        triggerPythonConfig.registerHLTChain(self.sig_id, l2chain) 
        triggerPythonConfig.registerHLTChain(self.sig_id, efchain)


#bjet_groups = ['MultiBjet']





class MergedChain(ChainTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, tomerge=None, OnlyEF=False, groups=None):
        if ef == DeduceChainConfig:
            ef = Chain()
        ef.replace_if_default('name', 'EF_%s'%sig)
        ChainTemplate.__init__(self, sig, l2, ef)        

        self.tomerge = tomerge
        self.OnlyEF = OnlyEF

	#"New" style chains go straight to the TrigPythonConfig
        #"Old" style chains only fill the TPC filled at the last stage
        #To be able to merge the two, first try the merge during init
        #If this fails, try again during menu generation
        try:
            self.merge(self.tomerge, onlyEF=self.OnlyEF)
            self.seq_generated = True
        except AttributeError:
            self.seq_generated = False

        if groups is not None:
            for x in groups: self.addGroups(x)
        else:
            for x in ['RATE:MultiBjet','BW:Bjets']: self.addGroups(x)
        self.addStreams('jets')
        
    def generateMenu(self, tpc):
        if not self.seq_generated:
            self.merge(self.tomerge, onlyEF=self.OnlyEF)
            self.seq_generated = True


####################################################################
#!!!! v2 and v3 menu definitions!!!!
####################################################################
if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus()\
    or TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus()\
    or TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus()):
##    or TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus()):

    MultiBjets = [
        
        MultiBjet('2b10_3L1J10'),
        MultiBjet('2b10_4L1J5'),
        MultiBjet('3b10_4L1J10'),
        MultiBjet('3b10_4L1J5'),
        MultiBjet('2b15_3L1J15'),
        MultiBjet('2b15_3b10_4L1J10'),
    
    
        # 3b 4L1J loose
        CombinedChainTemplate('3b10_loose_4L1J10',
                              l2=Chain(name='L2_3b10_loose_4L1J10', counter=153, seed='L1_4J10'),
                              efmerge=['EF_b10_loose']*3, 
                              streams=['jets'], groups=['RATE:MultiBjet','BW:Bjets']),    
        
        # 2b medium
        CombinedChainTemplate('2b10_medium_3L1J10',
                              l2=Chain(name='L2_2b10_medium_3L1J10', counter=157, seed='L1_3J10'),
                              efmerge=['EF_b10_medium']*2,
                              streams=['jets'], groups=['RATE:MultiBjet','BW:Bjets']),
        
        #E.P.
        CombinedChainTemplate('2b15_medium_3L1J15',
                              l2=Chain(name='L2_2b15_medium_3L1J15', counter=667, seed='L1_3J15'),
                              efmerge=['EF_b15_medium']*2,
                              streams=['jets'], groups=['RATE:MultiBjet','BW:Bjets']),
        
        #E.P.
        CombinedChainTemplate('2b15_medium_2L1J10',
                              l2=Chain(name='L2_2b15_medium_2L1J10', counter=674, seed='L1_2J10'),
                              efmerge=['EF_b15_medium']*2,
                              streams=['jets'], groups=['RATE:MultiBjet','BW:Bjets']),
        
        
        #E.P.
        CombinedChainTemplate('2b15_medium_4L1J15',
                              l2=Chain(name='L2_2b15_medium_4L1J15', counter=672, seed='L1_4J15'),
                              efmerge=['EF_b15_medium']*2,
                              streams=['jets'], groups=['RATE:MultiBjet','BW:Bjets']),
        
        CombinedChainTemplate('2b20_medium_3L1J20',
                              l2=Chain(name='L2_2b20_medium_3L1J20', counter=963, seed='L1_3J20'),
                              efmerge=['EF_b20_medium']*2,
                              streams=['jets'], groups=['RATE:MultiBjet','BW:Bjets']),
        
        CombinedChainTemplate('2b20_medium_L1_2J15J50',
                              l2=Chain(name='L2_2b20_medium_L1_2J15J50', counter=964, seed='L1_2J15_J50'),
                              efmerge=['EF_b20_medium']*2,
                              streams=['jets'], groups=['RATE:MultiBjet','BW:Bjets']),
        
        CombinedChainTemplate('2b10_medium_4L1J10',
                              l2=Chain(name='L2_2b10_medium_4L1J10', counter=142, seed='L1_4J10'),
                              efmerge=['EF_b10_medium']*2,
                              streams=['jets'], groups=['RATE:MultiBjet','BW:Bjets']),
        
        #   CombinedChainTemplate('2b25_medium_4L1J10',
        #                         l2=Chain(name='L2_2b25_medium_4L1J10', counter=238, seed='L1_4J10'),
        #                         efmerge=['EF_b25_medium']*2,
        #                         streams=['jets'], groups=['RATE:MultiBjet','BW:Bjets']),
        
        
        CombinedChainTemplate('2b10_tight_4L1J10',
                              l2=Chain(name='L2_2b10_tight_4L1J10', counter=143, seed='L1_4J10'),
                              efmerge=['EF_b10_tight']*2,
                              streams=['jets'], groups=['RATE:MultiBjet','BW:Bjets']),
        
        CombinedChainTemplate('2b10_medium_L1JE100',
                              l2=Chain(name='L2_2b10_medium_L1JE100', counter=535, seed='L1_JE100'),
                              efmerge=['EF_b10_medium']*2, 
                              streams=['jets'], groups=['RATE:MultiBjet','BW:Bjets']),
        
        CombinedChainTemplate('2b10_medium_L1JE140',
                              l2=Chain(name='L2_2b10_medium_L1JE140', counter=536, seed='L1_JE140'),
                              efmerge=['EF_b10_medium']*2, 
                              streams=['jets'], groups=['RATE:MultiBjet','BW:Bjets']),
        
        CombinedChainTemplate('2b10_medium_L1_2J10J50',
                              l2=Chain(name='L2_2b10_medium_L1_2J10J50', counter=537, seed='L1_2J10_J50'),
                              efmerge=['EF_b10_medium']*2, 
                              streams=['jets'], groups=['RATE:MultiBjet','BW:Bjets']),
        
        CombinedChainTemplate('3b15_loose_4L1J15',
                              l2=Chain(name='L2_3b15_loose_4L1J15', counter=668, seed='L1_4J15'),
                              efmerge=['EF_b15_loose']*3, 
                              streams=['jets'], groups=['RATE:MultiBjet','BW:Bjets']),
        
        # 3b medium
        CombinedChainTemplate('3b10_medium_4L1J10',
                              l2=Chain(name='L2_3b10_medium_4L1J10', counter=1011, seed='L1_4J10'),
                              efmerge=['EF_b10_medium']*3, 
                              streams=['jets'], groups=['RATE:MultiBjet','BW:Bjets']),
        
        CombinedChainTemplate('3b15_medium_4L1J15',
                              l2=Chain(name='L2_3b15_medium_4L1J15', counter=668, seed='L1_4J15'),
                              efmerge=['EF_b15_medium']*3, 
                              streams=['jets'], groups=['RATE:MultiBjet','BW:Bjets']),
        
        ]

    MultiBjets += [
        MergedChain('2b10_medium_4j30_a4tc_EFFS', l2=Chain(name='L2_2b10_medium_4j25', counter=863, seed='L1_4J10'),
                    ef=Chain(name='EF_2b10_medium_4j30_a4tc_EFFS', counter=863, seed='L2_2b10_medium_4j25')).\
        merge(l2names=['L2_4j25']).merge(l2names=['L2_2b10_medium_4L1J10']).merge(['EF_2b10_medium_4L1J10', 'EF_4j30_a4tc_EFFS'],onlyEF=True),
        
        
        MergedChain('2b10_tight_4j30_a4tc_EFFS', l2=Chain(name='L2_2b10_tight_4j25', counter=1291, seed='L1_4J10'),
                    ef=Chain(name='EF_2b10_tight_4j30_a4tc_EFFS', counter=1291, seed='L2_2b10_tight_4j25')).\
        merge(l2names=['L2_4j25']).merge(l2names=['L2_2b10_tight_4L1J10']).merge(['EF_2b10_tight_4L1J10', 'EF_4j30_a4tc_EFFS'],onlyEF=True),
        
        MergedChain('3b10_medium_4j30_a4tc_EFFS', l2=Chain(name='L2_3b10_medium_4j25', counter=1280, seed='L1_4J10'),
                    ef=Chain(name='EF_3b10_medium_4j30_a4tc_EFFS', counter=1280, seed='L2_3b10_medium_4j25')).\
        merge(l2names=['L2_4j25']).merge(l2names=['L2_3b10_medium_4L1J10']).merge(['EF_3b10_medium_4L1J10', 'EF_4j30_a4tc_EFFS'],onlyEF=True),
        
        MergedChain('2b20_medium_3j45_a4tc_EFFS', l2=Chain(name='L2_2b20_medium_3j40', counter=1281, seed='L1_3J20'),
                    ef=Chain(name='EF_2b20_medium_3j45_a4tc_EFFS', counter=1281, seed='L2_2b20_medium_3j40')).\
        merge(l2names=['L2_3j40']).merge(l2names=['L2_2b20_medium_3L1J20']).merge(['EF_2b20_medium_3L1J20', 'EF_3j45_a4tc_EFFS'],onlyEF=True),
        
        MergedChain('3b15_medium_4j40_a4tc_EFFS', l2=Chain(name='L2_3b15_medium_4j35', counter=1282, seed='L1_4J15'),
                    ef=Chain(name='EF_3b15_medium_4j40_a4tc_EFFS', counter=1282, seed='L2_3b15_medium_4j35')).\
        merge(l2names=['L2_4j35']).merge(l2names=['L2_3b15_medium_4L1J15']).merge(['EF_3b15_medium_4L1J15', 'EF_4j40_a4tc_EFFS'],onlyEF=True),
        
        MergedChain('2b15_medium_3j40_a4tc_EFFS', l2=Chain(name='L2_2b15_medium_3j35', counter=1283, seed='L1_3J15'),
                    ef=Chain(name='EF_2b15_medium_3j40_a4tc_EFFS', counter=1283, seed='L2_2b15_medium_3j35')).\
        merge(l2names=['L2_3j35']).merge(l2names=['L2_2b15_medium_3L1J15']).merge(['EF_2b15_medium_3L1J15', 'EF_3j40_a4tc_EFFS'],onlyEF=True),
        
        #E.P.
        MergedChain('2b15_medium_4j45_a4tc_EFFS', l2=Chain(name='L2_2b15_medium_4j40_4L1J15', counter=673, seed='L1_4J15'),
                    ef=Chain(name='EF_2b15_medium_4j45_a4tc_EFFS', counter=673, seed='L2_2b15_medium_4j40_4L1J15')).\
        merge(l2names=['L2_4j40_4L1J15']).merge(l2names=['L2_2b15_medium_4L1J15']).merge(['EF_2b15_medium_4L1J15', 'EF_4j45_a4tc_EFFS'],onlyEF=True),
        
        
        MergedChain('2b10_medium_j75_j30_a4tc_EFFS', l2=Chain(name='L2_2b10_medium_j70_j25', counter=330, seed='L1_2J10_J50'),
                    ef=Chain(name='EF_2b10_medium_j75_j30_a4tc_EFFS', counter=330, seed='L2_2b10_medium_j70_j25')).\
        merge(l2names=['L2_j70_2j25']).merge(l2names=['L2_2b10_medium_L1_2J10J50']).merge(['EF_2b10_medium_L1_2J10J50', 'EF_j75_j30_a4tc_EFFS'],onlyEF=True),
        
        
        #Check this..
        MergedChain('2b10_medium_j75_2j30_a4tc_EFFS', l2=Chain(name='L2_2b10_medium_j70_2j25', counter=9, seed='L1_3J10_J50'),
                    ef=Chain(name='EF_2b10_medium_j75_2j30_a4tc_EFFS', counter=9, seed='L2_2b10_medium_j70_2j25')).\
        merge(l2names=['L2_j70_3j25']).merge(l2names=['L2_2b10_medium_L1_2J10J50']).merge(['EF_2b10_medium_L1_2J10J50', 'EF_j75_2j30_a4tc_EFFS'],onlyEF=True),
        
        
        MergedChain('2b10_medium_j75_j45_a4tc_EFFS', l2=Chain(name='L2_2b10_medium_j70_j40', counter=3908, seed='L1_2J15_J50'),
                    ef=Chain(name='EF_2b10_medium_j75_j45_a4tc_EFFS', counter=3908, seed='L2_2b10_medium_j70_j40')).\
        merge(l2names=['L2_j70_2j40']).merge(l2names=['L2_2b10_medium_L1_2J10J50']).merge(['EF_2b10_medium_L1_2J10J50', 'EF_j75_j45_a4tc_EFFS'],onlyEF=True),
        
        MergedChain('2b10_medium_4j40_a4tc_EFFS', l2=Chain(name='L2_2b10_medium_4j35', counter=3912, seed='L1_4J15'),
                    ef=Chain(name='EF_2b10_medium_4j40_a4tc_EFFS', counter=3912, seed='L2_2b10_medium_4j35')).\
        merge(l2names=['L2_4j35']).merge(l2names=['L2_2b10_medium_4L1J10']).merge(['EF_2b10_medium_4L1J10', 'EF_4j40_a4tc_EFFS'],onlyEF=True),
        
        
        MergedChain('2b20_medium_j75_j45_a4tc_EFFS', l2=Chain(name='L2_2b20_medium_j70_2j40', counter=332, seed='L1_2J15_J50'),
                    ef=Chain(name='EF_2b20_medium_j75_j45_a4tc_EFFS', counter=332, seed='L2_2b20_medium_j70_2j40')).\
        merge(l2names=['L2_j70_2j40']).merge(l2names=['L2_2b20_medium_L1_2J15J50']).merge(['EF_2b20_medium_L1_2J15J50', 'EF_j75_j45_a4tc_EFFS'],onlyEF=True),
        
        
        MergedChain('2b15_medium_j75_j40_a4tc_EFFS', l2=Chain(name='L2_2b15_medium_j70_2j35', counter=4444, seed='L1_2J15_J50'),
                    ef=Chain(name='EF_2b15_medium_j75_j40_a4tc_EFFS', counter=4444, seed='L2_2b15_medium_j70_2j35')).\
        merge(l2names=['L2_j70_2j35']).merge(l2names=['L2_2b15_medium_3L1J15']).merge(['EF_2b15_medium_3L1J15', 'EF_j75_j40_a4tc_EFFS'],onlyEF=True),
        
        #E.P. what happen if I put a dummy counter?
        #MergedChain('2b15_medium_j75_j40_a4tc_EFFS_2L1J10', l2=Chain(name='L2_2b15_medium_j70_j35_2L1J10', counter=925, seed='L1_2J10_J50'),
        #            ef=Chain(name='EF_2b15_medium_j75_j40_a4tc_EFFS_2L1J10', counter=925, seed='L2_2b15_medium_j70_j35_2L1J10')).\
        #merge(l2names=['L2_j70_2j35_2L1J10']).merge(l2names=['L2_2b15_medium_2L1J10']).merge(['EF_2b15_medium_2L1J10', 'EF_j75_j40_a4tc_EFFS_2L1J10_'],onlyEF=True),
        
        MergedChain('2b15_medium_j75_j40_a4tc_EFFS_ht350', l2=Chain(name='L2_2b15_medium_j70_2j35_2L1J10', counter=925, seed='L1_2J10_J50'),
                    ef=Chain(name='EF_2b15_medium_j75_j40_a4tc_EFFS_ht350', counter=925, seed='L2_2b15_medium_j70_2j35_2L1J10')).\
        merge(l2names=['L2_j70_2j35_2L1J10']).merge(l2names=['L2_2b15_medium_2L1J10']).merge(['EF_2b15_medium_2L1J10', 'EF_j75_j40_a4tc_EFFS_ht350'],onlyEF=True),
        
        MergedChain('2b10_medium_j100_j30_a4tc_EFFS', l2=Chain(name='L2_2b10_medium_j95_j25', counter=4123, seed='L1_2J10_J75'),
                    ef=Chain(name='EF_2b10_medium_j100_j30_a4tc_EFFS', counter=4123, seed='L2_2b10_medium_j95_j25')).\
        merge(l2names=['L2_j95_2j25']).merge(l2names=['L2_2b10_medium_3L1J10']).merge(['EF_2b10_medium_3L1J10', 'EF_j100_j30_a4tc_EFFS'],onlyEF=True),
        
        
        ]


####################################################################
#!!!! v2 and v3 menu definitions!!!!
####################################################################
if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus() 
    or TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus()
    or TriggerFlags.triggerMenuSetup() in Lvl1Flags.ALFAv2Menus()):

    MultiBjets = [
        
        ]

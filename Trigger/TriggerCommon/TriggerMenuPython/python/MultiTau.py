# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from string import atoi
from AthenaCommon.Logging import logging        
from TriggerMenuPython.TriggerPythonConfig import *
from TriggerMenuPython.Combined import CombinedChain

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags
from TriggerMenuPython.ChainTemplate import *

from TrigTauHypo.T2TauTauCombConfig import *
from TrigTauHypo.EFTauTauCombConfig import *
fexes = FreeStore()
hypos = FreeStore()
fexes.l2_tautauCombFex = T2TauTauCombFex()
fexes.ef_tautauCombFex = EFTauTauCombFex()

hypos.l2_tau27Ti_loose2_tau18Ti_loose2_deta18  = T2TauTauCombHypo_tight("T2TauTauCombHypo_tau27tau18l2_deta18")
hypos.ef_tau27Ti_loose2_tau18Ti_loose2_deta18  = EFTauTauCombHypo_tight("EFTauTauCombHypo_tau27tau18l2_deta18")

hypos.l2_tau27Ti_loose2_tau18Ti_loose2_deta20  = T2TauTauCombHypo_medium("T2TauTauCombHypo_tau27tau18l2_deta20")
hypos.ef_tau27Ti_loose2_tau18Ti_loose2_deta20  = EFTauTauCombHypo_medium("EFTauTauCombHypo_tau27tau18l2_deta20")

hypos.l2_tau27Ti_loose2_tau18Ti_loose2_deta25  = T2TauTauCombHypo_loose("T2TauTauCombHypo_tau27tau18l2_deta25")
hypos.ef_tau27Ti_loose2_tau18Ti_loose2_deta25  = EFTauTauCombHypo_loose("EFTauTauCombHypo_tau27tau18l2_deta25")

hypos.l2_tau27Ti_loose3_tau18Ti_loose3_deta20  = T2TauTauCombHypo_medium("T2TauTauCombHypo_tau27tau18l3_deta20")
hypos.ef_tau27Ti_loose3_tau18Ti_loose3_deta20  = EFTauTauCombHypo_medium("EFTauTauCombHypo_tau27tau18l3_deta20")

hypos.l2_tau27Ti_loose3_tau18Ti_loose3_deta25  = T2TauTauCombHypo_loose("T2TauTauCombHypo_tau27tau18l3_deta25")
hypos.ef_tau27Ti_loose3_tau18Ti_loose3_deta25  = EFTauTauCombHypo_loose("EFTauTauCombHypo_tau27tau18l3_deta25")



class MultiTau (CombinedChain) :
    def __init__(self, sig_id, chain_counter, L1_chain_name, EF_ingredients, 
                 prescale_L2, prescale_EF, pass_throughL2, pass_throughEF):
        
        CombinedChain.__init__( self, sig_id, chain_counter, L1_chain_name, EF_ingredients, ['tauEtmiss'],
                                prescale_L2, prescale_EF, pass_throughL2, pass_throughEF, removeOverlap=None)

    def generateMenu(self, triggerPythonConfig):
         CombinedChain.generateMenu(self, triggerPythonConfig)

###########################################################################################
class TauTauTopoComb(ChainTemplate):
    def __init__(self, signature, l2, ef=DeduceChainConfig, l2merge=None, efmerge=None, l2_hypo=None, ef_hypo=None):
        ChainTemplate.__init__(self, signature, l2=l2, ef=ef)

        self.addGroups('RATE:MultiTau').addGroups('BW:Tau')
        self.addStreams('JetTauEtmiss')

        self.seq_generated = False
        self.l2merge = l2merge
        self.efmerge = efmerge
        self.l2_hypo = l2_hypo
        self.ef_hypo = ef_hypo
        if l2_hypo == None and hasattr(hypos, 'l2_%s' % signature):
            self.l2_hypo = getattr(hypos, 'l2_%s' % signature)
        if ef_hypo == None and hasattr(hypos, 'ef_%s' % signature):
            self.ef_hypo = getattr(hypos, 'ef_%s' % signature)

        try:
            # in generateMenu()
            self.mergeThem()
            self.seq_generated = True
        except AttributeError:
            # Do it later in generateMenu()
            self.seq_generated = False

    def mergeThem(self):
        self.merge(efnames=self.efmerge, l2names=self.l2merge, onlyEF=True)
        self.addSequence(self.l2merge, [fexes.l2_tautauCombFex, self.l2_hypo], \
                         self.l2.chain_name).addSignature('L2')
        self.addSequence(self.efmerge, [fexes.ef_tautauCombFex, self.ef_hypo], \
                         self.ef.chain_name).addSignature('EF')
    def generateMenu(self, tpc):
        if not self.seq_generated:
            try:
                self.mergeThem()
                self.seq_generated = True
            except AttributeError:
                print 'something went wrong in combined chains'

    pass

###########################################################################################
# for chain IDs see Tau.py

MultiTaus = [ 
    MultiTau('2tau29_medium1', 995, 'L1_2TAU11', ['EF_tau29_medium1']*2, prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
    MultiTau('2tau29T_medium1', 832, 'L1_2TAU15', ['EF_tau29T_medium1']*2, prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
    MultiTau('2tau29i_medium1', 980, 'L1_2TAU11I', ['EF_tau29i_medium1']*2, prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
    MultiTau('tau29T_medium1_tau20T_medium1', 76, 'L1_2TAU11_TAU15', ['EF_tau29T_medium1', 'EF_tau20T_medium1'],
             prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
    ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus() or \
       TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():
    MultiTaus += [
        MultiTau('tau100_loose1_tau70_loose1', 80, 'L1_2TAU20', ['EF_tau100_loose1', 'EF_tau70_loose1'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        
        MultiTau('tau29Ti_medium1_tau20Ti_medium1', 177, 'L1_2TAU11I_TAU15', ['EF_tau29T_medium1', 'EF_tau20Ti_medium1'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        MultiTau('tau29Ti_tight1_tau20Ti_tight1', 998, 'L1_2TAU11I_TAU15', ['EF_tau29T_tight1', 'EF_tau20Ti_tight1'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        #Chains with loosened pT thresholds at L2 and EF
        MultiTau('tau29Ti_medium1_tau20Ti_medium1_L2loose', 1770, 'L1_2TAU11I_TAU15', ['EF_tau29T_medium1', 'EF_tau20Ti_medium1_L2loose'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        MultiTau('tau29Ti_medium1_L2loose_tau20Ti_medium1_L2loose', 1771, 'L1_2TAU11I_TAU15', ['EF_tau29T_medium1_L2loose', 'EF_tau20Ti_medium1_L2loose'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        MultiTau('tau29Ti_medium1_tau18Ti_medium1_L2loose', 1773, 'L1_2TAU11I_TAU15', ['EF_tau29T_medium1', 'EF_tau18Ti_medium1_L2loose'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        MultiTau('tau27Ti_medium1_L2loose_tau18Ti_medium1_L2loose', 556, 'L1_2TAU11I_TAU15', ['EF_tau27T_medium1_L2loose', 'EF_tau18Ti_medium1_L2loose'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        MultiTau('tau27Ti_loose3_tau18Ti_loose3', 7561, 'L1_2TAU11I_TAU15', ['EF_tau27T_loose3', 'EF_tau18Ti_loose3'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        MultiTau('tau27Ti_loose2_tau18Ti_loose2', 23, 'L1_2TAU11I_TAU15', ['EF_tau27T_loose2', 'EF_tau18Ti_loose2'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        TauTauTopoComb('tau27Ti_loose2_tau18Ti_loose2_deta25',
                       l2=Chain(name='L2_tau27Ti_loose2_tau18Ti_loose2_deta25', counter=379, seed='L1_2TAU11I_TAU15'),
                       efmerge=['EF_tau27Ti_loose2', 'EF_tau18Ti_loose2'], l2merge=['L2_tau27Ti_loose2', 'L2_tau18Ti_loose2']),

        TauTauTopoComb('tau27Ti_loose2_tau18Ti_loose2_deta20',
                       l2=Chain(name='L2_tau27Ti_loose2_tau18Ti_loose2_deta20', counter=25, seed='L1_2TAU11I_TAU15'),
                       efmerge=['EF_tau27Ti_loose2', 'EF_tau18Ti_loose2'], l2merge=['L2_tau27Ti_loose2', 'L2_tau18Ti_loose2']),

        TauTauTopoComb('tau27Ti_loose2_tau18Ti_loose2_deta18',
                       l2=Chain(name='L2_tau27Ti_loose2_tau18Ti_loose2_deta18', counter=376, seed='L1_2TAU11I_TAU15'),
                       efmerge=['EF_tau27Ti_loose2', 'EF_tau18Ti_loose2'], l2merge=['L2_tau27Ti_loose2', 'L2_tau18Ti_loose2']),
        
        TauTauTopoComb('tau27Ti_loose3_tau18Ti_loose3_deta25',
                       l2=Chain(name='L2_tau27Ti_loose3_tau18Ti_loose3_deta25', counter=4448, seed='L1_2TAU11I_TAU15'),
                       efmerge=['EF_tau27Ti_loose3', 'EF_tau18Ti_loose3'], l2merge=['L2_tau27Ti_loose3', 'L2_tau18Ti_loose3']),

        TauTauTopoComb('tau27Ti_loose3_tau18Ti_loose3_deta20',
                       l2=Chain(name='L2_tau27Ti_loose3_tau18Ti_loose3_deta20', counter=4449, seed='L1_2TAU11I_TAU15'),
                       efmerge=['EF_tau27Ti_loose3', 'EF_tau18Ti_loose3'], l2merge=['L2_tau27Ti_loose3', 'L2_tau18Ti_loose3']),

        ##L2 Pass through test chains
        MultiTau('tau29Ti_loose1_L2NoCut_tau20Ti_loose1_L2NoCut', 5518, 'L1_2TAU11I_TAU15', ['EF_tau29Ti_loose1_L2NoCut', 'EF_tau20Ti_loose1_L2NoCut'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='1',pass_throughEF='0'),
        MultiTau('tau29Ti_loose2_L2NoCut_tau20Ti_loose2_L2NoCut', 5519, 'L1_2TAU11I_TAU15', ['EF_tau29Ti_loose2_L2NoCut', 'EF_tau20Ti_loose2_L2NoCut'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='1',pass_throughEF='0'),
        MultiTau('tau29Ti_medium1_L2NoCut_tau20Ti_medium1_L2NoCut', 5520, 'L1_2TAU11I_TAU15', ['EF_tau29Ti_medium1_L2NoCut', 'EF_tau20Ti_medium1_L2NoCut'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='1',pass_throughEF='0'),

        ]



if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():
    MultiTaus += [
        MultiTau('2tau38T_medium1', 931, 'L1_2TAU20', ['EF_tau38T_medium1']*2, prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        MultiTau('2tau38T_medium',  1933, 'L1_2TAU20', ['EF_tau38T_medium']*2,  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        MultiTau('2tau38T_medium1_llh',  190, 'L1_2TAU20', ['EF_tau38T_medium1_llh']*2,  prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        MultiTau('tau29Ti_medium1_llh_tau20Ti_medium1_llh', 272, 'L1_2TAU11I_TAU15', ['EF_tau29T_medium1_llh', 'EF_tau20Ti_medium1_llh'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        MultiTau('tau29Ti_tight1_llh_tau20Ti_tight1_llh', 999, 'L1_2TAU11I_TAU15', ['EF_tau29T_tight1_llh', 'EF_tau20Ti_tight1_llh'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),

        ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():
    MultiTaus += [
        MultiTau('tau29Ti_medium1_tau20i_medium1', 178, 'L1_2TAU8I_TAU15', ['EF_tau29T_medium1', 'EF_tau20i_medium1'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
#    MultiTau('tau29T_medium2_tau20T_medium2', 445, 'L1_2TAU11_TAU15', ['EF_tau29T_medium2', 'EF_tau20T_medium2'],
#             prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
#        MultiTau('tau29Ti_medium2_tau20Ti_medium2', 446, 'L1_2TAU11I_TAU15', ['EF_tau29T_medium2', 'EF_tau20Ti_medium2'],
#                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
#    MultiTau('tau29T_medium1_1P_tau20T_medium2', 447, 'L1_2TAU11_TAU15', ['EF_tau29T_medium1_1P', 'EF_tau20T_medium2'],
#             prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
#    MultiTau('tau29Ti_medium1_1P_tau20Ti_medium2', 448, 'L1_2TAU11I_TAU15', ['EF_tau29T_medium1_1P', 'EF_tau20Ti_medium2'],
#             prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus() or \
       TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
    MultiTaus += [
        MultiTau('tau29_medium1_tau20_medium1', 986, 'L1_2TAU8_TAU11', ['EF_tau29_medium1', 'EF_tau20_medium1'], prescale_L2='1',
                 prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        ]
    
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
    MultiTaus += [
        MultiTau('2tau16_loose',        '605', 'L1_2TAU6',   ['EF_tau16_loose','EF_tau16_loose'],
                 prescale_L2='1', prescale_EF='1',pass_throughL2='0',pass_throughEF='0'),
        #MultiTau('2tau20_loose',       '606', 'L1_2TAU8',   ['EF_tau20_loose','EF_tau20_loose'],
               # prescale_L2='1', prescale_EF='1',pass_throughL2='0',pass_throughEF='0'),
        MultiTau('2tau29_loose',        '578', 'L1_2TAU11',  ['EF_tau29_loose','EF_tau29_loose'],
                 prescale_L2='1', prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        MultiTau('2tau29_loose1',       '726', 'L1_2TAU11',  ['EF_tau29_loose1','EF_tau29_loose1'],
                 prescale_L2='1', prescale_EF='1', pass_throughL2='0',pass_throughEF='0'), 
        MultiTau('2tau38_loose',        '522', 'L1_2TAU11',  ['EF_tau38_loose','EF_tau38_loose'],
                 prescale_L2='1', prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        MultiTau('tau29_medium_tau38_medium', 4300, 'L1_2TAU11', ['EF_tau29_medium', 'EF_tau38_medium'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        MultiTau('tau29_medium_tau38_loose', 4301, 'L1_2TAU11', ['EF_tau29_medium', 'EF_tau38_loose'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        MultiTau('tau29_loose_tau50_loose', 4302, 'L1_2TAU11', ['EF_tau29_loose', 'EF_tau50_loose'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        MultiTau('tau29_loose_tau38_loose', 4303, 'L1_2TAU11', ['EF_tau29_loose', 'EF_tau38_loose'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        MultiTau('tau20_loose_tau29_loose', 4304, 'L1_2TAU8_TAU11', ['EF_tau20_loose', 'EF_tau29_loose'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        #    MultiTau('tau29_medium_tau20_loose_1prong', 3447, 'L1_2TAU8_TAU11', ['EF_tau29_medium', 'EF_tau20_loose_1prong'],
        #             prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        #    MultiTau('tau29_medium1_tau20_loose_1prong', 3448, 'L1_2TAU8_TAU11', ['EF_tau29_medium1', 'EF_tau20_loose_1prong'],
        #             prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        MultiTau('tau38_medium1_tau29_medium1', 4327, 'L1_2TAU11_TAU15', ['EF_tau38_medium1', 'EF_tau29_medium1'],
                 prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        MultiTau('tau38T_medium1_tau29T_medium1', 4328, 'L1_2TAU15_TAU20', ['EF_tau38T_medium1', 'EF_tau29T_medium1'],
             prescale_L2='1',  prescale_EF='1', pass_throughL2='0',pass_throughEF='0'),
        ]




for item in MultiTaus :
    if hasattr(item, 'group'): item.group += ["RATE:MultiTau",'BW:Tau']


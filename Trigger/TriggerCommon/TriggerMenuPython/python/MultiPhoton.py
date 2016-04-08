# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from string import atoi
from AthenaCommon.Logging import logging
        
from TriggerMenuPython.TriggerPythonConfig import *
from TriggerMenuPython.Combined import CombinedChain2
from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags

seed1 = 'L1_EM12'
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.NoVHThresholdMenus():
    seed1 = 'L1_2EM14'
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.VHThresholdMenus():
    seed1 = 'L1_2EM12'

class MultiPhoton:
    def __init__(self, sig_id):

        mlog = logging.getLogger( 'MultiPhoton.py' )
        self.sig_id = sig_id
        
        self.__supported = [
                             '2g5_loose', '2g10_loose','2g10_medium',
                             '2g15_loose','2g20_loose',
                             '2g20i_loose'
                             ]

        if sig_id not in self.__supported:
            mlog.error(sig_id+" not supprted "+str(self.__supported))
            raise Exception("ERROR: "+sig_id+" not supprted "+str(self.__supported) )

        ## spcify simple chain names which wil be later used to build (AND/merge) into this chains
        self.l2_ingredients = {
            '2g5_loose': [ 'L2_g5_loose', 'L2_g5_loose', ],
            '2g10_loose': ['L2_g10_loose', 'L2_g10_loose'],
            '2g10_medium': ['L2_g10_medium', 'L2_g10_medium'],
            '2g15_loose': ['L2_g15_loose', 'L2_g15_loose'],
            '2g20_loose': ['L2_g20_loose', 'L2_g20_loose'],
            '2g20i_loose': ['L2_g20i_loose', 'L2_g20i_loose'], 
            }

        self.l2_chains = {
            '2g5_loose': HLTChain(chain_name='L2_2g5_loose',          chain_counter='938', lower_chain_name='L1_2EM3', level='L2', prescale='1', pass_through='0'),
            "2g10_loose":  HLTChain( chain_name="L2_2g10_loose",      chain_counter="77", lower_chain_name="L1_2EM5", level="L2", prescale="1", pass_through="0"),
            "2g10_medium":  HLTChain( chain_name="L2_2g10_medium",      chain_counter="94", lower_chain_name="L1_2EM5", level="L2", prescale="1", pass_through="0"),
            "2g15_loose":  HLTChain( chain_name="L2_2g15_loose",      chain_counter="78", lower_chain_name="L1_2EM10", level="L2", prescale="1", pass_through="0"),
            "2g20_loose":  HLTChain( chain_name="L2_2g20_loose",      chain_counter="79", lower_chain_name=seed1, level="L2", prescale="1", pass_through="0"),
            "2g20i_loose": HLTChain( chain_name="L2_2g20i_loose",      chain_counter="81", lower_chain_name="L1_2EM14I", level="L2", prescale="1", pass_through="0"),
            }

        
        self.ef_ingredients = {
            '2g5_loose': [ 'EF_g5_loose', 'EF_g5_loose', ],
            '2g10_loose': ['EF_g10_loose', 'EF_g10_loose'],
            '2g10_medium': ['EF_g10_medium', 'EF_g10_medium'],
            '2g15_loose': ['EF_g15_loose', 'EF_g15_loose'],
            '2g20_loose': ['EF_g20_loose', 'EF_g20_loose'],
            '2g20i_loose': ['EF_g20i_loose', 'EF_g20i_loose'], 
            }
        self.ef_chains = {
                           '2g5_loose': HLTChain(chain_name='EF_2g5_loose',          chain_counter='938', lower_chain_name='L2_2g5_loose', level='EF', prescale='1', pass_through='0'),
                           "2g10_loose":  HLTChain( chain_name="EF_2g10_loose",      chain_counter="77", lower_chain_name="L2_2g10_loose", level="EF", prescale="1", pass_through="0"),
                           "2g10_medium":  HLTChain( chain_name="EF_2g10_medium",      chain_counter="94", lower_chain_name="L2_2g10_medium", level="EF", prescale="1", pass_through="0"),
                           "2g15_loose":  HLTChain( chain_name="EF_2g15_loose",      chain_counter="78", lower_chain_name="L2_2g15_loose", level="EF", prescale="1", pass_through="0"),
                           "2g20_loose":  HLTChain( chain_name="EF_2g20_loose",      chain_counter="79", lower_chain_name="L2_2g20_loose", level="EF", prescale="1", pass_through="0"),
                           "2g20i_loose": HLTChain( chain_name="EF_2g20i_loose",      chain_counter="81", lower_chain_name="L2_2g20i_loose", level="EF", prescale="1", pass_through="0"), 
                           }
        



    def generateMenu(self, triggerPythonConfig):
        
        mlog = logging.getLogger( 'MultiPhoton.py' )  ## get the logger
        mlog.debug("generating config for: "+self.sig_id)
        l2parts = [ triggerPythonConfig.getHLTChain(i) for i in self.l2_ingredients[self.sig_id]  ] ## find chains objects given names fro L2
        efparts = [ triggerPythonConfig.getHLTChain(i) for i in self.ef_ingredients[self.sig_id]  ] ## same for EF
        if None in l2parts + efparts:  ## some action if simple chains are not found
            mlog.warning("Basic chains used to built combined chain: "+self.sig_id+" are not present.")
            mlog.warning("This were found:  "+str([c.chain_name for c in  l2parts + efparts if c != None ]))
            mlog.warning("This were needed: "+str( self.l2_ingredients[self.sig_id] +  self.ef_ingredients[self.sig_id]))
            return 

        l2chain = self.l2_chains[self.sig_id]
        l2chain.addStreamTag('egamma')   ## add stream tags
        l2chain.addGroup(["RATE:MultiPhoton",'BW:Egamma'])
        
        l2chain.mergeAndAppendChains(l2parts)  ## here the complexity is hidden, chains are inspected and signatures merged
 
        ## identical for EF as for L2
        efchain = self.ef_chains[self.sig_id]
        efchain.addStreamTag('egamma')
        efchain.addGroup(["RATE:MultiPhoton",'BW:Egamma'])

        if TriggerFlags.doEF():
            efchain.mergeAndAppendChains(efparts)  ## creating EF chain by merging appropriate ef chains 

        triggerPythonConfig.registerHLTChain(self.sig_id, l2chain) ## register chains to be eventually put into the output XML
        triggerPythonConfig.registerHLTChain(self.sig_id, efchain)
        

MultiPhotons = [
    MultiPhoton('2g5_loose'),
    MultiPhoton('2g10_loose'),
    MultiPhoton('2g15_loose'),
    MultiPhoton('2g20_loose'),
    MultiPhoton('2g20i_loose'), 

    CombinedChain2('g5_loose_2L1J5', 659, ['egamma'],
                   ['RATE:Photon','BW:Egamma'], ['RATE:Photon','BW:Egamma'], 
                   'L2_g5_loose_2L1J5', ['L2_g5_loose']*1, 'L1_EM3_2J5',1,0,
                   'EF_g5_loose_2L1J5', ['EF_g5_loose']*1, 1,0),

    CombinedChain2('g5_medium_2L1J5', 658, ['egamma'],
                   ['RATE:Photon','BW:Egamma'], ['RATE:Photon','BW:Egamma'],
                   'L2_g5_medium_2L1J5', ['L2_g5_medium']*1, 'L1_EM3_2J5',1,0,
                   'EF_g5_medium_2L1J5', ['EF_g5_medium']*1, 1,0),

    CombinedChain2('2g5_medium', 647, ['egamma'],
                   ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                   'L2_2g5_medium', ['L2_g5_medium']*2, 'L1_2EM3',1,0,
                   'EF_2g5_medium', ['EF_g5_medium']*2, 1,0),

    CombinedChain2('2g7_loose', 148, ['egamma'],
                   ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                   'L2_2g7_loose', ['L2_g7_loose']*2, 'L1_2EM3',1,0,
                   'EF_2g7_loose', ['EF_g7_loose']*2, 1,0),
    
    CombinedChain2('2g7_medium', 371, ['egamma'],
                   ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                   'L2_2g7_medium', ['L2_g7_medium']*2, 'L1_2EM3',1,0,
                   'EF_2g7_medium', ['EF_g7_medium']*2, 1,0),

    CombinedChain2('2g10_medium', 705, ['egamma'],
                   ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                   'L2_2g10_medium', ['L2_g10_medium']*2, 'L1_2EM5',1,0,
                   'EF_2g10_medium', ['EF_g10_medium']*2, 1,0),

    CombinedChain2('2g50_etcut', 802, ['egamma'],
                   ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                   'L2_2g50_etcut', ['L2_g50_etcut']*2, seed1,1,0,
                   'EF_2g50_etcut', ['EF_g50_etcut']*2, 1,0),

    CombinedChain2('2g15vh_medium',2001, ['egamma'],
                   ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                   'L2_2g15vh_medium', ['L2_g15vh_medium']*2, 'L1_2EM10VH',1,0,
                   'EF_2g15vh_medium', ['EF_g15vh_medium']*2, 1,0),

    CombinedChain2('2g15vh_loose',2010, ['egamma'],
                   ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                   'L2_2g15vh_loose', ['L2_g15vh_loose']*2, 'L1_2EM10VH',1,0,
                   'EF_2g15vh_loose', ['EF_g15vh_loose']*2, 1,0),
    

    CombinedChain2('g100_etcut_g50_etcut', 804, ['egamma'],
                   ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                   'L2_g100_etcut_g50_etcut', ['L2_g100_etcut', 'L2_g50_etcut'], 'L1_2EM14',1,0,
                   'EF_g100_etcut_g50_etcut', ['EF_g100_etcut', 'EF_g50_etcut'], 1,0),


    ]


if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():
    MultiPhotons += [
        
        CombinedChain2('g30_loose_g20_loose', 526, ['egamma'],
                       ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                       'L2_g30_loose_g20_loose', ['L2_g20_loose', 'L2_g30_loose'], 'L1_2EM12_EM16V',1,0,
                       #'L2_g30_loose_g20_loose', ['L2_g20_loose', 'L2_g30_loose'], 'L1_2EM12',1,0,
                       'EF_g30_loose_g20_loose', ['EF_g20_loose', 'EF_g30_loose'], 1,0),

        #next five new in 92627
        CombinedChain2('g35_loose_g25_loose', 527, ['egamma'],
                       ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                       'L2_g35_loose_g25_loose', ['L2_g35_loose', 'L2_g25_loose'], 'L1_2EM12_EM16V',1,0,
                       'EF_g35_loose_g25_loose', ['EF_g35_loose', 'EF_g25_loose'], 1,0),

        CombinedChain2('g35_loose_g30_loose', 528, ['egamma'],
                       ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                       'L2_g35_loose_g30_loose', ['L2_g35_loose', 'L2_g30_loose'], 'L1_2EM12_EM16V',1,0,
                       'EF_g35_loose_g30_loose', ['EF_g35_loose', 'EF_g30_loose'], 1,0),
        
        CombinedChain2('g40_loose_g25_loose', 529, ['egamma'],
                       ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                       'L2_g40_loose_g25_loose', ['L2_g40_loose', 'L2_g25_loose'], 'L1_2EM12_EM16V',1,0,
                       'EF_g40_loose_g25_loose', ['EF_g40_loose', 'EF_g25_loose'], 1,0),
        
        CombinedChain2('g40_loose_g30_loose', 530, ['egamma'],
                       ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                       'L2_g40_loose_g30_loose', ['L2_g40_loose', 'L2_g30_loose'], 'L1_2EM12_EM16V',1,0,
                       'EF_g40_loose_g30_loose', ['EF_g40_loose', 'EF_g30_loose'], 1,0),
        
        CombinedChain2('2g30_loose', 7531, ['egamma'],
                       ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                       'L2_2g30_loose', ['L2_g30_loose', 'L2_g30_loose'], 'L1_2EM12_EM16V',1,0,
                       'EF_2g30_loose', ['EF_g30_loose', 'EF_g30_loose'], 1,0),

        CombinedChain2('2g40_loose', 160, ['egamma'],
                       ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                       'L2_2g40_loose', ['L2_g40_loose', 'L2_g40_loose'], 'L1_EM30',1,0,
                       'EF_2g40_loose', ['EF_g40_loose', 'EF_g40_loose'], 1,0),
        
        CombinedChain2('g30_medium_g20_medium', 533, ['egamma'],
                       ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                       'L2_g30_medium_g20_medium', ['L2_g30_medium', 'L2_g20_medium'], 'L1_2EM12_EM16V',1,0,
                       'EF_g30_medium_g20_medium', ['EF_g30_medium', 'EF_g20_medium'], 1,0),

        CombinedChain2('g30_medium_g20_medium_AthSel', 5583, ['egamma'],
                       ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                       'L2_g30_medium_g20_medium_AthSel', ['L2_g30_medium_AthSel', 'L2_g20_medium_AthSel'], 'L1_2EM12_EM16V',1,0,
                       'EF_g30_medium_g20_medium_AthSel', ['EF_g30_medium_AthSel', 'EF_g20_medium_AthSel'], 1,0),

        ####

        CombinedChain2('2g15vh_loose_g10_loose', 6537, ['egamma'],
                       ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                       'L2_2g15vh_loose_g10_loose', ['L2_2g15vh_loose','L2_g10_loose'], 'L1_2EM10VH',1,0,
                       'EF_2g15vh_loose_g10_loose',       ['EF_2g15vh_loose','EF_g10_loose'], 1,0),


        CombinedChain2('2g15vh_medium_g10_medium', 162, ['egamma'],
                       ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                       'L2_2g15vh_medium_g10_medium', ['L2_2g15vh_medium','L2_g10_medium'],'L1_2EM10VH',1,0,
                       'EF_2g15vh_medium_g10_medium', ['EF_2g15vh_medium','EF_g10_medium'], 1,0),

        
        CombinedChain2('3g15vh_loose', 539, ['egamma'],
                       ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                       'L2_3g15vh_loose', ['L2_g15vh_loose']*3,'L1_2EM10VH',1,0,
                       'EF_3g15vh_loose', ['EF_g15vh_loose']*3, 1,0),


        CombinedChain2('3g15vh_medium', 2003, ['egamma'],
                       ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                       'L2_3g15vh_medium', ['L2_g15vh_medium']*3,'L1_2EM10VH',1,0,
                       'EF_3g15vh_medium', ['EF_g15vh_medium']*3, 1,0),


        CombinedChain2('2g20vh_medium', 161, ['egamma'],
                       ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                       'L2_2g20vh_medium', ['L2_g20vh_medium']*2,'L1_2EM10VH',1,0,
                       'EF_2g20vh_medium', ['EF_g20vh_medium']*2, 1,0),
        
        ]
    
else:
    
    MultiPhotons += [
        CombinedChain2('g30_loose_g20_loose', 526, ['egamma'],
                       ['RATE:MultiPhoton','BW:Egamma'], ['RATE:MultiPhoton','BW:Egamma'],
                       'L2_g30_loose_g20_loose', ['L2_g20_loose', 'L2_g30_loose'], 'L1_2EM14',1,0,
                       'EF_g30_loose_g20_loose', ['EF_g20_loose', 'EF_g30_loose'], 1,0),
        
    ]
    

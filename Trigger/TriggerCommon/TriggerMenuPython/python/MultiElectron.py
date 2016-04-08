# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Multi Electron trigger slice signatures  """

__author__  = 'P.Urquijo'
__version__=""
__doc__="Implementation of Multi Electron Slice  electron signatures"

from string import atoi
from AthenaCommon.Logging import logging
        
from TriggerMenuPython.TriggerPythonConfig import *
from TriggerMenuPython.Combined import CombinedChain2
from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags

from TrigEgammaHypo.TrigL2DielectronMassHypoConfig import TrigL2DielectronMassFex_Zee, TrigL2DielectronMassHypo_Zee, TrigL2DielectronMassHypo_Zee2, TrigL2DielectronMassFex_Jpsiee, TrigL2DielectronMassHypo_Jpsiee, TrigL2DielectronMassFex_Upsiee, TrigL2DielectronMassHypo_Upsiee
from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFex_Zee, TrigEFDielectronMassHypo_Zee, TrigEFDielectronMassHypo_Zee2, TrigEFDielectronMassFex_Jpsi, TrigEFDielectronMassHypo_Jpsi, TrigEFDielectronMassFex_Upsi, TrigEFDielectronMassHypo_Upsi

log = logging.getLogger( 'MultiElectron.py' )

class MultiElectron:
    def __init__(self, sig_id):

        mlog = logging.getLogger( 'MultiElectron.py' )
        self.sig_id = sig_id
        
        self.__supported = ['Upsiee', 'Jpsiee', 'Zee']
        if sig_id not in self.__supported:
            mlog.error(sig_id+" not supprted "+str(self.__supported))
            raise Exception("ERROR: "+sig_id+" not supprted "+str(self.__supported) )
        
        ## spcify simple chain names which wil be later used to build (AND/merge) into this chains
        self.l2_ingredients = {
        }
        self.l2_chains = {
        }

        ## some chains may end with topological sequence
        if TriggerFlags.doLVL2():
            self.l2_topologicals = {
                }
            
        self.ef_ingredients = {
            }

        self.ef_chains = {
            }

        self.ef_topologicals = {
            }


    def generateMenu(self, triggerPythonConfig):
        
        mlog = logging.getLogger( 'MultiElectron.py' )  ## get the logger
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
        l2chain.addGroup(["RATE:MultiElectron",'BW:Egamma'])         
        
        l2chain.mergeAndAppendChains(l2parts)  ## here the complexity is hidden, chains are inspected and signatures merged
        if TriggerFlags.doLVL2():            
            if self.l2_topologicals[self.sig_id] is not None: ## here topological algo is added at the end
                if 0 not in [ len(c.siglist) for c in l2parts ]: ## check if all chains do define some steps (can be that not because they are disabled by some flags)
                    TES = []
                    [TES.extend(c.siglist[-1].tes) for c in l2parts]
                    mlog.info("Using this TES as input to toplogical sequence: "+str(TES))
                    triggerPythonConfig.addSequence(TES,  self.l2_topologicals[self.sig_id], 'L2_'+self.sig_id)
                    l2chain.addHLTSignature('L2_'+self.sig_id)



        ## identical for EF as for L2
        efchain = self.ef_chains[self.sig_id]
        efchain.addStreamTag('egamma')
            
        efchain.addGroup(["RATE:MultiElectron",'BW:Egamma'])                 
        efchain.mergeAndAppendChains(efparts)  ## creating EF chain by merging appropriate ef chains 
        if TriggerFlags.doEF():        
            if self.ef_topologicals[self.sig_id] is not None:
                if 0 not in [ len(c.siglist) for c in efparts ]: ## check if all chains do define some steps (can be that not because they are disabled by some flags)
                    TES = []
                    [TES.extend(c.siglist[-1].tes) for c in efparts]
                    mlog.info("Using this TES as input to toplogical sequence: "+str(TES))
                    triggerPythonConfig.addSequence(TES,  self.ef_topologicals[self.sig_id], 'EF_'+self.sig_id)
                    efchain.addHLTSignature('EF_'+self.sig_id)
                else: mlog.warning("PROBLEM with topological - some parts are empty : " + str([ len(c.siglist) for c in efparts ]))

        triggerPythonConfig.registerHLTChain(self.sig_id, l2chain) ## register chains to be eventually put into the output XML
        triggerPythonConfig.registerHLTChain(self.sig_id, efchain)
        

MultiElectrons = [
    ]

MultiElectrons_IdScanDef = [
    CombinedChain2('2e10_loose', 61, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_2e10_loose', ['L2_e10_loose']*2, 'L1_2EM5',1,0,
                   'EF_2e10_loose', ['EF_e10_loose']*2,1,0),

    CombinedChain2('2e10_medium', 62, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_2e10_medium', ['L2_e10_medium']*2, 'L1_2EM5',1,0,
                   'EF_2e10_medium', ['EF_e10_medium']*2,1,0),

    CombinedChain2('2e5_medium', 60, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_2e5_medium', ['L2_e5_medium']*2, 'L1_2EM3',1,0,
                   'EF_2e5_medium', ['EF_e5_medium']*2,1,0),

    CombinedChain2('2e5_medium_SiTrk', 1630, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_2e5_medium_SiTrk', ['L2_e5_medium_SiTrk']*2, 'L1_2EM3',1,0,
                   'EF_2e5_medium_SiTrk', ['EF_e5_medium_SiTrk']*2,1,0),

    CombinedChain2('2e5_medium_TRT', 844, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_2e5_medium_TRT', ['L2_e5_medium_TRT']*2, 'L1_2EM3',1,0,
                   'EF_2e5_medium_TRT', ['EF_e5_medium_TRT']*2,1,0),

    CombinedChain2('2e15_loose', 54, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_2e15_loose', ['L2_e15_loose']*2, 'L1_2EM10',1,0,
                   'EF_2e15_loose', ['EF_e15_loose']*2,1,0),

    CombinedChain2('e5_tight_e5_NoCut', 902, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_e5_tight_e5_NoCut', ['L2_e5_tight', 'L2_e5_NoCut'], 'L1_2EM3',1,0,
                   'EF_e5_tight_e5_NoCut', ['EF_e5_tight', 'EF_e5_NoCut'],1,0),

    CombinedChain2('e5_tight1_e5_NoCut', 903, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_e5_tight1_e5_NoCut', ['L2_e5_tight1', 'L2_e5_NoCut'], 'L1_2EM3',1,0,
                   'EF_e5_tight1_e5_NoCut', ['EF_e5_tight1', 'EF_e5_NoCut'],1,0),

    CombinedChain2('e9_tight1_e5_etcut', 126, ['egamma'],
                       ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                       'L2_e9_tight1_e5_etcut', ['L2_e9_tight1', 'L2_e5_etcut'], 'L1_EM5',1,0,
                       'EF_e9_tight1_e5_etcut', ['EF_e9_tight1', 'EF_e5_etcut'],1,0),

    CombinedChain2('2e5_tight', 900, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_2e5_tight', ['L2_e5_tight']*2, 'L1_2EM3',1,0,
                   'EF_2e5_tight', ['EF_e5_tight']*2,1,0),

    CombinedChain2('2e5_loose1', 602, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_2e5_loose1', ['L2_e5_loose1']*2, 'L1_2EM3',1,0,
                   'EF_2e5_loose1', ['EF_e5_loose1']*2,1,0),

    CombinedChain2('2e5_medium1', 603, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_2e5_medium1', ['L2_e5_medium1']*2, 'L1_2EM3',1,0,
                   'EF_2e5_medium1', ['EF_e5_medium1']*2,1,0),

    CombinedChain2('2e5_tight1', 656, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_2e5_tight1', ['L2_e5_tight1']*2, 'L1_2EM3',1,0,
                   'EF_2e5_tight1', ['EF_e5_tight1']*2,1,0),


    CombinedChain2('3e10_medium', 362, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_3e10_medium', ['L2_e10_medium']*3, 'L1_3EM5',1,0, 
                   'EF_3e10_medium', ['EF_e10_medium']*3,1,0),

    CombinedChain2('2e12_medium', 361, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_2e12_medium', ['L2_e12_medium']*2, 'L1_2EM7',1,0,
                   'EF_2e12_medium', ['EF_e12_medium']*2,1,0),

    CombinedChain2('2e11T_medium', 717, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_2e11T_medium', ['L2_e11T_medium']*2, 'L1_2EM10',1,0,
                   'EF_2e11T_medium', ['EF_e11T_medium']*2,1,0),


    CombinedChain2('e24vh_medium1_e7_medium1', 789, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_e24vh_medium1_e7_medium1', [], 'L1_EM18VH',1,0,
                   'EF_e24vh_medium1_e7_medium1', ['EF_e24vh_medium1', 'EF_e7_medium1'],1,0),

    CombinedChain2('e24vh_medium1_e7_medium1_L2StarB', 778, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_e24vh_medium1_e7_medium1_L2StarB', [], 'L1_EM18VH',1,0,
                   'EF_e24vh_medium1_e7_medium1_L2StarB', ['EF_e24vh_medium1', 'EF_e7_medium1_L2StarB'],1,0),

    CombinedChain2('e18vh_medium1_2e7T_medium1', 790, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_e18vh_medium1_2e7T_medium1', [], 'L1_3EM6_EM14VH',1,0,
                   'EF_e18vh_medium1_2e7T_medium1', ['EF_e18vh_medium1','EF_e7T_medium1','EF_e7T_medium1'],1,0),

    CombinedChain2('e18vh_medium1_2e7T_medium1_L2StarB', 777, ['egamma'],
                   ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                   'L2_e18vh_medium1_2e7T_medium1_L2StarB', [], 'L1_3EM6_EM14VH',1,0,
                   'EF_e18vh_medium1_2e7T_medium1_L2StarB', ['EF_e18vh_medium1','EF_e7T_medium1_L2StarB','EF_e7T_medium1_L2StarB'],1,0),
    
    ]

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
    MultiElectrons_IdScanDef += [
        CombinedChain2('2e12T_medium', 718, ['egamma'],
                       ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                       'L2_2e12T_medium', ['L2_e12T_medium']*2, 'L1_2EM10',1,0,
                       'EF_2e12T_medium', ['EF_e12T_medium']*2,1,0),

        CombinedChain2('e15_medium_e12_medium', 139, ['egamma'],
                       ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                       'L2_e15_medium_e12_medium', ['L2_e15_medium', 'L2_e12_medium'], 'L1_2EM7_EM10',1,0,
                       'EF_e15_medium_e12_medium', ['EF_e15_medium', 'EF_e12_medium'],1,0),

        CombinedChain2('e11T_medium_2e6T_medium', 799, ['egamma'],
                       ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                       'L2_e11T_medium_2e6T_medium', ['L2_e11T_medium', 'L2_e6T_medium', 'L2_e6T_medium'], 'L1_3EM5_EM10',1,0,
                       'EF_e11T_medium_2e6T_medium', ['EF_e11T_medium', 'EF_e6T_medium', 'EF_e6T_medium'],1,0),

        CombinedChain2('2e15_medium', 618, ['egamma'],
                       ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                       'L2_2e15_medium', ['L2_e15_medium']*2, 'L1_2EM10',1,0,
                       'EF_2e15_medium', ['EF_e15_medium']*2,1,0),

        ]
    
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.VHThresholdMenus():
    MultiElectrons_IdScanDef += [

        CombinedChain2('2e12Tvh_loose1', 719, ['egamma'],
                       ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                       'L2_2e12Tvh_loose1', ['L2_e12Tvh_loose1']*2, 'L1_2EM10VH',1,0,
                       'EF_2e12Tvh_loose1', ['EF_e12Tvh_loose1']*2,1,0),

        #FTK version for MC tests
        CombinedChain2('2e12Tvh_loose1_FTK', 2899, ['egamma'],
                       ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                       'L2_2e12Tvh_loose1_FTK', ['L2_e12Tvh_loose1_FTK']*2, 'L1_2EM10VH',1,0,
                       'EF_2e12Tvh_loose1_FTK', ['EF_e12Tvh_loose1_FTK']*2,1,0),

        CombinedChain2('2e12Tvh_medium', 718, ['egamma'],
                       ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                       'L2_2e12Tvh_medium', ['L2_e12Tvh_medium']*2, 'L1_2EM10VH',1,0,
                       'EF_2e12Tvh_medium', ['EF_e12Tvh_medium']*2,1,0),

        CombinedChain2('e12Tvh_medium_2e6T_medium', 799, ['egamma'],
                       ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                       'L2_e12Tvh_medium_2e6T_medium', ['L2_e12Tvh_medium', 'L2_e6T_medium', 'L2_e6T_medium'], 'L1_3EM5_EM10VH',1,0,
                       'EF_e12Tvh_medium_2e6T_medium', ['EF_e12Tvh_medium', 'EF_e6T_medium', 'EF_e6T_medium'],1,0),

        CombinedChain2('2e12Tvh_loose1_L2StarB', 426, ['egamma'],
                       ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                       'L2_2e12Tvh_loose1_L2StarB', ['L2_e12Tvh_loose1_L2StarB']*2, 'L1_2EM10VH',1,0,
                       'EF_2e12Tvh_loose1_L2StarB', ['EF_e12Tvh_loose1_L2StarB']*2,1,0),
        

        CombinedChain2('e22vh_loose1_e10_loose1', 8012, ['egamma'],
                       ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                       'L2_e22vh_loose1_e10_loose1', ['L2_e22vh_loose1', 'L2_e10_loose1'], 'L1_2EM5_EM16VH',1,0,
                       'EF_e22vh_loose1_e10_loose1', ['EF_e22vh_loose1', 'EF_e10_loose1'],1,0),

        CombinedChain2('e22_etcut_e10_loose1', 8010, ['egamma'],
                       ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                       'L2_e22_etcut_e10_loose1', ['L2_e22_etcut_EM12', 'L2_e10_loose1_EM3'], 'L1_2EM3_EM12',1,0,
                       'EF_e22_etcut_e10_loose1', ['EF_e22_etcut_EM12', 'EF_e10_loose1_EM3'],1,0),

        CombinedChain2('e22_loose1_e10_etcut', 8011, ['egamma'],
                       ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                       'L2_e22_loose1_e10_etcut', ['L2_e22_loose1_EM16VH', 'L2_e10_etcut'], 'L1_2EM5_EM16VH',1,0,
                       'EF_e22_loose1_e10_etcut', ['EF_e22_loose1_EM16VH', 'EF_e10_etcut'],1,0),

        CombinedChain2('2e15vh_medium', 618, ['egamma'],
                       ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                       'L2_2e15vh_medium', ['L2_e15vh_medium']*2, 'L1_2EM10VH',1,0,
                       'EF_2e15vh_medium', ['EF_e15vh_medium']*2,1,0),

        CombinedChain2('e22vh_medium1_e5_medium', 3333, ['egamma'],
                       ['RATE:MultiElectron','BW:Egamma'], ['RATE:MultiElectron','BW:Egamma'],
                       'L2_e22vh_medium1_e5_medium', ['L2_e22vh_medium1', 'L2_e5_medium'], 'L1_EM16VH',1,0,
                       'EF_e22vh_medium1_e5_medium', ['EF_e22vh_medium1', 'EF_e5_medium'],1,0),
        

        ]

from TriggerMenuPython.ChainTemplate import *

fexes = FreeStore()
hypos = FreeStore()

from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFex_Zee, TrigEFDielectronMassHypo_Zee, TrigEFDielectronMassHypo_Zee2, TrigEFDielectronMassFex_Jpsi, TrigEFDielectronMassHypo_Jpsi, TrigEFDielectronMassFex_Upsi, TrigEFDielectronMassHypo_Upsi
from TrigEgammaHypo.TrigL2DielectronMassHypoConfig import TrigL2DielectronMassFex_Zee, TrigL2DielectronMassHypo_Zee, TrigL2DielectronMassHypo_Zee2, TrigL2DielectronMassFex_Jpsiee, TrigL2DielectronMassHypo_Jpsiee, TrigL2DielectronMassFex_Upsiee, TrigL2DielectronMassHypo_Upsiee


fexes.l2_zee = TrigL2DielectronMassFex_Zee()
fexes.ef_zee = TrigEFDielectronMassFex_Zee()
fexes.zee = TrigEFDielectronMassFex_Zee()
fexes.jpsiee = TrigEFDielectronMassFex_Jpsi()
fexes.upsiee = TrigEFDielectronMassFex_Upsi()
fexes.l2_jpsiee = TrigL2DielectronMassFex_Jpsiee()
fexes.l2_upsiee = TrigL2DielectronMassFex_Upsiee()
fexes.l2_e5_tight_e4_etcut_Jpsi = fexes.l2_jpsiee
fexes.l2_e9_tight_e4_etcut_Jpsi = fexes.l2_jpsiee
fexes.l2_e14_tight_e4_etcut_Jpsi = fexes.l2_jpsiee
fexes.l2_e5_tight_e9_etcut_Jpsi = fexes.l2_jpsiee
fexes.l2_e5_tight_e5_NoCut_Jpsi = fexes.l2_jpsiee

hypos.zee = TrigEFDielectronMassHypo_Zee()
hypos.l2_zee2 = TrigL2DielectronMassHypo_Zee2()
hypos.ef_zee2 = TrigEFDielectronMassHypo_Zee2()
hypos.jpsiee = TrigEFDielectronMassHypo_Jpsi()
hypos.l2_jpsiee = TrigL2DielectronMassHypo_Jpsiee()
hypos.upsiee = TrigEFDielectronMassHypo_Upsi()
hypos.l2_upsiee = TrigL2DielectronMassHypo_Upsiee()
hypos.ef_e5_tight_e4_etcut_Jpsi = hypos.jpsiee
hypos.ef_e9_tight_e4_etcut_Jpsi = hypos.jpsiee
hypos.ef_e14_tight_e4_etcut_Jpsi = hypos.jpsiee
hypos.ef_e5_tight_e9_etcut_Jpsi = hypos.jpsiee
hypos.ef_e5_tight_e5_NoCut_Jpsi = hypos.jpsiee

class DiElectron_resonance(ChainTemplate):
    def __init__(self, signature, l2, ef=DeduceChainConfig,
                 l2merge=None, efmerge=None, fex=None, hypo=None,
                 l2_fex=None, l2_hypo=None):
        ChainTemplate.__init__(self, signature, l2=l2, ef=ef)

        self.addGroups('RATE:MultiElectron').addGroups('BW:Egamma')
        self.addStreams('egamma')

        self.tmp_generated = False
        self.efmerge = None
        self.fex = None
        self.hypo = None
        self.l2_fex = l2_fex
        self.l2_hypo = l2_hypo
        try:
            # Signle electron chains are still configured in the old way.
            # So HLTChain objects may not be created yet. If so, try again
            # in generateMenu()
            if efmerge != None: self.merge(efnames=efmerge)
            self.tmp_generated = True
        except:
            self.tmp_generated = False
            self.efmerge = efmerge
            self.fex = fex
            self.hypo = hypo
        if self.tmp_generated:
            l2fex, l2hypo = None, None
            if self.sig_id.find('_Jpsi') >= 0:
                l2fex = fexes.l2_jpsiee
                l2hypo = hypos.l2_jpsiee
            if self.sig_id.find('_Upsi') >= 0:
                l2fex = fexes.l2_upsiee
                l2hypo = hypos.l2_upsiee
            if self.l2_fex != None: l2fex = self.l2_fex                
            if self.l2_hypo != None: l2hypo = self.l2_hypo
            l2tes = []
            if len(self.l2.siglist) > 0 and l2fex != None and l2hypo!=None:
                l2tes = self.l2.siglist[-1].tes
                self.addSequence(l2tes, [l2fex, l2hypo], \
                                 self.l2.chain_name).addSignature('L2')
            self.addSequence(efmerge, [fex, hypo], \
                             self.ef.chain_name).addSignature('EF')
        
    def generateMenu(self, tpc):
        if not self.tmp_generated:
            l2fex, l2hypo = None, None
            if self.sig_id.find('_Jpsi') >= 0:
                l2fex = fexes.l2_jpsiee
                l2hypo = hypos.l2_jpsiee
            if self.sig_id.find('_Upsi') >= 0:
                l2fex = fexes.l2_upsiee
                l2hypo = hypos.l2_upsiee
            if self.l2_fex != None: l2fex = self.l2_fex                
            if self.l2_hypo != None: l2hypo = self.l2_hypo
            l2tes = []
            if self.efmerge != None: self.merge(efnames=self.efmerge)
            if len(self.l2.siglist) > 0 and l2fex != None and l2hypo!=None:
                l2tes = self.l2.siglist[-1].tes
                self.addSequence(l2tes, [l2fex, l2hypo], \
                                 self.l2.chain_name).addSignature('L2')
            self.addSequence(self.efmerge, [self.fex, self.hypo], \
                             self.ef.chain_name).addSignature('EF')
            self.tmp_generated = True
    pass


if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
    MultiElectrons_IdScanDef += [
        DiElectron_resonance('e20_tight_e15_NoCut_Zee',
                             l2=Chain(name='L2_e20_tight_e15_NoCut_Zee', counter=210, seed='L1_2EM14'),
                             ef=DeduceChainConfig, efmerge=['EF_e20_tight', 'EF_e15_NoCut'],
                             fex=fexes.zee, hypo=hypos.zee),
        ]
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():
    MultiElectrons_IdScanDef += [
        DiElectron_resonance('e20_tight_e15_NoCut_Zee',
                             l2=Chain(name='L2_e20_tight_e15_NoCut_Zee', counter=210, seed='L1_2EM12'),
                             ef=DeduceChainConfig, efmerge=['EF_e20_tight', 'EF_e15_NoCut'],
                             fex=fexes.zee, hypo=hypos.zee),
        ]

MultiElectrons_IdScanDef += [

##2EM3 -> e5_tight1_e4_etcut_Jpsi_TRT (in coh PS with e5_tight1_e4_etcut_Jpsi) 
##- 2EM3 -> e5_tight1_e4_etcut_Jpsi_IdScan (in coh PS with e5_tight1_e4_etcut_Jpsi) 
##- 2EM3 -> e5_tight1_e4_etcut_Jpsi_SiTrl (in coh PS with e5_tight1_e4_etcut_Jpsi) 

    DiElectron_resonance('e5_tight1_e4_etcut_Jpsi_IdScan',
                         l2=Chain(name='L2_e5_tight1_e4_etcut_Jpsi_IdScan', counter=1010, seed='L1_2EM3'),
                         ef=DeduceChainConfig, efmerge=['EF_e5_tight1_IdScan', 'EF_e4_etcut_IdScan'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    DiElectron_resonance('e5_tight1_e4_etcut_Jpsi_SiTrk',
                         l2=Chain(name='L2_e5_tight1_e4_etcut_Jpsi_SiTrk', counter=1011, seed='L1_2EM3'),
                         ef=DeduceChainConfig, efmerge=['EF_e5_tight1_SiTrk', 'EF_e4_etcut_SiTrk'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    DiElectron_resonance('e5_tight1_e4_etcut_Jpsi_TRT',
                         l2=Chain(name='L2_e5_tight1_e4_etcut_Jpsi_TRT', counter=1012, seed='L1_2EM3'),
                         ef=DeduceChainConfig, efmerge=['EF_e5_tight1_TRT', 'EF_e4_etcut_TRT'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    DiElectron_resonance('e5_tight1_e4_etcut_Jpsi_L2StarB',
                         l2=Chain(name='L2_e5_tight1_e4_etcut_Jpsi_L2StarB', counter=367, seed='L1_2EM3'),
                         ef=DeduceChainConfig, efmerge=['EF_e5_tight1_L2StarB', 'EF_e4_etcut_L2StarB'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    DiElectron_resonance('e5_tight1_e4_etcut_Jpsi_L2StarC',
                         l2=Chain(name='L2_e5_tight1_e4_etcut_Jpsi_L2StarC', counter=368, seed='L1_2EM3'),
                         ef=DeduceChainConfig, efmerge=['EF_e5_tight1_L2StarC', 'EF_e4_etcut_L2StarC'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    DiElectron_resonance('e5_tight_e5_NoCut_Jpsi',
                         l2=Chain(name='L2_e5_tight_e5_NoCut_Jpsi', counter=5400, seed='L1_2EM3'),
                         ef=DeduceChainConfig, efmerge=['EF_e5_tight', 'EF_e5_NoCut'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),
    DiElectron_resonance('e5_tight_e4_etcut_Jpsi',
                         l2=Chain(name='L2_e5_tight_e4_etcut_Jpsi', counter=969, seed='L1_2EM3'),
                         ef=DeduceChainConfig, efmerge=['EF_e5_tight', 'EF_e4_etcut'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),
    
    DiElectron_resonance('e5_tight1_e4_etcut_Jpsi',
                         l2=Chain(name='L2_e5_tight1_e4_etcut_Jpsi', counter=916, seed='L1_2EM3'),
                         ef=DeduceChainConfig, efmerge=['EF_e5_tight1', 'EF_e4_etcut'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    DiElectron_resonance('e5_tight_e7_etcut_Jpsi',
                         l2=Chain(name='L2_e5_tight_e7_etcut_Jpsi', counter=5402, seed='L1_2EM3'),
                         ef=DeduceChainConfig, efmerge=[ 'EF_e7_etcut', 'EF_e5_tight'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    DiElectron_resonance('e5_tight_e9_etcut_Jpsi',
                         l2=Chain(name='L2_e5_tight_e9_etcut_Jpsi', counter=968, seed='L1_2EM3_EM7'), # New L1
                         ef=DeduceChainConfig, efmerge=['EF_e9_etcut','EF_e5_tight' ],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    DiElectron_resonance('e5_tight1_e9_etcut_Jpsi',
                         l2=Chain(name='L2_e5_tight1_e9_etcut_Jpsi', counter=917, seed='L1_2EM3_EM6'),
                         ef=DeduceChainConfig, efmerge=['EF_e9_etcut','EF_e5_tight1' ],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),


    DiElectron_resonance('e5_tight_e4_etcut_Jpsi_SiTrk',
                         l2=Chain(name='L2_e5_tight_e4_etcut_Jpsi_SiTrk', counter=967, seed='L1_2EM3'),
                         ef=DeduceChainConfig, efmerge=['EF_e5_tight_SiTrk', 'EF_e4_etcut_SiTrk'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    DiElectron_resonance('e5_tight_e14_etcut_Jpsi',
                         l2=Chain(name='L2_e5_tight_e14_etcut_Jpsi', counter=143, seed='L1_2EM3_EM12'),
                         ef=DeduceChainConfig, efmerge=['EF_e5_tight', 'EF_e14_etcut_L1EM12'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    DiElectron_resonance('e5_tight1_e14_etcut_Jpsi',
                         l2=Chain(name='L2_e5_tight1_e14_etcut_Jpsi', counter=918, seed='L1_2EM3_EM12'),
                         ef=DeduceChainConfig, efmerge=['EF_e5_tight1', 'EF_e14_etcut_L1EM12'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    DiElectron_resonance('e7_tight_e14_etcut_Jpsi',
                         l2=Chain(name='L2_e7_tight_e14_etcut_Jpsi', counter=155, seed='L1_2EM7_EM10'),
                         ef=DeduceChainConfig, efmerge=['EF_e7_tight_L1EM7', 'EF_e14_etcut'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    DiElectron_resonance('e9_tight_e4_etcut_Jpsi',
                         l2=Chain(name='L2_e9_tight_e4_etcut_Jpsi', counter=964, seed='L1_2EM3_EM7'),
                         ef=DeduceChainConfig, efmerge=['EF_e9_tight', 'EF_e4_etcut'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    DiElectron_resonance('e9_tight1_e4_etcut_Jpsi',
                         l2=Chain(name='L2_e9_tight1_e4_etcut_Jpsi', counter=919, seed='L1_2EM3_EM6'),
                         ef=DeduceChainConfig, efmerge=['EF_e9_tight1', 'EF_e4_etcut'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    DiElectron_resonance('e14_tight_e4_etcut_Jpsi',
                         l2=Chain(name='L2_e14_tight_e4_etcut_Jpsi', counter=940, seed='L1_2EM3_EM12'),
                         ef=DeduceChainConfig, efmerge=['EF_e14_tight', 'EF_e4_etcut'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    DiElectron_resonance('e14_tight1_e4_etcut_Jpsi',
                         l2=Chain(name='L2_e14_tight1_e4_etcut_Jpsi', counter=924, seed='L1_2EM3_EM12'),
                         ef=DeduceChainConfig, efmerge=['EF_e14_tight1', 'EF_e4_etcut'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    DiElectron_resonance('e5_tight_e4_etcut_Jpsi_TRT',
                         l2=Chain(name='L2_e5_tight_e4_etcut_Jpsi_TRT', counter=966, seed='L1_2EM3'),
                         ef=DeduceChainConfig, efmerge=['EF_e5_tight_TRT', 'EF_e4_etcut_TRT'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    DiElectron_resonance('2e5_tight_Jpsi',
                         l2=Chain(name='L2_2e5_tight_Jpsi', counter=965, seed='L1_2EM3'),
                         ef=DeduceChainConfig, efmerge=['EF_e5_tight', 'EF_e5_tight'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    DiElectron_resonance('2e5_tight1_Jpsi',
                         l2=Chain(name='L2_2e5_tight1_Jpsi', counter=963, seed='L1_2EM3'),
                         ef=DeduceChainConfig, efmerge=['EF_e5_tight1', 'EF_e5_tight1'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),



    DiElectron_resonance('e9_tight_e5_tight_Jpsi',
                         l2=Chain(name='L2_e9_tight_e5_tight_Jpsi', counter=964, seed='L1_2EM3_EM7'),
                         ef=DeduceChainConfig, efmerge=['EF_e9_tight', 'EF_e5_tight'],
                         fex=fexes.jpsiee, hypo=hypos.jpsiee),

    ]


if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.VHThresholdMenus()\
    or TriggerFlags.triggerMenuSetup() in Lvl1Flags.ALFAv2Menus()):
    MultiElectrons_IdScanDef += [
        DiElectron_resonance('e24vh_tight1_e15_NoCut_Zee',
                             l2=Chain(name='L2_e24vh_tight1_e15_NoCut_Zee', counter=142, seed='L1_EM18VH'),
                             ef=DeduceChainConfig, efmerge=['EF_e24vh_tight1', 'EF_e15_NoCut'],
                             fex=fexes.ef_zee, hypo=hypos.ef_zee2,
                             l2_fex=fexes.l2_zee, l2_hypo=hypos.l2_zee2),

]


if TriggerFlags.triggerMenuSetup() in Lvl1Flags.VHThresholdMenus():
    MultiElectrons_IdScanDef += [
        DiElectron_resonance('2e15vh_loose_Zee',
                             l2=Chain(name='L2_2e15vh_loose_Zee', counter=5965, seed='L1_2EM10VH'),
                             ef=DeduceChainConfig, efmerge=['EF_e15_loose_EM10VH', 'EF_e15_loose_EM10VH'],
                             fex=fexes.ef_zee, hypo=hypos.ef_zee2,
                             l2_fex=fexes.l2_zee, l2_hypo=hypos.l2_zee2),

        DiElectron_resonance('2e15vh_loose1_Zee',
                             l2=Chain(name='L2_2e15vh_loose1_Zee', counter=5966, seed='L1_2EM10VH'),
                             ef=DeduceChainConfig, efmerge=['EF_e15_loose1_EM10VH', 'EF_e15_loose1_EM10VH'],
                             fex=fexes.ef_zee, hypo=hypos.ef_zee2,
                             l2_fex=fexes.l2_zee, l2_hypo=hypos.l2_zee2),

        DiElectron_resonance('e15vh_loose1_e12Tvh_loose1_Zee',
                             l2=Chain(name='L2_e15vh_loose1_e12Tvh_loose1_Zee', counter=141, seed='L1_2EM10VH'),
                             ef=DeduceChainConfig, efmerge=['EF_e15_loose1_EM10VH', 'EF_e12T_loose1_EM10VH'],
                             fex=fexes.ef_zee, hypo=hypos.ef_zee2,
                             l2_fex=fexes.l2_zee, l2_hypo=hypos.l2_zee2),

        #new upsi triggers 90881 PJB
        DiElectron_resonance('e20vhT_tight1_g6T_etcut_Upsi',
                             l2=Chain(name='L2_e20vhT_tight1_g6T_etcut_Upsi', counter=581, seed='L1_2EM6_EM16VH'),
                             ef=DeduceChainConfig, efmerge=['EF_e20vhT_tight1', 'EF_g6T_etcut'],
                             fex=fexes.upsiee, hypo=hypos.upsiee),

        DiElectron_resonance('e20vhT_medium1_g6T_etcut_Upsi',
                             l2=Chain(name='L2_e20vhT_medium1_g6T_etcut_Upsi', counter=582, seed='L1_2EM6_EM16VH'),
                             ef=DeduceChainConfig, efmerge=['EF_e20vhT_medium1', 'EF_g6T_etcut'],
                             fex=fexes.upsiee, hypo=hypos.upsiee),
        
        ]




from TrigEgammaHypo.TrigEFHadronicRecoilConfig import TrigEFHadronicRecoil_recoil40
fexes.recoil40 = TrigEFHadronicRecoil_recoil40()

class Chain_Wenu(ChainTemplate):
    def __init__(self, signature, l2, ef=DeduceChainConfig,
                 l2merge=None, efmerge=None, recoil_fex=None):
        ChainTemplate.__init__(self, signature, l2=l2, ef=ef)

        self.addGroups('RATE:MultiElectron').addGroups('BW:Egamma')
        self.addStreams('egamma')

        self.tmp_generated = False
        self.efmerge = efmerge
        self.recoil_fex = recoil_fex
        try:
            # Signle electron chains are still configured in the old way.
            # So HLTChain objects may not be created yet. If so, try again
            # in generateMenu()
            if efmerge != None: self.merge(efnames=efmerge)
            self.tmp_generated = True
        except:
            self.tmp_generated = False
            self.efmerge = efmerge
        if self.tmp_generated:
            self.addSequence(self.efmerge, [self.recoil_fex], \
                             self.ef.chain_name).addSignature('EF')
        
    def generateMenu(self, tpc):
        if not self.tmp_generated:
            if self.efmerge != None: self.merge(efnames=self.efmerge)
            self.addSequence(self.efmerge, [self.recoil_fex], \
                             self.ef.chain_name).addSignature('EF')
            self.tmp_generated = True
    pass


if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or \
       TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus() or \
       TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus() or \
       TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus():

    MultiElectrons_IdScanDef += [
        Chain_Wenu('e25_loose2_xe20_noMu_recoil40',
                   ef=Chain(name='EF_e25_loose2_xe20_noMu_recoil40'), 
                   l2=Chain(name='L2_e25_loose2_xe20_noL2', counter=6210, seed='L1_EM16VH'),
                   efmerge=['EF_xe20_noL2_noMu', 'EF_e25_loose2' ], # MET must be first
                   recoil_fex=fexes.recoil40),
        ]

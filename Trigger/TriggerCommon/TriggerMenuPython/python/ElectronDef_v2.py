# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Electron trigger slice signatures  """

__author__  = 'P. Bell'
__version__=""
__doc__="Implementation of single electron signatures"

from AthenaCommon.Logging import logging
from ChainTemplate import *
from AthenaCommon.SystemOfUnits import GeV

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags

#GENERAL FEXES
fexes = FreeStore()

from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_SwSeed
from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_SwSeed_NoCut 
from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_SwCluster
from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_Ringer
from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_cells
fexes.eGamma       = T2CaloEgamma_eGamma()
fexes.SwSeed       = T2CaloEgamma_SwSeed()
fexes.SwSeed_NoCut = T2CaloEgamma_SwSeed_NoCut()
fexes.SwCluster    = T2CaloEgamma_SwCluster()
fexes.Ringer       = T2CaloEgamma_Ringer()
fexes.cells        = T2CaloEgamma_cells("T2CaloEgamma_cells")

from TrigIDSCAN.TrigIDSCAN_Config               import TrigIDSCAN_eGamma
from TrigSiTrack.TrigSiTrack_Config             import TrigSiTrack_eGamma, TrigSiTrack_eGamma_robust
from TrigL2TRTSegFinder.TrigTRTSegFinder_Config import TrigTRTSegFinder_eGamma
fexes.IDSCAN_eGamma         = TrigIDSCAN_eGamma()
fexes.SiTrack_eGamma        = TrigSiTrack_eGamma()
fexes.SiTrack_eGamma_robust = TrigSiTrack_eGamma_robust()
fexes.TRTSegFinder_eGamma   = TrigTRTSegFinder_eGamma()

from InDetTrigRecExample.EFInDetConfig       import *
from TrigEgammaHypo.TrigEFTrackHypoConfig    import *
from TrigEgammaHypo.TrigEFElectronHypoConfig import *
from TrigEgammaRec.TrigEgammaRecConfig       import *
fexes.EFIDInsideOut_Electron        = TrigEFIDInsideOut_Electron("Electron").getSequence()
fexes.EFIDOutsideInTRTOnly_Electron = TrigEFIDOutsideInTRTOnly_Electron().getSequence()
fexes.EFIDCombined_Electron         = TrigEFIDCombined_Electron().getSequence()
fexes.EgammaRec_eGamma              = TrigEgammaRec_eGamma()

from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma
from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma_cells
from TrigCaloRec.TrigCaloRecConfig import  TrigCaloTowerMaker_eGamma
from TrigCaloRec.TrigCaloRecConfig import  TrigCaloClusterMaker_slw
fexes.CaloCellMaker_eGamma      = TrigCaloCellMaker_eGamma()
fexes.CaloCellMaker_eGamma_cells= TrigCaloCellMaker_eGamma_cells()
fexes.CaloTowerMaker_eGamma     = TrigCaloTowerMaker_eGamma()
fexes.CaloClusterMaker_slw      = TrigCaloClusterMaker_slw()

#SPECIFIC FEXES
from TrigEgammaHypo.TrigL2ElectronFexConfig  import *
fexes.l2electronfex_IdScan = L2ElectronFex_IdScan()

#SPECIFIC HYPOS
hypos = FreeStore() 

from TrigEgammaHypo.TrigL2CaloHypoConfig     import *
hypos.l2calohypo_e20_loose = L2CaloHypo_e20_loose()

from TrigEgammaHypo.TrigL2ElectronHypoConfig import *
hypos.l2electronhypo_e20_loose = L2ElectronHypo_e20_loose()
hypos.eftrackhypo_e20_loose    = EFTrackHypo_e20_loose()
hypos.efegammahypo_e20_loose   = TrigEFElectronHypo_e20_loose()


#############################################################################
class ElectronTemplate(ChainTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig):
        """
        Base template for all electron chains. Assigning groups & streams.
        Deduction rules:
        EF chain name  --- from signature name i.e. EF_+sig
        """
        if ef == DeduceChainConfig:
            ef = Chain()
        ef.replace_if_default('name', 'EF_%s'%sig)
                    
        #print 'ef name', ef.name, sig
        ChainTemplate.__init__(self, sig, l2, ef)
        # basic config which every electron chain should have
        self.addStreams('egamma')
        
#############################################################################
class L2EFChain_electron(ElectronTemplate):
    """
   Idscan elecron chains
    """
    def __init__(self, sig, l2, ef, l2hypo=None, efhypo=None, l1threshold=None):
        # generate EF chain name to be identical as the signature name (if not given)
        ElectronTemplate.__init__(self, sig, l2, ef)
        
        # find l1 threshold
        l1_tes = l2.seed.split('_')[-1]
        
        # l2 sequence
        self.addSequence(l1_tes, [fexes.eGamma, hypos.l2calohypo_e20_loose], self.l2.chain_name+'cl' ).addSignature()
        self.continueSequence([fexes.IDSCAN_eGamma], self.l2.chain_name+'id').addSignature()
        self.continueSequence([fexes.l2electronfex_IdScan, hypos.l2electronhypo_e20_loose], self.l2.chain_name).addSignature()

        # ef sequence (do not have to specify explicitly this are ef algs)
        self.continueSequence([fexes.CaloCellMaker_eGamma, fexes.CaloTowerMaker_eGamma, fexes.CaloClusterMaker_slw], self.ef.chain_name+'calo').addSignature()
        self.continueSequence(fexes.EFIDInsideOut_Electron + [hypos.eftrackhypo_e20_loose], self.ef.chain_name+'id').addSignature()
        self.continueSequence([fexes.EgammaRec_eGamma , hypos.efegammahypo_e20_loose], self.ef.chain_name).addSignature() 

        self.addGroups('BW:Egamma')
        if('EMPTY' in l2.seed):
            self.addGroups('RATE:Cosmic_Electron')
        else:
            self.addGroups('RATE:SingleElectron')
                
        pass

#############################################################################

l2chains = FreeStore()
l2chains.e20_loose = Chain(name='L2_e20_loose', counter=7, seed='L1_EM14')
l2chains.e22vh_loose = Chain(name='L2_e22vh_loose', counter=7, seed='L1_EM16VH')

Electrons_v2 = [

    L2EFChain_electron('e20_loose', l2=l2chains.e20_loose, ef=Chain(counter=7)),

]


##e22vh_loose for pp_v4 is defined in ElectronChain.py
if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus()):

    Electrons_v2 += [
        L2EFChain_electron('e22vh_loose', l2=l2chains.e22vh_loose, ef=Chain(counter=7)),
        ]
    


 

            
        

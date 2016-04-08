# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################
# SliceDef file for Bjet chains/signatures
###########################################################################

from string import atoi
import re
from AthenaCommon.Logging import logging

from TriggerMenuPython.HltConfig import *
from TriggerMenuPython.TriggerPythonConfig import *
from TriggerMenuPython.ChainTemplate import *
from TriggerJobOpts.TriggerFlags import TriggerFlags

from TrigMuonHypo.TrigMuonHypoConfig import *

from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Jet, TrigSiTrack_Muon
from TrigIDSCAN.TrigIDSCAN_Config   import TrigIDSCAN_Jet

from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxConfig import *

from TrigBjetHypo.TrigBjetFexConfig  import *
from TrigBjetHypo.TrigBjetHypoConfig import *

from TrigBjetHypo.TrigLeptonJetFexConfig  import *
from TrigBjetHypo.TrigLeptonJetHypoConfig import *

from TrigBjetHypo.TrigLeptonJetFexAllTEConfig    import *
from TrigBjetHypo.TrigLeptonJetMatchAllTEConfig  import *

from TrigmuComb.TrigmuCombConfig import *
from TrigmuFast.TrigmuFastConfig import *

from InDetTrigRecExample.EFInDetConfig import *
from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxConfig import EFHistoPrmVtx_Jet

#from TrigBjetHypo.TrigLeptonJetFexConfig  import EFLeptonJetFex
#from TrigBjetHypo.TrigLeptonJetHypoConfig import EFLeptonJetHypo

# will be removed soon
from TrigBjetHypo.TrigLeptonJetFexAllTEConfig  import EFLeptonJetFexAllTE, EFLeptonJetFexAllTE_CloseBy

from TrigSecVtxFinder.TrigSecVtxFinderConfig import L2SecVtx, L2SecVtx_JetB  , L2SecVtx_JetF  
from InDetTrigVxSecondary.InDetTrigVxSecondary_LoadTools import TrigVxSecondary_EF  

from TrigGenericAlgs.TrigGenericAlgsConf import  AcceptAnyInput

# For multi-roi
from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxAllTEConfig import *
from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxComboConfig import *

from TrigBjetHypo.TrigEFBjetSequenceAllTEConfig import * 
from TrigBjetHypo.TrigBjetEtHypoConfig          import *


if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    mu4_j10L2chain = "L2_mu4"
    mu4_j10EFchain = "EF_mu4"
    mu4Thr = "MU0"
else:
    mu4_j10L2chain = "L2_mu4T"
    mu4_j10EFchain = "EF_mu4T"
    mu4Thr = "MU4"

import re

###########################################################################
# FEXES
###########################################################################
fexes = FreeStore()

if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or
    TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus() or
    TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus()):
##    TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus()): 
    fexes.l2_bjet = getBjetFexInstance("L2","2011","SiTrack") 
    fexes.l2_BjetFex_IDScan = getBjetFexInstance("L2","2011","IDScan") 
    fexes.l2_bjet_JetA = getBjetFexInstance("L2","2011","JetA") 
    fexes.l2_bjet_JetB = getBjetFexInstance("L2","2011","JetB")
    fexes.ef_bjet = getBjetFexInstance("EF","2011","EFID")
else:
    fexes.l2_bjet = getBjetFexInstance("L2","2012","SiTrack")
    fexes.l2_bjet_JetA = getBjetFexInstance("L2","2012","JetA")
    fexes.l2_bjet_JetB = getBjetFexInstance("L2","2012","JetB")
    fexes.l2_bjet_JetF = getBjetFexInstance("L2","2012","JetF")
    fexes.l2_bjet_JetFR = getBjetFexInstance("L2","2012","JetFR") 
    fexes.l2_bjet_JetB_l15 = getBjetFexInstance("L1.5","2012","JetB")
    fexes.ef_bjet = getBjetFexInstance("EF","2012","EFID")

fexes.l2_TrigIDSCAN_Jet = TrigIDSCAN_Jet()
fexes.l2_TrigSiTrack_Jet = TrigSiTrack_Jet()
fexes.l2_HistoPrmVtx_IDScan = L2HistoPrmVtx_IDScan()
fexes.l2_HistoPrmVtx_SiTrack = L2HistoPrmVtx_SiTrack()
fexes.l2_HistoPrmVtx_JetA = L2HistoPrmVtx_JetA()
fexes.l2_HistoPrmVtx_JetB = L2HistoPrmVtx_JetB()
#fexes.l2_HistoPrmVtx_JetF = L2HistoPrmVtx_JetF()
fexes.l2_HistoPrmVtxAllTE_JetA = L2HistoPrmVtxAllTE_JetA()
fexes.l2_HistoPrmVtxAllTE_JetB = L2HistoPrmVtxAllTE_JetB()
fexes.l2_HistoPrmVtxAllTE_JetF = L2HistoPrmVtxAllTE_JetF()
fexes.l2_HistoPrmVtxAllTE_JetFR = L2HistoPrmVtxAllTE_JetFR()
fexes.l2_HistoPrmVtxCombo_Jet = L2HistoPrmVtxCombo_Jet()
fexes.l2_SecVtx = L2SecVtx()
fexes.l2_SecVtx_JetB = L2SecVtx_JetB()
fexes.l2_SecVtx_JetF = L2SecVtx_JetF()

fexes.ef_bjetSequence=getEFBjetAllTEInstance()

fexes.ef_bjetEtHypo_StartSequence_15GeV = getBjetEtHypoInstance("EF","StartSequence","15GeV")
fexes.ef_bjetEtHypo_StartSequence_35GeV = getBjetEtHypoInstance("EF","StartSequence","35GeV")

#fexes.ef_bjetEtHypo_Vertexing_35GeV = getBjetEtHypoInstance("EF","Vertexing","35GeV")
#fexes.ef_bjetEtHypo_Vertexing_15GeV = getBjetEtHypoInstance("EF","Vertexing","15GeV")

fexes.ef_bjetEtHypo_Btagging_10GeV  = getBjetEtHypoInstance("EF","Btagging","10GeV")
fexes.ef_bjetEtHypo_Btagging_15GeV  = getBjetEtHypoInstance("EF","Btagging","15GeV")
fexes.ef_bjetEtHypo_Btagging_25GeV  = getBjetEtHypoInstance("EF","Btagging","25GeV")
fexes.ef_bjetEtHypo_Btagging_35GeV  = getBjetEtHypoInstance("EF","Btagging","35GeV")
fexes.ef_bjetEtHypo_Btagging_45GeV  = getBjetEtHypoInstance("EF","Btagging","45GeV")
fexes.ef_bjetEtHypo_Btagging_55GeV  = getBjetEtHypoInstance("EF","Btagging","55GeV")
fexes.ef_bjetEtHypo_Btagging_65GeV  = getBjetEtHypoInstance("EF","Btagging","65GeV")
fexes.ef_bjetEtHypo_Btagging_80GeV  = getBjetEtHypoInstance("EF","Btagging","80GeV")
fexes.ef_bjetEtHypo_Btagging_110GeV = getBjetEtHypoInstance("EF","Btagging","110GeV")
fexes.ef_bjetEtHypo_Btagging_145GeV = getBjetEtHypoInstance("EF","Btagging","145GeV")
fexes.ef_bjetEtHypo_Btagging_165GeV = getBjetEtHypoInstance("EF","Btagging","165GeV")
fexes.ef_bjetEtHypo_Btagging_180GeV = getBjetEtHypoInstance("EF","Btagging","180GeV")
fexes.ef_bjetEtHypo_Btagging_220GeV = getBjetEtHypoInstance("EF","Btagging","220GeV")
fexes.ef_bjetEtHypo_Btagging_240GeV = getBjetEtHypoInstance("EF","Btagging","240GeV")
fexes.ef_bjetEtHypo_Btagging_280GeV = getBjetEtHypoInstance("EF","Btagging","280GeV")
fexes.ef_bjetEtHypo_Btagging_360GeV = getBjetEtHypoInstance("EF","Btagging","360GeV")

fexes.ef_HistoPrmVtx_Jet = EFHistoPrmVtx_Jet()
fexes.ef_VxSecondary_EF = TrigVxSecondary_EF()
fexes.ef_HistoPrmVtxAllTE_Jet = EFHistoPrmVtxAllTE_Jet()
fexes.ef_HistoPrmVtxCombo_Jet = EFHistoPrmVtxCombo_Jet()

fexes.ef_bjet_tracks = TrigEFIDSequence("Bjet","bjet","InsideOut").getSequence()
fexes.alwaysAcceptAnyInput = AcceptAnyInput()


###########################################################################
# HYPOS
###########################################################################
hypos = FreeStore()

hypos.l2_bjet            = getBjetHypoInstance("L2","2011","medium") 
hypos.l2_b_loose         = getBjetHypoInstance("L2","2011-2","loose")
hypos.l2_b_medium        = getBjetHypoInstance("L2","2011-2","medium")
hypos.l2_b_tight         = getBjetHypoInstance("L2","2011-2","tight") 

hypos.l2_b_loose_2012    = getBjetHypoInstance("L2","2012","loose")
hypos.l2_b_medium_2012   = getBjetHypoInstance("L2","2012","medium")
hypos.l2_b_tight_2012    = getBjetHypoInstance("L2","2012","tight") 

hypos.l2_BjetHypo_NoCut  = getBjetHypoNoCutInstance("L2") 


hypos.ef_bjet            = getBjetHypoInstance("EF","2011","medium") 
hypos.ef_b_loose         = getBjetHypoInstance("EF","2011-2","loose")
hypos.ef_b_medium        = getBjetHypoInstance("EF","2011-2","medium")
hypos.ef_b_tight         = getBjetHypoInstance("EF","2011-2","tight")

hypos.ef_b_loose_2012    = getBjetHypoInstance("EF","2012","loose")
hypos.ef_b_medium_2012   = getBjetHypoInstance("EF","2012","medium")
hypos.ef_b_tight_2012    = getBjetHypoInstance("EF","2012","tight")

hypos.ef_b_looseEF_2012  = getBjetHypoInstance("EF","2012","looseEF")
hypos.ef_b_mediumEF_2012 = getBjetHypoInstance("EF","2012","mediumEF")
hypos.ef_b_tightEF_2012  = getBjetHypoInstance("EF","2012","tightEF")

hypos.ef_BjetHypo_NoCut  = getBjetHypoNoCutInstance("EF")


#matched chains
hypos.l2_mufastHypo = MufastHypoConfig("Muon","4GeV")
hypos.l2_LeptonJetHypo = L2LeptonJetHypo()
fexes.l2_LeptonJetFex = L2LeptonJetFex()
hypos.l2_muCombHypo = MucombHypoConfig("Muon", "4GeV")
hypos.ef_LeptonJetHypo = EFLeptonJetHypo()
fexes.ef_LeptonJetFex = EFLeptonJetFex()
fexes.l2_mucomb=TrigmuCombConfig('Muon', 'SITRACK')
fexes.ef_LeptonJetFexAllTE = EFLeptonJetFexAllTE()

if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus()):
    fexes.ef_LeptonJetFexAllTE = EFLeptonJetFexAllTE_CloseBy()
else:
    fexes.ef_LeptonJetFexAllTE_R  = getLeptonJetMatchAllTEInstance("CloseBy","R")
    fexes.ef_LeptonJetFexAllTE_RZ = getLeptonJetMatchAllTEInstance("CloseBy","RZ")    


helpers = FreeStore()
helpers.te_no_keys = ['cosmic', 'firstempty', 'unpaired', 'iso', 'noniso']


from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_JetA,TrigL2SiTrackFinder_JetB,TrigL2SiTrackFinder_JetC

fexes.l2_theTrigL2SiTrackFinder_JetA = TrigL2SiTrackFinder_JetA()
fexes.l2_theTrigL2SiTrackFinder_JetB = TrigL2SiTrackFinder_JetB()
fexes.l2_theTrigL2SiTrackFinder_JetC = TrigL2SiTrackFinder_JetC()

try:
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_JetF
    fexes.l2_theTrigL2SiTrackFinder_JetF = TrigL2SiTrackFinder_JetF()
except:
    fexes.l2_theTrigL2SiTrackFinder_JetF = None

try:
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_JetF_Refit
    fexes.l2_theTrigL2SiTrackFinder_JetFR = TrigL2SiTrackFinder_JetF_Refit()
except:
    fexes.l2_theTrigL2SiTrackFinder_JetFR = None

try:
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_JetBC
    fexes.l2_theTrigL2SiTrackFinder_JetBC = TrigL2SiTrackFinder_JetBC()
except:
    fexes.l2_theTrigL2SiTrackFinder_JetBC = None


##FOR NEW IDT chains###
from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence
from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Jet
theTrigEFIDInsideOut_MergedDP_Bjet = TrigEFIDSequence("Bjet","bjet","DataPrep").getSequence()
FTF_bjet = TrigFastTrackFinder_Jet()
theTrigEFIDInsideOutMerged_Bjet =TrigEFIDSequence("Bjet","bjet","InsideOutMerged").getSequence()
theTrigEFIDIVTX = TrigEFIDSequence("Bjet","bjet","Vtx").getSequence() 



###########################################################################
###########################################################################
# CHAIN TEMPLATE DEFINITIONS HERE,
# CHAIN DEFINITIONS AT THE END
###########################################################################
###########################################################################


###########################################################################
# Chain template class for v4 menus
###########################################################################
class ChainTemplate_Bjet_v4(ChainTemplate):
    def __init__(self, signature, counter, l2thresh=None, effexthresh=35):
        """Chain template for HLT seeded b-jet chain.
           Assumes name of form "XbYY_cutLevel_jetChain" where "JetChain" is defined in JetDef.py
           All seeding is extracted from jet definition
           ToDo:  EF-only jets, L1.5 jet support, asymmetric jet thresholds, support of chains with HT
        """
        #################
        #first decode name to understand b-jet setup and find the corresponding jet chain at EF and L2
        #################
        try:
            parts=signature.split('_')

            #---------------------------
            #multiplicity of bjet part
            #---------------------------
            l2mult=1
            efmult=1
            bjet=parts[0].split('b')
            if bjet[0]:
                l2mult=int(bjet[0])
                efmult=int(bjet[0])

            #---------------------------
            #b-jet threshold
            #---------------------------
            thresh=int(bjet[1])

            #---------------------------
            #asymmetric jet trigger thresholds
            #so far convention is to take the 2nd one
            #---------------------------
            jets = 0
            #determine number of jet thresholds in chain
            for index in range(len(parts)):                
                if 'j' in parts[index] and not 'L2j' in parts[index]:
                    jets+=1            

            jetparts=[0.]*jets
            jetlist=['meow']*jets
            jetmult=[0.]*jets
            jetthresh=[9999.]*jets
            i=0
            for index in range(len(parts)):
                if 'j' in parts[index] and not 'L2j' in parts[index]:
                    jetlist[i]=parts[index]
                    jetparts=parts[index].split('j')
                    jetthresh[i]=jetparts[1]
                    jetmult[i]=jetparts[0]
                    i+=1
                    
            #---------------------------
            #setting l2thresh if not passed from chain def
            #---------------------------
            l2threshgiven=1
            if not l2thresh:
                l2threshgiven=0
                if (jets>1):
                    thresh2=0
                    thresh2=int(jetthresh[jets-1])
                    l2thresh=thresh2-5
                elif (jets == 1):
                    l2thresh=thresh-5  #assume standard 5 GeV Jet pt convention for L2 and EF jets
                else:
                    log.warn('no jet part in bjet chain!!!')

            #---------------------------
            #e.g medium, loose...
            #---------------------------
            cutLevel=parts[1]

            #---------------------------
            #obfind jet chain definitions, so we can get seed
            #---------------------------

            jetChainName='EF_'+('_'.join(parts[2:])) #EF jet chain name

                        
            #---------------------------
            #Special cases for naming conventions
            #---------------------------
            if ('_1bL2' in jetChainName):
                l2mult=1
                efmult=int(bjet[0])
                jetChainName=jetChainName.replace('_1bL2','')
            if ('test' in jetChainName):
                jetChainName=jetChainName.replace('_test','')
            if ('FTK' in jetChainName):
                jetChainName=jetChainName.replace('_FTK','')
                if ('Refit' in jetChainName):
                    jetChainName=jetChainName.replace('_Refit','')

            if ('IDT' in jetChainName):
                jetChainName=jetChainName.replace('_IDT','')

            #---------------------------
            # obtain jet chain and l2 jet chain
            #---------------------------            
            jetChain=TriggerPythonConfig.currentTriggerConfig().getHLTChain(jetChainName)
            if not jetChain:
                log.error('Failed to find jet chain with name %s' % jetChainName)
                raise ValueError

            l2JetChainName=jetChain.lower_chain_name
            l2JetChain=TriggerPythonConfig.currentTriggerConfig().getHLTChain(l2JetChainName)

            #---------------------------            
            # set l2 name
            #---------------------------

            myl2JetChainName=l2JetChainName[3:]
            rg = re.search('(?<=j)\d+', l2JetChainName)
            if rg:
                mythresh = int(rg.group(0))
        

            if (l2threshgiven == 1):                    
                    if (mythresh != l2thresh):
                        l2thresh_str='%d' % l2thresh
                        mythresh_str='%d' % mythresh
                        newl2JetChainName=l2JetChainName.replace(mythresh_str,l2thresh_str,1)
                        myl2JetChainName=newl2JetChainName[3:]

            
            if ('L2_rd0_filled_NoAlg' in l2JetChainName):
                l2name='L2_rd0_filled_NoAlg'
                l2tename='L2_rd0_filled_NoAlg'            
            elif (l2mult ==1):
                if cutLevel in ['looseEF','mediumEF','tightEF']:
                    l2name='L2_%s' % (myl2JetChainName)
                    l2tename='L2_%s' % (myl2JetChainName)
                else:
                    l2name='L2_b%d_%s_%s' % (l2thresh,cutLevel,myl2JetChainName)
                    l2tename='L2_b%d_%s_%s' % (l2thresh,cutLevel,myl2JetChainName)                  
            else:
                if cutLevel in ['looseEF','mediumEF','tightEF']:
                    l2name='L2_%s' % (l2JetChainName[3:])
                    l2tename='L2_%s' % (l2JetChainName[3:])
                else:
                    l2name='L2_%db%d_%s_%s' % (l2mult,l2thresh,cutLevel,myl2JetChainName)
                    l2tename='L2_%db%d_%s_%s' % (l2mult,l2thresh,cutLevel,myl2JetChainName)

            if ('_test' in signature):                
                l2name='L2_b%d_%s_%s_test' % (l2thresh,cutLevel,myJetChainName)
                l2tename='L2_b%d_%s_%s_test' % (l2thresh,cutLevel,myJetChainName)

            if ('_FTK' in signature):                
                l2name='L2_b%d_%s_%s_FTK' % (l2thresh,cutLevel,myl2JetChainName)
                l2tename='L2_b%d_%s_%s_FTK' % (l2thresh,cutLevel,myl2JetChainName)

            if ('_FTK_Refit' in signature):                
                l2name='L2_b%d_%s_%s_FTK_Refit' % (l2thresh,cutLevel,myl2JetChainName)
                l2tename='L2_b%d_%s_%s_FTK_Refit' % (l2thresh,cutLevel,myl2JetChainName)
            if ('_IDT' in signature):
                l2name='L2_b%d_%s_%s_IDT' % (l2thresh,cutLevel,myl2JetChainName)
                l2tename='L2_b%d_%s_%s_IDT' % (l2thresh,cutLevel,myl2JetChainName)
                
            l2=Chain(name=l2name,counter=counter,seed=l2JetChain.lower_chain_name)

            #---------------------------            
            #set ef name
            #---------------------------            
            if (efmult == 1): 
                efname='EF_%s' % signature
                eftename='EF_b%d_%s_%s' % (thresh,cutLevel,jetChainName[3:])
                efte6 = 'EF_b%d_%s' % (thresh,jetChainName[3:])
            else:
                efname='EF_%s' % signature
                eftename='EF_%db%d_%s_%s' % (efmult,thresh,cutLevel,jetChainName[3:])
                efte6='EF_%db%d_%s' % (efmult,thresh,jetChainName[3:])

            if ('_test' in signature):
                efname='EF_%s' % signature
                eftename='EF_%db%d_%s_%s_test' % (efmult,thresh,cutLevel,jetChainName[3:])
                efte6='EF_%db%d_%s_test' % (efmult,thresh,jetChainName[3:])
                
            ef=Chain(name=efname)                

        except:
            log.error('Unsupported signature name: %s' % signature)
            raise

        ChainTemplate.__init__(self, signature, l2, ef)
        if l2name == 'L2_rd0_filled_NoAlg':
            self.merge([jetChainName], onlyEF=True)
        else:
            self.merge([jetChainName])

        ###########
        # TE naming
        ###########

        if ((jetChainName == 'EF_j15_a4tchad') or (jetChainName == 'EF_j25_a4tchad') or (jetChainName == 'EF_j35_a4tchad_L1J15')):
            l2seed =  'L2_%s' % myl2JetChainName                    
        elif (cutLevel in ['looseEF','mediumEF','tightEF']) and (l2name == 'L2_rd0_filled_NoAlg'):
            l2seed = jetChainName #this gives the EFjetchainname
        elif (cutLevel in ['looseEF','mediumEF','tightEF']) and (l2name != 'L2_rd0_filled_NoAlg'):
            l2seed =  'L2_%s' % myl2JetChainName
        #elif (cutLevel == 'NoCut') and (l2name == 'L2_rd0_filled_NoAlg'):
        elif (  'NoCut' in cutLevel ) and (l2name == 'L2_rd0_filled_NoAlg'):
            l2seed = 'L2_%s' % myl2JetChainName
        else:
            l2seeds=l2JetChain.siglist[-1].tes
            l2seed=l2seeds[-1]
        
        #if not ('j%d_' % l2thresh) in l2seed:
        #    log.warn('L2 jet input (%s) to b-jet chain %s does not match at threshold level' % (l2seed,signature))
            
        #efseeds=jetChain.siglist[-1].tes
        #efseed=efseeds[-1]
        #if not ('j%d_' % thresh) in efseed:
        #    log.warn('EF jet input (%s) to b-jet chain %s does not match at threshold level' % (efseed,signature))

        #---------------------------            
        #get the hypos for ef and l2
        #ToDo: run nothing at L2 for looseEF etc   
        #---------------------------            
        #if cutLevel == 'NoCut':
        if 'NoCut' in cutLevel :
            l2_hypo=eval('hypos.l2_BjetHypo_NoCut')
            ef_hypo=eval('hypos.ef_BjetHypo_NoCut')
        elif cutLevel in ['looseEF','mediumEF','tightEF']:
            ef_hypo=eval('hypos.ef_b_%s_2012' % cutLevel)
        else:
            l2_hypo=eval('hypos.l2_b_%s_2012' % cutLevel)
            ef_hypo=eval('hypos.ef_b_%s_2012' % cutLevel)
        
        ###########
        # L2 part
        ###########

        if 'FTK' in signature:
            trkAlgo='JetF'
            if 'Refit' in signature:
                trkAlgo='JetFR'                
        elif 'L2_b50_NoCut_j50_c4cchad' in l2name:
            trkAlgo='Jet'
        else:
            trkAlgo='JetB'

        te1 = '%s_%s'       % (l2seed, trkAlgo)
        te2 = '%s_AllTE_%s' % (l2seed, trkAlgo      )
        te3 = '%s_COMBO_%s' % (l2seed, trkAlgo      )
        te4 = '%s_SV_%s'    % (l2seed, trkAlgo   )     
        
        if cutLevel not in ['looseEF','mediumEF','tightEF']:

            if 'FTK' in signature:

                if 'Refit' in signature:

                    self.addSequence(l2seed, [ fexes.l2_theTrigL2SiTrackFinder_JetFR], te1)
                    self.addSequence(te1, [ fexes.l2_HistoPrmVtxAllTE_JetFR], te2)
                    self.addSequence([te1, te2], [ fexes.l2_HistoPrmVtxCombo_Jet], te3)
                    self.addSequence(te3, [fexes.l2_SecVtx_JetF], te4)
                    
                    if l2name == 'L2_rd0_filled_NoAlg':
                        pass
                    else:
                        self.addSequence(te4, [fexes.l2_bjet_JetFR,l2_hypo], l2tename).addSignature('L2',mult=l2mult)

                else:

                    self.addSequence(l2seed, [ fexes.l2_theTrigL2SiTrackFinder_JetF], te1)
                    self.addSequence(te1, [ fexes.l2_HistoPrmVtxAllTE_JetF], te2)
                    self.addSequence([te1, te2], [ fexes.l2_HistoPrmVtxCombo_Jet], te3)
                    self.addSequence(te3, [fexes.l2_SecVtx_JetF], te4)
                    
                    if l2name == 'L2_rd0_filled_NoAlg':
                        pass
                    else:
                        self.addSequence(te4, [fexes.l2_bjet_JetF,l2_hypo], l2tename).addSignature('L2',mult=l2mult)


            elif 'IDT' in l2name:
                pass

            else:
                if 'L2_b50_NoCut_j50_c4cchad' in l2name:
                    self.addSequence(l2seed, [fexes.l2_theTrigL2SiTrackFinder_JetA,fexes.l2_theTrigL2SiTrackFinder_JetB,fexes.l2_theTrigL2SiTrackFinder_JetC,fexes.l2_theTrigL2SiTrackFinder_JetBC], te1)            
                else:
                    self.addSequence(l2seed, [fexes.l2_theTrigL2SiTrackFinder_JetB], te1)

                self.addSequence(te1, [ fexes.l2_HistoPrmVtxAllTE_JetB], te2)
                self.addSequence([te1, te2], [ fexes.l2_HistoPrmVtxCombo_Jet], te3)
                self.addSequence(te3, [fexes.l2_SecVtx_JetB,fexes.l2_bjet_JetB], te4)
                
                if (jetChainName.find('L2FS')!=-1):
                    self.addSequence(te4, [fexes.l2_bjet_JetB_l15,l2_hypo], l2tename).addSignature('L2',mult=l2mult)
                elif l2name == 'L2_rd0_filled_NoAlg':
                    pass
                else:
                    self.addSequence(te4, [fexes.l2_bjet_JetB,l2_hypo], l2tename).addSignature('L2',mult=l2mult)                                                

        ##te1 = '%s_JetB' % l2seed 
        ##te2 = '%s_AllTE_JetB' % l2seed
        ##te3 = '%s_COMBO_JetB' % l2seed
        ##te4 = '%s_SV_JetB' % l2seed        
        ##
        ##if cutLevel not in ['looseEF','mediumEF','tightEF']:
        ##    
        ##        self.addSequence(l2seed, [ fexes.l2_theTrigL2SiTrackFinder_JetB], te1)
        ##        self.addSequence(te1, [ fexes.l2_HistoPrmVtxAllTE_JetB], te2)
        ##        self.addSequence([te1, te2], [ fexes.l2_HistoPrmVtxCombo_Jet], te3)
        ##        self.addSequence(te3, [fexes.l2_SecVtx_JetB], te4)
        ##
        ##    
        ##    if (jetChainName.find('L2FS')!=-1):
        ##        self.addSequence(te4, [fexes.l2_bjet_JetB_l15,l2_hypo], l2tename).addSignature('L2',mult=l2mult)
        ##    elif l2name == 'L2_rd0_filled_NoAlg':
        ##        pass
        ##    else:
        ##        self.addSequence(te4, [fexes.l2_bjet_JetB,l2_hypo], l2tename).addSignature('L2',mult=l2mult)
            
        ###########
        # EF part
        ###########
        #TODO: needs fix 90545 comment #9 to get allTE fex with a given jet threshold cut
        #also doesn't handle an HT trigger as that adds another signature step

        efseed = 'EF_j15_a4tchad'
        ef2 ='EF_BjetSeed'
        ef3 ='EF_BjetSeed_EtCut%sGeV' % effexthresh
        ef4 ='EF_BjetSeed_EtCut%sGeV_EFID'  % effexthresh
        ef5 ='EF_BjetSeed_EtCut%sGeV_AllTEPrmVtx'  % effexthresh
        ef6 ='EF_BjetSeed_EtCut%sGeV_ComboPrmVtx'  % effexthresh
        if (efmult==1):
            ef7 ='EF_b%d_%s' % (thresh,jetChainName[3:])
            eftename ='EF_b%d_%s_%s' % (thresh,cutLevel,jetChainName[3:])
        else:
            ef7 = 'EF_%db%d_%s' % (efmult,thresh,jetChainName[3:])
            eftename = 'EF_%db%d_%s_%s' % (efmult,thresh,cutLevel,jetChainName[3:])
            
        if '_IDT' in efname:
            ef4 ='EF_BjetSeed_EtCut%sGeV_EFID_IDT'  % effexthresh
            ef5 ='EF_BjetSeed_EtCut%sGeV_AllTEPrmVtx_IDT'  % effexthresh
            ef6 ='EF_BjetSeed_EtCut%sGeV_ComboPrmVtx_IDT'  % effexthresh
            if (efmult==1):
                ef7 ='EF_b%d_%s_IDT' % (thresh,jetChainName[3:])
                eftename ='EF_b%d_%s_%s_IDT' % (thresh,cutLevel,jetChainName[3:])
            else:
                ef7 = 'EF_%db%d_%s_IDT' % (efmult,thresh,jetChainName[3:])
                eftename = 'EF_%db%d_%s_%s_IDT' % (efmult,thresh,cutLevel,jetChainName[3:])

        ef_fex=eval('fexes.ef_bjetSequence')
        if (effexthresh == 35):
            ef_ethypo_startseq=eval('fexes.ef_bjetEtHypo_StartSequence_35GeV')
        elif (effexthresh ==15):
            ef_ethypo_startseq=eval('fexes.ef_bjetEtHypo_StartSequence_15GeV')
        else:
            log.error('Failed to find ef with name fexes.ef_bjetEtHypo_StartSequence_%sGeV' % effexthresh)

        threshstr = '%dGeV'% thresh
        ef_EtHypo_Btagging=eval('fexes.ef_bjetEtHypo_Btagging_%s'% threshstr)
        

        self.addSequence(efseed, [ef_fex], ef2)
        self.addSequence(ef2, ef_ethypo_startseq, ef3)
        
        if "_IDT" in efname:
            
            trkseq_bj= list(theTrigEFIDInsideOut_MergedDP_Bjet)
            trkseq_bj.append(FTF_bjet)
            trkseq_bj+=theTrigEFIDInsideOutMerged_Bjet

            self.addSequence(ef3,  trkseq_bj, ef4)
            self.addSequence(ef4, theTrigEFIDIVTX, ef5)             
            self.addSequence([ef4, ef5], [EFHistoPrmVtxCombo_Jet()], ef6)
            self.addSequence(ef6, [fexes.ef_VxSecondary_EF,ef_EtHypo_Btagging], ef7)        
            self.addSequence(ef7, [fexes.ef_bjet, ef_hypo], eftename).addSignature('EF',mult=efmult)            

        else:
            self.addSequence(ef3, fexes.ef_bjet_tracks, ef4)
            self.addSequence(ef4, [EFHistoPrmVtxAllTE_Jet()], ef5)
            self.addSequence([ef4, ef5], [EFHistoPrmVtxCombo_Jet()], ef6)
            self.addSequence(ef6, [fexes.ef_VxSecondary_EF,ef_EtHypo_Btagging], ef7)        
            self.addSequence(ef7, [fexes.ef_bjet, ef_hypo], eftename).addSignature('EF',mult=efmult)        

        if efmult==1:
            for x in ['RATE:SingleBjet','BW:Bjets']: self.addGroups(x)
        else:
            for x in ['RATE:MultiBjet','BW:Bjets']: self.addGroups(x)
        self.addStreams('jets')


###########################################################################
#
###########################################################################
class ChainTemplate_Bjet(ChainTemplate):
    def __init__(self, signature, l2, ef=DeduceChainConfig, l2_hypo=None, ef_hypo=None, l1roi=None, l2seed=None, efseed=None):
        """Chain template for single b-jet chain. Assumes L2 seed contains L1_JXX|L1_JXX_*"""
        ChainTemplate.__init__(self, signature, l2, ef)

        # Get L1 threshold name
        rois = []
        if l1roi != None:
            if type(l1roi) == type(''): rois = [l1roi]
            else: rois.extend(l1roi)
        else:
            roi = ''
            mg = re.search('L1_(J\d+)', l2.seed)
            if mg: roi = mg.group(1)
            else:
                roi='J10'
            rois = [roi]


            
        if l2seed == None:
            self.addSequence(rois, [ TrigSiTrack_Jet(), fexes.l2_HistoPrmVtx_SiTrack,
                                     fexes.l2_bjet, l2_hypo ], self.l2.chain_name).addSignature('L2')
        else:
            self.addSequence(l2seed, [ TrigSiTrack_Jet(), fexes.l2_HistoPrmVtx_SiTrack,
                                       fexes.l2_bjet, l2_hypo ], self.l2.chain_name).addSignature('L2')


        if efseed == None:
            self.continueSequence(fexes.ef_bjet_tracks + \
                                  [ EFHistoPrmVtx_Jet(), fexes.ef_VxSecondary_EF,  
                                    #[ fexes.ef_HistoPrmVtx_Jet, 
                                    fexes.ef_bjet, ef_hypo ], self.ef.chain_name).addSignature('EF')
        else:
            ef2 = '%s_ALLTEbjet'% efseed

            parts=signature.split('_')
            bjet=parts[0].split('b')
            thresh=int(bjet[1])
            threshstr = '%dGeV'% thresh
            ef_fex=eval('fexes.ef_bjetSequence')
            
            self.addSequence(efseed, [ef_fex], ef2)
            self.continueSequence(fexes.ef_bjet_tracks + \
                                [ EFHistoPrmVtx_Jet(), fexes.ef_VxSecondary_EF,  
                                  fexes.ef_bjet, ef_hypo ], self.ef.chain_name).addSignature('EF')



            
        for x in ['RATE:SingleBjet','BW:Bjets']: self.addGroups(x)
        self.addStreams('jets')


###########################################################################
# variation for both trk-fexes
###########################################################################
class ChainTemplate_Bjet_IDTrkNoCut(ChainTemplate):
    def __init__(self, signature, l2, ef=DeduceChainConfig, l2_hypo=None, ef_hypo=None, l1roi=None, l2seed=None, efseed=None):
        """Chain template for ssupporting b-jet chain. Assumes L2 seed contains L1_JXX|L1_JXX_*"""
        ChainTemplate.__init__(self, signature, l2, ef)

        # Get L1 threshold name
        rois = []
        if l1roi != None:
            if type(l1roi) == type(''): rois = [l1roi]
            else: rois.extend(l1roi)
        else:
            roi = ''
            mg = re.search('L1_(J\d+)', l2.seed)
            if mg: roi = mg.group(1)
            else:
                roi='J10'
            rois = [roi]


        if l2seed == None:            
            self.addSequence(rois, [ TrigSiTrack_Jet(), fexes.l2_HistoPrmVtx_SiTrack,
                                     fexes.l2_TrigIDSCAN_Jet, fexes.l2_HistoPrmVtx_IDScan,
                                     L2SecVtx(),  
                                     fexes.l2_bjet, fexes.l2_BjetFex_IDScan,
                                     hypos.l2_BjetHypo_NoCut], self.l2.chain_name).addSignature('L2')
        else:
            self.addSequence(l2seed, [ TrigSiTrack_Jet(), fexes.l2_HistoPrmVtx_SiTrack,
                                       fexes.l2_TrigIDSCAN_Jet, fexes.l2_HistoPrmVtx_IDScan,
                                       L2SecVtx(),  
                                       fexes.l2_bjet, fexes.l2_BjetFex_IDScan,
                                       hypos.l2_BjetHypo_NoCut], self.l2.chain_name).addSignature('L2')
            


        if efseed == None:
            self.continueSequence(fexes.ef_bjet_tracks + \
                                  [ EFHistoPrmVtx_Jet(), fexes.ef_VxSecondary_EF,  
                                    #[ fexes.ef_HistoPrmVtx_Jet, 
                                    fexes.ef_bjet, ef_hypo ], self.ef.chain_name).addSignature('EF')
        else:
            ef2 = '%s_ALLTEbjet'% efseed

            parts=signature.split('_')
            bjet=parts[0].split('b')
            thresh=int(bjet[1])
            
            threshstr = '%dGeV'% thresh
            ef_fex=eval('fexes.ef_bjetSequence')
            
            self.addSequence(efseed, [ef_fex], ef2)
            self.continueSequence(fexes.ef_bjet_tracks + \
                                [ EFHistoPrmVtx_Jet(), fexes.ef_VxSecondary_EF,  
                                  fexes.ef_bjet, ef_hypo ], self.ef.chain_name).addSignature('EF')


  
        for x in ['RATE:SingleBjet','BW:Bjets']: self.addGroups(x)
        self.addStreams('jets')


###########################################################################
# multiroi 
###########################################################################
class ChainTemplate_Bjet_L2Star_IDTrkNoCut_multiroi(ChainTemplate):
    def __init__(self, signature, l2, ef=DeduceChainConfig, l2_hypo=None, ef_hypo=None, l1roi=None, l2seed=None, efseed=None):
        """Chain template for ssupporting b-jet chain. Assumes L2 seed contains L1_JXX|L1_JXX_*"""
        ChainTemplate.__init__(self, signature, l2, ef)
        
        # Get L1 threshold name
        rois = []
        if l1roi != None:
            if type(l1roi) == type(''): rois = [l1roi]
            else: rois.extend(l1roi)
        else:
            roi = ''
            mg = re.search('L1_(J\d+)', l2.seed)
            if mg: roi = mg.group(1)
            else:
                roi='J10'
            rois = [roi]
            
        #L2
        te1 = 'JetA_%s' % rois
        te2 = 'AllTE_JetA_%s' % rois
        te3 = 'Combo_JetA%s'% rois

        te4 = 'JetB_%s'% rois
        te5 = 'AllTE_JetB_%s'% rois
        te6 = 'COMBO_JetB_%s'% rois

        te7 = self.l2.chain_name
        #EF
        ef1 = 'Bjetef_%s' % rois
        ef2 = 'ALLTE_EF_%s'% rois
        ef22= 'Combo_EF_%s'% rois
        ef3 = self.ef.chain_name

        # Are these needed? fexes.l2_bjet_JetA, fexes.l2_bjet_JetB


        if l2seed == None:            
            self.addSequence(rois,      [ fexes.l2_theTrigL2SiTrackFinder_JetA], te1)
        else:
            self.addSequence(l2seed,      [ fexes.l2_theTrigL2SiTrackFinder_JetA], te1)
            
        self.addSequence(te1,       [ fexes.l2_HistoPrmVtxAllTE_JetA], te2) 
        self.addSequence([te1,te2], [ fexes.l2_HistoPrmVtxCombo_Jet], te3)
        self.addSequence(te3,       [ fexes.l2_theTrigL2SiTrackFinder_JetB],te4)
        self.addSequence(te4,       [ fexes.l2_HistoPrmVtxAllTE_JetB],te5)
        self.addSequence([te4,te5], [ fexes.l2_HistoPrmVtxCombo_Jet],te6)           
        self.addSequence(te6, [ fexes.l2_bjet_JetA, fexes.l2_bjet_JetB,
                                hypos.l2_BjetHypo_NoCut], te7).addSignature('L2')

        if efseed == None:
            self.continueSequence(fexes.ef_bjet_tracks, ef1)
            self.addSequence(ef1,       [fexes.ef_HistoPrmVtxAllTE_Jet],ef2)
            self.addSequence([ef1,ef2], [fexes.ef_HistoPrmVtxCombo_Jet],ef22)
            self.addSequence(ef22,      [fexes.ef_bjet, hypos.ef_BjetHypo_NoCut],ef3).addSignature('EF')
        else:
            ef0 = '%s_ALLTEbjet'% efseed
            parts=signature.split('_')
            bjet=parts[0].split('b')
            thresh=int(bjet[1])

            threshstr = '%dGeV'% thresh
            ef_fex=eval('fexes.ef_bjetSequence')
            
            self.addSequence(efseed, [ef_fex], ef2)
            self.addSequence(ef0, [fexes.ef_bjet_tracks], ef1)
            self.addSequence(ef1,       [fexes.ef_HistoPrmVtxAllTE_Jet],ef2)
            self.addSequence([ef1,ef2], [fexes.ef_HistoPrmVtxCombo_Jet],ef22)
            self.addSequence(ef22,      [fexes.ef_bjet, hypos.ef_BjetHypo_NoCut],ef3).addSignature('EF')
            
        
        for x in ['RATE:SingleBjet','BW:Bjets']: self.addGroups(x)
        self.addStreams('jets')

                
###########################################################################
# Merged Chains
###########################################################################
class MergedChain(ChainTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, tomerge=None, OnlyEF=False, groups=None,
                 straighten=False,
                 runSequencially=False):
        if ef == DeduceChainConfig:
            ef = Chain()
        ef.replace_if_default('name', 'EF_%s'%sig)
        ChainTemplate.__init__(self, sig, l2, ef)        

        self.tomerge = tomerge
        self.OnlyEF = OnlyEF
        self.straighten = straighten # Straighten combined chain signature list
        self.runSequencially = runSequencially
        
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
            for x in ['RATE:SingleBjet','BW:Bjets']: self.addGroups(x)
        self.addStreams('jets')
        
    def generateMenu(self, tpc):
        if not self.seq_generated:
            self.merge(self.tomerge, onlyEF=self.OnlyEF)
            self.seq_generated = True
        if self.straighten:
            # L2
            telist, tes = [], []
            for sig in self.l2.siglist:
                for te in sig.tes:
                    telist.append(te)
            self.l2.siglist = []
            for te in telist:
                if te not in tes:
                    tes.append(te)
                    self.l2.addHLTSignature([te])
            # EF
            telist, tes = [], []
            for sig in self.ef.siglist:
                for te in sig.tes:
                    telist.append(te)
            self.ef.siglist = []
            for te in telist:
                if te not in tes:
                    tes.append(te)
                    self.ef.addHLTSignature([te])
        if self.runSequencially:
            def reorder(chain):
                # Reorder signature list to have 1 TE in each signature
                # Original siglist[sig][te] reordered to
                # sl[0][0],sl[1][0],sl[2][0], ..., sl[0][1],sl[1][1], ...
                telist = []
                n = max(map(lambda x: x.sigcounter, chain.siglist) )
                n = max(map(lambda x: len(x.tes), chain.siglist) )
                sig1 = 1
                if len(chain.siglist)>0: sig1 = chain.siglist[0].sigcounter
                for i in range(n):
                    for sig in chain.siglist:
                        if len(sig.tes) > i and sig.tes[i] not in telist:
                            telist.append(sig.tes[i])
                chain.siglist = []
                #print 'telist: ', telist
                for te in telist:
                    chain.addHLTSignature([te], sigcounter=sig1)
                    sig1 += 1
            reorder(self.l2)
            reorder(self.ef)
    pass


###########################################################################
# matched
###########################################################################
class ChainTemplate_Bjet_matched(ChainTemplate):
    def __init__(self, signature, l2, ef=DeduceChainConfig, l1threshold=None, l2seed=None, efseed=None):
        """Chain template for ssupporting b-jet chain. Assumes L2 seed contains L1_JXX|L1_JXX_*"""
        ChainTemplate.__init__(self, signature, l2, ef)

        # Get L1 threshold name
         # guess L1 threshold
        if l1threshold == None:
            l1threshold = self.l2.lower_chain_name.split('_')

        l1mu=None
        l1j=None
        for th in l1threshold:
            if 'MU' in th: l1mu=th
            if 'J' in th: l1j=th
#        print 'TWEET igb: mu', l1mu
#        print 'TWEET igb: j', l1j
            
        br = self.l2.chain_name.split('_')
        te = '_'.join(b for b in br[1:] if b not in helpers.te_no_keys)

        #L2
        te1 = 'L2_muComb%s' % te
        te2 = 'L2_LeptonJet%s' % te
        te3 = self.l2.chain_name
        self.addSequence(l1mu, [TrigmuFastConfig("Muon"),
                                hypos.l2_mufastHypo], 'L2_muFast%s'%te)
        self.continueSequence([TrigSiTrack_Muon(),
                               TrigmuCombConfig("Muon","SITRACK"),
                               hypos.l2_muCombHypo], te1)
        self.addSequence([te1, l1j], [fexes.l2_LeptonJetFex,
                                      hypos.l2_LeptonJetHypo], te2).addSignature(sigcounter=2)

        if l2seed == None:            
            self.addSequence(l1j, [TrigSiTrack_Jet(), fexes.l2_HistoPrmVtx_SiTrack,
                                   fexes.l2_TrigIDSCAN_Jet, fexes.l2_HistoPrmVtx_IDScan,
                                   fexes.l2_bjet, fexes.l2_BjetFex_IDScan], te3).addSignature()
        else:
            self.addSequence(l2seed, [TrigSiTrack_Jet(), fexes.l2_HistoPrmVtx_SiTrack,
                                   fexes.l2_TrigIDSCAN_Jet, fexes.l2_HistoPrmVtx_IDScan,
                                   fexes.l2_bjet, fexes.l2_BjetFex_IDScan], te3).addSignature()
            
        #EF
        if efseed == None:
            self.addSequence(te3, fexes.ef_bjet_tracks + \
                             [fexes.ef_HistoPrmVtx_Jet, fexes.ef_VxSecondary_EF, fexes.ef_bjet], 'EF_Bjet%s'%te).addSignature()
            self.addSequence([self.l2.chain_name, 'EF_Bjet%s'%te],
                             [fexes.ef_LeptonJetFex, hypos.ef_LeptonJetHypo],self.ef.chain_name).addSignature()

        else:
            ef2 = '%s_ALLTEbjet'% efseed
            parts=signature.split('_')
            bjet=parts[0].split('b')
            thresh=int(bjet[1])
            
            threshstr = '%dGeV'% thresh
            ef_fex=eval('fexes.ef_bjetSequence')
            
            self.addSequence(efseed, [ef_fex], ef2)
            self.addSequence(ef2, fexes.ef_bjet_tracks + \
                             [fexes.ef_HistoPrmVtx_Jet, fexes.ef_VxSecondary_EF, fexes.ef_bjet], 'EF_Bjet%s'%te).addSignature()
            self.addSequence([self.l2.chain_name, 'EF_Bjet%s'%te],
                             [fexes.ef_LeptonJetFex, hypos.ef_LeptonJetHypo],self.ef.chain_name).addSignature()
                        


        
        for x in ['RATE:SingleBjet','BW:Bjets']: self.addGroups(x)
        self.addStreams('jets')


###########################################################################
# L2Star code
###########################################################################
class ChainTemplate_Bjet_L2Star_IDTrkNoCut(ChainTemplate):
    def __init__(self, signature, l2, ef=DeduceChainConfig, l2_hypo=None, ef_hypo=None, l1roi=None, l2seed=None, efseed=None):
        """Chain template for ssupporting b-jet chain. Assumes L2 seed contains L1_JXX|L1_JXX_*"""
        ChainTemplate.__init__(self, signature, l2, ef)
        
        # Get L1 threshold name
        rois = []
        if l1roi != None:
            if type(l1roi) == type(''): rois = [l1roi]
            else: rois.extend(l1roi)
        else:
            roi = ''
            mg = re.search('L1_(J\d+)', l2.seed)
            if mg: roi = mg.group(1)
            else:
                roi='J10'
                rois = [roi]

        if l2seed == None:
            self.addSequence(rois, [ fexes.l2_theTrigL2SiTrackFinder_JetA, fexes.l2_HistoPrmVtx_JetA,
                                     fexes.l2_theTrigL2SiTrackFinder_JetB, fexes.l2_HistoPrmVtx_JetB,
                                     fexes.l2_bjet_JetA, fexes.l2_bjet_JetB,
                                     hypos.l2_BjetHypo_NoCut], self.l2.chain_name).addSignature('L2')
        else:
            self.addSequence(l2seed, [ fexes.l2_theTrigL2SiTrackFinder_JetA, fexes.l2_HistoPrmVtx_JetA,
                                       fexes.l2_theTrigL2SiTrackFinder_JetB, fexes.l2_HistoPrmVtx_JetB,
                                       fexes.l2_bjet_JetA, fexes.l2_bjet_JetB,
                                       hypos.l2_BjetHypo_NoCut], self.l2.chain_name).addSignature('L2')

        if efseed == None:
            self.continueSequence(fexes.ef_bjet_tracks + \
                                  [ fexes.ef_HistoPrmVtx_Jet, fexes.ef_VxSecondary_EF,
                                    #[ fexes.ef_HistoPrmVtx_Jet,
                                    fexes.ef_bjet, hypos.ef_BjetHypo_NoCut], self.ef.chain_name).addSignature('EF')
            
        else:
            ef2 = '%s_ALLTEbjet'% efseed
            
            parts=signature.split('_')
            bjet=parts[0].split('b')
            thresh=int(bjet[1])

            threshstr = '%dGeV'% thresh
            ef_fex=eval('fexes.ef_bjetSequence')
            
            self.addSequence(efseed, [ef_fex], ef2)
            self.continueSequence(fexes.ef_bjet_tracks + \
                                  [ fexes.ef_HistoPrmVtx_Jet, fexes.ef_VxSecondary_EF,
                                    #[ fexes.ef_HistoPrmVtx_Jet,
                                    fexes.ef_bjet, hypos.ef_BjetHypo_NoCut], self.ef.chain_name).addSignature('EF')
        
        for x in ['RATE:SingleBjet','BW:Bjets']: self.addGroups(x)
        self.addStreams('jets')
        

# ------
# --- the new L2Star code presnet in release 17 and above, configure the chain only there
try:
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_JetA
    print 'igb L2Star for bjets code exists'
    configL2Star=True
except:
    configL2Star=False
    print 'igb L2Star for bjets does not exist'



###########################################################################
###########################################################################
# v2/v3 chain definitions!!!
###########################################################################
###########################################################################

if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus()\
    or TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus()\
    or TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus()):
##    or TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus()):


    #----------------------
    # L2Star chains
    #----------------------
    if configL2Star :
        Bjets = [
            ChainTemplate_Bjet_L2Star_IDTrkNoCut('b10_L2Star_IDTrkNoCut', l2=Chain(name='L2_b10_L2Star_IDTrkNoCut', counter=1321, seed='L1_J10'),
                                                 l1roi='J10'),
            ]


        #HI chains
        if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MuonHIV1Menus() :
            efseq_mu4_MSonly='EF_muSA_mu4_MSonly'
        else:
            efseq_mu4_MSonly='EF_SA_mu4_MSonly'
            
            
        Bjets = [

        #----------------------
        # Chain Template_Bjets
        #----------------------
        
        ChainTemplate_Bjet('b10', l2=Chain(name='L2_b10', counter=151, seed='L1_J10'),
                           l2_hypo=hypos.l2_bjet, ef_hypo=hypos.ef_bjet, l1roi='J10'),
        
        #b_loose - L1J10
        ChainTemplate_Bjet('b10_loose', l2=Chain(name='L2_b10_loose', counter=142, seed='L1_J10'),
                           l2_hypo=hypos.l2_b_loose, ef_hypo=hypos.ef_b_loose, l1roi='J10'), 
        ChainTemplate_Bjet('b15_loose', l2=Chain(name='L2_b15_loose', counter=141, seed='L1_J15'),
                           l2_hypo=hypos.l2_b_loose, ef_hypo=hypos.ef_b_loose, l1roi='J15'), 
        
        #b_medium - L1J10
        ChainTemplate_Bjet('b10_medium', l2=Chain(name='L2_b10_medium', counter=145, seed='L1_J10'),
                           l2_hypo=hypos.l2_b_medium, ef_hypo=hypos.ef_b_medium, l1roi='J10'), 
        
        ChainTemplate_Bjet('b10_medium_4L1J10', l2=Chain(name='L2_b10_medium_4L1J10', counter=4077, seed='L1_4J10'),
                           l2_hypo=hypos.l2_b_medium, ef_hypo=hypos.ef_b_medium, l1roi='J10'), 
        
        ##    ChainTemplate_Bjet('b15_medium', l2=Chain(name='L2_b15_medium', counter=26, seed='L1_J15'),
        ##                       l2_hypo=hypos.l2_b_medium, ef_hypo=hypos.ef_b_medium, l1roi='J15'), 
        ##
        ##    ChainTemplate_Bjet('b15_medium_4L1J15', l2=Chain(name='L2_b15_medium_4L1J15', counter=152, seed='L1_4J15'),
        ##                       l2_hypo=hypos.l2_b_medium, ef_hypo=hypos.ef_b_medium, l1roi='J15'), 
        ##
        ##    ChainTemplate_Bjet('b15_medium_2L1J30', l2=Chain(name='L2_b15_medium_2L1J30', counter=168, seed='L1_2J30'),
        ##                       l2_hypo=hypos.l2_b_medium, ef_hypo=hypos.ef_b_medium, l1roi='J30'), 
        ##
        ##    ChainTemplate_Bjet('b15_medium_L1J75', l2=Chain(name='L2_b15_medium_L1J75', counter=464, seed='L1_J75'),
        ##                       l2_hypo=hypos.l2_b_medium, ef_hypo=hypos.ef_b_medium, l1roi='J15'),
        ##
        ##    ChainTemplate_Bjet('b15_medium_L12J15J50', l2=Chain(name='L2_b15_medium_L12J15J50', counter=465, seed='L1_2J15_J50'),
        ##                       l2_hypo=hypos.l2_b_medium, ef_hypo=hypos.ef_b_medium, l1roi='J15'),


        ChainTemplate_Bjet('b15_medium', l2=Chain(name='L2_b15_medium', counter=26, seed='L1_J15'),
                           l2_hypo=hypos.l2_b_medium_2012, ef_hypo=hypos.ef_b_medium_2012, l1roi='J15'), 
        
        ChainTemplate_Bjet('b15_medium_4L1J15', l2=Chain(name='L2_b15_medium_4L1J15', counter=152, seed='L1_4J15'),
                           l2_hypo=hypos.l2_b_medium_2012, ef_hypo=hypos.ef_b_medium_2012, l1roi='J15'), 
        
        ChainTemplate_Bjet('b15_medium_2L1J30', l2=Chain(name='L2_b15_medium_2L1J30', counter=168, seed='L1_2J30'),
                           l2_hypo=hypos.l2_b_medium_2012, ef_hypo=hypos.ef_b_medium_2012, l1roi='J30'), 
        
        ChainTemplate_Bjet('b15_medium_L1J75', l2=Chain(name='L2_b15_medium_L1J75', counter=464, seed='L1_J75'),
                           l2_hypo=hypos.l2_b_medium_2012, ef_hypo=hypos.ef_b_medium_2012, l1roi='J15'),
        
        ChainTemplate_Bjet('b15_medium_L12J15J50', l2=Chain(name='L2_b15_medium_L12J15J50', counter=465, seed='L1_2J15_J50'),
                           l2_hypo=hypos.l2_b_medium_2012, ef_hypo=hypos.ef_b_medium_2012, l1roi='J15'),
        
        
        ChainTemplate_Bjet('b20_medium', l2=Chain(name='L2_b20_medium', counter=27, seed='L1_J20'),
                           l2_hypo=hypos.l2_b_medium, ef_hypo=hypos.ef_b_medium, l1roi='J20'), 
        
        #    ChainTemplate_Bjet('b75_medium', l2=Chain(name='L2_b75_medium', counter=526, seed='L1_J75'),
        #                       l2_hypo=hypos.l2_b_medium, ef_hypo=hypos.ef_b_medium, l1roi='J75'),
        
        ChainTemplate_Bjet('b75_medium', l2=Chain(name='L2_b75_medium', counter=526, seed='L1_J75'),
                           l2_hypo=hypos.l2_b_medium_2012, ef_hypo=hypos.ef_b_medium_2012, l1roi='J75'),
    
        #b_tight - L1J10
        ChainTemplate_Bjet('b10_tight', l2=Chain(name='L2_b10_tight', counter=146, seed='L1_J10'),
                           l2_hypo=hypos.l2_b_tight, ef_hypo=hypos.ef_b_tight, l1roi='J10'), 
        
        ChainTemplate_Bjet('b10_tight_4L1J10', l2=Chain(name='L2_b10_tight_4L1J10', counter=154, seed='L1_4J10'),
                       l2_hypo=hypos.l2_b_tight, ef_hypo=hypos.ef_b_tight, l1roi='J10'), 
        
        ChainTemplate_Bjet('b10_tight_L1JE100', l2=Chain(name='L2_b10_tight_L1JE100', counter=538, seed='L1_JE100'),
                           l2_hypo=hypos.l2_b_tight, ef_hypo=hypos.ef_b_tight, l1roi='J10'), 
        
        ChainTemplate_Bjet('b10_tight_L1JE140', l2=Chain(name='L2_b10_tight_L1JE140', counter=539, seed='L1_JE140'),
                           l2_hypo=hypos.l2_b_tight, ef_hypo=hypos.ef_b_tight, l1roi='J10'), 
        
        
        ChainTemplate_Bjet('b15', l2=Chain(name='L2_b15', counter=651, seed='L1_J15'),
                           l2_hypo=hypos.l2_bjet, ef_hypo=hypos.ef_bjet, l1roi='J15'),


        #----------------------
        # ChainTemplate_Bjet_IDTrkNoCut
        #----------------------
        ChainTemplate_Bjet_IDTrkNoCut('b10_IDTrkNoCut', l2=Chain(name='L2_b10_IDTrkNoCut', counter=932, seed='L1_J10'),
                                      l1roi='J10'),
        
        ChainTemplate_Bjet_IDTrkNoCut('b15_IDTrkNoCut', l2=Chain(name='L2_b15_IDTrkNoCut', counter=4053, seed='L1_J15'),
                                      l1roi='J15'),
        
        ChainTemplate_Bjet_IDTrkNoCut('b20_IDTrkNoCut', l2=Chain(name='L2_b20_IDTrkNoCut', counter=4041, seed='L1_J20'),
                                      l1roi='J20'),

        
        #----------------------
        # ChainTemplate_Bjet_IDTrkNoCut_multiroi
        #----------------------
        ChainTemplate_Bjet_L2Star_IDTrkNoCut_multiroi('b10_L2Star_IDTrkNoCut_multiroi',
                                                  l2=Chain(name='L2_b10_L2Star_IDTrkNoCut_multiroi', counter=859, seed='L1_J10'),
                                                  l1roi='J10'),



        #----------------------
        # MergedChain
        #----------------------
        MergedChain('b10_j30_a4tc_EFFS_IDTrkNoCut', 
                    l2=Chain(name='L2_b10_j30_a4tc_EFFS_IDTrkNoCut', counter=564, seed='L1_J10'),
                    ef=Chain(name='EF_b10_j30_a4tc_EFFS_IDTrkNoCut', counter=564, seed='L2_b10_j30_a4tc_EFFS_IDTrkNoCut')).\
        merge(l2names=['L2_j25']).merge(l2names=['L2_b10_IDTrkNoCut']).merge(['EF_j30_a4tc_EFFS','EF_b10_IDTrkNoCut'],onlyEF=True),
        
        
        MergedChain('b10_EFj10_a4tc_EFFS_IDTrkNoCut', None,
                    ef=Chain(name='EF_b10_EFj10_a4tc_EFFS_IDTrkNoCut', counter=101, seed='L2_b10_IDTrkNoCut'),
                    tomerge=['EF_b10_IDTrkNoCut', 'EF_j10_a4tc_EFFS'],OnlyEF=True),
        
        MergedChain('b10_tight_4j30_a4tc_EFFS', l2=Chain(name='L2_b10_tight_4j25', counter=4067, seed='L1_4J10'),
                    ef=Chain(name='EF_b10_tight_4j30_a4tc_EFFS', counter=4067, seed='L2_b10_tight_4j25')).\
        merge(l2names=['L2_4j25']).merge(l2names=['L2_b10_tight_4L1J10']).merge(['EF_b10_tight_4L1J10', 'EF_4j30_a4tc_EFFS'],onlyEF=True),
        
        MergedChain('b10_medium_4j30_a4tc_EFFS', l2=Chain(name='L2_b10_medium_4j25', counter=100, seed='L1_4J10'),
                    ef=Chain(name='EF_b10_medium_4j30_a4tc_EFFS', counter=100, seed='L2_b10_medium_4j25')).\
        merge(l2names=['L2_4j25']).merge(l2names=['L2_b10_medium']).merge(['EF_b10_medium_4L1J10', 'EF_4j30_a4tc_EFFS'],onlyEF=True),
        
        MergedChain('b10_medium_j75_j55_2j30_a4tc_EFFS', l2=Chain(name='L2_b10_medium_j70_2j50_4j25', counter=108, seed='L1_4J10'),
                    ef=Chain(name='EF_b10_medium_j75_j55_2j30_a4tc_EFFS', counter=108, seed='L2_b10_medium_j70_2j50_4j25')).\
        merge(l2names=['L2_j70_2j50_4j25']).merge(l2names=['L2_b10_medium']).merge(['EF_b10_medium_4L1J10', 'EF_j75_j55_2j30_a4tc_EFFS'],onlyEF=True),
        
        MergedChain('b15_medium_4j45_a4tc_EFFS', l2=Chain(name='L2_b15_medium_4j40_4L1J15', counter=4880, seed='L1_4J15'),
                    ef=Chain(name='EF_b15_medium_4j45_a4tc_EFFS', counter=4880, seed='L2_b15_medium_4j40_4L1J15')).\
        merge(l2names=['L2_4j40_4L1J15']).merge(l2names=['L2_b15_medium_4L1J15']).merge(['EF_b15_medium_4L1J15', 'EF_4j45_a4tc_EFFS'],onlyEF=True),
        
        MergedChain('b15_medium_2j55_a4tc_EFFS', l2=Chain(name='L2_b15_medium_2j50', counter=881, seed='L1_2J30'),
                    ef=Chain(name='EF_b15_medium_2j55_a4tc_EFFS', counter=881, seed='L2_b15_medium_2j50')).\
        merge(l2names=['L2_2j50']).merge(l2names=['L2_b15_medium_2L1J30']).merge(['EF_b15_medium_2L1J30', 'EF_2j55_a4tc_EFFS'],onlyEF=True),
        
        
        MergedChain('b10_tight_j75_j55_2j30_a4tc_EFFS', l2=Chain(name='L2_b10_tight_j70_2j50_4j25', counter=110, seed='L1_4J10'),
                    ef=Chain(name='EF_b10_tight_j75_j55_2j30_a4tc_EFFS', counter=110, seed='L2_b10_tight_j70_2j50_4j25')).\
        merge(l2names=['L2_j70_2j50_4j25']).merge(l2names=['L2_b10_tight_4L1J10']).merge(['EF_b10_tight_4L1J10', 'EF_j75_j55_2j30_a4tc_EFFS'],onlyEF=True),    
        
        ### To be seeded by L1_2J15_J75
        MergedChain('b15_medium_j100_j40_a4tc_EFFS', l2=Chain(name='L2_b15_medium_j95_j35', counter=882, seed='L1_J75'),
                    ef=Chain(name='EF_b15_medium_j100_j40_a4tc_EFFS', counter=1882, seed='L2_b15_medium_j95_j35')).\
        merge(l2names=['L2_j95_j35']).merge(l2names=['L2_b15_medium_L1J75']).merge(['EF_b15_medium_L1J75', 'EF_j100_j40_a4tc_EFFS'],onlyEF=True),
        
        MergedChain('b15_medium_j100_j40_a4tc_EFFS_ht300', l2=Chain(name='L2_b15_medium_j95_j35', counter=882, seed='L1_J75'),
                    ef=Chain(name='EF_b15_medium_j100_j40_a4tc_EFFS_ht300', counter=883, seed='L2_b15_medium_j95_j35')).\
        merge(l2names=['L2_j95_j35']).merge(l2names=['L2_b15_medium_L1J75']).merge(['EF_b15_medium_L1J75', 'EF_j100_j40_a4tc_EFFS_ht300'],onlyEF=True),
        
        # To be seeded by L1_2J15_J50_XE35
        MergedChain('b15_medium_j75_j40_a4tc_EFFS', l2=Chain(name='L2_b15_medium_j70_j35', counter=884, seed='L1_2J15_J50'),
                    ef=Chain(name='EF_b15_medium_j75_j40_a4tc_EFFS', counter=884, seed='L2_b15_medium_j70_j35')).\
        merge(l2names=['L2_j70_j35_L12J15J50']).merge(l2names=['L2_b15_medium_L12J15J50']).merge(['EF_b15_medium_L12J15J50', 'EF_j75_j40_a4tc_EFFS_L12J15J50'],onlyEF=True),
        
        MergedChain('b75_medium_j100_a4tc_EFFS', l2=Chain(name='L2_b75_medium_j95', counter=886, seed='L1_J75'),
                    ef=Chain(name='EF_b75_medium_j100_a4tc_EFFS', counter=885, seed='L2_b75_medium_j95')).\
        merge(l2names=['L2_j95']).merge(l2names=['L2_b75_medium']).merge(['EF_b75_medium', 'EF_j100_a4tc_EFFS'],onlyEF=True),
        
        MergedChain('b75_medium_j100_a4tc_EFFS_ht300', l2=Chain(name='L2_b75_medium_j95', counter=886, seed='L1_J75'),
                    ef=Chain(name='EF_b75_medium_j100_a4tc_EFFS_ht300', counter=886, seed='L2_b75_medium_j95')).\
        merge(l2names=['L2_j95']).merge(l2names=['L2_b75_medium']).merge(['EF_b75_medium', 'EF_j100_a4tc_EFFS_ht300'],onlyEF=True),
        
        MergedChain('b75_medium_j100_a4tc_EFFS_ht200', l2=Chain(name='L2_b75_medium_j95', counter=886, seed='L1_J75'),
                    ef=Chain(name='EF_b75_medium_j100_a4tc_EFFS_ht200', counter=887, seed='L2_b75_medium_j95')).\
        merge(l2names=['L2_j95']).merge(l2names=['L2_b75_medium']).merge(['EF_b75_medium', 'EF_j100_a4tc_EFFS_ht200'],onlyEF=True),
        
        
        MergedChain('mu4_j10_a4tc_EFFS_matched', None,
                    ef=Chain(name='EF_mu4_j10_a4tc_EFFS_matched', counter=556,seed=mu4_j10L2chain),
                    tomerge=[mu4_j10EFchain, 'EF_j10_a4tc_EFFS'], OnlyEF=True).
        addSequence([mu4_j10EFchain, 'EF_j10_a4tc_EFFS'],[fexes.ef_LeptonJetFexAllTE], 'EF_mu4_j10_a4tc_EFFS_matched').addSignature(),
        
        MergedChain('mu4_j20_a4tc_EFFS_matched', None,
                    ef=Chain(name='EF_mu4_j20_a4tc_EFFS_matched', counter=557,seed='L2_mu4'),
                    tomerge=['EF_mu4', 'EF_j20_a4tc_EFFS'], OnlyEF=True).
        addSequence(['EF_mu4', 'EF_j20_a4tc_EFFS'],[fexes.ef_LeptonJetFexAllTE], 'EF_mu4_j20_a4tc_EFFS_matched').addSignature(),
        
        MergedChain('mu4_MSonly_j15_a2hi_EFFS_matched', None,
                    #l2=Chain(name='L2_mu4_matched', counter=1064, seed='L1_MU0'),
                    ef=Chain(name='EF_mu4_MSonly_j15_a2hi_EFFS_matched', counter=838,seed='L2_mu4_MSonly'),
                    tomerge=['EF_mu4_MSonly', 'EF_j15_a2hi_EFFS_L1ZDC'], OnlyEF=True).
        addSequence([efseq_mu4_MSonly, 'EF_j15_a2hi_EFFS_L1ZDC'],[fexes.ef_LeptonJetFexAllTE], 'EF_mu4_MSonly_j15_a2hi_EFFS_matched').addSignature(),
        
        #----------------------
        # Bjet_matched
        #----------------------
        #b5_matched
        ChainTemplate_Bjet_matched('mu4_L1J5_matched', l2=Chain(name='L2_mu4_L1J5_matched', counter=458, seed='L1_%s_J5' % mu4Thr)),
        
        #b10_matched
        ChainTemplate_Bjet_matched('mu4_L1J10_matched', l2=Chain(name='L2_mu4_L1J10_matched', counter=457, seed='L1_%s_J10' % mu4Thr)),
        
        #b20_matched
        ChainTemplate_Bjet_matched('mu4_L1J15_matched', l2=Chain(name='L2_mu4_L1J15_matched', counter=654, seed='L1_%s_J15' % mu4Thr)),
        
        #b20_matched
        ChainTemplate_Bjet_matched('mu4_L1J20_matched', l2=Chain(name='L2_mu4_L1J20_matched', counter=627, seed='L1_%s_J15' % mu4Thr), l1threshold=[mu4Thr, 'J20']),
        
        #b40_matched
        ChainTemplate_Bjet_matched('mu4_L1J30_matched', l2=Chain(name='L2_mu4_L1J30_matched', counter=655, seed='L1_%s_J30' % mu4Thr)),
        
        ChainTemplate_Bjet_matched('mu4_L1J50_matched', l2=Chain(name='L2_mu4_L1J50_matched', counter=656, seed='L1_%s_J50' % mu4Thr)),
        
        #b70_matched
        # Change the seed from L1_MU0_J55 to L1_MU0_J75
        ChainTemplate_Bjet_matched('mu4_L1J75_matched', l2=Chain(name='L2_mu4_L1J75_matched', counter=970, seed='L1_%s_J75' % mu4Thr)),
        

        #----------------------
        # Merged Chain with matched items
        #----------------------
        MergedChain('mu4_j100_a4tc_EFFS_L1matched',
                    l2=Chain(name='L2_mu4_j95_L1matched', counter=938, seed='L1_MU0_J75'),
                    ef=Chain(name='EF_mu4_j100_a4tc_EFFS_L1matched', counter=938, seed='L2_mu4_j95_L1matched'),
                    tomerge=['EF_j100_a4tc_EFFS', 'EF_mu4_L1J75_matched'], runSequencially=True
                    ),
        
        MergedChain('mu4_j135_a4tc_EFFS_L1matched',
                    l2=Chain(name='L2_mu4_j95_L1matched', counter=994, seed='L1_%s_J75' % mu4Thr),
                    ef=Chain(name='EF_mu4_j135_a4tc_EFFS_L1matched', counter=994, seed='L2_mu4_j95_L1matched'),
                    #    merge(l2names=['L2_mu4_j95_L1matched']).merge(l2names=['L2_j95']).merge(['EF_mu4_L1J75_matched', 'EF_j135_a4tc_EFFS'],onlyEF=True),
                    tomerge=['EF_j135_a4tc_EFFS', 'EF_mu4_L1J75_matched'], runSequencially=True),
        
        MergedChain('mu4_j180_a4tc_EFFS_L1matched',
                    l2=Chain(name='L2_mu4_j95_L1matched', counter=727, seed='L1_%s_J75' % mu4Thr),
                    ef=Chain(name='EF_mu4_j180_a4tc_EFFS_L1matched', counter=727, seed='L2_mu4_j95_L1matched'),
                    #    merge(l2names=['L2_mu4_j95_L1matched']).merge(l2names=['L2_j95']).merge(['EF_mu4_L1J75_matched', 'EF_j180_a4tc_EFFS'],onlyEF=True),
                    tomerge=['EF_j180_a4tc_EFFS', 'EF_mu4_L1J75_matched'], runSequencially=True),
        
        ]


###########################################################################
###########################################################################
# v4 chain defintions !!!
###########################################################################
###########################################################################

if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus() or
    TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus() or
    TriggerFlags.triggerMenuSetup() in Lvl1Flags.ALFAv2Menus()):

    Bjets = [

        #---------------------------------------------
        #primary single b-jet triggers (#91842)
        #---------------------------------------------
        ChainTemplate_Bjet_v4('b145_loose_j145_a4tchad',counter=663),
        ChainTemplate_Bjet_v4('b180_loose_j180_a4tchad_L2j140',counter=678,l2thresh=140),
        ChainTemplate_Bjet_v4('b220_loose_j220_a4tchad_L2j140',counter=679,l2thresh=140),
        ChainTemplate_Bjet_v4('b280_loose_j280_a4tchad_L2j140',counter=681,l2thresh=140),
        ChainTemplate_Bjet_v4('b360_loose_j360_a4tchad_L2j140',counter=683,l2thresh=140),
        
        ChainTemplate_Bjet_v4('b55_looseEF_j55_a4tchad',counter=687),
        ChainTemplate_Bjet_v4('b80_looseEF_j80_a4tchad',counter=696),
        ChainTemplate_Bjet_v4('b110_looseEF_j110_a4tchad',counter=700),
        ChainTemplate_Bjet_v4('b15_looseEF_j15_a4tchad',counter=669,effexthresh=15),
        ChainTemplate_Bjet_v4('b25_looseEF_j25_a4tchad',counter=680,effexthresh=15),
        ##EF
        ChainTemplate_Bjet_v4('b35_looseEF_j35_a4tchad_L1J15',counter=682),
        ChainTemplate_Bjet_v4('b45_looseEF_j45_a4tchad_L1J15',counter=686),


        #---------------------------------------------
        #L1.5 jets+bjets broken - 92152
        #---------------------------------------------
        ChainTemplate_Bjet_v4('b55_medium_4j55_a4tchad',counter=252),
        ChainTemplate_Bjet_v4('2b55_medium_4j55_a4tchad',counter=4248),
        ChainTemplate_Bjet_v4('2b55_loose_4j55_a4tchad',counter=246),
        ChainTemplate_Bjet_v4('b55_medium_3j55_a4tchad_L2FS_4L1J15',counter=715,l2thresh=15),
        ChainTemplate_Bjet_v4('b45_medium_3j45_a4tchad_L2FS_4L1J15',counter=5265,l2thresh=15),
        ChainTemplate_Bjet_v4('b35_medium_3j35_a4tchad_L2FS_4L1J15',counter=6922,l2thresh=15),

        ChainTemplate_Bjet_v4('2b35_medium_3j35_a4tchad_L2FS_4L1J15',counter=241,l2thresh=15),
        ChainTemplate_Bjet_v4('2b45_medium_3j45_a4tchad_L2FS_4L1J15',counter=6743,l2thresh=15),
        ChainTemplate_Bjet_v4('2b55_medium_3j55_a4tchad_L2FS_4L1J15',counter=4245,l2thresh=15),


        #---------------------------------------------
        # not combined b-jet triggers (#91828)
        #---------------------------------------------
        #(i) 1b/2j and 2b/2j triggers
        ChainTemplate_Bjet_v4('2b55_loose_j110_j55_a4tchad',counter=7201),
        ChainTemplate_Bjet_v4('b55_medium_j110_j55_a4tchad',counter=7202),
        ChainTemplate_Bjet_v4('2b55_loose_j110_j55_a4tchad_1bL2',counter=7234),

        #(ii)1b/4j and 2b/4j 
        ChainTemplate_Bjet_v4('b55_medium_4j55_a4tchad_L2FS',counter=253, l2thresh=15),
        ChainTemplate_Bjet_v4('2b55_medium_4j55_a4tchad_L2FS',counter=2249, l2thresh=15),
        ChainTemplate_Bjet_v4('2b55_loose_4j55_a4tchad_L2FS',counter=247, l2thresh=15),
        #        ChainTemplate_Bjet_v4('b55_medium_4j55_a4tchad',counter=7198),
        #        ChainTemplate_Bjet_v4('2b55_medium_4j55_a4tchad',counter=7199),
        ChainTemplate_Bjet_v4('b55_loose_4j55_a4tchad',counter=4254),

        #(iii) non-standard 1b/4j and 2b/4j triggers
        ChainTemplate_Bjet_v4('b55_medium_3j55_a4tchad_4L1J15',counter=716),
        ChainTemplate_Bjet_v4('b45_medium_3j45_a4tchad_4L1J15',counter=6964),
        ChainTemplate_Bjet_v4('b35_medium_3j35_a4tchad_4L1J15',counter=5281),
        ChainTemplate_Bjet_v4('2b55_medium_3j55_a4tchad_4L1J15',counter=6914),
        ChainTemplate_Bjet_v4('2b45_medium_3j45_a4tchad_4L1J15',counter=6942),
        ChainTemplate_Bjet_v4('2b35_medium_3j35_a4tchad_4L1J15',counter=240),

        #(iv)NoCut trigger
        ChainTemplate_Bjet_v4('b55_NoCut_j55_a4tchad', counter=490),
        ChainTemplate_Bjet_v4('b55_NoCut_j55_a4tchad_IDT', counter=8015),
                
        ChainTemplate_Bjet_v4('b55_NoCut_j55_a4tchad_FTK', counter=8000),
        ChainTemplate_Bjet_v4('b55_NoCut_j55_a4tchad_FTK_Refit', counter=8001),

        ChainTemplate_Bjet_v4('b35_NoCut_4j35_a4tchad', counter=494),
        ChainTemplate_Bjet_v4('b35_NoCut_4j35_a4tchad_L2FS', counter=586, l2thresh=15),


        #---------------------------------------------
        # b-tagged fat jets (#91900)
        #---------------------------------------------
        ChainTemplate_Bjet_v4('b110_loose_j110_a10tcem_L2FS_L1J75',counter=7186,l2thresh=100),
        ChainTemplate_Bjet_v4('b145_loose_j145_a10tcem_L2FS',counter=7187,l2thresh=100),
        ChainTemplate_Bjet_v4('b180_loose_j180_a10tcem_L2FS',counter=7188),
#        ChainTemplate_Bjet_v4('b180_loose_j180_a10tcem_L2FS',counter=7188,l2thresh=100),
        ChainTemplate_Bjet_v4('b180_loose_j180_a10tcem_L2j140',counter=7189,l2thresh=140),
        ChainTemplate_Bjet_v4('b180_medium_j180_a10tcem_L2j140',counter=7220,l2thresh=140),
        ChainTemplate_Bjet_v4('b220_loose_j220_a10tcem',counter=7190,l2thresh=140),
        ChainTemplate_Bjet_v4('b280_loose_j280_a10tcem',counter=7191,l2thresh=140),
        ChainTemplate_Bjet_v4('b240_loose_j240_a10tcem_L2FS',counter=726,l2thresh=140),
        ChainTemplate_Bjet_v4('b240_loose_j240_a10tcem_L2j140',counter=723,l2thresh=140),


        #---------------------------------------------
        #HT triggers (#91860)
        #---------------------------------------------
        ChainTemplate_Bjet_v4('b145_medium_j145_a4tchad_ht400',counter=290),
        ChainTemplate_Bjet_v4('b165_medium_j165_a4tchad_ht500',counter=293),
        ChainTemplate_Bjet_v4('2b55_loose_j110_j55_a4tchad_ht500', counter=5295),

        ChainTemplate_Bjet_v4('2b55_medium_j110_j55_a4tchad_ht500', counter=6294),
        ChainTemplate_Bjet_v4('2b55_medium_j165_j55_a4tchad_ht500', counter=5296),
        
        ChainTemplate_Bjet_v4('2b80_medium_j165_j80_a4tchad_ht500', counter=5297),
        ChainTemplate_Bjet_v4('b45_medium_j145_j45_a4tchad_ht400', counter=289),
        ChainTemplate_Bjet_v4('b45_medium_j145_j45_a4tchad_ht500', counter=287),


        #---------------------------------------------
        #ZH triggers (#92105)
        #---------------------------------------------
        ChainTemplate_Bjet_v4('b80_loose_j80_a4tchad',counter=620),
        ChainTemplate_Bjet_v4('b110_loose_j110_a4tchad',counter=621),


        #---------------------------------------------
        #chain for bjet+xe
        #---------------------------------------------
        ChainTemplate_Bjet_v4('b35_mediumEF_j35_a4tchad',counter=7216),
        ChainTemplate_Bjet_v4('b55_mediumEF_j110_j55_a4tchad',counter=7208),
        ChainTemplate_Bjet_v4('b45_mediumEF_j110_j45_a4tchad',counter=7209),
        ChainTemplate_Bjet_v4('b145_mediumEF_j145_a4tchad',counter=7210),
        ChainTemplate_Bjet_v4('2b35_loose_j110_2j35_a4tchad',counter=298),
        ChainTemplate_Bjet_v4('2b55_loose_j145_j55_a4tchad',counter=174),
        ChainTemplate_Bjet_v4('2b45_loose_j145_j45_a4tchad',counter=175),
        ChainTemplate_Bjet_v4('b35_loose_j110_2j35_a4tchad',counter=1615),
        ChainTemplate_Bjet_v4('b45_looseEF_j145_a4tchad',counter=1616),

        #---------------------------------------------
        # bjet chains for mu+jet combined matched chains  (#91661)
        #---------------------------------------------
        ChainTemplate_Bjet_v4('b15_NoCut_j15_a4tchad', counter=489, effexthresh=15),        
        ChainTemplate_Bjet_v4('b25_NoCut_j25_a4tchad',counter=7771,effexthresh=15),
        ChainTemplate_Bjet_v4('b35_NoCut_j35_a4tchad',counter=7772),
        ChainTemplate_Bjet_v4('b45_NoCut_j45_a4tchad_L1J15',counter=7784),
        ChainTemplate_Bjet_v4('b45_NoCut_j45_a4tchad',counter=5184),
        ChainTemplate_Bjet_v4('b65_NoCut_j65_a4tchad',counter=7773),
        ChainTemplate_Bjet_v4('b80_NoCut_j80_a4tchad',counter=390),
        ChainTemplate_Bjet_v4('b110_NoCut_j110_a4tchad',counter=397),
        ChainTemplate_Bjet_v4('b145_NoCut_j145_a4tchad',counter=411),
        ChainTemplate_Bjet_v4('b180_NoCut_j180_a4tchad',counter=427),
        ChainTemplate_Bjet_v4('b220_NoCut_j220_a4tchad',counter=430),
        ChainTemplate_Bjet_v4('b280_NoCut_j280_a4tchad',counter=433),
        ChainTemplate_Bjet_v4('b360_NoCut_j360_a4tchad',counter=187),

        ChainTemplate_Bjet_v4('b45_NoCut_4j45_a4tchad',counter=243,l2thresh=10),
        ChainTemplate_Bjet_v4('b45_NoCut_4j45_a4tchad_L2FS',counter=249,l2thresh=10),

        ChainTemplate_Bjet_v4('b45_NoCut_j45_a4tchad_L2FS_L1J15',counter=2784,l2thresh=10),
        ChainTemplate_Bjet_v4('b45_NoCut_j45_a4tchad_L2FS',counter=5185,l2thresh=10),
        ChainTemplate_Bjet_v4('b55_NoCut_j55_a4tchad_L2FS',counter=493,l2thresh=15),
        ChainTemplate_Bjet_v4('b65_NoCut_j65_a4tchad_L2FS',counter=2773,l2thresh=20),
        ChainTemplate_Bjet_v4('b80_NoCut_j80_a4tchad_L2FS',counter=2774,l2thresh=30),
        ChainTemplate_Bjet_v4('b110_NoCut_j110_a4tchad_L2FS',counter=2775,l2thresh=50),
        ChainTemplate_Bjet_v4('b145_NoCut_j145_a4tchad_L2FS',counter=2776,l2thresh=75),
        ChainTemplate_Bjet_v4('b180_NoCut_j180_a4tchad_L2FS',counter=2777,l2thresh=75),
        ChainTemplate_Bjet_v4('b220_NoCut_j220_a4tchad_L2FS',counter=2778,l2thresh=75),
        ChainTemplate_Bjet_v4('b280_NoCut_j280_a4tchad_L2FS',counter=2779,l2thresh=75),
        ChainTemplate_Bjet_v4('b360_NoCut_j360_a4tchad_L2FS',counter=2780,l2thresh=75),

        ChainTemplate_Bjet_v4('b55_medium_2j55_a4tchad_L1J20',counter=7973),
        ChainTemplate_Bjet_v4('2b55_loose_2j55_a4tchad_L1J20',counter=7974),
        ChainTemplate_Bjet_v4('b65_medium_2j65_a4tchad',counter=7975),


        #---------------------------------------------
        # L1_MU6_2J20 seeded triggers (#93820)
        #---------------------------------------------
        ChainTemplate_Bjet_v4('2b55_medium_2j55_a4tchad_L1J20',counter=7978),
        

        
        #---------------------------------------------
        #Chain for Z->bb trigger (#92592)
        #---------------------------------------------
        ChainTemplate_Bjet_v4('2b35_loose_j145_j100_j35_a4tchad', counter=314),

        #---------------------------------------------
        # alternative 4J15-seeded bjet triggers (#92019)
        #---------------------------------------------
        ChainTemplate_Bjet_v4('b45_medium_4j45_a4tchad', counter=277),
        ChainTemplate_Bjet_v4('b45_medium_4j45_a4tchad_L2FS', counter=775,l2thresh=15),
        ChainTemplate_Bjet_v4('2b35_loose_4j35_a4tchad', counter=273),
        ChainTemplate_Bjet_v4('2b35_loose_3j35_a4tchad_4L1J15', counter=274),
        ChainTemplate_Bjet_v4('2b35_loose_j145_2j35_a4tchad', counter=275),
        ChainTemplate_Bjet_v4('2b35_loose_j145_j35_a4tchad', counter=276),

        ChainTemplate_Bjet_v4('2b35_loose_3j35_a4tchad_L2FS_4L1J15', counter=251,l2thresh=15),
        ChainTemplate_Bjet_v4('2b35_loose_4j35_a4tchad_L2FS', counter=250,l2thresh=15),
        ChainTemplate_Bjet_v4('b55_loose_4j55_a4tchad_L2FS', counter=5256,l2thresh=15),


        #---------------------------------------------
        # Additional Multijet Triggers (#92451)
        #---------------------------------------------
        ChainTemplate_Bjet_v4('b55_medium_4j55_a4tchad_4L1J10',counter=4111),
        ChainTemplate_Bjet_v4('b55_medium_4j55_a4tchad_L2FS_4L1J10',counter=4112,l2thresh=10),
        ChainTemplate_Bjet_v4('b45_medium_4j45_a4tchad_4L1J10',counter=4113),
        ChainTemplate_Bjet_v4('b45_medium_4j45_a4tchad_L2FS_4L1J10',counter=4114,l2thresh=10),
        ChainTemplate_Bjet_v4('2b35_loose_3j35_a4tchad_4L1J10',counter=4115,l2thresh=10),
        ChainTemplate_Bjet_v4('2b35_loose_3j35_a4tchad_L2FS_4L1J10',counter=4116,l2thresh=10),
        ChainTemplate_Bjet_v4('b35_loose_4j35_a4tchad_L2FS_5L1J10',counter=4117,l2thresh=10),
        ChainTemplate_Bjet_v4('b35_NoCut_4j35_a4tchad_L2FS_5L1J10',counter=4588,l2thresh=10),
        ChainTemplate_Bjet_v4('b35_loose_4j35_a4tchad_5L1J10',counter=4119),
        ChainTemplate_Bjet_v4('b35_NoCut_4j35_a4tchad_5L1J10',counter=4587),


        #---------------------------------------------
        # Adding tight Multijet Triggers (#94044)
        #---------------------------------------------
        ChainTemplate_Bjet_v4('b55_tight_4j55_a4tchad',counter=701),
        ChainTemplate_Bjet_v4('b55_tight_4j55_a4tchad_L2FS',counter=702, l2thresh=15),

        ChainTemplate_Bjet_v4('b45_tight_4j45_a4tchad', counter=752),
        ChainTemplate_Bjet_v4('b45_tight_4j45_a4tchad_L2FS', counter=753,l2thresh=15),
        ChainTemplate_Bjet_v4('b55_tight_3j55_a4tchad_4L1J15',counter=754),

        ChainTemplate_Bjet_v4('b55_tight_3j55_a4tchad_L2FS_4L1J15',counter=727,l2thresh=15),
        ChainTemplate_Bjet_v4('b45_tight_j145_j45_a4tchad_ht400', counter=728),


        #---------------------------------------------
        # bjet+forward jet Triggers (#97969)
        #---------------------------------------------
        ChainTemplate_Bjet_v4('b35_medium_j35_a4tchad_3L1J15_FJ15',counter=7728),
        ChainTemplate_Bjet_v4('b35_medium_j35_a4tchad_L2FS_3L1J15_FJ15',counter=7729),
        ChainTemplate_Bjet_v4('b35_NoCut_j35_a4tchad_3L1J15_FJ15',counter=7730),
        ChainTemplate_Bjet_v4('b35_NoCut_j35_a4tchad_L2FS_3L1J15_FJ15',counter=7731),

        ChainTemplate_Bjet_v4('b35_medium_j35_a4tchad_2L1FJ15',counter=7732),
        ChainTemplate_Bjet_v4('b35_medium_j35_a4tchad_L2FS_2L1FJ15',counter=7733),
        ChainTemplate_Bjet_v4('b35_NoCut_j35_a4tchad_2L1FJ15',counter=7734),
        ChainTemplate_Bjet_v4('b35_NoCut_j35_a4tchad_L2FS_2L1FJ15',counter=7735),
        
        ]
    
if ( TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus() ):
    Bjets += [
        ###HI extras
        MergedChain('mu4_MSonly_j15_a2hi_EFFS_matched', None,
                    ef=Chain(name='EF_mu4_MSonly_j15_a2hi_EFFS_matched', counter=838,seed='L2_mu4_MSonly'),
                    tomerge=['EF_mu4_MSonly', 'EF_j15_a2hi_EFFS_L1ZDC'], OnlyEF=True).
        addSequence(['EF_SA_mu4_MSonly', 'EF_j15_a2hi_EFFS_L1ZDC'],[fexes.ef_LeptonJetFexAllTE], 'EF_mu4_MSonly_j15_a2hi_EFFS_matched').addSignature(),
      
	MergedChain('mu4_MSonly_j15_a4tchad_EFFS_matched', None,
                    #l2=Chain(name='L2_mu4_matched', counter=1064, seed='L1_MU0'),
                    ef=Chain(name='EF_mu4_MSonly_j15_a4tchad_EFFS_matched', counter=391,seed='L2_mu4_MSonly'),
                    tomerge=['EF_mu4_MSonly', 'EF_j15_a4tchad_EFFS_L2mbMbts'], OnlyEF=True).
        addSequence(['EF_SA_mu4_MSonly', 'EF_j15_a4tchad_EFFS_L2mbMbts'],[fexes.ef_LeptonJetFexAllTE], 'EF_mu4_MSonly_j15_a4tchad_EFFS_matched').addSignature(),

        MergedChain('mu4_j15_a4tchad_EFFS_matched', None,
                    #l2=Chain(name='L2_mu4_matched', counter=1064, seed='L1_MU0'),
                    ef=Chain(name='EF_mu4_j15_a4tchad_EFFS_matched', counter=392,seed='L2_mu4'),
                    tomerge=['EF_mu4', 'EF_j15_a4tchad_EFFS_L2mbMbts'], OnlyEF=True).
        addSequence(['EF_mu4', 'EF_j15_a4tchad_EFFS_L2mbMbts'],[fexes.ef_LeptonJetFexAllTE], 'EF_mu4_j15_a4tchad_EFFS_matched').addSignature(),

        MergedChain('mu4_j10_a4tchad_EFFS_matched', None,
                    #l2=Chain(name='L2_mu4_matched', counter=1064, seed='L1_MU0'),
                    ef=Chain(name='EF_mu4_j10_a4tchad_EFFS_matched', counter=393,seed='L2_mu4'),
                    tomerge=['EF_mu4', 'EF_j10_a4tchad_EFFS_L2mbMbts'], OnlyEF=True).
        addSequence(['EF_mu4', 'EF_j10_a4tchad_EFFS_L2mbMbts'],[fexes.ef_LeptonJetFexAllTE], 'EF_mu4_j10_a4tchad_EFFS_matched').addSignature(),  
        
        MergedChain('mu4_j20_a4tchad_EFFS_matched', None,
                    #l2=Chain(name='L2_mu4_matched', counter=1064, seed='L1_MU0'),
                    ef=Chain(name='EF_mu4_j20_a4tchad_EFFS_matched', counter=596,seed='L2_mu4'),
                    tomerge=['EF_mu4', 'EF_j20_a4tchad_EFFS_L2mbMbts'], OnlyEF=True).
        addSequence(['EF_mu4', 'EF_j20_a4tchad_EFFS_L2mbMbts'],[fexes.ef_LeptonJetFexAllTE], 'EF_mu4_j20_a4tchad_EFFS_matched').addSignature(),

       MergedChain('mu4_j30_a4tchad_EFFS_matched', None,
                    #l2=Chain(name='L2_mu4_matched', counter=1064, seed='L1_MU0'),
                    ef=Chain(name='EF_mu4_j30_a4tchad_EFFS_matched', counter=688,seed='L2_mu4'),
                    tomerge=['EF_mu4', 'EF_j30_a4tchad_EFFS_L2mbMbts'], OnlyEF=True).
        addSequence(['EF_mu4', 'EF_j30_a4tchad_EFFS_L2mbMbts'],[fexes.ef_LeptonJetFexAllTE], 'EF_mu4_j30_a4tchad_EFFS_matched').addSignature(),

       MergedChain('mu4_j40_a4tchad_EFFS_matched', None,
                    #l2=Chain(name='L2_mu4_matched', counter=1064, seed='L1_MU0'),
                    ef=Chain(name='EF_mu4_j40_a4tchad_EFFS_matched', counter=689,seed='L2_mu4'),
                    tomerge=['EF_mu4', 'EF_j40_a4tchad_EFFS_L2mbMbts'], OnlyEF=True).
        addSequence(['EF_mu4', 'EF_j40_a4tchad_EFFS_L2mbMbts'],[fexes.ef_LeptonJetFexAllTE], 'EF_mu4_j40_a4tchad_EFFS_matched').addSignature(),

       MergedChain('mu4_j10_a4hi_EFFS_matched', None,
                    #l2=Chain(name='L2_mu4_matched', counter=1064, seed='L1_MU0'),
                    ef=Chain(name='EF_mu4_j10_a4hi_EFFS_matched', counter=1003,seed='L2_mu4'),
                    tomerge=['EF_mu4', 'EF_j10_a4hi_EFFS_L2mbMbts'], OnlyEF=True).
        addSequence(['EF_mu4', 'EF_j10_a4hi_EFFS_L2mbMbts'],[fexes.ef_LeptonJetFexAllTE], 'EF_mu4_j10_a4hi_EFFS_matched').addSignature(),

       MergedChain('mu4_j15_a4hi_EFFS_matched', None,
                    #l2=Chain(name='L2_mu4_matched', counter=1064, seed='L1_MU0'),
                    ef=Chain(name='EF_mu4_j15_a4hi_EFFS_matched', counter=1004,seed='L2_mu4'),
                    tomerge=['EF_mu4', 'EF_j15_a4hi_EFFS_L2mbMbts'], OnlyEF=True).
        addSequence(['EF_mu4', 'EF_j15_a4hi_EFFS_L2mbMbts'],[fexes.ef_LeptonJetFexAllTE], 'EF_mu4_j15_a4hi_EFFS_matched').addSignature(),

       MergedChain('mu4_j20_a4hi_EFFS_matched', None, 
                    #l2=Chain(name='L2_mu4_matched', counter=1064, seed='L1_MU0'),
                    ef=Chain(name='EF_mu4_j20_a4hi_EFFS_matched', counter=1002,seed='L2_mu4'),
                    tomerge=['EF_mu4', 'EF_j20_a4hi_EFFS_L2mbMbts'], OnlyEF=True).
        addSequence(['EF_mu4', 'EF_j20_a4hi_EFFS_L2mbMbts'],[fexes.ef_LeptonJetFexAllTE], 'EF_mu4_j20_a4hi_EFFS_matched').addSignature(),
      
      MergedChain('mu4_j30_a4hi_EFFS_matched', None,
                    #l2=Chain(name='L2_mu4_matched', counter=1064, seed='L1_MU0'),
                    ef=Chain(name='EF_mu4_j30_a4hi_EFFS_matched', counter=692,seed='L2_mu4'),
                    tomerge=['EF_mu4', 'EF_j30_a4hi_EFFS_L2mbMbts'], OnlyEF=True).
        addSequence(['EF_mu4', 'EF_j30_a4hi_EFFS_L2mbMbts'],[fexes.ef_LeptonJetFexAllTE], 'EF_mu4_j30_a4hi_EFFS_matched').addSignature(),

      MergedChain('mu4_j40_a4hi_EFFS_matched', None,
                    #l2=Chain(name='L2_mu4_matched', counter=1064, seed='L1_MU0'),
                    ef=Chain(name='EF_mu4_j40_a4hi_EFFS_matched', counter=693,seed='L2_mu4'),
                    tomerge=['EF_mu4', 'EF_j40_a4hi_EFFS_L2mbMbts'], OnlyEF=True).
        addSequence(['EF_mu4', 'EF_j40_a4hi_EFFS_L2mbMbts'],[fexes.ef_LeptonJetFexAllTE], 'EF_mu4_j40_a4hi_EFFS_matched').addSignature(),
      ]

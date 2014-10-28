# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for Muon chains
###########################################################################
__author__  = 'M.Woudstra, Y.Nakahama, K.Nagano'
__doc__="Definition of muon chains as used v4- menu" 

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)

from TriggerMenu.muon.MuonDef import L2EFChain_mu as L2EFChain_mu
try:
    from TriggerMenu.muon.MuonDefIdTest import L2EFChain_mu as L2EFChain_mu_IdTest
except:
    log.error('generateMuonChainDefs: Problems when importing MuonDefIdTest.')
    log.info(traceback.print_exc())


from TriggerMenu.menu.MenuUtils import *

Muons = []

###########################################################################
###########################################################################

##Create a list with the threshold of all the muons in the chain
##it's needed to correctly configure the hypo for the FS muon chains
def GetAllMuonThresholds(chainDict):
    
    muons = []
    for chainpart in chainDict['chainParts']:
      if 'mu' in chainpart['trigType']:        
          for x in range(0,int(chainpart['multiplicity'])):

              if chainpart['threshold']!='0':
                  thr= '%sGeV' %(chainpart['threshold'] )
              else:
                  thr= 'passthrough'

              muons.append(thr)

    return muons

def generateChainDefs(chainDict):
    chainParts = chainDict['chainParts']
    
    listOfChainDicts = splitChainDict(chainDict)

    listOfChainDefs = []

    asymDiMuonChain = False
    if (len(listOfChainDicts) > 1): asymDiMuonChain = True
    else: asymDiMuonChain = False

    for subChainDict in listOfChainDicts:
        if "IdTest" in subChainDict["chainParts"]["addInfo"]:
            Muon = L2EFChain_mu_IdTest(subChainDict, asymDiMuonChain)
        else:
            #If FS muon, needs total counts of muons in chain
            if 'noL1' in subChainDict["chainParts"]["extra"]:
                AllMuons=GetAllMuonThresholds(chainDict)
            else:
                AllMuons=[]

            Muon = L2EFChain_mu(subChainDict, asymDiMuonChain, AllMuons)

        listOfChainDefs += [Muon.generateHLTChainDef()]

    if len(listOfChainDefs)>1:
        if ('mergingStrategy' in chainDict.keys()):        
            theChainDef = mergeChainDefs(listOfChainDefs,chainDict["mergingStrategy"],chainDict["mergingOffset"])
        else:
            theChainDef = mergeChainDefs(listOfChainDefs)
    else:
        theChainDef = listOfChainDefs[0]

    #if chainDict["topo"]:
    #    theChainDef = _addTopoInfo(theChainDef,chainDict)

    return theChainDef


def _addTopoInfo(theChainDef,chainDict,doAtL2AndEF=True):

    maxL2SignatureIndex = -1
    for signatureIndex,signature in enumerate(theChainDef.signatureList):
        if signature['listOfTriggerElements'][0][0:2] == "L2":
            maxL2SignatureIndex = max(maxL2SignatureIndex,signatureIndex)
    
    inputTEsL2 = theChainDef.signatureList[maxL2SignatureIndex]['listOfTriggerElements']    
    inputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']

    L2ChainName = "L2_" + chainDict['chainName']
    EFChainName = "EF_" + chainDict['chainName']
    HLTChainName = "HLT_" + chainDict['chainName']
    
    if "Jpsimumu" in chainDict["topo"]:

        # to be adapted!!!!!!!!!!!!!!!!!

        from TrigEgammaHypo.TrigL2DielectronMassHypoConfig import TrigL2DielectronMassFex_Jpsiee, TrigL2DielectronMassHypo_Jpsiee
        from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFex_Jpsi, TrigEFDielectronMassHypo_Jpsi

        L2Fex = TrigL2DielectronMassFex_Jpsiee()
        L2Hypo = TrigL2DielectronMassHypo_Jpsiee()        

        EFFex = TrigEFDielectronMassFex_Jpsi()
        EFHypo = TrigEFDielectronMassHypo_Jpsi()


        theChainDef.addSequence([L2Fex, L2Hypo],inputTEsL2,L2ChainName)
        theChainDef.addSignatureL2([L2ChainName])

        theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])
    
    return theChainDef


Muons += [
    #L2EFChain_mu_autoconfig('mu4T',  800, rerun=1),
    # L2EFChain_mu_autoconfig('mu4Ti', 435, l2_chain_name='L2_mu4Ti'),
    # L2EFChain_mu_autoconfig('mu4T_L2StarB', 244),
    # L2EFChain_mu_autoconfig('mu4T_L2StarC', 802),
    # L2EFChain_mu_autoconfig('mu4T_l2muonSA', 1003),
    # L2EFChain_mu_autoconfig('mu4T_muFast',   7113),

    # L2EFChain_mu_autoconfig('mu6T',  8022, l2_chain_name='L2_mu6T'),

    # L2EFChain_mu_autoconfig('mu6',  810, rerun=1),
    # L2EFChain_mu_autoconfig('mu6_L2StarB', 336, l2_chain_name='L2_mu6_L2StarB'),
    
    # L2EFChain_mu_autoconfig('mu6_FTK', 7003),
    # L2EFChain_mu_autoconfig('mu6i', 1811, 1810),
    # L2EFChain_mu_autoconfig('mu8',  815, rerun=1),
    # L2EFChain_mu_autoconfig('mu10', 820, rerun=1),
    # L2EFChain_mu_autoconfig('mu10i',439, 820),
    # L2EFChain_mu_autoconfig('mu10i_loose', 994, 825, l2_inputTEs=["MU6"]),
    # L2EFChain_mu_autoconfig('mu13', 830, rerun=1),
    # L2EFChain_mu_autoconfig('mu15', 840),
    # L2EFChain_mu_autoconfig('mu15i', 839,840),
    # L2EFChain_mu_autoconfig('mu18', 850, rerun=1),
    # L2EFChain_mu_autoconfig('mu24', 851),

    # L2EFChain_mu_autoconfig('mu18_medium', 941),
    # L2EFChain_mu_autoconfig('mu24_medium', 942),

    # L2EFChain_mu_autoconfig('mu18_tight',          943, rerun=1),
    # L2EFChain_mu_autoconfig('mu24_tight',          944, rerun=1),
    # L2EFChain_mu_autoconfig('mu24_tight_MuonEF',   945, 944),
    # L2EFChain_mu_autoconfig('mu24_tight_MG',       946, 944),
    # L2EFChain_mu_autoconfig('mu24_tight_L2StarB',  912),
    # L2EFChain_mu_autoconfig('mu24_tight_L2StarC',  948),
    # L2EFChain_mu_autoconfig('mu24_tight_l2muonSA', 949),
    # L2EFChain_mu_autoconfig('mu24_tight_muFast',  7949),
    # L2EFChain_mu_autoconfig('mu18i4_tight',        703),
    # L2EFChain_mu_autoconfig('mu18it_tight',        973),
    # L2EFChain_mu_autoconfig('mu20it_tight',        970),
    # L2EFChain_mu_autoconfig('mu20i_tight',         440, 994),
    # L2EFChain_mu_autoconfig('mu24i_tight',         785, 944),
    # L2EFChain_mu_autoconfig('mu24i_tight_MuonEF',  786, 944),
    # L2EFChain_mu_autoconfig('mu24i_tight_MG',      787, 944),
    # L2EFChain_mu_autoconfig('mu24i_tight_l2muonSA',788, 949),
    # L2EFChain_mu_autoconfig('mu24i_tight_muFast', 7750,7949),
    # L2EFChain_mu_autoconfig('mu36_tight',          950),
    # L2EFChain_mu_autoconfig('mu40_tight',          951),

    # # cosmic chains
    # L2EFChain_mu_autoconfig('mu4T_cosmic',       743, l2_inputTEs=['MU4'],  l2_lower_chain_name='L1_MU4_EMPTY'),
    # L2EFChain_mu_autoconfig('mu4_L1MU11_cosmic', 744, l2_inputTEs=['MU11'], l2_lower_chain_name='L1_MU11_EMPTY'),

    # # slow chains
    # L2EFChain_mu_autoconfig('mu40_slow_tight',  860),
    # L2EFChain_mu_autoconfig('mu60_slow_tight1', 861),
    # L2EFChain_mu_autoconfig('mu60_slow1_tight1', 360),
    
    # L2EFChain_mu_autoconfig('mu40_slow_outOfTime_tight',  875),
    # L2EFChain_mu_autoconfig('mu60_slow_outOfTime_tight1', 876),

    # # for use in dimuon trigger
    # L2EFChain_mu_autoconfig('mu4T_wOvlpRm', 930),
    # L2EFChain_mu_autoconfig('mu6_wOvlpRm',  931),
    # L2EFChain_mu_autoconfig('mu8_wOvlpRm',  936),
    # L2EFChain_mu_autoconfig('mu10_wOvlpRm', 932),
    # L2EFChain_mu_autoconfig('mu13_wOvlpRm', 933),
    # L2EFChain_mu_autoconfig('mu13_wOvlpRm_l2muonSA', 934),
    # L2EFChain_mu_autoconfig('mu13_wOvlpRm_muFast',  7934),
    # L2EFChain_mu_autoconfig('mu15_wOvlpRm', 935),
    ]





#############

# Muons += [
#     L2EFChain_mu_L2EFMerged('mu18_medium_L2EFMerged',  7647),
# ]

# #test chain of the merged data preparation for ID
# Muons += [
#     L2EFChain_mu_MSonly('mu22_IDTrkNoCut_tight_IDMergedDataPrep',7648),
# ]

# #############

# Muons += [
#     L2EFChain_mu_MSonly('mu40_MSonly_barrel_tight', 870),
#     L2EFChain_mu_MSonly('mu50_MSonly_barrel_tight', 871),

#     L2EFChain_mu_MSonly('mu6_MSonly_wOvlpRm',  621),
#     L2EFChain_mu_MSonly('mu10_MSonly_wOvlpRm', 622),

#     L2EFChain_mu_MSonly('mu6_MSonly',  828, rerun=1),
#     L2EFChain_mu_MSonly('mu10_MSonly', 829, rerun=1),

#     L2EFChain_mu_MSonly('mu4_L1MU11_MSonly_cosmic', 724, l2_inputTEs=['MU11'], l2_lower_chain_name='L1_MU11_EMPTY'), 
    
#     L2EFChain_mu_MSonly('mu4T_IDTrkNoCut',       880),
#     L2EFChain_mu_MSonly('mu6_IDTrkNoCut',        881),
#     L2EFChain_mu_MSonly('mu13_IDTrkNoCut',       882),
#     L2EFChain_mu_MSonly('mu18_IDTrkNoCut_tight', 883),
#     L2EFChain_mu_MSonly('mu22_IDTrkNoCut_tight', 884),
    
#     L2EFChain_mu_MSonly('mu4T_IDTrkNoCut_R1',      2354),
#     #L2EFChain_mu_MSonly('mu6_IDTrkNoCut_R1',       2355),
#     #L2EFChain_mu_MSonly('mu13_IDTrkNoCut_R1',      2356),
#     #L2EFChain_mu_MSonly('mu18_IDTrkNoCut_tight_R1',2357),
#     L2EFChain_mu_MSonly('mu22_IDTrkNoCut_tight_R1',2358),
#     ]

# #############

# Muons += [
#     L2EFChain_mu_cal('mu4_l2cal_empty', 101, 'L1_MU4_EMPTY'),
#     L2EFChain_mu_cal('mu15_l2cal',      102, 'L1_MU15'),
#     ]

# #############

# Muons += [
#     L2EFChain_mu_muCombTag_NoEF('mu24_muCombTag_NoEF_tight', 104, 'L1_MU15'),

#     # alignment runs with toroid B=0
#     # n.b. seeded from L1_MU20 from menus point of view but actual roads in h/w
#     # will be MU40 in such runs
#     L2EFChain_mu_muCombTag_NoEF('mu10_muCombTag_NoEF_tight',5217, 'L1_MU15'),
#     L2EFChain_mu_muCombTag_NoEF('mu15_muCombTag_NoEF_tight',5627, 'L1_MU15'),
#     L2EFChain_mu_muCombTag_NoEF('mu20_muCombTag_NoEF_tight',5763, 'L1_MU15'),
                                
#     L2EFChain_mu_muCombTag_NoEF('mu10_muCombTag_NoEF_tight1',   103, 'L1_MU20'),
#     L2EFChain_mu_muCombTag_NoEF('mu15_muCombTag_NoEF_tight1',    74, 'L1_MU20'),
#     L2EFChain_mu_muCombTag_NoEF('mu20_muCombTag_NoEF_tight1',    75, 'L1_MU20'),
#     ]

# #############


# mu18_tight_TMEFonly = L2EFChain_mu_autoconfig('mu18_tight_TMEFonly',  953, 943, rerun=1)
# mu24_tight_TMEFonly = L2EFChain_mu_autoconfig('mu24_tight_TMEFonly',  954, 944, rerun=1)

# # Commented out for now, need adapting in MuonDef.py
# # Muons += [
# #     L2EFChain_mu_EFFSonly('mu18_mu8_EFFSOnly',  132, 2, TrigMuonEFCombinerMultiHypoConfig('Muon', '18GeV', '8GeV')),
# #     L2EFChain_mu_EFFSonly('mu24_mu6_EFFSOnly',  133, 2, TrigMuonEFCombinerMultiHypoConfig('Muon', '24GeV', '6GeV')),
# #     L2EFChain_mu_EFFSonly('mu18_2mu4_EFFSOnly', 134, 2, TrigMuonEFCombinerMultiHypoConfig('Muon', '18GeV', '4GeV', '4GeV')),
# #     L2EFChain_mu_EFFSonly('Jpsi_EFFSOnly',     7132, 2, TrigMuonEFCombinerDiMuonMassHypoConfig('Jpsi', 'OS')),

# #     mu18_tight_TMEFonly,
# #     mu24_tight_TMEFonly,
    
# #     L2EFChain_mu_Seeded_EFFS('mu18_tight_mu8_EFFS',  mu18_tight_TMEFonly,  'mu18_mu8_EFFSOnly',  545),
# #     L2EFChain_mu_Seeded_EFFS('mu24_tight_mu6_EFFS',  mu24_tight_TMEFonly,  'mu24_mu6_EFFSOnly',  546),
# #     L2EFChain_mu_Seeded_EFFS('mu18_tight_2mu4_EFFS', mu18_tight_TMEFonly,  'mu18_2mu4_EFFSOnly', 550),
# #     L2EFChain_mu_Seeded_EFFS('mu18_tight_Jpsi_EFFS', mu18_tight_TMEFonly,  'Jpsi_EFFSOnly',     872),
# #     ]

# #############

# Muons += [
#     L2EFChain_mu_NoAlg('L1MU10_NoAlg', 286, 'L1_MU10'),
#     L2EFChain_mu_NoAlg('L1MU15_NoAlg', 684, 'L1_MU15'),
#     L2EFChain_mu_NoAlg('L12MU4_NoAlg', 685, 'L1_2MU4'),
#     L2EFChain_mu_NoAlg('mu4_empty_NoAlg',  128, 'L1_MU4_EMPTY'),
#     L2EFChain_mu_NoAlg('mu11_empty_NoAlg', 129, 'L1_MU11_EMPTY'),
#     L2EFChain_mu_NoAlg('mu4_firstempty_NoAlg',   130, 'L1_MU4_FIRSTEMPTY'),
#     L2EFChain_mu_NoAlg('mu4_unpaired_iso_NoAlg', 131, 'L1_MU4_UNPAIRED_ISO'),
#    ]                                                                             

# ###########################################################################
# # NSW emulation chains
# ###########################################################################

# Muons += [
#     L2EFChain_mu_NSW_emulation('mu15_NSWemul_NoL2', 7104),
#     L2EFChain_mu_NSW_emulation('mu15_NSWemul_NoEF', 7108),
#     L2EFChain_mu_NSW_emulation('mu20_NSWemul_NoL2', 7114),
#     L2EFChain_mu_NSW_emulation('mu20_NSWemul_NoEF', 7118),
#     ]

# ###########################################################################
# # FTK chains
# ###########################################################################

# Muons += [

#     L2EFChain_mu_autoconfig('mu24_tight_FTK', 1814),
#     L2EFChain_mu_autoconfig('mu20it_tight_FTK',1815),
# ]
    
# ###########################################################################
# ###########################################################################

# # Remove all configs that failed
# MuonCandidates = Muons[:]
# Muons = []
# for m in MuonCandidates:
#     if not hasattr(m,"initOK") or m.initOK():
#         Muons.append(m)
#     else:
#         logging.getLogger(__name__).error("Removing chain %r from Muons because initialization failed", m.sig_id) 
        

# ###########################################################################
# ###########################################################################

# #if __name__ == '__main__':
# #    triggerPythonConfig = TriggerPythonConfig('hlt.xml', None)
# #    for m in Muons:
# #        m.generateMenu(triggerPythonConfig)
# #    triggerPythonConfig.writeConfigFiles()

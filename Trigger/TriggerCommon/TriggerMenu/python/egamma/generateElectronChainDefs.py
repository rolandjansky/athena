# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################################################
##########################################################################################
__doc__=""
__version__="Implementation of Electron Slice single electron signatures"

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenu.egamma.generateElectronChainDefs' )
log.info("Importing %s",__name__)

import traceback

from TriggerMenu.egamma.ElectronDef import L2EFChain_e as L2EFChain_e
try:
    from TriggerMenu.egamma.ElectronDefIdTest import L2EFChain_e as L2EFChain_e_IdTest
except:
    log.error('generateElectronChainDefs: Problems when importing ElectronDefIdTest.')
    log.info(traceback.print_exc())

#from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenu.menu.MenuUtils import *
Electrons = []

##########################################################################################
##########################################################################################

def generateChainDefs(chainDict):
    chainParts = chainDict['chainParts']
    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:
        if "IdTest" in subChainDict["chainParts"]["addInfo"]:
            Electron = L2EFChain_e_IdTest(subChainDict)
        else:
            Electron = L2EFChain_e(subChainDict)
            
        listOfChainDefs += [Electron.generateHLTChainDef()]
    
    if len(listOfChainDefs)>1:
        theChainDef = mergeChainDefs(listOfChainDefs)
    else:
        theChainDef = listOfChainDefs[0]

    if chainDict["topo"]:
        theChainDef = _addTopoInfo(theChainDef,chainDict)    

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


    topoThresh = chainDict['topoThreshold']
    topoStartFrom = setupTopoStartFrom(topoThresh,theChainDef) if topoThresh else None

    if "Jpsiee" in chainDict["topo"]:
        topo2StartFrom = None
        if topoStartFrom:
            #L2ChainName = L2ChainName+'_tsf'
            #topo2StartFrom = L2ChainName
            EFChainName = EFChainName+'_tsf'

        #from TrigEgammaHypo.TrigL2DielectronMassHypoConfig import TrigL2DielectronMassFex_Jpsiee, TrigL2DielectronMassHypo_Jpsiee
        from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFex_Jpsi, TrigEFDielectronMassHypo_Jpsi

        #L2Fex = TrigL2DielectronMassFex_Jpsiee()
        #L2Hypo = TrigL2DielectronMassHypo_Jpsiee()        

        EFFex = TrigEFDielectronMassFex_Jpsi()
        EFHypo = TrigEFDielectronMassHypo_Jpsi()

        #theChainDef.addSequence([L2Fex, L2Hypo],inputTEsL2,L2ChainName, topo_start_from = topoStartFrom)
        #theChainDef.addSignatureL2([L2ChainName])

        #theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName, topo_start_from = topo2StartFrom)
        theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName, topo_start_from = topoStartFrom)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])
    
    elif "Zeg" in chainDict["topo"]:

        #from TrigEgammaHypo.TrigL2DielectronMassHypoConfig import TrigL2DielectronMassFex_Zeg, TrigL2DielectronMassHypo_Zeg
        from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFex_Zeg, TrigEFDielectronMassHypo_Zeg

        #L2Fex = TrigL2DielectronMassFex_Zeg()
        #L2Hypo = TrigL2DielectronMassHypo_Zeg()        

        EFFex = TrigEFDielectronMassFex_Zeg()
        EFHypo = TrigEFDielectronMassHypo_Zeg()

        #theChainDef.addSequence([L2Fex, L2Hypo],inputTEsL2,L2ChainName)
        #theChainDef.addSignatureL2([L2ChainName])

        theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])

    elif "Zee" in chainDict["topo"]:

        #from TrigEgammaHypo.TrigL2DielectronMassHypoConfig import TrigL2DielectronMassFex_Zee, TrigL2DielectronMassHypo_ZeeTight, TrigL2DielectronMassHypo_Zee
        from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFex_Zee, TrigEFDielectronMassHypo_ZeeTight, TrigEFDielectronMassHypo_Zee

        #L2Fex = TrigL2DielectronMassFex_Zee()
        #L2Hypo = TrigL2DielectronMassHypo_ZeeTight()

        if 'etcut' in chainDict['chainName']:
            from TrigEgammaHypo.TrigEFDielectronMassHypoConfig import TrigEFDielectronMassFexElectronCluster_Zee, TrigEFDielectronMassHypoElectronCluster_Zee
            EFFex = TrigEFDielectronMassFexElectronCluster_Zee()
            EFHypo = TrigEFDielectronMassHypoElectronCluster_Zee()
        else:
            EFFex = TrigEFDielectronMassFex_Zee()
            EFHypo = TrigEFDielectronMassHypo_ZeeTight()
            

        #theChainDef.addSequence([L2Fex, L2Hypo],inputTEsL2,L2ChainName)
        #theChainDef.addSignatureL2([L2ChainName])

        theChainDef.addSequence([EFFex, EFHypo],inputTEsEF,EFChainName)
        theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1, [EFChainName])

        if 'etcut' in chainDict['chainName']:
             from TrigIDTPMonitor.TrigIDTPMonitorConfig import IDTPMonitorElectron                                                               
             IDTP = IDTPMonitorElectron()
             myInputTEsEF = theChainDef.signatureList[-1]['listOfTriggerElements']
             theChainDef.addSequence([IDTP],myInputTEsEF,EFChainName+"_monit")
             theChainDef.addSignature(theChainDef.signatureList[-1]['signature_counter']+1,[EFChainName+"_monit"])
    
    return theChainDef




# chains compatible with pp_v4, ALFA_v2, HI_v2

# Electrons += [        
#     L2EFChain_e('e5_NoCut_firstempty', 961), # not in v4 menus
#     L2EFChain_e('e10_NoCut', 998), # not in v4 menus
#     L2EFChain_e('e10_loose', 4513),
#     L2EFChain_e('e10_loose1', 854),
#     L2EFChain_e('e10_medium1', 149),
#     L2EFChain_e('e10_loose1_EM3', 5),
#     L2EFChain_e('e15_loose', 4543), 
#     L2EFChain_e('e15_loose0', 520),
#     L2EFChain_e('e15_loose1', 523), 
#     L2EFChain_e('e15_medium1', 532),
#     L2EFChain_e('e15_loose_EM10VH', 777), 
#     L2EFChain_e('e15_loose1_EM10VH', 778), 
#     L2EFChain_e('e14_tight', 2114),
#     L2EFChain_e('e14_tight1', 960),
#     L2EFChain_e('e15_tight', 114),
#     L2EFChain_e('e15_HLTtighter', 115),
#     L2EFChain_e('e18_tight', 24), 
#     L2EFChain_e('e20_tight', 25), 
#     L2EFChain_e('e22_tight', 46), 
#     L2EFChain_e('e15_tight1', 2600),
#     L2EFChain_e('e25_medium', 299), 
#     L2EFChain_e('e5_medium', 28), 
#     L2EFChain_e('e5_tight', 978), 
#     L2EFChain_e('e5_tight1', 979), 
#     L2EFChain_e('e7_tight', 5411), 
#     L2EFChain_e('e5_loose1', 146),
#     L2EFChain_e('e5_medium1', 116), 
#     L2EFChain_e('e7_loose1', 145),
#     L2EFChain_e('e7_medium1', 952), 
#     L2EFChain_e('e7_tight_L1EM7', 5412), 
#     L2EFChain_e('e7T_loose1', 535),
#     L2EFChain_e('e7T_medium1', 48), 
#     L2EFChain_e('e9_tight', 5410), 
#     L2EFChain_e('e7_tight1', 414),
#     L2EFChain_e('e9_tight1', 996), 
#     L2EFChain_e('e10_medium', 30), 
#     L2EFChain_e('e12T_loose', 7819), 
#     L2EFChain_e('e12T_loose1_EM10VH', 7821), 
#     L2EFChain_e('e11T_medium', 5421), 
#     L2EFChain_e('e6T_medium', 5422), 
#     L2EFChain_e('e12_medium', 818), 
#     L2EFChain_e('e18_loose1', 608), 
#     L2EFChain_e('e18_medium1', 50), 
#     L2EFChain_e('e18_medium', 4523),  
#     L2EFChain_e('e20_medium', 297), 
#     L2EFChain_e('e25_loose', 32), 
#     L2EFChain_e('e30_loose', 31), 
#     L2EFChain_e('e25i5vh_medium2', 113), 
#     L2EFChain_e('e22i6vh_medium2', 114), 
#     L2EFChain_e('e25i6vh_medium2', 3862), 
#     L2EFChain_e('e30_medium', 3831), 
#     L2EFChain_e('e30vh_medium2', 115), ##mistake
#     L2EFChain_e('e33_medium', 248),    ##mistake
#     L2EFChain_e('e35_medium', 249),    ##mistake
#     L2EFChain_e('e40_medium', 265), 
#     L2EFChain_e('e45_medium', 546), 
#     L2EFChain_e('e45_loose1', 2144), 
#     L2EFChain_e('e45_medium1', 547), 
#     L2EFChain_e('e60_loose0', 138), 
#     L2EFChain_e('e60_loose', 527), 
#     L2EFChain_e('e60_loose1', 215), 
#     L2EFChain_e('e60_medium1', 676), 
#     L2EFChain_e('e4_etcut', 2150),
#     L2EFChain_e('e5_etcut', 117),
#     L2EFChain_e('e5_loose0', 118), 
#     L2EFChain_e('e7_etcut', 2151),
#     L2EFChain_e('e9_etcut', 2152),
#     L2EFChain_e('e10_etcut', 27),
#     L2EFChain_e('e11_etcut', 140),
#     L2EFChain_e('e13_etcut', 2253),
#     L2EFChain_e('e13_etcut_EM7', 2254),
#     L2EFChain_e('e14_etcut_L1EM12', 2285),
#     L2EFChain_e('e14_etcut', 2259),
#     L2EFChain_e('e15vh_loose0', 139), 
#     L2EFChain_e('e20_etcut', 5200),
#     L2EFChain_e('e22_etcut_EM12', 5203),
#     L2EFChain_e('e22_etcut', 5201),
#     L2EFChain_e('e45_etcut', 5204),
#     L2EFChain_e('e60_etcut', 5205),
#     L2EFChain_e('e22_etcut_EM5', 5202),
#     L2EFChain_e('e13_etcutTRT', 2254),
#     L2EFChain_e('e20_medium1', 529), 
#     L2EFChain_e('e20_medium2', 51), 
#     L2EFChain_e('e13_etcutTrk', 2280),
#     L2EFChain_e('e13_etcutTrk_EM7', 2281),
#     L2EFChain_e('e20_etcutTrk', 2282),
#     L2EFChain_e('e20_etcutTrk_EM12', 2283),
#     L2EFChain_e('e22_etcutTrk', 2284),
#     L2EFChain_e('e22_loose', 170), 
#     L2EFChain_e('e22_loose1', 171), 
#     L2EFChain_e('e22_loose1_EM16VH', 174),
#     L2EFChain_e('e22_looseTrk', 172), 
#     L2EFChain_e('e12Tvh_medium', 235), 
#     L2EFChain_e('e12Tvh_medium1', 235), 
#     L2EFChain_e('e12Tvh_loose1', 234), 
#     L2EFChain_e('e15vh_medium', 3), 
#     L2EFChain_e('e15vh_medium1', 10), 
#     L2EFChain_e('e18vh_medium', 8), 
#     L2EFChain_e('e18vh_medium1', 9), 
#     L2EFChain_e('e22vh_medium', 528), 
#     L2EFChain_e('e22vh_loose0', 628), 
#     L2EFChain_e('e22vh_loose1', 401), 
#     L2EFChain_e('e22vh_looseTrk', 402), 
#     L2EFChain_e('e22vh_medium1', 49), 
#     L2EFChain_e('e22vh_medium2', 175), 
#     L2EFChain_e('e22vhi_medium1', 180), 
#     L2EFChain_e('e24vhi_medium1', 178), 
#     L2EFChain_e('e24vhi_loose1', 1018), 
#     L2EFChain_e('e24vh_medium1', 179),
#     L2EFChain_e('e24vh_tight1', 176),
#     L2EFChain_e('e24vh_loose', 181), 
#     L2EFChain_e('e24vh_loose0', 632), 
#     L2EFChain_e('e24vh_loose1', 182), 
#     L2EFChain_e('e25vh_medium1', 1349), 
#     L2EFChain_e('e20vhT_medium1', 1414), 
#     L2EFChain_e('e20vhT_tight1', 1415), 
#     L2EFChain_e('e24vhi_medium1_L2ET21', 3176), 
#     L2EFChain_e('e24vhi_medium1_L2ET22', 3177), 
#     L2EFChain_e('e24vhi_medium1_L2ET23', 3178), 
#     L2EFChain_e('e24vhi_medium1_L2ET24', 3179), 
#     L2EFChain_e('e12Tvh_loose1_FTK', 2897), 
#     L2EFChain_e('e24vh_medium1_FTK', 2898), 
#     L2EFChain_e('e24vh_medium1_SiTrk', 1001), 
#     L2EFChain_e('e22vh_medium1_SiTrk', 1005), 
#     L2EFChain_e('e22_medium_SiTrk', 173), 
#     L2EFChain_e('e4_etcut_SiTrk', 2153),
#     L2EFChain_e('e22vh_medium_SiTrk', 104), 
#     L2EFChain_e('e5_medium_SiTrk', 1629), 
#     L2EFChain_e('e5_tight_SiTrk', 2329), 
#     L2EFChain_e('e5_tight1_SiTrk', 1008), 
#     L2EFChain_e('e4_etcut_IdScan', 2155),    
#     L2EFChain_e('e22vh_medium1_IdScan', 1006), 
#     L2EFChain_e('e24vh_medium1_IdScan', 1002), 
#     L2EFChain_e('e5_tight1_IdScan', 1009), 
#     L2EFChain_e('e22vh_medium1_TRT', 1014), 
#     L2EFChain_e('e4_etcut_TRT', 2154),
#     L2EFChain_e('e24vh_medium1_TRT', 1013), 
#     L2EFChain_e('e5_medium_TRT', 2006), 
#     L2EFChain_e('e5_tight_TRT', 2330), 
#     L2EFChain_e('e5_tight1_TRT', 1009), 
#     L2EFChain_e('e20_loose_TRT', 2016), 
#     L2EFChain_e_TRT_Ringer('e20_medium_TRT', 105),     #not in v4 menu
#     L2EFChain_e_TRT_Ringer('e22vh_medium_TRT', 105),   #not in v4 menu
#     L2EFChain_e_L2Star('e4_etcut_L2StarB', 314),
#     L2EFChain_e_L2Star('e4_etcut_L2StarC', 315),
#     L2EFChain_e_L2Star('e12Tvh_loose1_L2StarB', 416),
#     L2EFChain_e_L2Star('e12Tvh_medium1_L2StarB', 577),
#     L2EFChain_e_L2Star('e7_medium1_L2StarB', 414),
#     L2EFChain_e_L2Star('e7T_medium1_L2StarB', 327),
#     L2EFChain_e_L2Star('e7T_loose1_L2StarB', 634), 
#     L2EFChain_e_L2Star('e7_loose1_L2StarB', 5123),
#     L2EFChain_e_L2Star('e12_medium1_L2StarB', 5124),
#     L2EFChain_e_L2Star('e24vh_medium1_L2StarB', 316),
#     L2EFChain_e_L2Star('e24vh_medium1_L2StarC', 317),
#     L2EFChain_e_L2Star('e5_tight1_L2StarB', 3374),
#     L2EFChain_e_L2Star('e5_tight1_L2StarC', 3375),
#     L2EFChain_e_NoIDTrkCut('e5_NoCut', 2341), 
#     L2EFChain_e_NoIDTrkCut('e15_NoCut', 531),
#     L2EFChain_e_NoIDTrkCut('e15_NoCut_R1', 2342),
#     L2EFChain_e_NoIDTrkCut('e5_medium1_IDTrkNoCut', 234),
#     L2EFChain_e_NoIDTrkCut('e15_loose1_IDTrkNoCut', 235),
#     L2EFChain_e_NoIDTrkCut('e15_loose1_IDTrkNoCut_R1', 2344),
#     L2EFChain_e_NoIDTrkCut('e22vh_medium_IDTrkNoCut', 530),
#     L2EFChain_e_NoIDTrkCut('e22vh_medium_IDTrkNoCut_R1', 2347),
#     L2EFChain_e_NoIDTrkCut('e22_medium_IDTrkNoCut', 175),
#     L2EFChain_e_NoIDTrkCut('e20_loose_IDTrkNoCut', 997), 
#     L2EFChain_e_NoIDTrkCut('e22_medium_IDTrkNoCut_R1', 2345),
#     L2EFChain_e_NoIDTrkCut('e20_loose_IDTrkNoCut_R1', 2346), 
#     L2EFChain_e_NoIDTrkCut('e24vh_medium1_IDTrkNoCut', 1004),
#     L2EFChain_e_NoIDTrkCut('e22vh_medium1_IDTrkNoCut', 1007),
#     L2EFChain_e_NoIDTrkCut('e24vh_medium1_IDTrkNoCut_R1', 2348),
#     L2EFChain_e_NoIDTrkCut('e22vh_medium1_IDTrkNoCut_R1', 2349),
#     L2EFChain_e_NoIDTrkCut_IDT('e24vh_medium1_IDTrkNoCut_IDT', 2450), #not in v4 menu
#     L2EFChain_e_noL2ID('e24vhi_medium1_noL2ID', 3181),
#     L2EFChain_e_noL2IDnoEFID('e24vhi_medium1_noL2IDnoEFID', 3182),
#     L2EFChain_e_noL2IDnoEF('e24vhi_medium1_noL2IDnoEF', 3183),
#     L2EFChain_e_noL2noEFID('e24vhi_medium1_noL2noEFID', 3184),
#     L2EFChain_e_noL2('e24vhi_medium1_noL2', 3185), 
#     ]


# if (TriggerFlags.triggerMenuSetup() in Lvl1Flags.ALFAv2Menus()):
#     Electrons += [
#         L2EFChain_e('e5_tight1_ALFA_ANY', 777), 
#         L2EFChain_e('e5_tight1_ALFA_MBTS_C', 778), 
#         L2EFChain_e('e5_tight1_ALFA_MBTS_A', 783), 
#         L2EFChain_e('e5_tight1_ALFA_ANY_UNPAIREDISO', 784), 
#         L2EFChain_e('e5_tight1_ALFA_MBTS_C_UNPAIREDISO', 781), 
#         L2EFChain_e('e5_tight1_ALFA_MBTS_A_UNPAIREDISO', 782), 
#         ]

# if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():
#     Electrons += [  ##e22vh_loose for previous menu is defined in ElectronDef_v2.py
#         L2EFChain_e('e22vh_loose', 534), 
#         ]
    
# if TriggerFlags.triggerMenuSetup() in Lvl1Flags.NoVHThresholdMenus(): #v2, v3, HI_v2, HI_v3
#     Electrons += [        
#         L2EFChain_e('e12T_medium', 5420), 
#         L2EFChain_e('e15_medium', 3), 
#         L2EFChain_e('e20_loose1', 401), 
#         L2EFChain_e('e20_looseTrk', 402), 
#         L2EFChain_e('e22_medium', 528), 
#         L2EFChain_e('e22_medium1', 49), 
#         L2EFChain_e('e22_medium2', 69), 
#         L2EFChain_e('e20_medium_SiTrk', 104), 
#         L2EFChain_e('e22_medium_TRT', 174), 
#         L2EFChain_e_NoIDTrkCut('e20_medium_IDTrkNoCut', 530),
#         L2EFChain_e_NoIDTrkCut('e20_medium_IDTrkNoCut_R1', 2350),
#         ]
    
# try:
#     from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e24i5_medium1_AthSel
#     print 'AthSel code does exist'
#     configAthSel=True
# except:
#     print 'AthSel code does not exist'
#     configAthSel=False
    
# if  configAthSel:
#     Electrons += [
#         L2EFChain_e('e60_medium1_AthSel', 5676), 
#         L2EFChain_e('e12Tvh_loose1_AthSel', 5234), 
#         L2EFChain_e('e24vh_loose0_AthSel', 5632), 
#         L2EFChain_e('e24vhi_medium1_AthSel', 3187), 
#         L2EFChain_e('e24vhi_medium1_AthLHSel', 3188), 
#         ]
    
# try:
#     from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_eGammaA_Brem 
#     doABrem=True
# except:
#     doABrem=False

# if doABrem:
#     Electrons += [            
#         L2EFChain_e('e24vhi_medium1_L2StarABrem', 3180), 
#         ]

# try:
#     from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_eGammaA
#     configL2Star=True
# except:
#     configL2Star=False

# if configL2Star :    
#     Electrons += [
#         L2EFChain_e_L2Star('e5_medium_L2StarA', 3370),
#         L2EFChain_e_L2Star('e5_medium_L2StarB', 3371),
#         L2EFChain_e_L2Star('e5_medium_L2StarC', 3372),
#         L2EFChain_e_L2Star('e12_medium_L2StarA', 3373),
#         L2EFChain_e_L2Star('e12_medium_L2StarB', 3374),
#         L2EFChain_e_L2Star('e12_medium_L2StarC', 3375), 
#         L2EFChain_e_L2Star('e22vh_medium_L2StarA',3376),
#         L2EFChain_e_L2Star('e22vh_medium_L2StarB',3377),
#         L2EFChain_e_L2Star('e22vh_medium_L2StarC',3378),
#         L2EFChain_e_NoIDTrkCut('e22vh_medium_L2Star_IDTrkNoCut', 1345),
#         L2EFChain_e_NoIDTrkCut('e20_L2Star_IDTrkNoCut', 1320),
#         ]


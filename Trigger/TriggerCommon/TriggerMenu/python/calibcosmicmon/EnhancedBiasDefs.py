# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
#
# SliceDef file for HLT signatures used for EnhancedBias Slice
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger('TriggerMenu.calibcosmon.EnhancedBiasDef')

from TriggerMenu.menu.HltConfig import *
from TriggerJobOpts.TriggerFlags  import TriggerFlags

#########################################################################


##USUAL STUPID HARDCODING OF THE ITEM LISTS DEPENDING ON THE MENU 
if TriggerFlags.triggerMenuSetup() == 'LS1_v1':        
    l1_seeds ={ 
                
        'low'       : ['L1_2EM3_EM12', 'L1_XS45', 'L1_J50', 'L1_MU4_J15', #'L1_FJ30',
                       'L1_MU10', 'L1_J30', 'L1_TE500', 'L1_EM10VH', 'L1_EM16VH', 'L1_TE700', 'L1_XE30', 'L1_XE20', 'L1_3J15', 'L1_TAU20', 'L1_MU6', 'L1_EM12'],
        
        'high' : ['L1_EM6_MU6','L1_2EM6_EM16VH',
                  'L1_2MU4',  'L1_2MU4_MU6', 'L1_2MU4_BARREL', 
                  'L1_2MU4_2EM3', 'L1_MU4_J30',
                  'L1_XS50','L1_XS60', #'L1_FJ50',
                  'L1_EM16V_XE20', 'L1_MU4_J50' ] ,
        
        'firstempty'    :['L1_EM3_FIRSTEMPTY','L1_TAU8_FIRSTEMPTY',
                          'L1_MU6_FIRSTEMPTY','L1_MU4_FIRSTEMPTY','L1_MU10_FIRSTEMPTY','L1_MU20_FIRSTEMPTY',                                 
                          'L1_2MU4_FIRSTEMPTY',
                          'L1_J10_FIRSTEMPTY''L1_J30_FIRSTEMPTY', #'L1_FJ30_FIRSTEMPTY'
                          ],


                
        'empty': ['L1_EM3_EMPTY','L1_EM6_EMPTY','L1_TAU8_EMPTY',
                         'L1_MU4_EMPTY','L1_MU10_EMPTY','L1_MU11_EMPTY',
                         'L1_J10_EMPTY','L1_J30_EMPTY',#'L1_FJ30_EMPTY',
                         'L1_2MU4_EMPTY','L1_MU4_J15_EMPTY'],                         

        'unpairediso' :['L1_EM3_UNPAIRED_ISO','L1_TAU8_UNPAIRED_ISO',
                        'L1_MU10_UNPAIRED_ISO','L1_MU4_UNPAIRED_ISO','L1_2MU6_UNPAIRED_ISO',                            
                        'L1_MU4_J15_UNPAIRED_ISO',
                        'L1_J30_UNPAIRED_ISO',#'L1_FJ30_UNPAIRED_ISO',
                        'L1_J10_UNPAIRED_ISO'],


        'unpairednoniso':['L1_EM3_UNPAIRED_NONISO','L1_TAU8_UNPAIRED_NONISO',
                          'L1_MU4_UNPAIRED_NONISO','L1_2MU6_UNPAIRED_NONISO',
                          'L1_J10_UNPAIRED_NONISO','L1_J30_UNPAIRED_NONISO'],
        
        }
else:
    l1_seeds ={ 

        'low'       : ['L1_MU6','L1_MU10','L1_TAU20',
                           'L1_EM12','L1_EM10VH', 'L1_EM18VH',
                           'L1_J50',  'L1_J30','L1_3J15', 'L1_J30.32ETA49'                           
                           'L1_XE35' ],
        
        'empty': ['L1_EM3_EMPTY','L1_EM7_EMPTY','L1_TAU8_EMPTY',
                         'L1_MU4_EMPTY','L1_MU11_EMPTY',
                         'L1_J12_EMPTY','L1_J30_EMPTY','L1_J30.32ETA49_EMPTY'],

        }
    


#########################################################################

class L2EFChain_EnhancedBiasTemplate(L2EFChainDef):
    def __init__(self, chainDict):
        self.L2sequenceList   = []
        self.EFsequenceList   = []
        self.L2signatureList  = []
        self.EFsignatureList  = []
        self.TErenamingDict   = []
        
        self.chainName = chainDict['chainName']
        self.chainPart = chainDict['chainParts']  
        self.chainL1Item = chainDict['L1item']       
        self.chainCounter = chainDict['chainCounter']       
        self.L2Name = 'L2_'+self.chainPart['chainPartName']
        self.EFName = 'EF_'+self.chainPart['chainPartName']
        self.chainPartName = self.chainPart['chainPartName']             
        self.algType  = self.chainPart['algType']
        
        self.L2InputTE = self.chainL1Item 
        newL1items = self.L2InputTE 


        # sequence/signature/TErenaming
        from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo        
        from TrigHypoCommonTools.TrigHypoCommonToolsConf import L1InfoHypo
        
        self.dummyAlg = PESA__DummyUnseededAllTEAlgo("EF_DummyFEX_%s" %(self.algType))
        self.dummyAlg.createRoIDescriptors  = False
        self.dummyAlg.NumberOfOutputTEs     = 1
            
        self.hypoEB = L1InfoHypo("EF_Hypo_%s" %(self.algType))
        
        if self.algType in l1_seeds:
            self.hypoEB.L1ItemNames = l1_seeds[self.algType]        
            self.hypoEB.AlwaysPass                = False
            self.hypoEB.InvertSelection           = False
            self.hypoEB.InvertL1ItemNameSelection = False
            self.hypoEB.L1TriggerBitMask          = 0
            self.hypoEB.TriggerTypeBitMask        = 0
            self.hypoEB.UseBeforePrescaleBit      = True

            self.EFsequenceList += [['',
                                     [self.dummyAlg, self.hypoEB],
                                     'EF_eb']]            
            self.EFsignatureList += [[['EF_eb']]]
            self.TErenamingDict = {
                'EF_eb':     'EF_' + self.chainName,
                }
        
        else:
            log.error("No configuration exist for EB chain: %s" %(self.algType))
            self.hypoEB.L1ItemNames = []


        L2EFChainDef.__init__(self, self.chainName, self.L2Name, self.chainCounter,
                              newL1items, self.EFName, self.chainCounter, self.L2InputTE)

            
    def defineSequences(self):   
        for sequence in self.L2sequenceList:
            self.addL2Sequence(*sequence)            
        for sequence in self.EFsequenceList:
            self.addEFSequence(*sequence)
          
    def defineSignatures(self):
        for signature in self.L2signatureList:
            self.addL2Signature(*signature)
        for signature in self.EFsignatureList:
            self.addEFSignature(*signature)          
           
    def defineTErenaming(self):
        self.TErenamingMap=self.TErenamingDict


        

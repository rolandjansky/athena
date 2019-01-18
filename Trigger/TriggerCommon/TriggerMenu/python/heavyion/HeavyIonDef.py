# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for heavy ion chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from TriggerMenu.menu.HltConfig import L2EFChainDef, mergeRemovingOverlap

from TrigT2MinBias.TrigT2MinBiasConfig import MbMbtsHypo, L2MbMbtsFex, L2MbSpFex, L2MbSpUPC

from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence
#fexes.efid = TrigEFIDSequence("minBias","minBias","InsideOut").getSequence()
#fexes.efid2P = TrigEFIDSequence("minBias2P","minBias2","InsideOutLowPt").getSequence()
efiddataprep = TrigEFIDSequence("minBias","minBias","DataPrep").getSequence()
efid = TrigEFIDSequence("minBias","minBias","InsideOut").getSequence()
efid_heavyIon = TrigEFIDSequence("heavyIonFS","heavyIonFS","InsideOut").getSequence()
efid2P = TrigEFIDSequence("minBias2P","minBias2","InsideOutLowPt").getSequence()

from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo as DummyRoI
dummyRoI=DummyRoI(name='MinBiasDummyRoI', createRoIDescriptors = True, NumberOfOutputTEs=1)

# for HI
from TrigHIHypo.TrigHIHypoConfig import HIEFTrackHypo_AtLeastOneTrack
atLeastOneTrack = HIEFTrackHypo_AtLeastOneTrack(name='HIEFTrackHypo_AtLeastOneTrack')

#For noise supression
from TriggerMenu.commonUtils.makeCaloSequences import getFullScanCaloSequences

###########################################################################
#  All min bias
###########################################################################
class L2EFChain_HI(L2EFChainDef):
    
    def __init__(self, chainDict, doggFgap = False):

        self.L2sequenceList   = []
        self.EFsequenceList   = []
        self.L2signatureList  = []
        self.EFsignatureList  = []
        self.TErenamingDict   = []
        
        self.chainPart = chainDict['chainParts']
        
        self.chainL1Item = chainDict['L1item']        
        self.chainPartL1Item = self.chainPart['L1item']
        
        self.chainCounter = chainDict['chainCounter']       
        self.L2Name = 'L2_'+self.chainPart['chainPartName']
        self.EFName = 'EF_'+self.chainPart['chainPartName']
        #self.mult = int(self.chainPart['multiplicity'])
        self.chainName = chainDict['chainName']
        self.chainPartName = self.chainPart['chainPartName']
        #self.chainPartNameNoMult = self.chainPartName[1:] if self.mult > 1 else self.chainPartName
        self.doggFgap = doggFgap
        
        self.L2InputTE = self.chainPartL1Item or self.chainL1Item
        # cut of L1_, _EMPTY,..., & multiplicity
        if self.L2InputTE is not '':
            self.L2InputTE = self.L2InputTE.replace("L1_","")
            self.L2InputTE = self.L2InputTE.split("_")[0]
            self.L2InputTE = self.L2InputTE[1:] if self.L2InputTE[0].isdigit() else self.L2InputTE

        if "v2" in self.chainPart['eventShape']:
            self.setup_hi_eventshape()
        elif "v3" in self.chainPart['eventShape']:
            self.setup_hi_eventshape()
        elif "ucc" in self.chainPart['recoAlg']:
            self.setup_hi_ultracentral()
        elif "upc" in self.chainPart['recoAlg']:
            if '' == self.chainPart['gap'] or 'gg' in self.chainPart['hypoL2Info']:
                self.setup_hi_ultraperipheral()
            else:
                self.setup_hi_ultraperipheral_gap()
        elif "hipeb" in self.chainPart['addInfo']:
            self.setup_hi_PEB()    
        
        L2EFChainDef.__init__(self, self.chainName, self.L2Name, self.chainCounter, self.chainL1Item, self.EFName, self.chainCounter, self.L2InputTE)

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
        self.TErenamingMap = self.TErenamingDict

############################### HELPER FUNCTIONS ##############################
    def setup_hi_eventshape(self):

        EShypo_temp = self.chainPart['extra']
        ESth=EShypo_temp.lstrip('th')
        #print 'igb: ES threshold:', ESth
        VetoHypo = None
        ESHypo=None
        ESDiHypo=None
        if 'v2' == self.chainPart['eventShape']:
            from TrigHIHypo.VnHypos import V2_th
            chainSuffix = 'v2_th'+ESth
            assert V2_th.has_key(int(ESth)), "Missing V2 configuration for threshold "+ESth
            ESHypo=V2_th[int(ESth)] 
            if self.chainPart['eventShapeVeto'] == 'veto3':
                from TrigHIHypo.VnHypos import V3_th1_veto
                VetoHypo = V3_th1_veto
                
        elif 'v3' == self.chainPart['eventShape']:
            from TrigHIHypo.VnHypos import V3_th
            chainSuffix = 'v3_th'+ESth
            assert V3_th.has_key(int(ESth)), "Missing V3 configuration for threshold "+ESth         
            ESHypo=V3_th[int(ESth)] 
            if self.chainPart['eventShapeVeto']  == 'veto2':
                from TrigHIHypo.VnHypos import V2_th1_veto
                VetoHypo = V2_th1_veto

        elif 'v23' == self.chainPart['eventShape']:
            from TrigHIHypo.VnHypos import V3_th,V2_th
            chainSuffix = 'th'+ESth
            th=int(ESth)
            ESDiHypo = [V2_th[th], V3_th[th]]

        elif 'v2A' == self.chainPart['eventShape']:
            from TrigHIHypo.VnHypos import V2Assym
            chainSuffix = 'v2A_th'+ESth
            ESHypo=V2Assym(ESth, 'A')

        elif 'v2C' == self.chainPart['eventShape']:
            from TrigHIHypo.VnHypos import V2Assym
            chainSuffix = 'v2C_th'+ESth
            ESHypo=V2Assym(ESth, 'C')


        elif 'v3A' == self.chainPart['eventShape']:
            from TrigHIHypo.VnHypos import V3Assym
            chainSuffix = 'v3A_th'+ESth
            ESHypo=V3Assym(ESth, 'A')

        elif 'v3C' == self.chainPart['eventShape']:
            from TrigHIHypo.VnHypos import V3Assym
            chainSuffix = 'v3C_th'+ESth
            ESHypo=V3Assym(ESth, 'C')



        #print "ERROR", ESHypo, VetoHypo, ESDiHypo, " when making chain ", self.chainPart

        from TrigHIHypo.UE import theUEMaker, theFSCellMaker

        ########### Sequence List ##############
        self.L2sequenceList += [["",
                                 [dummyRoI],
                                 'L2_hi_step1']] 
        self.EFsequenceList += [[['L2_hi_step1'], 
                                     [theFSCellMaker], 'EF_hi_step1_fs']]

        self.EFsequenceList += [[['EF_hi_step1_fs'], 
                                 [theUEMaker], 'EF_hi_step1_ue']]

        self.L2signatureList += [ [['L2_hi_step1']] ]
        self.EFsignatureList += [ [['EF_hi_step1_fs']] ]
        self.EFsignatureList += [ [['EF_hi_step1_ue']] ]

        if ESHypo:
            self.EFsequenceList += [[['EF_hi_step1_ue'], 
                                     [ESHypo], 'EF_hi_step2']]
            self.EFsignatureList += [ [['EF_hi_step2']] ]

        if VetoHypo:
            self.EFsequenceList += [[['EF_hi_step2'], 
                                     [VetoHypo], 'EF_hi_step_veto']]
            self.EFsignatureList += [ [['EF_hi_step_veto']] ]

        if ESDiHypo:
            self.EFsequenceList += [[['EF_hi_step1_ue'], 
                                     [ESDiHypo[0]], 'EF_hi_step2']]
            self.EFsequenceList += [[['EF_hi_step2'], 
                                     [ESDiHypo[1]], 'EF_hi_step3']]

            self.EFsignatureList += [ [['EF_hi_step2']] ]
            self.EFsignatureList += [ [['EF_hi_step3']] ]

    
        self.TErenamingDict = {
            'L2_hi_step1': mergeRemovingOverlap('L2_hi_step1_', chainSuffix),
            'EF_hi_step1_fs': mergeRemovingOverlap('EF_hi_fs_', chainSuffix),
            'EF_hi_step1_ue': mergeRemovingOverlap('EF_hi_ue_', chainSuffix),
            'EF_hi_step2': mergeRemovingOverlap('EF_hi_', chainSuffix),
            'EF_hi_step3': mergeRemovingOverlap('EF_hi_', '_and_v3_'+chainSuffix),
            'EF_hi_step_veto': mergeRemovingOverlap('EF_hi_veto_', chainSuffix),
            }

###########################
    def setup_hi_ultracentral(self):

        if 'perfzdc' in self.chainPart['extra']:
            from TrigHIHypo.UltraCentralHypos import UltraCentral_PT
            from TrigT2MinBias.TrigT2MinBiasConfig import L2MbZdcFex_LG, L2MbZdcFex_HG, L2MbZdcHypo_PT
            theL2Fex1  = L2MbZdcFex_LG
            theL2Fex2  = L2MbZdcFex_HG
            theL2Hypo1 = L2MbZdcHypo_PT
            chainSuffix = 'perfzdc_ucc'
            UChypo=UltraCentral_PT("UltraCentralHypo_PT")
        elif 'perf'  in self.chainPart['extra']:
            from TrigHIHypo.UltraCentralHypos import UltraCentral_PT
            chainSuffix = 'perf_ucc'
            UChypo=UltraCentral_PT("UltraCentralHypo_PT")
        else:
            from TrigHIHypo.UltraCentralHypos import UCC_th
            threshold = self.chainPart['extra']
            UChypo=UCC_th[threshold]
            chainSuffix = threshold
            if 'zdcpu' in self.chainPart['pileupInfo']:
                from TrigT2MinBias.TrigT2MinBiasConfig import L2MbZdcFex_LG
                from TrigT2MinBias.TrigT2MinBiasConfig import L2MbZdcHypo_sideAC_zdc_LG
                theL2Fex1  = L2MbZdcFex_LG
                theL2Hypo1 = L2MbZdcHypo_sideAC_zdc_LG

        from TrigHIHypo.UE import theUEMaker, theFSCellMaker

        ########### Sequence List ##############
        self.L2sequenceList += [["",
                                 [dummyRoI],
                                 'L2_hi_step1']] 
        self.EFsequenceList += [[['L2_hi_step1'], 
                                     [theFSCellMaker], 'EF_hi_step1_fs']]

        self.EFsequenceList += [[['EF_hi_step1_fs'], 
                                 [theUEMaker], 'EF_hi_step1_ue']]

        self.EFsequenceList += [[['EF_hi_step1_ue'], 
                                 [UChypo], 'EF_hi_step2']]
        if 'perfzdc' in self.chainPart['extra']:
            self.EFsequenceList += [[['EF_hi_step2'],
                                     [theL2Fex1], 'EF_hi_step3']]
            self.EFsequenceList += [[['EF_hi_step3'],
                                     [theL2Fex2, theL2Hypo1], 'EF_hi_step4']]
        if 'zdcpu' in self.chainPart['pileupInfo']:
            self.EFsequenceList += [[['EF_hi_step2'],
                                     [theL2Fex1, theL2Hypo1], 'EF_hi_step3']]


        ########### Signatures ###########
        self.L2signatureList += [ [['L2_hi_step1']] ]
        self.EFsignatureList += [ [['EF_hi_step1_fs']] ]
        self.EFsignatureList += [ [['EF_hi_step1_ue']] ]
        self.EFsignatureList += [ [['EF_hi_step2']] ]
        if 'perfzdc' in self.chainPart['extra']:
            self.EFsignatureList += [ [['EF_hi_step3']] ]
            self.EFsignatureList += [ [['EF_hi_step4']] ]
        if 'zdcpu' in self.chainPart['pileupInfo']:
            self.EFsignatureList += [ [['EF_hi_step3']] ]
    
        self.TErenamingDict = {
            'L2_hi_step1': mergeRemovingOverlap('L2_hi_step1_', chainSuffix),
            'EF_hi_step1_fs': mergeRemovingOverlap('EF_hi_fs_', chainSuffix),
            'EF_hi_step1_ue': mergeRemovingOverlap('EF_hi_ue_', chainSuffix),
            'EF_hi_step2': mergeRemovingOverlap('EF_hi_lg_', chainSuffix),
            }
        if 'perfzdc' in self.chainPart['extra']:
            self.TErenamingDict['EF_hi_step3'] = mergeRemovingOverlap('EF_hi_hg_', chainSuffix)
            self.TErenamingDict['EF_hi_step4'] = mergeRemovingOverlap('EF_hi_', chainSuffix)
        if 'zdcpu' in self.chainPart['pileupInfo']:
            self.TErenamingDict['EF_hi_step3'] = mergeRemovingOverlap('EF_hi_', chainSuffix+'_zdcpu')

###########################
    def setup_hi_ultraperipheral(self):
            
        theL2MbtsFex=L2MbMbtsFex
        theL2MbtsHypo=MbMbtsHypo("L2MbMbtsHypo_1_1_inn_veto")
        theL2PixelFex  = L2MbSpFex
        
        fullScanSeqMap = getFullScanCaloSequences()

        if 'loose' in self.chainPart['hypoL2Info']:
            minPixel=6
            maxPixel=40
            chainSuffix = 'loose_upc'
        if 'medium' in self.chainPart['hypoL2Info']:
            minPixel=6
            maxPixel=30
            chainSuffix = 'medium_upc'
        if 'tight' in self.chainPart['hypoL2Info']:
            minPixel=6
            maxPixel=25
            chainSuffix = 'tight_upc'
        if 'gg' in self.chainPart['hypoL2Info']:
            minPixel=0
            maxPixel=15
            chainSuffix = 'gg_upc'

        theL2PixelHypo  = L2MbSpUPC("MbPixelSpUPC_min"+str(minPixel)+'_max'+str(maxPixel), minPixel, maxPixel)
        
        if self.doggFgap:
             chainSuffix = chainSuffix + '_' + self.chainPart['gap']
            
        ########### Sequence List ##############    
        self.L2sequenceList += [["",
                                 [dummyRoI],
                                 'L2_hi_step1']] 
         	
        if not self.doggFgap:    
            self.L2sequenceList += [[['L2_hi_step1'], 
                                         [theL2MbtsFex, theL2MbtsHypo], 'L2_hi_mbtsveto']]
            self.L2sequenceList += [['L2_hi_mbtsveto',
                                         efiddataprep,
                                     'L2_hi_iddataprep']]
        else:
             self.L2sequenceList += [['L2_hi_step1',
                                         efiddataprep,
                                     'L2_hi_iddataprep']]
        self.L2sequenceList += [[['L2_hi_iddataprep'],
                                 [theL2PixelFex, theL2PixelHypo],
                                 'L2_hi_pixel']]
        
        if "noiseSup" in self.chainPart['addInfo']:
            te_in=''
            hypo = []
            for step in fullScanSeqMap:
                if step == 'EF_FSTopoClustersED': continue
                if step == 'EF_FSTopoClusters':
                    from TrigHIHypo.TrigHIHypoConfig import LbyLTopoClusterHypoConfig
                    hypo = [ LbyLTopoClusterHypoConfig() ]
                self.EFsequenceList += [[[te_in],fullScanSeqMap[step] + hypo ,step]]
                self.EFsignatureList += [ [[step]] ]
                te_in=step

        
        ########### Signatures ###########
        self.L2signatureList += [ [['L2_hi_step1']] ]
        if not self.doggFgap: self.L2signatureList += [ [['L2_hi_mbtsveto']] ]
        self.L2signatureList += [ [['L2_hi_iddataprep']] ]
        self.L2signatureList += [ [['L2_hi_pixel']] ]
    
        self.TErenamingDict = {
            'L2_hi_step1': mergeRemovingOverlap('L2_hi_step1_', chainSuffix),
            'L2_hi_iddataprep': mergeRemovingOverlap('EF_hi_iddataprep_', chainSuffix),
            'L2_hi_pixel': mergeRemovingOverlap('EF_hi_pixel_', chainSuffix),
            }
        if not self.doggFgap:
            self.TErenamingDict ['L2_hi_mbtsveto'] = mergeRemovingOverlap('EF_hi_mbtsveto_', chainSuffix)
        if "noiseSup" in self.chainPart['addInfo']:
            self.TErenamingDict['EF_full']=mergeRemovingOverlap('EF_', chainSuffix+'fs')
            self.TErenamingDict['EF_full_cell']=mergeRemovingOverlap('EF_', chainSuffix+'fscalocell')
            self.TErenamingDict['EF_FSTopoClusters']=mergeRemovingOverlap('EF_', chainSuffix+'fscalotopo')
            self.TErenamingDict['EF_FSTopoClustersED']=mergeRemovingOverlap('EF_', chainSuffix+'fscalotopoed')
                
    def setup_hi_ultraperipheral_gap(self):
        gapthX=""    
        from TrigCaloRec.TrigCaloRecConf import TrigL1BSTowerMaker
        from TrigCaloRec.TrigCaloRecConf import TrigL1FCALTTSumFex
        theL1BS = TrigL1BSTowerMaker()
        theL1BSFex = TrigL1FCALTTSumFex('TrigL1FCALTTSumFex')

        from TrigHIHypo.UE import theUEMaker, theFSCellMaker
        from TrigHIHypo.GapHypos import ttFgapA, ttFgapC, cellFgapA, cellFgapC, ttFgapA3, ttFgapC3, cellFgapA3, cellFgapC3, ttFgapA5, ttFgapC5, cellFgapA5, cellFgapC5, ttFgapA10, ttFgapC10, cellFgapA10, cellFgapC10, cellFgapAPerf, cellFgapCPerf
            
        # L2 sel (TT) is used when The chain is sither L2Fgap* or Fgap*, and not used when EFGap, similarily for EF (cells) part
        gap  = self.chainPart['gap']
        gapthX = gap[len(gap.rstrip('0123456789')):]
            
        #        theSptrkMaker = 
        #        theSptrkHypo = 
        self.L2sequenceList += [["",
                                 [dummyRoI],
                                 'L2_upc_step1']] 
        #self.L2signatureList += [ [['L2_upc_step1']] ]

        self.L2sequenceList += [["L2_upc_step1",
                                     [theL1BS, theL1BSFex],
                                     'L2_upc_step2']] 
        #self.L2signatureList += [ [['L2_upc_step2']] ]

        if not gapthX:     
            self.L2sequenceList += [["L2_upc_step2",
                                     [ttFgapA],
                                     'L2_upc_step3_A']] 
            self.L2sequenceList += [["L2_upc_step2",
                                     [ttFgapC],
                                     'L2_upc_step3_C']] 
        elif int(gapthX)==3:                
            self.L2sequenceList += [["L2_upc_step2",
                                     [ttFgapA3],
                                     'L2_upc_step3_A']] 
            self.L2sequenceList += [["L2_upc_step2",
                                     [ttFgapC3],
                                     'L2_upc_step3_C']]
        elif int(gapthX)==5:                
            self.L2sequenceList += [["L2_upc_step2",
                                     [ttFgapA5],
                                     'L2_upc_step3_A']] 
            self.L2sequenceList += [["L2_upc_step2",
                                     [ttFgapC5],
                                     'L2_upc_step3_C']]
        elif int(gapthX)==10:                
            self.L2sequenceList += [["L2_upc_step2",
                                     [ttFgapA10],
                                     'L2_upc_step3_A']] 
            self.L2sequenceList += [["L2_upc_step2",
                                     [ttFgapC10],
                                     'L2_upc_step3_C']]
         
        if gap in [ 'FgapA', 'FgapA5', 'FgapA10', 'L2FgapA']:
            self.L2signatureList += [ [['L2_upc_step3_A']] ]
        if gap in [ 'FgapC', 'FgapC5', 'FgapC10', 'L2FgapC']:
            self.L2signatureList += [ [['L2_upc_step3_C']] ]
        if gap in [ 'FgapAC', 'FgapAC3', 'L2FgapAC']:
            self.L2signatureList += [ [['L2_upc_step3_A']] ]
            self.L2signatureList += [ [['L2_upc_step3_C']] ]


        # sequence starts back from RoI    
        self.EFsequenceList += [[['L2_upc_step1'], 
                                 [theFSCellMaker], 
                                 'EF_upc_step1']]
        #self.EFsignatureList += [ [['EF_upc_step1']] ]

        self.EFsequenceList += [["EF_upc_step1",
                                 [theUEMaker],
                                 'EF_upc_step2']] 
        #self.EFsignatureList += [ [['EF_upc_step2']] ]

        if 'EF' in gap: # this are perfromance triggers
            self.EFsequenceList += [["EF_upc_step2",
                                     [cellFgapCPerf ],
                                     'EF_upc_step3_CPerf']] 
              
            self.EFsequenceList += [["EF_upc_step2",
                                     [cellFgapAPerf ],
                                     'EF_upc_step3_APerf']] 
        else:
            if not gapthX:
                self.EFsequenceList += [["EF_upc_step2",
                                         [cellFgapC ],
                                         'EF_upc_step3_C']] 
                
                self.EFsequenceList += [["EF_upc_step2",
                                         [cellFgapA ],
                                         'EF_upc_step3_A']] 
            elif int(gapthX)==3:
                self.EFsequenceList += [["EF_upc_step2",
                                         [cellFgapC3 ],
                                         'EF_upc_step3_C']] 
                    
                self.EFsequenceList += [["EF_upc_step2",
                                         [cellFgapA3 ],
                                         'EF_upc_step3_A']]
            elif int(gapthX)==5:
                self.EFsequenceList += [["EF_upc_step2",
                                         [cellFgapC5 ],
                                         'EF_upc_step3_C']] 
                    
                self.EFsequenceList += [["EF_upc_step2",
                                         [cellFgapA5 ],
                                         'EF_upc_step3_A']]
            elif int(gapthX)==10:
                self.EFsequenceList += [["EF_upc_step2",
                                         [cellFgapC10 ],
                                         'EF_upc_step3_C']] 
                   
                self.EFsequenceList += [["EF_upc_step2",
                                         [cellFgapA10 ],
                                         'EF_upc_step3_A']]
                    
                
        if gap in [ 'FgapA', 'FgapA5', 'FgapA10']:
            self.EFsignatureList += [ [['EF_upc_step3_A']] ]
        if gap == 'EFFgapA':
            self.EFsignatureList += [ [['EF_upc_step3_APerf']] ]

        if  gap in [ 'FgapC', 'FgapC5', 'FgapC10']:
            self.EFsignatureList += [ [['EF_upc_step3_C']] ]
        if gap =='EFFgapC':
            self.EFsignatureList += [ [['EF_upc_step3_CPerf']] ]
        if gap in [ 'FgapAC', 'FgapAC3']:
            self.EFsignatureList += [ [['EF_upc_step3_A']] ]
            self.EFsignatureList += [ [['EF_upc_step3_C']] ]
        if  gap ==  'EFFgapAC':
            self.EFsignatureList += [ [['EF_upc_step3_APerf']] ]
            self.EFsignatureList += [ [['EF_upc_step3_CPerf']] ]
        
        self.TErenamingDict = {
            'L2_upc_step1': 'L2_dummy_sp',
            'L2_upc_step2': 'L2_TTGapCalculation',
            'L2_upc_step3_A': 'L2_TTGapA'+str(gapthX),
            'L2_upc_step3_C': 'L2_TTGapC'+str(gapthX),
            'EF_upc_step1': 'EF_AllCells',
            'EF_upc_step2': 'EF_UE',
            'EF_upc_step3_A': 'EF_CellGapA'+str(gapthX),
            'EF_upc_step3_C': 'EF_CellGapC'+str(gapthX),
            'EF_upc_step3_APerf': 'EF_CellGapAPerf',
            'EF_upc_step3_CPerf': 'EF_CellGapCPerf',
            'EF_upc_step4': 'EF_twoTracks',
            }
    def setup_hi_PEB(self):
        from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter
        HISubDetListWriter = TrigSubDetListWriter("HISubDetListWriter")
        HISubDetListWriter.SubdetId = ['TDAQ_CTP','InnerDetector','FCal','FORWARD_ZDC','Muons'] 
        HISubDetListWriter.MaxRoIsPerEvent=1
 
        self.robWriter = [HISubDetListWriter]            
        self.L2sequenceList += [['', self.robWriter, 'L2_hipeb']]     
        self.L2signatureList += [[['L2_hipeb']]]                    
#####################################################################
    
#if __name__ == '__main__':
#    triggerPythonConfig = TriggerPythonConfig('hlt.xml', None)
#    for m in Muons:
#        m.generateMenu(triggerPythonConfig)
#    triggerPythonConfig.writeConfigFiles()
    

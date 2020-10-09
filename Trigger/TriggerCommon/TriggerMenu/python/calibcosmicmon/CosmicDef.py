# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
#
# SliceDef file for HLT signatures used exclusively in the cosmic slice
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from TriggerMenu.menu.HltConfig import L2EFChainDef, mergeRemovingOverlap

###################################################################################
class L2EFChain_CosmicTemplate(L2EFChainDef):

    def __init__(self, chainDict):

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
        self.mult = int(self.chainPart['multiplicity'])
        self.chainName = chainDict['chainName']
        self.chainPartName = self.chainPart['chainPartName']


        self.L2InputL1Item = self.chainPartL1Item or self.chainL1Item
        if self.L2InputL1Item:
            from TriggerMenu.menu.L1Seeds import getInputTEfromL1Item
            self.L2InputTE = getInputTEfromL1Item(self.L2InputL1Item)
        else:
            self.L2InputTE = ''

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % self.chainName

        log.verbose('in L2EFChain_CosmicTemplate constructor for %s', self.chainName)

        #---------------------------------
        # CHAIN DEFINITION SPECIFICATIONS
        #---------------------------------
        if ('tilecalib' in  self.chainPart['purpose']) \
                & ('laser' in self.chainPart['addInfo']):
            self.setupCosmicTileCalibration()

        elif 'larps' in self.chainPart['purpose']:
            self.setupCosmicLArPreSNoise()

        elif 'larhec' in self.chainPart['purpose']:
            self.setupCosmicLArHECNoise()

            
        elif ('sct' in self.chainPart['purpose']):
            self.setupCosmicIDNoiseCalibration()

        elif ('id' in  self.chainPart['purpose']):
            self.setupCosmicAllTeChains()

        else:
            log.error('Chain %s could not be assembled' % (self.chainPartName))
            return False      
        L2EFChainDef.__init__(self, self.chainName, self.L2Name, self.chainCounter,
                            self.chainL1Item, self.EFName, self.chainCounter, self.L2InputTE)

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
      

    ##################################################################
    def setupCosmicTileCalibration(self):
        
        # Configuration
        from TrigDetCalib.TrigDetCalibConfig import TileSubDetListWriter
        self.l2_tileSubDetListWriter = TileSubDetListWriter("L2_Cosmic"+self.chainName+"TileSubDetListWriter")
        self.l2_tileSubDetListWriter.Subdetectors = "Tile"
        self.l2_tileSubDetListWriter.MaxRoIsPerEvent = 1
        self.l2_tileSubDetListWriter.addL1Calo = True
        
        from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
        self.EF_Dummy = DummyFEX("EF_%s" % self.chainName)
        
        # Sequences
        self.L2sequenceList += [[ self.L2InputTE, 
                                  [self.l2_tileSubDetListWriter],
                                  'L2_step1']]
        self.EFsequenceList += [[ ['L2_step1'], [self.EF_Dummy], 'EF_step1']]
        
        # Signatures
        self.L2signatureList += [ [['L2_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_step1']*self.mult] ]
        # TE renaming
        self.TErenamingDict = {
            'L2_step1': mergeRemovingOverlap('L2_', 'Cosmic_'+self.chainName+'_TileSubDetListWriter'),
            'EF_step1': mergeRemovingOverlap('EF_', self.chainName),
            }               	         


    ##################################################################
    def setupCosmicLArPreSNoise(self):
        #L2 sequence setup
        if 'L1_EM' in self.chainL1Item:            
            from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_AllEm
            theTrigT2CaloEgammaFex = T2CaloEgamma_AllEm()
        else:   # ('L1_J' in self.chainL1Item) | ('L1_TAU' in self.chainL1Item):
            from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_All
            theTrigT2CaloEgammaFex = T2CaloEgamma_All()
            
        from TrigEgammaHypo.TrigL2CaloHypoConfig import TrigL2CaloLayersHypo_PreS_080

        self.L2sequenceList += [[ self.L2InputTE, 
                                  [theTrigT2CaloEgammaFex,
                                   TrigL2CaloLayersHypo_PreS_080('TrigL2CaloLayersHypo_'+self.chainName+'_080')],
                                  'L2_step1']]

        self.L2signatureList+=[ [['L2_step1']*self.mult] ]
        self.TErenamingDict = {
            'L2_step1': mergeRemovingOverlap('L2_','TrigL2CaloLayersHypo_'+self.chainName+'_080'),
            }

    ##################################################################
    def setupCosmicLArHECNoise(self):
        
        from TrigDetCalib.TrigDetCalibConfig import EtaHypo_HEC,LArL2ROBListWriter
        
        self.L2sequenceList += [[ self.L2InputTE, 
                                  [ EtaHypo_HEC('EtaHypo_HEC_' + self.chainPartName),],
                                  'L2_step1']]
        
        self.L2sequenceList += [[ 'L2_step1',
                                  [LArL2ROBListWriter('LArL2ROBListWriter_' + self.chainName, addCTPResult = True, addL2Result = True,etaWidth = 0.2, phiWidth = 0.2),],                                  
                                  'L2_step2']]
        
        self.L2signatureList+=[ [['L2_step1']*self.mult] ]
        self.L2signatureList+=[ [['L2_step2']*self.mult] ]
        
        self.TErenamingDict = {
            'L2_step1': mergeRemovingOverlap('L2_','EtaHypo_HEC_'+self.chainName),
            'L2_step2': mergeRemovingOverlap('L2_','LArL2ROBListWriter_'+self.chainName),
            
            }
    
        
        
            
    ##################################################################



    def setupCosmicIDNoiseCalibration(self):

        from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter

        l2_SCTSubDetListWriter = TrigSubDetListWriter("CosmicSCTNoiseSubDetListWriter")
        l2_SCTSubDetListWriter.Subdetectors = "SCT"
        l2_SCTSubDetListWriter.extraROBs = []
        theRobWriter= [l2_SCTSubDetListWriter]
        
        
        self.L2sequenceList += [[ '', theRobWriter,  'L2_step1']]
        self.L2signatureList+=[ [['L2_step1']*self.mult] ]
        
        self.TErenamingDict = {
            'L2_step1': mergeRemovingOverlap('L2_','Calib'+self.chainName),
            }

        
##################################################################
    def setupCosmicAllTeChains(self):

        newchainName = self.chainName
        if ('ds' in self.chainPart['addInfo']): newchainName = self.chainName.replace('_ds','')
        if ('_L1' in self.chainName): 
            pos = self.chainName.find('_L1')
            newchainName = self.chainName[:pos]

        # common L2 sequence
        from TrigGenericAlgs.GenericDummyUnseededAllTEAlgoConfig import GenericDummyUnseededAllTEAlgo

        theAllTEDummyFakeROI = GenericDummyUnseededAllTEAlgo("Cosmic"+newchainName+"AllTEDummy")

        from TrigHypoCommonTools.TrigHypoCommonToolsConf import L1InfoHypo
        theL1InfoHypo = L1InfoHypo("L1InfoHypo"+newchainName)
        theL1InfoHypo.TriggerTypeBitMask=0x80
        theL1InfoHypo.TriggerTypeBit=0x80
        theL1InfoHypo.InvertSelection=False
    
        theL1InfoHypoRNDMReject = L1InfoHypo("L1InfoHypoRNDMReject"+newchainName) 
        theL1InfoHypoRNDMReject.InvertSelection=True
        theL1InfoHypoRNDMReject.TriggerTypeBit = 0x81
        theL1InfoHypoRNDMReject.TriggerTypeBitMask=0xff
        
        self.L2sequenceList += [['',
                                [theAllTEDummyFakeROI, theL1InfoHypo, theL1InfoHypoRNDMReject],
                                'L2_ih']]
        self.L2signatureList+=[ [['L2_ih']*self.mult] ]

 
        # former CosmicsAllTeEFID chain        
        from InDetTrigRecExample.EFInDetConfig import TrigEFIDInsideOut_CosmicsN
        theEFIDTracking=TrigEFIDInsideOut_CosmicsN()
        from TrigMinBias.TrigMinBiasConfig import MbTrkFex_1, MbTrkHypo_1
        thetrackcnt =  MbTrkFex_1("MbTrkFex_"+newchainName)
        thetrackcnt.InputTrackContainerName = "InDetTrigTrackSlimmerIOTRT_CosmicsN_EFID"
        theefidcosmhypo = MbTrkHypo_1("MbTrkHypo_"+newchainName)
        theefidcosmhypo.AcceptAll_EF=False
        theefidcosmhypo.Required_ntrks=1
        theefidcosmhypo.Max_z0=1000.0
        
        self.EFsequenceList += [[['L2_ih'],theEFIDTracking.getSequence()+[thetrackcnt,  theefidcosmhypo], 'EF_efid']]
        self.EFsignatureList +=  [ [['EF_efid']*self.mult] ]


        # DATASCOUTING CHAIN  
        if ("ds" in self.chainPart['addInfo']) and (self.mult==1):
            from TrigDetCalib.TrigDetCalibConf import ScoutingStreamWriter
            dsAlg  = ScoutingStreamWriter("MuonCosmicDataScouting")
            # this should go to a dedicated place for datascouting configuration
            dsAlg.CollectionTypeName = ['xAOD::TrackParticleContainer_v1#InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID',
                                        'xAOD::TrackParticleAuxContainer_v1#InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFIDAux',
                                        'xAOD::VertexContainer_v1#xPrimVx',
                                        'xAOD::VertexAuxContainer_v1#xPrimVxAux'
                                        ] 
            inputTE = 'EF_efid'
            outputTE = "EF_efid_ds"                
            self.EFsequenceList += [[ [inputTE], [dsAlg], outputTE ] ]
            self.EFsignatureList +=  [ [['EF_efid_ds']*self.mult] ]
        
            
        
        self.TErenamingDict = {
            'L2_ih':    "L2_Cosmic_"+newchainName+"_AllTEDummy",
            'EF_efid':  "EF_TrigEFIDInsideOut_CosmicsN",
            }

        if "ds" in self.chainPart['addInfo']:
            self.TErenamingDict.update({'EF_efid_ds'  : mergeRemovingOverlap('EF_efid_',    self.chainName),})


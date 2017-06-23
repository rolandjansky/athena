# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for min bias chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)

logMinBiasDef = logging.getLogger("TriggerMenu.minbias.MinBiasDef")

from AthenaCommon.SystemOfUnits import GeV

from TriggerMenu.menu.HltConfig import L2EFChainDef,mergeRemovingOverlap

from TrigT2MinBias.TrigT2MinBiasConfig import (MbMbtsHypo,L2MbMbtsFex,L2MbSpFex,L2MbSpFex_SCTNoiseSup,L2MbSpFex_ncb,
                                               L2MbSpHypo_blayer,L2MbSpHypo_veto,L2MbSpFex_noPix,L2MbSpMhNoPixHypo_hip,
                                               L2MbSpFex_BLayer,L2MbSpHypo_ncb,L2MbSpHypo,L2MbSpHypo_PT,
                                               L2MbMbtsHypo_PT,L2MbZdcFex_LG,L2MbZdcHypo_PT,L2MbZdcFex_HG,trigT2MinBiasProperties)
from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence
#fexes.efid = TrigEFIDSequence("minBias","minBias","InsideOut").getSequence()
#fexes.efid2P = TrigEFIDSequence("minBias2P","minBias2","InsideOutLowPt").getSequence()

efiddataprep = TrigEFIDSequence("minBias","minBias","DataPrep").getSequence()
efid = TrigEFIDSequence("minBias","minBias","InsideOut").getSequence()
efid_heavyIon = TrigEFIDSequence("heavyIonFS","heavyIonFS","InsideOut").getSequence()
efid2P = TrigEFIDSequence("minBias2P","minBias2","InsideOutLowPt").getSequence()

from TrigMinBias.TrigMinBiasConfig import (EFMbTrkFex,EFMbTrkHypoExclusiveLoose,EFMbTrkHypoExclusiveTight,EFMbTrkHypo,
                                           EFMbVxFex,MbVxHypo,MbTrkHypo)

from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo as DummyRoI
from TrigGenericAlgs.TrigGenericAlgsConf import PrescaleAlgo
dummyRoI=DummyRoI(name='MinBiasDummyRoI', createRoIDescriptors = True, NumberOfOutputTEs=1)
terminateAlgo = PrescaleAlgo('terminateAlgo')

# for HI
from TrigHIHypo.TrigHIHypoConfig import HIEFTrackHypo_AtLeastOneTrack
atLeastOneTrack = HIEFTrackHypo_AtLeastOneTrack(name='HIEFTrackHypo_AtLeastOneTrack')

from TrigHIHypo.TrigHIHypoConfig import HIL2VtxMultHypo
#hypos.update(hi_hypos)

#L2 pileup suppression
from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_FullScan_ZF_OnlyA  #TrigL2SiTrackFinder_FullScanA_ZF_OnlyA

theL2PileupSup = TrigL2SiTrackFinder_FullScan_ZF_OnlyA()

###########################################################################
#  All min bias
###########################################################################
class L2EFChain_MB(L2EFChainDef):
    
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
        #self.mult = int(self.chainPart['multiplicity'])
        self.chainName = chainDict['chainName']
        self.chainPartName = self.chainPart['chainPartName']
        #self.chainPartNameNoMult = self.chainPartName[1:] if self.mult > 1 else self.chainPartName
        
        self.L2InputTE = self.chainPartL1Item or self.chainL1Item
        # cut of L1_, _EMPTY,..., & multiplicity
        if self.L2InputTE is not '':
            self.L2InputTE = self.L2InputTE.replace("L1_","")
            self.L2InputTE = self.L2InputTE.split("_")[0]
            self.L2InputTE = self.L2InputTE[1:] if self.L2InputTE[0].isdigit() else self.L2InputTE

        if "sptrk" in self.chainPart['recoAlg']:
            self.setup_mb_sptrk()
        elif "sp" in self.chainPart['recoAlg']:
            self.setup_mb_sptrk()
        elif "mbts" in self.chainPart['recoAlg']:
            self.setup_mb_mbts()
        elif "perf" in self.chainPart['recoAlg']:
            self.setup_mb_perf()
        elif "idperf" in self.chainPart['recoAlg']:
            self.setup_mb_idperf()
        elif "zdcperf" in self.chainPart['recoAlg']:
            self.setup_mb_zdcperf()
        elif "hmt" in self.chainPart['recoAlg']:
            self.setup_mb_hmt()
        elif "hmtperf" in self.chainPart['recoAlg']:
            self.setup_mb_hmtperf()
        elif "noalg" in self.chainPart['recoAlg']:
            self.setup_mb_noalg()


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
    def setup_mb_sptrk(self):
        doHeavyIon=False
        if 'ion' in self.chainPart['extra']:
            doHeavyIon=True

        doSpNcb=False
        if 'ncb' in  self.chainPart['extra']:
            doSpNcb=True

        doBLayer=False
        if 'blayer' in  self.chainPart['extra']:
            doBLayer=True

        doVetoSp=False
        if 'vetosp' in self.chainPart['extra']:
            doVetoSp=True

        doSptrk=False
        if "sptrk" in self.chainPart['recoAlg']: #do EFID
            doSptrk=True

        doMbtsVeto=False
        if "vetombts2in" in self.chainPart['extra'] or "vetospmbts2in" in self.chainPart['extra']: #do EFID
            doMbtsVeto=True
            theL2MbtsFex=L2MbMbtsFex
            theL2MbtsHypo=MbMbtsHypo("L2MbMbtsHypo_1_1_inn_veto")

        if "vetombts1side2in" in self.chainPart['extra']: #do EFID
            doMbtsVeto=True
            theL2MbtsFex=L2MbMbtsFex
            theL2MbtsHypo=MbMbtsHypo("L2MbMbtsHypo_1_1_inn_one_side_veto")

        ########## L2 algos ##################
        #if "sptrk" or "sp" in self.chainPart['recoAlg']:
        if "noisesup" in self.chainPart['extra']:
            chainSuffix = "sptrk_noisesup"
            theL2Fex  = L2MbSpFex_SCTNoiseSup
        elif doSpNcb:
            # spacepoint halo trigger is SCT only
            theL2Fex  = L2MbSpFex_ncb
            chainSuffix = "sp_ncb"
        elif doBLayer:
            # pix occupancy trigger only counts the innermost pix layer
            theL2Fex = L2MbSpFex_BLayer
            chainSuffix = "sp_blayer"
        else:
            theL2Fex  = L2MbSpFex
            if doSptrk:
                chainSuffix = "sptrk"
            elif doVetoSp:
                chainSuffix = "sp_vetosp"
            else:
                chainSuffix = "sp"
        
        if doMbtsVeto:
            if "vetombts2in" in self.chainPart['extra']:
                chainSuffix = chainSuffix+"_vetombts2in"
            if "vetombts1side2in" in self.chainPart['extra']:
                chainSuffix = chainSuffix+"_vetombts1side2in"

        if doMbtsVeto and doVetoSp: # this will never be done w tracks
            chainSuffix = "sp_vetospmbts2in"

        if doSpNcb:
            theL2Hypo = L2MbSpHypo_ncb
        elif doBLayer:
            theL2Hypo = L2MbSpHypo_blayer
        elif doVetoSp:
            theL2Hypo = L2MbSpHypo_veto
        else:
            theL2Hypo = L2MbSpHypo

        ########## EF algos ##################
        #if "sptrk" in self.chainPart['recoAlg']:
        if "costr" in self.chainPart['trkInfo']:
            chainSuffix = chainSuffix+"_costr"
                
            from InDetTrigRecExample.EFInDetConfig import TrigEFIDInsideOut_CosmicsN
            efid_costr=TrigEFIDInsideOut_CosmicsN()
            theEFFex1 = efid_costr.getSequence()
            from TrigMinBias.TrigMinBiasConfig import MbTrkFex_1, MbTrkHypo_1
            theEFFex2 =  MbTrkFex_1("MbTrkFex_"+chainSuffix)
            theEFFex2.InputTrackContainerName = "InDetTrigTrackSlimmerIOTRT_CosmicsN_EFID"
            theEFHypo = MbTrkHypo_1("MbTrkHypo_"+chainSuffix)
            theEFHypo.AcceptAll_EF=False
            theEFHypo.Required_ntrks=1
            theEFHypo.Max_z0=1000.0
        else:
            if not doHeavyIon:
                theEFFex1 =  efid
            else:
                theEFFex1 =  efid_heavyIon

            theEFFex2 =  EFMbTrkFex
            efhypo = self.chainPart['hypoEFInfo']
            if efhypo:
                if "pt" in self.chainPart['hypoEFInfo']:
                    efth=efhypo.lstrip('pt')
                    threshold=float(efth)
                    theEFHypo = MbTrkHypo('EFMbTrkHypo_pt%d'% threshold)
                    theEFHypo.Min_pt = threshold
                    theEFHypo.Max_z0 = 401.
                    chainSuffix = chainSuffix+'_pt'+efth
                elif "trk" in self.chainPart['hypoEFInfo']:
                    efth=efhypo.lstrip('trk')
                    theEFHypo = MbTrkHypo('EFMbTrkHypo_trk%i'% int(efth))
                    theEFHypo.Required_ntrks = int(efth)
                    theEFHypo.Min_pt = 0.200
                    theEFHypo.Max_z0 = 401.
                    chainSuffix = chainSuffix+'_trk'+efth
            elif 'exclusiveloose' in self.chainPart['extra']:
                efth=0.200 #default
                theEFHypo =  EFMbTrkHypoExclusiveLoose
                chainSuffix = chainSuffix+"_exclusiveloose"
            elif 'exclusivetight' in self.chainPart['extra']:
                efth=0.200 #default
                theEFHypo =  EFMbTrkHypoExclusiveTight
                chainSuffix = chainSuffix+"_exclusivetight"
            else:
                efth=0.200 #default
                theEFHypo =  EFMbTrkHypo

            #print "igb: ", efhypo
            #print "igb: ", theEFHypo.Min_pt

        ########### Sequence List ##############

        self.L2sequenceList += [["",
                                 [dummyRoI],
                                 'L2_mb_dummy']] 

        if doMbtsVeto:
            self.L2sequenceList += [['L2_mb_dummy',
                                     [theL2MbtsFex, theL2MbtsHypo],
                                     'L2_mb_mbtsveto']] 
            self.L2sequenceList += [['L2_mb_mbtsveto',
                                     efiddataprep,
                                     'L2_mb_iddataprep']] 
        else:
            self.L2sequenceList += [['L2_mb_dummy',
                                     efiddataprep,
                                     'L2_mb_iddataprep']] 

        self.L2sequenceList += [[['L2_mb_iddataprep'],
                                 [theL2Fex, theL2Hypo],
                                 'L2_mb_step1']]

        if doSptrk:
            self.EFsequenceList += [[['L2_mb_step1'],
                                     theEFFex1+[theEFFex2, theEFHypo],
                                     'EF_mb_step1']]
            if 'peb' in self.chainPart['addInfo']:
                from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter
                ALFASubDetListWriter = TrigSubDetListWriter("ALFASubDetListWriter")
                ALFASubDetListWriter.SubdetId = ['TDAQ_HLT','TDAQ_CTP','InnerDetector','DBM','FORWARD_ALPHA','FORWARD_LUCID','FORWARD_ZDC','FORWARD_BCM']
                ALFASubDetListWriter.MaxRoIsPerEvent=1
                self.EFsequenceList += [[['EF_mb_step1'],
                                         [ ALFASubDetListWriter ],
                                         'EF_mb_step2']]

        ########### Signatures ###########
        
        self.L2signatureList += [ [['L2_mb_dummy']] ]
        if doMbtsVeto:
            self.L2signatureList += [ [['L2_mb_mbtsveto']] ]
        self.L2signatureList += [ [['L2_mb_iddataprep']] ]
        self.L2signatureList += [ [['L2_mb_step1']] ]
        if doSptrk:
            self.EFsignatureList += [ [['EF_mb_step1']] ]
            if 'peb' in self.chainPart['addInfo']:
                self.EFsignatureList += [ [['EF_mb_step2']] ]

        self.TErenamingDict = {
            'L2_mb_dummy': mergeRemovingOverlap('L2_dummy_', chainSuffix),        
            'L2_mb_mbtsveto': mergeRemovingOverlap('L2_mbtsveto_', chainSuffix),        
            'L2_mb_iddataprep': mergeRemovingOverlap('L2_iddataprep_', chainSuffix),
            'L2_mb_step1': mergeRemovingOverlap('L2_', chainSuffix),
            'EF_mb_step1': mergeRemovingOverlap('EF_', chainSuffix),
            }

        if 'peb' in self.chainPart['addInfo']:
            self.TErenamingDict ['EF_mb_step2'] = mergeRemovingOverlap('EF_', chainSuffix+'_peb')

###########################
    def setup_mb_idperf(self):
        doHeavyIon=False
        if 'ion' in self.chainPart['extra']:
            doHeavyIon=True

        ########## EF algos ##################
        if "idperf" in self.chainPart['recoAlg']:
            chainSuffix = "idperf"
            if not doHeavyIon:
                theEFFex1 =  efid
            else:
                theEFFex1 =  efid_heavyIon

        ########### Sequence List ##############

        self.L2sequenceList += [["",
                                 [dummyRoI],
                                 'L2_mb_step0']] 

        self.EFsequenceList += [[['L2_mb_step0'],
                                  theEFFex1,
                                 'EF_mb_step1']]

        ########### Signatures ###########
        self.L2signatureList += [ [['L2_mb_step0']] ]
        self.EFsignatureList += [ [['EF_mb_step1']] ]

        self.TErenamingDict = {
            'L2_mb_step0': mergeRemovingOverlap('L2_', chainSuffix),
            'EF_mb_step1': mergeRemovingOverlap('EF_', chainSuffix),
                }

###########################
    def setup_mb_perf(self):

        ########## L2 algos ##################
        if "perf" in self.chainPart['recoAlg']:
            chainSuffix = "perf"
            theL2Fex1  = L2MbSpFex_noPix
            theL2Hypo1 = L2MbSpHypo_PT
            theL2Fex2  = L2MbMbtsFex
            theL2Hypo2 = L2MbMbtsHypo_PT

        ########## EF algos ##################

        ########### Sequence List ##############

        self.L2sequenceList += [["",
                                 [dummyRoI],
                                 'L2_mb_dummy']] 

        self.L2sequenceList += [['L2_mb_dummy',
                                 efiddataprep,
                                 'L2_mb_iddataprep']] 
        
        self.L2sequenceList += [[['L2_mb_iddataprep'],
                                 [theL2Fex1, theL2Hypo1],
                                 'L2_mb_step1']]

        self.L2sequenceList += [[['L2_mb_step1'],
                                 [theL2Fex2, theL2Hypo2],
                                 'L2_mb_step2']]

        ########### Signatures ###########
        self.L2signatureList += [ [['L2_mb_dummy']] ]
        self.L2signatureList += [ [['L2_mb_iddataprep']] ]
        self.L2signatureList += [ [['L2_mb_step1']] ]
        self.L2signatureList += [ [['L2_mb_step2']] ]


        self.TErenamingDict = {
            'L2_mb_dummy': mergeRemovingOverlap('L2_dummy', chainSuffix),     
            'L2_mb_iddataprep': mergeRemovingOverlap('L2_iddataprep_', chainSuffix),
            'L2_mb_step1': mergeRemovingOverlap('L2_sp_', chainSuffix),
            'L2_mb_step2': mergeRemovingOverlap('L2_mbts_', chainSuffix),
            }

###########################
    def setup_mb_zdcperf(self):

        ########## L2 algos ##################
        if "zdcperf" in self.chainPart['recoAlg']:
            if "lg" in self.chainPart['ZDCinfo']:
                chainSuffix = "lg_zdcperf"
                theL2Fex1  = L2MbZdcFex_LG
                theL2Hypo1 = L2MbZdcHypo_PT
            elif "hg" in self.chainPart['ZDCinfo']:
                chainSuffix = "hg_zdcperf"
                theL2Fex1  = L2MbZdcFex_HG
                theL2Hypo1 = L2MbZdcHypo_PT

        ########## EF algos ##################

        ########### Sequence List ##############

        self.L2sequenceList += [["",
                                 [dummyRoI],
                                 'L2_mb_step0']] 
        
        self.L2sequenceList += [[['L2_mb_step0'],
                                 [theL2Fex1, theL2Hypo1],
                                 'L2_mb_step1']]

        ########### Signatures ###########
        self.L2signatureList += [ [['L2_mb_step0']] ]
        self.L2signatureList += [ [['L2_mb_step1']] ]
 

        self.TErenamingDict = {
            'L2_mb_step0': mergeRemovingOverlap('L2_', 'dummyroi_'+chainSuffix),
            'L2_mb_step1': mergeRemovingOverlap('L2_', 'zdc_'+chainSuffix),
            }

###########################
    def setup_mb_mbts(self):

        theL2Fex  = L2MbMbtsFex

        doMbtsVeto=False
        if "vetombts2in" in self.chainPart['extra']: #do EFID
            doMbtsVeto=True
            theL2MbtsVetoHypo=MbMbtsHypo("L2MbMbtsHypo_1_1_inn_veto")

        if "vetombts1side2in" in self.chainPart['extra']: #do EFID
            doMbtsVeto=True
            theL2MbtsVetoHypo=MbMbtsHypo("L2MbMbtsHypo_1_1_inn_one_side_veto")

        ########## L2 algos ##################
        if "mbts" in self.chainPart['recoAlg']:
            l2hypo = self.chainName
            #print 'igb before:', l2hypo
            l2HypoCut_temp=l2hypo.replace('mb_mbts_L1MBTS_','')
            #print 'igb after:', l2HypoCut_temp
            if  len(l2HypoCut_temp) > 3:
                if len(l2HypoCut_temp.replace('_UNPAIRED_ISO','')) < 4:
                    l2HypoCut=l2HypoCut_temp.replace('_UNPAIRED_ISO','')
                if len(l2HypoCut_temp.replace('_EMPTY','')) < 4:
                    l2HypoCut=l2HypoCut_temp.replace('_EMPTY','')
            else:
                l2HypoCut=l2HypoCut_temp
            
            #print 'igb end:', l2HypoCut
            #print 'igb:', l2hypo
            #print 'igb:', l2HypoCut
            #print 'igb', len(l2HypoCut)

            chainSuffix = "mbts"
            theL2Fex  = L2MbMbtsFex
            
            if not doMbtsVeto:
                theL2Hypo = MbMbtsHypo('L2MbMbtsHypo_'+l2HypoCut)
                theL2Hypo.AcceptAll = False
                theL2Hypo.TimeCut = trigT2MinBiasProperties.mbtsTimeCut()

                if(len(l2HypoCut) == 1):
                    theL2Hypo.MbtsCounters = int(l2HypoCut)
                    theL2Hypo.Coincidence = False
                elif(len(l2HypoCut) == 3):
                    L2th = l2HypoCut.split("_")[0]
                 #print 'igb: threshold: ', L2th
                    theL2Hypo.MbtsCounters = int(L2th)
                    theL2Hypo.Coincidence = True
                else:
                    logMinBiasDef.error("Something weird in the setup_mb_mbts(), please check")
            else:
                theL2Hypo = theL2MbtsVetoHypo
                if "vetombts2in" in self.chainPart['extra']:
                    chainSuffix = chainSuffix+"_vetombts2in"
                if "vetombts1side2in" in self.chainPart['extra']:
                    chainSuffix = chainSuffix+"_vetombts1side2in"
                
        ########## EF algos ##################

        ########theL2MbtsVetoHypo### Sequence List ##############

        self.L2sequenceList += [["",
                                 [dummyRoI],
                                 'L2_mb_step0']] 
        if not doMbtsVeto:
            self.L2sequenceList += [[['L2_mb_step0'],
                                     [theL2Fex, theL2Hypo],
                                     'L2_mb_step1']]
        else:
            self.L2sequenceList += [[['L2_mb_step0'],
                                     [theL2Fex, theL2Hypo],
                                     'L2_mb_mbtsveto']]

        ########### Signatures ###########
        if not doMbtsVeto:
            self.L2signatureList += [ [['L2_mb_step1']] ]
            self.TErenamingDict = {
                'L2_mb_step1': mergeRemovingOverlap('L2_', ''+chainSuffix+'_'+l2HypoCut),
                }
        else:
            self.L2signatureList += [ [['L2_mb_mbtsveto']] ]
            self.TErenamingDict = {
                'L2_mb_mbtsveto': mergeRemovingOverlap('L2_mbtsveto_', ''+chainSuffix),
            }

########################### high multiplicity triggers
    def setup_mb_hmt(self):
        l2hypo1 = self.chainPart['hypoL2Info']
        l2hypo2 = self.chainPart['pileupInfo']
        efhypo1 = self.chainPart['hypoEFsumEtInfo']
        efhypo2 = self.chainPart['hypoEFInfo']
        l2th1=l2hypo1.lstrip('sp')
        l2th2=l2hypo2.lstrip('pusup')
        efth1=efhypo1.lstrip('sumet')
        efth2=efhypo2.lstrip('trk')
        
        # no pileup nor sumEt cuts by default
        doPusup=False
        doSumEt=False

        #print 'igb - l2th1:', l2th1
        #print 'igb - l2th2:', l2th2
        #print 'igb - efth1:', efth1
        #print 'igb - efth2:', efth2
        
        ########## L2 algos ##################
        if "hmt" in self.chainPart['recoAlg']:
            chainSuffix = "hmt"
            chainSuffixL2 = "hmt"
            chainSuffixEF = "hmt"
            theL2Fex1  = L2MbSpFex_noPix
            theL2Hypo1 = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_"+l2th1, float(l2th1))
            if "pusup" in self.chainPart['pileupInfo']:
                doPusup=True
                chainSuffixL2=l2hypo2+'_'+chainSuffixL2
                theL2Fex2  = theL2PileupSup
                theL2Hypo2 = HIL2VtxMultHypo("HIL2VtxMultHyp_"+l2th2, int(l2th2))
        ########## EF algos ##################
            if "sumet" in self.chainPart['hypoEFsumEtInfo']:
                doSumEt=True
                chainSuffixEF=efhypo1+'_'+chainSuffixEF
                #sum Et fex
                from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex_2sidednoiseSupp
                theEFMETFex = EFMissingET_Fex_2sidednoiseSupp()
                # sum Et hypo
                from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoTE
                threshold=float(efth1)
                theEFMETHypo = EFMetHypoTE('EFMetHypo_te%d'% threshold,ef_thr=threshold*GeV)
                
            theEFFex1 =  efid
            theEFFex2 =  EFMbTrkFex
            theEFFex3 =  EFMbVxFex

            theEFHypo =  MbVxHypo("EFMbVxHypoMh_hip_"+efth2)
            theEFHypo.AcceptAll_EF = False
            theEFHypo.RejectPileup = False
            theEFHypo.Required_ntrks = int(efth2)

        ########### Sequence List ##############

        self.L2sequenceList += [["",
                                 [dummyRoI],
                                 'L2_mb_dummy']] 

        self.L2sequenceList += [['L2_mb_dummy',
                                 efiddataprep,
                                 'L2_mb_iddataprep']] 

        self.L2sequenceList += [[['L2_mb_iddataprep'],
                                 [theL2Fex1, theL2Hypo1],
                                 'L2_mb_step1']]
        
        if doPusup:
            self.L2sequenceList += [[['L2_mb_step1'],
                                     [theL2Fex2, theL2Hypo2],
                                     'L2_mb_step2']]
            
            if doSumEt:
                self.EFsequenceList += [[['L2_mb_step2'],
                                         [theEFMETFex, theEFMETHypo],
                                         'EF_mb_step1']]
                self.EFsequenceList += [[['EF_mb_step1'],
                                         theEFFex1+[theEFFex2, theEFFex3, theEFHypo],
                                         'EF_mb_step2']]
            else:
                self.EFsequenceList += [[['L2_mb_step2'],
                                         theEFFex1+[theEFFex2, theEFFex3, theEFHypo],
                                         'EF_mb_step1']]
        else:
             if doSumEt:
                 self.EFsequenceList += [[['L2_mb_step1'],
                                         [theEFMETFex, theEFMETHypo],
                                         'EF_mb_step1']]
                 self.EFsequenceList += [[['EF_mb_step1'],
                                         theEFFex1+[theEFFex2, theEFFex3, theEFHypo],
                                         'EF_mb_step2']]
             else:
                 self.EFsequenceList += [[['L2_mb_step1'],
                                         theEFFex1+[theEFFex2, theEFFex3, theEFHypo],
                                         'EF_mb_step1']]

        ########### Signatures ###########
        self.L2signatureList += [ [['L2_mb_dummy']] ]
        self.L2signatureList += [ [['L2_mb_iddataprep']] ]
        self.L2signatureList += [ [['L2_mb_step1']] ]
        if doPusup:
            self.L2signatureList += [ [['L2_mb_step2']] ]
        self.EFsignatureList += [ [['EF_mb_step1']] ]
        if doSumEt:
            self.EFsignatureList += [ [['EF_mb_step2']] ]
            
        if not doSumEt:
            chainSuffixL2=efhypo2+'_'+chainSuffixL2
        
        self.TErenamingDict = {
            'L2_mb_dummy': mergeRemovingOverlap('L2_dummy_', chainSuffix),
            'L2_mb_iddataprep': mergeRemovingOverlap('L2_iddataprep_', l2hypo1+'_'+chainSuffix),
            'L2_mb_step1': mergeRemovingOverlap('L2_', l2hypo1+'_'+chainSuffix),
            'L2_mb_step2': mergeRemovingOverlap('L2_', l2hypo1+'_'+l2hypo2+'_'+chainSuffix),
            'EF_mb_step1': mergeRemovingOverlap('EF_', l2hypo1+'_'+efhypo1+'_'+chainSuffixL2),
            'EF_mb_step2': mergeRemovingOverlap('EF_', l2hypo1+'_'+efhypo2+'_'+chainSuffixEF),
            }

########################### supporting triggers for high multiplicity triggers
    def setup_mb_hmtperf(self):
        l2hypo1 = self.chainPart['hypoL2Info']
        l2th1=l2hypo1.lstrip('sp')

        #print 'igb - l2th1:', l2th1
        ########## L2 algos ##################
        if "hmtperf" in self.chainPart['recoAlg']:
            chainSuffix = "sp"+l2th1+"_hmtperf"

            theL2Fex1  = L2MbSpFex_noPix
            theL2Hypo1 = L2MbSpMhNoPixHypo_hip("L2MbSpMhNoPixHypo_hip_"+l2th1, float(l2th1))

            theL2Fex2  = theL2PileupSup
            theL2Hypo2 = HIL2VtxMultHypo("HIL2VtxMultHyp_PT")
            theL2Hypo2.AcceptAll = True

        ########## EF algos ##################
            #sum Et fex
            from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex_2sidednoiseSupp
            theEFMETFex = EFMissingET_Fex_2sidednoiseSupp()
            # sum Et hypo
            from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoTE
            theEFMETHypo = EFMetHypoTE('EFMetHypo_PT')
            theEFMETHypo.forceAccept=True
            
            #tracking and vertexing fexes
            theEFFex1 =  efid
            theEFFex2 =  EFMbTrkFex
            theEFFex3 =  EFMbVxFex

            theEFHypo =  MbVxHypo("EFMbVxHypoMh_hip_PT")
            theEFHypo.AcceptAll_EF = True

        ########### Sequence List ##############

        self.L2sequenceList += [["",
                                 [dummyRoI],
                                 'L2_mb_dummy']] 

        self.L2sequenceList += [['L2_mb_dummy',
                                 efiddataprep,
                                 'L2_mb_iddataprep']] 

        self.L2sequenceList += [[['L2_mb_iddataprep'],
                                 [theL2Fex1, theL2Hypo1],
                                 'L2_mb_step1']]
        
        self.L2sequenceList += [[['L2_mb_step1'],
                                 [theL2Fex2, theL2Hypo2],
                                 'L2_mb_step2']]
            
        self.EFsequenceList += [[['L2_mb_step2'],
                                 [theEFMETFex, theEFMETHypo],
                                 'EF_mb_step1']]
        self.EFsequenceList += [[['EF_mb_step1'],
                                 theEFFex1+[theEFFex2, theEFFex3, theEFHypo],
                                 'EF_mb_step2']]

        ########### Signatures ###########
        self.L2signatureList += [ [['L2_mb_dummy']] ]
        self.L2signatureList += [ [['L2_mb_iddataprep']] ]
        self.L2signatureList += [ [['L2_mb_step1']] ]
        self.L2signatureList += [ [['L2_mb_step2']] ]
        self.EFsignatureList += [ [['EF_mb_step1']] ]
        self.EFsignatureList += [ [['EF_mb_step2']] ]
                
        self.TErenamingDict = {
            'L2_mb_dummy': mergeRemovingOverlap('L2_dummy_', chainSuffix),
            'L2_mb_iddataprep': mergeRemovingOverlap('L2_iddataprep_', l2hypo1+'_'+chainSuffix),
            'L2_mb_step1': mergeRemovingOverlap('L2_', l2hypo1+'_'+chainSuffix),
            'L2_mb_step2': mergeRemovingOverlap('L2_pusup0','_'+chainSuffix),
            'EF_mb_step1': mergeRemovingOverlap('EF_sumet0','_'+chainSuffix),
            'EF_mb_step2': mergeRemovingOverlap('EF_trk0','_'+chainSuffix),
            }

    def setup_mb_noalg(self):
        #this function removes "mb_" from the chain name
        chainName=self.chainName
        shortName=chainName.strip('mb_')
        self.chainName       = shortName
        

#####################################################################


#if __name__ == '__main__':
#    triggerPythonConfig = TriggerPythonConfig('hlt.xml', None)
#    for m in Muons:
#        m.generateMenu(triggerPythonConfig)
#    triggerPythonConfig.writeConfigFiles()
    

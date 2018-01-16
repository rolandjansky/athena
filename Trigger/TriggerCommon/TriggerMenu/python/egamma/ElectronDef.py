# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Electron trigger slice signatures  """

__author__  = 'Moritz Backes & Catrin Bernius'
__version__=""
__doc__="Implementation of Electron Signature"
import collections

from AthenaCommon.Logging import logging

from TriggerMenu.menu.HltConfig import L2EFChainDef, mergeRemovingOverlap
from TrigHIHypo.UE import theUEMaker, theFSCellMaker, theElectronUEMonitoring
from TrigMultiVarHypo.TrigL2CaloRingerHypoConfig import TrigL2CaloRingerFexHypo_e_NoCut
from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_EtCut
from TrigEgammaHypo.TrigL2ElectronFexConfig import L2ElectronFex_Clean
from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyCopyAllTEAlgo
from TriggerMenu.commonUtils.makeCaloSequences import getFullScanCaloSequences
from TriggerJobOpts.TriggerFlags import TriggerFlags
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenu.egamma.ElectronDef")

##########################################################################################
#
# L2 & EF Chain configuration helper classes
#
##########################################################################################

class L2EFChain_e(L2EFChainDef):

    # Define frequently used instances here as class variables
    DummyMergerAlgo = PESA__DummyCopyAllTEAlgo("DummyMergerAlgo")
    fullScanSeqMap = getFullScanCaloSequences()
    def __init__(self, chainDict, seqObj):

        self.L2sequenceList   = []
        self.EFsequenceList   = []
        self.L2signatureList  = []
        self.EFsignatureList  = []
        self.TErenamingDict   = {}
    
        self.chainPart = chainDict['chainParts']
        self.seqObj = seqObj
        self.el_sequences = seqObj.get_sequences()

        self.chainL1Item = chainDict['L1item']        
        self.chainPartL1Item = self.chainPart['L1item']

        self.chainCounter = chainDict['chainCounter']       
        self.L2Name = 'L2_'+self.chainPart['chainPartName']
        self.EFName = 'EF_'+self.chainPart['chainPartName']
        self.mult = int(self.chainPart['multiplicity'])
        self.chainName = chainDict['chainName']
        self.chainPartName = self.chainPart['chainPartName']
        self.chainPartNameNoMult = self.chainPartName[1:] if self.mult > 1 else self.chainPartName
        self.chainPartNameNoMult += "_"+self.chainL1Item

        
        self.L2InputTE = self.chainPartL1Item or self.chainL1Item
        # cut of L1_, _EMPTY,..., & multiplicity
        self.L2InputTE = self.L2InputTE.replace("L1_","")
        if 'AFP' in self.L2InputTE:
           self.L2InputTE = self.L2InputTE.replace("AFP_C_","")
        self.L2InputTE = self.L2InputTE.split("_")[0]
        self.L2InputTE = self.L2InputTE[1:] if self.L2InputTE[0].isdigit() else self.L2InputTE

        self.use_v7=False
        self._ringer_selection=False
        self.setRingerSelection()

        #if TriggerFlags.run2Config() == '2017':
        if '_v7' in TriggerFlags.triggerMenuSetup():
            self.use_v7=True
        
        self.doCaloIsolation=False
        for item in self.chainPart['isoInfo']:
            if 'icalo' in item:
                self.doCaloIsolation=True
        # eXXvh_ID type chains:
        if self.use_v7:
            self.setup_electron()
        elif self.chainPart['trkInfo']=='idperf': #True:# self.chainPart['IDinfo'] \
            if 'ion' in self.chainPart['extra']:
                self.setup_eXXvh_idperf_heavyIon()
            else:
                self.setup_eXXvh_idperf()
        elif 'ringer' in self.chainPart['addInfo']:
           self.setup_eXXvh_ID_ringer()
        else:
            if 'ion' in self.chainPart['extra']:
                self.setup_eXXvh_ID_run2_heavyIon()
            else:
                self.setup_eXXvh_ID_run2()

        L2EFChainDef.__init__(self, 
                              self.chainName, 
                              self.L2Name, 
                              self.chainCounter, 
                              self.chainL1Item, 
                              self.EFName, 
                              self.chainCounter, 
                              self.L2InputTE)

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
    
    def setRingerSelection(self):
        '''
        determine whether to apply the ringer preselection
        can be set globally via TriggerFlag
        or by chain type, e.g. idperf 
        '''
        thr = self.chainPart['threshold']
        log.debug("Apply ringer %s",self._ringer_selection)

        if 'noringer' in self.chainPart['addInfo']:
          log.debug('No Ringer selection used. Disable the ringer for this chain.')
          self._ringer_selection=False
        elif TriggerFlags.EgammaSlice.doRinger:
            log.debug('Ringer selection applied for all chains above 15 GeV %s',thr)
            if 'merged' in self.chainPart['IDinfo']:
                self._ringer_selection=False
            elif 'bloose' in self.chainPart['IDinfo']:
                self._ringer_selection=False
            elif float(thr)>15.0:
                self._ringer_selection=True
            else:
                self._ringer_selection=False
        elif 'ringer' in self.chainPart['addInfo']:
            self._ringer_selection = True
        elif self.chainPart['trkInfo']=='idperf': 
            if float(thr)>15.0:
                self._ringer_selection=True
            else:
                self._ringer_selection=False
        
        log.debug("Apply ringer is set %s",self._ringer_selection)


    def setupFromDict(self, seq_te_dict, seq_dict=None):
        ''' 
        Dictionary completely defines steps, algos, sequences, etc..
        pass a dictionary with sequence key values
        and tuple with te_out and alias for TE renaming
        i.e. seq_te_dict['fastringer']=('L2_e_step1','cl')
        '''
        
        # Use default sequence map if not passed modified map
        if seq_dict is None:
            seq_dict=self.el_sequences

        log.debug('%s'%seq_dict)
        te_in=self.L2InputTE
        for step in seq_te_dict:
            level=seq_te_dict[step][0].split('_')[0]
            te_out=seq_te_dict[step][0]
            alias=seq_te_dict[step][-1]
            algos=seq_dict[step]
            log.debug('%s %s %s %s'%(level,te_in,algos,te_out))
            if level == 'L2':
                self.L2sequenceList += [[te_in,algos,te_out]]                     
                self.L2signatureList += [ [[te_out]*self.mult] ]
            elif level == 'EF':
                self.EFsequenceList += [[te_in,algos,te_out]]                     
                self.EFsignatureList += [ [[te_out]*self.mult] ]
            else:
                log.error('Sequence TE dictionary incorrect')
            te_in=[te_out]
            self.TErenamingDict[te_out] = mergeRemovingOverlap(level+'_',self.chainPartNameNoMult+alias)

############################### DEFINE GROUPS OF CHAINS HERE ##############################

    def setup_electron(self):
        ''' 
        MC_pp_v7 onwards configuration method for electron chains 
        TrigEMCluster and Ringer reconstruction (same as photons)
        Fast Calo-only rejection (w/ ringer or cut-based)
        Full track reconstruction
        Fast electron reconstruction (optimize pt cut for low/high pt chains)
            Mark tracks which match to cluster
        Precise Calo
        MVA calibration + Et cut hypo on clusters
        Offline reco + hypo
        
        When moving to ringer default, need to make low et chains use cut-based
        Also the L2ElectronFex will have low/high et 
        To preserve backward compt, need to do all this here :(
        '''
        
        # define the ordered dictionary for sequences and TEs
        
        thr = self.chainPart['threshold']
        name = str(self.chainPart['threshold'])
        name = name.split('.')[0]
        dofastrecseq = TriggerFlags.EgammaSlice.doFastElectronFex
        if 'perf' in self.chainPartName: 
            dofastrecseq=False
        log.debug('setup_electron %s, apply ringer %s for %s',self.chainName,self._ringer_selection,thr )
        # Ringer chains not tuned for low-et
        # use standard hypo
        fastcalohypo=self.el_sequences['fastcalohypo']
        ringerfex,ringerhypo = TrigL2CaloRingerFexHypo_e_NoCut(thr)
        # To preserve backward compatibility just remove hypo from calibration step
        # Replace by Et cut (marks the clusters as passing)
        # Essentially breaks the class design :(
        # but 
        algo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_e"+name+"_EtCut",thr)
        fastrec_algo = L2ElectronFex_Clean()
        fastrec_hypo = self.el_sequences['fastrec'][-1]
        precisecalocalib =  self.el_sequences['precisecalocalib']
        precisecalocalib.pop()
        precisecalocalib.extend([algo])
        
        seq_dict = self.el_sequences
        seq_dict['precisecalocalib']=precisecalocalib

        if not self._ringer_selection:
            seq_dict['fastringerhypo'] = [ringerfex,ringerhypo]
            seq_dict['fastcalorec'].extend(fastcalohypo)
        
        seq_te_dict = collections.OrderedDict()
        
        # Here we revert to the traditional sequence
        # But apply selection at L2Electron
        if dofastrecseq:
            seq_dict['fastrec']=[fastrec_algo,fastrec_hypo]
            log.debug('FastRec sequence %s'%seq_dict['fastrec'])
            # remove track hypo from precisetrack step
            precisetrack = self.el_sequences['precisetrack']
            precisetrack.pop()
            seq_dict['precisetrack']=precisetrack
            seq_te_dict['fastcalorec']=('L2_e_step1','cl')
            seq_te_dict['fastringerhypo']=('L2_e_step2','clhypo')
            seq_te_dict['fasttrack']=('L2_e_step3','ftf')
            seq_te_dict['fastrec']=('L2_e_step4','')
            seq_te_dict['precisecalo']=('EF_e_step1','calo')
            seq_te_dict['precisecalocalib']=('EF_e_step2','calocalib')    
            seq_te_dict['precisetrack']=('EF_e_step3','idtrig') 
            if not self.doCaloIsolation:
                seq_te_dict['preciserec']=('EF_e_step4','')
        else:    
            seq_te_dict['fastcalorec']=('L2_e_step1','cl')
            seq_te_dict['fastringerhypo']=('L2_e_step2','clhypo')
            seq_te_dict['trackrec']=('L2_e_step3','trk')
            seq_te_dict['fastrec']=('L2_e_step4','') 
            seq_te_dict['precisecalo']=('EF_e_step1','cl')
            seq_te_dict['precisecalocalib']=('EF_e_step2','calocalib')    
            if not self.doCaloIsolation:
                seq_te_dict['preciserec']=('EF_e_step3','')

        self.setupFromDict(seq_te_dict,seq_dict)
        
        if self.doCaloIsolation:
            # full scan topo and merging w/ RoI TEs
            te_in=''
            if dofastrecseq:
                pos=3
                for step in self.fullScanSeqMap:
                    self.EFsequenceList.insert(pos,[[te_in],self.fullScanSeqMap[step],step])
                    self.EFsignatureList.insert(pos,[[step]] )
                    te_in=step
                    pos+=1
                
                self.EFsequenceList.insert(pos,[ ['EF_e_step3',te_in],[self.DummyMergerAlgo],'EF_eCache_step3'])
                self.EFsignatureList.insert(pos,[['EF_eCache_step3']])
                self.EFsequenceList.insert(pos+1,[['EF_eCache_step3'],self.el_sequences['preciserec'],'EF_e_step4'])
                self.EFsignatureList.insert(pos+1,[['EF_e_step4']*self.mult])
            else:
                pos=2
                for step in self.fullScanSeqMap:
                    self.EFsequenceList.insert(pos,[[te_in],self.fullScanSeqMap[step],step])
                    self.EFsignatureList.insert(pos,[[step]] )
                    te_in=step
                    pos+=1
                
                self.EFsequenceList.insert(pos,[ ['EF_e_step2',te_in],[self.DummyMergerAlgo],'EF_eCache_step2'])
                self.EFsignatureList.insert(pos,[['EF_eCache_step2']])
                self.EFsequenceList.insert(pos+1,[['EF_eCache_step2'],self.el_sequences['preciserec'],'EF_e_step3'])
                self.EFsignatureList.insert(pos+1,[['EF_e_step3']*self.mult])

            ########### TE renaming ###########

            self.TErenamingDict['EF_full']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fs')
            self.TErenamingDict['EF_full_cell']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fscalocell')
            self.TErenamingDict['EF_FSTopoClusters']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fscalotopo')
            self.TErenamingDict['EF_FSTopoClustersED']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fscalotopoed')
            
            if dofastrecseq:
                self.TErenamingDict['EF_eCache_step3']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_ED')
                self.TErenamingDict['EF_e_step4']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult)
            else:
                self.TErenamingDict['EF_eCache_step2']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_ED')
                self.TErenamingDict['EF_e_step3']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult)
    
    def setup_eXXvh_idperf(self):

        seq_te_dict = collections.OrderedDict()
        seq_te_dict['fastcalo']=('L2_e_step1','cl')
        seq_te_dict['precisecalo']=('EF_e_step1','cl')
        seq_te_dict['precisecalocalib']=('EF_e_step2','calocalib')    
        seq_te_dict['trackrec']=('EF_e_step3','trk') 
        seq_te_dict['preciserec']=('EF_e_step4','')

        self.setupFromDict(seq_te_dict)

    def setup_eXXvh_idperf_heavyIon(self):

        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        isoInfo = self.chainPart['isoInfo']
        run1 = self.chainPart['trkInfo']
       
        log.debug('setup_eXXvh_idperf_heavyIon')
        log.debug('threshold: %s',threshold)
        log.debug('isoInfo: %s',isoInfo)
        log.debug('IDinfo: %s',IDinfo)
        log.debug('trkInfo: %s',run1)
        
        # Here we can modify a bit the sequence map (to handle special triggers, e.g. HI)
        precisecalocalib =  self.el_sequences['precisecalocalib']
        precisecalocalib.insert(1,theElectronUEMonitoring) 
        
        self.L2sequenceList += [[self.L2InputTE,self.el_sequences['fastcalo'],'L2_e_step1']]                      
        self.EFsequenceList += [[['L2_e_step1'],[theFSCellMaker], 'EF_e_step1_fs']] 
        self.EFsequenceList += [[['EF_e_step1_fs'],[theUEMaker], 'EF_e_step1_ue']] 
        self.EFsequenceList += [[['L2_e_step1'],self.el_sequences['precisecalo'],'EF_e_step1']]                      
        self.EFsequenceList += [[['EF_e_step1'],precisecalocalib,'EF_e_step2']] 
        self.EFsequenceList += [[['EF_e_step2'],self.el_sequences['trackrec'],'EF_e_step3']] 
        self.EFsequenceList += [[['EF_e_step3'],self.el_sequences['preciserec'],'EF_e_step4']]                      

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_e_step1']*self.mult] ]
        self.L2signatureList += [ [['EF_e_step1_fs']] ]
        self.L2signatureList += [ [['EF_e_step1_ue']] ]
        self.EFsignatureList += [ [['EF_e_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step4']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'cl'),
            'EF_e_step1_fs': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'fs'),
            'EF_e_step1_ue': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'ue'),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'cl'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calocalib'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'trk'),
            'EF_e_step4': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }

    def setup_eXXvh_ID_ringer(self):
        
        seq_te_dict = collections.OrderedDict()
        seq_te_dict['fastringer']=('L2_e_step1','cl')
        seq_te_dict['precisecalo']=('EF_e_step1','calo')
        seq_te_dict['precisecalocalib']=('EF_e_step2','calocalib')    
        seq_te_dict['trackrec']=('EF_e_step3','id') 
        seq_te_dict['preciserec']=('EF_e_step4','')

        self.setupFromDict(seq_te_dict)

    def setup_eXXvh_ID_run2(self):
        
        seq_te_dict = collections.OrderedDict()
        seq_te_dict['fastcalo']=('L2_e_step1','cl')
        seq_te_dict['fasttrack']=('L2_e_step2','id')
        seq_te_dict['fastrec']=('L2_e_step3','')
        seq_te_dict['precisecalo']=('EF_e_step1','calo')
        seq_te_dict['precisecalocalib']=('EF_e_step2','calocalib')    
        seq_te_dict['precisetrack']=('EF_e_step3','id') 
        seq_te_dict['preciserec']=('EF_e_step4','')

        self.setupFromDict(seq_te_dict)

    def setup_eXXvh_ID_run2_heavyIon(self):
        
        precisecalocalib =  self.el_sequences['precisecalocalib']
        precisecalocalib.insert(1,theElectronUEMonitoring) 
       
        seq_dict = self.el_sequences
        seq_dict['precisecalocalib']=precisecalocalib

        seq_te_dict = collections.OrderedDict()
        seq_te_dict['fastcalo']=('L2_e_step1','cl')
        seq_te_dict['fasttrack']=('L2_e_step2','id')
        seq_te_dict['fastrec']=('L2_e_step3','')
        seq_te_dict['precisecalo']=('EF_e_step1','calo')
        seq_te_dict['precisecalocalib']=('EF_e_step2','calocalib')    
        seq_te_dict['precisetrack']=('EF_e_step3','id') 
        seq_te_dict['preciserec']=('EF_e_step4','')
        
        # First set the normal sequence 
        self.setupFromDict(seq_te_dict,seq_dict)
        
        # Now insert additional steps
        self.EFsequenceList.insert(0,[['L2_e_step3'],[theFSCellMaker], 'EF_e_step1_fs'])
        self.EFsequenceList.insert(1,[['EF_e_step1_fs'],[theUEMaker], 'EF_e_step1_ue'])
        self.EFsignatureList.insert(0, [['EF_e_step1_fs']] )
        self.EFsignatureList.insert(1, [['EF_e_step1_ue']] )

        self.TErenamingDict['EF_e_step1_fs']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'fs')
        self.TErenamingDict['EF_e_step1_ue']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'ue')
        

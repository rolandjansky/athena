# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerMenuPython.TriggerPythonConfig import *

from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloJet_Jet
from TrigJetHypo.TrigJetHypoConfig import L2JetHypo
from TrigJetHypo.TrigJetHypoConfig import EFJetHypo
from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_jet
from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_fullcalo
from TrigCaloRec.TrigCaloRecConfig import TrigCaloTowerMaker_jet
from TrigJetRec.TrigJetRecConfig import TrigJetRec_Cone
from TrigJetRec.TrigJetRecConfig import TrigJetRec_FastKt
from TriggerMenuPython.TriggerConfig_JetSeq_dict import jetSequencesDict
from TriggerMenuPython.JetSliceFlags import JetSliceFlags 

from AthenaCommon.SystemOfUnits import GeV



class JetSequence:
    def __init__(self, sig_id):

        configdic = jetSequencesDict[sig_id]     # get defaults params
        self.sig_id = sig_id  ## remember the sequence ID for the use in generate function
        self.doKt = False
        
        if ( (sig_id.find('fj')!= -1) or (sig_id.find('je')!=-1) ): # we distinguish between jets and forward jets...
          #from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo
          from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
          self.l2jetseq = [ DummyFEX('dummyfjet_L2_'+sig_id)]
          self.effjetDummy = DummyFEX('dummyfjet_EF_'+sig_id)
        else:
          self.l2jetseq = [ T2CaloJet_Jet(), L2JetHypo(name='L2JetHypo_'+sig_id, l2_thr=configdic['L2_Thr'])]
          self.trigCaloCellMakerJet = TrigCaloCellMaker_fullcalo()
          self.trigCaloTowerMakerJet = TrigCaloTowerMaker_jet()
          self.trigJetRecCone = TrigJetRec_Cone()
          self.efJetHypo = EFJetHypo(name='EFJetHypo_'+sig_id, ef_thr=configdic['EF_Thr'])
          if ( sig_id=='j5' and self.doKt ):
             self.trigJetRecKt = TrigJetRec_FastKt()            
             self.efJetHypoKt = EFJetHypo(name='EFJetHypoKt_'+sig_id, ef_thr=configdic['EF_Thr'])
        

    def generateMenu(self, triggerPythonConfig):
        from TriggerMenuPython.JetSliceFlags import JetSliceFlags ## slice flags are import so late to make sure they are never before used
        sig_id = self.sig_id
        configdic = jetSequencesDict[sig_id]     # get defaults params

        TE_L1 = configdic['TE_L1']

        if JetSliceFlags.doL2Calo():
        
           TE_L2 = triggerPythonConfig.addSequence(TE_L1, self.l2jetseq, "L2_"+sig_id)
           jetSequencesDict[sig_id]['TE_L2'] = TE_L2

        if JetSliceFlags.doEFCalo():
        
           if ( (sig_id.find('fj')!= -1) or (sig_id.find('je')!=-1) ): # we distinguish between jets and forward jets...
              jetSequencesDict[sig_id]['TE_EF'] = triggerPythonConfig.addSequence( TE_L2, [self.effjetDummy], "EF_"+sig_id)
           else:
              if ( sig_id=='j5' and self.doKt ):
                 jetSequencesDict[sig_id]['TE_EFCalo'] = triggerPythonConfig.addSequence( TE_L2, [self.trigCaloCellMakerJet, self.trigCaloTowerMakerJet], "EFCalo_"+sig_id)
                 jetSequencesDict[sig_id]['TE_EF'] = triggerPythonConfig.addSequence( jetSequencesDict[sig_id]['TE_EFCalo'], [self.trigJetRecCone, self.efJetHypo], "EF_"+sig_id)
                 jetSequencesDict[sig_id]['TE_EFKt'] = triggerPythonConfig.addSequence( jetSequencesDict[sig_id]['TE_EFCalo'], [self.trigJetRecKt, self.efJetHypoKt], "EFKt_"+sig_id)
              else:
                 jetSequencesDict[sig_id]['TE_EF'] = triggerPythonConfig.addSequence( TE_L2, [self.trigCaloCellMakerJet, self.trigCaloTowerMakerJet, self.trigJetRecCone, self.efJetHypo], "EF_"+sig_id)

JetSequences = [
                JetSequence("j5"),JetSequence("j10"),JetSequence("j18"),JetSequence("j20"),JetSequence("j23"),JetSequence("j33"),
                JetSequence("j35"),JetSequence("j42"),JetSequence("j50"),JetSequence("j55"),JetSequence("j62"),JetSequence("j70"),
                JetSequence("j84"),JetSequence("j120"),JetSequence("j150"),JetSequence("j200"),JetSequence("j300"),
                JetSequence("j400"), 
                JetSequence("fj10"),JetSequence("fj18"),JetSequence("fj26"),JetSequence("fj65"),JetSequence("fj35"),
                JetSequence("fj70"), JetSequence("fj120"),JetSequence("fj200"),
                JetSequence("je120"), JetSequence("je220"), JetSequence("je280"),JetSequence("je400"),JetSequence("je500"),
                JetSequence("je340")]

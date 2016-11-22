#configuration file for jet160 signature
include.block("TrigJetHypo/TriggerConfig_jet160.py")


from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloJet_Jet
from TrigJetHypo.TrigJetHypoL2Config import L2JetHypo_jet160

l2chain = HLTChain( chain_name="jet160_L2", chain_counter="1", lower_chain_name="L1_J80", level="L2", prescale="1", pass_through="1")
l2chain.addTriggerTypeBit(1)
l2chain.addStreamTag("jets")


efchain = HLTChain( chain_name="jet160_EF", chain_counter="1", lower_chain_name="jet160_L2", level="EF", prescale="1", pass_through="1")
efchain.addTriggerTypeBit(1)
efchain.addStreamTag("jets")


TE="JT55"

if TriggerFlags.JetSlice.doL2Calo():

   sequence = []
   
   # config for T2CaloJet
   t2calojet = T2CaloJet_Jet()
   sequence += [ t2calojet ]
   #include("TrigT2CaloCommon/TriggerConfig_TrigT2Calo.py")
   #t2caloJet=TriggerConfig_TrigT2Calo("L2","jet")

   #include("TrigJetHypo/TriggerConfig_L2_TrigJetHypo.py")
   #t2JetHypo=TriggerConfig_L2_TrigJetHypo("L2","jet160")
   t2JetHypo_jet160=L2JetHypo_jet160()

   #TE = triggerPythonConfig.addSequence( TE, appendLevel(
   #    [ t2caloJet.classAndInstanceName()[0], t2JetHypo.classAndInstanceName()[0]], "L2"), "L2_jet160" )
   #sequence += t2JetHypo.classAndInstanceName()
   sequence += t2JetHypo_jet160

   TE = triggerPythonConfig.addSequence(
      TE, sequence, "L2_jet160")
   l2chain.addHLTSignature(TE)

if TriggerFlags.JetSlice.doEFCalo():

   sequence = []

   include("TrigCaloRec/TriggerConfig_TrigCaloRec.py")

   trigCaloCellMakerJet=TriggerConfig_TrigCaloCellMaker("EF","tower")
   sequence+=trigCaloCellMakerJet.classAndInstanceName()

   trigCaloTowerMakerJet=TriggerConfig_TrigCaloTowerMaker("EF","tower")
   sequence+=trigCaloTowerMakerJet.classAndInstanceName()

   include("TrigJetRec/TriggerConfig_TrigJetRec.py")

   trigJetRecCone = TriggerConfig_TrigJetRec("EF","cone")
   sequence+=trigJetRecCone.classAndInstanceName()

   include("TrigJetHypo/TriggerConfig_EF_TrigJetHypo.py")
   trigJetHypo =TriggerConfig_TrigJetHypo("EF","jet160") 
   sequence+=trigJetHypo.classAndInstanceName()
   #trigJetHypo_jet160 = EFJetHypo("EF","jet160") 
   #sequence+=trigJetHypo.classAndInstanceName()
  
   TE = triggerPythonConfig.addSequence( TE, appendLevel(sequence,"EF"), "EF_jet160")

   efchain.addHLTSignature(TE)

triggerPythonConfig.addHLTChain(l2chain)
triggerPythonConfig.addHLTChain(efchain)

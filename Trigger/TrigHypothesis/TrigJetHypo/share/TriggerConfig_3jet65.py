#configuration file for 3jet65 signature
include.block("TrigJetHypo/TriggerConfig_3jet65.py")

l2chain = HLTChain( chain_name="3jet65_L2", chain_counter="1", lower_chain_name="L1_J35", level="L2", prescale="1", pass_through="1")
l2chain.addTriggerTypeBit(1)
l2chain.addStreamTag("jets")


efchain = HLTChain( chain_name="3jet65_EF", chain_counter="1", lower_chain_name="3jet65_L2", level="EF", prescale="1", pass_through="1")
efchain.addTriggerTypeBit(1)
efchain.addStreamTag("jets")


TE="JT35"

if TriggerFlags.JetSlice.doL2Calo():
   include("TrigT2CaloCommon/TriggerConfig_TrigT2Calo.py")
   include("TrigJetHypo/TriggerConfig_L2_TrigJetHypo.py")

   t2caloJet=TriggerConfig_TrigT2Calo("L2","jet")
   t2JetHypo=TriggerConfig_L2_TrigJetHypo("L2","3jet65")

   TE = triggerPythonConfig.addSequence( TE, appendLevel(
        [ t2caloJet.classAndInstanceName()[0], t2JetHypo.classAndInstanceName()[0]], "L2"), "L2_3jet65" )

   l2chain.addHLTSignature(TElist=[ TE, TE, TE ])

if TriggerFlags.JetSlice.doEFCalo():

    sequence[]

    include("TrigCaloRec/TriggerConfig_TrigCaloRec.py")

    trigCaloCellMakerJet=TriggerConfig_TrigCaloCellMaker("EF","tower")
    sequence+=trigCaloCellMakerJet.classAndInstanceName()

    trigCaloTowerMakerJet=TriggerConfig_TrigCaloTowerMaker("EF","tower")
    sequence+=trigCaloTowerMakerJet.classAndInstanceName()

    include("TrigJetRec/TriggerConfig_TrigJetRec.py")

    trigJetRecCone = TriggerConfig_TrigJetRec("EF","cone")
    sequence+=trigJetRecCone.classAndInstanceName()

    include("TrigJetHypo/TriggerConfig_EF_TrigJetHypo.py")
    trigJetHypo =TriggerConfig_TrigJetHypo("EF","3jet65") 
    sequence+=trigJetHypo.classAndInstanceName()
  
    TE = triggerPythonConfig.addSequence( TE, appendLevel(sequence,"EF"), "EF_3jet65")

    efchain.addHLTSignature(TElist=[ TE, TE, TE ]) 

triggerPythonConfig.addHLTChain(l2chain)
triggerPythonConfig.addHLTChain(efchain)

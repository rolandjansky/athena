#configuration file for 4jet50 signature
include.block("TrigJetHypo/TriggerConfig_4jet50.py")

l2chain = HLTChain( chain_name="4jet50_L2", chain_counter="1", lower_chain_name="L1_J35", level="L2", prescale="1", pass_through="1")
l2chain.addTriggerTypeBit(1)
l2chain.addStreamTag("jets")


efchain = HLTChain( chain_name="4jet50_EF", chain_counter="1", lower_chain_name="4jet50_L2", level="EF", prescale="1", pass_through="1")
efchain.addTriggerTypeBit(1)
efchain.addStreamTag("jets")


TE="JT20"

if TriggerFlags.JetSlice.doL2Calo():
   include("TrigT2CaloCommon/TriggerConfig_TrigT2Calo.py")
   include("TrigJetHypo/TriggerConfig_L2_TrigJetHypo.py")

   t2caloJet=TriggerConfig_TrigT2Calo("L2","jet")
   t2JetHypo=TriggerConfig_L2_TrigJetHypo("L2","4jet50")

   TE = triggerPythonConfig.addSequence( TE, appendLevel(
        [ t2caloJet.classAndInstanceName()[0], t2JetHypo.classAndInstanceName()[0]], "L2"), "L2_4jet50" )

   l2chain.addHLTSignature(TElist=[ TE, TE, TE, TE ])

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
    trigJetHypo =TriggerConfig_TrigJetHypo("EF","4jet50") 
    sequence+=trigJetHypo.classAndInstanceName()
  
    TE = triggerPythonConfig.addSequence( TE, appendLevel(sequence,"EF"), "EF_4jet50")

    efchain.addHLTSignature(TElist=[ TE, TE, TE, TE ]) 

triggerPythonConfig.addHLTChain(l2chain)
triggerPythonConfig.addHLTChain(efchain)

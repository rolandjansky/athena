#configuration file for jet20 signature
include.block("TrigJetHypo/TriggerConfig_jet20.py")

#Sequence leading to one jet: jet20

jet20list = ["jet20a", "jet20b", "jet20c", "jet20d"]


for jet20 in jet20list:



    l2chain = HLTChain( chain_name=jet20+"_L2", chain_counter=str(jet20list.index(jet20)), lower_chain_name="L1_J35", level="L2", prescale="1", pass_through="1")
    l2chain.addTriggerTypeBit(1)
    l2chain.addStreamTag("jets")


    efchain = HLTChain( chain_name=jet20+"_EF", chain_counter=str(jet20list.index(jet20)), lower_chain_name=jet20+"_L2", level="EF", prescale="1", pass_through="1")
    efchain.addTriggerTypeBit(1)
    efchain.addStreamTag("jets")



    TE="JT15"



    if TriggerFlags.JetSlice.doL2Calo():
        include("TrigT2CaloCommon/TriggerConfig_TrigT2Calo.py")
        include("TrigJetHypo/TriggerConfig_L2_TrigJetHypo.py")
        #from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloJet_Jet
        #from TrigJetHypo.TrigL2JetHypo import TrigL2JetHypo_jet20

        t2caloJet=TriggerConfig_TrigT2Calo("L2","jet")
        t2JetHypo=TriggerConfig_L2_TrigJetHypo("L2",jet20)

        TE = triggerPythonConfig.addSequence( TE, appendLevel(
            [ t2caloJet.classAndInstanceName()[0], t2JetHypo.classAndInstanceName()[0]], "L2"), "L2_"+jet20 )

        if jet20 == "jet20a":
            TElist=[ TE ]
        if jet20 == "jet20b":
            TElist=[ TE, TE ]
        if jet20 == "jet20c":
            TElist=[ TE, TE, TE ]
        if jet20 == "jet20d":
            TElist=[ TE, TE, TE, TE ]

        l2chain.addHLTSignature(TElist)



        # if TriggerFlags.JetSlice.doEFCalo():
        # include("TrigCaloRec/TriggerConfig_TrigCaloRec.py")
#        include("TrigJetRec/TriggerConfig_TrigJetRec.py")
#        include("TrigJetHypo/TriggerConfig_EF_TrigJetHypo.py")

#        trigCaloCellMakerJet=TriggerConfig_TrigCaloCellMaker("EF","tower")
#        trigCaloTowerMakerJet=TriggerConfig_TrigCaloTowerMaker("EF","tower")
#        trigJetRecCone = TriggerConfig_TrigJetRec("EF","cone")
#        trigJetHypoCone =TriggerConfig_TrigJetHypo("EF",jet20)

#        TE = sequenceFile.addSequence("EF",TE,trigCaloCellMakerJet.classAndInstanceName(),"EF"+jet20+"cells")
#        TE = sequenceFile.addSequence("EF",TE,trigCaloTowerMakerJet.classAndInstanceName(),"EF"+jet20+"towers")
#        TE = sequenceFile.addSequence("EF",TE,trigJetRecCone.classAndInstanceName(),"EF"+jet20+"conerec")
#        TE = sequenceFile.addSequence("EF",TE,trigJetHypoCone.classAndInstanceName(),"EF_"+jet20+"Et")


    triggerPythonConfig.addHLTChain(l2chain)
    triggerPythonConfig.addHLTChain(efchain)

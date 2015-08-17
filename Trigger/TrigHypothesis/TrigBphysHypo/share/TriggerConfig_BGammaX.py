
include.block("TrigBphysHypo/TriggerConfig_BJpsiee.py")

# Add sequences here for each signature
#----------------------------------------------------------
# Sequence leading to BGammaX
# Using FullScan for now
# JK (19/7/07) Only L2 at moment 

TE="EM03" 
l2chain = HLTChain( chain_name="BGammaX_L2", chain_counter="1610", lower_chain_name="L1_MU06EM03", level="L2", prescale="1", pass_through="1")
l2chain.addTriggerTypeBit(1)
l2chain.addStreamTag("bphysics")


#efchain = HLTChain( chain_name="BGammaX_EF", chain_counter="1610", lower_chain_name="BsDsPhiPi_L2", level="EF", prescale="1", pass_through="1")
#efchain.addTriggerTypeBit(1)
#efchain.addStreamTag("bphysics")

if TriggerFlags.doMuon:
    if TriggerFlags.BphysicsSlice.doL2Muon():
        l2chain.addHLTSignature("L2_mu6")
        l2chain.evaluateSignatureAfter( "L2_mu6", "L2_mu6", "L2_mu6" ) 
    if TriggerFlags.BphysicsSlice.doEFMuon():
        efchain.addHLTSignature("EF_mu6")

from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_FullScan
from TrigSteeringTest.TrigSteeringTestConf import  PESA__newDummyAlgoScan

sequence = []
if TriggerFlags.BphysicsSlice.doL2Calo():
        t2caloBphysics = T2CaloEgamma_eGamma()
        sequence += [ t2caloBphysics ]
#        t2caloHypoBphysics = L2CaloHypo_e10()
#        sequence += [ t2caloHypoBphysics ]

        TE = triggerPythonConfig.addSequence(TE,sequence,"L2calo_BGammaX")
        l2chain.addHLTSignature(TE)


sequence = []
if TriggerFlags.BphysicsSlice.doL2ID():
    DummyAlgo = PESA__newDummyAlgoScan("DummyAlgo")
    sequence += [DummyAlgo]
    if TriggerFlags.BphysicsSlice.testFlag("doIDSCAN"):
        idscanFullScan = TrigIDSCAN_FullScan()
        sequence += [idscanFullScan]

TE = triggerPythonConfig.addSequence("", sequence, "L2_BGammaX_FS")
l2chain.addHLTSignature(TE)

#sequence = []
#
#from TrigBphysHypo.TrigL2BGammaXFexConfig import L2DsPhiPiFex_FS
#t2DsPhiPiFex = L2DsPhiPiFex_FS()
#sequence += [ t2DsPhiPiFex ]
#
#from TrigBphysHypo.TrigL2DsPhiPiHypoConfig import L2DsPhiPiHypo_FS
#t2DsPhiPiHypo = L2DsPhiPiHypo_FS()
#sequence += [ t2DsPhiPiHypo ]
#
#
#TE = triggerPythonConfig.addSequence(TE, sequence ,"L2_BsDsPhiPiFullScan")

#--------------------------------------------------------------------
# Add signature
l2chain.addHLTSignature(TE)


#sequence = []    
#if TriggerFlags.BphysicsSlice.doEFID():
#    from InDetTrigRecExample.EFInDetConfig import *
#    TrigEFTrackingBphys = TrigEFIDInsideOut_Bphysics()
#    TE = triggerPythonConfig.addSequence(TE, TrigEFTrackingBphys.getSequence() ,"EF_Bstracks")
#    efchain.addHLTSignature(TE)

# now the DsPhiPi Fex and hypo
#sequence=[]

#from TrigBphysHypo.TrigEFDsPhiPiFexConfig import EFDsPhiPiFex_1
#EFDsPhiPiFex = EFDsPhiPiFex_1()
#sequence += [ EFDsPhiPiFex ]

#from TrigBphysHypo.TrigEFDsPhiPiHypoConfig import EFDsPhiPiHypo_1
#EFDsPhiPiHypo = EFDsPhiPiHypo_1()
#sequence += [ EFDsPhiPiHypo ]

#TE = triggerPythonConfig.addSequence(TE, sequence ,"EF_BsDsPhiPi")
#efchain.addHLTSignature(TE)


#
#--------------------------------------------------------------------
# Add signature
triggerPythonConfig.addHLTChain(l2chain)
#triggerPythonConfig.addHLTChain(efchain)


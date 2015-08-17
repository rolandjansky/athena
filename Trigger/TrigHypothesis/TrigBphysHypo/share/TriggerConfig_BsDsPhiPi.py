include.block("TrigBphysHypo/TriggerConfig_BsDsPhiPi.py")

# Add sequences here for each signature
#----------------------------------------------------------
# Sequence leading to single electron 
#

TE="JT04" 
#l2chain = HLTChain( chain_name="BsDsPhiPi_L2", chain_counter="1602", lower_chain_name="JT04", level="L2", prescale="1", pass_through="1")
l2chain = HLTChain( chain_name="BsDsPhiPi_L2", chain_counter="1602", lower_chain_name="L1_MU06JT04", level="L2", prescale="1", pass_through="1")
l2chain.addTriggerTypeBit(1)
l2chain.addStreamTag("bphysics")


efchain = HLTChain( chain_name="BsDsPhiPi_EF", chain_counter="1602", lower_chain_name="BsDsPhiPi_L2", level="EF", prescale="1", pass_through="1")
efchain.addTriggerTypeBit(1)
efchain.addStreamTag("bphysics")


if TriggerFlags.doMuon:
    if TriggerFlags.BphysicsSlice.doL2Muon():
        l2chain.addHLTSignature("L2_mu6")
        l2chain.evaluateSignatureAfter( "L2_mu6", "L2_mu6", "L2_mu6" ) 
    if TriggerFlags.BphysicsSlice.doEFMuon():
        efchain.addHLTSignature("EF_mu6")


from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Bphysics

from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Bphysics


# this helper variable is to keep track of algorithms beeing added throud all the if statements below
sequence = []    
if TriggerFlags.BphysicsSlice.doL2ID():
    if TriggerFlags.BphysicsSlice.testFlag("doIDSCAN"):
        idscanBphysics = TrigIDSCAN_Bphysics()
        sequence += [ idscanBphysics ]

    if TriggerFlags.BphysicsSlice.testFlag("doSiTrack"):
        sitrackBphysics = TrigSiTrack_Bphysics()
        sequence += [ sitrackBphysics ]

if len(sequence) is not 0:    
    TE = triggerPythonConfig.addSequence(TE,sequence,"L2_Bstracks")
    l2chain.addHLTSignature(TE)

sequence = []    

from TrigBphysHypo.TrigL2DsPhiPiFexConfig import L2DsPhiPiFex_1
t2DsPhiPiFex = L2DsPhiPiFex_1()
sequence += [ t2DsPhiPiFex ]

from TrigBphysHypo.TrigL2DsPhiPiHypoConfig import L2DsPhiPiHypo_1
t2DsPhiPiHypo = L2DsPhiPiHypo_1()
sequence += [ t2DsPhiPiHypo ]

TE = triggerPythonConfig.addSequence(TE, sequence ,"L2_BsDsPhiPi")
l2chain.addHLTSignature(TE)



sequence = []    
if TriggerFlags.BphysicsSlice.doEFID():
    from InDetTrigRecExample.EFInDetConfig import *

    TrigEFTrackingBphys = TrigEFIDInsideOut_Bphysics()

    TE = triggerPythonConfig.addSequence(TE, TrigEFTrackingBphys.getSequence() ,"EF_Bstracks")
    efchain.addHLTSignature(TE)

# now the DsPhiPi Fex and hypo
sequence=[]


from TrigBphysHypo.TrigEFDsPhiPiFexConfig import EFDsPhiPiFex_1
EFDsPhiPiFex = EFDsPhiPiFex_1()
sequence += [ EFDsPhiPiFex ]

from TrigBphysHypo.TrigEFDsPhiPiHypoConfig import EFDsPhiPiHypo_1
EFDsPhiPiHypo = EFDsPhiPiHypo_1()
sequence += [ EFDsPhiPiHypo ]

TE = triggerPythonConfig.addSequence(TE, sequence ,"EF_BsDsPhiPi")
efchain.addHLTSignature(TE)


#
#--------------------------------------------------------------------
# Add signature
triggerPythonConfig.addHLTChain(l2chain)
triggerPythonConfig.addHLTChain(efchain)


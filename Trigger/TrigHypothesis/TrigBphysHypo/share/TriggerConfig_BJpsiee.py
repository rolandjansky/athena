
include.block("TrigBphysHypo/TriggerConfig_BJpsiee.py")

# Add sequences here for each signature
#----------------------------------------------------------
# Sequence leading to Jpsiee
# JK (2/5/07) Only L2 at moment 

TE="EM03" 
#l2chain = HLTChain( chain_name="BJpsiee_L2", chain_counter="1604", lower_chain_name="L1_EM5", level="L2", prescale="1", pass_through="1")
l2chain = HLTChain( chain_name="BJpsiee_L2", chain_counter="1604", lower_chain_name="L1_MU06EM03", level="L2", prescale="1", pass_through="1")
l2chain.addTriggerTypeBit(1)
l2chain.addStreamTag("bphysics")


#efchain = HLTChain( chain_name="BsDsPhiPi_EF", chain_counter="1604", lower_chain_name="BsDsPhiPi_L2", level="EF", prescale="1", pass_through="1")
#efchain.addTriggerTypeBit(1)
#efchain.addStreamTag("bphysics")

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
    TE = triggerPythonConfig.addSequence(TE,sequence,"L2_JpsieeTracks")
    l2chain.addHLTSignature(TE)

sequence = []    

from TrigBphysHypo.TrigL2JpsieeFexConfig import L2JpsieeFex_1
t2JpsieeFex = L2JpsieeFex_1()
sequence += [ t2JpsieeFex ]

from TrigBphysHypo.TrigL2JpsieeHypoConfig import L2JpsieeHypo_1
t2JpsieeHypo = L2JpsieeHypo_1()
sequence += [ t2JpsieeHypo ]


TE = triggerPythonConfig.addSequence(TE, sequence ,"L2_BJpsiee")
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


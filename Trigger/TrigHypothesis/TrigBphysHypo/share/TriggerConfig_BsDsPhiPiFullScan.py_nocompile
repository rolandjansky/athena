include.block("TrigBphysHypo/TriggerConfig_BsDsPhiPiFullScan.py")

# Add sequences here for each signature
#----------------------------------------------------------
# Sequence leading to single electron
#

if TrigT1ConfigVersion=="DC2" and TriggerFlags.doLVL1==False:
    TE = "L1_MU01"
elif HLTconfigVersion.find("900GeV")!=-1:
    TE = "L1_MU00" # 900 GeV menu
else:
    TE="L1_MU06"

l2chain = HLTChain( chain_name="BsDsPhiPiFullScan_L2", chain_counter="1603", lower_chain_name=TE, level="L2", prescale="1", pass_through="0")
#l2chain = HLTChain( chain_name="BsDsPhiPiFullScan_L2", chain_counter="1603", lower_chain_name="JT04", level="L2", prescale="1", pass_through="0")
l2chain.addTriggerTypeBit(1)
l2chain.addStreamTag("bphysics")

efchain = HLTChain( chain_name="BsDsPhiPiFullScan_EF", chain_counter="2", lower_chain_name="BsDsPhiPiFullScan_L2", level="EF", prescale="1", pass_through="1")
efchain.addTriggerTypeBit(1)
efchain.addStreamTag("bphysics")


from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_FullScan
from TrigSteeringTest.TrigSteeringTestConf import  PESA__newDummyAlgoScan


sequence = []
if TriggerFlags.BphysicsSlice.doL2ID():
    DummyAlgo = PESA__newDummyAlgoScan("DummyAlgo")
    sequence += [DummyAlgo]
    if TriggerFlags.BphysicsSlice.testFlag("doIDSCAN"):
        idscanFullScan = TrigIDSCAN_FullScan()
        sequence += [idscanFullScan]

TE = triggerPythonConfig.addSequence("", sequence, "L2tracks_BsDsPhiPiFS")
l2chain.addHLTSignature(TE)

sequence = []

from TrigBphysHypo.TrigL2DsPhiPiFexConfig import L2DsPhiPiFex_FS
t2DsPhiPiFex = L2DsPhiPiFex_FS()
sequence += [ t2DsPhiPiFex ]

from TrigBphysHypo.TrigL2DsPhiPiHypoConfig import L2DsPhiPiHypo_FS
t2DsPhiPiHypo = L2DsPhiPiHypo_FS()
sequence += [ t2DsPhiPiHypo ]


TE = triggerPythonConfig.addSequence(TE, sequence ,"L2_BsDsPhiPiFullScan")

#--------------------------------------------------------------------
# Add signature
l2chain.addHLTSignature(TE)

sequence = []    
if TriggerFlags.BphysicsSlice.doEFID():
#    from InDetTrigRecExample.TriggerConfig_InDetEF import *
    from InDetTrigRecExample.EFInDetConfig import *
#    include("InDetTrigRecExample/TriggerConfig_InDetEF.py")

    TrigEFTrackingBphys = TrigEFIDInsideOut_FullScan()
#    sequence += TrigEFTrackingBphys.classAndInstanceName()
#    sequence += [TrigEFTrackingBphys]
#    efTracking = 

#if len(sequence) is not 0:
    TE = triggerPythonConfig.addSequence(TE, TrigEFTrackingBphys.getSequence() ,"EFtracks_BsDsPhiPiFS")
    efchain.addHLTSignature(TE)

# now the DsPhiPi Fex and hypo
sequence=[]


from TrigBphysHypo.TrigEFDsPhiPiFexConfig import EFDsPhiPiFex_FS
EFDsPhiPiFex = EFDsPhiPiFex_FS()
sequence += [ EFDsPhiPiFex ]

from TrigBphysHypo.TrigEFDsPhiPiHypoConfig import EFDsPhiPiHypo_FS
EFDsPhiPiHypo = EFDsPhiPiHypo_FS()
sequence += [ EFDsPhiPiHypo ]

TE = triggerPythonConfig.addSequence(TE, sequence ,"EF_BsDsPhiPiFullScan")
efchain.addHLTSignature(TE)


#
#--------------------------------------------------------------------
# Add signature
triggerPythonConfig.addHLTChain(l2chain)
triggerPythonConfig.addHLTChain(efchain)


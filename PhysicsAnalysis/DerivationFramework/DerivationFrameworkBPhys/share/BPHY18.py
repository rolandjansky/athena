
# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

triggerList = [
"HLT_2e5_lhvloose_nod0_bBeexM6000t",  #37,143,877  inb
"HLT_e5_lhvloose_nod0_bBeexM6000t",  #37,143,877
"HLT_e5_lhvloose_nod0_bBeexM6000t_2mu4_nomucomb_L1BPH-0DR3-EM7J15_2MU4",   #37,312,506
"HLT_e5_lhvloose_nod0_bBeexM6000t_mu6_nomucomb_L1BPH-0DR3-EM7J15_MU6",   #27,041,892
"HLT_e5_lhvloose_nod0_bBeexM6000_mu6_nomucomb_L1BPH-0DR3-EM7J15_MU6",   #149,100	
"HLT_e9_lhloose_bBeexM2700_2mu4_nomucomb_L1BPH-0DR3-EM7J15_2MU4",   #2,681,764
"HLT_e9_lhloose_bBeexM2700_mu6_nomucomb_L1BPH-0DR3-EM7J15_MU6",   #1,979,362
"HLT_e9_lhloose_bBeexM6000_2mu4_nomucomb_L1BPH-0DR3-EM7J15_2MU4",   #3,359,105
"HLT_e9_lhloose_bBeexM6000_mu6_nomucomb_L1BPH-0DR3-EM7J15_MU6",   #2,426,663
"HLT_e9_lhloose_e5_lhloose_bBeexM2700_2mu4_nomucomb_L1BPH-0M9-EM7-EM5_2MU4",   #2,950,935
"HLT_e9_lhloose_e5_lhloose_bBeexM2700_mu6_nomucomb_L1BPH-0M9-EM7-EM5_MU6",   #2,928,030
"HLT_e9_lhloose_e5_lhloose_bBeexM6000_2mu4_nomucomb_L1BPH-0M9-EM7-EM5_2MU4",   #3,647,507
"HLT_e9_lhloose_e5_lhloose_bBeexM6000_mu6_nomucomb_L1BPH-0M9-EM7-EM5_MU6",   #3,605,371
"HLT_e9_lhvloose_nod0_e5_lhvloose_nod0_bBeexM6000t_2mu4_nomucomb_L1BPH-0M9-EM7-EM5_2MU4",   #40,169,436
"HLT_e9_lhvloose_nod0_e5_lhvloose_nod0_bBeexM6000t_mu6_nomucomb_L1BPH-0M9-EM7-EM5_MU6",   #37,312,506
"HLT_e9_lhvloose_nod0_e5_lhvloose_nod0_bBeexM6000_mu6_nomucomb_L1BPH-0M9-EM7-EM5_MU6"   #677,340
]	# BeeX triggers
#triggerList =  ['HLT_noalg_L1RD0_FILLED']
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
BPHY18TriggerSkim = DerivationFramework__TriggerSkimmingTool(name = "BPHY18TriggerSkim",
                                                            TriggerListOR = triggerList)

ToolSvc += BPHY18TriggerSkim
print BPHY18TriggerSkim


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
BPHY18SkimmingOR = DerivationFramework__FilterCombinationOR(
    "BPHY18SkimmingOR",
    FilterList =# [BPHY18_SelectBeeKstEvent]) 
                    [BPHY18TriggerSkim]) #OR of all your different filters
ToolSvc += BPHY18SkimmingOR
print      BPHY18SkimmingOR

# The name of the kernel (BPHY18Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
    "BPHY18Kernel",
    #Only skim if not MC
#    SkimmingTools     = [BPHY18SkimmingOR] if skimTruth or not isSimulation else [],
    SkimmingTools     = [BPHY18SkimmingOR]
    )


#====================================================================
# SET UP STREAM   
#====================================================================
streamName   = derivationFlags.WriteDAOD_BPHY18Stream.StreamName
fileName     = buildFileName( derivationFlags.WriteDAOD_BPHY18Stream )
BPHY18Stream  = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY18Stream.AcceptAlgs(["BPHY18Kernel"])

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY18SlimmingHelper = SlimmingHelper("BPHY18SlimmingHelper")
AllVariables  = []
StaticContent = []
ExtraVariables = []
BPHY18SlimmingHelper.SmartCollections = ["Electrons"]

BPHY18SlimmingHelper.AppendContentToStream(BPHY18Stream)

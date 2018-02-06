#====================================================================
# HION10.py
# author: Qipeng Hu <qipeng.hu@cern.ch>, Jakub Kremer <jakub.kremer@cern.ch>
# Application: D meson analyses
# Triggers: MinBias, jet
# Selection: only trigger selection
# Content: InDetTrackParticles, PrimaryVertices, R=0.4 EMTopo jets, slimmed FCal information
#====================================================================

# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkHI.HISkimmingTools import *
from DerivationFrameworkHI.HIDerivationFlags import HIDerivationFlags

#====================================================================
#Check to see if it is MC Derivation
#====================================================================
GetConditionsFromMetaData()

isSimulation = False
if HIDerivationFlags.isSimulation() or globalflags.DataSource()=='geant4':
    isSimulation = True

print '++++++++++++++++++++++++++++++++++ Start Checking Flags +++++++++++++++++++++++++++++++++++'
print HIDerivationFlags
print globalflags
print isSimulation
print '+++++++++++++++++++++++++++++++++++ End Checking Flags +++++++++++++++++++++++++++++++++++'


#====================================================================
# SKIMMING
#====================================================================
#removing events
#==========================================================
#Trigger selection
#==========================================================

MBTriggers =  [	'HLT_mb_mbts_L1MBTS_1', 'HLT_mb_sptrk_L1MBTS_1', 'HLT_mb_sptrk']

jetTriggers = [	'HLT_j30_ion_0eta490_L1TE10', 
	 	'HLT_j40_ion_L1J5', 
		'HLT_j50_ion_L1J10', 
		'HLT_j60_ion_L1J20', 
		'HLT_j75_ion_L1J20', 
		'HLT_j100_ion_L1J20', 
		'HLT_j40_L1J5', 
		'HLT_j50_L1J10', 
		'HLT_j60', 
		'HLT_j75_L1J20', 
		'HLT_j100_L1J20']

bjetTriggers = ['HLT_j40_bmv2c2070_split_L1J10',
		'HLT_j50_bmv2c2070_split_L1J15',
		'HLT_j60_bmv2c2070_split_L1J15', 
		'HLT_j75_bmv2c2070_split_L1J20']

HMTTriggers =  ['HLT_mb_sp2400_pusup500_trk120_hmt_L1TE20',
		'HLT_mb_sp2800_pusup600_trk140_hmt_L1TE20',
		'HLT_mb_sp2800_pusup600_trk140_hmt_L1TE50',
		'HLT_mb_sp3100_pusup700_trk160_hmt_L1TE50',
		'HLT_mb_sp3500_pusup800_trk180_hmt_L1TE90',
		'HLT_mb_sp4100_pusup900_trk200_hmt_L1TE90',
		'HLT_mb_sp4100_pusup900_trk200_hmt_L1TE120',
		'HLT_mb_sp4500_pusup1000_trk220_hmt_L1TE120', 
		'HLT_mb_sp4800_pusup1100_trk240_hmt_L1TE160',
		'HLT_mb_sp5000_pusup1100_trk240_hmt_L1TE200',
		'HLT_mb_sp5000_pusup1200_trk260_hmt_L1TE160',
		'HLT_mb_sp5600_pusup1300_trk260_hmt_L1TE200',
		'HLT_mb_sp5200_pusup1300_trk280_hmt_L1TE160',
		'HLT_mb_sp6000_pusup1400_trk280_hmt_L1TE200' ]

triggers = MBTriggers + jetTriggers + bjetTriggers + HMTTriggers


#==========================================================
#Event Selection
#==========================================================

req_trig = '(' + ' || '.join(triggers) + ')'
req_pv = 'count(abs(PrimaryVertices.z)<10000)>0'

expression = req_trig+'&&('+req_pv+')'

if isSimulation:
   expression = req_pv

print "================Expression====BEGIN=================="
print expression
print "================Expression====END===================="


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HION10SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HION10SkimmingTool",
                                                                      expression = expression )
ToolSvc += HION10SkimmingTool

#====================================================================
# KERNEL
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HION10Kernel",
                                                                       SkimmingTools = [HION10SkimmingTool]
                                                                      )

#====================================================================
# SET UP STREAM
#====================================================================

streamName = derivationFlags.WriteDAOD_HION10Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HION10Stream )
HION10Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HION10Stream.AcceptAlgs(["HION10Kernel"])

#====================================================================
# CONTENT LIST
#====================================================================
# Add the required contents (offline)
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HION10SlimmingHelper = SlimmingHelper("HION10SlimmingHelper")

HION10SlimmingHelper.SmartCollections = [ "AntiKt4EMTopoJets" ]
HION10SlimmingHelper.IncludeJetTriggerContent = True

HION10SlimmingHelper.AllVariables = ["HIEventShape"] 
HION10SlimmingHelper.AllVariables += ["PrimaryVertices"]
HION10SlimmingHelper.AllVariables += ["InDetTrackParticles"]

if isSimulation:
    HION10SlimmingHelper.AllVariables += ["TruthEvents","TruthParticles","TruthVertices"]


HION10SlimmingHelper.AppendContentToStream(HION10Stream)

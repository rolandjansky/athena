#====================================================================
# MuonSelectionExample.py 
# This example shows how the various muon selector tools can be used 
# to define a derivation.
# It is defined with the flag MUONTEST --reductionConf argument     
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
include("DerivationFrameworkCore/DerivationFrameworkMaster.py")

#====================================================================
# AUGMENTATION TOOLS 
#====================================================================

# This tool makes a transient entry into StoreGate indicating which of the muons
# have passed the MCP selection - as a vector of ints 
from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__MCPInDetRecommendations
MCPInDet = DerivationFramework__MCPInDetRecommendations(	name                    	= "MCPInDet1",
								StoreGateEntryName		= "passesMCPInDet",
								# Collection NTUPName: blank for 3rd chain
								# muid_ for Muid, staco_ for Staco	 
								CollectionNTUPName		= "" )
								
ToolSvc += MCPInDet 

# This tool makes a transient entry into StoreGate indicating which of the muons
# have passed the MCP high pt selection - as a vector of ints 
from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__MCPHighPtRecommendations
MCPHighPt = DerivationFramework__MCPHighPtRecommendations(        name                            = "MCPHighPt1",
                                                                StoreGateEntryName3Stations       = "passesMCPHighPt3st",
								StoreGateEntryName2Stations       = "passesMCPHighPt2st",
                                                                # Collection NTUPName: blank for 3rd chain
                                                                # muid_ for Muid, staco_ for Staco       
                                                                CollectionNTUPName              = "" )

ToolSvc += MCPHighPt

#====================================================================
# STRING BASED SKIMMING TOOL 
#====================================================================

# Now make the full selection via the string handler, including the entry
# made above 
expression = 'count((passesMCPInDet) && (mu_isCombinedMuon) && ((passesMCPHighPt3st) || (passesMCPHighPt2st)) && (mu_pt > 25*GeV)) >= 1'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__NTUPStringSkimmingTool
muonSkimmingTool = DerivationFramework__NTUPStringSkimmingTool(	name = "muonSkimmingTool1",
                                                             	expression = expression)

ToolSvc += muonSkimmingTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("MUONTEST_Kernel",
									AugmentationTools = [MCPInDet,MCPHighPt],
                                                                        SkimmingTools = [muonSkimmingTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
# Production flags (derivation name)
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_MUONTEST
ntupProdFlags = jobproperties.NTUPtoNTUPProdFlags
ntupFormatDefinition = prodFlags.WriteDNTUP_MUONTEST
ntupStreamName = ntupFormatDefinition.StreamName
ntupFileName   = buildFileName( ntupFormatDefinition )
## set input tree:
tree_name=ntupProdFlags.TreeName()
## Create the NTUP streams:
from NTUPtoNTUPCore import NTUPUtils
MNSMgr = NTUPUtils.MNSMgr()
ToolAndStringExStream = MNSMgr.NewNTUPStream( ntupStreamName, ntupFileName, tree_name)
ToolAndStringExStream.AddRequireAlgs( "MUONTEST_Kernel" )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkExamples.NTUPSkimExampleBranches import NTUPSkimExampleBranchList
ToolAndStringExStream.AddItem(NTUPSkimExampleBranchList)

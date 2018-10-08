#====================================================================
# TRIG4.py
# For use to study xAOD reduction based on AODSets defined in TriggerEDM
# Creates derivation from AODSuperSlim 
#====================================================================
# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
#====================================================================
# SKIMMING TOOLS 
#====================================================================
# Set up your skimming tools (you can have as many as you need). 
# The tools must use the DerivationFrameworkInterfaces as in this example.
# Currently no skimming - keep all events
#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
# The name of the kernel (TRIG2 in this case) must be unique to this 
# derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TRIG4Kernel")

# First define the content list from TriggerEDM

# Configure collection list via TriggerEDM.py
from TrigEDMConfig.TriggerEDM import getTriggerEDMList
edmlist=[]
for item in getTriggerEDMList('AODSUPERSLIM',2):
    edmlist.extend(getTriggerEDMList('AODSUPERSLIM',2)[item])
edmlist.remove('TrigNavigation')
edmlist.remove('TrigConfKeys')
#from DerivationFrameworkCore.CompulsoryContent import *
#edmlist(set(CompulsoryTriggerNavigation))
#edmlist(set())
#====================================================================
# SET UP STREAM   
#====================================================================
# SKIMMING
# The base name (DAOD_TRIG4 here) must match the string in 
# DerivationFrameworkProdFlags (in DerivationFrameworkCore) 
streamName = derivationFlags.WriteDAOD_TRIG4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TRIG4Stream )
TRIG4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
TRIG4Stream.AcceptAlgs(["TRIG4Kernel"])
        
# Thinning
from DerivationFrameworkTrigger.TrigThinningHelper import ThinningHelper
TRIG4ThinningHelper = ThinningHelper("TRIG4ThinningHelper",edmlist,'slimming')
TRIG4ThinningHelper.AppendToStream(TRIG4Stream)

#====================================================================
# CONTENT LIST  
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
TRIG4SlimmingHelper = SlimmingHelper("TRIG4SlimmingHelper")
##Smart Slimming
TRIG4SlimmingHelper.SmartCollections = []
TRIG4SlimmingHelper.AllVariables = edmlist
TRIG4SlimmingHelper.AppendContentToStream(TRIG4Stream)

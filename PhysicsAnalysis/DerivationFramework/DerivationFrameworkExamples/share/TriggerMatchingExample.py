# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#====================================================================
# TriggerMatchingExample.py
# This an example job options script showing how to set up trigger
# navigation in the derivation framework.
# It requires the reductionConf flag TEST11 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import (
    derivationFlags, buildFileName, MSMgr, DerivationFrameworkJob)
from DerivationFrameworkCore.DerivationFrameworkCoreConf import (
    DerivationFramework__DerivationKernel)
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkTrigger.TriggerMatchingHelper import (
    TriggerMatchingHelper)

#====================================================================
# SET UP MATCHING
#====================================================================

trig_names = ["HLT_mu26_ivarmedium", "HLT_e140_etcut", "HLT_2e17_lhvloose_nod0", "HLT_e20_lhmedium_nod0_g35_loose", "HLT_e30_mergedtight_ivarloose_g35_medium_icalovloose_Heg", "HLT_tau80_medium1_tracktwo"]
matching_helper = TriggerMatchingHelper(
        trigger_list, add_to_df_job=True)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (TEST11Kernel in this case) must be unique to
# this derivation
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
    "TEST11Kernel")

#====================================================================
# SET UP STREAM   
#====================================================================

# SKIMMING
# The base name (DAOD_TEST11 here) must match the string in 
# DerivationFrameworkProdFlags (in DerivationFrameworkCore) 
streamName = derivationFlags.WriteDAOD_TEST11Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TEST11Stream )
TEST11Stream = MSMgr.NewPoolRootStream( streamName, fileName )
TEST11Stream.AcceptAlgs(["TEST11Kernel"])

#====================================================================
# CONTENT LIST  
#====================================================================
TEST11SlimmingHelper = SlimmingHelper("TEST11SlimmingHelper")
# Add the offline objects that we're matching to
TEST11SlimmingHelper.SmartCollections = [
  "Electrons", "Photons", "Muons", "TauJets"]
matching_helper.add_to_slimming(TEST11SlimmingHelper)
TEST11SlimmingHelper.AppendContentToStream(TEST11Stream)


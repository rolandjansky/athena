# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# --------------------------------------------------------------
# EVGEN configuration
# --------------------------------------------------------------
evgenConfig.description = "PowhegControl example of NNLOPS syntax"
evgenConfig.keywords = ["SM", "Higgs", "jet"]
evgenConfig.contact = ["james.robinson@cern.ch"]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg Hj process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_Hj_Common.py")

# --------------------------------------------------------------
# Set integration parameters to tiny values for fast testing
# DO NOT USE THESE SETTINGS IN PRODUCTION!
# --------------------------------------------------------------
PowhegConfig.ncall1 = 1000
PowhegConfig.ncall1rm = 1000
PowhegConfig.ncall2 = 1000
PowhegConfig.ncall2rm = 1000
PowhegConfig.nubound = 1000
PowhegConfig.foldphi = 1
PowhegConfig.foldcsi = 1
PowhegConfig.foldy = 1
PowhegConfig.itmx1 = 1
PowhegConfig.itmx2 = 1

# --------------------------------------------------------------
# Reweighting: construct event weight group for quark mass variations.
# --------------------------------------------------------------
PowhegConfig.define_event_weight_group(group_name="quark_mass_variation", parameters_to_vary=["bmass_in_minlo", "quarkmasseffects"])
# mtmb means both top and b masses are considered in the loop. This is the nominal choice.
# mtinf is "EFT", meaning the top quark mass is set to infinity, and mb=0 (ignored)
# mtmb-bminlo is an alternative treatment of the b-mass
PowhegConfig.add_weight_to_group(group_name="quark_mass_variation", weight_name="mtmb", parameter_values=[0, 1])
PowhegConfig.add_weight_to_group(group_name="quark_mass_variation", weight_name="mtinf", parameter_values=[0, 0])
PowhegConfig.add_weight_to_group(group_name="quark_mass_variation", weight_name="mtmb-bminlo", parameter_values=[1, 1])

# --------------------------------------------------------------
# NNLOPS: specify the input files
# --------------------------------------------------------------
PowhegConfig.NNLO_reweighting_inputs["nn-mtinf"] = "H1250_CM13_CT10_APX0_11.top"
PowhegConfig.NNLO_reweighting_inputs["nn-mtmb"] = "H1250_CM13_CT10_APX2_22.top"

# --------------------------------------------------------------
# NNLOPS: specify the output weights: remember to escape nested quotes
# --------------------------------------------------------------
PowhegConfig.NNLO_output_weights["nnlops-mtmb"] = "combine 'nn-mtmb' and 'mtmb'"
PowhegConfig.NNLO_output_weights["nnlops-mtinf"] = "combine \"nn-mtinf\" and \"mtinf\""
PowhegConfig.NNLO_output_weights["nnlops-mtmb-bminlo"] = "combine \"nn-mtmb\" and 'mtmb-bminlo'"

# --------------------------------------------------------------
# Generate events
# --------------------------------------------------------------
PowhegConfig.generate()

# --------------------------------------------------------------
# Pythia8 showering with the AZNLO tune
# --------------------------------------------------------------
include("MC15JobOptions/Pythia8_AZNLO_CTEQ6L1_EvtGen_Common.py")
include("MC15JobOptions/Pythia8_Powheg.py")
# Decay to diphoton only
genSeq.Pythia8.Commands += ["25:onMode = off", "25:onIfMatch = 22 22"]

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Powheg Hj setup starting from ATLAS defaults
#--------------------------------------------------------------
include("PowhegControl/PowhegControl_Hj_Common.py")
# # To construct three additional event weights with user-defined names
PowhegConfig.define_event_weight_group( group_name="quark_mass_variation", parameters_to_vary=["bmass_in_minlo","quarkmasseffects"] )
PowhegConfig.add_weight_to_group( group_name="quark_mass_variation", weight_name="mtmb", parameter_values=[0,1] )
PowhegConfig.add_weight_to_group( group_name="quark_mass_variation", weight_name="mtinf", parameter_values=[0,0] )
PowhegConfig.add_weight_to_group( group_name="quark_mass_variation", weight_name="mtmb-bminlo", parameter_values=[1,1] )
# To combine the user-defined weights with NNLOPS
PowhegConfig.NNLO_reweighting_inputs = { "nn-mtinf":"H1250_CM13_CT10_APX0_11.top", "nn-mtmb":"H1250_CM13_CT10_APX2_22.top" }
PowhegConfig.NNLO_output_weights = { "nnlops-mtmb":"combine "nn-mtmb" and "mtmb"",\
                                     "nnlops-mtinf":"combine "nn-mtinf" and "mtinf"",\
                                     "nnlops-mtmb-bminlo":"combines "nn-mtmb" and "mtmb-bminlo""
                                   }
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 tune and NNPDF 2.3
#--------------------------------------------------------------
include("MC15JobOptions/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("MC15JobOptions/Pythia8_Powheg.py")

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 H+jet production with A14 NNPDF2.3 tune"
evgenConfig.keywords    = [ "Higgs", "jets" ]
evgenConfig.contact     = [ "james.robinson@cern.ch" ]

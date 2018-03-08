# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Powheg tt setup starting from ATLAS defaults
#--------------------------------------------------------------
include("PowhegControl/PowhegControl_tt_Common.py")
PowhegConfig.define_event_weight_group( group_name="hdamp_variation", parameters_to_vary=["hdamp","PDF"] )
PowhegConfig.add_weight_to_group( group_name="hdamp_variation", weight_name="hdamp_2mtop", parameter_values=[345.0,260000] )
PowhegConfig.add_weight_to_group( group_name="hdamp_variation", weight_name="hdamp_mtop_MMHT2014", parameter_values=[172.5,25200] )
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia6 showering with the Perugia2011 tune
#--------------------------------------------------------------
include("MC15JobOptions/PowhegPythia_Perugia2011C_Common.py")
include("MC15JobOptions/Pythia_Tauola.py")
include("MC15JobOptions/Pythia_Photos.py")

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia6 ttbar production with Perugia 2011c tune. Additional weights for 2 hdamp variations."
evgenConfig.keywords    = [ "SM", "top" ]
evgenConfig.contact     = [ "james.robinson@cern.ch" ]

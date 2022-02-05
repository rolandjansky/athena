# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# This is an example joboption to generate events with Powheg
# using ATLAS' interface. Users should optimise and carefully
# validate the settings before making an official sample request.
#--------------------------------------------------------------

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 tt+jet production with MiNNLO and A14 NNPDF2.3 tune."
evgenConfig.keywords = ["SM", "top", "1jet"]
evgenConfig.contact = ["tpelzer@cern.ch"]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg ttj_MiNNLO process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_ttj_MiNNLO_Common.py")

# --------------------------------------------------------------
# Settings
# --------------------------------------------------------------
# define the decay mode
PowhegConfig.decay_mode = "t t~ > all" # inclusive is the default
#PowhegConfig.decay_mode = "t t~ > all b j j b~ j j"
#PowhegConfig.decay_mode = "t t~ > all b l+ vl b~ l- vl~"
#PowhegConfig.decay_mode = "t t~ > b emu+ vemu b~ emu- vemu~"
#PowhegConfig.decay_mode = "t t~ > semileptonic"
#PowhegConfig.decay_mode = "t t~ > undecayed"
## for handling decays with MadSpin
#PowhegConfig.decay_mode = "t t~ > all [MadSpin]"
#PowhegConfig.MadSpin_decays= ["decay t > w+ b, w+ > l+ vl", "decay t~ > w- b~, w- > l- vl~"]
#PowhegConfig.MadSpin_process= "generate p p > t t~ j [QCD]" # this process is default - can be changed (for studies)

# --------------------------------------------------------------
# Generate events
# --------------------------------------------------------------
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune, main31 routine
#--------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_Powheg_Main31.py")
# Setting the appropriate number of final state particles for the main31 routine
genSeq.Pythia8.Commands += [ 'Powheg:NFinal = 3' ]

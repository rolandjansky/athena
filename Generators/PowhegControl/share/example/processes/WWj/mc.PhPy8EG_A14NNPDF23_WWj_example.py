# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 WWj production with A14 NNPDF2.3 tune."
evgenConfig.keywords = ["SM", "diboson", "WW", "1jet"]
evgenConfig.contact = ["tpelzer@cern.ch"]

# --------------------------------------------------------------
# Load ATLAS defaults for the Powheg WWj process
# --------------------------------------------------------------
include("PowhegControl/PowhegControl_WWj_Common.py")

# --------------------------------------------------------------
# parameters for integration precision - very low values for quick testing!
# --------------------------------------------------------------
PowhegConfig.ncall1       = "30"
PowhegConfig.ncall2       = "30"
PowhegConfig.ncall2rm     = "30"
PowhegConfig.nubound      = "30"

# --------------------------------------------------------------
# setting the decay channel
# --------------------------------------------------------------
# Possible values (PowhegControl syntax):
# "w+ w- > e+ ve e- ve~"
# "w+ w- > mu+ vm mu- vm~"
# "w+ w- > tau+ vt tau- vt~"
# "w+ w- > e+ ve mu- vm~ / mu+ vm e- ve~"
# "w+ w- > l+ vl l'- vl'~"
# "w+ w- > l+ vl j j / j j l- vl~"
# "w+ w- > e+ ve j j / j j e- ve~ / mu+ vmu j j / j j mu- vmu~"
# "w+ w- > j j j j"
# "w+ w- > e+ ve mu- vm~"
# "w+ w- > mu+ vm e- ve~"
# --------------------------------------------------------------
PowhegConfig.decay_mode   = "w+ w- > l+ vl l'- vl'~"

# --------------------------------------------------------------
# running scale
# --------------------------------------------------------------
# 0: fixed scale 2m(W)
# 1: m(WW)
# 2: mT(W+) + mT(W-)
# --------------------------------------------------------------
PowhegConfig.runningscales = "2"

# --------------------------------------------------------------
# minlo or nnlops - uncomment each line to activate them
# --------------------------------------------------------------
# from the manual: "Note that Minlo overwrites any other running scale choice. The NNLOPS
# works only if the Minlo option is on. Finally, if Minlo is switched off, one
# needs to use a Born suppression factor, or, alternatively, a cut in the phase-space
# generation, in order to make the cross-section finite. The latter cut can be set
# through the token bornktmin."
# --------------------------------------------------------------
#PowhegConfig.minlo        = "1"
#PowhegConfig.minlo_nnll   = "1"
#PowhegConfig.nnlops       = "1"

# --------------------------------------------------------------
# Generate events
# --------------------------------------------------------------
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
#--------------------------------------------------------------
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_Powheg.py")

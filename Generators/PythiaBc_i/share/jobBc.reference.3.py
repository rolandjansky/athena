###############################################################
#
#  B_c -> J/Psi(mu6mu4) Pi production with PythiaBc_i
#  author Konstantin Toms (ktoms@mail.cern.ch)
#  p_T of generated B_c > 10GeV
#  p_T of final hadrons > 500 MeV
#
#==============================================================
#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------

# MAKE EVTGEN MYDECAY FILE ON THE FLY
f = open("MYDECAY_Bc_JPsi_Pi.DEC","w")
f.write("Decay B_c+\n")
f.write("1.0000  J/psi pi+  SVS;\n")
f.write("Enddecay\n")
f.write("#\n")
f.write("Decay B_c-\n")
f.write("1.0000  J/psi pi-  SVS;\n")
f.write("Enddecay\n")
f.write("#\n")
f.write("Decay J/psi\n")
f.write("1.0000  mu+ mu-  VLL;\n")
f.write("Enddecay\n")
f.write("#\n")
f.write("End\n")
f.close()

from MC10JobOptions.PythiaEvgenConfig import evgenConfig
evgenConfig.auxfiles   = [ "DECAY.DEC","pdt.table" ]
evgenConfig.efficiency = 0.13

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

include( "MC10JobOptions/MC10_Pythia_Common.py" )

from EvtGen_i.EvtGen_iConf import EvtDecay
topAlg += EvtDecay()
EvtDecay = topAlg.EvtDecay

#from TruthExamples.TruthExamplesConf import TestHepMC
#TestHepMC.EnergyDifference = 100000000. # temporary fix, suppresses unnecessary output

from GeneratorFilters.GeneratorFiltersConf import BSignalFilter
topAlg += BSignalFilter()
BSignalFilter = topAlg.BSignalFilter

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------

Pythia.PythiaCommand = ["pyinit user PYTHIABC"]

# PythiaBc parameters to be specified by the user:
# 1: Switch for excited states.
# 2: reserved (int)
# 3: Total Centre of Mass Energy in GeV.
# 4: reserved (double)
# 5: PtCut of the Bc, in GeV.
# 6: Mass of c-quark, GeV.
# 7: Mass of b-quark, GeV.
	
# The default values are : "pythiabc 1 0", "pythiabc 2 0", "pythiabc 3 7000", "pythiabc 4 0","pythiabc 5 10", "pythiabc 6 1.5", "pythiabc 7 4.8"
	
Pythia.PythiaCommand += ["pythiabc 1 0", "pythiabc 2 0",
	                         "pythiabc 3 7000", "pythiabc 4 0",
	                         "pythiabc 5 10", "pythiabc 6 1.5",
	                         "pythiabc 7 4.8"]

Pythia.PythiaCommand += ["pyinit pylistf 2", "pyinit dumpr 1 10"]

# We have to many  color reconnection error otherwise...
Pythia.PythiaCommand += ["pypars mstp 61 0", "pypars mstp 71 0", "pypars mstp 81 0" ]

# CTEQ6L with UE tuning by Arthur Moraes
# author: B-Physics group, 2006-11-09
#
#--------------------------------------------------------------------------
Pythia.PythiaCommand += [
    "pysubs ckin 9 -4.5",
    "pysubs ckin 10 4.5",
    "pysubs ckin 11 -4.5",
    "pysubs ckin 12 4.5",
    "pydat1 mstj 26 0",
    "pydat1 mstj 22 2",
    "pydat1 parj 13 0.65",
    "pydat1 parj 14 0.12",
    "pydat1 parj 15 0.04",
    "pydat1 parj 16 0.12",
    "pydat1 parj 17 0.2",
    "pydat1 parj 55 -0.006",
    ]

#########################################################################################
####################### Arthur Moraes tuning (for reference) ############################

# lambda value for alpha_s
#                          "pydat1 parj 81 0.14",
# effects the amount of color reconnection
#                          "pypars parp 78 0.2",
# suppression probability for string pieces reattaching beam remnant 
# to hard scatter
#                          "pypars parp 80 0.01",
# effective pT_min for MI (pt0 scale)
#			  "pypars parp 82 1.9",
# matter distribution
#			  "pypars parp 83 0.3",
#			  "pypars parp 84 0.5",
# pT0 scale
#			  "pypars parp 89 1800",
#			  "pypars parp 90 0.22",
# ISR
#                          "pypars mstp 70 2",
# max scale for FSR dipoles
#                          "pypars mstp 72 0",
# new multiple interaction model and new parton shower selected
#                          "pypars mstp 81 1",
# MPI
#                          "pypars mstp 82 4",
# ISR
#                          "pypars mstp 84 1",
# FSR
#                          "pypars mstp 85 1",
# MPI
#                          "pypars mstp 86 2",
# sea quarks: large x behavior of assumed gluon distribution
#                          "pypars mstp 87 4",
# color reconnection 
#                          "pypars mstp 88 0",
#                          "pypars mstp 89 1",
#                          "pypars mstp 90 1",
#                          "pypars mstp 95 1",
#                          
#########################################################################################
#########################################################################################

#   Force J/psi to mu+ mu-
Pythia.PythiaCommand += ["pydat3 mdme 858 1 0" ,
                         "pydat3 mdme 859 1 1",
                         "pydat3 mdme 860 1 0"        ]
                                                                              
# open your exclusive channel here  Bc+ -> J/psi pi+
Pythia.PythiaCommand += ["pydat3 mdme 1013 1 1",
                         "pydat3 mdme 1013 2 0",
                         "pydat3 kfdp 1013 1 443",
                         "pydat3 kfdp 1013 2 211",
                         "pydat3 kfdp 1013 3 0",
                         "pydat3 kfdp 1013 4 0"        ]

                                                                
#  ------------- For how many events store B-chain  -------------
BSignalFilter.LVL1MuonCutOn = True
BSignalFilter.LVL1MuonCutPT = 6000.0
BSignalFilter.LVL1MuonCutEta = 2.5
BSignalFilter.LVL2MuonCutOn = True
BSignalFilter.LVL2ElectronCutOn = False
BSignalFilter.LVL2MuonCutPT = 4000.0
BSignalFilter.LVL2MuonCutEta = 2.5

BSignalFilter.Cuts_Final_hadrons_switch = False
BSignalFilter.Cuts_Final_hadrons_pT = 500.
BSignalFilter.Cuts_Final_hadrons_eta = 2.5
BSignalFilter.BParticle_cuts = 541

#EvtDecay.PdtTableDir = "MYinclusive.pdt" -- TODO FIXME -- mass of B_c, add excited states -- ktoms
EvtDecay.userDecayTableName = "MYDECAY_Bc_JPsi_Pi.DEC"

try:
  StreamEVGEN.RequireAlgs += [ "BSignalFilter" ]
except Exception, e:
  pass

#==============================================================
#
# End of job options file
#
###############################################################



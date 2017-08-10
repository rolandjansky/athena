###############################################################################
#
# MC11.108424.PythiaB_Bs_Jpsi_mu0mu0_phi_KK_nopTcuts.py
# Author: ATLAS B-physics group
# Generator of Bs -> phi(K+K-) J/psi(mu+mu-) decay
# PRODUCTION SYSTEM FRAGMENT
#
###############################################################################

#------------------------------------------------------------------------------
# Production driving parameters
#------------------------------------------------------------------------------

from MC11JobOptions.PythiaBEvgenConfig import evgenConfig
evgenConfig.minevents = 100

#------------------------------------------------------------------------------
# Import all needed algorithms (in the proper order)
#------------------------------------------------------------------------------

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg")

include( "MC11JobOptions/MC11_PythiaB_Common.py" )

#from GeneratorFilters.GeneratorFiltersConf import BSignalFilter
#topAlg += BSignalFilter()
#BSignalFilter = topAlg.BSignalFilter

#------------------------------------------------------------------------------
# PythiaB parameters settings
#------------------------------------------------------------------------------

PythiaB.ForceCDecay = "no"
PythiaB.ForceBDecay = "yes"

# Updated B-decays list (changes B-hadron decay tables including mdme line
# number to start from 4500 - see also ...CloseAntibQuarkNew.py below)
include( "MC11JobOptions/MC11_PythiaB_Bchannels.py" )

# Clasical PythiaB way of producing exclusive decay channel
# (close whole the decay and then open only the one requested channel)
include( "MC11JobOptions/MC11_PythiaB_CloseAntibQuarkNew.py" )

# Open channel for B0s -> phi J/psi and ban other than J/psi->mumu decays
PythiaB.PythiaCommand += [ "pydat3 mdme 4730 1 1",
                           "pydat3 mdme 858 1 0",
                           "pydat3 mdme 860 1 0" ]

# User-finsel to accept only charged phi daughters
PythiaB.ForceDecayChannel = "BsAngles"
# Parameter of the finsel: flat(0)/physics(1), pT_L1_mu, pT_L2_mu, eta_mu, kaon pT,eta, Bs pT,eta, (Bs lifetime)
PythiaB.DecayChannelParameters = [ 1.0, 0.0, 0.0, 100.0, 0.0, 100.0, 0.0, 100.0, 0.0, 0.0 ]

# Correct B0s lifetime as in PDG2010
PythiaB.PythiaCommand += ["pydat2 pmas 531 4 0.4599"]

# Production settings
include( "MC11JobOptions/MC11_PythiaB_Btune.py" )
PythiaB.PythiaCommand += [ "pysubs ckin 3 8.0",
                           "pysubs ckin 9 -3.0",
                           "pysubs ckin 10 3.0",
                           "pysubs ckin 11 -3.0",
                           "pysubs ckin 12 3.0",
                           "pysubs msel 1" ]

# Simulate only b-flavour events
PythiaB.flavour = 5.

# Pythia b-quark cuts
PythiaB.cutbq = [ "0. 102.5 and 6.0 3.5" ]

# Repeated hadronization
PythiaB.mhadr = 10.

#------------------------------------------------------------------------------
# Signal event filtering
#------------------------------------------------------------------------------

# LVL1: pT_L1, eta_L1
PythiaB.lvl1cut = [ 1., 0.0, 102.5 ]
# LVL2: pdg (muon/electron), pT_L2, eta_L2
PythiaB.lvl2cut = [ 0., 13., 0., 2.5 ]
# Offline: pT, eta cuts for kaon/pion, muon, electron
PythiaB.offcut = [ 0., 0.5, 2.5, 2.5, 2.5, 0.5, 2.5 ]

# Ntuple content settings
#BSignalFilter.SignaltoNtup = 10.0*float(evgenConfig.minevents)/float(evgenConfig.efficiency)
#BSignalFilter.StoreBQuarks = True

###############################################################################
#
# End of job options fragment for B0s -> phi(K+K-) J/psi(mu+mu-)
#
###############################################################################

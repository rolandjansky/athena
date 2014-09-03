from AthenaCommon.Resilience import treatException
    
# jobOption files for CBNT, truth specific part
# truth information

from CBNT_Truth.CBNTAA_TruthCfg import CBNTAA_Truth





#
# ----- CBNT_Truth subAlgorithm
#

theCBNTAA_Truth=CBNTAA_Truth(MaxNbParticles = 20000 ,  # maximum number of particles                            
                             MaxNbVertices = 20000 , # maximum number of vertices
                             PtMin = 5. * GeV, # defaut Pt cut
                             EtaMax= 2.55  ) # defaut eta cut

# Configurable selectors are added to select particle based on 
# their kinematic (overriding CBNTAA_Truth default), pdg_id or history.
# To be written in the ntuple a particle should satisfy at least one selector.
# Particles are written only once even if they are selected by different
# selectors. 
#
#
# add selectors
#
# All : take everything without selection (but keep disabled by default)

from CBNT_Truth.CBNT_TruthConf import CBNT_TruthSelector
theCBNTAA_Truth += CBNT_TruthSelector("All",
                                      PtMin = -1. * GeV,
                                      EtaMax = -1,
                                      OptionAll = True,
                                      Enable = False)  
#compat layer
All=theCBNTAA_Truth.All
# Disable this selector by default
#To enable this selector put the following line at the end of the jobOption
#All.Enable = True

#
#add a generator history selector
theCBNTAA_Truth += CBNT_TruthSelector("History",
                                      OptionMainEvent = False,   # switch off the default which is all particles except pile-up
                                      OptionHistoryGene = True,  # select history
                                      PtMin = -1. * GeV,    # no pt cut
                                      EtaMax = -1 )  # no etacut

# other boolean switches available beside OptionGeneHistory:
# OptionAll : take everything 
# OptionGene  :  generator particles
# OptionHistoryGene : generator history particles
# OptionFinalGene : final generator particle
# OptionGeant : particles tracked by geant (primaries and secondaries)
# OptionPrimaryGeant : primary particle tracked by geant
# OptionMainEvent : non pileup particles from generator or geant (default)
# OptionInteractedPileUp : pileup particles that have interacted in
#                          the detector
# OptionNonInteractedPileUp : pileup particles that have not
#                             interacted in the detector
# a particle need to satisfy at least one of the switches that is set
# these switched can be combined with the other properties, so that
# it is possible for example to select only primary electron

# add a charged particle selector: e, mu, pi, K, P within tracker acceptance
theCBNTAA_Truth += CBNT_TruthSelector("VisibleCharged",
                                      PDG_IDs = [ 11, 13, 211, 321, 2212], # list of particle ID (CC is implied)
                                      PtMin = 0.4 * GeV,
                                      EtaMax = 2.55)

#add a boson selector
theCBNTAA_Truth += CBNT_TruthSelector("Boson",
                                      PDG_IDs = [ 23, 24, 25, 32, 33, 34, 35, 36, 37 ],
                                      #take them all
                                      PtMin = -1. * GeV ,
                                      EtaMax = -1. )

#add a lepton (except electron) selector : muon tau, neutrino  selector
theCBNTAA_Truth += CBNT_TruthSelector("Lepton",
                                      PDG_IDs = [ 12, 13, 14, 15, 16],
                                      #take them all
                                      PtMin = -1. * GeV ,
                                      EtaMax = -1. )

#add an electron selector
theCBNTAA_Truth += CBNT_TruthSelector("Electron",
                                      PDG_IDs = [ 11 ],
                                      #take them all
                                      PtMin = 1. * GeV ,
                                      EtaMax = -1. )

# add a heavy quark selector
theCBNTAA_Truth += CBNT_TruthSelector("HeavyQuark",
                                      PDG_IDs = [ 5, 6],
                                      #take them all
                                      PtMin = -1. * GeV ,
                                      EtaMax = -1. )
#add a photon selector (5 GeV cut, otherwise many bremsstralung
theCBNTAA_Truth += CBNT_TruthSelector("Photon",
                                      PDG_IDs = [ 22 ],
                                      PtMin = 5. * GeV ,
                                      EtaMax = -1. )


# new tau selector with daughters
theCBNTAA_Truth += CBNT_TruthSelector("Tau",
                                      PDG_IDs = [ 15 ],
                                      #take them all
                                      PtMin = -1. * GeV ,
                                      EtaMax = -1. )

# daughters: the genealogy takes a selected list(s) as input
# and select all chidren, without any cuts
# only tau daughters selected, otherwise many many secondaries are selected
theCBNTAA_Truth += CBNT_TruthSelector("AllDaughters",
                                      InputNames = [ "Tau" ],
                                      PtMin = -1. * GeV, 
                                      EtaMax = -1., 
                                      Genealogy = -1 )# take all daughters recursively


#ancestors: the genealogy takes a selected list(s) as input
# and select all ancestors
theCBNTAA_Truth += CBNT_TruthSelector("AllAncestors",
                                      InputNames = [ "Boson","Electron","Lepton","Tau","HeavyQuark","Photon","VisibleCharged" ],
                                      PtMin = -1. * GeV ,
                                      EtaMax = -1 ,
                                      Genealogy = 1) # take all ancestors
                                      
# CAREFUL : if this fails one should make sure to include CBNT_Athena/CBNT_Athena_jobOptions.py
# finally add to CBNT_AthenaAware
CBNT_AthenaAware+=theCBNTAA_Truth



# add special MonteCarlo =>  dropped
# from CBNT_Truth.CBNT_TruthConf import CBNTAA_SpclMC
# CBNT_AthenaAware+=CBNTAA_SpclMC()

# spcial for HeavyIon simulation
# spcial for HeavyIon simulation

try:
    from HIRecExample.HIRecExampleFlags import jobproperties
    if jobproperties.HIRecExampleFlags.withHijingEventPars :
        from CBNT_Truth.CBNT_TruthConf import CBNTAA_HijingEventParams
        CBNT_AthenaAware+=CBNTAA_HijingEventParams()
except Exception:
    treatException("Could not configure CBNTAA_HijingEventParams")


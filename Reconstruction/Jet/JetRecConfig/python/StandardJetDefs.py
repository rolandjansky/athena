# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

########################################################################
#                                                                      #
# StandardJetDefs: A module defining standard definitions for jet      #
# constituents and jet algorithms.                                     #
# These can be copied and modified by users who want something a bit   #
# different from what is provided.                                     #
# Standard lists of ghost associated particles and modifiers are       #
# provided elsewhere.                                                  #
#                                                                      #
# Author: TJ Khoo                                                      #
# * Written for the time being to work in R21, but with R22 in mind    #
#                                                                      #
########################################################################


########################################################################
# Typical jet constituents
from JetDefinition import xAODType, JetConstit
from copy import deepcopy

# Topoclusters with origin correction.
# We don't currently use raw TopoClusters, though some handling
# could be added to support this.
EMTopoOrigin = JetConstit(xAODType.CaloCluster, ["EM","Origin"])
LCTopoOrigin = JetConstit(xAODType.CaloCluster, ["LC","Origin"])
# LC clusters with Constituent Subtraction + SoftKiller
LCTopoCSSK = deepcopy(LCTopoOrigin)
LCTopoCSSK.modifiers += ["CS","SK"]

# EM-scale particle flow objects with charged hadron subtraction
# For now we don't specify a scale, as only one works well, but
# this could be incorporated into the naming scheme and config
CHSPFlow = JetConstit(xAODType.ParticleFlow)
# Particle Flow Objects with Constituent Subtraction + SoftKiller
CSSKPFlow = deepcopy(CHSPFlow)
CSSKPFlow.modifiers += ["CS","SK"]

# Track particles from the primary vertex
PV0Track = JetConstit(xAODType.TrackParticle,["PV0"])

# Truth particles from the hard scatter vertex prior to Geant4 simulation.
# Neutrinos and muons are omitted; all other stable particles are included.
Truth = JetConstit(xAODType.TruthParticle)

# Truth particles from the hard scatter vertex prior to Geant4 simulation.
# Prompt electrons, muons and neutrinos are excluded, all other stable particles
# are included, in particular leptons and neutrinos from hadron decays.
TruthWZ = JetConstit(xAODType.TruthParticle,["NoWZ"])

########################################################################
# Typical jet algorithm definitions
# Filter pts, ghost lists and modifier lists can be updated by the user,
# though there is some risk of overwriting...
from JetDefinition import JetDefinition

# Standard small-radius (0.4) AntiKt jets
AntiKt4LCTopo = JetDefinition("AntiKt",0.4,LCTopoOrigin)
AntiKt4EMTopo = JetDefinition("AntiKt",0.4,EMTopoOrigin)
AntiKt4EMPFlow = JetDefinition("AntiKt",0.4,CHSPFlow)

# Standard large-radius (1.0) AntiKt ungroomed jets
AntiKt10LCTopo = JetDefinition("AntiKt",1.0,LCTopoOrigin)
AntiKt10LCTopoCSSK = JetDefinition("AntiKt",1.0,LCTopoCSSK)

# Standard truth jets
AntiKt4Truth = JetDefinition("AntiKt",0.4,Truth)
AntiKt4TruthWZ = JetDefinition("AntiKt",0.4,TruthWZ)
AntiKt10Truth = JetDefinition("AntiKt",1.0,Truth)
AntiKt10TruthWZ = JetDefinition("AntiKt",1.0,TruthWZ)

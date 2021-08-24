# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# this is just a thin wrapper on ParticleJetTools, I want to
# keep the scope of the MR where we moved this limited. Eventually I
# should move the things that point to this to point to the library
# below.

from ParticleJetTools.JetParticleAssociationAlgConfig import ( # noqa: F401
    JetParticleAssociationAlgCfg)                              # noqa: F401

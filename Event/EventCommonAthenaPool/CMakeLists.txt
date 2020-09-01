# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventCommonAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( EventCommonAthenaPoolPoolCnv
                           src/*.cxx
                           FILES EventKernel/INavigable4Momentum.h NavFourMom/INav4MomLinkContainer.h NavFourMom/IParticleLinkContainer.h NavFourMom/I4MomentumContainer.h NavFourMom/INavigable4MomentumCollection.h NavFourMom/IParticleContainer.h NavFourMom/INav4MomAssocs.h
                           LINK_LIBRARIES AthenaPoolCnvSvcLib DataModelAthenaPoolLib EventCommonTPCnv EventKernel GaudiKernel NavFourMom StoreGateLib )

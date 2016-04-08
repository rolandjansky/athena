################################################################################
# Package: EventCommonAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( EventCommonAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Control/DataModelAthenaPool
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Event/EventCommonTPCnv
                          Event/EventKernel
                          Event/NavFourMom
                          GaudiKernel )

# Component(s) in the package:
atlas_add_poolcnv_library( EventCommonAthenaPoolPoolCnv
                           src/*.cxx
                           FILES EventKernel/INavigable4Momentum.h NavFourMom/INav4MomLinkContainer.h NavFourMom/IParticleLinkContainer.h NavFourMom/I4MomentumContainer.h NavFourMom/INavigable4MomentumCollection.h NavFourMom/IParticleContainer.h NavFourMom/INav4MomAssocs.h
                           LINK_LIBRARIES AthenaPoolUtilities DataModelAthenaPoolLib StoreGateLib SGtests AthenaPoolCnvSvcLib EventCommonTPCnv EventKernel NavFourMom GaudiKernel )

# Install files from the package:
atlas_install_headers( EventCommonAthenaPool )


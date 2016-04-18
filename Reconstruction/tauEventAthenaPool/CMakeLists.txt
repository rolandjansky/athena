################################################################################
# Package: tauEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( tauEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          Reconstruction/tauEvent
                          Reconstruction/tauEventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( tauEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES tauEvent/TauJetContainer.h tauEvent/TauDetailsContainer.h
                           TYPES_WITH_NAMESPACE Analysis::TauJetContainer Analysis::TauDetailsContainer
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib GaudiKernel tauEvent tauEventTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )


################################################################################
# Package: TrackParticleTruthAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( TrackParticleTruthAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Reconstruction/ParticleTruth
                          Reconstruction/TrackParticleTruthTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( TrackParticleTruthAthenaPoolPoolCnv
                           src/*.cxx
                           FILES ParticleTruth/TrackParticleTruthCollection.h ParticleTruth/TrackParticleTruthCollectionContainer.h
                           LINK_LIBRARIES AthenaPoolUtilities GaudiKernel StoreGateLib SGtests AthenaPoolCnvSvcLib ParticleTruth TrackParticleTruthTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )


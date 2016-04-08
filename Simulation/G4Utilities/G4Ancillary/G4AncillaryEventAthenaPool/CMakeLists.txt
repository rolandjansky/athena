################################################################################
# Package: G4AncillaryEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( G4AncillaryEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Simulation/G4Utilities/G4Ancillary/G4AncillarySimEvent )

# Component(s) in the package:
atlas_add_poolcnv_library( G4AncillaryEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES G4AncillarySimEvent/ScintillatorHitCollection.h G4AncillarySimEvent/SimpleScintillatorHitCollection.h
                           LINK_LIBRARIES AthenaPoolUtilities AtlasSealCLHEP AthenaPoolCnvSvcLib G4AncillarySimEvent )


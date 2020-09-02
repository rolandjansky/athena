################################################################################
# Package: G4AncillaryEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( G4AncillaryEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( G4AncillaryEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES G4AncillarySimEvent/ScintillatorHitCollection.h G4AncillarySimEvent/SimpleScintillatorHitCollection.h
                           LINK_LIBRARIES AthenaPoolUtilities AtlasSealCLHEP AthenaPoolCnvSvcLib G4AncillarySimEvent )


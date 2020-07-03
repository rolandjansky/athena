################################################################################
# Package: G4SimAthenaPOOL
################################################################################

# Declare the package name:
atlas_subdir( G4SimAthenaPOOL )

# Component(s) in the package:
atlas_add_poolcnv_library( G4SimAthenaPOOLPoolCnv
                           src/*.cxx
                           FILES TrackRecord/TrackRecordCollection.h
                           LINK_LIBRARIES AthenaPoolUtilities AtlasSealCLHEP GaudiKernel AthenaPoolCnvSvcLib G4SimTPCnv )

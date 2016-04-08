################################################################################
# Package: G4SimAthenaPOOL
################################################################################

# Declare the package name:
atlas_subdir( G4SimAthenaPOOL )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          GaudiKernel
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Simulation/G4Sim/TrackRecord
                          Simulation/G4SimCnv/G4SimTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( G4SimAthenaPOOLPoolCnv
                           src/*.cxx
                           FILES TrackRecord/TrackRecordCollection.h
                           LINK_LIBRARIES AthenaPoolUtilities AtlasSealCLHEP GaudiKernel AthenaPoolCnvSvcLib G4SimTPCnv )

# Install files from the package:
atlas_install_headers( G4SimAthenaPOOL )


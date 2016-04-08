################################################################################
# Package: TBDetDescrAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( TBDetDescrAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          TestBeam/TBDetDescr )

# Component(s) in the package:
atlas_add_poolcnv_library( TBDetDescrAthenaPoolPoolCnv
                           src/*.cxx
                           FILES TBDetDescr/TBElementContainer.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib TBDetDescr )


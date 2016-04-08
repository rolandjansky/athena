################################################################################
# Package: ByteStreamEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( ByteStreamEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Event/ByteStreamData
                          Event/ByteStreamEventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( ByteStreamEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES ByteStreamData/ByteStreamMetadataContainer.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib ByteStreamData ByteStreamData_test ByteStreamEventTPCnv )


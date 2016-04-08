################################################################################
# Package: InDetSimEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( InDetSimEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          AtlasTest/TestTools
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          GaudiKernel
                          InnerDetector/InDetEventCnv/InDetSimEventTPCnv
                          InnerDetector/InDetSimEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_poolcnv_library( InDetSimEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES InDetSimEvent/SiHitCollection.h InDetSimEvent/TRTUncompressedHitCollection.h
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities AtlasSealCLHEP GaudiKernel InDetSimEventTPCnv InDetSimEvent )

atlas_add_dictionary( InDetSimEventAthenaPoolCnvDict
                      InDetSimEventAthenaPool/InDetSimEventAthenaPoolCnvDict.h
                      InDetSimEventAthenaPool/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities AtlasSealCLHEP GaudiKernel InDetSimEventTPCnv InDetSimEvent )

# Install files from the package:
atlas_install_headers( InDetSimEventAthenaPool )
atlas_install_joboptions( share/*.py )


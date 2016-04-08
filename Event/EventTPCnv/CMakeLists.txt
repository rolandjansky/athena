################################################################################
# Package: EventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( EventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Event/EventInfo
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( EventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS EventTPCnv
                         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib EventInfo TestTools AthenaKernel )

atlas_add_dictionary( EventTPCnvDict
                      EventTPCnv/EventTPCnvDict.h
                      EventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib EventInfo TestTools AthenaKernel EventTPCnv )

atlas_add_dictionary( OLD_EventTPCnvDict
                      EventTPCnv/EventTPCnvDict.h
                      EventTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib EventInfo TestTools AthenaKernel EventTPCnv )

atlas_add_test( vectorize_test
                SOURCES
                test/vectorize_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib EventInfo TestTools AthenaKernel EventTPCnv )


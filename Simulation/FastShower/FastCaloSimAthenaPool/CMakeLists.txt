################################################################################
# Package: FastCaloSimAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( FastCaloSimAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( FastCaloSimAthenaPool
                   src/*.cxx
                   PUBLIC_HEADERS FastCaloSimAthenaPool
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools AthenaPoolUtilities AthenaPoolCnvSvcLib )

atlas_add_poolcnv_library( FastCaloSimAthenaPoolPoolCnv
                           src/*.cxx
                           FILES FastCaloSimAthenaPool/FastShowerInfoContainer.h FastCaloSimAthenaPool/FastShowerInfo.h
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools AthenaPoolUtilities AthenaPoolCnvSvcLib FastCaloSimAthenaPool )

atlas_add_dictionary( FastCaloSimAthenaPoolDict
                      FastCaloSimAthenaPool/FastCaloSimAthenaPoolDict.h
                      FastCaloSimAthenaPool/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools AthenaPoolUtilities AthenaPoolCnvSvcLib FastCaloSimAthenaPool )


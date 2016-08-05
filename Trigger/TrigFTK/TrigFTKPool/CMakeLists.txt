################################################################################
# Package: TrigFTKPool
################################################################################

# Declare the package name:
atlas_subdir( TrigFTKPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities )

# External dependencies:
find_package( ROOT COMPONENTS MathCore Core Tree Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigFTKPool
                   src/*.cxx
                   PUBLIC_HEADERS TrigFTKPool
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolUtilities )

atlas_add_poolcnv_library( TrigFTKPoolPoolCnv
                           src/*.cxx
                           FILES TrigFTKPool/FTKTestData.h TrigFTKPool/FTKAthTrackContainer.h TrigFTKPool/FTKTrackFitterStats.h
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools AthenaPoolCnvSvcLib AthenaPoolUtilities TrigFTKPool )

atlas_add_dictionary( TrigFTKPoolDict
                      TrigFTKPool/TrigFTKPoolDict.h
                      TrigFTKPool/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools AthenaPoolCnvSvcLib AthenaPoolUtilities TrigFTKPool )


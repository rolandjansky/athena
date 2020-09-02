################################################################################
# Package: AthenaPoolExampleConverter
################################################################################

# Declare the package name:
atlas_subdir( AthenaPoolExampleConverter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModelAthenaPool
                          Control/CxxUtils
                          Database/AthenaPOOL/AthenaPoolExample/AthenaPoolExampleData
                          Database/AthenaPOOL/AthenaPoolServices
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AthenaPoolExampleConverter
                   src/*.cxx
                   PUBLIC_HEADERS AthenaPoolExampleConverter
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaPoolExampleData AthenaPoolUtilities DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolServicesLib )

atlas_add_poolcnv_library( AthenaPoolExampleConverterPoolCnv
                           FILES AthenaPoolExampleData/ExampleHitContainer.h AthenaPoolExampleData/ExampleTrackContainer.h
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolExampleData AthenaPoolUtilities AthenaPoolCnvSvcLib AthenaPoolExampleConverter )

atlas_add_dictionary( AthenaPoolExampleConverterDict
                      AthenaPoolExampleConverter/AthenaPoolExampleConverterDict.h
                      AthenaPoolExampleConverter/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModelAthenaPoolLib AthenaPoolExampleData AthenaPoolUtilities AthenaPoolCnvSvcLib AthenaPoolExampleConverter )

# Install files from the package:
atlas_install_joboptions( share/*.py )


################################################################################
# Package: LumiBlockComps
################################################################################

# Declare the package name:
atlas_subdir( LumiBlockComps )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Control/xAODRootAccess
                          Database/AthenaPOOL/AthenaPoolKernel
                          Database/CoolLumiUtilities
                          Event/xAOD/xAODLuminosity
                          GaudiKernel
                          LumiBlock/LumiCalc
                          PRIVATE
                          DataQuality/GoodRunsLists
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AthenaPOOL/DBDataModel
                          Database/CoraCool
                          Event/EventInfo )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( LumiBlockCompsLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS LumiBlockComps
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel xAODLuminosity GaudiKernel StoreGateLib SGtests CoolLumiUtilitiesLib LumiBlockCoolQuery GoodRunsListsLib
                   PRIVATE_LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} AthenaPoolUtilities DBDataModel EventInfo )

atlas_add_component( LumiBlockComps
                     src/components/*.cxx
                     INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests CoolLumiUtilitiesLib xAODLuminosity GaudiKernel LumiBlockCoolQuery GoodRunsListsLib AthenaPoolUtilities DBDataModel EventInfo LumiBlockCompsLib )

atlas_add_dictionary( LumiBlockCompsDict
                      LumiBlockComps/LumiBlockCompsDict.h
                      LumiBlockComps/selection.xml
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests CoolLumiUtilitiesLib xAODLuminosity GaudiKernel LumiBlockCoolQuery GoodRunsListsLib AthenaPoolUtilities DBDataModel EventInfo LumiBlockCompsLib )

atlas_add_executable( getLumi
                      util/getLumi.cxx
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests CoolLumiUtilitiesLib xAODLuminosity GaudiKernel LumiBlockCoolQuery GoodRunsListsLib AthenaPoolUtilities DBDataModel EventInfo LumiBlockCompsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )



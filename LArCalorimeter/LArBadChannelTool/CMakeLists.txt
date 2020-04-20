################################################################################
# Package: LArBadChannelTool
################################################################################

# Declare the package name:
atlas_subdir( LArBadChannelTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloConditions
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArCabling )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( LArBadChannelToolLib
                   src/*.cxx
                   PUBLIC_HEADERS LArBadChannelTool
                   INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} CaloConditions CaloIdentifier AthenaBaseComps AthenaKernel AthenaPoolUtilities Identifier GaudiKernel LArIdentifier LArRecConditions StoreGateLib SGtests LArCablingLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_component( LArBadChannelTool
                     src/components/*.cxx
                     INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES LArBadChannelToolLib )

atlas_add_dictionary( BadChanDict
                      LArBadChannelTool/LArBadChannelDBToolsDict.h
                      LArBadChannelTool/selection.xml
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} CaloConditions CaloIdentifier AthenaBaseComps AthenaKernel StoreGateLib SGtests AthenaPoolUtilities Identifier GaudiKernel LArIdentifier LArRecConditions LArCablingLib LArBadChannelToolLib )


atlas_add_test( LArBadChannelConfigTest
   SCRIPT python -m LArBadChannelTool.LArBadChannelConfig
   POST_EXEC_SCRIPT nopost.sh )


# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/LArBuildBadChannelDB.sh share/LArBuildMissingFebDB.sh )

atlas_add_test( flake8
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( flake8_share
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 --ignore=F401,F821,ATL900 ${CMAKE_CURRENT_SOURCE_DIR}/share
                POST_EXEC_SCRIPT nopost.sh )

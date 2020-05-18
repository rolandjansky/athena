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
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
atlas_install_scripts( share/LArBuildBadChannelDB.sh share/LArBuildMissingFebDB.sh )

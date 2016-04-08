################################################################################
# Package: CoolLumiUtilities
################################################################################

# Declare the package name:
atlas_subdir( CoolLumiUtilities )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          GaudiKernel
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( CoolLumiUtilitiesLib
                   src/*.cxx
                   PUBLIC_HEADERS CoolLumiUtilities
                   INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolUtilities )

atlas_add_component( CoolLumiUtilities
                     src/components/*.cxx
                     INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel AthenaPoolUtilities CoolLumiUtilitiesLib )

atlas_add_dictionary( CoolLumiUtilitiesDict
                      CoolLumiUtilities/CoolLumiUtilitiesDict.h
                      CoolLumiUtilities/selection.xml
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel AthenaPoolUtilities CoolLumiUtilitiesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/*.py )


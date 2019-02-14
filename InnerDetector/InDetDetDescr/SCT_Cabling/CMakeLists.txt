################################################################################
# Package: SCT_Cabling
################################################################################

# Declare the package name:
atlas_subdir( SCT_Cabling )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/Identifier
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/CoraCool
                          Event/EventInfo
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Tools/PathResolver )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( SCT_CablingLib
                   src/*.cxx
                   PUBLIC_HEADERS SCT_Cabling
                   PRIVATE_INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps Identifier GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaPoolUtilities CoraCool EventInfo InDetIdentifier PathResolver )

atlas_add_component( SCT_Cabling
                     src/components/*.cxx
                     INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps Identifier GaudiKernel StoreGateLib SGtests AthenaPoolUtilities CoraCool EventInfo InDetIdentifier PathResolver SCT_CablingLib )

atlas_add_test( TestSCT_CablingCfg
                SCRIPT share/TestSCT_CablingCfg.sh
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.dat )
atlas_install_scripts( share/*.sh )
atlas_install_python_modules( python/*.py )

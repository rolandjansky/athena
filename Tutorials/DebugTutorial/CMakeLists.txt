################################################################################
# Package: DebugTutorial
################################################################################

# Declare the package name:
atlas_subdir( DebugTutorial )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/StoreGate
                          Control/AthenaBaseComps
                          Event/xAOD/xAODJet )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist )
find_package( CLHEP )


atlas_add_component( DebugTutorial
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES AthContainers StoreGateLib AthenaBaseComps
                                    xAODJet
                                    ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} )


# Install files from the package:
atlas_install_joboptions( share/*.py )


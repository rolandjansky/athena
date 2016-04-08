################################################################################
# Package: CommissionUtils
################################################################################

# Declare the package name:
atlas_subdir( CommissionUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          PRIVATE
                          Commission/CommissionEvent
                          LArCalorimeter/LArSimEvent
                          Simulation/G4Sim/TrackRecord )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( CommissionUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS CommissionUtils
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} CommissionEvent LArSimEvent )

atlas_add_component( CommissionUtils
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel CommissionEvent LArSimEvent CommissionUtilsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )


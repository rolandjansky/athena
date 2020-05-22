################################################################################
# Package: CommissionRec
################################################################################

# Declare the package name:
atlas_subdir( CommissionRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Commission/CommissionUtils
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Commission/CommissionEvent
                          Simulation/G4Sim/TrackRecord )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( CommissionRec
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} CommissionUtilsLib AthenaBaseComps GaudiKernel CommissionEvent TrackRecordLib )

# Install files from the package:
atlas_install_headers( CommissionRec )


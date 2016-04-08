################################################################################
# Package: ForwardRegionMgField
################################################################################

# Declare the package name:
atlas_subdir( ForwardRegionMgField )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Simulation/G4Sim/FADS/FadsField
                          PRIVATE
                          Control/AthenaKernel
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( ForwardRegionMgField
                   src/*.cxx
                   PUBLIC_HEADERS ForwardRegionMgField
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel FadsField
                   PRIVATE_LINK_LIBRARIES AthenaKernel PathResolver )

# Install files from the package:
atlas_install_runtime( share/*.dat )


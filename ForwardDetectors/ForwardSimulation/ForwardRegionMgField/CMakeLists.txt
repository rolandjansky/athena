################################################################################
# Package: ForwardRegionMgField
################################################################################

# Declare the package name:
atlas_subdir( ForwardRegionMgField )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          GaudiKernel
                          Control/AthenaBaseComps
                          MagneticField/MagFieldInterfaces
                          ForwardDetectors/ForwardSimulation/ForwardRegionGeoModel
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( ForwardRegionMgField
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel AthenaBaseComps MagFieldInterfaces PathResolver ForwardRegionGeoModelLib )

# Install files from the package:
atlas_install_runtime( share/*.dat )
atlas_install_python_modules( python/*.py )

################################################################################
# Package: ISF_Geant4CommonServices
################################################################################

# Declare the package name:
atlas_subdir( ISF_Geant4CommonServices )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          AtlasGeometryCommon/SubDetectorEnvelopes
                          Control/AthenaBaseComps
                          DetectorDescription/AtlasDetDescr
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( ISF_Geant4CommonServices
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel AthenaBaseComps AtlasDetDescr ISF_Event ISF_Interfaces )

# Install files from the package:
atlas_install_headers( ISF_Geant4CommonServices )
atlas_install_python_modules( python/*.py )


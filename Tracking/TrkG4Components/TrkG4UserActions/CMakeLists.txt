################################################################################
# Package: TrkG4UserActions
################################################################################

# Declare the package name:
atlas_subdir( TrkG4UserActions )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Atlas/G4AtlasTools
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          Control/CxxUtils
                          DetectorDescription/GeoPrimitives
                          Simulation/G4Sim/SimHelpers
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          Tracking/TrkValidation/TrkValInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )
find_package( Boost )

# Component(s) in the package:
atlas_add_component( TrkG4UserActions
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel G4AtlasInterfaces G4AtlasToolsLib TrkDetDescrInterfaces TrkGeometry TrkMaterialOnTrack TrkParameters CxxUtils StoreGateLib SGtests GeoPrimitives SimHelpers TrkSurfaces TrkExInterfaces TrkExUtils TrkValInterfaces )

# Install files from the package:
atlas_install_headers( TrkG4UserActions )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )


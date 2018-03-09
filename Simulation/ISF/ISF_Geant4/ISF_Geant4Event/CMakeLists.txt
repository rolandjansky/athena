################################################################################
# Package: ISF_Geant4Event
################################################################################

# Declare the package name:
atlas_subdir( ISF_Geant4Event )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Simulation/G4Sim/SimHelpers
                          Simulation/Barcode/BarcodeEvent
                          Simulation/G4Sim/MCTruth
                          PRIVATE
                          DetectorDescription/GeoPrimitives )


        
# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( Geant4 )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( ISF_Geant4Event
                   src/*.cxx
                   PUBLIC_HEADERS ISF_Geant4Event
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   ${HEPMC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS}
                   ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES}
                   ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES}
                   ${GEANT4_LIBRARIES} ${EIGEN_LIBRARIES} ISF_Event
                   SimHelpers GeoPrimitives MCTruth)

#atlas_add_test(SOURCES
#               INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
#              ${HEPMC_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
#               LINK_LIBRARIES
#              ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES}
#               ${EIGEN_LIBRARIES} ISF_Event SimHelpers GeoPrimitives
#               BarcodeEvent MCTruth)

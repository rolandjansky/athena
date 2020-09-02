################################################################################
# Package: TestLArDetDescr
################################################################################

# Declare the package name:
atlas_subdir( TestLArDetDescr )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloTTDetDescr
                          Calorimeter/CaloTrackingGeometry
                          Control/StoreGate
                          LArCalorimeter/LArIdentifier )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TestLArDetDescr
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} CaloDetDescrLib CaloIdentifier AthenaBaseComps GaudiKernel CaloGeoHelpers CaloTTDetDescr StoreGateLib SGtests LArIdentifier CaloTrackingGeometryLib )

# Install files from the package:
atlas_install_headers( TestLArDetDescr )
atlas_install_joboptions( share/*.py )


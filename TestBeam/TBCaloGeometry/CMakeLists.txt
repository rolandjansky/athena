################################################################################
# Package: TBCaloGeometry
################################################################################

# Declare the package name:
atlas_subdir( TBCaloGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloDetDescr
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloGeoHelpers
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          Event/EventInfo
                          TestBeam/TBConditions/TBCaloConditions
                          TestBeam/TBDetDescr )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TBCaloGeometryLib
                   src/*.cxx
                   PUBLIC_HEADERS TBCaloGeometry
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel CaloDetDescrLib StoreGateLib SGtests TBCaloConditionsLib
                   PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} CaloGeoHelpers GeoPrimitives EventInfo TBDetDescr )

atlas_add_component( TBCaloGeometry
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} CaloDetDescrLib AthenaBaseComps GaudiKernel CaloGeoHelpers StoreGateLib SGtests GeoPrimitives EventInfo TBCaloConditionsLib TBDetDescr TBCaloGeometryLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )


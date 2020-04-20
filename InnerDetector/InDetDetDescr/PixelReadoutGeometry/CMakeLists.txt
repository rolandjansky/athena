################################################################################
# Package: InDetReadoutGeometry
################################################################################

# Declare the package name:
atlas_subdir( PixelReadoutGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/GeoModel/GeoModelUtilities
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetCondTools
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/IdDictDetDescr)

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:

atlas_add_library( PixelReadoutGeometry
  src/*.c*
  PUBLIC_HEADERS PixelReadoutGeometry
  INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
  DEFINITIONS ${CLHEP_DEFINITIONS}
  LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} InDetReadoutGeometry AthenaKernel SGTools AtlasDetDescr GeoPrimitives Identifier GaudiKernel InDetIdentifier TrkDetElementBase TrkSurfaces TrkEventPrimitives StoreGateLib SGtests
  PRIVATE_LINK_LIBRARIES AthenaPoolUtilities DetDescrConditions IdDictDetDescr)


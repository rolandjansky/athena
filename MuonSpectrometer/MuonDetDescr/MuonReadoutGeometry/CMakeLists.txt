################################################################################
# Package: MuonReadoutGeometry
################################################################################

# Declare the package name:
atlas_subdir( MuonReadoutGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonAlignment/MuonAlignmentData
                          MuonSpectrometer/MuonIdHelpers
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkDistortedSurfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/AGDD/AGDDModel
                          MuonSpectrometer/MuonDetDescr/MuonAGDDDescription )

# External dependencies:
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( MuonReadoutGeometry
                   src/*.c*
                   PUBLIC_HEADERS MuonReadoutGeometry
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives Identifier GaudiKernel MuonAlignmentData TrkDetElementBase TrkDistortedSurfaces TrkSurfaces MuonIdHelpersLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AGDDModel MuonAGDDDescription )


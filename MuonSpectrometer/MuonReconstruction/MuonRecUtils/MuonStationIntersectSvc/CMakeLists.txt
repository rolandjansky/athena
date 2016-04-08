################################################################################
# Package: MuonStationIntersectSvc
################################################################################

# Declare the package name:
atlas_subdir( MuonStationIntersectSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonIdHelpers
                          PRIVATE
                          Control/StoreGate
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          Tracking/TrkUtilityPackages/TrkDriftCircleMath )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MuonStationIntersectSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonStationIntersectSvc
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives Identifier GaudiKernel MuonIdHelpersLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES MuonCondInterface MuonReadoutGeometry TrkDriftCircleMath )

atlas_add_component( MuonStationIntersectSvc
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives Identifier GaudiKernel MuonIdHelpersLib StoreGateLib SGtests MuonCondInterface MuonReadoutGeometry TrkDriftCircleMath MuonStationIntersectSvcLib )


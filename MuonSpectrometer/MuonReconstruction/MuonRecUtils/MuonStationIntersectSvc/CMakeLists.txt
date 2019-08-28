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
                          DetectorDescription/GeoModel/GeoModelUtilities
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          Tracking/TrkUtilityPackages/TrkDriftCircleMath
			  MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondData )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MuonStationIntersectSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonStationIntersectSvc
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives Identifier GaudiKernel MuonIdHelpersLib StoreGateLib SGtests GeoModelUtilities
                   PRIVATE_LINK_LIBRARIES MuonReadoutGeometry TrkDriftCircleMath MuonCondData )

atlas_add_component( MuonStationIntersectSvc
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives Identifier GaudiKernel MuonIdHelpersLib StoreGateLib SGtests MuonReadoutGeometry TrkDriftCircleMath MuonCondData MuonStationIntersectSvcLib )

atlas_install_python_modules( python/*.py )

#atlas_add_test( MdtIntersectGeometry_test
#  SCRIPT python -m MuonStationIntersectSvc.MdtIntersectGeometry_test
#  PROPERTIES TIMEOUT 600
#  EXTRA_PATTERNS "-s dead tube" )

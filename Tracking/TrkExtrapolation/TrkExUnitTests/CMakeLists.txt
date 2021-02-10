################################################################################
# Package: TrkExUnitTests
################################################################################

# Declare the package name:
atlas_subdir( TrkExUnitTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          GaudiKernel
                          HighGranularityTimingDetector/HGTD_DetDescr/HGTD_Identifier
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          Tracking/TrkValidation/TrkValInterfaces )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# tag use_new_tracking_edm was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( TrkExUnitTests
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AtlasDetDescr GeoPrimitives Identifier HGTD_Identifier InDetReadoutGeometry GaudiKernel InDetIdentifier TrkGeometry TrkEventPrimitives TrkNeutralParameters TrkParameters TrkExInterfaces TrkExUtils TrkValInterfaces )

# Install files from the package:
atlas_install_headers( TrkExUnitTests )


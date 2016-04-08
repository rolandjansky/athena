################################################################################
# Package: TrkDetDescrUnitTests
################################################################################

# Declare the package name:
atlas_subdir( TrkDetDescrUnitTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkDetDescr/TrkVolumes
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkUtilityPackages/TrkValidationUtils )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# tag use_new_tracking_edm was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( TrkDetDescrUnitTests
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel TrkDetDescrInterfaces GeoPrimitives TrkDetDescrUtils TrkGeometry TrkSurfaces TrkVolumes TrkNeutralParameters TrkValidationUtils )

# Install files from the package:
atlas_install_headers( TrkDetDescrUnitTests )


################################################################################
# Package: MuonCondInterface
################################################################################

# Declare the package name:
atlas_subdir( MuonCondInterface )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          MuonSpectrometer/MuonAlignment/MuonAlignmentData )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MuonCondInterface
                   PUBLIC_HEADERS MuonCondInterface
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaKernel GeoPrimitives GaudiKernel MuonAlignmentData StoreGateLib SGtests )


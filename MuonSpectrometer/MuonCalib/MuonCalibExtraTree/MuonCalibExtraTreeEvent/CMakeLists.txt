################################################################################
# Package: MuonCalibExtraTreeEvent
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibExtraTreeEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          MuonSpectrometer/MuonCalib/MuonCalibEventBase
                          MuonSpectrometer/MuonCalib/MuonCalibExtraTree/MuonCalibExtraUtils
                          MuonSpectrometer/MuonCalib/MuonCalibIdentifier
                          PRIVATE
                          MuonSpectrometer/MuonCalib/MuonCalibUtils/MuonCalibStl )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MuonCalibExtraTreeEvent
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibExtraTreeEvent
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives MuonCalibEventBase MuonCalibExtraUtils MuonCalibIdentifier )


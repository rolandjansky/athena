################################################################################
# Package: MuonCalibEvent
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          DetectorDescription/GeoPrimitives
                          MuonSpectrometer/MuonCalib/MuonCalibEventBase
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          PRIVATE
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/MuonCalibITools )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MuonCalibEvent
                   src/MdtCalibHit.cxx
                   PUBLIC_HEADERS MuonCalibEvent
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} DataModel SGTools GeoPrimitives MuonCalibEventBase MuonPrepRawData
                   PRIVATE_LINK_LIBRARIES GaudiKernel MuonCalibITools )


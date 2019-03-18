################################################################################
# Package: MuonAlignErrorTool
################################################################################

# Declare the package name:
atlas_subdir( MuonAlignErrorTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          MuonSpectrometer/MuonAlignment/MuonAlignError/MuonAlignErrorBase
			  MuonSpectrometer/MuonCalib/MuonCalibITools
			  MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondSvc
                          MuonSpectrometer/MuonIdHelpers
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Control/StoreGate
                          Tools/PathResolver
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )
find_package( cx_Oracle )

# Component(s) in the package:
atlas_add_component( MuonAlignErrorTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel MuonAlignErrorBase MuonCalibITools MuonCondSvc MuonIdHelpersLib TrkToolInterfaces StoreGateLib SGtests PathResolver TrkCompetingRIOsOnTrack TrkPrepRawData TrkRIO_OnTrack TrkTrack )

# Install files from the package:
atlas_install_headers( MuonAlignErrorTool )
atlas_install_runtime( data/alignment_error_blob_2015.txt data/alignment_error_blob_2016.txt data/alignment_error_blob_2017_start.txt )


################################################################################
# Package: MdtCalibRt
################################################################################

# Declare the package name:
atlas_subdir( MdtCalibRt )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibData
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibFitters
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibInterfaces
                          MuonSpectrometer/MuonCalib/MuonCalibEventBase
                          MuonSpectrometer/MuonCalib/MuonCalibUtils/MuonCalibMath
                          PRIVATE
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibT0 )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint RooFitCore RooFit Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( MdtCalibRt
                   src/*.cxx
                   PUBLIC_HEADERS MdtCalibRt
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives MdtCalibData MdtCalibFitters MuonCalibEventBase MuonCalibMath
                   PRIVATE_LINK_LIBRARIES MdtCalibT0 )


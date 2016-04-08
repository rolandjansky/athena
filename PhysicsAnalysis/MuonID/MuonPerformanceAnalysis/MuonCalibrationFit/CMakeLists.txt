################################################################################
# Package: MuonCalibrationFit
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibrationFit )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS MathCore RIO Matrix Physics RooFit RooFitCore Core Tree Hist pthread MathMore Minuit Minuit2 HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )
find_package( TBB )

# Component(s) in the package:
atlas_add_component( MuonCalibrationFit
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${TBB_LIBRARIES} AthenaBaseComps GaudiKernel )

# Install files from the package:
atlas_install_headers( MuonCalibrationFit )
atlas_install_joboptions( share/*.py )


################################################################################
# Package: ALFA_Raw2Digit
################################################################################

# Declare the package name:
atlas_subdir( ALFA_Raw2Digit )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          ForwardDetectors/ALFA/ALFA_RawEv
                          GaudiKernel
                          PRIVATE
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Table MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( ALFA_Raw2Digit
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps ALFA_RawEv GaudiKernel PathResolver )

# Install files from the package:
atlas_install_headers( ALFA_Raw2Digit )
atlas_install_runtime( mapping/*.dat )


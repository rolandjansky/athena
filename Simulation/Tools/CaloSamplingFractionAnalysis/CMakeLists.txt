################################################################################
# Package: CaloSamplingFractionAnalysis
################################################################################

# Declare the package name:
atlas_subdir( CaloSamplingFractionAnalysis )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Table MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( CaloSamplingFractionAnalysis
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel CaloDetDescrLib CaloIdentifier CaloSimEvent CaloEvent AthenaBaseComps GeoAdaptors EventInfo LArSimEvent LArG4RunControl LArReadoutGeometry TileDetDescr TileSimEvent )

# Install files from the package:
# atlas_install_headers( CaloSamplingFractionAnalysis )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.C )


################################################################################
# Package: MM_Digitization
################################################################################

# Declare the package name:
atlas_subdir( MM_Digitization )

# External dependencies:
find_package( AIDA )
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Table MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( MM_DigitizationLib
                   src/*.cxx
                   PUBLIC_HEADERS MM_Digitization
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${AIDA_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives Identifier GaudiKernel MagFieldInterfaces MuonSimEvent HitManagement PileUpToolsLib StoreGateLib SGtests MuonIdHelpersLib NSWCalibToolsLib MagFieldElements MagFieldConditions MuonReadoutGeometry MuonSimData MuonDigitContainer MuonDigToolInterfacesLib
                   PRIVATE_LINK_LIBRARIES AtlasHepMCLib GeneratorObjects MuonAGDDDescription AtlasCLHEP_RandomGenerators PathResolver TrkDetDescrUtils TrkSurfaces TrkEventPrimitives )

atlas_add_component( MM_Digitization
                     src/components/*.cxx
                     LINK_LIBRARIES MM_DigitizationLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

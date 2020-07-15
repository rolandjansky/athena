################################################################################
# Package: sTGC_Digitization
################################################################################

# Declare the package name:
atlas_subdir( sTGC_Digitization )

# External dependencies:
find_package( AIDA )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( sTGC_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${AIDA_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel AthContainers PileUpToolsLib Identifier xAODEventInfo GaudiKernel MuonSimEvent HitManagement StoreGateLib SGtests EventInfo GeneratorObjects MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonSimData PathResolver TrkDetDescrUtils TrkSurfaces TrkEventPrimitives NSWCalibToolsLib MuonDigToolInterfacesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.dat )
atlas_install_runtime( share/*.config )


################################################################################
# Package: RDOAnalysis
################################################################################

# Declare the package name:
atlas_subdir( RDOAnalysis )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Table MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( RDOAnalysis
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps InDetIdentifier InDetRawData InDetSimData InDetBCM_RawData InDetPrepRawData LArRawEvent MuonRDO MuonSimData TileEvent TrkSurfaces TrkTruthData )

# Install files from the package:
atlas_install_joboptions( share/*.py )

################################################################################
# Package: ALFA_GloRec
################################################################################

# Declare the package name:
atlas_subdir( ALFA_GloRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/EventInfo
                          ForwardDetectors/ALFA/ALFA_Geometry
                          ForwardDetectors/ALFA/ALFA_RawEv
                          ForwardDetectors/ALFA/ALFA_RecEv/ALFA_GloRecEv
                          ForwardDetectors/ALFA/ALFA_RecEv/ALFA_LocRecCorrEv
                          ForwardDetectors/ALFA/ALFA_RecEv/ALFA_LocRecEv
                          GaudiKernel
                          Generators/GeneratorObjects )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Table MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( ALFA_GloRec
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps StoreGateLib SGtests EventInfo ALFA_Geometry ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv GaudiKernel GeneratorObjects )

# Install files from the package:
atlas_install_headers( ALFA_GloRec )
atlas_install_joboptions( share/*.py share/*.C )


################################################################################
# Package: ALFA_LocRec
################################################################################

# Declare the package name:
atlas_subdir( ALFA_LocRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventInfo
                          ForwardDetectors/ALFA/ALFA_Geometry
                          ForwardDetectors/ALFA/ALFA_RawEv
                          ForwardDetectors/ALFA/ALFA_RecEv/ALFA_LocRecEv
                          GaudiKernel
                          Generators/GeneratorObjects
                          Generators/AtlasHepMC
                           )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( ALFA_LocRec
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel StoreGateLib SGtests AthenaPoolUtilities EventInfo ALFA_Geometry ALFA_RawEv ALFA_LocRecEv GaudiKernel GeneratorObjects )

# Install files from the package:
atlas_install_headers( ALFA_LocRec )
atlas_install_joboptions( share/*.py )


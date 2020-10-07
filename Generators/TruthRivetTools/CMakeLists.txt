################################################################################
# Package: TruthRivetTools
################################################################################

# Declare the name of the package:
atlas_subdir( TruthRivetTools )

# Declare the dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
	Control/AthToolSupport/AsgTools
    Generators/AtlasHepMC
   PRIVATE
	GaudiKernel
    GenInterfaces )

# External dependencies:
find_package( HepMC )
find_package( Rivet )
find_package( YODA )
find_package( FastJet )
find_package( ROOT COMPONENTS Core Physics )
find_package( GSL )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( TruthRivetToolsLib
	Root/*.cxx TruthRivetTools/*.h
   SHARED
   PUBLIC_HEADERS TruthRivetTools
   INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS} ${RIVET_INCLUDE_DIRS} ${YODA_INCLUDE_DIRS}
   ${FASTJET_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS}
	LINK_LIBRARIES ${HEPMC_LIBRARIES} ${RIVET_LIBRARIES} ${YODA_LIBRARIES}
   ${FASTJET_LIBRARIES} ${ROOT_LIBRARIES} ${GSL_LIBRARIES} AsgTools AtlasHepMCLib )

atlas_add_component( TruthRivetTools
   src/components/*.cxx
   LINK_LIBRARIES TruthRivetToolsLib GaudiKernel )

atlas_add_dictionary( TruthRivetToolsDict
   TruthRivetTools/TruthRivetToolsDict.h TruthRivetTools/selection.xml
   LINK_LIBRARIES TruthRivetToolsLib )

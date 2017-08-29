# $Id: CMakeLists.txt 769086 2016-08-22 12:03:50Z krasznaa $
################################################################################
# Package: TruthConverters
################################################################################

# Set the name of the package:
atlas_subdir( TruthConverters )

# Set the dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC 
	Control/AthToolSupport/AsgTools
	Event/xAOD/xAODTruth
	Event/xAOD/xAODEventInfo 
	PRIVATE 
	GaudiKernel
    GenInterfaces )

# External(s) needed by the package:
find_package( HepMC )

# Component(s) in the package:
atlas_add_library( TruthConvertersLib
   TruthConverters/*.h Root/*.cxx
   SHARED
	PUBLIC_HEADERS TruthConverters
	INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
	LINK_LIBRARIES ${HEPMC_LIBRARIES} AsgTools xAODEventInfo xAODTruth )

atlas_add_component( TruthConverters 
	src/components/*.cxx
	LINK_LIBRARIES TruthConvertersLib GaudiKernel )

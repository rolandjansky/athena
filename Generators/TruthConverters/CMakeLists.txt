################################################################################
# Package: TruthConverters
################################################################################

# Extra dependencies based on the build environment:
set( extra_deps )
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/AthAnalysisBaseComps/ )
   set( extra_libs AthAnalysisBaseCompsLib )
endif()
# Set the name of the package:
atlas_subdir( TruthConverters )

# Set the dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC 
   Control/AthToolSupport/AsgTools
   ${extra_deps}
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
   LINK_LIBRARIES ${HEPMC_LIBRARIES} AsgTools xAODEventInfo xAODTruth ${extra_libs})

atlas_add_component( TruthConverters 
		     		          src/components/*.cxx
						     LINK_LIBRARIES TruthConvertersLib GaudiKernel)

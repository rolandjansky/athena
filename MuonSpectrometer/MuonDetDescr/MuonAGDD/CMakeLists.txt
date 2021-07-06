################################################################################
# Package: MuonAGDD
################################################################################

# Declare the package name:
atlas_subdir( MuonAGDD )

# Possible extra dependencies:
set( extra_lib )
if( NOT SIMULATIONBASE )
   # the dependency on AmdcAth is only for dumping the passive material XML 
   # located inside old AMDB files (using AmdcsimrecAthenaSvc in the MuonAGDDTool)
   # in principle, AGDD is independent from AMDB and all information can be accessed 
   # via AGDD itself (as it is done in recent muon layouts)
   set( extra_lib AmdcAthLib )
endif()

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( MuonAGDD
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AGDDControl GaudiKernel StoreGateLib AGDDKernel AGDDModel MuonAGDDBase MuonReadoutGeometry MuonDetDescrUtils ${extra_lib}
                     PRIVATE_LINK_LIBRARIES AGDD2GeoSvcLib )

# Install files from the package:
atlas_install_headers( MuonAGDD )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

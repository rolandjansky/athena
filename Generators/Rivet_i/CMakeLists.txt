################################################################################
# Package: Rivet_i
################################################################################

# Declare the package name:
atlas_subdir( Rivet_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Control/AthenaKernel
                          Generators/GeneratorObjects
                          Generators/GenInterfaces
                          Tools/PathResolver
                          Event/EventInfo )

# External dependencies:
find_package( FastJet )
find_package( FastJetContrib )
find_package( HepMC )
find_package( ROOT COMPONENTS Matrix Core Tree MathCore Hist RIO pthread )
find_package( Rivet )
find_package( YODA )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Rivet_iLib
                   src/*.cxx
                   PUBLIC_HEADERS Rivet_i
                   INCLUDE_DIRS ${RIVET_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${YODA_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS} ${FASTJETCONTRIB_INCLUDE_DIRS}
                   LINK_LIBRARIES ${RIVET_LIBRARIES} AthenaBaseComps GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} ${YODA_LIBRARIES} ${FASTJET_LIBRARIES} ${FASTJETCONTRIB_LIBRARIES} AthenaKernel GeneratorObjects PathResolver EventInfo )

atlas_add_component( Rivet_i
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${YODA_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS} ${FASTJETCONTRIB_INCLUDE_DIRS} ${RIVET_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} ${YODA_LIBRARIES} ${FASTJET_LIBRARIES} ${FASTJETCONTRIB_LIBRARIES} ${RIVET_LIBRARIES} AthenaBaseComps GaudiKernel AthenaKernel GeneratorObjects PathResolver EventInfo Rivet_iLib )

atlas_install_scripts(share/*.sh)

set( RivetEnvironment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of RivetEnvironment.cmake" )
find_package( RivetEnvironment )

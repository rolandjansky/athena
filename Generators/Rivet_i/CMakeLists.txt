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
                          Tools/PathResolver )

# Set the version of Rivet to use:
set( RIVET_VERSION 2.5.3 )
set( RIVET_ROOT
   ${LCG_RELEASE_DIR}/MCGenerators/rivet/${RIVET_VERSION}/${ATLAS_PLATFORM} )

set( YODA_VERSION 1.6.6 )
set( YODA_ROOT
   ${LCG_RELEASE_DIR}/MCGenerators/yoda/${YODA_VERSION}/${ATLAS_PLATFORM} )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( FastJet )
find_package( HepMC )
find_package( ROOT COMPONENTS Matrix Core Tree MathCore Hist RIO pthread )
find_package( Rivet )
find_package( YODA )
find_package( GSL )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Rivet_iLib
                   src/*.cxx
                   PUBLIC_HEADERS Rivet_i
                   INCLUDE_DIRS ${RIVET_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${YODA_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${RIVET_LIBRARIES} AthenaBaseComps GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${GSL_LIBRARIES} ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} ${YODA_LIBRARIES} ${FASTJET_LIBRARIES} AthenaKernel GeneratorObjects PathResolver )

atlas_add_component( Rivet_i
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${YODA_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS} ${RIVET_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${GSL_LIBRARIES} ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} ${YODA_LIBRARIES} ${FASTJET_LIBRARIES} ${RIVET_LIBRARIES} AthenaBaseComps GaudiKernel AthenaKernel GeneratorObjects PathResolver Rivet_iLib )


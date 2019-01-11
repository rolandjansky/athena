################################################################################
# Package: Pythia8B_i
################################################################################

# Declare the package name:
atlas_subdir( Pythia8B_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Generators/Pythia8_i
                          PRIVATE
                          Generators/GeneratorObjects )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( HepMC )
find_package( Lhapdf )
find_package( Pythia8 )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Pythia8B_iLib
                   src/*.cxx
                   PUBLIC_HEADERS Pythia8B_i
                   INCLUDE_DIRS ${PYTHIA8_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${PYTHIA8_LIBRARIES} ${LHAPDF_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel Pythia8_iLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPMC_LIBRARIES} GeneratorObjects )

atlas_add_component( Pythia8B_i
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${PYTHIA8_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${PYTHIA8_LIBRARIES} ${LHAPDF_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel Pythia8_iLib GeneratorObjects Pythia8B_iLib )

# Install files from the package:
atlas_install_joboptions( share/common/*.py
                          share/common_old/*.py )


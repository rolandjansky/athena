################################################################################
# Package: ReweightTools
################################################################################

# Declare the package name:
atlas_subdir( ReweightTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Generators/GenInterfaces
                          PRIVATE
                          Control/AthContainers
                          Control/StoreGate
                          Generators/GeneratorObjects )

# External dependencies:
find_package( HepMC )
find_package( Lhapdf )

# Component(s) in the package:
atlas_add_library( ReweightToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS ReweightTools
                   INCLUDE_DIRS ${LHAPDF_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   LINK_LIBRARIES ${LHAPDF_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthContainers GeneratorObjects )

atlas_add_component( ReweightTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${LHAPDF_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${LHAPDF_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel AthContainers StoreGateLib SGtests GeneratorObjects ReweightToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )


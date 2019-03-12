################################################################################
# Package: ALFA_Digitization
################################################################################

# Declare the package name:
atlas_subdir( ALFA_Digitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/PileUpTools
                          ForwardDetectors/ALFA/ALFA_RawEv
                          ForwardDetectors/ALFA/ALFA_SimEv
                          GaudiKernel
                          Simulation/HitManagement
                          PRIVATE
                          DetectorDescription/Identifier
                          Generators/GeneratorObjects
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( ALFA_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps AthenaKernel PileUpToolsLib ALFA_RawEv ALFA_SimEv GaudiKernel HitManagement Identifier GeneratorObjects PathResolver )

# Install files from the package:
atlas_install_headers( ALFA_Digitization )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( xtalk/*.txt )


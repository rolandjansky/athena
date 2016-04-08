################################################################################
# Package: TruthJiveXML
################################################################################

# Declare the package name:
atlas_subdir( TruthJiveXML )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          graphics/JiveXML
                          PRIVATE
                          Control/StoreGate
                          Event/EventPrimitives
                          Generators/GeneratorObjects
                          Simulation/G4Sim/TrackRecord )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( TruthJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel JiveXMLLib StoreGateLib SGtests EventPrimitives GeneratorObjects )

# Install files from the package:
atlas_install_headers( TruthJiveXML )
atlas_install_joboptions( share/*.py )


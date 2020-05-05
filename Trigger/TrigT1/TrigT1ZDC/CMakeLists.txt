################################################################################
# Package: TrigT1ZDC
################################################################################

# Declare the package name:
atlas_subdir( TrigT1ZDC )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          PRIVATE
                          Generators/GeneratorObjects
                          Generators/AtlasHepMC
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:

# Component(s) in the package:
atlas_add_component( TrigT1ZDC
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS 
                     LINK_LIBRARIES AtlasHepMCLib AthenaBaseComps GaudiKernel GeneratorObjects TrigConfL1Data TrigT1Interfaces )

# Install files from the package:
atlas_install_headers( TrigT1ZDC )


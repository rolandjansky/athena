################################################################################
# Package: MultiInputExample
################################################################################

# Declare the package name:
atlas_subdir( MultiInputExample )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Control/PileUpTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolExample/AthenaPoolExampleData
                          Event/EventInfo )

# Component(s) in the package:
atlas_add_component( MultiInputExample
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel PileUpToolsLib StoreGateLib SGtests AthenaPoolExampleData EventInfo )

# Install files from the package:
atlas_install_headers( MultiInputExample )
atlas_install_joboptions( share/MultiInputInit.py )


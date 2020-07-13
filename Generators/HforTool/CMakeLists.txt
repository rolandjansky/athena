################################################################################
# Package: HforTool
################################################################################

# Declare the package name:
atlas_subdir( HforTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/EventInfo
                          GaudiKernel
                          Reconstruction/Jet/JetEvent
                          PRIVATE
                          Control/StoreGate
                          Generators/TruthUtils
                          Generators/GeneratorObjects
                          Generators/AtlasHepMC
                          Generators/GenInterfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( HforTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib AthenaBaseComps EventInfo GaudiKernel JetEvent StoreGateLib SGtests GeneratorObjects TruthUtils GenInterfacesLib )

# Install files from the package:
atlas_install_headers( HforTool )


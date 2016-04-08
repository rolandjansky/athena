################################################################################
# Package: xAODTruthCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODTruthCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/DataModel
                          Control/StoreGate
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          Generators/GeneratorObjects )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( xAODTruthCnv
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel DataModel StoreGateLib SGtests xAODTruth GaudiKernel GeneratorObjects )

# Install files from the package:
atlas_install_joboptions( share/*.py )


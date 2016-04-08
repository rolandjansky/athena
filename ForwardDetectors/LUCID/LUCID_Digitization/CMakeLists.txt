################################################################################
# Package: LUCID_Digitization
################################################################################

# Declare the package name:
atlas_subdir( LUCID_Digitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/PileUpTools
                          Event/xAOD/xAODEventInfo
                          ForwardDetectors/LUCID/LUCID_RawEvent
                          ForwardDetectors/LUCID/LUCID_SimUtils/LUCID_SimEvent
                          GaudiKernel
                          Generators/GeneratorObjects
                          Simulation/HitManagement )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( LUCID_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel PileUpToolsLib xAODEventInfo LUCID_RawEvent LUCID_SimEvent GaudiKernel GeneratorObjects HitManagement )

# Install files from the package:
atlas_install_headers( LUCID_Digitization )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )


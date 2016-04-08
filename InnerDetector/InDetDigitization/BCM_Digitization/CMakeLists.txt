################################################################################
# Package: BCM_Digitization
################################################################################

# Declare the package name:
atlas_subdir( BCM_Digitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/PileUpTools
                          Event/xAOD/xAODEventInfo
                          Generators/GeneratorObjects
                          InnerDetector/InDetRawEvent/InDetBCM_RawData
                          InnerDetector/InDetRawEvent/InDetSimData
                          InnerDetector/InDetSimEvent
                          Simulation/Tools/AtlasCLHEP_RandomGenerators )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( BCM_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} GaudiKernel AthenaBaseComps AthenaKernel PileUpToolsLib xAODEventInfo GeneratorObjects InDetBCM_RawData InDetSimData InDetSimEvent AtlasCLHEP_RandomGenerators )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )


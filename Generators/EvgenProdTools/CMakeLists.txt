################################################################################
# Package: EvgenProdTools
################################################################################

# Declare the package name:
atlas_subdir( EvgenProdTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Generators/GenAnalysisTools/TruthHelper
                          Generators/GenInterfaces
                          Generators/GeneratorModules
                          Generators/AtlasHepMC
                          PRIVATE
                          Control/AthenaKernel
                          Event/EventInfo
                          GaudiKernel
                          Generators/TruthUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( EvgenProdToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS EvgenProdTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib AthenaBaseComps TruthHelper GeneratorModulesLib GenInterfacesLib
                   PRIVATE_LINK_LIBRARIES AthenaKernel EventInfo GaudiKernel TruthUtils )

atlas_add_component( EvgenProdTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib AthenaBaseComps TruthHelper GeneratorModulesLib AthenaKernel EventInfo GaudiKernel TruthUtils EvgenProdToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/common/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
atlas_install_runtime( share/file/*.txt )
atlas_install_scripts( scripts/simple_lhe_plotter.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

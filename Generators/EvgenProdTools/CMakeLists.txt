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
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib AthenaBaseComps TruthHelper GeneratorModulesLib
                   PRIVATE_LINK_LIBRARIES AthenaKernel EventInfo GaudiKernel TruthUtils )

atlas_add_component( EvgenProdTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib AthenaBaseComps TruthHelper GeneratorModulesLib AthenaKernel EventInfo GaudiKernel TruthUtils EvgenProdToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/common/*.py )
atlas_install_runtime( share/file/*.txt )
atlas_install_scripts( scripts/simple_lhe_plotter.py )

atlas_add_test( flake8_share
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 --ignore=F401,F821,ATL900 ${CMAKE_CURRENT_SOURCE_DIR}/share
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( flake8_scripts
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                POST_EXEC_SCRIPT nopost.sh )

################################################################################
# Package: CaloBadChannelTool
################################################################################

# Declare the package name:
atlas_subdir( CaloBadChannelTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloConditions
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          LArCalorimeter/LArRecConditions
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate )

# Component(s) in the package:
atlas_add_component( CaloBadChannelTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloConditions LArRecConditions CaloIdentifier AthenaBaseComps GaudiKernel StoreGateLib SGtests )

# Install files from the package:
atlas_install_headers( CaloBadChannelTool )

atlas_install_python_modules( python/*.py )

# Check python syntax:
atlas_add_test( flake8
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( CaloBadChanToolConfig_test
                SCRIPT python -m CaloBadChannelTool.CaloBadChanToolConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

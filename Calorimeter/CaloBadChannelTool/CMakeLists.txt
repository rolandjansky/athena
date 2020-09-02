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
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Tests:
atlas_add_test( CaloBadChanToolConfig_test
                SCRIPT python -m CaloBadChannelTool.CaloBadChanToolConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

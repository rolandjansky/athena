################################################################################
# Package: AFP_DBTools
################################################################################

# Declare the package name:
atlas_subdir( AFP_DBTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel)

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_component( AFP_DBTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel StoreGateLib SGtests AthenaPoolUtilities GaudiKernel ${Boost_LIBRARIES})

# Install files from the package:
atlas_install_headers( AFP_DBTools )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_test( flake8_scripts
                SCRIPT ${ATLAS_FLAKE8} ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )

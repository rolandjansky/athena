################################################################################
# Package: RecExAlgs
################################################################################

# Declare the package name:
atlas_subdir( RecExAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/SGTools
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate )

# Component(s) in the package:
atlas_add_component( RecExAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel SGTools GaudiKernel StoreGateLib SGtests )

# Install files from the package:
atlas_install_headers( RecExAlgs )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

atlas_add_test( flake8_share
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/share
                POST_EXEC_SCRIPT nopost.sh )

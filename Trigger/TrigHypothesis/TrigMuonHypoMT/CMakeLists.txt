################################################################################
# Package: TrigMuonHypoMT
################################################################################

# Declare the package name:
atlas_subdir( TrigMuonHypoMT )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTrigMuon
                          Trigger/TrigSteer/DecisionHandling
                          Control/AthViews
                          PRIVATE
                          Trigger/TrigT1/TrigT1Interfaces
                          Control/AthenaMonitoringKernel
                          Event/xAOD/xAODMuon
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrigMuonHypoMT
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} xAODTrigMuon TrigInterfacesLib DecisionHandlingLib AthViews xAODMuon GaudiKernel TrigT1Interfaces AthenaMonitoringKernelLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

# Unit tests:
atlas_add_test( TrigMuonHypoMTConfig
                SCRIPT python -m TrigMuonHypoMT.TrigMuonHypoMTConfig
                POST_EXEC_SCRIPT nopost.sh )

# Check python syntax:
atlas_add_test( flake8
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 --enable-extension=ATL900,ATL901 ${CMAKE_CURRENT_SOURCE_DIR}/python
                POST_EXEC_SCRIPT nopost.sh )

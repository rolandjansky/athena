################################################################################
# Package: TrigStreamerHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigStreamerHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          GaudiKernel
                          Trigger/TrigSteer/DecisionHandling
                          Trigger/TrigSteer/TrigCompositeUtils)

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrigStreamerHypo
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} TrigInterfacesLib TrigTimeAlgsLib GaudiKernel DecisionHandlingLib TrigCompositeUtilsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

# Unit tests:
atlas_add_test( TrigStreamerHypoConfigMT SCRIPT python -m TrigStreamerHypo.TrigStreamerHypoConfigMT
                PROPERTIES TIMEOUT 300
		        POST_EXEC_SCRIPT nopost.sh )

# Check Python syntax:
atlas_add_test( flake8
   SCRIPT flake8 --select=ATL,F,E7,E9,W6 --enable-extension=ATL900,ATL901 ${CMAKE_CURRENT_SOURCE_DIR}/python
   POST_EXEC_SCRIPT nopost.sh )

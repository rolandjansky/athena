################################################################################
# Package: TrigMissingETHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigMissingETHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Event/xAOD/xAODTrigMissingET
                          GaudiKernel
                          Trigger/TrigEvent/TrigMissingEtEvent 
                          Trigger/TrigSteer/DecisionHandling
                          Trigger/TrigAlgorithms/TrigEFMissingET)

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrigMissingETHypo
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} TrigInterfacesLib TrigTimeAlgsLib xAODTrigMissingET GaudiKernel TrigMissingEtEvent DecisionHandling  )

atlas_add_test( TrigMissingETHypoConfigMT SCRIPT python -m TrigMissingETHypo.TrigMissingETHypoConfigMT
                PROPERTIES TIMEOUT 300
		        POST_EXEC_SCRIPT nopost.sh )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/TriggerConfig_*.py )


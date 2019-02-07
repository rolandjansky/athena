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
                          Event/xAOD/xAODMuon
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrigMuonHypoMT
		     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} xAODTrigMuon TrigInterfacesLib DecisionHandling AthViews xAODMuon GaudiKernel )

atlas_add_test( TrigMuonHypoMTConfig SCRIPT python -m  	TrigMuonHypoMT.TrigMuonHypoMTConfig
		POST_EXEC_SCRIPT nopost.sh )

# Install files from the package:
atlas_install_python_modules( python/*.py )


################################################################################
# Package: TrigAFPHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigAFPHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTrigger
			              Event/xAOD/xAODForward
						  Tools/PathResolver
                          Trigger/TrigEvent/TrigMissingEtEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigAFPHypo
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
					 LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel TrigParticle TrigSteeringEvent TrigInterfacesLib xAODJet xAODTrigger TrigMissingEtEvent xAODForward PathResolver)

# Install files from the package:
atlas_install_joboptions( share/HypoJobOptions.py )
atlas_install_python_modules( python/*.py )
atlas_install_runtime( optics/*.txt )


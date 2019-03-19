################################################################################
# Package: TrigTauDiscriminant
################################################################################

# Declare the package name:
atlas_subdir( TrigTauDiscriminant )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
			  Reconstruction/tauRecTools
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Control/CxxUtils
                          Event/xAOD/xAODTau
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigT1/TrigT1Interfaces )

# Component(s) in the package:
atlas_add_library( TrigTauDiscriminantLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigTauDiscriminant
                   LINK_LIBRARIES GaudiKernel tauRecToolsLib TrigInterfacesLib TrigTimeAlgsLib
                   PRIVATE_LINK_LIBRARIES xAODTau TrigParticle TrigSteeringEvent TrigT1Interfaces )

atlas_add_component( TrigTauDiscriminant
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel tauRecToolsLib TrigInterfacesLib TrigTimeAlgsLib xAODTau TrigParticle TrigSteeringEvent TrigT1Interfaces TrigTauDiscriminantLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.txt share/*.py )
atlas_install_runtime( share/*.root share/*.bin share/*.txt )
atlas_install_data( share )


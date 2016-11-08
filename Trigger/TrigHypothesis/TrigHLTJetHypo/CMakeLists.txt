###############################################################################
# Package: TrigHLTJetHypo
###############################################################################

#
# Declare the package name:
atlas_subdir( TrigHLTJetHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODJet
                          GaudiKernel
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigHLTJetHypoLib
                   src/*.cxx
                   src/TrigHLTJetHypoUtils/*.cxx
                   PUBLIC_HEADERS TrigHLTJetHypo
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES xAODJet GaudiKernel TrigParticle TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_component( TrigHLTJetHypo
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} xAODJet GaudiKernel TrigParticle TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib TrigHLTJetHypoLib )

# Install files from the package:
atlas_install_python_modules( python/*.py ) 


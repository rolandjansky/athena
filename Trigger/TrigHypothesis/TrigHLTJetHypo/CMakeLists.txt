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
                          Trigger/TrigTools/TrigTimeAlgs
                          Trigger/TrigSteer/DecisionHandling
                          Trigger/TrigSteer/TrigCompositeUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigHLTJetHypoLib
                   src/*.cxx
                   src/TrigHLTJetHypoUtils/*.cxx
                   PUBLIC_HEADERS TrigHLTJetHypo
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES xAODJet GaudiKernel TrigParticle TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib DecisionHandlingLib TrigCompositeUtilsLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_component( TrigHLTJetHypo
                     src/components/*.cxx
                     LINK_LIBRARIES TrigHLTJetHypoLib )

atlas_add_test( TrigHLTJetHypoTool SCRIPT python -m TrigHLTJetHypo.TrigJetHypoToolConfig
				POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( flake8
   SCRIPT flake8 --select=ATL,F,E7,E9,W6 --extend-ignore=E701 ${CMAKE_CURRENT_SOURCE_DIR}/python
   POST_EXEC_SCRIPT nopost.sh )

		      
# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} ) 

atlas_add_test( flake8_share
                SCRIPT flake8 --select=ATL,F,E7,E9,W6  ${CMAKE_CURRENT_SOURCE_DIR}/share
                POST_EXEC_SCRIPT nopost.sh )

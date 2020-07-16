################################################################################
# Package: TrigBphysHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigBphysHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthLinks
                          Control/AthViews
                          Control/CxxUtils
                          Control/StoreGate
                          Event/FourMomUtils
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigBphys
                          Event/xAOD/xAODTrigMuon
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          LumiBlock/LumiBlockComps
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          InnerDetector/InDetRecTools/InDetConversionFinderTools
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          Tracking/TrkVertexFitter/TrkVKalVrtFitter
                          Trigger/TrigEvent/TrigBphysicsEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          Trigger/TrigSteer/DecisionHandling
                          Trigger/TrigSteer/TrigCompositeUtils)

# External dependencies:

# Component(s) in the package:
atlas_add_component( TrigBphysHypo
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS 
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel AthLinks BeamSpotConditionsData ElectronPhotonSelectorToolsLib InDetConversionFinderToolsLib DecisionHandlingLib FourMomUtils StoreGateLib xAODEventInfo xAODMuon xAODTracking xAODTrigBphys xAODTrigMuon GaudiKernel TrkVKalVrtFitterLib TrigBphysicsEvent TrigInDetEvent TrigMuonEvent TrigNavigationLib TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib AthViews TrigCompositeUtilsLib TrigT1Interfaces TrigParticle)

#TrigParticle is needed for linking getRecentFeaturesLinks for some reason

# Install files from the package:
atlas_install_python_modules( python/*.py )

# Check Python syntax:
atlas_add_test( flake8
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 --enable-extension=ATL900,ATL901 ${CMAKE_CURRENT_SOURCE_DIR}/python
                POST_EXEC_SCRIPT nopost.sh )

################################################################################
# Package: TrigEgammaMatchingTool
################################################################################

# Declare the package name:
atlas_subdir( TrigEgammaMatchingTool )

# Extra dependencies, based on the build environment:
set( extra_deps )
if( NOT XAOD_STANDALONE )
    set( extra_deps Control/AthenaBaseComps GaudiKernel )
   endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigCalo
                          Event/xAOD/xAODTrigEgamma
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTrigger
			  ${extra_deps} )



# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigEgammaMatchingToolLib
                   Root/*.cxx
                   PUBLIC_HEADERS TrigEgammaMatchingTool
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools xAODCaloEvent xAODEgamma xAODTracking xAODTrigCalo xAODTrigEgamma TrigConfHLTData TrigSteeringEvent TrigDecisionToolLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} xAODMuon xAODTau xAODTrigger  )


if( NOT XAOD_STANDALONE )
atlas_add_component( TrigEgammaMatchingTool
                     src/*.h src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma xAODTracking xAODTrigCalo xAODTrigEgamma TrigDecisionToolLib TrigConfHLTData TrigSteeringEvent AthenaBaseComps xAODMuon xAODTau xAODTrigger GaudiKernel TrigEgammaMatchingToolLib )

endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )


################################################################################
# Package: DerivationFrameworkInDet
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkInDet )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/IRegionSelector
                          DetectorDescription/Identifier
                          DetectorDescription/RoiDescriptor
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          InnerDetector/InDetConditions/SCT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/SCT_Cabling
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecTools/TRT_ToT_Tools
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          PRIVATE
                          Commission/CommissionEvent
                          DetectorDescription/AtlasDetDescr
                          Event/FourMomUtils
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODTruth
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel
                          InnerDetector/InDetConditions/TRT_ConditionsServices
			  InnerDetector/InDetRecEvent/InDetRIO_OnTrack
			  InnerDetector/InDetRecTools/InDetAssociationTools
                          InnerDetector/InDetValidation/InDetPhysValMonitoring
                          LArCalorimeter/LArRecEvent
                          PhysicsAnalysis/CommonTools/ExpressionEvaluation
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkInDetLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkInDet
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks AthenaBaseComps AthenaKernel SGTools IRegionSelector Identifier RoiDescriptor xAODBase xAODEgamma xAODEventInfo xAODTracking InDetIdentifier InDetReadoutGeometry InDetPrepRawData TrkTrack VxVertex StoreGateLib SGtests SCT_CablingLib TRT_ConditionsServicesLib ExpressionEvaluationLib
                   PRIVATE_LINK_LIBRARIES CommissionEvent AtlasDetDescr FourMomUtils xAODJet xAODMuon xAODTau xAODPFlow xAODTruth xAODCaloEvent GaudiKernel InDetRIO_OnTrack LArRecEvent TrkCompetingRIOsOnTrack TrkEventPrimitives TrkEventUtils TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkExInterfaces TrkToolInterfaces TrkVertexFitterInterfaces )

atlas_add_component( DerivationFrameworkInDet
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests IRegionSelector Identifier RoiDescriptor xAODBase xAODEgamma xAODEventInfo xAODTracking InDetIdentifier InDetReadoutGeometry SCT_CablingLib InDetPrepRawData TrkTrack VxVertex CommissionEvent AtlasDetDescr FourMomUtils xAODJet xAODMuon xAODTau xAODPFlow xAODTruth xAODCaloEvent GaudiKernel TRT_ConditionsServicesLib InDetRIO_OnTrack LArRecEvent ExpressionEvaluationLib TrkCompetingRIOsOnTrack TrkEventPrimitives TrkEventUtils TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkExInterfaces TrkToolInterfaces TrkVertexFitterInterfaces DerivationFrameworkInDetLib )


# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

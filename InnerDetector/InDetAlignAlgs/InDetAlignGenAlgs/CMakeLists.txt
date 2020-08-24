################################################################################
# Package: InDetAlignGenAlgs
################################################################################

# Declare the package name:
atlas_subdir( InDetAlignGenAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          InnerDetector/InDetAlignTools/InDetAlignGenTools
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkFitter/TrkFitterUtils
                          PRIVATE
                          Control/AthenaKernel
                          Database/RegistrationServices
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          DetectorDescription/IdDictDetDescr
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetAlignGenAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives Identifier EventPrimitives xAODEventInfo GaudiKernel TRT_ConditionsServicesLib TrkParameters TrkFitterUtils AthenaKernel AtlasDetDescr DetDescrConditions IdDictDetDescr InDetIdentifier InDetReadoutGeometry TRT_ReadoutGeometry InDetPrepRawData InDetRIO_OnTrack TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkTrack TrkTrackSummary TrkExInterfaces TrkFitterInterfaces TrkToolInterfaces InDetAlignGenToolsLib RegistrationServicesLib )

# Install files from the package:
atlas_install_headers( InDetAlignGenAlgs )
atlas_install_joboptions( share/*.py )


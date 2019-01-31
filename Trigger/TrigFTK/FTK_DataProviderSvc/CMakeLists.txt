################################################################################
# Package: FTK_DataProviderSvc
################################################################################

# Declare the package name:
atlas_subdir( FTK_DataProviderSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasPolicy
                          AtlasReconstructionRunTime
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/xAOD/xAODTracking
#                          Event/xAOD/xAODViews
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetCondTools
                          InnerDetector/InDetConditions/PixelConditionsData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tools/PyJobTransforms
                          Tracking/TrkEvent/TrkTrack
#                          Tracking/TrkEvent/TrkViews
                          Trigger/TrigFTK/FTK_DataProviderInterfaces
                          Trigger/TrigFTK/TrigFTK_RawData
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/IRegionSelector
                          DetectorDescription/RoiDescriptor
                          Event/EventInfo
                          Generators/GeneratorObjects
                          InnerDetector/InDetConditions/PixelConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkFitter/TrkFitterInterfaces
			  Tracking/TrkVertexFitter/TrkVertexFitterInterfaces 
                          Tracking/TrkFitter/TrkFitterUtils
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigFTK/FTK_RecToolInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( FTK_DataProviderSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS FTK_DataProviderSvc
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODTracking GaudiKernel InDetPrepRawData InDetRIO_OnTrack TrkTrack FTK_DataProviderInterfaces TrigFTK_RawData StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${EIGEN_LIBRARIES} AtlasDetDescr GeoPrimitives IRegionSelector RoiDescriptor EventInfo GeneratorObjects InDetIdentifier InDetReadoutGeometry InDetRecToolInterfaces TrkEventPrimitives TrkParameters TrkRIO_OnTrack TrkTrackSummary TrkTruthData TrkFitterInterfaces TrkVertexFitterInterfaces  TrkFitterUtils TrkToolInterfaces PixelConditionsData)

atlas_add_component( FTK_DataProviderSvc
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps StoreGateLib SGtests xAODTracking GaudiKernel InDetPrepRawData InDetRIO_OnTrack TrkTrack FTK_DataProviderInterfaces TrigFTK_RawData AtlasDetDescr GeoPrimitives IRegionSelector RoiDescriptor EventInfo GeneratorObjects InDetIdentifier InDetReadoutGeometry InDetRecToolInterfaces TrkEventPrimitives TrkParameters TrkRIO_OnTrack TrkTrackSummary TrkTruthData TrkFitterInterfaces TrkVertexFitterInterfaces TrkFitterUtils TrkToolInterfaces FTK_DataProviderSvcLib PixelConditionsData)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )


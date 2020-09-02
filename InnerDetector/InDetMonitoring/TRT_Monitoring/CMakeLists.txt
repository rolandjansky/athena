
################################################################################
# Package: TRT_Monitoring
################################################################################

# Declare the package name:
atlas_subdir( TRT_Monitoring )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TRT_Monitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AthenaMonitoringLib GaudiKernel InDetRawData LumiBlockCompsLib CommissionEvent AthContainers AtlasDetDescr Identifier xAODEventInfo xAODTrigger EventPrimitives TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry TRT_ReadoutGeometry InDetRIO_OnTrack LWHists TrkTrack TrkTrackSummary TrkToolInterfaces TRT_DriftFunctionToolLib InDetByteStreamErrors )

# Install files from the package:
atlas_install_joboptions( share/*.py )


################################################################################
# Package: InDetLowBetaFinder
################################################################################

# Declare the package name:
atlas_subdir( InDetLowBetaFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          PRIVATE
                          Control/CxxUtils
                          Control/StoreGate
                          InnerDetector/InDetConditions/TRT_ConditionsData
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetLowBetaInfo
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          InnerDetector/InDetRecTools/TRT_ElectronPidTools
                          Tracking/TrkEvent/TrkParameters
                          Event/xAOD/xAODTracking
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
			  MagneticField/MagFieldElements
                          MagneticField/MagFieldConditions
			  )

# External dependencies:
find_package( ROOT COMPONENTS Minuit RIO Core Tree MathCore Hist pthread MathMore Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( InDetLowBetaFinder
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel TRT_ConditionsServicesLib CxxUtils StoreGateLib SGtests TRT_ConditionsData InDetIdentifier InDetLowBetaInfo InDetRIO_OnTrack TrkParameters xAODTracking TrkRIO_OnTrack TrkTrack MagFieldElements MagFieldConditions TRT_ElectronPidToolsLib )

# Install files from the package:
atlas_install_headers( InDetLowBetaFinder )


################################################################################
# Package: CscRawDataMonitoring
################################################################################

# Declare the package name:
atlas_subdir( CscRawDataMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigAnalysis/TrigDecisionTool
                          PRIVATE
                          Control/StoreGate
                          Event/xAOD/xAODEventInfo
                          MuonSpectrometer/MuonCalib/CscCalib/CscCalibTools
                          MuonSpectrometer/MuonCnv/MuonCSC_CnvTools
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonReconstruction/MuonDataPrep/CscClusterization
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( CscRawDataMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringLib GaudiKernel MuonPrepRawData MuonSegment TrkParameters TrkSegment TrkTrack TrigDecisionToolLib StoreGateLib SGtests xAODEventInfo MuonIdHelpersLib MuonRDO CscClusterizationLib MuonRIO_OnTrack MuonRecHelperToolsLib TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives )

# Install files from the package:
atlas_install_headers( CscRawDataMonitoring )
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )


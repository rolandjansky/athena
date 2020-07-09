################################################################################
# Package: TrkTrackSummaryTool
################################################################################

# Declare the package name:
atlas_subdir( TrkTrackSummaryTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthContainers
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkTools/TrkToolInterfaces
                          InnerDetector/InDetRecTools/TRT_ElectronPidTools
			  Control/CxxUtils
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/Identifier
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_component( TrkTrackSummaryTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthContainers GaudiKernel TrkParameters TrkTrackSummary TrkToolInterfaces AtlasDetDescr 
		     Identifier TrkDetElementBase TrkGeometry TrkCompetingRIOsOnTrack TrkEventPrimitives TrkMeasurementBase TrkRIO_OnTrack TrkTrack CxxUtils
                     TRT_ElectronPidToolsLib )

# Install files from the package:
atlas_install_headers( TrkTrackSummaryTool )
atlas_install_python_modules( python/*.py )


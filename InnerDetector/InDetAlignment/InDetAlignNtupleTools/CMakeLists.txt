################################################################################
# Package: InDetAlignNtupleTools
################################################################################

# Declare the package name:
atlas_subdir( InDetAlignNtupleTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkAlignment/TrkAlignInterfaces
                          PRIVATE
                          Event/EventPrimitives
                          Event/xAOD/xAODEventInfo
                          Generators/GeneratorObjects
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Tracking/TrkAlignment/TrkAlignEvent
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkEvent/TrkVertexOnTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetAlignNtupleTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel EventPrimitives xAODEventInfo GeneratorObjects InDetIdentifier TrkAlignEvent TrkEventPrimitives TrkParameters TrkRIO_OnTrack TrkTrack TrkTrackSummary TrkTruthData TrkVertexOnTrack TrkExInterfaces TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( InDetAlignNtupleTools )


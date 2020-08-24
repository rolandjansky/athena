################################################################################
# Package: ISF_FatrasRecoTools
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasRecoTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Simulation/ISF/ISF_Fatras/ISF_FatrasEvent
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkTruthTracks/TrkTruthTrackInterfaces
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          Event/EventPrimitives
                          Generators/AtlasHepMC
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/SiSpacePoint
                          Simulation/ISF/ISF_Fatras/ISF_FatrasDetDescrModel
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTruthData )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( ISF_FatrasRecoTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AtlasHepMCLib AthenaBaseComps GeoPrimitives Identifier ISF_FatrasEvent TrkEventPrimitives TrkParameters TrkSpacePoint TrkTrackSummary TrkToolInterfaces AtlasDetDescr EventPrimitives GaudiKernel InDetIdentifier InDetPrepRawData SiSpacePoint ISF_FatrasDetDescrModel TrkDetElementBase TrkCompetingRIOsOnTrack TrkEventUtils TrkPrepRawData TrkRIO_OnTrack TrkTrack TrkTruthData TrkTruthTrackInterfaces )

# Install files from the package:
atlas_install_headers( ISF_FatrasRecoTools )


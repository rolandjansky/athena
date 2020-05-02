################################################################################
# Package: InDetRecStatistics
################################################################################

# Declare the package name:
atlas_subdir( InDetRecStatistics )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/Identifier
                          GaudiKernel
                          Generators/GeneratorObjects
                          Generators/AtlasHepMC
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IdDictDetDescr
                          Generators/GenAnalysisTools/TruthHelper
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( InDetRecStatistics
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps Identifier GaudiKernel GeneratorObjects TrkTrack TrkTrackSummary AtlasDetDescr IdDictDetDescr TruthHelper InDetIdentifier InDetRIO_OnTrack TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkEventUtils TrkParameters TrkRIO_OnTrack TrkTruthData VxVertex TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( InDetRecStatistics )
atlas_install_joboptions( share/*.py )


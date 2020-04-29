################################################################################
# Package: TrkTruthCreatorTools
################################################################################

# Declare the package name:
atlas_subdir( TrkTruthCreatorTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthLinks
                          Control/AthContainers
                          DetectorDescription/AtlasDetDescr
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Generators/GeneratorObjects
                          Generators/AtlasHepMC
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:

# Component(s) in the package:
atlas_add_component( TrkTruthCreatorTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS 
                     LINK_LIBRARIES AtlasHepMCLib AthenaBaseComps AthLinks AthContainers AtlasDetDescr GaudiKernel TrkEventUtils TrkTruthData TrkToolInterfaces GeneratorObjects TrkCompetingRIOsOnTrack TrkMeasurementBase TrkPrepRawData TrkRIO_OnTrack TrkTrack )

# Install files from the package:
atlas_install_headers( TrkTruthCreatorTools )


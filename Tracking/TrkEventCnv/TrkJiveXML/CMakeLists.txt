################################################################################
# Package: TrkJiveXML
################################################################################

# Declare the package name:
atlas_subdir( TrkJiveXML )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters
                          graphics/JiveXML
                          PRIVATE
                          Control/DataModel
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackLink
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_component( TrkJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkParameters JiveXMLLib DataModel StoreGateLib SGtests AtlasDetDescr Particle TrkCompetingRIOsOnTrack TrkEventPrimitives TrkEventUtils TrkMeasurementBase TrkParticleBase TrkRIO_OnTrack TrkSegment TrkTrack TrkTrackSummary TrkTruthData VxVertex TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( TrkJiveXML )
atlas_install_joboptions( share/*.py )


# $Id: CMakeLists.txt 769538 2016-08-23 12:55:49Z krasznaa $
################################################################################
# Package: TrkValAlgs
################################################################################

# Declare the package name:
atlas_subdir( TrkValAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   GaudiKernel
   Tracking/TrkEvent/TrkParameters
   Tracking/TrkEvent/TrkTrack
   Tracking/TrkEvent/TrkTruthData
   PRIVATE
   DetectorDescription/AtlasDetDescr
   Generators/GeneratorObjects
   Generators/AtlasHepMC
   Tracking/TrkDetDescr/TrkSurfaces
   Tracking/TrkEvent/TrkEventUtils
   Tracking/TrkEvent/TrkMaterialOnTrack
   Tracking/TrkEvent/TrkParticleBase
   Tracking/TrkEvent/TrkPrepRawData
   Tracking/TrkEvent/TrkRIO_OnTrack
   Tracking/TrkEvent/TrkSegment
   Tracking/TrkEvent/TrkSpacePoint
   Tracking/TrkEvent/TrkTrackSummary
   Tracking/TrkEvent/VxVertex
   Tracking/TrkTools/TrkToolInterfaces
   Tracking/TrkValidation/TrkValEvent
   Tracking/TrkValidation/TrkValInterfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree )

# Component(s) in the package:
atlas_add_component( TrkValAlgs
   TrkValAlgs/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
   LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib AthenaBaseComps
   GaudiKernel TrkParameters TrkTrack TrkTruthData AtlasDetDescr
   GeneratorObjects TrkSurfaces TrkEventUtils TrkMaterialOnTrack
   TrkParticleBase TrkPrepRawData TrkRIO_OnTrack TrkSegment TrkSpacePoint
   TrkTrackSummary VxVertex TrkToolInterfaces TrkValEvent TrkValInterfaces )

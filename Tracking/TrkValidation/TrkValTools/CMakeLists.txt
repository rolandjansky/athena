################################################################################
# Package: TrkValTools
################################################################################

# Declare the package name:
atlas_subdir( TrkValTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/DataModel
                          Control/SGTools
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkValidation/TrkValEvent
                          Tracking/TrkValidation/TrkValInterfaces
                          PRIVATE
                          Commission/CommissionEvent
                          DetectorDescription/AtlasDetDescr
                          Event/EventInfo
                          Event/EventPrimitives
                          Generators/GeneratorObjects
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkFitter/TrkFitterUtils )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( TrkValTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps DataModel SGTools xAODTracking GaudiKernel TrkEventPrimitives TrkParameters TrkTrack TrkToolInterfaces TrkValEvent TrkValInterfaces CommissionEvent AtlasDetDescr EventInfo EventPrimitives GeneratorObjects TrkSurfaces TrkCompetingRIOsOnTrack TrkEventUtils TrkMaterialOnTrack TrkMeasurementBase TrkParticleBase TrkRIO_OnTrack TrkTrackSummary TrkTruthData TrkExInterfaces TrkFitterUtils )

# Install files from the package:
atlas_install_headers( TrkValTools )


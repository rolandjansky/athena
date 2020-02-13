################################################################################
# Package: TrkVertexFitters
################################################################################

# Declare the package name:
atlas_subdir( TrkVertexFitters )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParametersBase
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          PRIVATE
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkLinks
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxMultiVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# Component(s) in the package:
atlas_add_library( TrkVertexFittersLib
                   src/*.cxx
                   PUBLIC_HEADERS TrkVertexFitters
                   LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel TrkParameters TrkParametersBase TrkParticleBase VxVertex TrkVertexFitterInterfaces
                   PRIVATE_LINK_LIBRARIES TrkSurfaces TrkLinks TrkTrack VxMultiVertex TrkExInterfaces )

atlas_add_component( TrkVertexFitters
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel TrkParameters TrkParametersBase TrkParticleBase VxVertex TrkVertexFitterInterfaces TrkSurfaces TrkLinks TrkTrack VxMultiVertex TrkExInterfaces TrkVertexFittersLib )


# Install files from the package:
atlas_install_joboptions( share/*.py )


atlas_add_test( AdaptiveVertexFitter_test
                SCRIPT athena.py TrkVertexFitters/AdaptiveVertexFitter_test.py
                PROPERTIES TIMEOUT 300
                LOG_IGNORE_PATTERN " INFO |WARNING |found service|Adding private|^ +[+]|HepPDT Version|^indet|^pixel|^phi|^eta|^GEOPIXEL|in material|no dictionary|^subdet|^part|^barrel_endcap|^layer|^sct|^bec|^side0|^strip0|^lay_disk|TrackingGeometrySvc|^PixelID" )


atlas_add_test( AdaptiveMultiVertexFitter_test
                SCRIPT athena.py TrkVertexFitters/AdaptiveMultiVertexFitter_test.py
                PROPERTIES TIMEOUT 300
                LOG_IGNORE_PATTERN " INFO |WARNING |found service|Adding private|^ +[+]|HepPDT Version|^indet|^pixel|^phi|^eta|^GEOPIXEL|in material|no dictionary|^subdet|^part|^barrel_endcap|^layer|^sct|^bec|^side0|^strip0|^lay_disk|TrackingGeometrySvc|^PixelID" )

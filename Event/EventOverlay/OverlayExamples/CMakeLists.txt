################################################################################
# Package: OverlayExamples
################################################################################

# Declare the package name:
atlas_subdir( OverlayExamples )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/PileUpTools
                          Event/EventInfo
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArSimEvent )

# Component(s) in the package:
atlas_add_component( OverlayExamples
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps PileUpToolsLib EventInfo LArRawEvent LArSimEvent )

# Install files from the package:
atlas_install_headers( OverlayExamples )
atlas_install_joboptions( share/*.py )


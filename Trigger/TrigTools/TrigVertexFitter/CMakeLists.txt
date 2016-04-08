################################################################################
# Package: TrigVertexFitter
################################################################################

# Declare the package name:
atlas_subdir( TrigVertexFitter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          PRIVATE
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigTools/TrigTimeAlgs )

# Component(s) in the package:
atlas_add_component( TrigVertexFitter
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkEventPrimitives TrkParameters TrkTrack TrigInDetEvent TrigTimeAlgsLib )

# Install files from the package:
atlas_install_headers( TrigVertexFitter )
atlas_install_joboptions( share/*.py )


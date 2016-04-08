################################################################################
# Package: TrkAssociationTools
################################################################################

# Declare the package name:
atlas_subdir( TrkAssociationTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/Identifier
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_component( TrkAssociationTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps TrkToolInterfaces AtlasDetDescr Identifier GaudiKernel TrkDetElementBase TrkEventUtils TrkRIO_OnTrack TrkTrack )

# Install files from the package:
atlas_install_headers( TrkAssociationTools )


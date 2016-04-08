################################################################################
# Package: InDetAssociationTools
################################################################################

# Declare the package name:
atlas_subdir( InDetAssociationTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_component( InDetAssociationTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps Identifier GaudiKernel InDetPrepRawData TrkPrepRawData TrkToolInterfaces AtlasDetDescr InDetIdentifier InDetRIO_OnTrack TrkDetElementBase TrkRIO_OnTrack TrkTrack )

# Install files from the package:
atlas_install_headers( InDetAssociationTools )


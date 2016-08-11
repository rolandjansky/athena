################################################################################
# Package: InDetEventCnvTools
################################################################################

# Declare the package name:
atlas_subdir( InDetEventCnvTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Tracking/TrkEventCnv/TrkEventCnvTools
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IdDictDetDescr
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# Component(s) in the package:
atlas_add_component( InDetEventCnvTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AtlasDetDescr IdDictDetDescr Identifier GaudiKernel InDetReadoutGeometry InDetPrepRawData InDetRIO_OnTrack TrkPrepRawData TrkRIO_OnTrack )

# Install files from the package:
atlas_install_headers( InDetEventCnvTools )


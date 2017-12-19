################################################################################
# Package: InDetTruthAlgs
################################################################################

# Declare the package name:
atlas_subdir( InDetTruthAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          InnerDetector/InDetRawEvent/InDetSimData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetTruth/InDetTruthInterfaces
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkTools/TrkToolInterfaces
                          Event/EventContainers
                          PRIVATE
                          Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_component( InDetTruthAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests GaudiKernel InDetSimData InDetPrepRawData TrkTruthData TrkToolInterfaces TrkTrack EventContainers )

# Install files from the package:
atlas_install_headers( InDetTruthAlgs )


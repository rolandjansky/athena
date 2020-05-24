################################################################################
# Package: MuonEventCnvTools
################################################################################

# Declare the package name:
atlas_subdir( MuonEventCnvTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Tracking/TrkEventCnv/TrkEventCnvTools
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          GaudiKernel
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# Component(s) in the package:
atlas_add_component( MuonEventCnvTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests Identifier EventPrimitives GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData MuonRIO_OnTrack TrkDetElementBase TrkPrepRawData TrkRIO_OnTrack TrkEventCnvToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )


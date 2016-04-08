################################################################################
# Package: RpcClusterization
################################################################################

# Declare the package name:
atlas_subdir( RpcClusterization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          GaudiKernel
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          TestPolicy
                          PRIVATE
                          MuonSpectrometer/MuonIdHelpers )

# Component(s) in the package:
atlas_add_component( RpcClusterization
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests Identifier EventPrimitives GaudiKernel MuonReadoutGeometry MuonDigitContainer MuonPrepRawData MuonIdHelpersLib )

# Install files from the package:
atlas_install_headers( RpcClusterization )
atlas_install_joboptions( share/*.py )


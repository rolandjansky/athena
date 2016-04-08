################################################################################
# Package: MuonPrdSelector
################################################################################

# Declare the package name:
atlas_subdir( MuonPrdSelector )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/MuonCalibIdentifier
                          MuonSpectrometer/MuonCalib/MuonCalibTools
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          PRIVATE
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers )

# Component(s) in the package:
atlas_add_component( MuonPrdSelector
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests Identifier GaudiKernel MuonCalibIdentifier MuonCalibToolsLib MuonPrepRawData MuonReadoutGeometry MuonIdHelpersLib )

# Install files from the package:
atlas_install_headers( MuonPrdSelector )
atlas_install_joboptions( share/*.py )


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

atlas_add_library( MuonPrdSelectorLib
                   MuonPrdSelector/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonPrdSelector
                   LINK_LIBRARIES GaudiKernel MuonCalibToolsLib MuonCalibIdentifier Identifier AthenaBaseComps MuonCalibToolsLib MuonIdHelpersLib MuonPrepRawData )

# Component(s) in the package:
atlas_add_component( MuonPrdSelector
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES MuonPrdSelectorLib StoreGateLib SGtests MuonReadoutGeometry )

# Install files from the package:
atlas_install_joboptions( share/*.py )


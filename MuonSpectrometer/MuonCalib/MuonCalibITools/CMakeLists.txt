################################################################################
# Package: MuonCalibITools
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibITools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/MuonCalibEventBase
                          MuonSpectrometer/MuonCalib/MuonCalibIdentifier )

# Component(s) in the package:
atlas_add_library( MuonCalibITools
                   PUBLIC_HEADERS MuonCalibITools
                   LINK_LIBRARIES Identifier GaudiKernel MuonCalibEventBase MuonCalibIdentifier )


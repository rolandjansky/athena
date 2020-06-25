################################################################################
# Package: MdtCalibUtils
################################################################################

# Declare the package name:
atlas_subdir( MdtCalibUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibData
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibInterfaces
                          MuonSpectrometer/MuonCalib/MuonCalibEvent
                          MuonSpectrometer/MuonCalib/MuonCalibEventBase
                          MuonSpectrometer/MuonCalib/MuonCalibIdentifier
                          PRIVATE
                          MuonSpectrometer/MuonCalib/MuonCalibUtils/MuonCalibStl )

# Component(s) in the package:
atlas_add_library( MdtCalibUtils
                   src/*.cxx
                   PUBLIC_HEADERS MdtCalibUtils
                   LINK_LIBRARIES MdtCalibData MuonCalibEvent MuonCalibEventBase MuonCalibIdentifier MdtCalibInterfacesLib )


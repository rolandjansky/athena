################################################################################
# Package: MdtCalibEventLoop
################################################################################

# Declare the package name:
atlas_subdir( MdtCalibEventLoop )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibInterfaces
                          MuonSpectrometer/MuonCalib/MuonCalibEventBase )

# Component(s) in the package:
atlas_add_library( MdtCalibEventLoop
                   src/*.cxx
                   PUBLIC_HEADERS MdtCalibEventLoop
                   PRIVATE_LINK_LIBRARIES MuonCalibEventBase )


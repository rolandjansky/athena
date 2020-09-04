################################################################################
# Package: MdtCalibUtils
################################################################################

# Declare the package name:
atlas_subdir( MdtCalibUtils )

# Component(s) in the package:
atlas_add_library( MdtCalibUtils
                   src/*.cxx
                   PUBLIC_HEADERS MdtCalibUtils
                   LINK_LIBRARIES MdtCalibData MuonCalibEvent MuonCalibEventBase MuonCalibIdentifier MdtCalibInterfacesLib )


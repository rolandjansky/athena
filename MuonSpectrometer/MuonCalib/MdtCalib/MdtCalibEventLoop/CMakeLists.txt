################################################################################
# Package: MdtCalibEventLoop
################################################################################

# Declare the package name:
atlas_subdir( MdtCalibEventLoop )

# Component(s) in the package:
atlas_add_library( MdtCalibEventLoop
                   src/*.cxx
                   PUBLIC_HEADERS MdtCalibEventLoop
                   PRIVATE_LINK_LIBRARIES MuonCalibEventBase MdtCalibInterfacesLib )


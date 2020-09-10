################################################################################
#Package: NSWCalibData
################################################################################
 
# Declare the package name:
 atlas_subdir( NSWCalibData )
 
# External dependencies:
# find_package( CLHEP )
 
# Component(s) in the package:
 atlas_add_library( NSWCalibData
                    src/*.cxx
                    PUBLIC_HEADERS NSWCalibData
                    LINK_LIBRARIES Identifier MuonIdHelpersLib )

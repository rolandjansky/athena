################################################################################
#Package: NSWCalibData
################################################################################
 
# Declare the package name:
 atlas_subdir( NSWCalibData )
 
# Declare the package's dependencies:
 atlas_depends_on_subdirs( PUBLIC
                           Control/CLIDSvc
                           Control/DataModel
                           Control/SGTools
                           DetectorDescription/Identifier
                           MuonSpectrometer/MuonIdHelpers )
 
# External dependencies:
 find_package( CLHEP )
 
# Component(s) in the package:
 atlas_add_library( NSWCalibData
                    src/*.cxx
                    PUBLIC_HEADERS NSWCalibData
                    INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                    DEFINITIONS ${CLHEP_DEFINITIONS}
                    LINK_LIBRARIES ${CLHEP_LIBRARIES} DataModel SGTools Identifier MuonIdHelpersLib )


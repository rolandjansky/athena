################################################################################
# Package: MuonCalibNtuple
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibNtuple )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist pthread )

# Component(s) in the package:
atlas_add_library( MuonCalibNtuple
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibNtuple
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} MdtCalibFitters MuonCalibEventBase MuonRDO
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} MuonCalibIdentifier )


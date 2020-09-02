################################################################################
# Package: LArG4RunControl
################################################################################

# Declare the package name:
atlas_subdir( LArG4RunControl )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

string(REPLACE "-Wl,--as-needed" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")

# this line failed automatic conversion in cmt2cmake :
# macro_prepend LArG4RunControlDict_shlibflags " -lLArG4RunControl "

# Component(s) in the package:
atlas_add_library( LArG4RunControl
                   src/*.cxx
                   PUBLIC_HEADERS LArG4RunControl
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( LArG4RunControlDict
                      LArG4RunControl/LArG4RunControlDict.h
                      LArG4RunControl/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} StoreGateLib SGtests GaudiKernel LArG4RunControl )


################################################################################
# Package: TBEvent
################################################################################

# Declare the package name:
atlas_subdir( TBEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthLinks
                          Control/AthContainers
                          GaudiKernel
                          LArCalorimeter/LArRawEvent )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TBEvent
                   src/*.cxx
                   PUBLIC_HEADERS TBEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthLinks AthContainers AthenaKernel GaudiKernel LArRawEvent
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( TBEventDict
                      TBEvent/TBEventDict.h
                      TBEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks AthContainers GaudiKernel LArRawEvent TBEvent )


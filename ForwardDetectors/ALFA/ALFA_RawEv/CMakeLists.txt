################################################################################
# Package: ALFA_RawEv
################################################################################

# Declare the package name:
atlas_subdir( ALFA_RawEv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthContainers
                          Simulation/HitManagement
                          PRIVATE
                          Control/StoreGate
                          Event/EventContainers
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ALFA_RawEv
                   src/*.cxx
                   PUBLIC_HEADERS ALFA_RawEv
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers HitManagement StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES GaudiKernel EventContainers )

atlas_add_dictionary( ALFA_RawEvDict
                      ALFA_RawEv/ALFA_RawEvDict.h
                      ALFA_RawEv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers HitManagement StoreGateLib SGtests GaudiKernel ALFA_RawEv )


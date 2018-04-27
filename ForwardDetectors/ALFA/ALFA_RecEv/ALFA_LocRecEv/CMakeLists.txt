################################################################################
# Package: ALFA_LocRecEv
################################################################################

# Declare the package name:
atlas_subdir( ALFA_LocRecEv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthContainers
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ALFA_LocRecEv
                   src/*.cxx
                   PUBLIC_HEADERS ALFA_LocRecEv
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( ALFA_LocRecEvDict
                      ALFA_LocRecEv/ALFA_LocRecEvDict.h
                      ALFA_LocRecEv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers GaudiKernel ALFA_LocRecEv )


################################################################################
# Package: ALFA_LocRecCorrEv
################################################################################

# Declare the package name:
atlas_subdir( ALFA_LocRecCorrEv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthContainers
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ALFA_LocRecCorrEv
                   src/*.cxx
                   PUBLIC_HEADERS ALFA_LocRecCorrEv
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( ALFA_LocRecCorrEvDict
                      ALFA_LocRecCorrEv/ALFA_LocRecCorrEvDict.h
                      ALFA_LocRecCorrEv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers GaudiKernel ALFA_LocRecCorrEv )


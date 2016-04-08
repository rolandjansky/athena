################################################################################
# Package: ALFA_GloRecEv
################################################################################

# Declare the package name:
atlas_subdir( ALFA_GloRecEv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          ForwardDetectors/ALFA/ALFA_RecEv/ALFA_LocRecCorrEv
                          ForwardDetectors/ALFA/ALFA_RecEv/ALFA_LocRecEv
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ALFA_GloRecEv
                   src/*.cxx
                   PUBLIC_HEADERS ALFA_GloRecEv
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools ALFA_LocRecCorrEv ALFA_LocRecEv
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( ALFA_GloRecEvDict
                      ALFA_GloRecEv/ALFA_GloRecEvDict.h
                      ALFA_GloRecEv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools ALFA_LocRecCorrEv ALFA_LocRecEv GaudiKernel ALFA_GloRecEv )


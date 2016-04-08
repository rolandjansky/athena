################################################################################
# Package: ALFA_CLinkEv
################################################################################

# Declare the package name:
atlas_subdir( ALFA_CLinkEv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          Control/CLIDSvc
                          Control/SGTools
                          Database/AthenaPOOL/AthenaPoolUtilities
                          ForwardDetectors/ALFA/ALFA_RawEv
                          ForwardDetectors/ALFA/ALFA_RecEv/ALFA_LocRecCorrEv
                          ForwardDetectors/ALFA/ALFA_RecEv/ALFA_LocRecEv
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ALFA_CLinkEv
                   src/*.cxx
                   PUBLIC_HEADERS ALFA_CLinkEv
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthLinks SGTools AthenaPoolUtilities ALFA_RawEv ALFA_LocRecCorrEv ALFA_LocRecEv
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( ALFA_CLinkEvDict
                      ALFA_CLinkEv/ALFA_CLinkEvDict.h
                      ALFA_CLinkEv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks SGTools AthenaPoolUtilities ALFA_RawEv ALFA_LocRecCorrEv ALFA_LocRecEv GaudiKernel ALFA_CLinkEv )


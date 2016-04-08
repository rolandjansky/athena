################################################################################
# Package: AFP_LocRecoEv
################################################################################

# Declare the package name:
atlas_subdir( AFP_LocRecoEv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AFP_LocRecoEv
                   src/*.cxx
                   PUBLIC_HEADERS AFP_LocRecoEv
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( AFP_LocRecoEvDict
                      AFP_LocRecoEv/AFP_LocRecoEvDict.h
                      AFP_LocRecoEv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools GaudiKernel AFP_LocRecoEv )


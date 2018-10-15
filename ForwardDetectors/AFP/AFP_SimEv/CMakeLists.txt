################################################################################
# Package: AFP_SimEv
################################################################################

# Declare the package name:
atlas_subdir( AFP_SimEv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Simulation/HitManagement
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AFP_SimEv
                   src/*.cxx
                   PUBLIC_HEADERS AFP_SimEv
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES HitManagement
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( AFP_SimEvDict
                      AFP_SimEv/AFP_SimEvDict.h
                      AFP_SimEv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} HitManagement GaudiKernel AFP_SimEv )


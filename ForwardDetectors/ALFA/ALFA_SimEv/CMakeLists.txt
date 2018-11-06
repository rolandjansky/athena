################################################################################
# Package: ALFA_SimEv
################################################################################

# Declare the package name:
atlas_subdir( ALFA_SimEv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Simulation/HitManagement
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ALFA_SimEv
                   src/*.cxx
                   PUBLIC_HEADERS ALFA_SimEv
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel HitManagement
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( ALFA_SimEvDict
                      ALFA_SimEv/ALFA_SimEvDict.h
                      ALFA_SimEv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} HitManagement GaudiKernel ALFA_SimEv )


################################################################################
# Package: AraToolExample
################################################################################

# Declare the package name:
atlas_subdir( AraToolExample )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PhysicsAnalysis/AraTool
                          PRIVATE
                          Control/AthenaBaseComps )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AraToolExampleLib
                   src/*.cxx
                   PUBLIC_HEADERS AraToolExample
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES GaudiKernel AraTool
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps )

atlas_add_component( AraToolExample
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AraTool AthenaBaseComps AraToolExampleLib )

atlas_add_dictionary( AraToolExampleDict
                      AraToolExample/AraToolExampleDict.h
                      AraToolExample/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AraTool AthenaBaseComps AraToolExampleLib )


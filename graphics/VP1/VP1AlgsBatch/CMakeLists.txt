################################################################################
# Package: VP1AlgsBatch
################################################################################

# Declare the package name:
atlas_subdir( VP1AlgsBatch )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Database/AthenaPOOL/PoolSvc
                          GaudiKernel
                          PRIVATE
                          Database/APR/StorageSvc
                          Event/EventInfo
                          Tools/PathResolver
                          graphics/VP1/VP1UtilsBase )

# External dependencies:
find_package( Qt5 COMPONENTS Core OpenGL Gui HINTS ${QT5_ROOT} )



# Component(s) in the package:
atlas_add_component( VP1AlgsBatch
                     src/*.cxx
                     src/components/*.cxx
                   LINK_LIBRARIES ${QT5_LIBRARIES} GL AthenaBaseComps GaudiKernel StorageSvc EventInfo PathResolver VP1UtilsBase )

# Install files from the package:
atlas_install_headers( VP1AlgsBatch )
atlas_install_runtime( share/*.vp1 )


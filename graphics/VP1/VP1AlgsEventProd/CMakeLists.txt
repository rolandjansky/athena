################################################################################
# Package: VP1AlgsEventProd
################################################################################

# Declare the package name:
atlas_subdir( VP1AlgsEventProd )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Database/AthenaPOOL/PoolSvc
                          GaudiKernel
                          PRIVATE
                          Database/APR/CollectionBase
                          Database/APR/CollectionUtilities
                          Database/APR/POOLCore
                          Database/APR/PersistencySvc
                          Database/APR/StorageSvc
                          Event/EventInfo
                          Tools/PathResolver
                          graphics/VP1/VP1UtilsBase )

# External dependencies:
find_package( Qt4 COMPONENTS QtCore QtOpenGL QtGui )

include_directories( /usr/X11R6/include )

# Component(s) in the package:
atlas_add_component( VP1AlgsEventProd
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${QT4_INCLUDE_DIRS}
                     LINK_LIBRARIES ${QT4_LIBRARIES} GL AthenaBaseComps GaudiKernel CollectionBase CollectionUtilities POOLCore PersistencySvc StorageSvc EventInfo PathResolver VP1UtilsBase )

# Install files from the package:
atlas_install_headers( VP1AlgsEventProd )



################################################################################
# Package: AthenaPoolServices
################################################################################

# Declare the package name:
atlas_subdir( AthenaPoolServices )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/DataModelRoot
                          Database/APR/CollectionBase
                          Database/APR/CollectionUtilities
                          Database/APR/POOLCore
                          Database/APR/PersistencySvc
                          Database/APR/StorageSvc )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( AthenaRootStreamerSvc
                     src/AthenaRootConverterHandle.cxx
                     src/AthenaRootStreamer.cxx
                     src/AthenaRootStreamerSvc.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps DataModelRoot CollectionBase CollectionUtilities POOLCore PersistencySvc StorageSvc )

# Install files from the package:
atlas_install_headers( AthenaPoolServices )
atlas_install_joboptions( share/*.py )


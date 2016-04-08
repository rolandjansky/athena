################################################################################
# Package: TPTools
################################################################################

# Declare the package name:
atlas_subdir( TPTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          PRIVATE
                          Database/PersistentDataModel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TPTools
                   src/*.cxx
                   PUBLIC_HEADERS TPTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel AthenaPoolUtilities GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} PersistentDataModel )

atlas_add_dictionary( TPToolsDict
                      TPTools/TPToolsDict.h
                      TPTools/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel AthenaPoolUtilities GaudiKernel PersistentDataModel TPTools )


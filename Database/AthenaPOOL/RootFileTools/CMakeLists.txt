################################################################################
# Package: RootFileTools
################################################################################

# Declare the package name:
atlas_subdir( RootFileTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Database/PersistentDataModel )

# External dependencies:
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist pthread )

# Component(s) in the package:
atlas_add_executable( mergePOOL
                      src/merge.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps PersistentDataModel )

# Install files from the package:
atlas_install_python_modules( python/*.py )

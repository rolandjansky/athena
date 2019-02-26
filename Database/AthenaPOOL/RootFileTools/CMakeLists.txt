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

# this line failed automatic conversion in cmt2cmake :
# action releaseFileCache "$(cpp_name) $(src)releaseFileCache.cxx -o $(bin)releaseFileCache.exe;mkdir -p $(CMTINSTALLAREA)/$(tag)/bin; cd $(CMTINSTALLAREA)/$(tag)/bin;rm -f releaseFileCache.exe; ln -s ../../../Database/AthenaPOOL/RootFileTools/$(tag)/releaseFileCache.exe releaseFileCache.exe "

# Component(s) in the package:
atlas_add_executable( mergePOOL
                      src/merge.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps PersistentDataModel RootFileToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

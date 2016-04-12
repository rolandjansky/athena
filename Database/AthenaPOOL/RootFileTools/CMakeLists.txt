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
atlas_add_library( RootFileToolsLib
                   src/cobs.cxx
                   src/RootFileTransformationsTool.cxx
                   src/HistoPile.cxx
                   PUBLIC_HEADERS RootFileTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps PersistentDataModel )

atlas_add_component( RootFileTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps PersistentDataModel RootFileToolsLib )

atlas_add_dictionary( RootFileToolsDict
                      RootFileTools/RootFileToolsDict.h
                      RootFileTools/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps PersistentDataModel RootFileToolsLib )

atlas_add_executable( mergePOOL
                      src/merge.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps PersistentDataModel RootFileToolsLib )

atlas_add_executable( mergeD3PD
                      src/mergeD3PD.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps PersistentDataModel RootFileToolsLib )

atlas_add_executable( checkCache
                      src/checkCache.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps PersistentDataModel RootFileToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/orderFile.py share/optimizeBS.py share/showTrees.py )

# Aliases:
atlas_add_alias( optimizeBasketsSizes "optimizeBS.py" )
atlas_add_alias( orderFile "orderFile.py" )
atlas_add_alias( mergeD3PD "mergeD3PD.exe" )
atlas_add_alias( showTrees "showTrees.py" )


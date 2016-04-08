################################################################################
# Package: AsgTools
################################################################################

# Declare the package name:
atlas_subdir( AsgTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/SGTools
                          Database/IOVDbDataModel
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AsgTools
                   Root/MsgLevel.cxx
                   Root/AsgTool.cxx
                   Root/AsgMetadataTool.cxx
                   Root/AsgMessaging.cxx
                   Root/ToolStore.cxx
                   src/*.cxx
                   PUBLIC_HEADERS AsgTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps SGTools IOVDbDataModel GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( AsgToolsDict
                      AsgTools/AsgToolsDict.h
                      AsgTools/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps SGTools IOVDbDataModel GaudiKernel AsgTools )


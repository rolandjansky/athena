################################################################################
# Package: IOVDbDataModel
################################################################################

# Declare the package name:
atlas_subdir( IOVDbDataModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/SGTools
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          PRIVATE
                          Control/RootUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( IOVDbDataModel
                   src/*.cxx
                   PUBLIC_HEADERS IOVDbDataModel
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel SGTools AthenaPoolUtilities GaudiKernel RootUtilsPyROOT
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} RootUtils )

atlas_add_dictionary( IOVDbDataModelDict
                      IOVDbDataModel/IOVDbDataModelDict.h
                      IOVDbDataModel/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel SGTools AthenaPoolUtilities GaudiKernel RootUtils RootUtilsPyROOT IOVDbDataModel )


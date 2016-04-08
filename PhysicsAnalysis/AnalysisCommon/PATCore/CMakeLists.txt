################################################################################
# Package: PATCore
################################################################################

# Declare the package name:
atlas_subdir( PATCore )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthToolSupport/AsgTools
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( PATCoreLib
                   Root/*.cxx
                   PUBLIC_HEADERS PATCore
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AsgTools GaudiKernel )

atlas_add_dictionary( PATCoreDict
                      PATCore/PATCoreDict.h
                      PATCore/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AsgTools GaudiKernel PATCoreLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )


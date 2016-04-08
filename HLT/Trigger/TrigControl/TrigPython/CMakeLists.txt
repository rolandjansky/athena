################################################################################
# Package: TrigPython
################################################################################

# Declare the package name:
atlas_subdir( TrigPython )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          HLT/Trigger/TrigControl/TrigKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_dictionary( TrigPythonDict
                      TrigPython/TrigPythonDict.h
                      TrigPython/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TrigKernel )

# Install files from the package:
atlas_install_headers( TrigPython )


################################################################################
# Package: PyDumper
################################################################################

# Declare the package name:
atlas_subdir( PyDumper )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthenaPython
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS program_options regex )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_dictionary( PyDumperDictDict
                      PyDumper/PyDumperDict.h
                      PyDumper/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AthContainers TrkTruthData )

# Install files from the package:
atlas_install_headers( PyDumper )
atlas_install_python_modules( python/*.py )
atlas_install_scripts( bin/sg-dump.py )

# Aliases:
atlas_add_alias( sg-dump "sg-dump.py" )


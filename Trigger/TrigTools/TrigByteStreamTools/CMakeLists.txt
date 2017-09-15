################################################################################
# Package: TrigByteStreamTools
################################################################################

# Declare the package name:
atlas_subdir( TrigByteStreamTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common COMPONENTS CTPfragment )

# Component(s) in the package:
atlas_add_dictionary( TrigByteStreamToolsDict
                      TrigByteStreamTools/TrigByteStreamToolsDict.h
                      TrigByteStreamTools/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} )

# Install files from the package:
atlas_install_headers( TrigByteStreamTools )
atlas_install_python_modules( python/*.py )
atlas_install_scripts( bin/*.py python/addL1.py python/replaceMUCTPI.py python/trigbs_replaceLB.py python/trigbs_prescaleL1.py python/slimHLTBSFile.py )


################################################################################
# Package: MVAUtils
################################################################################

# Declare the package name:
atlas_subdir( MVAUtils )

# External dependencies:
find_package( ROOT COMPONENTS Tree TreePlayer TMVA XMLIO Core MathCore Hist RIO pthread )

atlas_depends_on_subdirs(
   PUBLIC
)

# Component(s) in the package:
atlas_add_library( MVAUtils
                   Root/*.cxx
                   PUBLIC_HEADERS MVAUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES})

atlas_add_dictionary( MVAUtilsDict
                      MVAUtils/MVAUtilsDict.h
                      MVAUtils/selection.xml
                      LINK_LIBRARIES MVAUtils)

atlas_install_python_modules( util/*.py )

atlas_add_executable( convertXmlToRootTree util/convertXmlToRootTree.cxx
		      INCLUDE_DIRS MVAUtils
		      LINK_LIBRARIES MVAUtils )

atlas_add_executable( check_timing_mvautils
                      util/check_timing_mvautils.cxx
                      INCLUDE_DIRS MVAUtils
                      LINK_LIBRARIES MVAUtils )

atlas_install_scripts( util/convertLGBMToRootTree.py
                       util/convertXGBoostToRootTree.py )

atlas_add_test( ut_test_MVAUtils.py
                SCRIPT test/ut_test_MVAUtils.py TestMVAUtilsBasic )

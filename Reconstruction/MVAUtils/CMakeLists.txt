################################################################################
# Package: MVAUtils
################################################################################

# Declare the package name:
atlas_subdir( MVAUtils )

atlas_depends_on_subdirs(
  PRIVATE
  Control/CxxUtils )

# External dependencies for main lib:
find_package( ROOT COMPONENTS Tree TMVA XMLIO Core MathCore RIO)

# Component(s) in the package:
atlas_add_library( MVAUtils
                   Root/*.cxx
                   PUBLIC_HEADERS MVAUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( MVAUtilsDict
                      MVAUtils/MVAUtilsDict.h
                      MVAUtils/selection.xml
                      LINK_LIBRARIES egammaMVACalibLib )
	      
# External ROOT dependencies for utils :
atlas_add_executable( convertXmlToRootTree util/convertXmlToRootTree.cxx
		      INCLUDE_DIRS MVAUtils ${ROOT_INCLUDE_DIRS}
		      LINK_LIBRARIES MVAUtils CxxUtils ${ROOT_LIBRARIES})



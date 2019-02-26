################################################################################
# Package: MVAUtils
################################################################################

# Declare the package name:
atlas_subdir( MVAUtils )

# External dependencies:
find_package( ROOT COMPONENTS Tree TreePlayer TMVA XMLIO Core MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MVAUtils
                   Root/*.cxx
                   PUBLIC_HEADERS MVAUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_executable( convertXmlToRootTree util/convertXmlToRootTree.cxx
		      INCLUDE_DIRS MVAUtils
		      LINK_LIBRARIES MVAUtils CxxUtils)


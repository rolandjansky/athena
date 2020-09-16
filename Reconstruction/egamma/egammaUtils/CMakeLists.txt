################################################################################
# Package: egammaUtils
################################################################################

# Declare the package name:
atlas_subdir( egammaUtils )

# External dependencies:
find_package( ROOT COMPONENTS Tree Core  Hist)

# Component(s in tne package:
atlas_add_library( egammaUtils
                   Root/*.cxx
                   PUBLIC_HEADERS egammaUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
		   PRIVATE_INCLUDE_DIRS 
		   LINK_LIBRARIES  ${ROOT_LIBRARIES} xAODCaloEvent xAODTracking xAODEgamma GeoPrimitives  
		   PRIVATE_LINK_LIBRARIES FourMomUtils PathResolver AnalysisUtilsLib)
	  
atlas_add_dictionary( egammaUtilsDict
		      egammaUtils/egammaUtilsDict.h
		      egammaUtils/selection.xml
		      LINK_LIBRARIES egammaUtils )

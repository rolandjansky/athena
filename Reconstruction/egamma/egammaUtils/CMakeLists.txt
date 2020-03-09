################################################################################
# Package: egammaUtils
################################################################################

# Declare the package name:
atlas_subdir( egammaUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
	                  Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODTracking
			  Event/xAOD/xAODEgamma
			  DetectorDescription/GeoPrimitives
                          PRIVATE
                          Event/FourMomUtils
			  PhysicsAnalysis/AnalysisCommon/AnalysisUtils
			  Tools/PathResolver)

# External dependencies:
find_package( ROOT COMPONENTS Tree Core  Hist)

# Component(s in tne package:
atlas_add_library( egammaUtils
                   Root/*.cxx
                   PUBLIC_HEADERS egammaUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
		   PRIVATE_INCLUDE_DIRS 
		   LINK_LIBRARIES  ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODTracking xAODEgamma GeoPrimitives  
		   PRIVATE_LINK_LIBRARIES FourMomUtils  PathResolver AnalysisUtilsLib)
	  
atlas_add_dictionary( egammaUtilsDict
		      egammaUtils/egammaUtilsDict.h
		      egammaUtils/selection.xml
		      LINK_LIBRARIES egammaUtils )

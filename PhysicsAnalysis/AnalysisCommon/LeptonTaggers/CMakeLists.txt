################################################################################
# Package: LeptonTaggers
################################################################################

# Declare the package name:
atlas_subdir( LeptonTaggers )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
			  GaudiKernel
			  DetectorDescription/GeoPrimitives
			  PRIVATE
   			  Control/AthenaBaseComps
			  Event/xAOD/xAODEgamma
			  Event/xAOD/xAODMuon
			  Event/xAOD/xAODJet
			  Event/xAOD/xAODTau
			  Event/xAOD/xAODTracking
                          Event/xAOD/xAODBTagging
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEventShape
			  Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
			  Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS TMVA Core )

# Component(s) in the package:
atlas_add_component( LeptonTaggers
		     src/*.h
		     src/*.cxx src/components/*.cxx
                     NO_PUBLIC_HEADERS
	   	     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}  
	   	     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel GeoPrimitives
		     PRIVATE_LINK_LIBRARIES AthenaBaseComps xAODEgamma 
                     xAODMuon xAODJet xAODTau xAODTracking xAODBTagging xAODCaloEvent xAODEventShape
                     InDetTrackSelectionToolLib
                     PathResolver )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py ) 

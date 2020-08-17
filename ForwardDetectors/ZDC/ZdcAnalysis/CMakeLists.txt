atlas_subdir( ZdcAnalysis )


atlas_depends_on_subdirs( PUBLIC
			  Control/AthToolSupport/AsgTools
			  GaudiKernel
			  Event/xAOD/xAODForward 
		   	  Event/xAOD/xAODTrigL1Calo
			  PRIVATE
       			  Event/xAOD/xAODEventInfo
			  Tools/PathResolver)

find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

atlas_add_component( ZdcAnalysis
		     src/components/*.cxx
	             INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
		     LINK_LIBRARIES ${ROOT_LIBRARIES}  AsgTools xAODForward xAODTrigL1Calo xAODEventInfo PathResolver ZdcAnalysisLib
		     )

atlas_add_library (ZdcAnalysisLib
		  Root/*.cxx
		  PUBLIC_HEADERS ZdcAnalysis
                  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                  LINK_LIBRARIES AsgDataHandlesLib
		  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODForward xAODTrigL1Calo xAODEventInfo PathResolver
		  )

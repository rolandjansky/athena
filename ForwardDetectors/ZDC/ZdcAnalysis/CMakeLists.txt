atlas_subdir( ZdcAnalysis )


if ( XAOD_STANDALONE )
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthToolSupport/AsgTools
      GaudiKernel
      Event/xAOD/xAODForward
      Event/xAOD/xAODTrigL1Calo
      PRIVATE
      Event/xAOD/xAODEventInfo
      Tools/PathResolver)
else()
   atlas_depends_on_subdirs( 
      PUBLIC
      Control/AthToolSupport/AsgTools
      GaudiKernel
      Event/xAOD/xAODForward 
      Event/xAOD/xAODTrigL1Calo
      PRIVATE
      Event/xAOD/xAODEventInfo
      Tools/PathResolver)	
endif()

find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

if( NOT XAOD_STANDALONE )
   atlas_add_component( ZdcAnalysis
      src/components/*.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES}  AsgTools xAODForward xAODTrigL1Calo xAODEventInfo PathResolver ZdcAnalysisLib   )
endif()

atlas_add_library (ZdcAnalysisLib
		  Root/*.cxx
		  PUBLIC_HEADERS ZdcAnalysis
                  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
		  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODForward xAODTrigL1Calo xAODEventInfo PathResolver
		  )


atlas_install_joboptions( share/*.py)
atlas_install_data( data/* )

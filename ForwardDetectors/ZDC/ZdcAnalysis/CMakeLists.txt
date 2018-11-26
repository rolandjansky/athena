atlas_subdir( ZdcAnalysis )

atlas_depends_on_subdirs(
  PUBLIC
  Control/AthToolSupport/AsgTools
  Event/xAOD/xAODForward
  Event/xAOD/xAODTrigL1Calo
  PRIVATE
  Tools/PathResolver
  Event/xAOD/xAODEventInfo )

find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

atlas_add_library (ZdcAnalysisLib
		  Root/*.cxx
		  PUBLIC_HEADERS ZdcAnalysis
                  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                  LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODForward xAODTrigL1Calo
		  PRIVATE_LINK_LIBRARIES xAODEventInfo PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( ZdcAnalysis
      src/components/*.cxx
      INCLUDE_DIRS
      LINK_LIBRARIES GaudiKernel ZdcAnalysisLib )
endif()

atlas_install_joboptions( share/*.py)
atlas_install_data( data/* )

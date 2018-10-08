################################################################################
# Package: DataQualityUtils
################################################################################

# Declare the package name:
atlas_subdir( DataQualityUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          DataQuality/DataQualityInterfaces
                          DataQuality/ZLumiScripts )

# External dependencies:
find_package( Boost COMPONENTS regex filesystem thread system )
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Graf Gpad MathCore RooFit RooFitCore Minuit Cint Core Tree Hist RIO pthread MathMore Minuit2 Matrix Physics HistPainter Rint Graf3d Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_root_dictionary( DataQualityUtils
                           DataQualityUtilsDictSource
                           ROOT_HEADERS DataQualityUtils/HanOutputFile.h DataQualityUtils/MonitoringFile.h DataQualityUtils/StatusFlagCOOL.h DataQualityUtils/StatusFlagCOOLBase.h DataQualityUtils/StatusFlagCommentCOOL.h DataQualityUtils/HistogramDataCOOL.h DataQualityUtils/CoolRpc.h DataQualityUtils/CoolMdt.h DataQualityUtils/CoolTgc.h DataQualityUtils/MdtDeadNoisyUtils.h DataQualityUtils/RootLinkDef.h
                           EXTERNAL_PACKAGES Boost COOL ROOT CORAL )

atlas_add_library( DataQualityUtils
                   src/HanOutputFile.cxx
                   src/MonitoringFile.cxx
                   src/MonitoringFile_CSCPostProcess.cxx
                   src/MonitoringFile_RPCPostProcess.cxx
                   src/MonitoringFile_MDTPostProcess.cxx
                   src/MonitoringFile_TGCPostProcess.cxx
                   src/MonitoringFile_MDTvsTGCPostProcess.cxx
                   src/MonitoringFile_PPFileWrapper.cxx
                   src/MonitoringFile_IDAlignPostProcess.cxx
                   src/MonitoringFile_IDPerfPostProcess.cxx
                   src/MonitoringFile_DiMuPostProcess.cxx
                   src/MonitoringFile_IDEnhancedPrimaryVertex.cxx
                   src/MonitoringFile_MoveVertexMonitoring.cxx
                   src/MonitoringFile_HLTMuonPostProcess.cxx
                   src/MonitoringFile_HLTEgammaPostProcess.cxx
                   src/MonitoringFile_HLTTauPostProcess.cxx
                   src/MonitoringFile_HLTMETPostProcess.cxx
                   src/MonitoringFile_HLTCaloPostProcess.cxx
                   src/MonitoringFile_HLTJetPostProcess.cxx
                   src/MonitoringFile_HLTMinBiasMonPostProcess.cxx
                   src/MonitoringFile_PixelPostProcess.cxx
                   src/MonitoringFile_SCTPostProcess.cxx
                   src/MonitoringFile_MuonTrackPostProcess.cxx
                   src/MonitoringFile_MergeAlgs.cxx
                   src/StatusFlagCOOL.cxx
                   src/StatusFlagCOOLBase.cxx
                   src/StatusFlagCommentCOOL.cxx
                   src/HistogramDataCOOL.cxx
                   src/CoolRpc.cxx
                   src/CoolMdt.cxx
                   src/CoolTgc.cxx
                   src/MdtDeadNoisyUtils.cxx
                   src/MonitoringFile_CSCChamberEfficiency.cxx
                   src/MonitoringFile_CSCClusterFit.cxx
                   src/MonitoringFile_TgcHistogramDivision.cxx
                   src/MonitoringFile_HLTMuonHistogramDivision.cxx
                   src/MonitoringFile_HLTEgammaEfficiency.cxx
                   src/MonitoringFile_HLTMETAveragePhivsEtaMaps.cxx
                   src/MonitoringFile_HLTMETDQFlagSummary.cxx
                   src/MonitoringFile_HLTJetCalcEfficiencyAndRate.cxx
                   src/MonitoringFile_HLTCaloAveragePtPhiEtaMaps.cxx
                   src/MonitoringFile_BJetTaggingPostProcess.cxx
                   src/MonitoringFile_BJetTaggingAdjustRanges.cxx
                   src/MonitoringFile_BJetTaggingNormalise.cxx
                   src/MonitoringFile_L1CaloPostProcess.cxx
                   ${DataQualityUtilsDictSource}
                   PUBLIC_HEADERS DataQualityUtils
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES DataQualityInterfaces )

atlas_add_executable( han-results-print
                      src/han_results_print.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} DataQualityInterfaces DataQualityUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/CreateDB.py scripts/CreateDB_Histo.py scripts/DQHistogramMerge.py scripts/DQHistogramPrintStatistics.py scripts/DQWebDisplay.py scripts/hancool_histo.py scripts/hancool.py scripts/handi.py scripts/historyDisplay.py scripts/DQFileMove.py scripts/CreateMDTConfig_algos.sh scripts/CreateMDTConfig_chambers.sh scripts/CreateMDTConfig_config.sh scripts/CreateMDTConfig_files.sh scripts/CreateMDTConfig_readme.sh scripts/DQM_Tier0Wrapper_trf.py scripts/DQM_Tier0Wrapper_tf.py scripts/DQHistogramMergeRegExp.py scripts/dq_make_web_display.py scripts/ScanHistFile.py scripts/physval_make_web_display.py )

# Aliases:
atlas_add_alias( DQHistogramPrintStatistics "DQHistogramPrintStatistics.py" )
atlas_add_alias( CreateMDTConfig_files "CreateMDTConfig_files.sh" )
atlas_add_alias( CreateMDTConfig_algos "CreateMDTConfig_algos.sh" )
atlas_add_alias( CreateDB_Histo "CreateDB_Histo.py" )
atlas_add_alias( hancool "hancool.py" )
atlas_add_alias( handi "handi.py" )
atlas_add_alias( han-results-print "han-results-print.exe" )
atlas_add_alias( CreateMDTConfig_config "CreateMDTConfig_config.sh" )
atlas_add_alias( hancool_histo "hancool_histo.py" )
atlas_add_alias( CreateMDTConfig_readme "CreateMDTConfig_readme.sh" )
atlas_add_alias( DQHistogramMerge "DQHistogramMerge.py" )
atlas_add_alias( DQWebDisplay "DQWebDisplay.py" )
atlas_add_alias( CreateMDTConfig_chambers "CreateMDTConfig_chambers.sh" )
atlas_add_alias( historyDisplay "historyDisplay.py" )


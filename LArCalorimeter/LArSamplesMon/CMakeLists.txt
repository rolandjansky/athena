################################################################################
# Package: LArSamplesMon
################################################################################

# Declare the package name:
atlas_subdir( LArSamplesMon )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )
find_package(TBB)
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase )


# Component(s) in the package:
atlas_add_library( LArSamplesMon
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     PUBLIC_HEADERS LArSamplesMon
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} CaloDetDescrLib CaloGeoHelpers CaloIdentifier AthenaMonitoringLib StoreGateLib SGtests Identifier GaudiKernel LArIdentifier LArRawEvent LArRecConditions LArCablingLib LWHists TrigDecisionToolLib CaloConditions AthenaKernel AthenaPoolUtilities xAODEventInfo LArCOOLConditions LArRawConditions LArRecEvent LArCafJobsLib )

# Component(s) in the package:
atlas_add_dictionary( LArSamplesMonDict
                      LArSamplesMon/LArSamplesMonDict.h
                      LArSamplesMon/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} LArCafJobsLib LArSamplesMon )

atlas_add_executable( LCE_CellList
                      src/LCE_CellList.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel LArSamplesMon LArCafJobsLib ) #LArSamplesMonLib LArCafJobsDict  

# Install files from the package:
atlas_install_headers( LArSamplesMon )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/LCE_postprocessing.py )


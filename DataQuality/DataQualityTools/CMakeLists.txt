################################################################################
# Package: DataQualityTools
################################################################################

# Declare the package name:
atlas_subdir( DataQualityTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# this line failed automatic conversion in cmt2cmake :
# macro_append cppflags ""  private

# Component(s) in the package:
atlas_add_component( DataQualityTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CaloGeoHelpers AthenaBaseComps AthenaMonitoringLib StoreGateLib SGtests xAODCaloEvent xAODEventInfo xAODJet xAODMissingET xAODMuon xAODTracking LUCID_RawEvent ZdcEvent ZdcIdentifier GaudiKernel InDetIdentifier InDetRawData InDetReadoutGeometry InDetPrepRawData LArRawEvent LArRecEvent MagFieldInterfaces MuonCalibITools MuonIdHelpersLib MuonRDO MuonRecHelperToolsLib TagEvent RecBackgroundEvent RecoToolInterfaces TileEvent LWHists TrkParameters TrkExInterfaces TrkVertexAnalysisUtilsLib TrigDecisionToolLib TrigCaloEvent TrigMuonEvent TrigParticle TrigT1Result MuonSelectorToolsLib MagFieldElements MagFieldConditions IsolationSelectionLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )


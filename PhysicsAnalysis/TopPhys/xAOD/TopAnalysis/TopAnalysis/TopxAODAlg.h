#ifndef TOPANALYSIS_TOPXAODALG_H_
#define TOPANALYSIS_TOPXAODALG_H_

/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#include <AnaAlgorithm/AnaAlgorithm.h>

#include "AsgTools/ToolHandle.h"
#include "TopAnalysis/AnalysisTrackingHelper.h"
#include "TopAnalysis/EventSaverBase.h"
#include "TopAnalysis/EventSelectionManager.h"
#include "TopDataPreparation/SampleXsection.h"
#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"
#include <memory>

class TFile;

namespace PMGTools {
class IPMGTruthWeightTool;
}

namespace top {

class TopConfig;
class ConfigurationSettings;
class TopToolStore;
class EventCleaningSelection;
class ObjectCollectionMaker;
class ParticleLevelLoader;
class CalcTtbarPartonHistory;
class PDFScaleFactorCalculator;
class TopEventMaker;
class ScaleFactorCalculator;
class TopObjectSelection;

class TopxAODAlg final : public EL::AnaAlgorithm {
public:
  TopxAODAlg(const std::string &name, ISvcLocator *pSvcLocator);
  StatusCode initialize() override;
  StatusCode beginInputFile() override;
  StatusCode execute() override;
  StatusCode endInputFile() override;
  StatusCode finalize() override;

private:
  std::shared_ptr<ConfigurationSettings> m_settings{nullptr};
  std::shared_ptr<TopConfig> m_config{nullptr};
  ToolHandle<top::TopToolStore> m_topTools;
  ToolHandle<top::EventCleaningSelection> m_eventCleaning;
  ToolHandle<top::ObjectCollectionMaker> m_systObjMaker;
  ToolHandle<top::TopObjectSelection> m_objectSelection;
  std::unique_ptr<top::EventSelectionManager> m_eventSelectionManager;
  std::unique_ptr<top::AnalysisTrackingHelper> m_tracker;
  ToolHandle<top::ParticleLevelLoader> m_particleLevelLoader;
  ToolHandle<top::CalcTtbarPartonHistory> m_topPartonHistory;
  ToolHandle<top::PDFScaleFactorCalculator> m_PDF_SF;
  ToolHandle<top::TopEventMaker> m_topEventMaker;
  ToolHandle<top::ScaleFactorCalculator> m_topScaleFactors;
  std::unique_ptr<top::EventSaverBase> m_eventSaver;
  ToolHandle<PMGTools::IPMGTruthWeightTool> m_pmg_weightTool{"PMGTruthWeightTool"};

  unsigned int m_doPerfStats = 0u;
  TFile *m_outputFile = nullptr;
  TTree *m_sumPdfWeights = nullptr;
  std::unordered_map<std::string, std::unique_ptr<std::vector<float>>> m_totalEventsPdfWeighted;
  bool m_pdfMetadataExists = false;
  double m_totalEventsWeighted = 0;
  double m_totalEventsWeighted_temp = 0;
  ULong64_t m_totalEvents = 0;
  ULong64_t m_totalEventsInFiles = 0;
  TTree *m_sumWeights = nullptr;
  bool m_recalc_LHE3 = false;
  bool m_recalculateNominalWeightSum = false;
  std::vector<std::string> m_names_LHE3;
  std::vector<double> m_totalEventsWeighted_LHE3;      // having doubles is necessary in case of
                                                       // re-calculation of the sum
  std::vector<double> m_totalEventsWeighted_LHE3_temp; // having doubles is necessary in case of
                                                       // re-calculation of the sum
  unsigned int m_entry = 0u;
  unsigned int m_firstEvent = 0u;
  unsigned int m_totalYield = 0u;
  unsigned int m_totalYieldSoFar = 0u;
  unsigned int m_skippedEventsSoFar = 0u;
  unsigned int m_eventSavedReco = 0u;
  unsigned int m_eventSavedRecoLoose = 0u;
  unsigned int m_eventSavedTruth = 0u;
  unsigned int m_eventSavedParticle = 0u;
  bool m_isFirst = true;

  unsigned int m_DSID;
  int m_isAFII;
  std::unordered_map<std::string, std::vector<std::string>> m_boostedTaggersSFSysNames;

  /// properties
  std::string m_configPath{""};
  std::string m_libraryNames{""};
  std::string m_derivationStream{""};
  bool m_crashOnMetaDataFail{true}; // if true, crash if metadata broken
  std::string m_streamName{"output"};
  unsigned int m_FTAGMapIndex;
  SampleXsection::showering m_showeringAlgo;
  std::string m_year;
};

} // namespace top

#endif

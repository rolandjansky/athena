/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef _TOPANALYSIS_ANALYSISTRACKINGHELPER_H_
#define _TOPANALYSIS_ANALYSISTRACKINGHELPER_H_

#include <memory>
#include <string>
#include <vector>

namespace top {
  class TopConfig;
}

namespace top {
/**
 * @brief Helper for collecting data for analysis tracking.
 */
  class AnalysisTrackingHelper {
  public:
    AnalysisTrackingHelper();
    ~AnalysisTrackingHelper();

    /**
     * @brief Notify helper that an input file was processed.
     *
     * @param path Path to the input file.
     * @param entriesProcessed Number of entries that were processed.
     */
    void addInputFile(std::string const& path, unsigned long long entriesProcessed);

    /**
     * @brief Notify helper about configuration options.
     *
     * @param topConfig Pointer to the TopConfig object.
     */
    void setTopConfig(std::shared_ptr<TopConfig> const& topConfig);

    /**
     * @brief Store tracking data in current TFile.
     *
     * @param treename Name of the TTree used to store the data.
     *
     * @remarks
     *    It's your responsibility to call TFile::cd() before this function.
     */
    void writeTree(std::string const& treename);
  private:
    struct InputFileInfo {
      InputFileInfo(std::string const& path, unsigned long long const entriesProcessed) :
        path(path),
        entriesProcessed(entriesProcessed) {
      }

      std::string const path;
      unsigned long long const entriesProcessed;
    };

    void finish();

    std::string m_json;
    bool m_finished;
    std::shared_ptr<TopConfig> m_topConfig;
    std::vector<InputFileInfo> m_inputFiles;
  };
}

#endif

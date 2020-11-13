/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_HistogramFiller_OfflineHistogramProvider_h
#define AthenaMonitoringKernel_HistogramFiller_OfflineHistogramProvider_h

#include <memory>

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaMonitoringKernel/HistogramDef.h"
#include "AthenaMonitoringKernel/IHistogramProvider.h"

#include "GaudiKernel/ContextSpecificPtr.h"

#include "HistogramFactory.h"

#include "TTree.h"

// this mutex is used to protect access to the metadata trees
namespace {
  static std::mutex s_metadataMutex;
}

namespace Monitored {
  /**
   * @brief Implementation of IHistogramProvider for offline histograms
   */
  class OfflineHistogramProvider : public IHistogramProvider {
  public:
    /**
     * @brief Constructor
     *
     * @param gmTool Source of the lumi block info
     * @param factory ROOT object factory
     * @param def General definition of a histogram
     */
    OfflineHistogramProvider(GenericMonitoringTool* const gmTool,
                             std::shared_ptr<HistogramFactory> factory,
                             const HistogramDef& histDef)
    : IHistogramProvider()
    , m_gmTool(gmTool)
    , m_factory(factory)
    , m_histDef(new HistogramDef(histDef))
    , m_objcache({0, 0, nullptr})
    {}

    // store metadata trees on object destruction
    virtual ~OfflineHistogramProvider() override { storeMetadata(); }

    /**
     * @brief Getter of ROOT object
     *
     * Each time the method is called, factory produces ROOT object based on the current
     * lumi block. Note: ROOT objects are cached at the factory. Nevertheless, it is 
     * recommended to call this method as rarely as possible.
     *
     * @return ROOT object
     */
    TNamed* histogram() override {
      const unsigned runNumber = m_gmTool->runNumber();
      const unsigned lumiBlock = m_gmTool->lumiBlock();

      // do we have the object already?
      std::scoped_lock lock(m_cacheMutex);
      objcache& objcacheref = m_objcache;
      if (objcacheref.lumiBlock == lumiBlock
	        && objcacheref.runNumber == runNumber
	        && objcacheref.object) {
	      return objcacheref.object;
      }

      std::string lbString;
      HistogramDef::RunPeriod period = m_histDef->runperiod;
      if ( period == HistogramDef::RunPeriod::Run ) {
        lbString = "";
      } else if ( period == HistogramDef::RunPeriod::LowStat ) {
        const unsigned lbBase = lumiBlock-(((int64_t)lumiBlock-1)%20);
        lbString = "/lowStat_LB"+std::to_string(lbBase)+"-"+std::to_string(lbBase+19);
      } else {
        lbString = "/lb_"+std::to_string(lumiBlock);
      }
      m_histDef->tld = "/run_"+std::to_string(runNumber)+lbString+"/";

      objcacheref.lumiBlock = lumiBlock;
      objcacheref.runNumber = runNumber;
      objcacheref.object = m_factory->create(*m_histDef);
      const auto fullName = m_factory->getFullName(*m_histDef);
      if (std::find(m_storedPaths.begin(), m_storedPaths.end(), fullName) == m_storedPaths.end()) {
        m_storedPaths.push_back(fullName);
      }
      return objcacheref.object;
    }

  private:
    GenericMonitoringTool* const m_gmTool;
    std::shared_ptr<HistogramFactory> m_factory;
    std::shared_ptr<HistogramDef> m_histDef;

    struct objcache {
      unsigned int runNumber;
      unsigned int lumiBlock;
      TNamed* object;
    };
    mutable Gaudi::Hive::ContextSpecificData<objcache> m_objcache ATLAS_THREAD_SAFE;
    std::mutex m_cacheMutex;

    std::vector<std::string> m_storedPaths;

    /**
     * @brief Store metadata trees
     *
     * Offline ROOT output should have "metadata" TTrees; this function makes them
     *
     */ 
    void storeMetadata() const {
      std::scoped_lock<std::mutex> metadataLock(s_metadataMutex);
      for (const auto &path : m_storedPaths) {
        //std::cout << "Path " << path << std::endl;
        size_t pos = path.find_last_of('/');
        auto splitPath = std::make_pair(path.substr(0, pos), path.substr(pos + 1));
        std::string treePath = splitPath.first + "/metadata";
        auto &histSvc = m_gmTool->histogramService();
        std::string interval;
        char triggerData[] = "<none>";
        const std::string mergeDataStr = m_histDef->merge == "" ? "<default>" : m_histDef->merge;
        std::vector<char> mergeData{mergeDataStr.begin(), mergeDataStr.end()};
        mergeData.push_back('\0');

        HistogramDef::RunPeriod period = m_histDef->runperiod;
        if (period == HistogramDef::RunPeriod::Run) {
          interval = "run";
        } else if (period == HistogramDef::RunPeriod::LowStat) {
          interval = "lowStat";
        } else {
          interval = "lumiBlock";
        }
        if (!histSvc->existsTree(treePath)) {
          auto tree = std::make_unique<TTree>("metadata", "Monitoring Metadata");

          tree->Branch("Name", &(splitPath.second[0]), "Name/C");
          tree->Branch("Interval", &(interval[0]), "Interval/C");
          tree->Branch("TriggerChain", triggerData, "TriggerChain/C");
          tree->Branch("MergeMethod", mergeData.data(), "MergeMethod/C");
          tree->Fill();

          if (!histSvc->regTree(treePath, std::move(tree))) {
            MsgStream log(Athena::getMessageSvc(), "OfflineHistogramProvider");
            log << MSG::ERROR
                << "Failed to register DQ metadata TTree " << treePath << endmsg;
          }
        } else {
          TTree *tree{nullptr};
          if (histSvc->getTree(treePath, tree).isSuccess()) {
            tree->SetBranchAddress("Name", &(splitPath.second[0]));
            tree->SetBranchAddress("Interval", &(interval[0]));
            tree->SetBranchAddress("TriggerChain", triggerData);
            tree->SetBranchAddress("MergeMethod", mergeData.data());
            tree->Fill();
          } else {
            MsgStream log(Athena::getMessageSvc(), "OfflineHistogramProvider");
            log << MSG::ERROR
                << "Failed to retrieve DQ metadata TTree " << treePath << " which is reported to exist" << endmsg;
          }
        }
      }
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_OfflineHistogramProvider_h */

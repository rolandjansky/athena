/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "TopAnalysis/AnalysisTrackingHelper.h"

#include "TopConfiguration/ConfigurationSettings.h"
#include "TopConfiguration/JsonUtils.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyMgr.h"
#include "AsgTools/ToolStore.h"
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

#include <TTree.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/optional.hpp>
#include <boost/regex.hpp>
#include <boost/scope_exit.hpp>

#include <cstdlib>
#include <fstream>
#include <functional>
#include <set>
#include <sstream>
#include <stdexcept>

namespace {
/**
 * @brief List all tools registered in asg::ToolStore.
 */
  std::vector<std::string> listToolStore() {
    std::stringstream buffer;
    {
      auto&& stdoutbuf = std::cout.rdbuf(buffer.rdbuf());
      BOOST_SCOPE_EXIT(&stdoutbuf) {
        std::cout.rdbuf(stdoutbuf);
      } BOOST_SCOPE_EXIT_END
      asg::ToolStore::dumpToolConfig();
    }
    std::vector<std::string> result;
    {
      boost::regex re("(\\S+)\\s+=\\s+[^/]+/\\1\\s*");
      for (std::string line; std::getline(buffer, line); ) {
        boost::cmatch m;
        if (boost::regex_match(line.c_str(), m, re)) {
          result.push_back(m[1]);
        }
      }
    }
    return result;
  }
}

namespace top {
  AnalysisTrackingHelper::AnalysisTrackingHelper() :
    m_finished(false)
  { }


  AnalysisTrackingHelper::~AnalysisTrackingHelper() { }


  void AnalysisTrackingHelper::addInputFile(std::string const& path, unsigned long long entriesProcessed) {
    top::check(!m_finished, "AnalysisTrackingHelper::addInputFile called on finished instance");
    char* fullpath = realpath(path.c_str(), NULL);
    BOOST_SCOPE_EXIT(fullpath) {
      free(fullpath);
    } BOOST_SCOPE_EXIT_END
    m_inputFiles.emplace_back((fullpath == nullptr ? path.c_str() : fullpath), entriesProcessed);
  }

  void AnalysisTrackingHelper::setTopConfig(std::shared_ptr<top::TopConfig> const& topConfig) {
    top::check(!m_finished, "AnalysisTrackingHelper::setTopConfig called on finished instance");
    m_topConfig = topConfig;
  }

  void AnalysisTrackingHelper::writeTree(std::string const& treename) {
    finish();
    TTree* t = new TTree(treename.c_str(), "");
    t->SetAutoSave(0);
    t->SetAutoFlush(0);
    t->Branch("jsonData", &m_json, 262144);
    t->Fill();
    t->Write();
  }

  void AnalysisTrackingHelper::finish() {
    std::ostringstream out;

    std::string const json_null("null");
    out << "{\n";
    {
      // Store settings from configuration file:
      out << json_dump("configSettings") << ": {";
      auto const& config = *top::ConfigurationSettings::get();
      char const* sep = "\n ";
      for (auto&& kv : config.stringData()) {
        if (!kv.second.m_set) continue;
        out << sep << json_dump(kv.first) << ": " << json_dump(kv.second.m_data);
        sep = ",\n ";
      }
      out << "},\n";
    }
    {
      // Store small subset of environment variables:
      out << json_dump("environment") << ": {";
      char const* sep = "\n ";
      auto dumpEnvVar = [&](char const* name) {
                          char const* value = std::getenv(name);

                          out << sep << json_dump(name) << ": " << (value == nullptr ? json_null : json_dump(value));
                          sep = ",\n ";
                        };
      dumpEnvVar("AnalysisBase_DIR");
      dumpEnvVar("AnalysisBase_VERSION");
      dumpEnvVar("AnalysisBaseExternals_DIR");
      dumpEnvVar("AnalysisBaseExternals_VERSION");
      dumpEnvVar("AnalysisTop_DIR");
      dumpEnvVar("AnalysisTop_VERSION");
      dumpEnvVar("AtlasVersion");
      dumpEnvVar("HOSTNAME");
      dumpEnvVar("PanDA_TaskID");
      dumpEnvVar("PandaID");
      out << "},\n";
    }
    {
      // Store metadata of inputs:
      out << json_dump("inputConfig") << ": {\n";
      out << " " << json_dump("derivationFormat") << ": " << json_dump(m_topConfig->getDerivationStream()) << ",\n";
      out << " " << json_dump("dsid") << ": " << json_dump(m_topConfig->getDSID()) << ",\n";
      out << " " << json_dump("isMC") << ": " << json_dump(m_topConfig->isMC()) << ",\n";
      out << " " << json_dump("isAtlFast2") << ": " << json_dump(m_topConfig->isAFII()) << "\n";
      out << " " << json_dump("BtagCDIPath") << ": " << json_dump(m_topConfig->bTaggingCDIPath()) << "\n";
      out << "},\n";
    }
    {
      // List paths of loaded libraries, grouping them by directory:
      bool ok;
      std::map<std::string, std::set<std::string> > libs;
      try {
        boost::regex reLine("\\s*(?:\\S+\\s+){5}(/.*)");
        boost::cmatch m;
        std::ifstream stream("/proc/self/maps", std::ios_base::in | std::ios_base::binary);
        for (std::string line; std::getline(stream, line); ) {
          if (boost::regex_match(line.c_str(), m, reLine)) {
            std::string const& path = m[1];
            size_t i = path.rfind('/');
            assert(i != std::string::npos);
            std::string const& basename = path.substr(i + 1);
            if (boost::starts_with(basename, "lib")) {
              std::string dirpath = path.substr(0, (i ? i : 1));
              libs[dirpath].insert(basename);
            }
          }
        }
        stream.close();
        ok = true;
      }
      catch (std::exception const&) {
        ok = false;
      }
      // Store sorted list of loaded libraries (if listing was successful):
      if (ok) {
        out << json_dump("images") << ": [";
        char const* sep1 = "\n ";
        for (auto&& lib : libs) {
          out << sep1 << "[" << json_dump(lib.first) << ", [";
          sep1 = ",\n ";
          char const* sep2 = "\n  ";
          for (auto&& basename : lib.second) {
            out << sep2 << json_dump(basename);
            sep2 = ",\n  ";
          }
          out << "]]";
        }
        out << "],\n";
      }
    }
    {
      // Store information about processed input files:
      out << json_dump("inputFiles") << ": [";
      char const* sep = "\n ";
      for (auto const& inputFile : m_inputFiles) {
        out << sep << "[" << json_dump(inputFile.path) << "," << json_dump(inputFile.entriesProcessed) << "]";
        sep = ",\n ";
      }
      out << "],\n";
    }
    {
      // Store various PerfStats:
      auto&& readStats = xAOD::IOStats::instance().stats();
      out << json_dump("perfStats") << ": {\n";
      {
        out << " " << json_dump("readEntriesByContainer") << ": {";
        char const* sep = "\n  ";
        for (auto&& kv : readStats.containers()) {
          xAOD::BranchStats const& bs = kv.second;
          if (boost::ends_with(bs.GetName(), "TDS") || boost::ends_with(bs.GetName(), "Aux.")) continue;
          if (bs.readEntries()) {
            out << sep << json_dump(bs.GetName()) << ": " << json_dump(bs.readEntries());
            sep = ",\n  ";
          }
        }
        out << "},\n";
      }
      {
        out << " " << json_dump("readEntriesByBranch") << ": {";
        char const* sep = "\n  ";
        for (auto&& kv : readStats.branches()) {
          for (xAOD::BranchStats const* bs : kv.second) {
            if (bs && bs->readEntries()) {
              out << sep << json_dump(bs->GetName()) << ": " << json_dump(bs->readEntries());
              sep = ",\n  ";
            }
          }
        }
        out << "},\n";
      }
      out << " " << json_dump("totalEntries") << ": " << json_dump(readStats.nEvents()) << "},\n";
    }
    {
      // Store configuration of all (CP) tools:
      out << json_dump("toolConfig") << ": {";
      PropertyValueJsonDumper json_dump_prop;
      char const* toolsep = "\n ";
      for (std::string const& toolname : listToolStore()) {
        auto&& tool = asg::ToolStore::get<asg::AsgTool>(toolname);
        if (tool == nullptr) continue;
        if (boost::starts_with(tool->name(), "top::")) continue;
        out << toolsep << json_dump(tool->name()) << ": {";
        auto&& props = *tool->getPropertyMgr();
        char const* propsep = "\n  ";
        for (auto&& prop : props.getProperties()) {
          if (!prop.second->isValid()) continue;
          auto&& jvalue = json_dump_prop(prop.second);
          if (jvalue == boost::none) continue;
          out << propsep << json_dump(prop.first) << ": " << *jvalue;
          propsep = ",\n  ";
        }
        out << "}";
        toolsep = ",\n ";
      }
      out << "},\n";
    }
    out << json_dump("format") << ": " << json_dump(1) << "\n}\n";
    out.str().swap(m_json);
    m_finished = true;
  }
}

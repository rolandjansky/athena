/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopCorrections/TopCorrectionsTools.h"
#include <iostream>

#include "TopCorrections/MsgCategory.h"
using namespace TopCorrections;

namespace top {
  // B-tagging naming tool
  std::string bTagNamedSystCheck(std::shared_ptr<top::TopConfig> config,
                                 const std::string& systCollection,
                                 const std::string& tagWP,
                                 const bool isTrackJet,
                                 const bool verbose = false) {
    //  TODO -- to be reworked to use new message level settings to be implemented
    if (verbose) ATH_MSG_INFO("Searching for " << systCollection << " for WP " << tagWP);
    if (isTrackJet) {
      for (const std::string& name : config->trkjet_btagging_namedSysts(tagWP)) {
        if (verbose) ATH_MSG_INFO("...Possible matches : ");
        if (systCollection.find(betterBtagNamedSyst(name)) != std::string::npos) {
          if (verbose) ATH_MSG_INFO("......Matched!");
          if (systCollection.find("up") != std::string::npos) return name + "__1up";
          else if (systCollection.find("down") != std::string::npos) return name + "__1down";
          else return name;
        }
      }
    } else {
      for (const std::string& name : config->btagging_namedSysts(tagWP)) {
        if (verbose) ATH_MSG_INFO("...Possible matches : ");
        if (systCollection.find(betterBtagNamedSyst(name)) != std::string::npos) {
          if (verbose) ATH_MSG_INFO("......Matched!");
          if (systCollection.find("up") != std::string::npos) return name + "__1up";
          else if (systCollection.find("down") != std::string::npos) return name + "__1down";
          else return name;
        }
      }
    }
    return "";
  }

  // B-tagging naming tool
  std::string betterBtagNamedSyst(const std::string& name) {
    // remove "FT_EFF_", spaces, and "-" in named systematics
    std::string str = "FT_EFF_";
    std::string out = name;
    if (out.find(str) != std::string::npos) {
      out.replace(out.find(str), str.length(), "");
    }
    str = " ";
    while (out.find(str) != std::string::npos) {
      out.replace(out.find(str), str.length(), "_");
    }
    str = "-";
    while (out.find(str) != std::string::npos) {
      out.replace(out.find(str), str.length(), "_");
    }
    return out;
  }
}

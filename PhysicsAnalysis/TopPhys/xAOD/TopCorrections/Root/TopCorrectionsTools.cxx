/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopCorrections/TopCorrectionsTools.h"
#include <iostream>

namespace top {
  // B-tagging naming tool
  std::string bTagNamedSystCheck(std::shared_ptr<top::TopConfig> config, const std::string systCollection,
                                 const std::string tagWP, bool verbose = false) {
    if (verbose) std::cout << "Searching for " << systCollection << " for WP " << tagWP << std::endl;
    for (auto name : config->btagging_namedSysts(tagWP)) {
      if (verbose) std::cout << "...Possible matches : " << name << std::endl;
      if (systCollection.find(betterBtagNamedSyst(name)) != std::string::npos) {
        if (verbose) std::cout << "......Matched!" << std::endl;
        if (systCollection.find("up") != std::string::npos) return name + "__1up";
        else if (systCollection.find("down") != std::string::npos) return name + "__1down";
        else return name;
      }
    }
    return "";
  }

  // B-tagging naming tool
  std::string betterBtagNamedSyst(const std::string name) {
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

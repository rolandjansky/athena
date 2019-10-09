/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "getTree.h"

#include "TFile.h"
#include <stdexcept>
#include <set>
#include <fstream>
#include <regex>
#include <memory>

#include "TKey.h"

namespace {
  bool is_remote(const std::string& file_name) {
    std::regex remote_check("[a-z]+:.*");
    if (std::regex_match(file_name, remote_check)) {
      return true;
    }
    return false;
  }
  bool exists(const std::string& file_name) {
    std::ifstream file(file_name.c_str(), std::ios::binary);
    if (!file) {
      file.close();
      return false;
    }
    file.close();
    return true;
  }
}

namespace H5Utils {
  std::string getTree(const std::string& file_name) {
    if (!exists(file_name) && !is_remote(file_name)) {
      throw std::logic_error(file_name + " doesn't exist");
    }
    std::unique_ptr<TFile> file(TFile::Open(file_name.c_str()));
    if (!file || !file->IsOpen() || file->IsZombie()) {
      throw std::logic_error("can't open " + file_name);
    }
    std::set<std::string> keys;
    int n_keys = file->GetListOfKeys()->GetSize();
    if (n_keys == 0) {
      throw std::logic_error("no keys found in file");
    }
    for (int keyn = 0; keyn < n_keys; keyn++) {
      keys.insert(file->GetListOfKeys()->At(keyn)->GetName());
    }
    size_t n_unique = keys.size();
    if (n_unique > 1) {
      std::string prob = "Can't decide which tree to use, choose one of {";
      size_t uniq_n = 0;
      for (const auto& key: keys) {
        prob.append(key);
        uniq_n++;
        if (uniq_n < n_unique) prob.append(", ");
      }
      prob.append("} with the --tree-name option");
      throw std::logic_error(prob);
    }
    auto* key = dynamic_cast<TKey*>(file->GetListOfKeys()->At(0));
    std::string name = key->GetName();
    file->Close();
    return name;
  }

}

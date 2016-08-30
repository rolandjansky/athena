/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
// author Quentin Buat <quentin.buat@no.spam.cern.ch>
// author Geert-Jan Besjes <geert-jan.besjes@no.spam.cern.ch>
#ifndef HLTCHAIN_HLTCHAIN_H
#define HLTCHAIN_HLTCHAIN_H

#include <map>
#include <vector>
#include "TrigTauEmulation/HltItem.h"

class HltChain
{
  private:

    std::vector<HltItem> m_items;
    std::string m_l1_accept;
    int m_n_tau_items;

  public:

    HltChain(std::vector<HltItem> items, std::string l1_accept) {
      m_items = items;
      m_l1_accept = l1_accept;

      m_n_tau_items = 0;
      for (auto &it: m_items) 
        if (it.isTau()){
          ++m_n_tau_items;
        }
    }
    HltChain() {
      m_n_tau_items = 0;
    };

    void addHltItem(const HltItem &item) {
      m_items.push_back(item);
      if(item.isTau()) {
        ++m_n_tau_items;
      }
    }

    void addHltItem(const std::string& item_name, const std::string& item_seed){
      addHltItem(HltItem(item_name, item_seed));
    }

    std::vector<HltItem> items() { return m_items; }
    std::string l1_accept() { return m_l1_accept; }
    int n_tau_items() { return m_n_tau_items; }

};

typedef std::map<std::string, HltChain> HltChainRegistry;
namespace HltParsing {
  HltChainRegistry chains();
}

#endif


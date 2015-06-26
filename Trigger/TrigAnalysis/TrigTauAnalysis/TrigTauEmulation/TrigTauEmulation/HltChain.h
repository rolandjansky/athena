/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// author Quentin Buat <quentin.buat@no.spam.cern.ch>
#ifndef HLTCHAIN_HLTCHAIN_H
#define HLTCHAIN_HLTCHAIN_H

#include <map>
#include <vector>


 class HltItem
{

 private:

  std::string m_name;
  std::string m_l1_seed;

 public:
  
  HltItem(std::string name, std::string seed_name)
    {
    m_name = name;
    m_l1_seed = seed_name;
    }
  

  std::string name() {return m_name;}
  std::string seed() {return m_l1_seed;}
  bool isTau() 
  { 
    return (m_name.find("tau") != std::string::npos ? true : false);
  }
};


class HltChain
{

 private:

  std::vector<HltItem> m_items;
  std::string m_l1_accept;
  int m_tau_items;

 public:
  
  HltChain(std::vector<HltItem> items, std::string l1_accept) 
    {
      m_items = items;
      m_l1_accept = l1_accept;

      m_tau_items = 0;
      for (auto &it: m_items) 
	if (it.name().find("tau") != std::string::npos)
	  m_tau_items += 1;

    }
  HltChain() {;}

  std::vector<HltItem> items() {return m_items;}
  std::string l1_accept() {return m_l1_accept;}
  int n_tau_items() {return m_tau_items;}

};

namespace HltParsing {
  
  std::map<std::string, HltChain> chains();
}

#endif


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ChainEntry_h__
#define __ChainEntry_h__
/*
  ChainEntry.h
*/
#include <string>
#include <vector>
#include <map>
#include <ostream>

class ChainEntry {
public:
  enum RoIType {
    kUnknown=0,
    kMuonRoIType=1, 
    kElectronRoIType=2, 
    kTauRoIType=3, 
    kJetRoIType=4, 
    kBjetRoIType=5, 
    kMETRoIType=6, 
    kBphysicsRoIType=7, 

    kRoIType_L2_mu=1001, 
    kRoIType_L2_e=1002, 
    kRoIType_L2_tau=1003, 
    kRoIType_L2_j=1004, 
    kRoIType_L2_b=1005, 
    kRoIType_L2_xe=1006, 
    kRoIType_L2_DiMu=1007, 
    kRoIType_L2_TileMu=1010, 

    kRoIType_EF_mu=2001, 
    kRoIType_EF_e=2002, 
    kRoIType_EF_tau=2003, 
    kRoIType_EF_j=2004, 
    kRoIType_EF_b=2005, 
    kRoIType_EF_xe=2006, 
    kRoIType_EF_DiMu=2007, 
    kRoIType_EF_TileMu=2010

  };
  static const int kEFChainIdOffset = 10000;

  static const std::string chainName(int id);
  static int chainId(const std::string& chain_name);
  static const std::map<int, std::string>& chainIdMap();
  static void addChain(const std::string& chain_name, int id=-1);

private:
  static std::map<int, std::string> sChainIdMap;
  //  friend class RoILinksCnvTool;

public:
  ChainEntry(const std::string& chain_name="", int chain_counter=0);
  ~ChainEntry();

  void addRoI(int type, int index);
  const std::string& getName() const { return Name; }
  int getCounter() const { return Counter; }
  const std::vector<int>& getRoITypes() const { return RoITypes; }
  const std::vector<int>& getRoIIndices() const { return RoIIndices; }

  void dump() const;

private:
  std::string Name;                 // chain name
  int Counter;                      // = chain counter
  std::vector<int> RoITypes;        // Type of RoI
  std::vector<int> RoIIndices;      // index of RoILinks
};

std::ostream& operator<<(std::ostream& o, const ChainEntry& x);

#endif // __ChainEntry_h__

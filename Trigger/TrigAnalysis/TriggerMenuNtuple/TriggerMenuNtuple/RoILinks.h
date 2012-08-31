/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __RoILinks_h__
#define __RoILinks_h__
/*
  RoILinks.h
*/
#include <vector>
#include "TriggerMenuNtuple/MuonRoILinks.h"
#include "TriggerMenuNtuple/ElectronRoILinks.h"
#include "TriggerMenuNtuple/TauRoILinks.h"
#include "TriggerMenuNtuple/JetRoILinks.h"
#include "TriggerMenuNtuple/CombLinks.h"

class ChainEntry;

class RoILinks {
public:
  RoILinks();
  ~RoILinks();

  int addMuonRoILinks(const MuonRoILinks& MuonRoI);
  int addMuonRoILinks(const FeatureIndex& index_MuonFeature, 
		      const std::vector<FeatureIndex>& index_InDetTracks, 
		      const FeatureIndex& index_CombinedMuonFeature);
  int addMuonRoILinks(int index_MuonFeature, 
		      const std::vector<int>& index_InDetTracks, 
		      int index_CombinedMuonFeature,
		      int status_MuonFeature, 
		      int status_InDetTracks, 
		      int status_CombinedMuonFeature);
  int addElectronRoILinks(const ElectronRoILinks& ElectronRoI);
  int addElectronRoILinks(const FeatureIndex& index_EMCluster, 
			  const std::vector<FeatureIndex>& index_InDetTracks, 
			  const std::vector<FeatureIndex>& index_TrigElectrons);
  int addElectronRoILinks(int index_EMCluster, 
			  const std::vector<int>& index_InDetTracks, 
			  const std::vector<int>& index_TrigElectron,
			  const std::vector<int>& index_Egamma,
			  int status_EMCluster, 
			  int status_InDetTracks, 
			  int status_TrigElectron,
			  int status_Egamma);
  int addCombLinks(const CombLinks& links);

  const std::vector<MuonRoILinks>& getMuonRoILinks() const;
  const std::vector<ElectronRoILinks>& getElectronRoILinks() const;
  std::vector<ElectronRoILinks> getElectronRoILinks(const ChainEntry& chain) const;

  const std::vector<CombLinks>& getCombLinks() const { return CombIndex; }
  std::vector<CombLinks> getCombLinks(int roi_type) const;
  std::vector<CombLinks> getCombLinks(const ChainEntry& chain) const;

  void clear();
  
private:
  std::vector<MuonRoILinks> MuonRoIIndex;
  std::vector<ElectronRoILinks> ElectronRoIIndex;
  std::vector<TauRoILinks> TauRoIIndex;
  std::vector<JetRoILinks> JetRoIIndex;

  std::vector<CombLinks> CombIndex; // Generic link similar to Trig::Combination
};

//#include "SGTools/CLASS_DEF.h"
//CLASS_DEF( RoILinks , 137525413 , 1 )

#endif // __RoILinks_h__

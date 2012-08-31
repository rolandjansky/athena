/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigVarInElectronChain_h__
#define __TrigVarInElectronChain_h__
/*
  TrigVarInElectronChain.h
*/

#include "TriggerMenuNtuple/TrigVarInHlt.h"
#include <iostream>
#include <vector>
#include "L1_Cluster.h"
#include "L2_Cluster.h"
#include "L2_Track.h"

class TrigVarInElectronChain : public TrigVarInHlt {
public:
  TrigVarInElectronChain(std::string chain_name="", bool ActiveOnly=false);
  ~TrigVarInElectronChain();

  void define();


  void fill(std::vector<ChainEntry>& chains,
	    RoILinks& roi,
	    const L1_Cluster& L1RoI,
	    const L2_Cluster& L2Clus,
	    const L2_Track& L2track);

  template<class T>
  void fillL2Track(std::vector<T>& Vars, 
		   std::vector<ChainEntry>& chains,
		   RoILinks& roi,
		   const std::string& feature,
		   std::vector<float>* PtVec,
		   std::vector<T>* var);

  // Method to get Histograms
  TH1* getHistL1RoIEt() {return mHistL1RoIEt;}
  TH1* getHistL1RoIEta() {return mHistL1RoIEta;}
  TH1* getHistL1RoIPhi() {return mHistL1RoIPhi;}
  TH1* getHistL2ClusterEt() {return mHistL2ClusterEt;}
  TH1* getHistL2ClusterEta() {return mHistL2ClusterEta;}
  TH1* getHistL2ClusterPhi() {return mHistL2ClusterPhi;}
  TH1* getHistL2SiTrackPt() {return mHistL2SiTrackPt;}
  TH1* getHistL2SiTrackEta() {return mHistL2SiTrackEta;}
  TH1* getHistL2SiTrackPhi() {return mHistL2SiTrackPhi;}
  TH2* getHistColL1RoIEtL2ClusterEt() {return mHistColL1RoIEtL2ClusterEt;}
  TH2* getHistColL1RoIEtaL2ClusterEta() {return mHistColL1RoIEtaL2ClusterEta;}
  TH2* getHistColL1RoIPhiL2ClusterPhi() {return mHistColL1RoIPhiL2ClusterPhi;}
  TH2* getHistColL1RoIEtL2SiTrackPt() {return mHistColL1RoIEtL2SiTrackPt;}
  TH2* getHistColL1RoIEtaL2SiTrackEta() {return mHistColL1RoIEtaL2SiTrackEta;}
  TH2* getHistColL1RoIPhiL2SiTrackPhi() {return mHistColL1RoIPhiL2SiTrackPhi;}
  TH2* getHistColL2ClusterEtL2SiTrackPt() {return mHistColL2ClusterEtL2SiTrackPt;}
  TH2* getHistColL2ClusterEtaL2SiTrackEta() {return mHistColL2ClusterEtaL2SiTrackEta;}
  TH2* getHistColL2ClusterPhiL2SiTrackPhi() {return mHistColL2ClusterPhiL2SiTrackPhi;}
private:
  // Histograms
  TH1* mHistL1RoIEt;
  TH1* mHistL1RoIEta;
  TH1* mHistL1RoIPhi;
  TH1* mHistL2ClusterEt;
  TH1* mHistL2ClusterEta;
  TH1* mHistL2ClusterPhi;
  TH1* mHistL2SiTrackPt;
  TH1* mHistL2SiTrackEta;
  TH1* mHistL2SiTrackPhi;
  TH2* mHistColL1RoIEtL2ClusterEt;
  TH2* mHistColL1RoIEtaL2ClusterEta;
  TH2* mHistColL1RoIPhiL2ClusterPhi;
  TH2* mHistColL1RoIEtL2SiTrackPt;
  TH2* mHistColL1RoIEtaL2SiTrackEta;
  TH2* mHistColL1RoIPhiL2SiTrackPhi;
  TH2* mHistColL2ClusterEtL2SiTrackPt;
  TH2* mHistColL2ClusterEtaL2SiTrackEta;
  TH2* mHistColL2ClusterPhiL2SiTrackPhi;
  // Vectors to hold variables
  std::vector<float> mL1RoIEt;
  std::vector<float> mL1RoIEta;
  std::vector<float> mL1RoIPhi;
  std::vector<float> mL2ClusterEt;
  std::vector<float> mL2ClusterEta;
  std::vector<float> mL2ClusterPhi;
  std::vector<float> mL2SiTrackPt;
  std::vector<float> mL2SiTrackEta;
  std::vector<float> mL2SiTrackPhi;
  // Maps to hold variables
  std::multimap<float, float> mColL1RoIEtL2ClusterEt;
  std::multimap<float, float> mColL1RoIEtaL2ClusterEta;
  std::multimap<float, float> mColL1RoIPhiL2ClusterPhi;
  std::multimap<float, float> mColL1RoIEtL2SiTrackPt;
  std::multimap<float, float> mColL1RoIEtaL2SiTrackEta;
  std::multimap<float, float> mColL1RoIPhiL2SiTrackPhi;
  std::multimap<float, float> mColL2ClusterEtL2SiTrackPt;
  std::multimap<float, float> mColL2ClusterEtaL2SiTrackEta;
  std::multimap<float, float> mColL2ClusterPhiL2SiTrackPhi;
};



#endif // __TrigVarInElectronChain_h__

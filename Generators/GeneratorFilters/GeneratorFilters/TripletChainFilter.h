/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef GENERATORFILTERS_TRIPLETCHAINFILTER_H
#define GENERATORFILTERS_TRIPLETCHAINFILTER_H

#include "GeneratorModules/GenFilter.h"

// --------------------------------------------------
/** 
    @class GeneratorFilters/TripletChainFilter.h

    Just select the triplet using explicit PDG ID and Parent ID.
    1.Decay with "one step"
    If one particle decay into three chidren, parent limitation is just one generation.
    For example, tau-->3mu signature is simply decay into three body.
    In this case, the same parent is required for each particle.
    2.Decay with "two step"
    If grand parent particle decay into two particle at the first and one of the emitted particle 
    decay into two body, this can be reconstructed as triplet.
    But doublet limitation must be set.
    Then, you can set grand parent id for the second emitted particle.

    And also, you can set the thresholds for pT, eta, mass of each particle, and triplet.

    @author designed by Dai KOBAYASHI(Apr 2016) 
*/




class TripletChainFilter:public GenFilter {
public:
  TripletChainFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TripletChainFilter();
  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();

private:
  // Setable Properties:-

  int    m_NTriplet; // if you set as <0, "at least" mode is activated.
  int    m_PdgId1; // PDG ID
  int    m_PdgId2; // PDG ID
  int    m_PdgId3; // PDG ID
  int    m_NStep1; // non:0, from triplet:1, from emitted particle fomr triplet:2
  int    m_NStep2; // non:0, from triplet:1, from emitted particle fomr triplet:2
  int    m_NStep3; // non:0, from triplet:1, from emitted particle fomr triplet:2
  double m_PtMin1; // @MeV
  double m_PtMin2; // @MeV
  double m_PtMin3; // @MeV
  double m_EtaMax1;
  double m_EtaMax2;
  double m_EtaMax3;
  int    m_TripletPdgId; 
  double m_TripletPtMin; // @MeV
  double m_TripletEtaMax;
  double m_TripletMassMin; // @MeV
  double m_TripletMassMax; // @MeV
  int    m_DoubletPdgId; // if you have NStep=2 particle, you must set this.
  double m_DoubletPtMin; // @MeV
  double m_DoubletEtaMax;
  double m_DoubletMassMin; // @MeV
  double m_DoubletMassMax; // @MeV
  int    m_ThirdParentPdgId; // if you have NStep=3 particle not associated to doublet, you must set this.
  double m_ThirdParentPtMin; // @MeV
  double m_ThirdParentEtaMax;
  int    m_ParentPdgId; // if you need, you can set the limitaion for source of the triplet.
  double m_ParentPtMin; // @MeV
  double m_ParentEtaMax;
  double m_ParentMassMin; // @MeV
  double m_ParentMassMax; // @MeV
  
  // if pdg IDs were same, pt and eta threshold have to be apply without charge limitation.
  bool m_SamePdgId12;
  bool m_SamePdgId13;
  bool m_SamePdgId23;
  bool m_SamePdgId123;

  // Private Methods:=

};

#endif

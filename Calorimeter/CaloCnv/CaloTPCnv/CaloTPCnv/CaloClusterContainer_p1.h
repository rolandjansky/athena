/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOATHENAPOOL_CALOCLUSTERCONTAINER_P1_H
#define CALOATHENAPOOL_CALOCLUSTERCONTAINER_P1_H

#include "CaloTPCnv/CaloTowerSeg_p1.h"
#include "DataModelAthenaPool/ElementLink_p1.h"
#include "CaloTPCnv/CaloSamplingData_p1.h"
#include "CaloTPCnv/CaloClusterMomentStore_p1.h"
#include "EventCommonTPCnv/P4EEtaPhiM_p1.h"
#include <vector>

class CaloClusterContainer_p1  
{
 public:
 
  struct CaloCluster_p {
  public:
    //Very basic variables, members of CaloCluster ifself
    float m_basicSignal;
    float m_time;
    unsigned int m_samplingPattern; 
    bool m_barrel;  
    bool m_endcap;
    float m_eta0;
    float m_phi0;  

    //Inherited through CaloCompositeKineBase
    P4EEtaPhiM_p1 m_P4EEtaPhiM;  //Not yet part FourMomAthenaPool
    unsigned int m_caloRecoStatus;


    //Cached Stores
    CaloSamplingData_p1           m_dataStore;   
    CaloClusterMomentStore_p1     m_momentStore; 
    bool m_ownDataStore;

    //Linked Stores:
    ElementLinkInt_p1 m_dataLink;
    ElementLinkInt_p1 m_cellLink;
  };

  typedef std::vector<CaloCluster_p> contType;
  typedef contType::const_iterator const_iterator;
  typedef contType::iterator iterator;

  contType m_vec;
  CaloTowerSeg_p1 m_towerSeg;

};

#endif


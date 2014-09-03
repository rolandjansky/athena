/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOATHENAPOOL_CALOCLUSTERCONTAINER_P2_H
#define CALOATHENAPOOL_CALOCLUSTERCONTAINER_P2_H

#include "CaloTPCnv/CaloTowerSeg_p1.h"
#include "DataModelAthenaPool/ElementLink_p2.h"
#include "DataModelAthenaPool/ElementLinkContNames_p2.h"
#include "CaloTPCnv/CaloSamplingDataContainer_p1.h"
#include "CaloTPCnv/CaloClusterMomentContainer_p1.h"
#include "EventCommonTPCnv/P4EEtaPhiM_p1.h"
#include <vector>

class CaloClusterContainer;


class CaloClusterContainer_p2  
{
public:

//     static const CaloClusterContainer* container(const std::string& key);  
  
//     const CaloClusterContainer* toTrans(unsigned long long ievt, std::string& key) const;  
 
    struct CaloCluster_p {
    public:
        //Very basic variables, members of CaloCluster ifself
        float m_basicSignal;
        float m_time;
        float m_eta0;
        float m_phi0;  

        unsigned int m_samplingPattern; 
        unsigned int m_caloRecoStatus;
        unsigned int m_clusterSize;

        bool m_barrel; //Make transient, find from samplingPattern
        bool m_endcap; //Make transient, find from samplingPattern

        //Inherited through CaloCompositeKineBase
        P4EEtaPhiM_p1 m_P4EEtaPhiM;  //Not yet part FourMomAthenaPool

        //Linked Stores:
        ElementLinkInt_p2 m_dataLink;
        ElementLinkInt_p2 m_cellLink;
    };

  
    unsigned int nclusters() const {return m_vec.size();}

    typedef std::vector<CaloCluster_p> contType;
    typedef contType::const_iterator const_iterator;
    typedef contType::iterator iterator;

    contType m_vec;
    CaloSamplingDataContainer_p1  m_samplingDataContainer;
    CaloClusterMomentContainer_p1 m_momentContainer;
    CaloTowerSeg_p1               m_towerSeg;

    // Element link container name lookup table
    ElementLinkContNames_p2       m_linkNames;

private:



};


#endif


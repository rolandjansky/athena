/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOATHENAPOOL_CALOCLUSTERCONTAINER_P6_H
#define CALOATHENAPOOL_CALOCLUSTERCONTAINER_P6_H

#include "CaloTPCnv/CaloTowerSeg_p1.h"
#include "DataModelAthenaPool/ElementLink_p2.h"
#include "DataModelAthenaPool/ElementLinkContNames_p2.h"
#include "CaloTPCnv/CaloClusterMomentContainer_p2.h"
#include "EventCommonTPCnv/P4EEtaPhiMFloat_p2.h"
#include <vector>

class CaloClusterContainer;


class CaloClusterContainer_p6 
{
public:
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

        P4EEtaPhiMFloat_p2 m_P4EEtaPhiM; 

        //Linked Stores:
        ElementLinkInt_p2 m_dataLink;
        ElementLinkInt_p2 m_cellLink;
    };
    
    typedef std::vector<CaloCluster_p> contType;
	contType m_vec;
	
	typedef contType::const_iterator const_iterator;
    typedef contType::iterator iterator;

	
	unsigned int m_varTypePattern;  // same for container
    std::vector<float> m_dataStore; // stores variables in all the samplings
	
	CaloClusterMomentContainer_p2 m_momentContainer;
    CaloTowerSeg_p1               m_towerSeg;

    // Element link container name lookup table
    ElementLinkContNames_p2       m_linkNames;
	
    unsigned int nclusters() const {return m_vec.size();}  

    // bad channel information
    std::vector<short> m_badClusIndexList;  // index of cluster 
    std::vector<short> m_badLayerStatusList;   // list of layer+badchannel status flags
    std::vector<unsigned int> m_badEtaList;   // compressed etacell-etaclus list
    std::vector<unsigned int> m_badPhiList;   // compressed phicell-phiclus list

    // raw energy compressed information
    std::vector<unsigned int> m_rawE;
    std::vector<unsigned int> m_rawEtaPhiM;

    // altE (H1 weight) compressed information
    std::vector<unsigned int> m_altE;
    std::vector<unsigned int> m_altEtaPhiM;
	
private:
};
#endif


// Hi Emacs ! this is  -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:      TrigFullCaloClusterMaker
 * PACKAGE:   Trigger/TrigAlgorithms/TrigCaloRec
 *
 * AUTHOR:    P.Urquijo
 * CREATED:   September 2008
 *
 *          Builds Clusters from CaloCellContainer using offline calo tools.
 *          Produces one outputTE per CaloCluster
 *********************************************************************/
#ifndef TRIGCALOREC_TRIGFULLCALOCLUSTERMAKER_H
#define TRIGCALOREC_TRIGFULLCALOCLUSTERMAKER_H

#include "TrigInterfaces/AllTEAlgo.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class IAlgToolEFCalo;
class CaloClusterCollectionProcessor;
class CaloClusterProcessor;
class CaloClusterContainer;

class TrigFullCaloClusterMaker : public HLT::AllTEAlgo {
  
 public:
  
  TrigFullCaloClusterMaker(const std::string& name, ISvcLocator* pSvcLocator);
  
  ~TrigFullCaloClusterMaker();
  
  //  HLT::ErrorCode hltExecute( const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE );
  
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
			    unsigned int type_out);
 
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();

  HLT::ErrorCode hltEndEvent() { if ( !HLT::AllTEAlgo::reset() ) return false; m_useCachedResult = false; m_roiEtaLimit=4.8; m_cachedTEList.clear(); return HLT::OK; }
  //virtual bool reset() { if ( !HLT::AllTEAlgo::reset() ) return false; m_useCachedResult = false; m_roiEtaLimit=4.8; m_cachedTEList.clear(); return true; }
    
private:
  
  std::string retrieveCellContName ( HLT::TriggerElement* outputTE );
  std::string retrieveTowerContName( HLT::TriggerElement* outputTE );
  
  std::vector<std::string> m_clusterMakerNames;      // Cluster maker tools name
  std::vector<std::string> m_clusterCorrectionNames; // Cluster correction tools name
  
  
  // Following used for testing only :
  //bool        m_useMeaningfullNames;      
  std::string m_clustersOutputName;
  
  // Timers
  std::vector<TrigTimer*> m_timer;
  
  //Other members  
  std::vector<CaloClusterCollectionProcessor*> m_clusterMakerPointers;      //pointers to tools
  std::vector<CaloClusterProcessor*>           m_clusterCorrectionPointers; //pointers to tools
  
  xAOD::CaloClusterContainer* m_pCaloClusterContainer, *m_AllTECaloClusterContainer;
  
  double m_ClusterContainerSize;
 
  std::vector<float> m_L2Eta;
  std::vector<float> m_L2Phi;
  std::vector<float> m_Eta;
  std::vector<float> m_Phi;
  std::vector<float> m_Et;
 
  //std::vector<HLT::TriggerElement*> m_cachedTE;
  
  /** Internal caching variables */
  std::multimap<unsigned int,HLT::TriggerElement*> m_cachedTEList;

  bool m_useCachedResult;
  
  double m_roiEtaLimit;
  
public:
  
  inline xAOD::CaloClusterContainer* GetClusterContainer() const {return m_pCaloClusterContainer;}
  
};
#endif

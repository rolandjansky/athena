// Hi Emacs ! this is  -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:      TrigLArNoisyROAlg
 * PACKAGE:   Trigger/TrigAlgorithms/TrigCaloRec
 *
 * AUTHOR:    D.O. Damazio
 * CREATED:   January 2012
 *
 *          Loops over all cells and tries to conclude whether
 *          this is or not a noisy burst event
 *********************************************************************/
#ifndef TRIGCALOREC_TRIGLARNOISYROALG_H
#define TRIGCALOREC_TRIGLARNOISYROALG_H

#include "TrigInterfaces/AllTEAlgo.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"


class IAlgToolEFCalo;
class CaloClusterCollectionProcessor;
class CaloClusterProcessor;
class CaloClusterContainer;
class LArOnlineID;
class HWIdentifier;


class TrigLArNoisyROAlg : public HLT::AllTEAlgo {
  
 public:
  
  TrigLArNoisyROAlg(const std::string& name, ISvcLocator* pSvcLocator);
  
  ~TrigLArNoisyROAlg();
  
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
			    unsigned int type_out);
 
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();

 
  //virtual bool reset() { if ( !HLT::AllTEAlgo::reset() ) return false; m_useCachedResult = false; m_roiEtaLimit=4.8; m_cachedTEList.clear(); return true; }
  HLT::ErrorCode hltEndEvent() { if ( !HLT::AllTEAlgo::reset() ) return false; m_useCachedResult = false; m_roiEtaLimit=4.8; m_cachedTEList.clear(); return HLT::OK; }
  StatusCode geoInit();
    
private:

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  
  std::string retrieveCellContName ( HLT::TriggerElement* outputTE );
  
  std::vector<std::string> m_clusterMakerNames;      // Cluster maker tools name
  std::vector<std::string> m_clusterCorrectionNames; // Cluster correction tools name
  
  
  // Following used for testing only :
  //bool        m_useMeaningfullNames;      
  std::string m_clustersOutputName;
  
  // Timers
  std::vector<TrigTimer*> m_timer;
  
  //Other members  
  /** Internal caching variables */
  std::multimap<unsigned int,HLT::TriggerElement*> m_cachedTEList;

  bool m_useCachedResult;
  
  double m_roiEtaLimit;
  const LArOnlineID* m_onlineID;

  std::map<HWIdentifier, unsigned int> m_FEB_BadChanCount;
  unsigned int m_BadChanPerFEB;
  int m_CellQualityCut;
  //unsigned int m_MinBadFEB;
  xAOD::CaloClusterContainer* m_AllTECaloClusterContainer;
  
};
#endif

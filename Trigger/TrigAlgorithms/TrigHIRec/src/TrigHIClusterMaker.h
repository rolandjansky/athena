/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     TrigHIClusterMaker.h
PACKAGE:  Trigger/TrigAlgorithms/TrigHIRec

AUTHOR:  martin.spousta@cern.ch
         This is an HLT::Fex analogue of Reconstruction/HeavyIonRec/HIJetRec/HIClusterMaker.h  

********************************************************************/

#ifndef TRIGCALOREC_TrigHIClusterMaker_H
#define TRIGCALOREC_TrigHIClusterMaker_H


// General:
#include "TrigInterfaces/FexAlgo.h"

// For towers:
#include "GaudiKernel/ToolHandle.h"
//#include "AthenaBaseComps/AthAlgorithm.h"	// this is here because we need to run subalgorithms
#include "CaloEvent/CaloTowerSeg.h"
#include "CaloUtils/CaloTowerBuilderToolBase.h"

#include <string>
#include <vector>
#include <map>

// For clusters:
#include "xAODCaloEvent/CaloClusterContainer.h"	
#include "xAODCaloEvent/CaloClusterAuxContainer.h"


class StoreGateSvc;
class CaloTower; 
class CaloTowerContainer;
class ICaloTowerBuilderToolBase;
//class TrigCaloQuality;

class TrigHIClusterMaker : public HLT::FexAlgo
{

 public:

  TrigHIClusterMaker(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigHIClusterMaker();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
                            HLT::TriggerElement* outputTE);
  HLT::ErrorCode hltFinalize();

 private:

  // definitions and properties for towers
  // --------------------------------------
  std::string m_output_key; /// \brief Name of output CaloClusterContainer, e.g. HIClusters
  float m_E_min_moment; /// \brief For clusters w/ E less than this, set their eta/phi to tower eta/phi

  //TrigCaloQuality*      pTrigCaloQuality;

  // making output trigger element
  // -----------------------------
  HLT::ErrorCode attachClusterCollection(HLT::TriggerElement* outputTE, const xAOD::CaloClusterContainer* cl_container);
  
};
#endif

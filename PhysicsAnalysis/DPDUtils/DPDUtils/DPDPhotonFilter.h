/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     DPDPhotonFilter.h
PACKAGE:  offline/PhysicsAnalysis/DPDUtils  

AUTHORS:  L. Carminati
CREATED:  October, 2009

PURPOSE:  Simple photon filter : give the possibility to filter using
          trigger and/or offline photon

UPDATES:  

********************************************************************/

#ifndef _PHOTONFILTER_H 
#define _PHOTONFILTER_H 

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/AlgTool.h"

//#include "TrigDecision/TrigDecisionTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/ChainGroup.h"

// Forward declarations
class StoreGateSvc;
class IThinningSvc;
class PhotonContainer;

//Actual class 

class DPDPhotonFilter : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  // Constructor with parameters: 
  DPDPhotonFilter( const std::string& name, ISvcLocator* pSvcLocator );

  // Destructor: 
  virtual ~DPDPhotonFilter(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  
  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  int m_All;
  int m_pass;
 
  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  int m_EventCounter;
  
  //Collection Names
  std::string m_PhotonCollectionName;

  //cuts
  double m_PhotonEtCut;
  double m_PhotonEtaCut;
  long m_PhotonTightness;
  std::string m_PhotonType;

  ServiceHandle<IThinningSvc> m_ThinningSvc;

  std::vector<std::string> m_TriggerSignatures;
  std::string m_TriggerChain;
  ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
  bool m_UseTriggerSelection ;
  const Trig::ChainGroup* m_gTrig;

}; 

#endif 

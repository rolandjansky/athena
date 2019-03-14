/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECVALIDATION_CALIBRATIONNTUPLEMAKERTOOL_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECVALIDATION_CALIBRATIONNTUPLEMAKERTOOL_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODJet/JetContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

#include <vector>

class TTree;
class TH1;

class CalibrationNtupleMakerTool : public AthAlgorithm {
 
public:
  
  /** Standard Athena-Algorithm Constructor */
  CalibrationNtupleMakerTool(const std::string& name, ISvcLocator* pSvcLocator);
  
  /** Default Destructor */
  virtual ~CalibrationNtupleMakerTool();
  
  /** standard Athena-Algorithm method */
  StatusCode          initialize();

  /** standard Athena-Algorithm method */
  StatusCode          execute();
  
  /** standard Athena-Algorithm method */
  StatusCode          finalize();
  
  /* book the TTree branches */
  virtual StatusCode  bookTree();
  
private:
  
  // Get a data container; implementation at end of this header file
  template<class T> const T* getContainer( const std::string & containerName);
  
  int Matched(const xAOD::Jet* truth, const xAOD::JetContainer* jets, std::vector<const xAOD::Jet*>& matched, int& index);
  
  double DRmin(const xAOD::Jet* myjet, const xAOD::JetContainer* jets, double PtMin);
  
  float DetectorEta(const xAOD::Jet* jet);
  
    
  std::vector < std::string >                  m_collectionNames;
  std::map   < std::string, TTree* >           m_trees;
  std::string                                  m_treeFolder;  
  std::string                                  m_treeDescription;
  
  std::string                                  m_truthJetContainerName;
  std::string                                  m_vertexContainerName;
  
  double                                       m_recoIsoDR;
  double                                       m_recoIsoPtCut;
  double                                       m_trueIsoDR;
  double                                       m_trueIsoPtCut;
  
  double                                       m_matchingCut;
  
  TH1*                                         m_h_events;
  
  float                                        m_eventWeight;
  float                                        m_mu;
  float                                        m_npv;
  
  std::vector < int >*                         m_index;
  
  std::vector < double >*                      m_etaCalo;
  std::vector < double >*                      m_etaDetCalo;
  std::vector < double >*                      m_phiCalo;
  std::vector < double >*                      m_eCalo;
  std::vector < double >*                      m_mCalo;
              
  std::vector < double >*                      m_etaCorr;
  std::vector < double >*                      m_etaDetCorr;
  std::vector < double >*                      m_phiCorr;
  std::vector < double >*                      m_eCorr;
  std::vector < double >*                      m_mCorr;
                                                           
  std::vector < double >*                      m_etaTrue;
  std::vector < double >*                      m_phiTrue;
  std::vector < double >*                      m_eTrue;
  std::vector < double >*                      m_mTrue;
  
  // EventInfo
  SG::ReadHandleKey<xAOD::EventInfo>    m_evt  {this, "EvtInfo", "EventInfo", "EventInfo name"};
  
};

template<class T>
inline const T* CalibrationNtupleMakerTool::getContainer(const std::string & containerName){
  const T * ptr = evtStore()->retrieve< const T >( containerName );
  if (!ptr) {
    ATH_MSG_WARNING("Container '"<<containerName<<"' could not be retrieved");
  }
  return ptr;
}


#endif // TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECVALIDATION_CALIBRATIONNTUPLEMAKERTOOL_H

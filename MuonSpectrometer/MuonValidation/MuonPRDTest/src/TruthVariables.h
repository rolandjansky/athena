/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHVARIABLES_H
#define TRUTHVARIABLES_H

#include "ValAlgVariables.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class MuonIdHelper;

class TruthVariables : public ValAlgVariables
{
 public:
  TruthVariables(StoreGateSvc* evtStore,
                     const MuonGM::MuonDetectorManager* detManager,
                     TTree* tree,
                     std::string containername) :
    ValAlgVariables(evtStore, detManager, tree, containername, "TruthVariables"),
    m_Truth_nVertices(0),
    m_Truth_vertexX(0),
    m_Truth_vertexY(0),
    m_Truth_vertexZ(0),
    m_Truth_vertexT(0),
    m_Truth_vertexId(0),
    m_Truth_nParticles(0), 
    m_Truth_particlePt(0), 
    m_Truth_particleEta(0), 
    m_Truth_particlePhi(0), 
    m_Truth_particleE(0), 
    m_Truth_particleM(0), 
    m_Truth_particlePdg_id(0), 
    m_Truth_particleStatus(0), 
    m_Truth_particleBarcode(0),
    m_Truth_particleProduction_vertex_id(0), 
    m_Truth_particleEnd_vertex_id(0)
  {
  }

  ~TruthVariables()
  {
    if( (this->deleteVariables()).isFailure() ){
      ATH_MSG_ERROR("Failing to delete TruthVariables.");
      std::terminate();
    }
  }

  StatusCode initializeVariables();
  StatusCode fillVariables();

 private:

  void setHelper(const MuonIdHelper*){
    ATH_MSG_ERROR("No Helper for TruthVariables.");
  }

  StatusCode deleteVariables();
  StatusCode clearVariables();

  int m_Truth_nVertices;
  std::vector<double> *m_Truth_vertexX;
  std::vector<double> *m_Truth_vertexY;
  std::vector<double> *m_Truth_vertexZ;
  std::vector<double> *m_Truth_vertexT;
  std::vector<int> *m_Truth_vertexId;
  
  int m_Truth_nParticles;  
  std::vector<double> *m_Truth_particlePt;
  std::vector<double> *m_Truth_particleEta;
  std::vector<double> *m_Truth_particlePhi;
  std::vector<double> *m_Truth_particleE;
  std::vector<double> *m_Truth_particleM;
  std::vector<int> *m_Truth_particlePdg_id;
  std::vector<int> *m_Truth_particleStatus;
  std::vector<int> *m_Truth_particleBarcode;
  std::vector<int> *m_Truth_particleProduction_vertex_id;
  std::vector<int> *m_Truth_particleEnd_vertex_id; 

};

#endif // TRUTHVARIABLES_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUENTRYVARIABLES_H
#define MUENTRYVARIABLES_H

#include "ValAlgVariables.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <vector>

class MuonIdHelper;

class MuEntryVariables : public ValAlgVariables
{
 public:
  MuEntryVariables(StoreGateSvc* evtStore,
                     const MuonGM::MuonDetectorManager* detManager,
                     TTree* tree,
                     std::string containername) :
    ValAlgVariables(evtStore, detManager, tree, containername, "MuEntryVariables"),
    m_MuEntry_nParticles(0), 
    m_MuEntry_particlePt(0), 
    m_MuEntry_particleEta(0), 
    m_MuEntry_particlePhi(0), 
    m_MuEntry_particlePdg_id(0), 
    m_MuEntry_particleBarcode(0),
    m_MuEntry_positionEta(0), 
    m_MuEntry_positionPhi(0), 
    m_MuEntry_positionX(0),
    m_MuEntry_positionY(0),
    m_MuEntry_positionZ(0)
  {
  }

  ~MuEntryVariables()
  {
    if( (this->deleteVariables()).isFailure() ){
      ATH_MSG_ERROR("Failing to delete MuEntryVariables.");
      std::terminate();
    }
  }

  StatusCode initializeVariables();
  StatusCode fillVariables();

 private:

  void setHelper(const MuonIdHelper*){
    ATH_MSG_ERROR("No Helper for MuEntryVariables.");
  }

  StatusCode deleteVariables();
  StatusCode clearVariables();

  int m_MuEntry_nParticles;  
  std::vector<double> *m_MuEntry_particlePt;
  std::vector<double> *m_MuEntry_particleEta;
  std::vector<double> *m_MuEntry_particlePhi;
  std::vector<int> *m_MuEntry_particlePdg_id;
  std::vector<int> *m_MuEntry_particleBarcode;
  std::vector<double> *m_MuEntry_positionEta;
  std::vector<double> *m_MuEntry_positionPhi;
  std::vector<double> *m_MuEntry_positionX;
  std::vector<double> *m_MuEntry_positionY;
  std::vector<double> *m_MuEntry_positionZ;


};

#endif // MUENTRYVARIABLES_H

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTSDOVARIABLES_H
#define MDTSDOVARIABLES_H

#include "ValAlgVariables.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include <vector>

class MdtSDOVariables : public ValAlgVariables
{
 public:
  MdtSDOVariables(StoreGateSvc* evtStore, const MuonGM::MuonDetectorManager* detManager, const MuonIdHelper* idhelper,
                  TTree* tree, const std::string & containername, MSG::Level msglvl) :
    ValAlgVariables(evtStore, detManager, tree, containername, msglvl),
    m_mdtIdHelper(nullptr),
    m_mdt_nsdo(0), 
    m_mdt_sdo_stationName(),
    m_mdt_sdo_stationEta(),
    m_mdt_sdo_stationPhi(),
    m_mdt_sdo_multilayer(),
    m_mdt_sdo_tubeLayer(),
    m_mdt_sdo_tube(),
    m_mdt_sdo_channel(),
    m_mdt_sdo_word(),
    m_mdt_sdo_barcode(),
    m_mdt_sdo_globalPosX(),
    m_mdt_sdo_globalPosY(),
    m_mdt_sdo_globalPosZ(),
    m_mdt_sdo_globaltime(),
    m_mdt_sdo_localPosX(),
    m_mdt_sdo_localPosY()
  {
    setHelper(idhelper);
  }

  ~MdtSDOVariables()=default;

  StatusCode initializeVariables();
  StatusCode fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr);

 private:
  const MdtIdHelper* m_mdtIdHelper;

  void setHelper(const MuonIdHelper* idhelper){
    m_mdtIdHelper = dynamic_cast<const MdtIdHelper*>(idhelper);
    if(!m_mdtIdHelper) {
       ATH_MSG_ERROR("casting IdHelper to MdtIdHelper failed");
       throw std::runtime_error("Bad cast in MdtSDOVariables::setHelper");
    }
  }

  StatusCode clearVariables();
  void deleteVariables(){};

  int m_mdt_nsdo{};
  std::vector<std::string> m_mdt_sdo_stationName;
  std::vector<int> m_mdt_sdo_stationEta;
  std::vector<int> m_mdt_sdo_stationPhi;
  std::vector<int> m_mdt_sdo_multilayer;
  std::vector<int> m_mdt_sdo_tubeLayer;
  std::vector<int> m_mdt_sdo_tube;
  std::vector<int> m_mdt_sdo_channel;

  std::vector<int>    m_mdt_sdo_word;
  std::vector<int>    m_mdt_sdo_barcode;
  std::vector<double> m_mdt_sdo_globalPosX;
  std::vector<double> m_mdt_sdo_globalPosY;
  std::vector<double> m_mdt_sdo_globalPosZ;
  std::vector<double> m_mdt_sdo_globaltime;

  std::vector<double> m_mdt_sdo_localPosX;
  std::vector<double> m_mdt_sdo_localPosY;

};

#endif // MDTSDOVARIABLES_H

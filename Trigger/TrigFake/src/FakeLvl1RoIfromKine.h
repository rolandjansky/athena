/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FAKELVL1ROIFROMKINE_H
#define FAKELVL1ROIFROMKINE_H

#include <string>
#include <vector>
#include "TrigSteering/LvlConverter.h"

#define DEFAULT_RoIBRDOLocation "/RoIB/RoIBRDOLocation"
#define DEFAULT_CaloConfigLocation "/Run/CaloTrigConfig"

class StoreGateSvc;
class FakeRoI;
class HepLorentzVector;
class Hep3Vector;

class FakeLvl1RoIfromKine : public HLT::LvlConverter {
 public:
  FakeLvl1RoIfromKine(const std::string& name, const std::string& type,
		      const IInterface* parent); //!< constructor
  ~FakeLvl1RoIfromKine();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(std::vector<HLT::SteeringChain*>& activeSteeringChains);
  HLT::ErrorCode hltFinalize();

 private:

  std::vector<FakeRoI> * createRoIfromMC();
  StatusCode createRoI(FakeRoI &roi);

 private:

  unsigned int m_roiId;

  std::string m_mcEventCollectionName;
  double m_emTauRoiPtMin;
  double m_emTauRoiEtaMax;
  double m_muonRoiPtMin;
  double m_muonRoiEtaMax;
  double m_jetRoiPtMin;
  double m_jetRoiEtaMax;
  std::vector<int> m_emTauRoiParticles;
  std::vector<int> m_muonRoiParticles;
  std::vector<int> m_jetRoiParticles;

  std::string m_emRoiLabel;
  std::string m_muonRoiLabel;
  std::string m_jetRoiLabel;
  std::string m_tauRoiLabel;

};

#endif

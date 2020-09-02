/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FAKELVL1MULTIPLEROISATFIXEDETAPHI_H
#define FAKELVL1MULTIPLEROISATFIXEDETAPHI_H

#include <string>
#include <vector>
#include "TrigSteering/LvlConverter.h"

#define DEFAULT_RoIBRDOLocation "/RoIB/RoIBRDOLocation"
#define DEFAULT_CaloConfigLocation "/Run/CaloTrigConfig"

class StoreGateSvc;
class FakeRoI;
class HepLorentzVector;
class Hep3Vector;


class FakeLvl1MultipleRoIsatFixedEtaPhi : public HLT::LvlConverter
{
 public:

  FakeLvl1MultipleRoIsatFixedEtaPhi(const std::string& name, const std::string& type,
			   const IInterface* parent); //!< constructor
  ~FakeLvl1MultipleRoIsatFixedEtaPhi();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(std::vector<HLT::SteeringChain*>& activeSteeringChains);
  HLT::ErrorCode hltFinalize();

 private:

  unsigned int m_roiId;
  std::vector<double> m_muonRoiPhi;
  std::vector<double> m_muonRoiEta;
  std::vector<std::string> m_muonRoiLabel;

  std::vector<double> m_tauRoiPhi;
  std::vector<double> m_tauRoiEta;
  std::vector<std::string> m_tauRoiLabel;

  std::vector<double> m_emRoiPhi;
  std::vector<double> m_emRoiEta;
  std::vector<std::string> m_emRoiLabel;

  std::vector<double> m_jetRoiPhi;
  std::vector<double> m_jetRoiEta;
  std::vector<std::string> m_jetRoiLabel;

  unsigned int m_lvl1ID; //!< Level 1 ID, in case Level 1 result is read from file


};


#endif

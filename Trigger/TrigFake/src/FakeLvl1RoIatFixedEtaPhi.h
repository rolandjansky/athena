/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FAKELVL1ROIATFIXEDETAPHI_H
#define FAKELVL1ROIATFIXEDETAPHI_H

#include <string>
#include <vector>
#include "TrigSteering/LvlConverter.h"

#define DEFAULT_RoIBRDOLocation "/RoIB/RoIBRDOLocation"
#define DEFAULT_CaloConfigLocation "/Run/CaloTrigConfig"

class StoreGateSvc;
class FakeRoI;
class HepLorentzVector;
class Hep3Vector;


class FakeLvl1RoIatFixedEtaPhi : public HLT::LvlConverter
{
 public:

  FakeLvl1RoIatFixedEtaPhi(const std::string& name, const std::string& type,
			   const IInterface* parent); //!< constructor
  ~FakeLvl1RoIatFixedEtaPhi();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(std::vector<HLT::SteeringChain*>& activeSteeringChains);
  HLT::ErrorCode hltFinalize();

 private:

  unsigned int m_roiId;

  double m_tauRoiPhi;
  double m_tauRoiEta;
  double m_emRoiPhi;
  double m_emRoiEta;
  double m_muonRoiPhi;
  double m_muonRoiEta;
  double m_jetRoiPhi;
  double m_jetRoiEta;

  std::string m_emRoiLabel;
  std::string m_muonRoiLabel;
  std::string m_jetRoiLabel;
  std::string m_tauRoiLabel;

  unsigned int m_lvl1ID; //!< Level 1 ID, in case Level 1 result is read from file


};


#endif

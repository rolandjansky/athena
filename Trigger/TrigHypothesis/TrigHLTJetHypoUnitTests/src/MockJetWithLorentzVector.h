/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPOUNITTESTS_MOCKJETWITHLORENTZVECTOR_H
#define TRIGHLTJETHYPOUNITTESTS_MOCKJETWITHLORENTZVECTOR_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "gmock/gmock.h"
#include <TLorentzVector.h>

using ::testing::Invoke;

class MockJetWithLorentzVector: public HypoJet::IJet {
public:

 MockJetWithLorentzVector(const TLorentzVector& lv):m_lv{lv} {
    ON_CALL(*this, eta())
      .WillByDefault(Invoke(&m_lv, &TLorentzVector::Eta));
    ON_CALL(*this, et())
      .WillByDefault(Invoke(&m_lv,
			    static_cast<double(TLorentzVector::*)() const> 
			    (&TLorentzVector::Et)));
  }
  
  MOCK_CONST_METHOD0(pt, double());
  MOCK_CONST_METHOD0(eta, double());
  MOCK_CONST_METHOD0(phi, double());
  MOCK_CONST_METHOD0(m, double ());
  MOCK_CONST_METHOD0(e, double());
  MOCK_CONST_METHOD0(et, double());
  MOCK_CONST_METHOD0(rapidity, double());
  TLorentzVector p4() const {return m_lv;}
  std::string toString() const {
    double etav = eta();
    double etv = et();
    std::string s = "et: " + std::to_string(etv) +
      " eta: " + std::to_string(etav);
    return s;
  }

  MOCK_CONST_METHOD0(position, unsigned int());
  MOCK_CONST_METHOD2(getAttribute, bool (const std::string&, float&));

  // Had problems compiling mock method
  // MOCK_CONST_METHOD0(xAODJet, std::optional<const xAOD::Jet*>);
  virtual std::optional<const xAOD::Jet*> xAODJet() const override{
    return std::optional<const xAOD::Jet*> ();
  }
 private:
  TLorentzVector m_lv;

};
#endif

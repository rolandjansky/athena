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
  const TLorentzVector& p4() const {return m_lv;}
  MOCK_CONST_METHOD0(position, unsigned int());
  MOCK_CONST_METHOD2(getAttribute, bool (const std::string&, float&));
  MOCK_CONST_METHOD0(toString, std::string());

 private:
  TLorentzVector m_lv;

};
#endif

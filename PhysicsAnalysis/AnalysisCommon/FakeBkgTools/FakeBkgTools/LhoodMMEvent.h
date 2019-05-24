/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LhoodMMEvent_h
#define LhoodMMEvent_h

#include <vector>

class LhoodMMEvent {

 public :

  LhoodMMEvent(int nlep, const std::vector<double>& realEff, const std::vector<double>& fakeEff, const std::vector<bool>& isTight, const std::vector<int>& charge, double weight, double aux = 0., double aux2 = 0.);
  LhoodMMEvent(int nlep, const std::vector<float>& realEff, const std::vector<float>& fakeEff, const std::vector<bool>& isTight, const std::vector<int>& charge, float weight, float aux = 0., float aux2 = 0.);

  LhoodMMEvent(const LhoodMMEvent& mmevt);
  unsigned nlep() const {return m_nlep;}
  double realEff(unsigned ilep) const;
  double fakeEff(unsigned ilep) const;
  bool isTight(unsigned ilep) const;
  int charge(unsigned ilep) const;

  float weight() const {return m_weight;}

  double aux() const {return m_aux;}

  double aux2() const {return m_aux2;}

 private :
  
  unsigned m_nlep;
  std::vector<double> m_realEff, m_fakeEff;
  std::vector<int> m_charge;
  std::vector<bool> m_isTight;

  float m_weight;
  double m_aux, m_aux2;

  void Init(int nlep, const std::vector<double>& realEff, const std::vector<double>& fakeEff, const std::vector<bool>& isTight, const std::vector<int>& charge, float weight, double aux, double aux2);
};

#endif

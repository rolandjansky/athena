/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/MaximumBipartiteGroupsMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory2.h"
#include "./TLorentzVectorAsIJet.h"
#include "./TLorentzVectorFactory.h"
#include <TLorentzVector.h>
#include <chrono>

using namespace std::chrono;

class Timer{
 public:
  Timer(){SetUp();}
  void SetUp() {
    
    auto factory = TLorentzVectorFactory();
  
    m_eta0 = 0.5;
    m_eta1 = -0.5;
    m_et0 = 100.;
    m_et1 = 100.;


    m_tl0 = factory.make(m_eta0, m_et0);
    m_tl1 = factory.make(m_eta1, m_et1);

    m_eps = 0.00001;
    m_masseps = 0.01;

  }

  void timeit(){
    HypoJetVector jets;
    TLorentzVectorAsIJet  j0 (m_tl0);
    TLorentzVectorAsIJet  j1 (m_tl1);
    HypoJetVector jets0;
    jets0.push_back(&j0);

    HypoJetVector jets1;
    jets1.push_back(&j1);
    HypoJetGroupVector jetGroups{jets0, jets1};

    std::vector<double> etaMins{-1., -1., -1.};
    std::vector<double> etaMaxs{1., 1, 1.};
    std::vector<double> thresholds{100., 120, 140.};
    std::vector<int> asymmetricEtas{0, 0, 0, 0};

    auto conditions = conditionsFactoryEtaEt(etaMins, 
                                             etaMaxs, 
                                             thresholds,
                                             asymmetricEtas);

    MaximumBipartiteGroupsMatcher matcher(conditions);

    auto t0 = high_resolution_clock::now();
    for(int i = 0; i != 1000000; ++i){
      matcher.match(jetGroups.begin(), jetGroups.end());
    }
    auto t1 = high_resolution_clock::now();
    std::cout << std::boolalpha << matcher.pass() << '\n';
    std::cout << duration_cast<milliseconds>(t1-t0).count()
	      <<"ms\n";
  }
 private:
  TLorentzVector m_tl0;
  TLorentzVector m_tl1;

  double m_eta0;
  double m_eta1;
  double m_et0;
  double m_et1;
  double m_eps;
  double m_masseps;
  
};

int main(){
  Timer timer;
  timer.timeit();
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/FoxWolfram.h"
#include "TLorentzVector.h"
#include "JetSubStructureUtils/BoostToCenterOfMass.h"

using namespace std;
using namespace JetSubStructureUtils;

map<string, double> FoxWolfram::result(const fastjet::PseudoJet &jet) const
{
  map<string, double> Variables;
  Variables["FoxWolfram0"] =  -999.*1000.;
  Variables["FoxWolfram1"] =  -999.*1000.;
  Variables["FoxWolfram2"] =  -999.*1000.;
  Variables["FoxWolfram3"] =  -999.*1000.;
  Variables["FoxWolfram4"] =  -999.*1000.;

  double FoxWolframMoments[5] = {0};
  double ESum = 0;

  vector<fastjet::PseudoJet> clusters = boostToCenterOfMass(jet, jet.constituents());
  if(clusters.size() < 2) return Variables;

  for(unsigned int i1=0; i1<clusters.size(); i1++) {
    double p1 = sqrt(clusters.at(i1).px()*clusters.at(i1).px()
                     +      clusters.at(i1).py()*clusters.at(i1).py()
                     +      clusters.at(i1).pz()*clusters.at(i1).pz());

    for(unsigned int i2=i1+1; i2<clusters.size(); i2++) {
      double p2 = sqrt(clusters.at(i2).px()*clusters.at(i2).px()
                       +      clusters.at(i2).py()*clusters.at(i2).py()
                       +      clusters.at(i2).pz()*clusters.at(i2).pz());

      TVector3 cj(clusters[i2].px(), clusters[i2].py(), clusters[i2].pz());
      TLorentzVector quadvec(clusters[i1].px(), clusters[i1].py(), clusters[i1].pz(),  clusters[i1].e());

      double CosTheta12 = TMath::Cos(quadvec.Angle(cj)); 

      double P0 = 1.;
      double P1 = CosTheta12;
      double P2 = 0.5*(3.*CosTheta12*CosTheta12 - 1.);
      double P3 = 0.5*(5.*CosTheta12*CosTheta12*CosTheta12 - 3.*CosTheta12);
      double P4 = 0.125*(35.*CosTheta12*CosTheta12*CosTheta12*CosTheta12 - 30.*CosTheta12*CosTheta12 + 3.);
      
      FoxWolframMoments[0] += p1*p2*P0;
      FoxWolframMoments[1] += p1*p2*P1;
      FoxWolframMoments[2] += p1*p2*P2;
      FoxWolframMoments[3] += p1*p2*P3;
      FoxWolframMoments[4] += p1*p2*P4;
    }

    ESum += clusters[i1].e();
  }

  vector<double> R;

  if(ESum > 0) {
    for(int i=0; i<5; i++) {
      FoxWolframMoments[i] /= ESum*ESum;
      R.push_back(FoxWolframMoments[i]);
    }
  }
  else {
    return Variables;
  }

  Variables["FoxWolfram0"] = R.at(0);
  Variables["FoxWolfram1"] = R.at(1);
  Variables["FoxWolfram2"] = R.at(2);
  Variables["FoxWolfram3"] = R.at(3);
  Variables["FoxWolfram4"] = R.at(4);

  return Variables; 
}

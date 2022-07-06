/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PanTauAlgs/HelperFunctions.h"
#include "PanTauAlgs/TauConstituent.h"
#include "Math/SpecFuncMathMore.h"
#include "TMath.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include <vector>
#include <sstream>
#include <cmath>


PanTau::TauConstituent* PanTau::HelperFunctions::getNeutralConstWithLargestAngle(const TLorentzVector& charged,
										 const std::vector<PanTau::TauConstituent*>& neutral) const {
  if(neutral.empty()) return 0;
  //loop through neutrals to find the one with largest angle
  unsigned int    idx_Neutral = -1;
  double          angle_Neutral  = -1.;
  for(unsigned int iNeut=0; iNeut<neutral.size(); iNeut++) {
    TLorentzVector tlv_CurNeut = neutral[iNeut]->p4();
    double angle = charged.Angle(tlv_CurNeut.Vect());
    if(angle > angle_Neutral) {
      angle_Neutral = angle;
      idx_Neutral = iNeut;
    }
  }//end loop neutrals
    
  return neutral[idx_Neutral];
}


std::string PanTau::HelperFunctions::convertNumberToString(double x) const {
  std::stringstream tmpStream;
  tmpStream << x;
  return tmpStream.str();
}


int PanTau::HelperFunctions::getBinIndex(const std::vector<double>& binEdges, double value) const {
  int resBin = -1;
  for(unsigned int i=0; i<binEdges.size()-1; i++) {
    double lowerEdge = binEdges[i];
    double upperEdge = binEdges[i+1];
    if(lowerEdge <= value && value < upperEdge) resBin = i;
  }
  if(resBin == -1) {
    ATH_MSG_WARNING("Could not find matching bin for value " << value << " in these bin edges:");
    for(unsigned int i=0; i<binEdges.size(); i++) ATH_MSG_WARNING("\tbin edge " << i << ": " << binEdges[i]);
  }
  return resBin;
}


double PanTau::HelperFunctions::stddev(double sumOfSquares, double sumOfValues, int numConsts) const {
  // calculate standard deviations according to:
  // sigma^2 = (sum_i x_i^2) / N - ((sum_i x_i)/N)^2 (biased maximum-likelihood estimate)
  // directly set sigma^2 to 0 in case of N=1, otherwise numerical effects may yield very small negative sigma^2
  if(numConsts == 1) return 0;
  double a = sumOfSquares / ((double)numConsts);
  double b = sumOfValues / ((double)numConsts);
  double stdDev = a - b*b;
  if(stdDev < 0.) stdDev = 0;
  return std::sqrt(stdDev);
}


double PanTau::HelperFunctions::deltaRprime(const TVector3& vec1, const TVector3& vec2) const {
  const double a = vec1.DeltaPhi(vec2);
  const double b = vec1.Theta() - vec2.Theta();
  double dRprime = std::sqrt(a*a + b*b);
  return dRprime;
}


int PanTau::HelperFunctions::iPow(int man, int exp) const {
  int ans = 1;
  for (int k = 0; k < exp; k++) {
    ans = ans * man;
  }
  return ans;
}


double PanTau::HelperFunctions::ulAngle(double x, double y) const {
  Double_t ulangl = 0;
  Double_t r = std::sqrt(x * x + y * y);
  if (r < 1.0E-20) {
    return ulangl;
  }
  if (TMath::Abs(x) / r < 0.8) {
    ulangl = sign(TMath::ACos(x / r), y);
  } else {
    ulangl = TMath::ASin(y / r);
    if (x < 0. && ulangl >= 0.) {
      ulangl = TMath::Pi() - ulangl;
    } else if (x < 0.) {
      ulangl = -TMath::Pi() - ulangl;
    }
  }
  return ulangl;
}


double PanTau::HelperFunctions::sign(double a, double b) const {
  if (b < 0) {
    return -std::abs(a);
  } else {
    return std::abs(a);
  }
}

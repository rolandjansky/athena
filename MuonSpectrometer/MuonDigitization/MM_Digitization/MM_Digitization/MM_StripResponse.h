/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_MM_STRIPRESPONSE_H
#define MM_DIGITIZATION_MM_STRIPRESPONSE_H
//
// MM_StripResponse.cxx
//     Simulate strip response
//

#include "MM_Digitization/MM_IonizationCluster.h"
#include "MM_Digitization/MM_Electron.h"

#include <map>
#include <vector>
#include <algorithm>
#include <memory>

class MM_StripResponse {

  float timeResolution;
  float stripPitch;
  int stripID;
  int maxstripID;

  std::vector<MM_Electron*> Electrons;

  // First argument is time bin, second argument is strip ID
  std::map< int, std::map<int,float> > stripCharges;

  // Useful info for clustering later
  std::map<int, int> stripTimeThreshold;
  std::map<int, float> stripTotalCharge;
  std::map<int, float> stripMaxCharge;
  std::map<int, int> stripTimeMaxCharge;

  //using vector for the moment -- hopefully this has better access and is not so expensive on the memory
  std::vector<int> v_strip;
  std::vector < std::vector <float> > v_stripTimeThreshold;
  std::vector < std::vector <float> > v_stripTotalCharge;
  std::vector<float> v_stripMaxCharge;
  std::vector<float> v_stripTimeMaxCharge;

 public:

  MM_StripResponse();
  MM_StripResponse(std::vector<MM_IonizationCluster> IonizationClusters, float _timeResolution, float _stripPitch, int _stripID, int _maxstripID);
  void timeOrderElectrons();
  void calculateTimeSeries(float thetaD, int gasgap);
  //  void calculateTimeSeries();
  void simulateCrossTalk(float crossTalk1, float crossTalk2);
  void calculateSummaries(float chargeThreshold);
  std::map<int, int> getTimeThreshold() const;
  std::map<int, float> getTotalCharge() const;
  std::map<int, float> getMaxCharge() const;
  std::map<int, int> getTimeMaxCharge() const;

  std::vector<int> getStripVec() const;
  std::vector < std::vector < float > > getTimeThresholdVec() const;
  std::vector < std::vector < float > >getTotalChargeVec() const;
  std::vector<float> getMaxChargeVec() const;
  std::vector<float> getTimeMaxChargeVec() const;

  int getNElectrons();

};

#endif

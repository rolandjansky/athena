/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_Digitization/MM_StripResponse.h"

using namespace std;

MM_StripResponse::MM_StripResponse() {}

MM_StripResponse::MM_StripResponse(std::vector<MM_IonizationCluster> IonizationClusters, float timeResolution, float stripPitch, int stripID, int maxstripID) : m_timeResolution(timeResolution), m_stripPitch(stripPitch), m_stripID(stripID), m_maxstripID(maxstripID) {

  for (auto& IonizationCluster : IonizationClusters)       
    for (auto& Electron : IonizationCluster.getElectrons())	
      m_Electrons.push_back(Electron);
  
}

void MM_StripResponse::timeOrderElectrons() {

  std::sort(m_Electrons.begin(), m_Electrons.end(), [](const MM_Electron* a, const MM_Electron* b) -> bool { return a->getTime() < b->getTime(); });
  
}

void MM_StripResponse::calculateTimeSeries(float /*thetaD*/, int /*gasgap*/) {
  
  for (auto& Electron : m_Electrons) {
    int timeBin = (int) (Electron->getTime()/m_timeResolution);
    // m_stripID defines the initial strip where the muon entered the gas gap

    int stripVal = 0;
    if(fabs(Electron->getX())>m_stripPitch/2){
      if(Electron->getX()>0.0)
	stripVal = m_stripID + int( (Electron->getX()-m_stripPitch/2)/m_stripPitch ) + 1 ;
      else
	stripVal = m_stripID + int( (Electron->getX()+m_stripPitch/2)/m_stripPitch ) - 1 ;
    }
    else stripVal = m_stripID;
    //    int stripVal = Electron->getX() < 0 ? m_stripID + Electron->getX()/m_stripPitch : m_stripID + Electron->getX()/m_stripPitch;
    //    int stripVal = Electron->getX() < 0 ? m_stripID + Electron->getX()/m_stripPitch - 1 : m_stripID + Electron->getX()/m_stripPitch;

    if (stripVal < 0 || stripVal > m_maxstripID) stripVal = -1;
    (m_stripCharges[timeBin])[stripVal] += Electron->getCharge();
          
  }
}

  
void MM_StripResponse::simulateCrossTalk(float crossTalk1, float crossTalk2) {

  if (crossTalk1 > 0. || crossTalk2 > 0.) 
    for (map<int, map<int, float> >::iterator stripTimeSeries = m_stripCharges.begin(); stripTimeSeries != m_stripCharges.end(); stripTimeSeries++) {
      int timeBin = stripTimeSeries->first;
      for (map<int, float>::iterator stripCharge = stripTimeSeries->second.begin(); stripCharge != stripTimeSeries->second.end(); stripCharge++) {
	
	// Uncomment this if you think that crosstalk causes a smaller response on the central strip
	//	stripCharge.second *= (1-2*crossTalk1)*(1-2*crossTalk2);
	int stripVal = stripCharge->first;
	float stripCharge2 = stripCharge->second;

	if (stripVal-1 > -1) (m_stripCharges[timeBin])[stripVal-1] += stripCharge2 * crossTalk1;
	if (stripVal+1 > -1) (m_stripCharges[timeBin])[stripVal+1] += stripCharge2 * crossTalk1;
      
	if (stripVal-2 > -1) (m_stripCharges[timeBin])[stripVal-2] += stripCharge2 * crossTalk2;
	if (stripVal+2 > -1) (m_stripCharges[timeBin])[stripVal+2] += stripCharge2 * crossTalk2;
	
      }
    }
  
}	
     
void MM_StripResponse::calculateSummaries(float chargeThreshold) {

  for (auto& Electron : m_Electrons) {
    //    int stripVal      = Electron->getX() < 0 ? m_stripID + Electron->getX()/m_stripPitch - 1 : m_stripID + Electron->getX()/m_stripPitch;
    int stripVal = 0;
    if(fabs(Electron->getX())>m_stripPitch/2){
      if(Electron->getX()>0.0)
	stripVal = m_stripID + int( (Electron->getX()-m_stripPitch/2)/m_stripPitch ) + 1 ;
      else
	stripVal = m_stripID + int( (Electron->getX()+m_stripPitch/2)/m_stripPitch ) - 1 ;
    }
    else stripVal = m_stripID;

    float stripCharge = Electron->getCharge();
    float stripTime   = Electron->getTime();
    if(stripCharge < chargeThreshold) continue;

    bool found=false;
    for(size_t ii = 0; ii<m_v_strip.size(); ii++ ){
      if(m_v_strip[ii]==stripVal){
	m_v_stripTimeThreshold[ii].push_back(stripTime) ;
	m_v_stripTotalCharge[ii].push_back(stripCharge);
	found=true;
	break;
      }
    }
    if(!found){ // 	// strip not in vector, add new entry
      m_v_strip.push_back(stripVal);
      vector<float> qTemp;
      qTemp.push_back(stripCharge);
      m_v_stripTotalCharge.push_back(qTemp);
      vector<float> tTemp;
      tTemp.push_back(stripTime);
      m_v_stripTimeThreshold.push_back(tTemp);   
    }
  }

}

  
// accessors
std::map<int, int> MM_StripResponse::getTimeThreshold() const { return m_stripTimeThreshold; }
std::map<int, float> MM_StripResponse::getTotalCharge() const { return m_stripTotalCharge; }
std::map<int, float> MM_StripResponse::getMaxCharge() const { return m_stripMaxCharge; }
std::map<int, int> MM_StripResponse::getTimeMaxCharge() const { return m_stripTimeMaxCharge; }

std::vector<int> MM_StripResponse::getStripVec() const { return m_v_strip; }
std::vector< std::vector < float > > MM_StripResponse::getTimeThresholdVec() const { return m_v_stripTimeThreshold; }
std::vector< std::vector < float > >  MM_StripResponse::getTotalChargeVec() const { return m_v_stripTotalCharge; }
std::vector<float> MM_StripResponse::getMaxChargeVec() const { return m_v_stripMaxCharge; }
std::vector<float> MM_StripResponse::getTimeMaxChargeVec() const { return m_v_stripTimeMaxCharge; }

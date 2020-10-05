/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_Digitization/MM_StripResponse.h"

using namespace std;

MM_StripResponse::MM_StripResponse() {}

MM_StripResponse::MM_StripResponse(std::vector<MM_IonizationCluster> IonizationClusters, float timeResolution, float stripPitch, int stripID, int minstripID, int maxstripID) : m_timeResolution(timeResolution), m_stripPitch(stripPitch), m_stripID(stripID), m_minstripID(minstripID), m_maxstripID(maxstripID) {

	for (auto& IonizationCluster : IonizationClusters)
		for (auto& Electron : IonizationCluster.getElectrons())
			m_Electrons.push_back(Electron);

}

int MM_StripResponse::getNElectrons(){
	return m_Electrons.size();
}

float MM_StripResponse::getTotalCharge(){
	float qtot = 0;
	for(const MM_Electron* electron : m_Electrons) {
		qtot += electron->getCharge();
	}
	return qtot;
}

std::vector<MM_Electron*> MM_StripResponse::getElectrons(){
	return m_Electrons;
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

		// Only add the strips that are either read out, or can cross talk to the read out strips
    if (stripVal < m_minstripID-2 || stripVal > m_maxstripID+1) stripVal = -1;
    if (stripVal > 0)
      (m_stripCharges[timeBin])[stripVal] += Electron->getCharge();

	}
}


void MM_StripResponse::simulateCrossTalk(float crossTalk1, float crossTalk2) {

	// Unfortunately get stuck in the loop if you edit the map in the loop
	//     So make a copy!

	std::map< int, std::map<int,float> > stripChargesCopy1;
	stripChargesCopy1.insert(m_stripCharges.begin(), m_stripCharges.end());

	if (crossTalk1 > 0.){
		for (auto & stripTimeSeries : stripChargesCopy1){
			int timeBin = stripTimeSeries.first;
			for (auto & stripCharge : stripTimeSeries.second ){

				int stripVal = stripCharge.first;
				float stripChargeVal = stripCharge.second;

				if (stripChargeVal==0.) continue;

        // Allow crosstalk between strips that exist.
        // Will check for read out strips in calculateSummaries function
				if (stripVal-1 > 0) (m_stripCharges[timeBin])[stripVal-1] += stripChargeVal * crossTalk1;
				if (stripVal+1 < m_maxstripID) (m_stripCharges[timeBin])[stripVal+1] += stripChargeVal * crossTalk1;
				(m_stripCharges[timeBin])[stripVal] -= stripChargeVal * crossTalk1 * ( (stripVal-1 > 0) + (stripVal+1 < m_maxstripID) );

				if (crossTalk2 > 0.){
					if (stripVal-2 > 0) (m_stripCharges[timeBin])[stripVal-2] += stripChargeVal * crossTalk2;
					if (stripVal+2 < m_maxstripID) (m_stripCharges[timeBin])[stripVal+2] += stripChargeVal * crossTalk2;
					(m_stripCharges[timeBin])[stripVal] -= stripChargeVal * crossTalk2 * ( (stripVal-2 > 0) + (stripVal+2 < m_maxstripID) );
				}
			}
		}
	}
}

void MM_StripResponse::calculateSummaries(float chargeThreshold) {

	std::map< int, std::map<int,float> > stripChargesCopy2;
	stripChargesCopy2.insert(m_stripCharges.begin(), m_stripCharges.end());
	for (auto & stripTimeSeries : stripChargesCopy2){
		int timeBin = stripTimeSeries.first;
		for (auto & stripCharge : stripTimeSeries.second ){
			int stripVal = stripCharge.first;
      // remove dead (missing) strips
      // First active strip starts at m_minstripID
      // Last active strip numbrer is maxStripID-1
      if (stripVal < m_minstripID || stripVal > m_maxstripID-1) continue;
      // remove PCB gap strips
      if (stripVal == 1024 || stripVal == 1025 || stripVal == 2048 || stripVal == 2049 || stripVal == 3072 || stripVal == 3073 || stripVal == 4096 || stripVal == 4097) continue;
      float stripChargeVal = stripCharge.second;
      if(stripChargeVal < chargeThreshold) continue;
      bool found = false;

      for(size_t ii = 0; ii<m_v_strip.size(); ii++ ){
      	if(m_v_strip[ii]==stripVal){
      		m_v_stripTimeThreshold[ii].push_back(timeBin*m_timeResolution);
      		m_v_stripTotalCharge[ii].push_back(stripChargeVal);
      		found = true;
      		break;
      	}
      }

      if(!found){ // 	// strip not in vector, add new entry
      	m_v_strip.push_back(stripVal);
      	vector<float> qTemp;
      	qTemp.push_back(stripChargeVal);
      	m_v_stripTotalCharge.push_back(qTemp);
      	vector<float> tTemp;
      	tTemp.push_back(timeBin*m_timeResolution);
      	m_v_stripTimeThreshold.push_back(tTemp);
      }
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

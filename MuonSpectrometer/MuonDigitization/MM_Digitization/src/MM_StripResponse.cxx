/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_Digitization/MM_StripResponse.h"

using namespace std;

MM_StripResponse::MM_StripResponse() {}

MM_StripResponse::MM_StripResponse(std::vector<MM_IonizationCluster> IonizationClusters, float _timeResolution, float _stripPitch, int _stripID, int _maxstripID) : timeResolution(_timeResolution), stripPitch(_stripPitch), stripID(_stripID), maxstripID(_maxstripID) {

	for (auto& IonizationCluster : IonizationClusters)
		for (auto& Electron : IonizationCluster.getElectrons())
			Electrons.push_back(Electron);

}

int MM_StripResponse::getNElectrons(){
	return Electrons.size();
}

void MM_StripResponse::timeOrderElectrons() {

	std::sort(Electrons.begin(), Electrons.end(), [](const MM_Electron* a, const MM_Electron* b) -> bool { return a->getTime() < b->getTime(); });

}

void MM_StripResponse::calculateTimeSeries(float /*thetaD*/, int /*gasgap*/) {

	for (auto& Electron : Electrons) {
		int timeBin = (int) (Electron->getTime()/timeResolution);
// stripID defines the initial strip where the muon entered the gas gap

		int stripVal = 0;
		if(fabs(Electron->getX())>stripPitch/2){
			if(Electron->getX()>0.0)
				stripVal = stripID + int( (Electron->getX()-stripPitch/2)/stripPitch ) + 1 ;
			else
				stripVal = stripID + int( (Electron->getX()+stripPitch/2)/stripPitch ) - 1 ;
		}
		else stripVal = stripID;
//    int stripVal = Electron->getX() < 0 ? stripID + Electron->getX()/stripPitch : stripID + Electron->getX()/stripPitch;
//    int stripVal = Electron->getX() < 0 ? stripID + Electron->getX()/stripPitch - 1 : stripID + Electron->getX()/stripPitch;

		if (stripVal < 0 || stripVal > maxstripID) stripVal = -1;
		(stripCharges[timeBin])[stripVal] += Electron->getCharge();

	}
}


void MM_StripResponse::simulateCrossTalk(float crossTalk1, float crossTalk2) {

	// Unfortunately get stuck in the loop if you edit the map in the loop
	//     So make a copy!

	std::map< int, std::map<int,float> > stripChargesCopy;
	stripChargesCopy.insert(stripCharges.begin(), stripCharges.end());

	if (crossTalk1 > 0.){
		for (auto & stripTimeSeries : stripChargesCopy){
			int timeBin = stripTimeSeries.first;
			for (auto & stripCharge : stripTimeSeries.second ){

				int stripVal = stripCharge.first;
				float _stripCharge = stripCharge.second;

				if (_stripCharge==0.) continue;

				std::cout << "cross talk function: " << stripVal << " " << _stripCharge << std::endl;

				if (stripVal-1 > -1) (stripCharges[timeBin])[stripVal-1] += _stripCharge * crossTalk1;
				if (stripVal+1 > -1) (stripCharges[timeBin])[stripVal+1] += _stripCharge * crossTalk1;

				if (crossTalk2 > 0.){
					if (stripVal-2 > -1) (stripCharges[timeBin])[stripVal-2] += _stripCharge * crossTalk2;
					if (stripVal+2 > -1) (stripCharges[timeBin])[stripVal+2] += _stripCharge * crossTalk2;
				}
			}
		}
	}
}

void MM_StripResponse::calculateSummaries(float chargeThreshold) {

	for (auto& Electron : Electrons) {
//    int stripVal      = Electron->getX() < 0 ? stripID + Electron->getX()/stripPitch - 1 : stripID + Electron->getX()/stripPitch;
		int stripVal = 0;
		if(fabs(Electron->getX())>stripPitch/2){
			if(Electron->getX()>0.0)
				stripVal = stripID + int( (Electron->getX()-stripPitch/2)/stripPitch ) + 1 ;
			else
				stripVal = stripID + int( (Electron->getX()+stripPitch/2)/stripPitch ) - 1 ;
		}
		else stripVal = stripID;

		float stripCharge = Electron->getCharge();
		float stripTime   = Electron->getTime();
		if(stripCharge < chargeThreshold) continue;

		bool found=false;
		for(size_t ii = 0; ii<v_strip.size(); ii++ ){
			if(v_strip[ii]==stripVal){
				v_stripTimeThreshold[ii].push_back(stripTime) ;
				v_stripTotalCharge[ii].push_back(stripCharge);
				found=true;
				break;
			}
		}
if(!found){ // 	// strip not in vector, add new entry
	v_strip.push_back(stripVal);
	vector<float> qTemp;
	qTemp.push_back(stripCharge);
	v_stripTotalCharge.push_back(qTemp);
	vector<float> tTemp;
	tTemp.push_back(stripTime);
	v_stripTimeThreshold.push_back(tTemp);
}
}

}


// accessors
std::map<int, int> MM_StripResponse::getTimeThreshold() const { return stripTimeThreshold; }
std::map<int, float> MM_StripResponse::getTotalCharge() const { return stripTotalCharge; }
std::map<int, float> MM_StripResponse::getMaxCharge() const { return stripMaxCharge; }
std::map<int, int> MM_StripResponse::getTimeMaxCharge() const { return stripTimeMaxCharge; }

std::vector<int> MM_StripResponse::getStripVec() const { return v_strip; }
std::vector< std::vector < float > > MM_StripResponse::getTimeThresholdVec() const { return v_stripTimeThreshold; }
std::vector< std::vector < float > >  MM_StripResponse::getTotalChargeVec() const { return v_stripTotalCharge; }
std::vector<float> MM_StripResponse::getMaxChargeVec() const { return v_stripMaxCharge; }
std::vector<float> MM_StripResponse::getTimeMaxChargeVec() const { return v_stripTimeMaxCharge; }

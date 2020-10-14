/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
*  MM_ElectronicsResponseSimulation.cxx
*  MC for micromegas athena integration
*
**/
#include "MM_Digitization/MM_ElectronicsResponseSimulation.h"

#include <numeric>

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include "TF1.h"

/*******************************************************************************/
MM_ElectronicsResponseSimulation::MM_ElectronicsResponseSimulation():
  m_peakTime(0),
  m_timeWindowLowerOffset(0),
  m_timeWindowUpperOffset(0),
  m_electronicsThreshold(0),
  m_stripDeadTime(0),
  m_artDeadTime(0),
  m_useNeighborLogic(true)
{
}
/*******************************************************************************/
void MM_ElectronicsResponseSimulation::initialize()
{
  m_vmmShaper = std::make_unique<VMM_Shaper>(m_peakTime);
  m_vmmShaper->initialize();
}
/*******************************************************************************/
void MM_ElectronicsResponseSimulation::clearValues()
{
  m_tStripElectronicsAbThr.clear();
  m_qStripElectronics.clear();
  m_nStripElectronics.clear();
}
/*******************************************************************************/
MM_DigitToolOutput MM_ElectronicsResponseSimulation::getPeakResponseFrom(const MM_ElectronicsToolInput & digiInput, const float thresholdScaleFactor)
{
	clearValues();

	vmmPeakResponseFunction(digiInput.NumberOfStripsPos(), digiInput.chipCharge(), digiInput.chipTime(), thresholdScaleFactor);

	/// ToDo: include loop for calculating Trigger study vars
	MM_DigitToolOutput tmp(true, m_nStripElectronics, m_tStripElectronicsAbThr, m_qStripElectronics, 5, 0.3);

	return tmp;
}
/*******************************************************************************/
MM_DigitToolOutput MM_ElectronicsResponseSimulation::getThresholdResponseFrom(const MM_ElectronicsToolInput & digiInput, const float thresholdScaleFactor)
{
  clearValues();
  vmmThresholdResponseFunction(digiInput.NumberOfStripsPos(), digiInput.chipCharge(), digiInput.chipTime(), thresholdScaleFactor);
  MM_DigitToolOutput tmp(true, m_nStripElectronics, m_tStripElectronicsAbThr, m_qStripElectronics, 5, 0.3);
  return tmp;
}
/*******************************************************************************/
void MM_ElectronicsResponseSimulation::vmmPeakResponseFunction(const std::vector <int> & numberofStrip, const std::vector<std::vector <float>> & qStrip, const std::vector<std::vector <float>> & tStrip, const float thresholdScaleFactor){
  float scaledThreshold = m_electronicsThreshold * thresholdScaleFactor;
  for (unsigned int ii = 0; ii < numberofStrip.size(); ii++) {

    // find min and max times for each strip:
    bool thisStripFired = false;
    double leftStripFired = false;
    double rightStripFired = false;

    // find the maximum charge:
    if (m_useNeighborLogic) {  // only check neighbor strips if VMM neighbor logic is enabled
      if ( ii > 0 ) {
       leftStripFired =  m_vmmShaper->hasChargeAboveThreshold(qStrip.at(ii-1), tStrip.at(ii-1), scaledThreshold);
      }

      if ( ii+1 < numberofStrip.size() ) {
       rightStripFired =  m_vmmShaper->hasChargeAboveThreshold(qStrip.at(ii+1), tStrip.at(ii+1), scaledThreshold);
      }
    }

    thisStripFired = m_vmmShaper->hasChargeAboveThreshold(qStrip.at(ii), tStrip.at(ii), scaledThreshold);

    // check if neighbor strip was above threshold
    bool neighborFired = leftStripFired || rightStripFired;

    // Look at strip if it or its neighbor was above threshold  and if neighbor logic of the VMM is enabled:
    if (thisStripFired || (m_useNeighborLogic && neighborFired)) {
      double charge, time;

      // if strip is below threshold but read through NL reduce threshold to low value of 1e to still find the peak
      float tmpScaledThreshold = (thisStripFired ? scaledThreshold : 1);

      m_vmmShaper->vmmPeakResponse(qStrip.at(ii), tStrip.at(ii), tmpScaledThreshold, charge, time);

      m_nStripElectronics.push_back(numberofStrip.at(ii));
      m_tStripElectronicsAbThr.push_back(time);
      m_qStripElectronics.push_back(charge);
    }
  }
}


void MM_ElectronicsResponseSimulation::vmmThresholdResponseFunction(const std::vector <int> & numberofStrip, const std::vector<std::vector <float>> & qStrip, const std::vector<std::vector <float>> & tStrip, const float thresholdScaleFactor){
  float scaledThreshold = m_electronicsThreshold*thresholdScaleFactor;

  for (unsigned int ii = 0; ii < numberofStrip.size(); ii++) {
    double localThresholdt = 0;
    double localThresholdq = 0;

    m_vmmShaper->vmmThresholdResponse(qStrip.at(ii), tStrip.at(ii), scaledThreshold, localThresholdq, localThresholdt);

    if (localThresholdt  > 0) {
        m_nStripElectronics.push_back(numberofStrip.at(ii));
        m_tStripElectronicsAbThr.push_back(localThresholdt);
        m_qStripElectronics.push_back(localThresholdq);
    }
  }
}

MM_ElectronicsToolTriggerOutput MM_ElectronicsResponseSimulation::getTheFastestSignalInVMM(
	const MM_DigitToolOutput & ElectronicThresholdOutput,
	const int chMax,
	const int stationEta){

	const std::vector<int>  & electronicsThresholdStripPos    = ElectronicThresholdOutput.stripPos();
	const std::vector<float> & electronicsThresholdStripTime   = ElectronicThresholdOutput.stripTime();
	const std::vector<float> & electronicsThresholdStripCharge = ElectronicThresholdOutput.stripCharge();
	std::vector<int> trigger_MMFE_VMM_id;
	std::vector<int> trigger_VMM_id;
	getVMMId(electronicsThresholdStripPos, chMax, stationEta, trigger_VMM_id, trigger_MMFE_VMM_id);

	std::vector<int>   electronicsTriggerStripPos;
	std::vector<float> electronicsTriggerStripTime;
	std::vector<float> electronicsTriggerStripCharge;
	std::vector<int>   electronicsTriggerVMMid;
	std::vector<int>   electronicsTriggerMMFEid;
	electronicsTriggerStripPos.clear();
	electronicsTriggerStripTime.clear();
	electronicsTriggerStripCharge.clear();
	electronicsTriggerVMMid.clear();
	electronicsTriggerMMFEid.clear();

	const int BXtime = 250; // (ns)
	int nstep = (int)(m_timeWindowUpperOffset-m_timeWindowLowerOffset)/BXtime;
	if((int)(m_timeWindowUpperOffset-m_timeWindowLowerOffset)%BXtime>0 ) nstep += 1;

	for(int istep = 0; istep<nstep ; istep++){
		float timeWindowLower = (float) m_timeWindowLowerOffset+istep*BXtime;
		float timeWindowUpper = (float) m_timeWindowLowerOffset+(istep+1)*BXtime;

		std::vector<int> tmp_VMM_id; tmp_VMM_id.clear();
		for(size_t i = 0; i<electronicsThresholdStripPos.size(); i++){
			if(electronicsThresholdStripTime[i] >= timeWindowLower &&
				electronicsThresholdStripTime[i] < timeWindowUpper){

				int VMM_id = trigger_VMM_id[i];
				std::vector< int >::iterator VMM_flag = std::find(tmp_VMM_id.begin(), tmp_VMM_id.end(), VMM_id);
				if(VMM_flag != tmp_VMM_id.end()) continue; // Already filled
				tmp_VMM_id.push_back(VMM_id);

				// Get id for the fastest signal in a VMM
				int theFastestSignal = getIdTheFastestSignalInVMM(
					electronicsThresholdStripTime[i],
					VMM_id,
					trigger_VMM_id,
					electronicsThresholdStripTime,
					(float)m_timeWindowLowerOffset+istep*BXtime,
					(float)m_timeWindowLowerOffset+(istep+1)*BXtime);

				electronicsTriggerStripPos.push_back(    electronicsThresholdStripPos[theFastestSignal]    );
				electronicsTriggerStripTime.push_back(   electronicsThresholdStripTime[theFastestSignal]   );
				electronicsTriggerStripCharge.push_back( electronicsThresholdStripCharge[theFastestSignal] );
				electronicsTriggerVMMid.push_back(       trigger_VMM_id[theFastestSignal]                    );
				electronicsTriggerMMFEid.push_back(      trigger_MMFE_VMM_id[theFastestSignal]               );

			}
		}
	}

	MM_ElectronicsToolTriggerOutput ElectronicsTriggerOutput (
		electronicsTriggerStripPos,
		electronicsTriggerStripCharge,
		electronicsTriggerStripTime,
		electronicsTriggerVMMid,
		electronicsTriggerMMFEid );

	return ElectronicsTriggerOutput;
}

int MM_ElectronicsResponseSimulation::getIdTheFastestSignalInVMM(
	float time,
	int VMM_id,
	std::vector<int> trigger_VMM_id,
	const std::vector<float> electronicsThresholdStripTime,
	float timeWindowLower,
	float timeWindowUpper){
	int theFastestSignal = -1;
	float min_time = 9999.0;
	for(size_t ii = 0; ii<trigger_VMM_id.size(); ii++){
		if(electronicsThresholdStripTime[ii]>= timeWindowLower &&
			electronicsThresholdStripTime[ii] < timeWindowUpper){

			if(trigger_VMM_id[ii]==VMM_id){
				if(time<min_time){
					theFastestSignal = ii;
					min_time=time;
				}
			}
		}
	}
    if(theFastestSignal==-1) {
        MsgStream log(Athena::getMessageSvc(),"MM_ElectronicsResponseSimulation");
        log << MSG::WARNING << "There is something wrong in getIdTheFastestSignalInVMM" << endmsg;
    }

	return theFastestSignal;
}

void MM_ElectronicsResponseSimulation::getVMMId(const std::vector< int > & electronicsThresholdStripPos,
	const int chMax,
	const int stationEta,
	std::vector< int > & trigger_VMM_id,
	std::vector< int > & trigger_MMFE_VMM_id){

	trigger_MMFE_VMM_id.clear();
	trigger_VMM_id.clear();
	const int VMMch = 64;
	electronics VmmMapping;
	for (size_t i=0;i<electronicsThresholdStripPos.size(); i++){
		int vmm_id = (int) electronicsThresholdStripPos.at(i)/VMMch;
		int mmfe_vmm_id = VmmMapping.elec(
			electronicsThresholdStripPos.at(i),
			"VMM",
			stationEta,
			chMax);
		trigger_MMFE_VMM_id.push_back(mmfe_vmm_id);
		trigger_VMM_id.push_back(vmm_id);
	}
}

MM_DigitToolOutput MM_ElectronicsResponseSimulation::applyDeadTimeStrip(const MM_DigitToolOutput & electronicsTriggerOutput){

	const std::vector<int>   & electronicsStripPos    = electronicsTriggerOutput.stripPos();
	const std::vector<float> & electronicsStripTime   = electronicsTriggerOutput.stripTime();
	const std::vector<float> & electronicsStripCharge = electronicsTriggerOutput.stripCharge();
	std::vector<int>   electronicsAppliedDeadtimeStripPos;
	std::vector<float> electronicsAppliedDeadtimeStripTime;
	std::vector<float> electronicsAppliedDeadtimeStripCharge;
	electronicsAppliedDeadtimeStripPos.clear();
	electronicsAppliedDeadtimeStripTime.clear();
	electronicsAppliedDeadtimeStripCharge.clear();

	float deadTime = m_stripDeadTime;
	std::vector<int> v_id = electronicsStripPos;

	for(size_t i = 0; i<electronicsStripPos.size(); i++){
		int id = v_id[i];
		float time = electronicsStripTime[i];
		bool DEAD = deadChannel(id, time, electronicsAppliedDeadtimeStripPos, electronicsAppliedDeadtimeStripTime, deadTime);
		if(!DEAD){
			electronicsAppliedDeadtimeStripPos.push_back(electronicsStripPos[i]);
			electronicsAppliedDeadtimeStripTime.push_back(electronicsStripTime[i]);
			electronicsAppliedDeadtimeStripCharge.push_back(electronicsStripCharge[i]);
		}
	}

	MM_DigitToolOutput ElectronicsTriggerOutputAppliedDeadTime(
		true,  // meaningless
		electronicsAppliedDeadtimeStripPos,
		electronicsAppliedDeadtimeStripTime,
		electronicsAppliedDeadtimeStripCharge,
		5,  // meaningless
		0.0  ); // meaningless

	return ElectronicsTriggerOutputAppliedDeadTime;
}


MM_ElectronicsToolTriggerOutput MM_ElectronicsResponseSimulation::applyARTTiming(const MM_ElectronicsToolTriggerOutput & electronicsTriggerOutput, float jitter, float offset){



	const std::vector<int>  & electronicsTriggerStripPos     = electronicsTriggerOutput.NumberOfStripsPos();
	const std::vector<float> & electronicsTriggerStripTime   = electronicsTriggerOutput.chipTime();
	const std::vector<float> & electronicsTriggerStripCharge = electronicsTriggerOutput.chipCharge();
	const std::vector<int> & electronicsTriggerVMMid         = electronicsTriggerOutput.VMMid();
	const std::vector<int> & electronicsTriggerMMFEid        = electronicsTriggerOutput.MMFEVMMid();
	std::vector<int>   electronicsTriggerAppliedTimingStripPos;
	std::vector<float> electronicsTriggerAppliedTimingStripTime;
	std::vector<float> electronicsTriggerAppliedTimingStripCharge;
	std::vector<int>   electronicsTriggerAppliedTimingVMMId;
	std::vector<int>   electronicsTriggerAppliedTimingMMFEId;
	electronicsTriggerAppliedTimingStripPos.clear();
	electronicsTriggerAppliedTimingStripTime.clear();
	electronicsTriggerAppliedTimingStripCharge.clear();
	electronicsTriggerAppliedTimingVMMId.clear();
	electronicsTriggerAppliedTimingMMFEId.clear();

	//bunchTime = bunchTime + artOffset + jitter;

	TF1 gaussianSmearing("timingSmearing","gaus",offset-jitter*5,offset+jitter*5);
	gaussianSmearing.SetParameters(1,offset,jitter);

	for(size_t i = 0; i<electronicsTriggerStripPos.size(); i++){
		// std::default_random_engine generator;
		// std::normal_distribution<double> distribution(offset,jitter);
		// float timingTransformation = distribution(generator);

		electronicsTriggerAppliedTimingStripPos.push_back(electronicsTriggerStripPos[i]);

		if(jitter || offset)    electronicsTriggerAppliedTimingStripTime.push_back(electronicsTriggerStripTime[i] + gaussianSmearing.GetRandom() );
		else electronicsTriggerAppliedTimingStripTime.push_back(electronicsTriggerStripTime[i] );

		electronicsTriggerAppliedTimingStripCharge.push_back(electronicsTriggerStripCharge[i]);
		electronicsTriggerAppliedTimingVMMId.push_back(electronicsTriggerVMMid[i]);
		electronicsTriggerAppliedTimingMMFEId.push_back(electronicsTriggerMMFEid[i]);

	}

	MM_ElectronicsToolTriggerOutput electronicsTriggerOutputAppliedTiming(
		electronicsTriggerAppliedTimingStripPos,
		electronicsTriggerAppliedTimingStripCharge,
		electronicsTriggerAppliedTimingStripTime,
		electronicsTriggerAppliedTimingVMMId,
		electronicsTriggerAppliedTimingMMFEId );

	return electronicsTriggerOutputAppliedTiming;
}


MM_ElectronicsToolTriggerOutput MM_ElectronicsResponseSimulation::applyDeadTimeART(const MM_ElectronicsToolTriggerOutput & electronicsTriggerOutput){

	const std::vector<int>  & electronicsTriggerStripPos     = electronicsTriggerOutput.NumberOfStripsPos();
	const std::vector<float> & electronicsTriggerStripTime   = electronicsTriggerOutput.chipTime();
	const std::vector<float> & electronicsTriggerStripCharge = electronicsTriggerOutput.chipCharge();
	const std::vector<int> & electronicsTriggerVMMid         = electronicsTriggerOutput.VMMid();
	const std::vector<int> & electronicsTriggerMMFEid        = electronicsTriggerOutput.MMFEVMMid();
	std::vector<int>   electronicsTriggerAppliedDeadtimeStripPos;
	std::vector<float> electronicsTriggerAppliedDeadtimeStripTime;
	std::vector<float> electronicsTriggerAppliedDeadtimeStripCharge;
	std::vector<int>   electronicsTriggerAppliedDeadtimeVMMid;
	std::vector<int>   electronicsTriggerAppliedDeadtimeMMFEid;
	electronicsTriggerAppliedDeadtimeStripPos.clear();
	electronicsTriggerAppliedDeadtimeStripTime.clear();
	electronicsTriggerAppliedDeadtimeStripCharge.clear();
	electronicsTriggerAppliedDeadtimeVMMid.clear();
	electronicsTriggerAppliedDeadtimeMMFEid.clear();

	float deadtime = m_artDeadTime;
	std::vector<int> v_id = electronicsTriggerVMMid;

	for(size_t i = 0; i<electronicsTriggerStripPos.size(); i++){
		int id = v_id[i];
		float time = electronicsTriggerStripTime[i];
		bool DEAD = deadChannel(id,
			time,
			electronicsTriggerAppliedDeadtimeVMMid,
			electronicsTriggerAppliedDeadtimeStripTime,
			deadtime);

		if(!DEAD){
			electronicsTriggerAppliedDeadtimeStripPos.push_back(electronicsTriggerStripPos[i]);
			electronicsTriggerAppliedDeadtimeStripTime.push_back(electronicsTriggerStripTime[i]);
			electronicsTriggerAppliedDeadtimeStripCharge.push_back(electronicsTriggerStripCharge[i]);
			electronicsTriggerAppliedDeadtimeVMMid.push_back(electronicsTriggerVMMid[i]);
			electronicsTriggerAppliedDeadtimeMMFEid.push_back(electronicsTriggerMMFEid[i]);
		}
	}

	MM_ElectronicsToolTriggerOutput electronicsTriggerOutputAppliedDeadTime(
		electronicsTriggerAppliedDeadtimeStripPos,
		electronicsTriggerAppliedDeadtimeStripCharge,
		electronicsTriggerAppliedDeadtimeStripTime,
		electronicsTriggerAppliedDeadtimeVMMid,
		electronicsTriggerAppliedDeadtimeMMFEid );

	return electronicsTriggerOutputAppliedDeadTime;
}
bool MM_ElectronicsResponseSimulation::deadChannel(int id, float time, std::vector<int> & v_id, const std::vector<float> & v_time, float deadtime){
	bool DEAD = false;
	for(size_t ii = 0; ii<v_id.size(); ii++){
		if(id == v_id[ii]){
			if(v_time[ii]<time && time-v_time[ii]<deadtime){
				DEAD = true;
				continue;
			}
		}
	}
	return DEAD;
}

MM_ElectronicsResponseSimulation::~MM_ElectronicsResponseSimulation()
{
	clearValues();
}
/*******************************************************************************/

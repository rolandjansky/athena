/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
*  ElectronicsResponseSimulation.cxx
*  MC for micromegas athena integration
*
**/
//~
/// PROJECTS
#include "GaudiKernel/MsgStream.h"
#include "MM_Digitization/ElectronicsResponseSimulation.h"

// #include <random>
#include "TF1.h"

std::vector<float> shaperInputTime;
std::vector<float> shaperInputCharge;
// set drift electron's timing and charge via above vector before use function
double shaperResponseFunction(double *x, double *par){
	double response=0;
	for(size_t i=0; i<shaperInputTime.size(); i++){
		double amp = (x[0]>shaperInputTime[i])? shaperInputCharge[i]*pow((x[0]-shaperInputTime[i])/par[1],par[0])*exp(-(x[0]-shaperInputTime[i])/par[1]) :0;
		response += amp;
	}
	return response;
}

/*******************************************************************************/
ElectronicsResponseSimulation::ElectronicsResponseSimulation():
	m_peakTime(0),
	m_alpha(0),
	m_timeWindowLowerOffset(0),
	m_timeWindowUpperOffset(0),
	m_electronicsThreshold(0),
	m_stripDeadTime(0),
	m_artDeadTime(0),
	m_stripResponseQThreshold(0),
	m_stripResponseDriftGapWidth(0),
	m_stripResponseDriftVelocity(0),
	m_decoupleShaperFunctionParamaters(false)
{
}
/*******************************************************************************/
void ElectronicsResponseSimulation::initialize()
{

	float peakTimeMultiplier = 0;

	if(!m_decoupleShaperFunctionParamaters){

		peakTimeMultiplier = sqrt(m_peakTime / 50.);
		m_alpha = 2.5 * peakTimeMultiplier;

		m_electronicsThreshold = (m_stripResponseQThreshold * ( TMath::Power(m_alpha,m_alpha)*TMath::Exp(-m_alpha)) ) ;
		h_intFn = new TF1("intFn", shaperResponseFunction, m_timeWindowLowerOffset, m_timeWindowUpperOffset, 2 );
		h_intFn->SetParameter( 0, 2.5 * peakTimeMultiplier ); // previously split into the alpha parameter
		h_intFn->SetParameter( 1, 20. * peakTimeMultiplier ); // ... and RC parameter

	} else {

		peakTimeMultiplier = (m_peakTime / 50.);
		m_alpha = 2.5;

		m_electronicsThreshold = (m_stripResponseQThreshold * ( TMath::Power(m_alpha,m_alpha)*TMath::Exp(-m_alpha)) ) ;
		h_intFn = new TF1("intFn", shaperResponseFunction, m_timeWindowLowerOffset, m_timeWindowUpperOffset, 2 );
		h_intFn->SetParameter( 0, m_alpha ); // previously split into the alpha parameter
		h_intFn->SetParameter( 1, 20. * peakTimeMultiplier ); // ... and RC parameter

	}

}
/*******************************************************************************/
void ElectronicsResponseSimulation::clearValues()
{
	m_tStripElectronicsAbThr.clear();
	m_qStripElectronics.clear();
	m_nStripElectronics.clear();
}
/*******************************************************************************/
MmDigitToolOutput ElectronicsResponseSimulation::getPeakResponseFrom(const MmElectronicsToolInput & digiInput)
{
	clearValues();

	vmmPeakResponseFunction(digiInput.NumberOfStripsPos(), digiInput.chipCharge(), digiInput.chipTime() );

	/// ToDo: include loop for calculating Trigger study vars
	// MmDigitToolOutput(bool hitWasEff, std::vector <int> strpos, std::vector<float> time, std::vector<int> charge, int strTrig, float strTimeTrig ):
	MmDigitToolOutput tmp(true, m_nStripElectronics, m_tStripElectronicsAbThr, m_qStripElectronics, 5, 0.3);

	return tmp;
}
/*******************************************************************************/
MmDigitToolOutput ElectronicsResponseSimulation::getThresholdResponseFrom(const MmElectronicsToolInput & digiInput)
{
	clearValues();
	vmmThresholdResponseFunction(digiInput.NumberOfStripsPos(), digiInput.chipCharge(), digiInput.chipTime() );
	MmDigitToolOutput tmp(true, m_nStripElectronics, m_tStripElectronicsAbThr, m_qStripElectronics, 5, 0.3);
	return tmp;
}
/*******************************************************************************/
void ElectronicsResponseSimulation::vmmPeakResponseFunction(const vector <int> & numberofStrip, const vector<vector <float>> & qStrip, const vector<vector <float>> & tStrip){

	for (unsigned int ii = 0; ii < numberofStrip.size(); ii++) {

		//find min and max times for each strip:
		double maxChargeThisStrip = 0;
		double maxChargeLeftNeighbor = 0;
		double maxChargeRightNeighbor = 0;

		// Athena::MsgStreamMember log("ElectronicsResponseSimulation::VMMResponseFunction");
		// find the maximum charge:
		if ( ii > 0 ) {
			// log << MSG::DEBUG << "for Left neighbor:   tStrip.at(ii-1) "<< tStrip.at(ii-1) << " qStrip.at(ii-1) " << qStrip.at(ii-1) << endmsg;
			shaperInputTime = tStrip.at(ii-1);
			shaperInputCharge = qStrip.at(ii-1);
			maxChargeLeftNeighbor = h_intFn->GetMaximum(m_timeWindowLowerOffset,m_timeWindowUpperOffset);
			//  maxChargeLeftNeighbor = h_intFn->GetMaximum(0,3*(m_stripResponseDriftGap/m_stripResponseDriftVelocity));
		}

		if ( ii+1 < numberofStrip.size() ) {
			// log << MSG::DEBUG << "for Right neighbor:   tStrip.at(ii+1) "<< tStrip.at(ii+1) << " qStrip.at(ii+1) " << qStrip.at(ii+1) << endmsg;
			shaperInputTime = tStrip.at(ii+1);
			shaperInputCharge = qStrip.at(ii+1);
			maxChargeRightNeighbor = h_intFn->GetMaximum(m_timeWindowLowerOffset,m_timeWindowUpperOffset);
			//  maxChargeRightNeighbor = h_intFn->GetMaximum(0,3*(m_stripResponseDriftGap/m_stripResponseDriftVelocity));
		}

		// log << MSG::DEBUG << "for This strip:    tStrip.at(ii) "<< tStrip.at(ii) << " qStrip.at(ii) " << qStrip.at(ii) << endmsg;
		shaperInputTime = tStrip.at(ii);
		shaperInputCharge = qStrip.at(ii);
		maxChargeThisStrip = h_intFn->GetMaximum(m_timeWindowLowerOffset,m_timeWindowUpperOffset);
		//	maxChargeThisStrip = h_intFn->GetMaximum(0,3*(m_stripResponseDriftGap/m_stripResponseDriftVelocity));


		// log << MSG::DEBUG << "Check if a strip or its neighbors were above electronics threshold (" << m_electronicsThreshold << "):   maxChargeLeftNeighbor: " << maxChargeLeftNeighbor << ", maxChargeRightNeighbor: " << maxChargeRightNeighbor << ", maxChargeThisStrip: " << maxChargeThisStrip << endmsg;

		// Look at strip if it or its neighbor was above threshold:
		if ( maxChargeLeftNeighbor > m_electronicsThreshold || maxChargeRightNeighbor > m_electronicsThreshold || maxChargeThisStrip > m_electronicsThreshold ) {
			shaperInputTime = tStrip.at(ii);
			shaperInputCharge = qStrip.at(ii);
			// float localPeak = 0;
			float localPeakt = 0;
			float localPeakq = 0;

			float stepSize = 0.1; //ns(?) step size corresponding to VMM discriminator

			for (int jj = 0; jj < (m_timeWindowUpperOffset-m_timeWindowLowerOffset)/stepSize; jj++) {
			//            for (int jj = 0; jj < (3*( m_stripResponseDriftGap/m_stripResponseDriftVelocity))/stepSize; jj++) {

				float thisStep = m_timeWindowLowerOffset+jj*stepSize;
				float nextStep = m_timeWindowLowerOffset+(jj+1)*stepSize;
				float oneAfterStep = m_timeWindowLowerOffset+(jj+2)*stepSize;

				//                if (localPeak == 0 ) {  //haven't found a local peak yet

				//check if the charge for the next points is less than the current step and the derivative of the first point is positive or 0 and the next point is negative:
				if ( ( h_intFn->Eval(thisStep,0,0) < h_intFn->Eval(nextStep,0,0) ) && ( h_intFn->Eval(nextStep,0,0) > h_intFn->Eval(oneAfterStep,0,0) ) && (h_intFn->Eval(thisStep+0.001)-h_intFn->Eval(thisStep-0.001))/0.001 > 0.0 && (h_intFn->Eval(oneAfterStep+0.001)-h_intFn->Eval(oneAfterStep-0.001))/0.001 < 0.0 ){ // h_intFn->Derivative() does not work. WHY? 2016/07/18
					//	      if ( ( h_intFn->Eval(thisStep,0,0) < h_intFn->Eval(nextStep,0,0) ) && ( h_intFn->Eval(nextStep,0,0) > h_intFn->Eval(oneAfterStep,0,0) ) && (h_intFn->Derivative(thisStep,0,0.001) > 0 && h_intFn->Derivative(oneAfterStep,0,0.001) < 0 ) ) {

					// localPeak = 1;
					localPeakt = nextStep;
					localPeakq = h_intFn->Eval(nextStep,0,0);

							// log << MSG::DEBUG << "found a peak!    for strip number: " << numberofStrip.at(ii) << " at time: " << nextStep << " with charge: " << h_intFn->Eval(nextStep,0,0) << endmsg;
					m_nStripElectronics.push_back(numberofStrip.at(ii));
					m_tStripElectronicsAbThr.push_back(localPeakt);
					m_qStripElectronics.push_back(localPeakq);
				}
				//                }
			}
		}
	}
}///end of VMM response function
/*******************************************************************************/




void ElectronicsResponseSimulation::vmmThresholdResponseFunction(const vector <int> & numberofStrip, const vector<vector <float>> & qStrip, const vector<vector <float>> & tStrip){

	//    float tmp_Stripq = 0;
	//    float tmp_Stript = 9999.0;

	for (unsigned int ii = 0; ii < numberofStrip.size(); ii++) {

		// Athena::MsgStreamMember log("ElectronicsResponseSimulation::VMMTriggerResponseFunction");

		shaperInputTime = tStrip.at(ii);
		shaperInputCharge = qStrip.at(ii);

		float localThresholdt = 0;
		float localThresholdq = 0;

		float stepSize = 0.1; //ns(?) step size corresponding to VMM discriminator

		for (int jj = 0; jj < (m_timeWindowUpperOffset-m_timeWindowLowerOffset)/stepSize; jj++) {
		//      for (int jj = 0; jj < (3*( m_stripResponseDriftGap/m_stripResponseDriftVelocity))/stepSize; jj++) {

			float thisStep = m_timeWindowLowerOffset+jj*stepSize;
			float preStep = (jj>0) ? m_timeWindowLowerOffset+(jj-1)*stepSize: 0.0;

			if ( ( h_intFn->Eval(thisStep,0,0) >  m_electronicsThreshold) && (h_intFn->Eval(thisStep+0.001)-h_intFn->Eval(thisStep-0.001))/0.001 > 0.0  && ( h_intFn->Eval(preStep,0,0) <  m_electronicsThreshold) ) {
				localThresholdt = thisStep;
				localThresholdq = h_intFn->Eval(thisStep,0,0);
				m_nStripElectronics.push_back(numberofStrip.at(ii));
				m_tStripElectronicsAbThr.push_back(localThresholdt);
				m_qStripElectronics.push_back(localThresholdq);
			}
		}
	}
}///end of VMM response function
/*******************************************************************************/


MmElectronicsToolTriggerOutput ElectronicsResponseSimulation::getTheFastestSignalInVMM(
	const MmDigitToolOutput & ElectronicThresholdOutput,
	const int chMax,
	const int stationEta){

	const std::vector<int>  & m_electronicsThresholdStripPos    = ElectronicThresholdOutput.stripPos();
	const std::vector<float> & m_electronicsThresholdStripTime   = ElectronicThresholdOutput.stripTime();
	const std::vector<float> & m_electronicsThresholdStripCharge = ElectronicThresholdOutput.stripCharge();
	std::vector<int> trigger_MMFE_VMM_id;
	std::vector<int> trigger_VMM_id;
	getVMMId(m_electronicsThresholdStripPos, chMax, stationEta, trigger_VMM_id, trigger_MMFE_VMM_id);

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
		for(size_t i = 0; i<m_electronicsThresholdStripPos.size(); i++){
			if(m_electronicsThresholdStripTime[i] >= timeWindowLower &&
				m_electronicsThresholdStripTime[i] < timeWindowUpper){

				int VMM_id = trigger_VMM_id[i];
				std::vector< int >::iterator VMM_flag = std::find(tmp_VMM_id.begin(), tmp_VMM_id.end(), VMM_id);
				if(VMM_flag != tmp_VMM_id.end()) continue; // Already filled
				tmp_VMM_id.push_back(VMM_id);

				// Get id for the fastest signal in a VMM
				int theFastestSignal = getIdTheFastestSignalInVMM(
					m_electronicsThresholdStripTime[i],
					VMM_id,
					trigger_VMM_id,
					m_electronicsThresholdStripTime,
					(float)m_timeWindowLowerOffset+istep*BXtime,
					(float)m_timeWindowLowerOffset+(istep+1)*BXtime);

				electronicsTriggerStripPos.push_back(    m_electronicsThresholdStripPos[theFastestSignal]    );
				electronicsTriggerStripTime.push_back(   m_electronicsThresholdStripTime[theFastestSignal]   );
				electronicsTriggerStripCharge.push_back( m_electronicsThresholdStripCharge[theFastestSignal] );
				electronicsTriggerVMMid.push_back(       trigger_VMM_id[theFastestSignal]                    );
				electronicsTriggerMMFEid.push_back(      trigger_MMFE_VMM_id[theFastestSignal]               );

			}
		}
	}

	MmElectronicsToolTriggerOutput ElectronicsTriggerOutput (
		electronicsTriggerStripPos,
		electronicsTriggerStripCharge,
		electronicsTriggerStripTime,
		electronicsTriggerVMMid,
		electronicsTriggerMMFEid );

	return ElectronicsTriggerOutput;
}

int ElectronicsResponseSimulation::getIdTheFastestSignalInVMM(
	float time,
	int VMM_id,
	std::vector<int> trigger_VMM_id,
	const std::vector<float> m_electronicsThresholdStripTime,
	float timeWindowLower,
	float timeWindowUpper){
	int theFastestSignal = -1;
	float min_time = 9999.0;
	for(size_t ii = 0; ii<trigger_VMM_id.size(); ii++){
		if(m_electronicsThresholdStripTime[ii]>= timeWindowLower &&
			m_electronicsThresholdStripTime[ii] < timeWindowUpper){

			if(trigger_VMM_id[ii]==VMM_id){
				if(time<min_time){
					theFastestSignal = ii;
					min_time=time;
				}
			}
		}
	}
	if(theFastestSignal==-1) std::cout << "There is something wrong in getIdTheFastestSignalInVMM" << std::endl;

	return theFastestSignal;
}

void ElectronicsResponseSimulation::getVMMId(const std::vector< int > & m_electronicsThresholdStripPos,
	const int chMax,
	const int stationEta,
	std::vector< int > & trigger_VMM_id,
	std::vector< int > & trigger_MMFE_VMM_id){

	trigger_MMFE_VMM_id.clear();
	trigger_VMM_id.clear();
	const int VMMch = 64;
	electronics VmmMapping;
	for (size_t i=0;i<m_electronicsThresholdStripPos.size(); i++){
		int vmm_id = (int) m_electronicsThresholdStripPos.at(i)/VMMch;
		int mmfe_vmm_id = VmmMapping.elec(
			m_electronicsThresholdStripPos.at(i),
			"VMM",
			stationEta,
			chMax);
		trigger_MMFE_VMM_id.push_back(mmfe_vmm_id);
		trigger_VMM_id.push_back(vmm_id);
	}
}

MmDigitToolOutput ElectronicsResponseSimulation::applyDeadTimeStrip(const MmDigitToolOutput & electronicsTriggerOutput){

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
	//    else std::cout <<  "Killed due to the strip dead time" << std::endl;
	}

	MmDigitToolOutput ElectronicsTriggerOutputAppliedDeadTime(
		true,  // meaningless
		electronicsAppliedDeadtimeStripPos,
		electronicsAppliedDeadtimeStripTime,
		electronicsAppliedDeadtimeStripCharge,
		5,  // meaningless
		0.0  ); // meaningless

	return ElectronicsTriggerOutputAppliedDeadTime;
}


MmElectronicsToolTriggerOutput ElectronicsResponseSimulation::applyARTTiming(const MmElectronicsToolTriggerOutput & electronicsTriggerOutput, float jitter, float offset){



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

	MmElectronicsToolTriggerOutput electronicsTriggerOutputAppliedTiming(
		electronicsTriggerAppliedTimingStripPos,
		electronicsTriggerAppliedTimingStripCharge,
		electronicsTriggerAppliedTimingStripTime,
		electronicsTriggerAppliedTimingVMMId,
		electronicsTriggerAppliedTimingMMFEId );

	return electronicsTriggerOutputAppliedTiming;
}


MmElectronicsToolTriggerOutput ElectronicsResponseSimulation::applyDeadTimeART(const MmElectronicsToolTriggerOutput & electronicsTriggerOutput){

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

	MmElectronicsToolTriggerOutput electronicsTriggerOutputAppliedDeadTime(
		electronicsTriggerAppliedDeadtimeStripPos,
		electronicsTriggerAppliedDeadtimeStripCharge,
		electronicsTriggerAppliedDeadtimeStripTime,
		electronicsTriggerAppliedDeadtimeVMMid,
		electronicsTriggerAppliedDeadtimeMMFEid );

	return electronicsTriggerOutputAppliedDeadTime;
}
bool ElectronicsResponseSimulation::deadChannel(int id, float time, std::vector<int> & v_id, const std::vector<float> & v_time, float deadtime){
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

ElectronicsResponseSimulation::~ElectronicsResponseSimulation()
{
	if (h_intFn) delete h_intFn;
	clearValues();
}
/*******************************************************************************/

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
	m_stripdeadtime(0),
	m_ARTdeadtime(0),
	m_StripResponse_qThreshold(0),
	m_StripResponse_driftGapWidth(0),
	m_StripResponse_driftVelocity(0),
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

		m_electronicsThreshold = (m_StripResponse_qThreshold * ( TMath::Power(m_alpha,m_alpha)*TMath::Exp(-m_alpha)) ) ;
		h_intFn = new TF1("intFn", shaperResponseFunction, m_timeWindowLowerOffset, m_timeWindowUpperOffset, 2 );
		h_intFn->SetParameter( 0, 2.5 * peakTimeMultiplier ); // previously split into the alpha parameter
		h_intFn->SetParameter( 1, 20. * peakTimeMultiplier ); // ... and RC parameter

	} else {

		peakTimeMultiplier = (m_peakTime / 50.);
		m_alpha = 2.5;

		m_electronicsThreshold = (m_StripResponse_qThreshold * ( TMath::Power(m_alpha,m_alpha)*TMath::Exp(-m_alpha)) ) ;
		h_intFn = new TF1("intFn", shaperResponseFunction, m_timeWindowLowerOffset, m_timeWindowUpperOffset, 2 );
		h_intFn->SetParameter( 0, m_alpha ); // previously split into the alpha parameter
		h_intFn->SetParameter( 1, 20. * peakTimeMultiplier ); // ... and RC parameter

	}

}
/*******************************************************************************/
void ElectronicsResponseSimulation::clearValues()
{
	tStripElectronicsAbThr.clear();
	qStripElectronics.clear();
	nStripElectronics.clear();
}
/*******************************************************************************/
MmDigitToolOutput ElectronicsResponseSimulation::GetPeakResponseFrom(const MmElectronicsToolInput & digiInput)
{
	clearValues();

	VMMPeakResponseFunction(digiInput.NumberOfStripsPos(), digiInput.chipCharge(), digiInput.chipTime() );

	/// ToDo: include loop for calculating Trigger study vars
	// MmDigitToolOutput(bool hitWasEff, std::vector <int> strpos, std::vector<float> time, std::vector<int> charge, int strTrig, float strTimeTrig ):
	MmDigitToolOutput tmp(true, nStripElectronics, tStripElectronicsAbThr, qStripElectronics, 5, 0.3);

	return tmp;
}
/*******************************************************************************/
MmDigitToolOutput ElectronicsResponseSimulation::GetThresholdResponseFrom(const MmElectronicsToolInput & digiInput)
{
	clearValues();
	VMMThresholdResponseFunction(digiInput.NumberOfStripsPos(), digiInput.chipCharge(), digiInput.chipTime() );
	MmDigitToolOutput tmp(true, nStripElectronics, tStripElectronicsAbThr, qStripElectronics, 5, 0.3);
	return tmp;
}
/*******************************************************************************/
void ElectronicsResponseSimulation::VMMPeakResponseFunction(const vector <int> & numberofStrip, const vector<vector <float>> & qStrip, const vector<vector <float>> & tStrip){

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
			//  maxChargeLeftNeighbor = h_intFn->GetMaximum(0,3*(m_StripResponse_driftGap/m_StripResponse_driftVelocity));
		}

		if ( ii+1 < numberofStrip.size() ) {
			// log << MSG::DEBUG << "for Right neighbor:   tStrip.at(ii+1) "<< tStrip.at(ii+1) << " qStrip.at(ii+1) " << qStrip.at(ii+1) << endmsg;
			shaperInputTime = tStrip.at(ii+1);
			shaperInputCharge = qStrip.at(ii+1);
			maxChargeRightNeighbor = h_intFn->GetMaximum(m_timeWindowLowerOffset,m_timeWindowUpperOffset);
			//  maxChargeRightNeighbor = h_intFn->GetMaximum(0,3*(m_StripResponse_driftGap/m_StripResponse_driftVelocity));
		}

		// log << MSG::DEBUG << "for This strip:    tStrip.at(ii) "<< tStrip.at(ii) << " qStrip.at(ii) " << qStrip.at(ii) << endmsg;
		shaperInputTime = tStrip.at(ii);
		shaperInputCharge = qStrip.at(ii);
		maxChargeThisStrip = h_intFn->GetMaximum(m_timeWindowLowerOffset,m_timeWindowUpperOffset);
		//	maxChargeThisStrip = h_intFn->GetMaximum(0,3*(m_StripResponse_driftGap/m_StripResponse_driftVelocity));


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
			//            for (int jj = 0; jj < (3*( m_StripResponse_driftGap/m_StripResponse_driftVelocity))/stepSize; jj++) {

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
					nStripElectronics.push_back(numberofStrip.at(ii));
					tStripElectronicsAbThr.push_back(localPeakt);
					qStripElectronics.push_back(localPeakq);
				}
				//                }
			}
		}
	}
}///end of VMM response function
/*******************************************************************************/




void ElectronicsResponseSimulation::VMMThresholdResponseFunction(const vector <int> & numberofStrip, const vector<vector <float>> & qStrip, const vector<vector <float>> & tStrip){

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
		//      for (int jj = 0; jj < (3*( m_StripResponse_driftGap/m_StripResponse_driftVelocity))/stepSize; jj++) {

			float thisStep = m_timeWindowLowerOffset+jj*stepSize;
			float preStep = (jj>0) ? m_timeWindowLowerOffset+(jj-1)*stepSize: 0.0;

			if ( ( h_intFn->Eval(thisStep,0,0) >  m_electronicsThreshold) && (h_intFn->Eval(thisStep+0.001)-h_intFn->Eval(thisStep-0.001))/0.001 > 0.0  && ( h_intFn->Eval(preStep,0,0) <  m_electronicsThreshold) ) {
				localThresholdt = thisStep;
				localThresholdq = h_intFn->Eval(thisStep,0,0);
				nStripElectronics.push_back(numberofStrip.at(ii));
				tStripElectronicsAbThr.push_back(localThresholdt);
				qStripElectronics.push_back(localThresholdq);
			}
		}
	}
}///end of VMM response function
/*******************************************************************************/


MmElectronicsToolTriggerOutput ElectronicsResponseSimulation::GetTheFastestSignalInVMM(
	const MmDigitToolOutput & ElectronicThresholdOutput,
	const int chMax,
	const int stationEta){

	const std::vector<int>  & m_ElectronicsThreshold_stripPos    = ElectronicThresholdOutput.stripPos();
	const std::vector<float> & m_ElectronicsThreshold_stripTime   = ElectronicThresholdOutput.stripTime();
	const std::vector<float> & m_ElectronicsThreshold_stripCharge = ElectronicThresholdOutput.stripCharge();
	std::vector<int> trigger_MMFE_VMM_id;
	std::vector<int> trigger_VMM_id;
	GetVMMId(m_ElectronicsThreshold_stripPos, chMax, stationEta, trigger_VMM_id, trigger_MMFE_VMM_id);

	std::vector<int>   ElectronicsTrigger_stripPos;
	std::vector<float> ElectronicsTrigger_stripTime;
	std::vector<float> ElectronicsTrigger_stripCharge;
	std::vector<int>   ElectronicsTrigger_VMMid;
	std::vector<int>   ElectronicsTrigger_MMFEid;
	ElectronicsTrigger_stripPos.clear();
	ElectronicsTrigger_stripTime.clear();
	ElectronicsTrigger_stripCharge.clear();
	ElectronicsTrigger_VMMid.clear();
	ElectronicsTrigger_MMFEid.clear();

	const int BXtime = 250; // (ns)
	int nstep = (int)(m_timeWindowUpperOffset-m_timeWindowLowerOffset)/BXtime;
	if((int)(m_timeWindowUpperOffset-m_timeWindowLowerOffset)%BXtime>0 ) nstep += 1;

	for(int istep = 0; istep<nstep ; istep++){
		float timeWindowLower = (float) m_timeWindowLowerOffset+istep*BXtime;
		float timeWindowUpper = (float) m_timeWindowLowerOffset+(istep+1)*BXtime;

		std::vector<int> tmp_VMM_id; tmp_VMM_id.clear();
		for(size_t i = 0; i<m_ElectronicsThreshold_stripPos.size(); i++){
			if(m_ElectronicsThreshold_stripTime[i] >= timeWindowLower &&
				m_ElectronicsThreshold_stripTime[i] < timeWindowUpper){

				int VMM_id = trigger_VMM_id[i];
				std::vector< int >::iterator VMM_flag = std::find(tmp_VMM_id.begin(), tmp_VMM_id.end(), VMM_id);
				if(VMM_flag != tmp_VMM_id.end()) continue; // Already filled
				tmp_VMM_id.push_back(VMM_id);

				// Get id for the fastest signal in a VMM
				int theFastestSignal = GetIdTheFastestSignalInVMM(
					m_ElectronicsThreshold_stripTime[i],
					VMM_id,
					trigger_VMM_id,
					m_ElectronicsThreshold_stripTime,
					(float)m_timeWindowLowerOffset+istep*BXtime,
					(float)m_timeWindowLowerOffset+(istep+1)*BXtime);

				ElectronicsTrigger_stripPos.push_back(m_ElectronicsThreshold_stripPos[theFastestSignal]);
				ElectronicsTrigger_stripTime.push_back(m_ElectronicsThreshold_stripTime[theFastestSignal]);
				ElectronicsTrigger_stripCharge.push_back(m_ElectronicsThreshold_stripCharge[theFastestSignal]);
				ElectronicsTrigger_VMMid.push_back(trigger_VMM_id[theFastestSignal]);
				ElectronicsTrigger_MMFEid.push_back(trigger_MMFE_VMM_id[theFastestSignal]);

			}
		}
	}

	MmElectronicsToolTriggerOutput ElectronicsTriggerOutput (
		ElectronicsTrigger_stripPos,
		ElectronicsTrigger_stripCharge,
		ElectronicsTrigger_stripTime,
		ElectronicsTrigger_VMMid,
		ElectronicsTrigger_MMFEid );

	return ElectronicsTriggerOutput;
}

int ElectronicsResponseSimulation::GetIdTheFastestSignalInVMM(
	float time,
	int VMM_id,
	std::vector<int> trigger_VMM_id,
	const std::vector<float> m_ElectronicsThreshold_stripTime,
	float timeWindowLower,
	float timeWindowUpper){
	int theFastestSignal = -1;
	float min_time = 9999.0;
	for(size_t ii = 0; ii<trigger_VMM_id.size(); ii++){
		if(m_ElectronicsThreshold_stripTime[ii]>= timeWindowLower &&
			m_ElectronicsThreshold_stripTime[ii] < timeWindowUpper){

			if(trigger_VMM_id[ii]==VMM_id){
				if(time<min_time){
					theFastestSignal = ii;
					min_time=time;
				}
			}
		}
	}
	if(theFastestSignal==-1) std::cout << "There is something wrong in GetIdTheFastestSignalInVMM" << std::endl;

	return theFastestSignal;
}

void ElectronicsResponseSimulation::GetVMMId(const std::vector< int > & m_ElectronicsThreshold_stripPos,
	const int chMax,
	const int stationEta,
	std::vector< int > & trigger_VMM_id,
	std::vector< int > & trigger_MMFE_VMM_id){

	trigger_MMFE_VMM_id.clear();
	trigger_VMM_id.clear();
	const int VMMch = 64;
	electronics VmmMapping;
	for (size_t i=0;i<m_ElectronicsThreshold_stripPos.size(); i++){
		int vmm_id = (int) m_ElectronicsThreshold_stripPos.at(i)/VMMch;
		int mmfe_vmm_id = VmmMapping.elec(m_ElectronicsThreshold_stripPos.at(i), "VMM", stationEta, chMax);
		trigger_MMFE_VMM_id.push_back(mmfe_vmm_id);
		trigger_VMM_id.push_back(vmm_id);
	}
}

MmDigitToolOutput ElectronicsResponseSimulation::ApplyDeadTimeStrip(const MmDigitToolOutput & ElectronicsTriggerOutput){

	const std::vector<int>  & Electronics_stripPos    = ElectronicsTriggerOutput.stripPos();
	const std::vector<float> & Electronics_stripTime   = ElectronicsTriggerOutput.stripTime();
	const std::vector<float> & Electronics_stripCharge = ElectronicsTriggerOutput.stripCharge();
	std::vector<int>   ElectronicsAppliedDeadtime_stripPos;
	std::vector<float> ElectronicsAppliedDeadtime_stripTime;
	std::vector<float> ElectronicsAppliedDeadtime_stripCharge;
	ElectronicsAppliedDeadtime_stripPos.clear();
	ElectronicsAppliedDeadtime_stripTime.clear();
	ElectronicsAppliedDeadtime_stripCharge.clear();

	float deadtime = m_stripdeadtime;
	std::vector<int> v_id = Electronics_stripPos;

	for(size_t i = 0; i<Electronics_stripPos.size(); i++){
		int id = v_id[i];
		float time = Electronics_stripTime[i];
		bool DEAD = DeadChannel(id, time, ElectronicsAppliedDeadtime_stripPos, ElectronicsAppliedDeadtime_stripTime, deadtime);
		if(!DEAD){
			ElectronicsAppliedDeadtime_stripPos.push_back(Electronics_stripPos[i]);
			ElectronicsAppliedDeadtime_stripTime.push_back(Electronics_stripTime[i]);
			ElectronicsAppliedDeadtime_stripCharge.push_back(Electronics_stripCharge[i]);
		}
	//    else std::cout <<  "Killed due to the strip dead time" << std::endl;
	}

	MmDigitToolOutput ElectronicsTriggerOutputAppliedDeadTime(
		true,  // meaningless
		ElectronicsAppliedDeadtime_stripPos,
		ElectronicsAppliedDeadtime_stripTime,
		ElectronicsAppliedDeadtime_stripCharge,
		5,  // meaningless
		0.0  ); // meaningless

	return ElectronicsTriggerOutputAppliedDeadTime;
}


MmElectronicsToolTriggerOutput ElectronicsResponseSimulation::ApplyARTTiming(const MmElectronicsToolTriggerOutput & ElectronicsTriggerOutput, float jitter, float offset){



	const std::vector<int>  & ElectronicsTrigger_stripPos    = ElectronicsTriggerOutput.NumberOfStripsPos();
	const std::vector<float> & ElectronicsTrigger_stripTime   = ElectronicsTriggerOutput.chipTime();
	const std::vector<float> & ElectronicsTrigger_stripCharge = ElectronicsTriggerOutput.chipCharge();
	const std::vector<int> & ElectronicsTrigger_VMMid = ElectronicsTriggerOutput.VMMid();
	const std::vector<int> & ElectronicsTrigger_MMFEid = ElectronicsTriggerOutput.MMFEVMMid();
	std::vector<int>   ElectronicsTriggerAppliedTiming_stripPos;
	std::vector<float> ElectronicsTriggerAppliedTiming_stripTime;
	std::vector<float> ElectronicsTriggerAppliedTiming_stripCharge;
	std::vector<int>   ElectronicsTriggerAppliedTiming_VMMid;
	std::vector<int>   ElectronicsTriggerAppliedTiming_MMFEid;
	ElectronicsTriggerAppliedTiming_stripPos.clear();
	ElectronicsTriggerAppliedTiming_stripTime.clear();
	ElectronicsTriggerAppliedTiming_stripCharge.clear();
	ElectronicsTriggerAppliedTiming_VMMid.clear();
	ElectronicsTriggerAppliedTiming_MMFEid.clear();

	//bunchTime = bunchTime + artOffset + jitter;

	TF1 gaussianSmearing("timingSmearing","gaus",offset-jitter*5,offset+jitter*5);
	gaussianSmearing.SetParameters(1,offset,jitter);



	for(size_t i = 0; i<ElectronicsTrigger_stripPos.size(); i++){
		// std::default_random_engine generator;
		// std::normal_distribution<double> distribution(offset,jitter);
		// float timingTransformation = distribution(generator);

		ElectronicsTriggerAppliedTiming_stripPos.push_back(ElectronicsTrigger_stripPos[i]);

		if(jitter || offset)    ElectronicsTriggerAppliedTiming_stripTime.push_back(ElectronicsTrigger_stripTime[i] + gaussianSmearing.GetRandom() );
		else ElectronicsTriggerAppliedTiming_stripTime.push_back(ElectronicsTrigger_stripTime[i] );

		ElectronicsTriggerAppliedTiming_stripCharge.push_back(ElectronicsTrigger_stripCharge[i]);
		ElectronicsTriggerAppliedTiming_VMMid.push_back(ElectronicsTrigger_VMMid[i]);
		ElectronicsTriggerAppliedTiming_MMFEid.push_back(ElectronicsTrigger_MMFEid[i]);

	}

	MmElectronicsToolTriggerOutput ElectronicsTriggerOutputAppliedTiming(
		ElectronicsTriggerAppliedTiming_stripPos,
		ElectronicsTriggerAppliedTiming_stripCharge,
		ElectronicsTriggerAppliedTiming_stripTime,
		ElectronicsTriggerAppliedTiming_VMMid,
		ElectronicsTriggerAppliedTiming_MMFEid );

	return ElectronicsTriggerOutputAppliedTiming;
}


MmElectronicsToolTriggerOutput ElectronicsResponseSimulation::ApplyDeadTimeART(const MmElectronicsToolTriggerOutput & ElectronicsTriggerOutput){

	const std::vector<int>  & ElectronicsTrigger_stripPos    = ElectronicsTriggerOutput.NumberOfStripsPos();
	const std::vector<float> & ElectronicsTrigger_stripTime   = ElectronicsTriggerOutput.chipTime();
	const std::vector<float> & ElectronicsTrigger_stripCharge = ElectronicsTriggerOutput.chipCharge();
	const std::vector<int> & ElectronicsTrigger_VMMid = ElectronicsTriggerOutput.VMMid();
	const std::vector<int> & ElectronicsTrigger_MMFEid = ElectronicsTriggerOutput.MMFEVMMid();
	std::vector<int>   ElectronicsTriggerAppliedDeadtime_stripPos;
	std::vector<float> ElectronicsTriggerAppliedDeadtime_stripTime;
	std::vector<float> ElectronicsTriggerAppliedDeadtime_stripCharge;
	std::vector<int>   ElectronicsTriggerAppliedDeadtime_VMMid;
	std::vector<int>   ElectronicsTriggerAppliedDeadtime_MMFEid;
	ElectronicsTriggerAppliedDeadtime_stripPos.clear();
	ElectronicsTriggerAppliedDeadtime_stripTime.clear();
	ElectronicsTriggerAppliedDeadtime_stripCharge.clear();
	ElectronicsTriggerAppliedDeadtime_VMMid.clear();
	ElectronicsTriggerAppliedDeadtime_MMFEid.clear();

	float deadtime = m_ARTdeadtime;
	std::vector<int> v_id = ElectronicsTrigger_VMMid;

	for(size_t i = 0; i<ElectronicsTrigger_stripPos.size(); i++){
		int id = v_id[i];
		float time = ElectronicsTrigger_stripTime[i];
		bool DEAD = DeadChannel(id, time, ElectronicsTriggerAppliedDeadtime_VMMid, ElectronicsTriggerAppliedDeadtime_stripTime, deadtime);
		if(!DEAD){
			ElectronicsTriggerAppliedDeadtime_stripPos.push_back(ElectronicsTrigger_stripPos[i]);
			ElectronicsTriggerAppliedDeadtime_stripTime.push_back(ElectronicsTrigger_stripTime[i]);
			ElectronicsTriggerAppliedDeadtime_stripCharge.push_back(ElectronicsTrigger_stripCharge[i]);
			ElectronicsTriggerAppliedDeadtime_VMMid.push_back(ElectronicsTrigger_VMMid[i]);
			ElectronicsTriggerAppliedDeadtime_MMFEid.push_back(ElectronicsTrigger_MMFEid[i]);
		}
	}

	MmElectronicsToolTriggerOutput ElectronicsTriggerOutputAppliedDeadTime(
		ElectronicsTriggerAppliedDeadtime_stripPos,
		ElectronicsTriggerAppliedDeadtime_stripCharge,
		ElectronicsTriggerAppliedDeadtime_stripTime,
		ElectronicsTriggerAppliedDeadtime_VMMid,
		ElectronicsTriggerAppliedDeadtime_MMFEid );

	return ElectronicsTriggerOutputAppliedDeadTime;
}
bool ElectronicsResponseSimulation::DeadChannel(int id, float time, std::vector<int> & v_id, const std::vector<float> & v_time, float deadtime){
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

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
*  MM_ElectronicsResponseSimulation.cxx
*  MC for micromegas athena integration
*
**/
//~
/// PROJECTS
#include <numeric>

#include "GaudiKernel/MsgStream.h"
#include "MM_Digitization/MM_ElectronicsResponseSimulation.h"

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
MM_ElectronicsResponseSimulation::MM_ElectronicsResponseSimulation():
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
  m_useNeighborLogic(true),
  m_decoupleShaperFunctionParamaters(false)
{
}
/*******************************************************************************/
void MM_ElectronicsResponseSimulation::initialize()
{
  
  float peakTimeMultiplier = 0;
  
  if(!m_decoupleShaperFunctionParamaters){
    
    peakTimeMultiplier = sqrt(m_peakTime / 50.);
    m_alpha = 2.5 * peakTimeMultiplier;
    
    m_h_intFn = new TF1("intFn", shaperResponseFunction, m_timeWindowLowerOffset, m_timeWindowUpperOffset, 2 );
    m_h_intFn->SetParameter( 0, 2.5 * peakTimeMultiplier ); // previously split into the alpha parameter
    m_h_intFn->SetParameter( 1, 20. * peakTimeMultiplier ); // ... and RC parameter
    
  } else {
    
    peakTimeMultiplier = (m_peakTime / 50.);
    m_alpha = 2.5;
    
    m_h_intFn = new TF1("intFn", shaperResponseFunction, m_timeWindowLowerOffset, m_timeWindowUpperOffset, 2 );
    m_h_intFn->SetParameter( 0, m_alpha ); // previously split into the alpha parameter
    m_h_intFn->SetParameter( 1, 20. * peakTimeMultiplier ); // ... and RC parameter
    
  }
  
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
  
  vmmPeakResponseFunction(digiInput.NumberOfStripsPos(), digiInput.chipCharge(), digiInput.chipTime(), thresholdScaleFactor );
  
  /// ToDo: include loop for calculating Trigger study vars
  // MM_DigitToolOutput(bool hitWasEff, std::vector <int> strpos, std::vector<float> time, std::vector<int> charge, int strTrig, float strTimeTrig ):
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
  for (unsigned int ii = 0; ii < numberofStrip.size(); ii++) {
    
    //find min and max times for each strip:
    double maxChargeThisStrip = 0;
    double maxChargeLeftNeighbor = 0;
    double maxChargeRightNeighbor = 0;
    
    // find the maximum charge:
    if(m_useNeighborLogic){// only check neighbor strips if VMM neighbor logic is enabled
      if ( ii > 0 ) {
	shaperInputTime = tStrip.at(ii-1);
	shaperInputCharge = qStrip.at(ii-1);
    // for now by pass the VMM charge measurement and use the total charge on the strip
    // maxChargeLeftNeighbor = m_h_intFn->GetMaximum(m_timeWindowLowerOffset,m_timeWindowUpperOffset);
    maxChargeLeftNeighbor = std::accumulate(shaperInputCharge.begin(), shaperInputCharge.end(), 0.0);
      }
      
      if ( ii+1 < numberofStrip.size() ) {
	shaperInputTime = tStrip.at(ii+1);
	shaperInputCharge = qStrip.at(ii+1);
    // for now by pass the VMM charge measurement and use the total charge on the strip
    //maxChargeRightNeighbor = m_h_intFn->GetMaximum(m_timeWindowLowerOffset,m_timeWindowUpperOffset);
    maxChargeRightNeighbor = std::accumulate(shaperInputCharge.begin(), shaperInputCharge.end(), 0.0);
      }
    }
    shaperInputTime = tStrip.at(ii);
    shaperInputCharge = qStrip.at(ii);
    //  for now by pass the VMM charge measurement and use the total charge on the strip
    //  maxChargeThisStrip = m_h_intFn->GetMaximum(m_timeWindowLowerOffset,m_timeWindowUpperOffset);
    maxChargeThisStrip = std::accumulate(shaperInputCharge.begin(), shaperInputCharge.end(), 0.0);

    float scaledThreshold = m_electronicsThreshold * thresholdScaleFactor;
    
    //check if neighbor strip was above threshold
    bool neighborFired = maxChargeLeftNeighbor > scaledThreshold || maxChargeRightNeighbor > scaledThreshold;
    
    // Look at strip if it or its neighbor was above threshold  and if neighbor logic of the VMM is enabled:
    if (maxChargeThisStrip > scaledThreshold || (m_useNeighborLogic && neighborFired) ) {
      shaperInputTime = tStrip.at(ii);
      shaperInputCharge = qStrip.at(ii);
      // float localPeak = 0;
      float localPeakt = 0;
      // not used for now since VMM charge measurement is temporarily by passed
      // float localPeakq = 0;

      float stepSize = 0.1;  // ns(?) step size corresponding to VMM discriminator

      for (int jj = 0; jj < (m_timeWindowUpperOffset-m_timeWindowLowerOffset)/stepSize; jj++) {

	float thisStep = m_timeWindowLowerOffset+jj*stepSize;
	float nextStep = m_timeWindowLowerOffset+(jj+1)*stepSize;
	float oneAfterStep = m_timeWindowLowerOffset+(jj+2)*stepSize;


	//check if the charge for the next points is less than the current step and the derivative of the first point is positive or 0 and the next point is negative:
	if ( ( m_h_intFn->Eval(thisStep,0,0) < m_h_intFn->Eval(nextStep,0,0) ) && ( m_h_intFn->Eval(nextStep,0,0) > m_h_intFn->Eval(oneAfterStep,0,0) ) && (m_h_intFn->Eval(thisStep+0.001)-m_h_intFn->Eval(thisStep-0.001))/0.001 > 0.0 && (m_h_intFn->Eval(oneAfterStep+0.001)-m_h_intFn->Eval(oneAfterStep-0.001))/0.001 < 0.0 ){ // m_h_intFn->Derivative() does not work. WHY? 2016/07/18

      localPeakt = nextStep;
      // not used for now since VMM charge measurement is temporarily by passed
      // localPeakq = m_h_intFn->Eval(nextStep,0,0);

      m_nStripElectronics.push_back(numberofStrip.at(ii));
      m_tStripElectronicsAbThr.push_back(localPeakt);
      // m_qStripElectronics.push_back(localPeakq);
      m_qStripElectronics.push_back(std::accumulate(shaperInputCharge.begin(), shaperInputCharge.end(), 0.0));
    }
	//                }
      }
    }
  }
}///end of VMM response function
/*******************************************************************************/




void MM_ElectronicsResponseSimulation::vmmThresholdResponseFunction(const std::vector <int> & numberofStrip, const std::vector<std::vector <float>> & qStrip, const std::vector<std::vector <float>> & tStrip, const float thresholdScaleFactor){
  
  
  for (unsigned int ii = 0; ii < numberofStrip.size(); ii++) {  
    
    shaperInputTime = tStrip.at(ii);
    shaperInputCharge = qStrip.at(ii);
    
    float localThresholdt = 0;
    float localThresholdq = 0;
    
    float stepSize = 0.1; //ns(?) step size corresponding to VMM discriminator
    
    float scaledThreshold = m_electronicsThreshold*thresholdScaleFactor;

    for (int jj = 0; jj < (m_timeWindowUpperOffset-m_timeWindowLowerOffset)/stepSize; jj++) {
      
      float thisStep = m_timeWindowLowerOffset+jj*stepSize;
      float preStep = (jj>0) ? m_timeWindowLowerOffset+(jj-1)*stepSize: 0.0;
      
      if ( ( m_h_intFn->Eval(thisStep,0,0) >  scaledThreshold) && (m_h_intFn->Eval(thisStep+0.001)-m_h_intFn->Eval(thisStep-0.001))/0.001 > 0.0  && ( m_h_intFn->Eval(preStep,0,0) <  scaledThreshold) ) {
	localThresholdt = thisStep;
	localThresholdq = m_h_intFn->Eval(thisStep,0,0);
	m_nStripElectronics.push_back(numberofStrip.at(ii));
	m_tStripElectronicsAbThr.push_back(localThresholdt);
	m_qStripElectronics.push_back(localThresholdq);
      }
    }
  }
}///end of VMM response function
/*******************************************************************************/


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
	if(theFastestSignal==-1) std::cout << "There is something wrong in getIdTheFastestSignalInVMM" << std::endl;

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
	//    else std::cout <<  "Killed due to the strip dead time" << std::endl;
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
	if (m_h_intFn) delete m_h_intFn;
	clearValues();
}
/*******************************************************************************/

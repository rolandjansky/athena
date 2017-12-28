/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
//
//     Author:
//     Alexander.Paramonov@cern.ch
//
//     Created:
//    05 February 2016
//
//  	 File Name:
//     TileRawChannelBuilderQIEFilter.h
//
//    Description:
//		The code calculates time and energy/charge of QIE pulses
//
//////////////////////////////////////////////////////////////////////

#include "TileEvent/TileRawChannel.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

// Gaudi includes
#include "GaudiKernel/Property.h"

// Atlas includes
#include "AthAllocators/DataPool.h"
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileRecUtils/TileRawChannelBuilderQIEFilter.h"
//#include "TileRecUtils/TileRawChannelBuilderQIEFilterLookup.h"
//#include "TileConditions/TileOptFilterWeights.h"
//#include "TileConditions/TilePulseShapes.h"
#include "CLHEP/Matrix/Matrix.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileDigits.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileInfo.h"

//using namespace std;
#include <algorithm>

//interface stuff
static const InterfaceID IID_ITileRawChannelBuilderQIEFilter("TileRawChannelBuilderQIEFilter", 1, 0);

const InterfaceID& TileRawChannelBuilderQIEFilter::interfaceID() {
	return IID_ITileRawChannelBuilderQIEFilter;
}

#define TILE_QIEFilterBUILDERVERBOSE false

TileRawChannelBuilderQIEFilter::TileRawChannelBuilderQIEFilter(const std::string& type, const std::string& name, const IInterface *parent) :
		TileRawChannelBuilder(type, name, parent)
//  , m_tileToolTiming("TileCondToolTiming")
//  , m_tileCondToolOfc("TileCondToolOfc")
//  , m_tileCondToolOfcCool("TileCondToolOfcCool")
//  , m_tileToolNoiseSample("TileCondToolNoiseSample")
//  , m_nSignal(0)
//  , m_nNegative(0)
//  , m_nCenter(0)
//  , m_nConst(0)
//  , m_nSamples(0)
//  , m_t0SamplePosition(0)
//  , m_maxTime(0.0)
//  , m_minTime(0.0)
{
	//declare interfaces
	declareInterface < TileRawChannelBuilder > (this);
	declareInterface < TileRawChannelBuilderQIEFilter > (this);

	m_rawChannelContainerKey = "TileRawChannelQIE";

	//declare properties
//  declareProperty("TileCondToolTiming", m_tileToolTiming);
//  declareProperty("TileCondToolOfc",    m_tileCondToolOfc  ,"TileCondToolOfc");
//  declareProperty("TileCondToolOfcCool",m_tileCondToolOfcCool  ,"TileCondToolOfcCool");
//  declareProperty("TileCondToolNoiseSample", m_tileToolNoiseSample,"TileCondToolNoiseSample");
//  declareProperty("MaxIterations",m_maxIterations = 5);
	declareProperty("PedestalMode", m_pedestalMode = 17);
//  declareProperty("TimeForConvergence",m_timeForConvergence = 0.5);
//  declareProperty("ConfTB",m_confTB = false);
//  declareProperty("OF2",m_of2 = true);
//  declareProperty("Minus1Iteration",m_minus1Iter = false);
//  declareProperty("AmplitudeCorrection",m_correctAmplitude = false);
//  declareProperty("BestPhase",m_bestPhase = false);
//  declareProperty("OfcfromCool",m_ofcFromCool = false);
//  declareProperty("EmulateDSP",m_emulateDsp = false);
}

TileRawChannelBuilderQIEFilter::~TileRawChannelBuilderQIEFilter() {
}

StatusCode TileRawChannelBuilderQIEFilter::initialize() {

	ATH_MSG_INFO("initialize()");

	m_rChType = TileFragHash::OptFilterOffline; // type for offline Opt Filter

	// init in superclass
	CHECK(TileRawChannelBuilder::initialize());

	// bits 12-15 - various options
	//if (m_correctAmplitude) m_bsflags |= 0x2000;
	//if (m_maxIterations > 1) m_bsflags |= 0x4000;
	//if (m_bestPhase) m_bsflags |= 0x8000;

	//ATH_MSG_DEBUG( " MaxIterations=" << m_maxIterations
	//              << " PedestalMode=" << m_pedestalMode
	//              << " TimeForConvergence=" << m_timeForConvergence
	//              << " ConfTB=" << m_confTB
	//              << " OF2=" << m_of2
	//              << " Minus1Iteration=" << m_minus1Iter
	//              << " AmplitudeCorrection=" << m_correctAmplitude
	//              << " Best Phase " << m_bestPhase );

	m_nSamples = m_tileInfo->NdigitSamples();
	//m_t0SamplePosition = m_tileInfo->ItrigSample();
	//m_maxTime = 25 * (m_nSamples - m_t0SamplePosition - 1);
	//m_minTime = -25 * m_t0SamplePosition;
	//ATH_MSG_DEBUG(" NSamples=" << m_nSamples
	//              << " T0Sample=" << m_t0SamplePosition
	//              << " minTime=" << m_minTime
	//              << " maxTime=" << m_maxTime );

	//if (m_pedestalMode % 10 > 2 && m_nSamples != m_pedestalMode % 10) {
	//  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Changing PedestalMode from " << m_pedestalMode;
	//  m_pedestalMode = (m_pedestalMode / 10) * 10 + m_nSamples;
	//  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " to " << m_pedestalMode << endmsg;
	//}

	if (m_nSamples != 7) { // && (m_pedestalMode == 71 || m_pedestalMode == 7621)) {
		ATH_MSG_ERROR("Incompatable pedestal mode [" << m_pedestalMode << "] and number of samples [" << m_nSamples << "]");
		return StatusCode::FAILURE;
	}

	//m_nSignal = 0;
	//m_nNegative = 0;
	//m_nCenter = 0;
	//m_nConst = 0;

	return StatusCode::SUCCESS;
}

StatusCode TileRawChannelBuilderQIEFilter::finalize() {

	/*
	 if (msgLvl(MSG::VERBOSE)) {
	 if (m_maxIterations == 1) { // Without iterations
	 msg(MSG::VERBOSE) << "Counters: Signal=" << m_nSignal
	 << " Constant=" << m_nConst
	 << " Total=" << m_nSignal + m_nConst << endmsg;
	 } else {
	 msg(MSG::VERBOSE) << "Counters: Signal=" << m_nSignal
	 << " Negat=" << m_nNegative
	 << " Center=" << m_nCenter
	 << " Constant=" << m_nConst
	 << " Total=" << m_nSignal + m_nNegative + m_nConst + m_nCenter << endmsg;
	 }
	 }*/

	ATH_MSG_DEBUG("Finalizing");

	return StatusCode::SUCCESS;
}

TileRawChannel * TileRawChannelBuilderQIEFilter::rawChannel(const TileDigits* digits) {

	++m_chCounter;

	float pedestal = 10.; // A non-zero default is used to avoid the data quality cuts.
	float energy = 0.;
	float time = 0.;
	float chi2 = 0.;
	m_digits = digits->samples(); //the 7 samples; currently charge is expected. It the future these will be digitized
	const HWIdentifier adcId = digits->adc_HWID();
	int gain = m_tileHWID->adc(adcId); //this is set to 1 for QIE FEB by default

	ATH_MSG_VERBOSE("Building Raw Channel, with QIEFilter, HWID:" << m_tileHWID->to_string(adcId) << " gain=" << gain);

	int ros = m_tileHWID->ros(adcId);
	int drawer = m_tileHWID->drawer(adcId);
	int channel = m_tileHWID->channel(adcId);
	chi2 = filter(ros, drawer, channel, energy, time);

//TODO: Energy calibration needs to be implemented
//  if (m_calibrateEnergy) {
//    energy = m_tileInfo->CisCalib(adcId, energy);
//  }

	if (msgLvl(MSG::VERBOSE)) {
		msg(MSG::VERBOSE) << "Creating QIEFilter RawChannel" << " a=" << energy << " t=" << time
		//<< " ped=" << pedestal
				<< " q=" << chi2 << endmsg;

		msg(MSG::VERBOSE) << "digits:";

		for (unsigned int i = 0; i < m_digits.size(); ++i)
			msg(MSG::VERBOSE) << " " << m_digits[i];

		msg(MSG::VERBOSE) << " " << endmsg;
	}

	// return new TileRawChannel
	// TileRawChannel *rawCh = new TileRawChannel(adcId,OptFilterEne,OptFilterTime,OptFilterChi2,OptFilterPed);
	DataPool<TileRawChannel> tileRchPool(m_dataPoollSize);
	TileRawChannel *rawCh = tileRchPool.nextElementPtr();
        rawCh->assign (adcId,
                       energy,
                       time,
                       chi2,
                       pedestal);

	//if (m_correctTime
	//    && (time != 0
	//        && time < m_maxTime
	//        && time > m_minTime)) {
	//
	//  rawCh->insertTime(m_tileInfo->TimeCalib(adcId, time));
	//  ATH_MSG_VERBOSE( "Correcting time, new time=" << rawCh->time() );
	//
	//}

	if (TileID::HIGHGAIN == gain) {
		++m_nChH;
		m_RChSumH += energy;
	} else {
		++m_nChL;
		m_RChSumL += energy;
	}

	return rawCh;
}

int TileRawChannelBuilderQIEFilter::findMaxDigitPosition() {

	ATH_MSG_VERBOSE("  findMaxDigitPosition()");

	int iMaxDigit = 0;
	float maxDigit = 0.;
	//bool saturated = false;

	for (unsigned int i = 0; i < m_digits.size(); i++) {
		//TODO: Enable when digitization is introduced
		//if (m_digits[i] > 510.99) saturated = true; //QIE returns range *128 + adc codes from 0 511
		if (maxDigit < m_digits[i]) {
			maxDigit = m_digits[i];
			iMaxDigit = i;
		}
	}

	if (msgLvl(MSG::VERBOSE)) {
		for (unsigned int i = 0; i < m_digits.size(); i++) {
			msg(MSG::VERBOSE) << " " << m_digits[i];
		}

		msg(MSG::VERBOSE) << "; Max: digit[" << iMaxDigit << "]=" << maxDigit << endmsg;

		//if (saturated)
		//	msg(MSG::VERBOSE) << " Samples saturated" << endmsg;
	}

	return iMaxDigit;
}

//There is no need to set the pedestal for QIE
//It is a part of the look-up table
/*
 float TileRawChannelBuilderQIEFilter::setPedestal(int ros, int drawer, int channel, int gain) {
 float pedestal = 0.;

 switch (m_pedestalMode) {
 case -1:
 // use pedestal from conditions DB
 pedestal = m_tileToolNoiseSample->getPed(TileCalibUtils::getDrawerIdx(ros, drawer), channel, gain);
 break;
 case 7:
 pedestal = m_digits[6];
 break;
 case 9:
 pedestal = m_digits[8];
 break;
 case 12:
 pedestal = .5 * (m_digits[0] + m_digits[1]);
 break;
 case 17:
 pedestal = .5 * (m_digits[0] + m_digits[6]);
 break;
 case 19:
 pedestal = .5 * (m_digits[0] + m_digits[8]);
 break;
 case 71:
 pedestal = std::min(m_digits[0], m_digits[6]);
 break;
 case 7621:
 pedestal = 0.5 * std::min(m_digits[0] + m_digits[1], m_digits[5] + m_digits[6]);
 break;
 default:
 pedestal = m_digits[0];
 break;
 }

 ATH_MSG_VERBOSE("setPedestal(): pedestal=" << pedestal);

 return pedestal;
 }
 */

float TileRawChannelBuilderQIEFilter::filter(int ros, int drawer, int channel, float &amplitude, float &time) {

	ATH_MSG_VERBOSE("filter()");

	amplitude = 0.;
	time = 0.;
	float chi2 = -1.;

	auto minMaxDigits = std::minmax_element(m_digits.begin(), m_digits.end());
	float minDigit = *minMaxDigits.first;
	float maxDigit = *minMaxDigits.second;

	if (maxDigit - minDigit < 0.01) { // constant value in all samples // TODO: the cut-off needs to be adjusted for the charge difference

		//pedestal = minDigit;
		chi2 = 0.;
		ATH_MSG_VERBOSE("CASE NO SIGNAL: maxdig-mindig = " << maxDigit << "-" << minDigit << " = " << maxDigit - minDigit);

		m_nConst++;

	} else {

		//pedestal = setPedestal(ros, drawer, channel, gain);
		//double phase = 0.;

		//unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
		//// AA 3.10.08 --- take best phase from COOL
		//if (m_bestPhase) {
		//	// AS 19.11.09 - note minus sign here - time in DB is opposite to best phase
		//	phase = -m_tileToolTiming->getSignalPhase(drawerIdx, channel, gain);
		//	ATH_MSG_VERBOSE("Best phase: " << phase << " drawerIdx " << drawerIdx << " channel " << channel);
		//}

		chi2 = compute(ros, drawer, channel, amplitude, time);

		//// If weights for tau=0 are used, deviations are seen in the amplitude =>
		//// function to correct the amplitude
		//if (m_correctAmplitude && amplitude > m_ampMinThresh && time > m_timeMinThresh && time < m_timeMaxThresh) {
		//
		//	amplitude *= correctAmp(time, m_of2);
		//	ATH_MSG_VERBOSE("Amplitude corrected by " << correctAmp(time, m_of2) << " new amplitude is " << amplitude);
		//}

		m_nSignal++;

	}

	return chi2;
}

/*
 int TileRawChannelBuilderOpt2Filter::iterate(int ros, int drawer, int channel, int gain, double &pedestal, double &amplitude, double &time, double &chi2) {

 ATH_MSG_VERBOSE("iterate()");

 int nIterations = 0;
 double savePhase = 0.0;
 double phase = 0.0;
 time = -1000.;

 // Mythic -1 iteration or DSP emulation case
 if (m_minus1Iter || (m_emulateDsp && (m_maxIterations > 1)))
 phase = 25 * (m_t0SamplePosition - findMaxDigitPosition());

 while ((time > m_timeForConvergence || time < (-1.) * m_timeForConvergence || m_emulateDsp) && nIterations < m_maxIterations) {

 chi2 = compute(ros, drawer, channel, gain, pedestal, amplitude, time, phase);

 savePhase = phase;

 if (m_emulateDsp)
 phase -= round(time); // rounding phase to integer like in DSP
 else if (m_ofcFromCool)
 phase -= round(time * 10.) / 10.; // rounding phase to 0.1 - OFC in DB are stored with 0.1ns steps
 else
 phase -= time; // no rounding at all for OFC on the fly

 if (phase > m_maxTime)
 phase = m_maxTime;
 if (phase < m_minTime)
 phase = m_minTime;

 ++nIterations;
 ATH_MSG_VERBOSE(
 " OptFilter computed with phase=" << savePhase << " Time=" << time << " END ITER=" << nIterations << " new phase=" << phase << " chi2=" << chi2 << "  Amp=" << amplitude);
 }

 time -= savePhase;
 if (time > m_maxTime)
 time = m_maxTime;
 if (time < m_minTime)
 time = m_minTime;

 ATH_MSG_VERBOSE("OptFilterEne=" << amplitude << " Phase=" << savePhase << " Absolute Time=" << time);

 return nIterations;
 }
 */

//charge per 25 ns is calculated here
float TileRawChannelBuilderQIEFilter::compute(int ros, int drawer, int channel, float &amplitude, float &time) {

	ATH_MSG_VERBOSE("compute();" << " ros=" << ros << " drawer=" << drawer << " channel=" << channel);

	//int i = 0, digits_size = m_digits.size();
	float chi2 = -999.; //the chi2 is not calculated by default
	float fraction_mean = 0;
	//float expected_fraction_RMS = 0;

	/*
	 double a[9];
	 double b[9];
	 double c[9];
	 double g[9];
	 double dg[9];
	 */

	amplitude = 0.;
	time = 0.; //TODO: Time needs to be implemented using the QIE TDC

	/*
	 float ofcPhase = (float) phase;

	 unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
	 const TileOfcWeightsStruct* m_weights;
	 if (m_ofcFromCool) {
	 m_weights = m_tileCondToolOfcCool->getOfcWeights(drawerIdx, channel, gain, ofcPhase, m_of2);
	 } else {
	 m_weights = m_tileCondToolOfc->getOfcWeights(drawerIdx, channel, gain, ofcPhase, m_of2);
	 }

	 for (i = 0; i < digits_size; ++i) {
	 a[i] = m_weights->w_a[i];
	 b[i] = m_weights->w_b[i];
	 g[i] = m_weights->g[i];
	 dg[i] = m_weights->dg[i];
	 if (m_of2)
	 c[i] = m_weights->w_c[i]; // [OptFilterPha+100];
	 }
	 */

	//Case 1: No pileup suppression
	// Here we assume that the digits are in fC
	// TODO; The code needs to be able to handle QIE counts instead of fC
	//amplitude = m_digits[3] + m_digits[4];
	//Case 2: with Pileup suppression
	//TODO: Timing information needs to be added
	//February 2016: The transfer constants are taken from the Oct 2015 testbeam results; they are not optimal:
	//0.16/(1-0.16) = 0.19
	//TODO: These numbers may be placed in a database
	const float avg_fraction = 0.18;
	const float Q_1pe = 17.; //charge per a photo-electron in fC
	//const float pileup_cutoff = 3.0; //Cut-off for the pileup subtractions

	//m_digits[3] charge in the central interaction
	//m_digits[4] charge in the following interaction;
	//it is expected to be ~5 times smaller than in the central interaction without the out-of-time pileup
	/*
	 if (m_digits[2] > 0) //check if there is pileup in the preceding BC.
	 amplitude = m_digits[3] - avg_fraction * m_digits[2] / (1. - avg_fraction); // suppress pileup from the previous BC.
	 else
	 amplitude = m_digits[3];

	 //Check if there is visible pileup in the next BC
	 //pileup only increases the measured charge

	 if (amplitude > 0) {
	 fraction_mean = m_digits[4] * (1. - avg_fraction) / amplitude;
	 expected_fraction_RMS = sqrt(avg_fraction * (1. - avg_fraction) * (1. - avg_fraction) * Q_1pe / amplitude);
	 chi2 = (fraction_mean - avg_fraction) / expected_fraction_RMS;

	 if (chi2 > pileup_cutoff) { //There may be pileup in the following BC
	 amplitude *= 1. / (1. - avg_fraction); // The pileup is high enough so we ignore the following interaction
	 } else { // The pileup is low enough so we include the following interaction charge as is
	 amplitude += m_digits[4];
	 }
	 }*/

	//Case 3: with pileup suppression
	double prob_cut_off = 0.005;

	if (m_digits[2] > 0) //check if there is pileup in the preceding BC.
		amplitude = m_digits[3] - avg_fraction * m_digits[2] / (1. - avg_fraction);

	if (amplitude < 0)
		amplitude = 0;

	if (m_digits[4] > 0) {
		amplitude += m_digits[4];
		fraction_mean = m_digits[4] / amplitude;

		if (fraction_mean > avg_fraction) { // Pileup is suspected in the 2nd integration window
			int n = (int) (amplitude / Q_1pe);
			int k = (int) (m_digits[4] / Q_1pe);

			//Here we calculate the binomial coefficient; n>=k
			// n!/(k! (n-k!))
			double binom_coeff = 1.;

			for (int j = n; j > 0; j--) {
				if (j > k)
					binom_coeff = binom_coeff * (double) j;

				if (j < (n - k + 1))
					binom_coeff = binom_coeff / (double) j;
			}

			//binomial probability
			double prob = binom_coeff * pow(avg_fraction, k) * pow(1. - avg_fraction, n - k);

			if (prob < prob_cut_off) { //likely there is too much pileup in the 2nd integration window.
				amplitude -= m_digits[4];
				amplitude *= 1. / (1. - avg_fraction);
			}
		}

	}

	bool goodEnergy = (fabs(amplitude) > 1.0e-04);
	if (goodEnergy) {
		if (msgLvl(MSG::VERBOSE)) {
			msg(MSG::VERBOSE) << "QIEFilterEne=" << amplitude << endmsg;
			msg(MSG::VERBOSE) << "QIEFilterTimee=" << time << endmsg;
		}
	} else {
		if (msgLvl(MSG::VERBOSE)) {
			msg(MSG::VERBOSE) << "QIEFilterEne=" << amplitude << "   ... assuming 0.0" << endmsg;
			msg(MSG::VERBOSE) << "QIEFilterTime=" << time << "   ... assuming 0.0" << endmsg;
		}
		//time = amplitude = 0.0;
	}

//  std::cout << "emulate " << m_emulatedsp << " OptFilterEne " << OptFilterEne << " OptFilterDigits[3]" << OptFilterDigits[3] << " OptFilterTime="<<OptFilterTime<<" OptFilterPed="<<OptFilterPed<<" OptFilterChi2="<<OptFilterChi2<<" g 3 " << g[3] << " dg 1 3 5 " << dg[1] << " " << dg[3] << " " << dg[5] <<std::endl;
	if (fabs(chi2) > 1.0e-04 || goodEnergy) {
		if (msgLvl(MSG::VERBOSE)) {
			msg(MSG::VERBOSE) << "QIEFilterTime=" << time << endmsg;
			msg(MSG::VERBOSE) << "QIEFilterChi2=" << chi2 << endmsg;
		}
	} else {
		if (msgLvl(MSG::VERBOSE)) {
			msg(MSG::VERBOSE) << "QIEFilterTime=" << time << endmsg;
			msg(MSG::VERBOSE) << "QIEFilterChi2=" << chi2 << "   ... assuming 0.0" << endmsg;
		}
		//chi2 = 0.0;
	}

	return chi2;
}


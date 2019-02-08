/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileDigitsFromPulse.cxx
//  Author   : Simon Molander
//  Created  : February 2013
//
//  DESCRIPTION
// 
//  Create TileDigits from simulated pulses. 
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Tile includes
#include "TileSimAlgs/TileDigitsFromPulse.h"
#include "TileEvent/TileDigits.h"
#include "TileEvent/TileMutableDigitsContainer.h"
#include "TileEvent/TileMutableRawChannelContainer.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileInfo.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

//Simulator includes
#include "TilePulseSimulator/TileSampleGenerator.h"
#include "TilePulseSimulator/TileSampleBuffer.h"
#include "TilePulseSimulator/TilePulseShape.h"

// Athena includes
#include "AthAllocators/DataPool.h"
#include "PathResolver/PathResolver.h"
//Random number service
#include "AthenaKernel/IAtRndmGenSvc.h"

#include <CLHEP/Random/Randomize.h>
#include <CLHEP/Units/SystemOfUnits.h>

//Root includes
#include "TRandom3.h"
#include "TFile.h"
#include "TH1F.h"
#include "TKey.h"
#include "TF1.h"

//C++ STL includes
#include <vector>


using CLHEP::RandGaussQ;
using CLHEP::RandFlat;

//
// Constructor
//
TileDigitsFromPulse::TileDigitsFromPulse(std::string name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_tileHWID(0),
  m_tileInfo(0),
  m_pHRengine(0),
  m_rndmSvc("AtRndmGenSvc", name)

{
	m_rChUnit = TileRawChannelUnit::ADCcounts;
	m_rChType = TileFragHash::Default;

	declareProperty("ImperfectionMean", m_imperfectionMean = 1.01);
	declareProperty("ImperfectionRms", m_imperfectionRms = 0.02);
	declareProperty("InTimeAmp", m_inTimeAmp = 300.);
	declareProperty("OutOfTimeAmp", m_ootAmp = 150.);
	declareProperty("InTimeOffset", m_itOffset = 0.);
	declareProperty("OutOfTimeOffset", m_ootOffset = 50.);
	declareProperty("OutOfTimeOffsetHistogramFile", m_ootOffsetFileName = "");
	declareProperty("OutOfTimeOffsetHistogramName", m_ootOffsetHistName = "");
	declareProperty("UseGaussNoise", m_gaussNoise = kFALSE);
	declareProperty("GaussNoiseAmpOne", m_GNAmpOne = 1 / 1.039);
	declareProperty("GaussNoiseSigmaOne", m_GNSigmaOne = 1.6);
	declareProperty("GaussNoiseAmpTwo", m_GNAmpTwo = 0.039);
	declareProperty("GaussNoiseSigmaTwo", m_GNSigmaTwo = 3.6);
	declareProperty("UseInTimeAmpDist", m_useItADist = kFALSE);
	declareProperty("UseOutOfTimeAmpDist", m_useOotADist = kFALSE);
	declareProperty("InTimeAmpDistFileName", m_itADistFileName = "");
	declareProperty("OutOfTimeAmpDistFileName", m_ootADistFileName = "");
	declareProperty("PileUpFraction", m_pileUpFraction = 1);
	declareProperty("GaussianC2CPhaseVariation", m_gausC2C = 0);
	declareProperty("ChannelSpecificPedestal", m_chanPed = kFALSE);
	declareProperty("ChannelSpecificNoise", m_chanNoise = kFALSE);
	declareProperty("AmpDistLowerLimit", m_AmpDistLowLim = 135);
	declareProperty("InTimeAmpDistHistogramName", m_itADistHistName = "h_Eopt_hi");
	declareProperty("OutOfTimeAmpDistHistogramName", m_ootADistHistName = "h_Eopt_hi");

	declareProperty("RandomSeed", m_seed = 4357);
	declareProperty("BunchSpacing", m_BunchSpacing = 25.); // 25, 50 or 75
	declareProperty("SimulateQIE", m_simQIE = kFALSE);

//   declareProperty("nSamples", nSamp = 7); //TBD
//   declareProperty("nPulses", nPul = 21);  //TBD

	//Number of samples and pileup, currently hard-coded
	m_nSamples = 7;
	m_nPul = 21;

	//Initialisations
	m_ps[0] = new TilePulseShape(msgSvc(), "TilePulseShapeLo"); //Low Gain
	m_ps[1] = new TilePulseShape(msgSvc(), "TilePulseShapeHi"); //High Gain

	m_buf = new TileSampleBuffer(m_nSamples, -75., 25.);
	m_tsg = new TileSampleGenerator(m_ps[0], m_buf, false); //Set third parameter to true for debug of the sum of pulses

	m_itFile = new TFile();
	m_itDist = new TH1F();
	m_ootFile = new TFile();
	m_ootDist = new TH1F();
	m_ootOffsetDist = new TH1F();
	m_ootOffsetFile = new TFile();

	m_useOffsetHisto = kFALSE;

	m_nPul_eff = (m_nPul - 1) / 2; //Used for symetrization of PU in computation

}

TileDigitsFromPulse::~TileDigitsFromPulse() {

	delete m_ootOffsetFile;
	delete m_ootFile;
	delete m_itFile;
	delete m_tsg;
	delete m_buf;
	delete m_ps[0];
	delete m_ps[1];
}

//
// Alg standard initialize function
//
StatusCode TileDigitsFromPulse::initialize() {
	ATH_MSG_DEBUG("in initialize()");

	ATH_CHECK(detStore()->retrieve(m_tileHWID, "TileHWID"));
	ATH_CHECK(detStore()->retrieve(m_tileInfo, "TileInfo"));

        ATH_CHECK(m_tileToolNoiseSample.retrieve());

        ATH_CHECK(m_digitsContainerKey.initialize());
        ATH_MSG_INFO("Output digits container: " <<  m_digitsContainerKey.key());

        ATH_CHECK(m_rawChannelContainerKey.initialize());
	ATH_MSG_INFO("Output raw channel container: " << m_rawChannelContainerKey.key());


	//Build pulse shapes
	m_ps[0]->setPulseShape(m_tileInfo->digitsFullShapeLo());
	m_ps[1]->setPulseShape(m_tileInfo->digitsFullShapeHi());

	//Initialise distribution histograms if in use
	if (m_useItADist) {
		if (m_itADistFileName.size() == 0) {
			m_itADistFileName = PathResolver::find_file("Distributions_small_h2000_177531_JetTauEtmiss.root", "DATAPATH");
			if (m_itADistFileName.size() == 0) {
				ATH_MSG_FATAL("Could not find input file Distributions_small_h2000_177531_JetTauEtmiss.root");
				return StatusCode::FAILURE;
			}
		}
		if (makeDist(m_itFile, m_itDist, m_itADistFileName, m_itADistHistName) == kFALSE)
			return StatusCode::FAILURE;
		ATH_MSG_DEBUG("Made in-time distribution");
	} else
		delete m_itDist;
	if (m_useOotADist) {
		if (m_ootADistFileName.size() == 0) {
			m_ootADistFileName = PathResolver::find_file("Distributions_small_h2000_177531_ZeroBias.root", "DATAPATH");
			if (m_ootADistFileName.size() == 0) {
				ATH_MSG_FATAL("Could not find input file Distributions_small_h2000_177531_ZeroBias.root");
				return StatusCode::FAILURE;
			}
		}
		if (makeDist(m_ootFile, m_ootDist, m_ootADistFileName, m_ootADistHistName) == kFALSE)
			return StatusCode::FAILURE;
		ATH_MSG_DEBUG("Made Oot distribution");
	} else
		delete m_ootDist;

	//Initialise timing offset distribution. If filename is empty, use static offset
	if (m_ootOffsetFileName.size() != 0) {
		m_ootOffsetFile = TFile::Open(m_ootOffsetFileName.c_str());
		if (m_ootOffsetFile->IsZombie()) {
			ATH_MSG_WARNING("Error reading offset timing distribution from " << m_ootOffsetFileName << ". Using static timing offset.");
		} else {
			TKey *key = m_ootOffsetFile->FindKey(m_ootOffsetHistName.c_str());
			if (key == 0) {
				ATH_MSG_WARNING("Histogram " << m_ootOffsetHistName << " not found in file " << m_ootOffsetFileName << ". Using static timing offset.");
			} else {
				m_ootOffsetDist = (TH1F*) m_ootOffsetFile->Get(m_ootOffsetHistName.c_str());
				m_useOffsetHisto = kTRUE;
			}
		}
	}

	//Start the random number service used to create channel specific noise
	if (!m_rndmSvc.retrieve().isSuccess()) {
		ATH_MSG_FATAL("Could not initialize find Random Number Service.");
		return StatusCode::FAILURE;
	} else {
		m_pHRengine = m_rndmSvc->GetEngine("Tile_DigitsMaker");
	}
	if (m_chanNoise)
		m_gaussNoise = kFALSE; //Make sure channel noise overrides gaussian noise.

	ATH_MSG_DEBUG("initialize() successful");

	return StatusCode::SUCCESS;
}
/*==========================================================================*/
//
// Begin Execution Phase.
//
StatusCode TileDigitsFromPulse::execute() {

	ATH_MSG_DEBUG("in execute()");

	// Create new container for digits
	auto digitsContainer = std::make_unique<TileMutableDigitsContainer>(true,
                                                                            TileFragHash::Digitizer,
                                                                            TileRawChannelUnit::ADCcounts,
                                                                            SG::VIEW_ELEMENTS);

        ATH_CHECK( digitsContainer->status() );

	//Create RawChannel for truth values.
	auto rawChannelContainer = std::make_unique<TileMutableRawChannelContainer>(true, m_rChType, m_rChUnit);
        ATH_CHECK( rawChannelContainer->status() );

	DataPool < TileDigits > tileDigitsPool(m_tileHWID->adc_hash_max());

	TRandom3 *random = new TRandom3(m_seed); //Randomizer for pulse-shape imperfection
	double tFit = 0, ped = 50.; //Settings for simulation

	TF1 *pdf = new TF1();
	if (!m_simQIE) {
		//Noise pdf for general noise. Maybe use as a member and put in init.
		//pdf = new TF1("pdf", "[0] * (Gaus(x,0,[1]) + [2] * Gaus(x,0,[3]))", -100., 100.); //Root goes not like "Gaus"
		pdf = new TF1("pdf", "[0] * (exp(-0.5*(x/[1])**2)/(sqrt(2*pi)*[1]) + [2] *exp(-0.5*(x/[3])**2)/(sqrt(2*pi)*[3]))", -100., 100.);
		pdf->SetParameters(m_GNAmpOne, m_GNSigmaOne, m_GNAmpTwo, m_GNSigmaTwo);
	}

	std::vector<float> samples(m_nSamples);

	double Rndm[16]; // Can't use variable size array,
	double Rndm_dG[1]; // uniform random number for the double gaussian

	ATH_MSG_DEBUG("Starting loop");
	int gain = 1;
	double n_inTimeAmp = 0.0; //!< Local loop variable for amplitude of in-time pulse

	for (int ros = 1; ros < 5; ++ros) {
		for (int drawer = 0; drawer < 64; ++drawer) {
			unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
			for (int channel = 0; channel < 48; ++channel) {

				if (!m_simQIE) { //3-in-1 is simulated below

					ATH_MSG_DEBUG("executing 3-in-1 code");
					bool isHGSaturated = false;

					for (int igain = 1; igain >= 0; igain--) {
						gain = igain;
						if (gain == 1) {
							n_inTimeAmp = m_useItADist ? m_itDist->GetRandom() : m_inTimeAmp;
							if (m_chanPed)
								ped = m_tileToolNoiseSample->getPed(drawerIdx, channel, gain);
							if (random->Rndm() >= m_pileUpFraction)
								m_ootAmp = 0; //Set oot amplitude to 0 if no pile-up.
							tFit = random->Gaus(0., m_gausC2C); //C2C phase variation
							double deformatedTime = random->Gaus(m_imperfectionMean, m_imperfectionRms); //Widening of pulseshape
							m_ps[gain]->scalePulse(deformatedTime, deformatedTime); // Deformation of pulse shape by changing its width
							//if(m_useOffsetHisto) m_ootOffset = m_ootOffsetDist->GetRandom();  //OLD Remove for 7 samples -> BunchSpacing

							//Pileup samples
							m_PUAmp.clear();
							m_PUAmp.resize(m_nPul);

							for (int i = 0; i <= m_nPul_eff; i++) {
								if (((i * 25) % m_BunchSpacing) == 0) {
									m_PUAmp.at(m_nPul_eff + i) = m_useOotADist ? m_ootDist->GetRandom() : m_ootAmp;
									m_PUAmp.at(m_nPul_eff - i) = m_useOotADist ? m_ootDist->GetRandom() : m_ootAmp;

								} else {
									m_PUAmp.at(m_nPul_eff + i) = 0;
									m_PUAmp.at(m_nPul_eff - i) = 0;
								}
							}
						} else {
							if (m_chanPed)
								ped = m_tileToolNoiseSample->getPed(drawerIdx, channel, gain);
							double deformatedTime = random->Gaus(m_imperfectionMean, m_imperfectionRms); //Widening of pulseshape
							m_ps[gain]->scalePulse(deformatedTime, deformatedTime); // Deformation of pulse shape by changing its width
							if (m_chanPed)
								ped = m_tileToolNoiseSample->getPed(drawerIdx, channel, gain);

							n_inTimeAmp /= 64;
							for (int i = 0; i <= m_nPul_eff; i++) {
								m_PUAmp.at(m_nPul_eff + i) /= 64;
								m_PUAmp.at(m_nPul_eff - i) /= 64;
							}
						}

						m_tsg->setPulseShape(m_ps[gain]);
						m_tsg->fill7Samples(tFit, ped, n_inTimeAmp, m_PUAmp, pdf, m_gaussNoise, m_itOffset); // Sum of Intime + PU pulses
						//m_tsg->fillSamples(tFit,ped,n_inTimeAmp,m_ootAmp,pdf,m_gaussNoise, m_itOffset, m_ootOffset);   //OLD Generation of pulse

						samples.clear();
						samples.resize(m_nSamples);
						m_buf->getValueVector(samples);

						if (m_chanNoise) {
							double Hfn1 = m_tileToolNoiseSample->getHfn1(drawerIdx, channel, gain);
							double Hfn2 = m_tileToolNoiseSample->getHfn2(drawerIdx, channel, gain);
							double Norm = m_tileToolNoiseSample->getHfnNorm(drawerIdx, channel, gain);
							RandGaussQ::shootArray(m_pHRengine, samples.size(), Rndm, 0.0, 1.0);
							RandFlat::shootArray(m_pHRengine, 1, Rndm_dG, 0.0, 1.0);
							for (unsigned int js = 0; js < samples.size(); ++js) {
								//using the same gaussian(sigma) for all samples in one channel in one event
								if (Rndm_dG[0] < Norm)
									samples[js] += (float) Hfn1 * Rndm[js];
								else
									samples[js] += (float) Hfn2 * Rndm[js];
							}
						}

						for (unsigned int i = 0; i < samples.size(); ++i) {
							if (samples[i] >= 1023)
								isHGSaturated = true;
						}
						if (!isHGSaturated)
							break;

					}

				} else { //QIE is simulated here --------------------------------------------

					//ATH_MSG_DEBUG("executing QIE code");

					gain = 1; //This is just a place holder. The gain is not used in QIE.
					n_inTimeAmp = m_useItADist ? m_itDist->GetRandom() : m_inTimeAmp;
					//if (random->Rndm() >= m_pileUpFraction) //m_pileUpFraction is 1 by default
					m_ootAmp = 0; //Set oot amplitude to 0 if no pile-up.
					tFit = 0; //TODO: Introduce jitter of the PMT pulse; random->Gaus(0., m_gausC2C); //C2C phase variation

					//Pileup samples
					//m_PUAmp.clear();
					//m_PUAmp.resize(nPul);
					float my_PUAmp[7]; //I use an array to store the energies/charges of the out-of-time pulses

					for (int i = 0; i < 7; i++)
						if ((((i - 3) * 25) % (int) m_BunchSpacing) == 0) {
							if (i != 3) { //index 3 corresponds to the in-time pulse, the signal
								//m_PUAmp.at(i) = m_useOotADist ? m_ootDist->GetRandom() : m_ootAmp; //out-of-time pulses
								my_PUAmp[i] = m_useOotADist ? m_ootDist->GetRandom() : m_ootAmp; //out-of-time pulses
							} else {
								//m_PUAmp.at(i) = 0; //it-time pulse
								my_PUAmp[i] = 0;
							}
						}

					//fill7SamplesQIE(float t0, float amp_it, float *amp_pu, bool addNoise);
					m_tsg->fill7SamplesQIE((float) n_inTimeAmp, my_PUAmp); // Sum of In time + out-of-time PU pulses

					samples.clear();
					samples.resize(m_nSamples);
					m_buf->getValueVector(samples);

				}

				ATH_MSG_VERBOSE("New ADC " << ros << "/" << drawer << "/" << channel << "/   saving gain  " << gain);

				TileDigits * digit = tileDigitsPool.nextElementPtr();
                                *digit = TileDigits (m_tileHWID->adc_id(ros, drawer, channel, gain),
                                                     std::move(samples));

				ATH_CHECK( digitsContainer->push_back(digit) );

				auto rawChannel = std::make_unique<TileRawChannel>(digit->adc_HWID(),
                                                                                   n_inTimeAmp,
                                                                                   tFit,
                                                                                   m_ootAmp,
                                                                                   m_ootOffset);

				ATH_CHECK( rawChannelContainer->push_back(std::move(rawChannel)) );
			}
		}
	}

        SG::WriteHandle<TileRawChannelContainer> rawChannelCnt(m_rawChannelContainerKey);
        ATH_CHECK( rawChannelCnt.record(std::move(rawChannelContainer)) );

        SG::WriteHandle<TileDigitsContainer> digitsCnt(m_digitsContainerKey);
        ATH_CHECK( digitsCnt.record(std::move(digitsContainer)) );

	if (!m_simQIE) {
		delete pdf;
	}
	delete random;

	ATH_MSG_DEBUG("Execution completed");

	return StatusCode::SUCCESS;
}

StatusCode TileDigitsFromPulse::finalize() {
	ATH_MSG_DEBUG("in finalize()");
	if (m_useItADist)
		m_itFile->Close();
	if (m_useOotADist)
		m_ootFile->Close();

	return StatusCode::SUCCESS;
}

bool TileDigitsFromPulse::makeDist(TFile*& file, TH1F*& hist, std::string fileName, std::string histName) {
	file = new TFile(fileName.c_str());
	if (file->IsZombie()) {
		ATH_MSG_FATAL("Error reading amplitude distribution from " << fileName << ".");
		return kFALSE;
	}
	TKey *key = file->FindKey(histName.c_str());
	if (key == 0) {
		ATH_MSG_FATAL("Could not find histogram " << histName << " in file " << fileName << ".");
		return kFALSE;
	}
	hist = (TH1F*) file->Get(histName.c_str());
	for (int i = 0; i < m_AmpDistLowLim; i++)
		hist->SetBinContent(i, 0.); // Puts a cut on the amplitude distribution.
	return kTRUE;

}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

// small hack to enable datapool usage
#define private public
#define protected public
#include "TileEvent/TileDigits.h"
#undef private
#undef protected

#include "DataModel/DataPool.h"
#include "PathResolver/PathResolver.h"

// Tile includes
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileInfo.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileSimAlgs/TileDigitsFromPulse.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

//C++ STL includes
#include <vector>

//Simulator includes
#include "TilePulseSimulator/TileSampleGenerator.h"
#include "TilePulseSimulator/TileSampleBuffer.h"
#include "TilePulseSimulator/TilePulseShape.h"

//Root includes
#include "TRandom3.h"
#include "TFile.h"
#include "TH1F.h"
#include "TKey.h"

//Random number service
#include "AthenaKernel/IAtRndmGenSvc.h"
#include <CLHEP/Random/Randomize.h>
#include <CLHEP/Units/SystemOfUnits.h>

using CLHEP::RandGaussQ;
using CLHEP::RandFlat;

//
// Constructor
//
TileDigitsFromPulse::TileDigitsFromPulse(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileHWID(0)
  , m_tileInfo(0)
  , m_pHRengine(0)
  , m_rndmSvc ("AtRndmGenSvc", name)

{
  m_rChUnit = TileRawChannelUnit::ADCcounts;
  m_rChType = TileFragHash::Default;
  
  declareProperty("OutputContainer", m_outputContainer = "TileDigitsCnt");
  declareProperty("ImperfectionMean", m_imperfectionMean = 1.01);
  declareProperty("ImperfectionRms", m_imperfectionRms = 0.02);
  declareProperty("InTimeAmp", m_inTimeAmp = 300.);
  declareProperty("OutOfTimeAmp", m_ootAmp = 150.);
  declareProperty("InTimeOffset", m_itOffset = 0.);
  declareProperty("OutOfTimeOffset", m_ootOffset = 50.);
  declareProperty("OutOfTimeOffsetHistogramFile", m_ootOffsetFileName = "");
  declareProperty("OutOfTimeOffsetHistogramName", m_ootOffsetHistName = "");
  declareProperty("UseGaussNoise", m_gaussNoise = kFALSE);
  declareProperty("GaussNoiseAmpOne", m_GNAmpOne = 1/1.039);
  declareProperty("GaussNoiseSigmaOne", m_GNSigmaOne = 1.6);  
  declareProperty("GaussNoiseAmpOne", m_GNAmpTwo = 0.039);
  declareProperty("GaussNoiseSigmaOne", m_GNSigmaTwo = 3.6);    
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

  double nSamp = 7; // number of time slices for each chan
  
  
  //Initialisations 
  m_ps[0] = new TilePulseShape(msgSvc(),"TilePulseShapeLo");
  m_ps[1] = new TilePulseShape(msgSvc(),"TilePulseShapeHi");
  m_buf = new TileSampleBuffer(nSamp, -75., 25.);
  m_tsg = new TileSampleGenerator(m_ps[0], m_buf);
  
  m_itFile = new TFile();
  m_itDist = new TH1F();
  m_ootFile = new TFile();
  m_ootDist = new TH1F();
  m_ootOffsetDist = new TH1F(); 
  m_ootOffsetFile = new TFile();
  
  m_useOffsetHisto = kFALSE;

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
StatusCode TileDigitsFromPulse::initialize() 
{ 
  ATH_MSG_DEBUG("in initialize()");

  CHECK(detStore()->retrieve(m_tileHWID, "TileHWID"));
  CHECK(detStore()->retrieve(m_tileInfo, "TileInfo"));

  ATH_MSG_INFO("output container: " << m_outputContainer);
  
  //Build pulse shapes
  m_ps[0]->setPulseShape(m_tileInfo->digitsFullShapeLo());
  m_ps[1]->setPulseShape(m_tileInfo->digitsFullShapeHi());
  
  
  //Initialise distribution histograms if in use
  if(m_useItADist) {
    if(m_itADistFileName.size() == 0) {
        m_itADistFileName = PathResolver::find_file ("Distributions_small_h2000_177531_JetTauEtmiss.root", "DATAPATH");
        if (m_itADistFileName.size() == 0) {
            ATH_MSG_FATAL ("Could not find input file Distributions_small_h2000_177531_JetTauEtmiss.root");
            return StatusCode::FAILURE;
        }   
    } 
    if (makeDist(m_itFile, m_itDist, m_itADistFileName, m_itADistHistName) == kFALSE) return StatusCode::FAILURE;
    ATH_MSG_DEBUG ("Made in-time distribution");
  }
  else delete m_itDist;
  if(m_useOotADist) {
    if(m_ootADistFileName.size() == 0) {
        m_ootADistFileName = PathResolver::find_file ("Distributions_small_h2000_177531_ZeroBias.root", "DATAPATH");
        if (m_ootADistFileName.size() == 0) {
            ATH_MSG_FATAL ("Could not find input file Distributions_small_h2000_177531_ZeroBias.root");
            return StatusCode::FAILURE;
        }   
    }
    if (makeDist(m_ootFile, m_ootDist, m_ootADistFileName, m_ootADistHistName) == kFALSE) return StatusCode::FAILURE;
    ATH_MSG_DEBUG ("Made Oot distribution");
  }
  else delete m_ootDist;
  
  //Initialise timing offset distribution. If filename is empty, use static offset
  if(m_ootOffsetFileName.size() != 0) {
    m_ootOffsetFile = TFile::Open(m_ootOffsetFileName.c_str());
    if (m_ootOffsetFile->IsZombie()) {
      ATH_MSG_WARNING("Error reading offset timing distribution from " << m_ootOffsetFileName << ". Using static timing offset.");
    } else {
      TKey *key = m_ootOffsetFile->FindKey(m_ootOffsetHistName.c_str());
      if (key ==0) {
        ATH_MSG_WARNING("Histogram " << m_ootOffsetHistName << " not found in file " << m_ootOffsetFileName << ". Using static timing offset.");
      } else {
        m_ootOffsetDist = (TH1F*) m_ootOffsetFile->Get(m_ootOffsetHistName.c_str());
        m_useOffsetHisto = kTRUE;
      }
    }
  }
  
    
  //Start the random number service used to create channel specific noise
  if (!m_rndmSvc.retrieve().isSuccess()) {
    ATH_MSG_FATAL ("Could not initialize find Random Number Service.");
    return StatusCode::FAILURE;
  } else {
    m_pHRengine = m_rndmSvc->GetEngine("Tile_DigitsMaker");
  }
  if (m_chanNoise) m_gaussNoise = kFALSE; //Make sure channel noise overrides gaussian noise.
  
  ATH_MSG_DEBUG ("initialize() successful");
  

  return StatusCode::SUCCESS;
}
/*==========================================================================*/
//
// Begin Execution Phase.
//
StatusCode TileDigitsFromPulse::execute() {

  ATH_MSG_DEBUG ("in execute()");

  // Create new container for digits
  TileDigitsContainer* outputCont = new TileDigitsContainer(true,SG::VIEW_ELEMENTS);
  if(!outputCont) {
    ATH_MSG_FATAL ("Could not create a new TileDigitsContainer");
    return StatusCode::FAILURE;
  }
  
  //Create RawChannel for truth values.
  TileRawChannel * pRawChannel;
  TileRawChannelContainer * pRawChannelContainer;
  pRawChannelContainer=new TileRawChannelContainer(true, m_rChType, m_rChUnit);

  static DataPool<TileDigits> tileDigitsPool(m_tileHWID->adc_hash_max());
  static bool first=true;
  
  TRandom3 *random = new TRandom3; //Randomizer for pulshe-shape imperfection
  double tFit = 0, ped = 50.; //Settings for simulation 
  TF1 *pdf = new TF1("pdf","[0] * (Gaus(x,0,[1]) + [2] * Gaus(x,0,[3]))",-100.,100.); //Noise pdf for general noise. Maybe use as a member and put in init.
  pdf->SetParameters(m_GNAmpOne, m_GNSigmaOne, m_GNAmpTwo, m_GNSigmaTwo);
  
  std::vector<float> samples(7);
  
  double Rndm[16];       // Can't use variable size array,
  double Rndm_dG[1];    // uniform random number for the double gaussian
  
  ATH_MSG_DEBUG ("Starting loop");
  
  for (int gain=0; gain<2; ++gain) {
    m_tsg->setPulseShape(m_ps[gain]);
    for (int ros=1; ros<5; ++ros) {
      for (int drawer=0; drawer<64; ++drawer) {
        unsigned int drawerIdx =  TileCalibUtils::getDrawerIdx(ros, drawer);
        for (int channel=0; channel<48; ++channel) {
          ATH_MSG_VERBOSE ("New ADC " << ros << "/" << drawer << "/" << channel << "/" << gain);
          if(m_useItADist) m_inTimeAmp = m_itDist->GetRandom();
          if(m_useOotADist) m_ootAmp = m_ootDist->GetRandom();
          if(m_chanPed) ped = m_tileToolNoiseSample->getPed(drawerIdx,channel,gain); 
          if(random->Rndm() >= m_pileUpFraction) m_ootAmp = 0; //Set oot amplitude to 0 if no pile-up.
          tFit = random->Gaus(0.,m_gausC2C); //C2C phase variation
          double deformatedTime = random->Gaus(m_imperfectionMean,m_imperfectionRms); //Widening of pulseshape
          m_ps[gain]->scalePulse(deformatedTime, deformatedTime);             // Deformation of pulse shape by changing its width
          if(m_useOffsetHisto) m_ootOffset = m_ootOffsetDist->GetRandom();
          m_tsg->fillSamples(tFit,ped,m_inTimeAmp,m_ootAmp,pdf,m_gaussNoise, m_itOffset, m_ootOffset);   // Generation of pulse
          m_buf->getValueVector(samples);
          if(m_chanNoise) {
            double Hfn1 = m_tileToolNoiseSample->getHfn1(drawerIdx,channel,gain);
            double Hfn2 = m_tileToolNoiseSample->getHfn2(drawerIdx,channel,gain);
            double Norm = m_tileToolNoiseSample->getHfnNorm(drawerIdx,channel,gain);
          	RandGaussQ::shootArray(m_pHRengine,samples.size(),Rndm,0.0, 1.0);
            RandFlat::shootArray(m_pHRengine,1,Rndm_dG,0.0, 1.0);
            for(unsigned int js=0; js<samples.size(); ++js) {
              //using the same gaussian(sigma) for all samples in one channel in one event
              if (Rndm_dG[0]<Norm)
                samples[js] += (float) Hfn1*Rndm[js];
              else
                samples[js] += (float) Hfn2*Rndm[js];
            }                                     
          }
          TileDigits * digit = tileDigitsPool.nextElementPtr();
          if (first) digit->m_adc_hwid = m_tileHWID->adc_id(ros,drawer,channel,gain); // needed only once
          digit->m_digits = samples;
          outputCont->push_back(digit);
          
          pRawChannel = new TileRawChannel(digit->adc_HWID(),m_inTimeAmp,tFit,m_ootAmp,m_ootOffset);
          pRawChannelContainer->push_back(pRawChannel);
        }
      }
    }
  }
  CHECK(evtStore()->record(pRawChannelContainer, "TrueAmp", false));
  first = false;
  
  if (m_outputContainer.size() > 0) {

    CHECK(evtStore()->record(outputCont, m_outputContainer, false));

  } else if (outputCont) {

    ATH_MSG_DEBUG ("Deleting filtered digits container");
    delete outputCont;
  }

  ATH_MSG_DEBUG ("Execution completed");

  return StatusCode::SUCCESS;
}

StatusCode TileDigitsFromPulse::finalize()
{
  ATH_MSG_DEBUG ("in finalize()");
  if (m_useItADist) m_itFile->Close();
  if (m_useOotADist) m_ootFile->Close();

  return StatusCode::SUCCESS;
}

bool TileDigitsFromPulse::makeDist(TFile*& file, TH1F*& hist, std::string fileName, std::string histName)
{
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
  for(int i = 0; i < m_AmpDistLowLim; i++) hist->SetBinContent(i,0.);   // Puts a cut on the amplitude distribution.
  return kTRUE;
 
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTElectronicsProcessing.h"

#include "TRTElectronicsNoise.h"
#include "TRTDigit.h"

#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"

#include "TRTDigSettings.h"

#include <iostream>
#include <limits>

//___________________________________________________________________________
TRTElectronicsProcessing::TRTElectronicsProcessing( const TRTDigSettings* digset,
						    ServiceHandle <IAtRndmGenSvc> atRndmGenSvc,
						    TRTElectronicsNoise * electronicsnoise )
  : m_settings(digset),
    m_pElectronicsNoise(electronicsnoise),
    m_msg("TRTElectronicsProcessing")
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) <<"TRTElectronicsProcessing::Constructor begin" << endmsg;
  m_pHRengine  = atRndmGenSvc->GetEngine("TRT_ThresholdFluctuations");
  Initialize();
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) <<"TRTElectronicsProcessing::Constructor done" << endmsg;
}

//___________________________________________________________________________
TRTElectronicsProcessing::~TRTElectronicsProcessing() {

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TRTElectronicsProcessing::Destructor begin" << endmsg;

  delete [] m_energyDistribution;
  delete [] m_lowThresholdDiscriminator;
  delete [] m_highThresholdDiscriminator;

  delete m_pElectronicsNoise;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TRTElectronicsProcessing::Destructor done" << endmsg;
}

//___________________________________________________________________________
void TRTElectronicsProcessing::Initialize() {

  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "TRTElectronicsProcessing::Initialize() begin" << endmsg;

  const int numberOfBins(static_cast<int>(m_settings->numberOfBins())); //returns unsigned int
  m_numberOfPostZeroBins = numberOfBins; //assigning to int
  m_timeInterval = m_settings->timeInterval(); // returns double
  m_binWidth = m_timeInterval / static_cast<double>(numberOfBins); //assigning to double
  const int numberOfCrossingsBeforeMain(m_settings->numberOfCrossingsBeforeMain()); // returns unsigned int
  if (m_settings->timeCorrection()) {
    m_numberOfPreZeroBins = numberOfBins * numberOfCrossingsBeforeMain / 3; //integer division... assigning to int
  } else {
    m_numberOfPreZeroBins = 0; // occurs when beamType='cosmics'
  }
  m_totalNumberOfBins = m_numberOfPreZeroBins + m_numberOfPostZeroBins; //assigning to int
  m_timeInterval += m_binWidth * static_cast<double>(m_numberOfPreZeroBins); //assigning to double

  //TK: why dont we use these? [fixme?]
  //AJB: Yes, what a mess!
  //  int numberOfBinsInEncodingBin = m_settings->numberOfBinsInEncodingBin();
  //  m_encodingBinWidth = m_binWidth * (double) numberOfBinsInEncodingBin;
  m_minDiscriminatorWidthInBinWidths =     static_cast<int>(m_settings->minDiscriminatorWidth()     / m_binWidth + 0.5);
  m_discriminatorSettlingTimeInBinWidths = static_cast<int>(m_settings->discriminatorSettlingTime() / m_binWidth + 0.5);
  m_discriminatorDeadTimeInBinWidths =     static_cast<int>(m_settings->discriminatorDeadTime()     / m_binWidth + 0.5);

  m_minWidthMinusSettlingTimeInBinWidths = m_minDiscriminatorWidthInBinWidths - m_discriminatorSettlingTimeInBinWidths;

  // low threshold settings for Xe, Kr and Ar
  m_lowThresholdBar[0] = m_settings->lowThresholdBar(0);
  m_lowThresholdBar[1] = m_settings->lowThresholdBar(1);
  m_lowThresholdBar[2] = m_settings->lowThresholdBar(2);
  m_lowThresholdEC[0]  = m_settings->lowThresholdEC(0);
  m_lowThresholdEC[1]  = m_settings->lowThresholdEC(1);
  m_lowThresholdEC[2]  = m_settings->lowThresholdEC(2);

  TabulateSignalShape();

  //for (int j=0; j<3; j++) { for (int i=0; i<m_numberOfPostZeroBins; i++) std::cout << "AJBLT " << j << " " << m_lowThresholdSignalShape[j][i] << std::endl; }
  //for (int j=0; j<3; j++) { for (int i=0; i<m_numberOfPostZeroBins; i++) std::cout << "AJBHT " << j << " " << m_highThresholdSignalShape[j][i] << std::endl; }

  m_energyDistribution = new double[m_totalNumberOfBins];
  m_lowThresholdSignal.reserve(m_totalNumberOfBins);
  m_lowThresholdSignal.resize(m_totalNumberOfBins, 0.0);
  m_highThresholdSignal.reserve(m_totalNumberOfBins);
  m_highThresholdSignal.resize(m_totalNumberOfBins, 0.0);
  m_lowThresholdDiscriminator  = new int[m_totalNumberOfBins];
  m_highThresholdDiscriminator = new int[m_totalNumberOfBins];

  // m_maskA  = 0x03FC0000;
  // m_maskB  = 0x0001FE00;
  // m_maskC  = 0x000000FF;
  // m_maskHT = 0x04020100;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TRTElectronicsProcessing::Initialize() done" << endmsg;
}

//___________________________________________________________________________
void TRTElectronicsProcessing::TabulateSignalShape() {

  // Fixme: This assumes 0.78125 ns bin spacing and m_numberOfPostZeroBins=160 bins!
  // So we need to hard-code the processing to reflect that this is not a parameter!

  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "TRTElectronicsProcessing::TabulateSignalShape() begin" << endmsg;

  // These arrays are cut and paste from the output of TRT_Digitization/share/signalShapes.cpp

  const double pXeLT[160] = {
    0.039062, 0.117617, 0.197695, 0.296573, 0.419494, 0.557328, 0.698813, 0.826937, 0.927195, 0.984970,
    0.994847, 0.956566, 0.877094, 0.767477, 0.641157, 0.510111, 0.385189, 0.273973, 0.180531, 0.105891,
    0.049175, 0.007925, -0.021127, -0.040969, -0.054005, -0.062364, -0.067278, -0.069242, -0.068776, -0.066547,
    -0.062738, -0.057634, -0.051691, -0.045232, -0.038621, -0.032235, -0.026218, -0.020790, -0.016104, -0.012117,
    -0.008966, -0.006491, -0.004585, -0.003105, -0.002121, -0.001400, -0.000890, -0.000544, -0.000332, -0.000202,
    -0.000084, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
  };

  const double pXeHT[160] = { // widened for middle-bit HT fraction tuning
    0.027700, 0.083200, 0.138700, 0.191900, 0.259700, 0.340300, 0.432000, 0.531200, 0.632900, 0.731400,
    0.820500, 0.894900, 0.950600, 0.985600, 1.000000, 0.995700, 0.975800, 0.943900, 0.903700, 0.857900,
    0.808500, 0.756500, 0.702400, 0.646200, 0.588000, 0.527900, 0.466600, 0.405000, 0.344100, 0.285200,
    0.229400, 0.177700, 0.131000, 0.089700, 0.054000, 0.024000, -0.000800, -0.020600, -0.036000, -0.047600,
    -0.055800, -0.061400, -0.064700, -0.066200, -0.066200, -0.065200, -0.063300, -0.060700, -0.057700, -0.054400,
    -0.050800, -0.047200, -0.043500, -0.039800, -0.036200, -0.032800, -0.029400, -0.026300, -0.023300, -0.020600,
    -0.018000, -0.015700, -0.013600, -0.011700, -0.010000, -0.008500, -0.007200, -0.006000, -0.005000, -0.004200,
    -0.003400, -0.002800, -0.002300, -0.001900, -0.001500, -0.001200, -0.001000, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
  };

  const double pKrLT[160] = {
    0.061537, 0.182727, 0.297239, 0.412757, 0.538228, 0.663099, 0.776969, 0.875428, 0.944050, 0.984161,
    0.993044, 0.966288, 0.913313, 0.856057, 0.794949, 0.730736, 0.666524, 0.602311, 0.536858, 0.471147,
    0.406935, 0.342722, 0.286429, 0.232919, 0.180758, 0.134739, 0.093536, 0.054859, 0.021586, -0.003114,
    -0.020773, -0.032170, -0.040197, -0.046147, -0.050428, -0.052697, -0.054131, -0.054672, -0.054255, -0.052971,
    -0.051862, -0.050965, -0.050133, -0.049063, -0.047992, -0.046922, -0.045852, -0.044782, -0.043711, -0.042365,
    -0.040850, -0.039084, -0.036943, -0.034803, -0.032809, -0.030865, -0.028922, -0.026979, -0.025035, -0.023092,
    -0.021149, -0.019206, -0.017262, -0.015759, -0.014533, -0.013345, -0.011740, -0.010638, -0.009568, -0.008497,
    -0.007427, -0.006357, -0.005287, -0.004217, -0.003146, -0.002076, -0.001006, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
  };

  const double pKrHT[160] = {
    0.027700, 0.083200, 0.138700, 0.191900, 0.259700, 0.340300, 0.432000, 0.531200, 0.632900, 0.731400,
    0.820500, 0.894900, 0.950600, 0.985600, 1.000000, 0.995700, 0.975800, 0.943900, 0.903700, 0.857900,
    0.808500, 0.756500, 0.702400, 0.646200, 0.588000, 0.527900, 0.466600, 0.405000, 0.344100, 0.285200,
    0.229400, 0.177700, 0.131000, 0.089700, 0.054000, 0.024000, -0.000800, -0.020600, -0.036000, -0.047600,
    -0.055800, -0.061400, -0.064700, -0.066200, -0.066200, -0.065200, -0.063300, -0.060700, -0.057700, -0.054400,
    -0.050800, -0.047200, -0.043500, -0.039800, -0.036200, -0.032800, -0.029400, -0.026300, -0.023300, -0.020600,
    -0.018000, -0.015700, -0.013600, -0.011700, -0.010000, -0.008500, -0.007200, -0.006000, -0.005000, -0.004200,
    -0.003400, -0.002800, -0.002300, -0.001900, -0.001500, -0.001200, -0.001000, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
  };

  const double pArLT[160] = {
    0.061537, 0.182727, 0.297239, 0.412757, 0.538228, 0.663099, 0.776969, 0.875428, 0.944050, 0.984161,
    0.993044, 0.966288, 0.913313, 0.856057, 0.794949, 0.730736, 0.666524, 0.602311, 0.536858, 0.471147,
    0.406935, 0.342722, 0.286429, 0.232919, 0.180758, 0.134739, 0.093536, 0.054859, 0.021586, -0.003114,
    -0.020773, -0.032170, -0.040197, -0.046147, -0.050428, -0.052697, -0.054131, -0.054672, -0.054255, -0.052971,
    -0.051862, -0.050965, -0.050133, -0.049063, -0.047992, -0.046922, -0.045852, -0.044782, -0.043711, -0.042365,
    -0.040850, -0.039084, -0.036943, -0.034803, -0.032809, -0.030865, -0.028922, -0.026979, -0.025035, -0.023092,
    -0.021149, -0.019206, -0.017262, -0.015759, -0.014533, -0.013345, -0.011740, -0.010638, -0.009568, -0.008497,
    -0.007427, -0.006357, -0.005287, -0.004217, -0.003146, -0.002076, -0.001006, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
  };

  const double pArHT[160] = { // widened for middle-bit HT fraction tuning
    0.027700, 0.083200, 0.138700, 0.191900, 0.259700, 0.340300, 0.432000, 0.531200, 0.632900, 0.731400,
    0.820500, 0.894900, 0.950600, 0.985600, 1.000000, 0.995700, 0.975800, 0.943900, 0.903700, 0.857900,
    0.808500, 0.756500, 0.702400, 0.646200, 0.588000, 0.527900, 0.466600, 0.405000, 0.344100, 0.285200,
    0.229400, 0.177700, 0.131000, 0.089700, 0.054000, 0.024000, -0.000800, -0.020600, -0.036000, -0.047600,
    -0.055800, -0.061400, -0.064700, -0.066200, -0.066200, -0.065200, -0.063300, -0.060700, -0.057700, -0.054400,
    -0.050800, -0.047200, -0.043500, -0.039800, -0.036200, -0.032800, -0.029400, -0.026300, -0.023300, -0.020600,
    -0.018000, -0.015700, -0.013600, -0.011700, -0.010000, -0.008500, -0.007200, -0.006000, -0.005000, -0.004200,
    -0.003400, -0.002800, -0.002300, -0.001900, -0.001500, -0.001200, -0.001000, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
  };

  // Temporary vectors
  std::vector<double> vpXeLT(m_numberOfPostZeroBins), vpXeHT(m_numberOfPostZeroBins);
  std::vector<double> vpKrLT(m_numberOfPostZeroBins), vpKrHT(m_numberOfPostZeroBins);
  std::vector<double> vpArLT(m_numberOfPostZeroBins), vpArHT(m_numberOfPostZeroBins);

  // Copy arrays elements to the temporary vectors
  for (int k=0; k<m_numberOfPostZeroBins; ++k) {
    vpXeLT.at(k)=pXeLT[k]; vpXeHT.at(k)=pXeHT[k];
    vpKrLT.at(k)=pKrLT[k]; vpKrHT.at(k)=pKrHT[k];
    vpArLT.at(k)=pArLT[k]; vpArHT.at(k)=pArHT[k];
  }

  // Build the vectors of shaping amplitudes
  m_lowThresholdSignalShape[0] = vpXeLT; m_highThresholdSignalShape[0] = vpXeHT;
  m_lowThresholdSignalShape[1] = vpKrLT; m_highThresholdSignalShape[1] = vpKrHT;
  m_lowThresholdSignalShape[2] = vpArLT; m_highThresholdSignalShape[2] = vpArHT;

  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "TRTElectronicsProcessing::TabulateSignalShape() done" << endmsg;
}

//___________________________________________________________________________

void TRTElectronicsProcessing::ProcessDeposits( const std::vector<TRTElectronicsProcessing::Deposit>& deposits,
						const int& hitID,
						TRTDigit& outdigit,
						double lowthreshold,
						const double& noiseamplitude,
						int strawGasType,
						double highthreshold
					      ) {
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Process the timed energy deposits at the FE for this straw:                                            //
  // - Put energy deposits in a fine-time array 0.0 < time < m_timeInterval.                                //
  // - Apply signal shaping with a Xenon, Krypton or Argon function.                                        //
  // - Add noise (LT only)                                                                                  //
  // - Apply (fine-bin) threshold discrimination; threshold fluctuations are already applied by this point. //
  // - Turn the fine discriminator array into a 27-bit output digit.                                        //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (deposits.empty() && noiseamplitude<std::numeric_limits<double>::epsilon()) {
    return;
  }

  if ( lowthreshold < 0 ) { // check if set to -1.0
    lowthreshold = !(hitID & 0x00200000) ? m_lowThresholdBar[strawGasType] :  m_lowThresholdEC[strawGasType];
  }
  if ( highthreshold < 0 ) { // check if set to -1.0
    highthreshold = getHighThreshold(hitID,strawGasType);
  }

  const double low_threshold_fluctuation(m_settings->relativeLowThresholdFluctuation());
  if ( low_threshold_fluctuation > 0 ) {
    lowthreshold = lowthreshold*CLHEP::RandGaussZiggurat::shoot(m_pHRengine, 1.0, low_threshold_fluctuation );
  }
  const double high_threshold_fluctuation(m_settings->relativeHighThresholdFluctuation());
  if ( high_threshold_fluctuation > 0 ) {
    highthreshold = highthreshold*CLHEP::RandGaussZiggurat::shoot(m_pHRengine, 1.0, high_threshold_fluctuation );
  }

  //Null out arrays: (AJB m_totalNumberOfBins=160(125ns))
  for (int i(0); i < m_totalNumberOfBins; ++i) {
    m_energyDistribution[i] = 0.;
    m_lowThresholdSignal[i] = 0.;
    m_highThresholdSignal[i] = 0.;
    m_lowThresholdDiscriminator[i] = 0;
    m_highThresholdDiscriminator[i] = 0;
  }
  // Fill cluster energies into relevant time bins
  // With pileup, m_timeInterval=125ns and signal event starting at 50 ns.
  const unsigned int numberOfDeposits(deposits.size());
  for (unsigned int i(0); i < numberOfDeposits; ++i) {
    const double time(deposits[i].time());
    if (time > 0.0 && time < m_timeInterval) {
      const int index(static_cast<int>(time / m_binWidth));
      m_energyDistribution[index] += deposits[i].energy();
    }
  }
  //std::cout << "AJB before shaping ";
  //for (int i=0; i<m_totalNumberOfBins; ++i) std::cout << m_energyDistribution[i]*1.0e6 << " "; // (eV)
  //std::cout << std::endl;

  // Signal shaping; 4 different shaping functions for: LT, HT, Argon, Xenon
  SignalShaping(strawGasType);
  //std::cout << "AJB after shaping ";
  //for (int i=0; i<m_totalNumberOfBins; ++i) std::cout <<  m_lowThresholdSignal[i]*1.0e6 << " "; // (eV); or m_highThresholdSignal[i]
  //std::cout << std::endl;

  // Add noise; LT only
  // (though both LT and HT also get fluctuations elsewhere which gives similar effect to noise).
  if ( m_pElectronicsNoise && noiseamplitude>0 ) {
    m_pElectronicsNoise->addElectronicsNoise(m_lowThresholdSignal,noiseamplitude); // LT signal only
    //std::cout << "AJB after adding noise ";
    //for (int i=0; i<m_totalNumberOfBins; ++i) std::cout <<  m_lowThresholdSignal[i]*1.0e6 << " "; // (eV); or m_highThresholdSignal[i]
    //std::cout << std::endl;
  }

  // Discriminator response (in what fine time bins are the thresholds exceeded)
  DiscriminatorResponse(lowthreshold,highthreshold);
  //std::cout << "AJB after discriminator " << strawGasType << getRegion(hitID) << std::endl;
  //for (int i=0; i<m_totalNumberOfBins; ++i) std::cout << m_lowThresholdDiscriminator[i]; // or m_highThresholdDiscriminator[i]
  //std::cout << std::endl;

  // Apply an independent LT T0 shift to m_lowThresholdDiscriminator[]
  LTt0Shift(hitID,strawGasType);
  //std::cout << "AJB after discriminator LT T0 shift " << strawGasType << getRegion(hitID) << std::endl;
  //for (int i=0; i<m_totalNumberOfBins; ++i) std::cout << m_lowThresholdDiscriminator[i];
  //std::cout << std::endl;

  // Apply an independent HT T0 shift to m_highThresholdDiscriminator[]
  HTt0Shift(hitID);
  //std::cout << "AJB after discriminator HT T0 shift " << strawGasType << getRegion(hitID) << std::endl;
  //for (int i=0; i<m_totalNumberOfBins; ++i) std::cout <<  m_highThresholdDiscriminator[i];
  //std::cout << std::endl;

  // Finally turn the fine discriminator response arrays into an output digit;
  // for RDO reduction, zero: msb, and first 4 unused bits, first and third HT bits, last 4 LT bits
  //
  //   bit31                          bit0
  //   |    leading          trailing |
  //   #----HLLLLLLLLHLLLLLLLLHLLLLLLLL
  //   00000011111111111111111011110000

  unsigned int digit = EncodeDigit() & 0x03FFFEF0;

  // Only attempt this if the digit is non-zero
  if ( m_settings->isOverlay() && digit ) { //doing overlay
    digit += (1<<31);//flag digit a "MC" one
    static bool first = true;
    if (first){
      first=false;
      msg(MSG::DEBUG) << "ACH666: Flagging digits as MC (for overlay)" << endmsg;
    }
  }

  // The digit only gets written to disk if it is non-zero
  if (digit) {
    outdigit = TRTDigit(hitID, digit);
  }

} // end of ProcessDeposits

//___________________________________________________________________________
void TRTElectronicsProcessing::SignalShaping(int strawGasType) {

  // Build m_lowThresholdSignal[] and m_highThresholdSignal[] arrays by
  // convoluting the deposit m_energyDistribution[] with electronics shaping functions.

  int i, j, k;
  for (i = 0; i < m_totalNumberOfBins; ++i)
  {
    if (m_energyDistribution[i] > 0.)
    {
      const double energyInBin(m_energyDistribution[i]);
      for (j = i; j < m_totalNumberOfBins; ++j)
      {
        k = j - i;
        if (k == m_numberOfPostZeroBins) { break; }
         m_lowThresholdSignal[j] +=  m_lowThresholdSignalShape[strawGasType][k] * energyInBin;
        m_highThresholdSignal[j] += m_highThresholdSignalShape[strawGasType][k] * energyInBin;
      }
    }
  }

}

//___________________________________________________________________________
void TRTElectronicsProcessing::DiscriminatorResponse(const double& lowthreshold, const double& highthreshold) const {
  //Input: m_lowThresholdSignal[],m_highThresholdSignal[]
  //
  //Output: m_lowThresholdDiscriminator[], m_highThresholdDiscriminator[]
  //
  //Uses internally:m_totalNumberOfBins,
  //                m_minWidthMinusSettlingTimeInBinWidths,
  //                m_minDiscriminatorWidthInBinWidths,
  //                m_discriminatorSettlingTimeInBinWidths,
  //                m_discriminatorDeadTimeInBinWidths
  //                threshold fluctuation variables from m_settings.

  int lowThresholdBins(0);
  int highThresholdBins(0);

  int lowThresholdAdditionalBins(0);
  int highThresholdAdditionalBins(0);

  for (int i(0); i < m_totalNumberOfBins; ++i)
  {
    // ----- low threshold -----
    if (lowThresholdBins == 0)
    {
      if (m_lowThresholdSignal[i] > lowthreshold)
      {
	m_lowThresholdDiscriminator[i] = 1;
	++lowThresholdBins;
      }
    }
    else if (lowThresholdBins > 0)
    {
      if (m_lowThresholdSignal[i] > lowthreshold)
      {
	m_lowThresholdDiscriminator[i] = 1;
	++lowThresholdBins;
	lowThresholdAdditionalBins = 0;
      }
      else
      {
	if (lowThresholdAdditionalBins == 0)
	{
	  if (lowThresholdBins < m_minWidthMinusSettlingTimeInBinWidths)
	  {
	    lowThresholdAdditionalBins = m_minDiscriminatorWidthInBinWidths - lowThresholdBins;
	  }
	  else
	  {
	    lowThresholdAdditionalBins = m_discriminatorSettlingTimeInBinWidths;
	  }
	}
	m_lowThresholdDiscriminator[i] = 1;
	++lowThresholdBins;
	--lowThresholdAdditionalBins;
	if (lowThresholdAdditionalBins == 0)
	{
	  if (!(m_lowThresholdSignal[i] > lowthreshold))
	  {
	    lowThresholdBins = -m_discriminatorDeadTimeInBinWidths;
	  }
	}
      }
    }
    else
    {
      ++lowThresholdBins;
    }

    //----- high threshold -----
    if (highThresholdBins == 0)
    {
      if (m_highThresholdSignal[i] > highthreshold)
      {
	m_highThresholdDiscriminator[i] = 1;
	++highThresholdBins;
      }
    }
    else if (highThresholdBins > 0)
    {
      if (m_highThresholdSignal[i] > highthreshold)
      {
	m_highThresholdDiscriminator[i] = 1;
	++highThresholdBins;
	highThresholdAdditionalBins = 0;
      }
      else
      {
	if (highThresholdAdditionalBins == 0)
	{
	  if (highThresholdBins < m_minWidthMinusSettlingTimeInBinWidths)
	  {
	    highThresholdAdditionalBins = m_minDiscriminatorWidthInBinWidths - highThresholdBins;
	  }
	  else
	  {
	    highThresholdAdditionalBins = m_discriminatorSettlingTimeInBinWidths;
	  }
	}
	m_highThresholdDiscriminator[i] = 1;
	++highThresholdBins;
	--highThresholdAdditionalBins;
	if (highThresholdAdditionalBins == 0)
	{
	  if (!(m_highThresholdSignal[i] > highthreshold))
	  {
	    highThresholdBins = -m_discriminatorDeadTimeInBinWidths;
	  }
	}
      }
    }
    else
    {
      ++highThresholdBins;
    }
  }
  return;
}

//___________________________________________________________________________
unsigned TRTElectronicsProcessing::EncodeDigit() const {
  //Input: m_lowThresholdDiscriminator[], m_highThresholdDiscriminator[]
  //
  //Output: digit
  //
  //Uses internally:m_numberOfPreZeroBins

  unsigned digit(0);
  const unsigned one(1);
  int i, j, k;

  for (i = 0; i < 24; ++i)
  {
    j = i * 4 + m_numberOfPreZeroBins;
    for (k = 0; k < 4; ++k)
    {
      if (m_lowThresholdDiscriminator[j + k] == 1)
      {
        digit += one << (25 - i - i / 8);
        break;
      }
    }
  }

  for (i = 0; i < 3; ++i)
  {
    j = i * 32 + m_numberOfPreZeroBins;
    for (k = 0; k < 32; ++k)
    {
      if (m_highThresholdDiscriminator[j + k] == 1)
      {
        digit += one << (26 - i * 9);
        break;
      }
    }
  }

  return digit;
}

//_____________________________________________________________________________
unsigned int TRTElectronicsProcessing::getRegion(int hitID) {

  // 1=barrelShort, 2=barrelLong, 3=ECA, 4=ECB
  const int mask(0x0000001F);
  const int word_shift(5);
  int layerID, ringID, wheelID;
  unsigned int region(0);

  if ( !(hitID & 0x00200000) ) { // barrel

    hitID >>= word_shift;
    layerID = hitID & mask;
    hitID >>= word_shift;
    hitID >>= word_shift;
    ringID = hitID & mask;
    region = ( (layerID < 9) && (ringID == 0) ) ? 1 : 2;

  } else { // endcap

    hitID >>= word_shift;
    hitID >>= word_shift;
    hitID >>= word_shift;
    wheelID = hitID & mask;
    region = wheelID < 8 ?  3 : 4;

  }

  return region;

}

//_____________________________________________________________________________
double TRTElectronicsProcessing::getHighThreshold ( int hitID, int strawGasType ) {
  double highthreshold(0.);
  switch ( getRegion(hitID) ) {
    case 1: highthreshold = m_settings->highThresholdBarShort(strawGasType);  break;
    case 2: highthreshold = m_settings->highThresholdBarLong(strawGasType);   break;
    case 3: highthreshold = m_settings->highThresholdECAwheels(strawGasType); break;
    case 4: highthreshold = m_settings->highThresholdECBwheels(strawGasType); break;
    default:
      if (msgLevel(MSG::WARNING)) {msg(MSG::WARNING) << "TRTDigitization::TRTElectronicsProcessing - getRegion is zero!" <<  endmsg; }
      break;
  }
  return highthreshold;
}

//___________________________________________________________________________
void TRTElectronicsProcessing::HTt0Shift(int hitID) {

  // Apply a (small)timing shift to m_highThresholdDiscriminator[]
  // t0Shift tuning is provided by the parameters:
  // htT0shiftBarShort, htT0shiftBarLong, htT0shiftECAwheels and m_htT0shiftECBwheels

  int t0Shift(0); // in 0.78125 ns steps
  switch ( getRegion(hitID) ) {
    case 1: t0Shift = m_settings->htT0shiftBarShort();  break;
    case 2: t0Shift = m_settings->htT0shiftBarLong();   break;
    case 3: t0Shift = m_settings->htT0shiftECAwheels(); break;
    case 4: t0Shift = m_settings->htT0shiftECBwheels(); break;
    default:
      if (msgLevel(MSG::WARNING)) {msg(MSG::WARNING) << "TRTDigitization::TRTElectronicsProcessing - getRegion is zero!" <<  endmsg; }
      break;
  }

  if (!t0Shift) return; // skip this process if there is no shift

  unsigned int vsum=0;
  for (int i=0; i<m_totalNumberOfBins; ++i) { vsum += m_highThresholdDiscriminator[i]; }
  if (!vsum) return; // skip this process if there are no HT bits

  if (t0Shift<0) { // for negative shifts

      for (int i=0; i<m_totalNumberOfBins; ++i) {
        if (i-t0Shift>=m_totalNumberOfBins) break;
        m_highThresholdDiscriminator[i]=m_highThresholdDiscriminator[i-t0Shift];
      }
      for (int i=m_totalNumberOfBins+t0Shift; i<m_totalNumberOfBins; ++i) if (i>=0) m_highThresholdDiscriminator[i]=0; // the last t0Shift bins are set to zero

  } else {  // for positive shifts

      for (int i=m_totalNumberOfBins-1; i>0; --i) {
        if (i-t0Shift<0) break;
        m_highThresholdDiscriminator[i]=m_highThresholdDiscriminator[i-t0Shift];
      }
      for (int i=0; i<t0Shift; ++i) if (i<m_totalNumberOfBins) m_highThresholdDiscriminator[i]=0; // the first t0Shift bins are set to zero

  }

  return;

}

//_____________________________________________________________________________
void TRTElectronicsProcessing::LTt0Shift( int hitID, int strawGasType ) {

  // Apply a (small)timing shift to m_lowThresholdDiscriminator[]
  // t0Shift tuning is provided by the parameters:
  // ltT0shiftBarShort, ltT0shiftBarLong, ltT0shiftECAwheels and m_ltT0shiftECBwheels

  int t0Shift(0); // in 0.78125 ns steps
  switch ( getRegion(hitID) ) {
    case 1: t0Shift = m_settings->ltT0shiftBarShort(strawGasType);  break;
    case 2: t0Shift = m_settings->ltT0shiftBarLong(strawGasType);   break;
    case 3: t0Shift = m_settings->ltT0shiftECAwheels(strawGasType); break;
    case 4: t0Shift = m_settings->ltT0shiftECBwheels(strawGasType); break;
    default:
      if (msgLevel(MSG::WARNING)) {msg(MSG::WARNING) << "TRTDigitization::TRTElectronicsProcessing - getRegion is zero!" <<  endmsg; }
      break;
  }

  if (!t0Shift) return; // skip this process if there is no shift

  unsigned int vsum=0;
  for (int i=0; i<m_totalNumberOfBins; ++i) { vsum += m_lowThresholdDiscriminator[i]; }
  if (!vsum) return; // skip this process if there are no LT bits

  if (t0Shift<0) { // for negative shifts

      for (int i=0; i<m_totalNumberOfBins; ++i) {
        if (i-t0Shift>=m_totalNumberOfBins) break;
        m_lowThresholdDiscriminator[i]=m_lowThresholdDiscriminator[i-t0Shift];
      }
      for (int i=m_totalNumberOfBins+t0Shift; i<m_totalNumberOfBins; ++i) if (i>=0) m_lowThresholdDiscriminator[i]=0; // the last t0Shift bins are set to zero

  } else {  // for positive shifts

      for (int i=m_totalNumberOfBins-1; i>0; --i) {
        if (i-t0Shift<0) break;
        m_lowThresholdDiscriminator[i]=m_lowThresholdDiscriminator[i-t0Shift];
      }
      for (int i=0; i<t0Shift; ++i) if (i<m_totalNumberOfBins) m_lowThresholdDiscriminator[i]=0; // the first t0Shift bins are set to zero

  }

  return;

}
//_____________________________________________________________________________

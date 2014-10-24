/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTElectronicsProcessing.h"

#include "TRTElectronicsNoise.h"
#include "TRTDigit.h"

#include "AthenaKernel/IAtRndmGenSvc.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
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
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) <<"TRTElectronicsProcessing::Constructor begin" << endreq;

  m_pHRengine  = atRndmGenSvc->GetEngine("TRT_ThresholdFluctuations");
  m_pHRengine2 = atRndmGenSvc->GetEngine("TRT_StreamerToHT");

  Initialize();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) <<"TRTElectronicsProcessing::Constructor done" << endreq;
}

//___________________________________________________________________________
TRTElectronicsProcessing::~TRTElectronicsProcessing() {

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TRTElectronicsProcessing::Destructor begin" << endreq;

  delete [] m_energyDistribution;
  delete [] m_lowThresholdDiscriminator;
  delete [] m_highThresholdDiscriminator;

  delete m_pElectronicsNoise;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TRTElectronicsProcessing::Destructor done" << endreq;
}

//___________________________________________________________________________
void TRTElectronicsProcessing::Initialize() {

  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "TRTElectronicsProcessing::Initialize() begin" << endreq;

  m_doStreamer = m_settings->doStreamer();
  m_streamerThreshold = m_settings->streamerThreshold();
  m_doFastOr = m_settings->doFastOr();

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

  m_lowThresholdEC[0]  = m_settings->lowThresholdEC(false);
  m_lowThresholdBar[0] = m_settings->lowThresholdBar(false);
  m_lowThresholdEC[1]  = m_settings->lowThresholdEC(true);
  m_lowThresholdBar[1] = m_settings->lowThresholdBar(true);

  TabulateSignalShape();

  // for (int j=0; j<4; j++) { for (int i=0; i<m_numberOfPostZeroBins; i++) std::cout << "AJB " << j << " " << m_lowThresholdSignalShape[j][i] << std::endl; }
  // for (int j=0; j<4; j++) { for (int i=0; i<m_numberOfPostZeroBins; i++) std::cout << "AJB " << j << " " << m_highThresholdSignalShape[j][i] << std::endl; }

  m_energyDistribution = new double[m_totalNumberOfBins];
  m_lowThresholdSignal.reserve(m_totalNumberOfBins);
  m_lowThresholdSignal.resize(m_totalNumberOfBins, 0.0);
  m_highThresholdSignal.reserve(m_totalNumberOfBins);
  m_highThresholdSignal.resize(m_totalNumberOfBins, 0.0);
  m_lowThresholdDiscriminator = new int[m_totalNumberOfBins];//TK switch to boolean vector[fixme:not deleted]
  m_highThresholdDiscriminator = new int[m_totalNumberOfBins];//[fixme:not deleted]

  m_maskA  = 0x03FC0000;
  m_maskB  = 0x0001FE00;
  m_maskC  = 0x000000FF;
  m_maskHT = 0x04020100;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "TRTElectronicsProcessing::Initialize() done" << endreq;
}

//___________________________________________________________________________
void TRTElectronicsProcessing::TabulateSignalShape() {

  // Fixme: This assumes 0.78125 ns bin spacing and m_numberOfPostZeroBins=160 bins!
  // So we need to hard-code the processing to reflect that this is not a parameter!

  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "TRTElectronicsProcessing::TabulateSignalShape() begin" << endreq;

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

  const double pXeHT[160] = {
    0.031250, 0.093941, 0.157311, 0.224553, 0.308086, 0.406172, 0.515030, 0.628505, 0.739303, 0.838516,
    0.918813, 0.971938, 0.996352, 0.988461, 0.950406, 0.886352, 0.803148, 0.705500, 0.601875, 0.497383,
    0.397172, 0.305766, 0.224766, 0.155939, 0.099530, 0.054139, 0.019225, -0.007841, -0.027986, -0.042395,
    -0.052809, -0.060523, -0.065396, -0.067861, -0.069166, -0.068754, -0.067225, -0.064703, -0.061309, -0.057052,
    -0.052265, -0.047181, -0.041933, -0.036656, -0.031588, -0.026811, -0.022384, -0.018355, -0.014849, -0.011875,
    -0.009328, -0.007185, -0.005418, -0.004090, -0.003031, -0.002197, -0.001556, -0.001091, -0.000766, -0.000524,
    -0.000350, -0.000227, -0.000150, -0.000052, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
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

  const double pArHT[160] = {
    0.031250, 0.093941, 0.157311, 0.224553, 0.308086, 0.406172, 0.515030, 0.628505, 0.739303, 0.838516,
    0.918813, 0.971938, 0.996352, 0.988461, 0.950406, 0.886352, 0.803148, 0.705500, 0.601875, 0.497383,
    0.397172, 0.305766, 0.224766, 0.155939, 0.099530, 0.054139, 0.019225, -0.007841, -0.027986, -0.042395,
    -0.052809, -0.060523, -0.065396, -0.067861, -0.069166, -0.068754, -0.067225, -0.064703, -0.061309, -0.057052,
    -0.052265, -0.047181, -0.041933, -0.036656, -0.031588, -0.026811, -0.022384, -0.018355, -0.014849, -0.011875,
    -0.009328, -0.007185, -0.005418, -0.004090, -0.003031, -0.002197, -0.001556, -0.001091, -0.000766, -0.000524,
    -0.000350, -0.000227, -0.000150, -0.000052, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
  };


  // Streamer versions of the shaping functions are included here for possible use in the future.
  // However they are currently commented and unused - instead the ternary output is modified for
  // streamers (but only with m_doFastOr = true)

  /*
  const double sXeLT[160] = {
    0.002900, 0.006600, 0.011900, 0.026800, 0.052900, 0.094800, 0.174100, 0.264600, 0.366100, 0.486200,
    0.633800, 0.764600, 0.889600, 0.971600, 0.999500, 1.000000, 0.981800, 0.968400, 0.961300, 0.959000,
    0.965400, 0.965000, 0.964900, 0.967500, 0.970800, 0.970000, 0.967600, 0.963700, 0.959600, 0.962400,
    0.959000, 0.954400, 0.947600, 0.934200, 0.912100, 0.884300, 0.848400, 0.802200, 0.743100, 0.675900,
    0.598700, 0.519300, 0.449700, 0.386900, 0.338500, 0.307100, 0.290700, 0.279200, 0.272900, 0.267400,
    0.263800, 0.263400, 0.260400, 0.257400, 0.253600, 0.247400, 0.235200, 0.233000, 0.225200, 0.215900,
    0.209200, 0.201700, 0.195900, 0.188900, 0.180700, 0.173000, 0.171200, 0.169900, 0.166000, 0.158900,
    0.155500, 0.152200, 0.150000, 0.147000, 0.140500, 0.139000, 0.139800, 0.137800, 0.131900, 0.126700,
    0.120900, 0.116600, 0.114100, 0.113400, 0.110400, 0.107100, 0.104800, 0.103700, 0.103300, 0.095600,
    0.091200, 0.092300, 0.097900, 0.091000, 0.089300, 0.086200, 0.082500, 0.082400, 0.082700, 0.080600,
    0.078900, 0.078200, 0.072300, 0.072500, 0.072900, 0.071100, 0.066100, 0.067600, 0.066000, 0.063200,
    0.060200, 0.058200, 0.059600, 0.058000, 0.055500, 0.054900, 0.053400, 0.051800, 0.051500, 0.051300,
    0.046900, 0.048100, 0.047500, 0.046700, 0.047500, 0.047500, 0.043600, 0.039800, 0.037900, 0.038700,
    0.036800, 0.038500, 0.038500, 0.034700, 0.037000, 0.031700, 0.027700, 0.027600, 0.032400, 0.027800,
    0.026700, 0.027400, 0.028000, 0.025100, 0.025200, 0.024600, 0.024200, 0.025000, 0.023200, 0.026400,
    0.025200, 0.019800, 0.020000, 0.019200, 0.017600, 0.017000, 0.017800, 0.014700, 0.015000, 0.016800
  };

  const double sXeHT[160] = {
    0.002900, 0.006600, 0.011900, 0.026800, 0.052900, 0.094800, 0.174100, 0.264600, 0.366100, 0.486200,
    0.633800, 0.764600, 0.889600, 0.971600, 0.999500, 1.000000, 0.981800, 0.968400, 0.961300, 0.959000,
    0.965400, 0.965000, 0.964900, 0.967500, 0.970800, 0.970000, 0.967600, 0.963700, 0.959600, 0.962400,
    0.959000, 0.954400, 0.947600, 0.934200, 0.912100, 0.884300, 0.848400, 0.802200, 0.743100, 0.675900,
    0.598700, 0.519300, 0.449700, 0.386900, 0.338500, 0.307100, 0.290700, 0.279200, 0.272900, 0.267400,
    0.263800, 0.263400, 0.260400, 0.257400, 0.253600, 0.247400, 0.235200, 0.233000, 0.225200, 0.215900,
    0.209200, 0.201700, 0.195900, 0.188900, 0.180700, 0.173000, 0.171200, 0.169900, 0.166000, 0.158900,
    0.155500, 0.152200, 0.150000, 0.147000, 0.140500, 0.139000, 0.139800, 0.137800, 0.131900, 0.126700,
    0.120900, 0.116600, 0.114100, 0.113400, 0.110400, 0.107100, 0.104800, 0.103700, 0.103300, 0.095600,
    0.091200, 0.092300, 0.097900, 0.091000, 0.089300, 0.086200, 0.082500, 0.082400, 0.082700, 0.080600,
    0.078900, 0.078200, 0.072300, 0.072500, 0.072900, 0.071100, 0.066100, 0.067600, 0.066000, 0.063200,
    0.060200, 0.058200, 0.059600, 0.058000, 0.055500, 0.054900, 0.053400, 0.051800, 0.051500, 0.051300,
    0.046900, 0.048100, 0.047500, 0.046700, 0.047500, 0.047500, 0.043600, 0.039800, 0.037900, 0.038700,
    0.036800, 0.038500, 0.038500, 0.034700, 0.037000, 0.031700, 0.027700, 0.027600, 0.032400, 0.027800,
    0.026700, 0.027400, 0.028000, 0.025100, 0.025200, 0.024600, 0.024200, 0.025000, 0.023200, 0.026400,
    0.025200, 0.019800, 0.020000, 0.019200, 0.017600, 0.017000, 0.017800, 0.014700, 0.015000, 0.016800
  };

  const double sArLT[160] = {
    0.002900, 0.006600, 0.011900, 0.026800, 0.052900, 0.094800, 0.174100, 0.264600, 0.366100, 0.486200,
    0.633800, 0.764600, 0.889600, 0.971600, 0.999500, 1.000000, 0.981800, 0.968400, 0.961300, 0.959000,
    0.965400, 0.965000, 0.964900, 0.967500, 0.970800, 0.970000, 0.967600, 0.963700, 0.959600, 0.962400,
    0.959000, 0.954400, 0.947600, 0.934200, 0.912100, 0.884300, 0.848400, 0.802200, 0.743100, 0.675900,
    0.598700, 0.519300, 0.449700, 0.386900, 0.338500, 0.307100, 0.290700, 0.279200, 0.272900, 0.267400,
    0.263800, 0.263400, 0.260400, 0.257400, 0.253600, 0.247400, 0.235200, 0.233000, 0.225200, 0.215900,
    0.209200, 0.201700, 0.195900, 0.188900, 0.180700, 0.173000, 0.171200, 0.169900, 0.166000, 0.158900,
    0.155500, 0.152200, 0.150000, 0.147000, 0.140500, 0.139000, 0.139800, 0.137800, 0.131900, 0.126700,
    0.120900, 0.116600, 0.114100, 0.113400, 0.110400, 0.107100, 0.104800, 0.103700, 0.103300, 0.095600,
    0.091200, 0.092300, 0.097900, 0.091000, 0.089300, 0.086200, 0.082500, 0.082400, 0.082700, 0.080600,
    0.078900, 0.078200, 0.072300, 0.072500, 0.072900, 0.071100, 0.066100, 0.067600, 0.066000, 0.063200,
    0.060200, 0.058200, 0.059600, 0.058000, 0.055500, 0.054900, 0.053400, 0.051800, 0.051500, 0.051300,
    0.046900, 0.048100, 0.047500, 0.046700, 0.047500, 0.047500, 0.043600, 0.039800, 0.037900, 0.038700,
    0.036800, 0.038500, 0.038500, 0.034700, 0.037000, 0.031700, 0.027700, 0.027600, 0.032400, 0.027800,
    0.026700, 0.027400, 0.028000, 0.025100, 0.025200, 0.024600, 0.024200, 0.025000, 0.023200, 0.026400,
    0.025200, 0.019800, 0.020000, 0.019200, 0.017600, 0.017000, 0.017800, 0.014700, 0.015000, 0.016800
  };

  const double sArHT[160] = {
    0.002900, 0.006600, 0.011900, 0.026800, 0.052900, 0.094800, 0.174100, 0.264600, 0.366100, 0.486200,
    0.633800, 0.764600, 0.889600, 0.971600, 0.999500, 1.000000, 0.981800, 0.968400, 0.961300, 0.959000,
    0.965400, 0.965000, 0.964900, 0.967500, 0.970800, 0.970000, 0.967600, 0.963700, 0.959600, 0.962400,
    0.959000, 0.954400, 0.947600, 0.934200, 0.912100, 0.884300, 0.848400, 0.802200, 0.743100, 0.675900,
    0.598700, 0.519300, 0.449700, 0.386900, 0.338500, 0.307100, 0.290700, 0.279200, 0.272900, 0.267400,
    0.263800, 0.263400, 0.260400, 0.257400, 0.253600, 0.247400, 0.235200, 0.233000, 0.225200, 0.215900,
    0.209200, 0.201700, 0.195900, 0.188900, 0.180700, 0.173000, 0.171200, 0.169900, 0.166000, 0.158900,
    0.155500, 0.152200, 0.150000, 0.147000, 0.140500, 0.139000, 0.139800, 0.137800, 0.131900, 0.126700,
    0.120900, 0.116600, 0.114100, 0.113400, 0.110400, 0.107100, 0.104800, 0.103700, 0.103300, 0.095600,
    0.091200, 0.092300, 0.097900, 0.091000, 0.089300, 0.086200, 0.082500, 0.082400, 0.082700, 0.080600,
    0.078900, 0.078200, 0.072300, 0.072500, 0.072900, 0.071100, 0.066100, 0.067600, 0.066000, 0.063200,
    0.060200, 0.058200, 0.059600, 0.058000, 0.055500, 0.054900, 0.053400, 0.051800, 0.051500, 0.051300,
    0.046900, 0.048100, 0.047500, 0.046700, 0.047500, 0.047500, 0.043600, 0.039800, 0.037900, 0.038700,
    0.036800, 0.038500, 0.038500, 0.034700, 0.037000, 0.031700, 0.027700, 0.027600, 0.032400, 0.027800,
    0.026700, 0.027400, 0.028000, 0.025100, 0.025200, 0.024600, 0.024200, 0.025000, 0.023200, 0.026400,
    0.025200, 0.019800, 0.020000, 0.019200, 0.017600, 0.017000, 0.017800, 0.014700, 0.015000, 0.016800
  };
  */

  // We need to build four LT\HT pairs of amplitudes with iMode = isArgonStraws + 2*isStreamer;
  //
  //   iMode = 0 (isArgon = 0, isStreamer = 0) : Xenon straw in proportional-mode
  //   iMode = 1 (isArgon = 1, isStreamer = 0) : Argon straw in proportional-mode
  //   iMode = 2 (isArgon = 0, isStreamer = 1) : Xenon straw in streamer-mode
  //   iMode = 3 (isArgon = 1, isStreamer = 1) : Argon straw in streamer-mode

  // Temporary vectors
  std::vector<double> vpXeLT(m_numberOfPostZeroBins), vpXeHT(m_numberOfPostZeroBins);
  std::vector<double> vpArLT(m_numberOfPostZeroBins), vpArHT(m_numberOfPostZeroBins);
  std::vector<double> vsXeLT(m_numberOfPostZeroBins), vsXeHT(m_numberOfPostZeroBins);
  std::vector<double> vsArLT(m_numberOfPostZeroBins), vsArHT(m_numberOfPostZeroBins);

  // Copy arrays elements to the temporary vectors
  for (int k=0; k<m_numberOfPostZeroBins; ++k) {
    vpXeLT.at(k)=pXeLT[k]; vpXeHT.at(k)=pXeHT[k];
    vpArLT.at(k)=pArLT[k]; vpArHT.at(k)=pArHT[k];
    //vsXeLT.at(k)=sXeLT[k]; vsXeHT.at(k)=sXeHT[k]; // Uncomment these if you want to use special
    //vsArLT.at(k)=sArLT[k]; vsArHT.at(k)=sArHT[k]; // streamer-mode shaping functions (and comment below).
    vsXeLT.at(k)=pXeLT[k]; vsXeHT.at(k)=pXeHT[k]; // NOTE: streamer-mode shaping is done using proportional-mode
    vsArLT.at(k)=pArLT[k]; vsArHT.at(k)=pArHT[k]; // arrays; the streamer is modelled at ternary output stage.
  }

  // Build the vectors of shaping amplitudes
  m_lowThresholdSignalShape[0] = vpXeLT; m_highThresholdSignalShape[0] = vpXeHT;
  m_lowThresholdSignalShape[1] = vpArLT; m_highThresholdSignalShape[1] = vpArHT;
  m_lowThresholdSignalShape[2] = vsXeLT; m_highThresholdSignalShape[2] = vsXeHT;
  m_lowThresholdSignalShape[3] = vsArLT; m_highThresholdSignalShape[3] = vsArHT;

  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "TRTElectronicsProcessing::TabulateSignalShape() done" << endreq;
}

//___________________________________________________________________________

void TRTElectronicsProcessing::ProcessDeposits( const std::vector<TRTElectronicsProcessing::Deposit>& deposits,
						const int& hitID,
						TRTDigit& outdigit,
                                                std::vector<int>& m_fastOrHtDiscriminator,
						double lowthreshold,
						const double& noiseamplitude,
						bool isArgonStraw,
						double highthreshold
					      ) {
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Process the timed energy deposits at the FE for this straw:                                            //
  // - Put energy deposits in a fine-time array 0.0 < time < m_timeInterval.                                //
  // - Apply signal shaping with a Xenon or Argon function.                                                 //
  // - Add noise (LT only)                                                                                  //
  // - Apply (fine-bin) threshold discrimination; threshold fluctuations are already applied by this point. //
  // - *optionally* Modify the (fine-bin) HT discriminator array for streamers (if m_doStreamer = true).    //
  // - Turn the fine discriminator array into a 27-bit output digit.                                        //
  // - *optionally* Send (fine-bin) HT discriminator array to the FastOR algorithm (if m_doFastOr = true).  //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////

  bool isStreamer=false; // fixme: not used currently, unless you want a special shaping function for streaming straws.
                         // we actually modify the ternary output for streamers - so remove?

  if (deposits.empty() && noiseamplitude<std::numeric_limits<double>::epsilon()) {
    return;
  }

  if ( lowthreshold < 0 ) { // check if set to -1.0
    lowthreshold = !(hitID & 0x00200000) ? m_lowThresholdBar[isArgonStraw] :  m_lowThresholdEC[isArgonStraw];
  }
  if ( highthreshold < 0 ) { // check if set to -1.0
    highthreshold = getHighThreshold(hitID,isArgonStraw);
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
  m_fastOrHtDiscriminator.clear();
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

  // Signal shaping; 8 different shaping functions for:
  // LT, HT, Argon, Xenon, proportional-mode, streamer mode.
  SignalShaping(isArgonStraw,isStreamer); // fixme: isStreamer is always 0.
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
  //std::cout << "AJB after discriminator ";
  //for (int i=0; i<m_totalNumberOfBins; ++i) std::cout <<  m_lowhThresholdDiscriminator[i] << " "; // or m_highThresholdDiscriminator[i]
  //std::cout << std::endl;

  // Apply an independent HT T0 shift to m_highThresholdDiscriminator[]
  HTdeltaShift(hitID);
  //std::cout << "AJB after discriminator HT delta T0 shift";
  //for (int i=0; i<m_totalNumberOfBins; ++i) std::cout <<  m_highhThresholdDiscriminator[i] << " ";
  //std::cout << std::endl;

  // Optionally check if the straw has gone into streamer mode (depSum>m_streamerThreshold).
  // This is almost always the case for HIPs but very rare for normal events. If the straw is streaming
  // then the response is to set the ToHT (from that time) using nBinsOverHT() as measure with alpha rays.
  // Fixme: the bool isStreamer is not used! The idea is that it would be available earlier on in the code for alternative shaping (which we don't use).
  if (m_doStreamer)
    {
      double depSum=0.0;
      for (int ibin=0; ibin<m_totalNumberOfBins; ++ibin) {
        depSum += m_energyDistribution[ibin];
        if (depSum>m_streamerThreshold) {
          isStreamer = true;
          int jbin = ibin + nBinsOverHT();
          if (jbin>m_totalNumberOfBins) jbin=m_totalNumberOfBins;
          for (int i=ibin; i<jbin; ++i)                { m_highThresholdDiscriminator[i]=1; } // set nBinsOverHT bins to "1",
          for (int i=jbin; i<m_totalNumberOfBins; ++i) { m_highThresholdDiscriminator[i]=0; } // thereafter all bins are "0".
          break;
        }
      }
      //  std::cout << "AJB after streamer modifications ";
      //  for (int i=0; i<m_totalNumberOfBins; ++i) std::cout <<  m_highThresholdDiscriminator[i] << " "; // or m_highThresholdDiscriminator[i]
      //  std::cout << std::endl;
    }

  // Finally turn the fine discriminator response arrays into an output digit.
  const unsigned int digit(EncodeDigit());
  if (digit) {
    outdigit = TRTDigit(hitID, digit);
  }

  //////////////////////////////////////////////////////////////
  // Optionally send the fine-bin discriminator HT output to  //
  // the FastOR algorithm via m_fastOrHtDiscriminator. This   //
  // gets returned anyway but is empty if m_doFastOr = false. //
  //////////////////////////////////////////////////////////////
  if (m_doFastOr)
    {
      // To save a lot of time in the FastOR class, only copy
      // m_highThresholdDiscriminator to m_fastOrHtDiscriminator
      // if there is at least one fine HT bit set. (increase?)
      int nHTbitsset=0;
      for (int ibin=0; ibin<m_totalNumberOfBins; ++ibin) {
        nHTbitsset += m_highThresholdDiscriminator[ibin];
      }
      // straw_HT_ternaryInfo: 1 element (bin) = 0.78125 ns. There are 160 bins for each HT-straw vector.
      // XPT: Make sure that after a leading edge in the HT the next 10 ns (~13 bins) are set to 1;
      //      this makes sure the FastOR signal out of the DTMROC has a minimum width of 10 ns.
      if (nHTbitsset>0) {
        for (int ibin=0; ibin<m_totalNumberOfBins; ++ibin) {
	  if (ibin>51) { // Start looking for leading edges at bin 51 (-10.15625 ns from 3rd bx)
	    if ( m_highThresholdDiscriminator[ibin] == 1 && m_highThresholdDiscriminator[ibin-1] == 0 ){ // leading edge found!
 	      for(int jbin=0; jbin<13 && ((ibin+jbin)<m_totalNumberOfBins) ; jbin++) {
  		m_highThresholdDiscriminator[ibin+jbin] = 1;
	      }
	    }
	  }
          m_fastOrHtDiscriminator.push_back(m_highThresholdDiscriminator[ibin]);
        }
      }
    } // end of FastOR section

} // end of ProcessDeposits

//___________________________________________________________________________
void TRTElectronicsProcessing::SignalShaping(bool isArgonStraw, bool isStreamer) {

  // Build m_lowThresholdSignal[] and m_highThresholdSignal[] arrays by
  // convoluting the deposit m_energyDistribution[] with electronics shaping functions.

  // Fixme: currently isStreamer is not used, and likely not to be in the future.
  //        Consider removing isStreamer throughout the package (remove 4 out of
  //        8 shaping functions and quite a bit off code).

  const unsigned int iMode = isArgonStraw + 2*isStreamer;

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
        m_lowThresholdSignal[j]  +=  m_lowThresholdSignalShape[iMode][k] * energyInBin;
        m_highThresholdSignal[j] += m_highThresholdSignalShape[iMode][k] * energyInBin;
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
double TRTElectronicsProcessing::getHighThreshold ( int hitID, bool isArgonStraw ) {

  const int mask(0x0000001F);
  const int word_shift(5);
  int layerID, ringID, wheelID;
  double highthreshold(0.);

  if ( !(hitID & 0x00200000) ) { // barrel

    hitID >>= word_shift;
    layerID = hitID & mask;
    hitID >>= word_shift;
    hitID >>= word_shift;
    ringID = hitID & mask;
    highthreshold = ( (layerID < 9) && (ringID == 0) ) ? m_settings->highThresholdBarShort(isArgonStraw) : m_settings->highThresholdBarLong(isArgonStraw) ;

  } else { // endcap

    hitID >>= word_shift;
    hitID >>= word_shift;
    hitID >>= word_shift;
    wheelID = hitID & mask;
    highthreshold = wheelID < 8 ?  m_settings->highThresholdECAwheels(isArgonStraw) : m_settings->highThresholdECBwheels(isArgonStraw);

  }

  return highthreshold;

}

//_____________________________________________________________________________
int TRTElectronicsProcessing::getHTdeltaT0Shift(int hitID) {

  const int mask(0x0000001F);
  const int word_shift(5);
  int layerID, ringID, wheelID;
  int deltaT0Shift(0);

  if ( !(hitID & 0x00200000) ) { // barrel

    hitID >>= word_shift;
    layerID = hitID & mask;
    hitID >>= word_shift;
    hitID >>= word_shift;
    ringID = hitID & mask;
    deltaT0Shift = ( (layerID < 9) && (ringID == 0) ) ? m_settings->htT0shiftBarShort() : m_settings->htT0shiftBarLong() ;

  } else { // endcap

    hitID >>= word_shift;
    hitID >>= word_shift;
    hitID >>= word_shift;
    wheelID = hitID & mask;
    deltaT0Shift = wheelID < 8 ?  m_settings->htT0shiftECAwheels() : m_settings->htT0shiftECBwheels();

  }

  return deltaT0Shift;

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

// RNG modelling of measured ToHT values for alpha particles by Ximo Poveda Torres May 2013).
// The statistics were not very high (580 values). See:
// https://indico.cern.ch/getFile.py/access?contribId=0&resId=0&materialId=slides&confId=225739
int TRTElectronicsProcessing::nBinsOverHT() const {
  double ToHT;
  const double split(CLHEP::RandFlat::shoot(m_pHRengine2));
  if (split>0.42) {
    ToHT = CLHEP::RandGaussZiggurat::shoot(m_pHRengine2, 52.66, 0.50 );
  } else {
    const double r1(CLHEP::RandFlat::shoot(m_pHRengine2));
    const double r2(CLHEP::RandFlat::shoot(m_pHRengine2));
    ToHT = 25.0 + 21.0*r1 + 5.0*r2;
  }
  return static_cast<int>(ToHT/m_binWidth+0.5);
}

//___________________________________________________________________________
void TRTElectronicsProcessing::HTdeltaShift(int hitID) {

  // Apply a (small)timing shift to m_highThresholdDiscriminator[] w.r.t the overall T0.
  // Tuning is provided by the parameters: htT0shiftBarShort, htT0shiftBarLong,
  // htT0shiftECAwheels and m_htT0shiftECBwheels which are fetched with getHTdeltaT0Shift(hitID).

  int j = getHTdeltaT0Shift(hitID);
  if (!j) return; // skip this process if there is no shift

  unsigned int vsum=0;
  for (int i=0; i<m_totalNumberOfBins; ++i) { vsum += m_highThresholdDiscriminator[i]; }
  if (!vsum) return; // skip this process if there are no HT bits

  if (j<0) { // for negative shifts

      for (int i=0; i<m_totalNumberOfBins; ++i) {
        if (i-j>=m_totalNumberOfBins) break;
        m_highThresholdDiscriminator[i]=m_highThresholdDiscriminator[i-j];
      }
      for (int i=m_totalNumberOfBins+j; i<m_totalNumberOfBins; ++i) if (i>=0) m_highThresholdDiscriminator[i]=0; // the last j bins are set to zero

  } else {  // for positive shifts

      for (int i=m_totalNumberOfBins-1; i>0; --i) {
        if (i-j<0) break;
        m_highThresholdDiscriminator[i]=m_highThresholdDiscriminator[i-j];
      }
      for (int i=0; i<j; ++i) if (i<m_totalNumberOfBins) m_highThresholdDiscriminator[i]=0; // the first j bins are set to zero

  }

  //std::cout << "AJB " << getRegion(hitID) << " ";
  //for (int i=0; i<m_totalNumberOfBins; ++i) std::cout <<  m_highThresholdDiscriminator[i];
  //std::cout << std::endl;

  return;

}
//_____________________________________________________________________________

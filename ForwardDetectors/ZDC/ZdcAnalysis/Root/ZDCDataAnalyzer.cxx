/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <ZdcAnalysis/ZDCDataAnalyzer.h>
#include <ZdcAnalysis/ZDCPulseAnalyzer.h>

#include <sstream>

ZDCDataAnalyzer::ZDCDataAnalyzer(int nSample, float deltaTSample, size_t preSampleIdx, std::string fitFunction,
				 const ZDCModuleFloatArray& peak2ndDerivMinSamples, 
				 const ZDCModuleFloatArray& peak2ndDerivMinThresholdsHG, 
				 const ZDCModuleFloatArray& peak2ndDerivMinThresholdsLG,
				 bool forceLG) : 
  _nSample(nSample), _deltaTSample(deltaTSample), _preSampleIdx(preSampleIdx),
  _fitFunction(fitFunction),
  _forceLG(forceLG),
  _debugLevel(-1),
  _eventCount(0),
  _moduleMask(0),
  _moduleSum({0, 0}), 
  _moduleSumErrSq({0, 0}), 
  _moduleSumPreSample({0,0}),
  _calibModuleSum({0, 0}), 
  _calibModuleSumErrSq({0,0}),
  _averageTime({0, 0}), 
  _fail({false, false}),
  _haveT0Calib(false),
  _haveECalib(false),
  _currentLB(-1)
{
  _moduleDisabled[0] = {false, false, false, false};
  _moduleDisabled[1] = {false, false, false, false};

  _moduleAnalyzers[0] = {0, 0, 0, 0};
  _moduleAnalyzers[1] = {0, 0, 0, 0};

  _calibAmplitude[0] = {0, 0, 0, 0};
  _calibAmplitude[1] = {0, 0, 0, 0};
  
  _calibTime[0] = {0, 0, 0, 0};
  _calibTime[1] = {0, 0, 0, 0};

  // For now we are using hard-coded gain factors and pedestals
  //
  _HGGains[0] = {9.51122, 9.51980, 9.51122, 9.51122};
  _HGGains[1] = {9.50842, 9.49662, 9.50853, 9.50842};
  
  _pedestals[0] = {100, 100, 100, 100};
  _pedestals[1] = {100, 100, 100, 100};
  
  // Default "calibrations"
  //
  _currentECalibCoeff = {1, 1, 1, 1, 1, 1, 1, 1};

  _currentT0OffsetsHG = {0, 0, 0, 0, 0, 0, 0, 0};
  _currentT0OffsetsLG = {0, 0, 0, 0, 0, 0, 0, 0};

  // Construct the per-module pulse analyzers
  //
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      std::ostringstream moduleTag;
      moduleTag << "_s" << side << "_m" << module;
      
      _moduleAnalyzers[side][module] = new ZDCPulseAnalyzer(moduleTag.str().c_str(), _nSample, _deltaTSample, _preSampleIdx, 
							    _pedestals[side][module], _HGGains[side][module], _fitFunction, 
							    peak2ndDerivMinSamples[side][module], 
							    peak2ndDerivMinThresholdsHG[side][module],
							    peak2ndDerivMinThresholdsLG[side][module]);
      if (_forceLG) _moduleAnalyzers[side][module]->SetForceLG(true);
    }
  }

  // By default we perform quiet pulse fits
  //
  ZDCPulseAnalyzer::SetQuietFits(true);
}

ZDCDataAnalyzer::~ZDCDataAnalyzer() 
{
  //  Delete the per-module pulse analyzers
  //
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      if (!_moduleAnalyzers[side][module]) delete _moduleAnalyzers[side][module];
    }
  }
}

bool ZDCDataAnalyzer::DisableModule(size_t side, size_t module)
{
  if (side < 2 && module < 4) {
    //
    // Can't disable in the middle of analysis
    //
    if (_dataLoaded[side][module]) return false;
    else {
      _moduleDisabled[side][module] = true;
      return true;
    }
  }
  else {
    return false;
  }
}

void ZDCDataAnalyzer::EnableDelayed(float deltaT, const ZDCModuleFloatArray& undelayedDelayedPedestalDiff)
{
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      _moduleAnalyzers[side][module]->EnableDelayed(deltaT, undelayedDelayedPedestalDiff[side][module]);
    }
  }
}

void ZDCDataAnalyzer::SetPeak2ndDerivMinTolerances(size_t tolerance) {
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      _moduleAnalyzers[side][module]->SetPeak2ndDerivMinTolerance(tolerance);
    }
  }
}

void ZDCDataAnalyzer::SetTauT0Values(const ZDCModuleBoolArray& fixTau1, const ZDCModuleBoolArray& fixTau2, 
				     const ZDCModuleFloatArray& tau1, const ZDCModuleFloatArray& tau2, 
				     const ZDCModuleFloatArray& t0HG, const ZDCModuleFloatArray& t0LG)
{
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      _moduleAnalyzers[side][module]->SetTauT0Values(fixTau1[side][module], fixTau2[side][module], 
						     tau1[side][module], tau2[side][module], t0HG[side][module], t0LG[side][module]);
    }
  }
}

void ZDCDataAnalyzer::SetADCOverUnderflowValues(const ZDCModuleFloatArray& HGOverflowADC, const ZDCModuleFloatArray& HGUnderflowADC, 
						const ZDCModuleFloatArray& LGOverflowADC)
{
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      _moduleAnalyzers[side][module]->SetADCOverUnderflowValues(HGOverflowADC[side][module], HGUnderflowADC[side][module], LGOverflowADC[side][module]);
    }
  }
}

void ZDCDataAnalyzer::SetCutValues(const ZDCModuleFloatArray& chisqDivAmpCutHG, const ZDCModuleFloatArray& chisqDivAmpCutLG,
				   const ZDCModuleFloatArray& deltaT0MinHG, const ZDCModuleFloatArray& deltaT0MaxHG, 
				   const ZDCModuleFloatArray&  deltaT0MinLG, const ZDCModuleFloatArray& deltaT0MaxLG)
{
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      _moduleAnalyzers[side][module]->SetCutValues(chisqDivAmpCutHG[side][module], chisqDivAmpCutLG[side][module],
						   deltaT0MinHG[side][module], deltaT0MaxHG[side][module], 
						   deltaT0MinLG[side][module], deltaT0MaxLG[side][module]);
    }
  }
}

void ZDCDataAnalyzer::SetTimingCorrParams(const std::array<std::array<std::vector<float>, 4>, 2>& HGParamArr,
					  const std::array<std::array<std::vector<float>, 4>, 2>& LGParamArr)
{
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      _moduleAnalyzers[side][module]->SetTimingCorrParams(HGParamArr.at(side).at(module), LGParamArr.at(side).at(module)); 
    }
  }

}

void ZDCDataAnalyzer::SetNonlinCorrParams(const std::array<std::array<std::vector<float>, 4>, 2>& HGNonlinCorrParams)
{
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      _moduleAnalyzers[side][module]->SetNonlinCorrParams(HGNonlinCorrParams.at(side).at(module));
    }
  }
}

void ZDCDataAnalyzer::StartEvent(int lumiBlock) 
{
  if (_debugLevel > 0) {
    std::cout << "Starting new event, event index = " << _eventCount << std::endl;
  }

  //  See if we have to load up new calibrations
  //
  if (lumiBlock != _currentLB) {
    if (_debugLevel > 0) {
      std::cout << "Starting new luminosity block " << lumiBlock << std::endl;
    }

    if (_haveECalib) {
      if (_debugLevel > 1) {
	std::cout << "Loading energy calibrations for event " << _eventCount << ", lumi block " << lumiBlock << std::endl;
      }

      for (size_t side : {0, 1}) {
	for (size_t module : {0, 1, 2, 3}) {
	  float splineLBMin = _LBDepEcalibSplines[side][module]->GetXmin();
	  float splineLBMax = _LBDepEcalibSplines[side][module]->GetXmax();
	  
	  if (lumiBlock >= splineLBMin && lumiBlock <= splineLBMax) { 
	    _currentECalibCoeff[side][module] = _LBDepEcalibSplines[side][module]->Eval(lumiBlock);
	  }
	  else if (lumiBlock < splineLBMin) {
	    _currentECalibCoeff[side][module] = _LBDepEcalibSplines[side][module]->Eval(splineLBMin);
	  }
	  else {
	    _currentECalibCoeff[side][module] = _LBDepEcalibSplines[side][module]->Eval(splineLBMax);
	  }
	}
      }
    } // end of if (_haveEcalib) {

    if (_haveT0Calib) {
      if (_debugLevel > 1) {
	std::cout << "Loading timing calibrations for event " << _eventCount << ", lumi block " << lumiBlock << std::endl;
      }

      for (size_t side : {0, 1}) {
	for (size_t module : {0, 1, 2, 3}) {
	  float splineLBMin = _T0HGOffsetSplines[side][module]->GetXmin();
	  float splineLBMax = _T0HGOffsetSplines[side][module]->GetXmax();
	  
	  if (lumiBlock >= splineLBMin && lumiBlock <= splineLBMax) { 
	    _currentT0OffsetsHG[side][module] = _T0HGOffsetSplines[side][module]->Eval(lumiBlock);
	    _currentT0OffsetsLG[side][module] = _T0LGOffsetSplines[side][module]->Eval(lumiBlock);
	  }
	  else if (lumiBlock < splineLBMin) {
	    _currentT0OffsetsHG[side][module] = _T0HGOffsetSplines[side][module]->Eval(splineLBMin);
	    _currentT0OffsetsLG[side][module] = _T0LGOffsetSplines[side][module]->Eval(splineLBMin);
	  }
	  else {
	    _currentT0OffsetsHG[side][module] = _T0HGOffsetSplines[side][module]->Eval(splineLBMax);
	    _currentT0OffsetsLG[side][module] = _T0LGOffsetSplines[side][module]->Eval(splineLBMax);
	  }
	}
      }
    } // end of if (_haveT0Calib)
  }

  // Initialize transient results
  //
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      _dataLoaded[side][module] = false;
      _moduleStatus[side][module] = 0;
      _calibAmplitude[side][module] = 0;
      _calibTime[side][module] = 0;
      //      _moduleFail[side][module] = false;
    }

    _moduleSum[side] = 0;
    _moduleSumErrSq[side] = 0;
    _moduleSumPreSample[side] = 0;

    _calibModuleSum[side] = 0;
    _calibModuleSumErrSq[side] = 0;

    _averageTime[side] = 0;
    _fail[side] = false;
  }

  _moduleMask = 0;
  _currentLB = lumiBlock;
}

void ZDCDataAnalyzer::LoadAndAnalyzeData(size_t side, size_t module, const std::vector<float> HGSamples, const std::vector<float> LGSamples)
{
  // We immediately return if this module is disabled
  //
  if (_moduleDisabled[side][module]) {
    if (_debugLevel > 2) {
      std::cout << "Skipping analysis of disabled mofule for event index " << _eventCount << ", side, module = " << side << ", " << module << std::endl;
    }

    return;
  }

  if (_debugLevel > 1) {
    std::cout << "/n Loading data for event index " << _eventCount << ", side, module = " << side << ", " << module << std::endl;
    
    if (_debugLevel > 2) {
      std::cout << " Number of HG and LG samples = " << HGSamples.size() << ", " << LGSamples.size() << std::endl;
      if (_debugLevel > 3) {
	for (size_t sample = 0; sample < HGSamples.size(); sample++) {
	  std::cout << "HGSample[" << sample << "] = " << HGSamples[sample] << std::endl;
	}
      }
    }
  }

  ZDCPulseAnalyzer* pulseAna_p = _moduleAnalyzers[side][module];

  bool result = pulseAna_p->LoadAndAnalyzeData(HGSamples, LGSamples);
  _dataLoaded[side][module] = true;

  if (result) {
    if (!pulseAna_p->BadT0() && !pulseAna_p->BadChisq()) {
      int moduleMaskBit = 4*side + module;
      _moduleMask |= 1<< moduleMaskBit;
      
      float amplitude = pulseAna_p->GetAmplitude();
      float ampError = pulseAna_p->GetAmpError();

      _calibAmplitude[side][module] = amplitude*_currentECalibCoeff[side][module];

      float calibAmpError = ampError * _currentECalibCoeff[side][module];

      float timeCalib = pulseAna_p->GetT0Corr();
      if (pulseAna_p->UseLowGain()) timeCalib -= _currentT0OffsetsLG[side][module];
      else timeCalib -= _currentT0OffsetsHG[side][module];

      _calibTime[side][module] = timeCalib;

      _moduleSum[side] += amplitude;
      _moduleSumErrSq[side] += ampError*ampError;

      _moduleSumPreSample[side] += pulseAna_p->GetPreSampleAmp();

      _calibModuleSum[side] += _calibAmplitude[side][module];
      _calibModuleSumErrSq[side] += calibAmpError*calibAmpError;

      _averageTime[side] += _calibTime[side][module]*_calibAmplitude[side][module];
    }
  }
  else {
    if (pulseAna_p->Failed()) {
      if (_debugLevel >= 0) {
	std::cout << "ZDCPulseAnalyzer::LoadData() returned fail for event " << _eventCount << ", side, module = " << side << ", " << module << std::endl;
      }

      _fail[side] = true;
    }
  }

  _moduleStatus[side][module] = pulseAna_p->GetStatusMask();
}

void ZDCDataAnalyzer::LoadAndAnalyzeData(size_t side, size_t module, const std::vector<float> HGSamples, const std::vector<float> LGSamples,
					 const std::vector<float> HGSamplesDelayed, const std::vector<float> LGSamplesDelayed)
{
  // We immediately return if this module is disabled
  //
  if (_moduleDisabled[side][module]) {
    if (_debugLevel > 2) {
      std::cout << "Skipping analysis of disabled mofule for event index " << _eventCount << ", side, module = " << side << ", " << module << std::endl;
    }

    return;
  }

  if (_debugLevel > 1) {
    std::cout << "Loading undelayed and delayed data for event index " << _eventCount << ", side, module = " << side << ", " << module << std::endl;
    
    if (_debugLevel > 2) {
      std::cout << " Number of HG and LG samples = " << HGSamples.size() << ", " << LGSamples.size() << std::endl;
      if (_debugLevel > 3) {
	for (size_t sample = 0; sample < HGSamples.size(); sample++) {
	  std::cout << "HGSample[" << sample << "] = " << HGSamples[sample] << std::endl;
	}

	for (size_t sample = 0; sample < HGSamples.size(); sample++) {
	  std::cout << "HGSampleDelayed[" << sample << "] = " << HGSamplesDelayed[sample] << std::endl;
	}
      }

    }
  }

  ZDCPulseAnalyzer* pulseAna_p = _moduleAnalyzers[side][module];

  bool result = pulseAna_p->LoadAndAnalyzeData(HGSamples, LGSamples, HGSamplesDelayed, LGSamplesDelayed);
  _dataLoaded[side][module] = true;

  if (result) {
    if (!pulseAna_p->BadT0() && !pulseAna_p->BadChisq()) {
      int moduleMaskBit = 4*side + module;
      _moduleMask |= 1<< moduleMaskBit;
      
      float amplitude = pulseAna_p->GetAmplitude();
      float ampError = pulseAna_p->GetAmpError();

      _calibAmplitude[side][module] = amplitude*_currentECalibCoeff[side][module];

      float calibAmpError = ampError * _currentECalibCoeff[side][module];

      float timeCalib = pulseAna_p->GetT0Corr();
      if (pulseAna_p->UseLowGain()) timeCalib -= _currentT0OffsetsLG[side][module];
      else timeCalib -= _currentT0OffsetsHG[side][module];

      _calibTime[side][module] = timeCalib;

      _moduleSum[side] += amplitude;
      _moduleSumErrSq[side] += ampError*ampError;

      _moduleSumPreSample[side] += pulseAna_p->GetPreSampleAmp();

      _calibModuleSum[side] += _calibAmplitude[side][module];
      _calibModuleSumErrSq[side] += calibAmpError*calibAmpError;

      _averageTime[side] += _calibTime[side][module]*_calibAmplitude[side][module];
    }
  }
  else {
    if (pulseAna_p->Failed()) {
      if (_debugLevel >= 0) {
	std::cout << "ZDCPulseAnalyzer::LoadData() returned fail for event " << _eventCount << ", side, module = " << side << ", " << module << std::endl;
      }

      _fail[side] = true;
    }
  }

  _moduleStatus[side][module] = pulseAna_p->GetStatusMask();
}

bool ZDCDataAnalyzer::FinishEvent()
{
  // First make sure that all data is loaded
  //
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      if (!_dataLoaded[side][module] && !_moduleDisabled[side][module]) return false;
    }

    // Divide the average times by the calibrated module sums  
    //
    if (_calibModuleSum[side] > 1e-6) {
      _averageTime[side] /= _calibModuleSum[side]; 
    }
    else {
      _averageTime[side] = 0;
    }
  }

  _eventCount++;
  return true;
}

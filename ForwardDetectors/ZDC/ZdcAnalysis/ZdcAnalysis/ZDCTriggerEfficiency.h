/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCANALYSIS_ZDCTRIGGEREFFICIENCY_H
#define ZDCANALYSIS_ZDCTRIGGEREFFICIENCY_H

#include <TSpline.h>
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>

class MsgStream;

class ZDCTriggerEfficiency{
  public:
	bool _haveParams;
  bool _haveCorrCoeffs;

  unsigned int _minLB;
  unsigned int _maxLB;

  unsigned int _currentLB;

  std::array<std::vector<TSpline3*>, 2> effParams;
  std::array<std::vector<TSpline3*>, 2> effParamErrors;
  std::array<std::vector<TSpline3*>, 2> effParamCorrCoeffs;

  std::array<std::vector<double>, 2> currentParams;
  std::array<std::vector<double>, 2> currentParamErrors;
  std::array<std::vector<double>, 2> currentCorrCoefff;

	void UpdatelumiBlock(unsigned int lumiBlock)
	{
		if (!_haveParams) throw std::runtime_error("No params for trigger efficiency.");

		if (lumiBlock != _currentLB) {
			int newLB = std::max(std::min(lumiBlock, _maxLB), _minLB);
			for (int side : {0, 1}) {
				currentParams[side].clear();
				currentParamErrors[side].clear();
				currentCorrCoefff[side].clear();

				for (size_t ipar = 0; ipar < effParams[side].size(); ipar++) {
					currentParams[side].push_back(effParams[side][ipar]->Eval(newLB));
					currentParamErrors[side].push_back(effParamErrors[side][ipar]->Eval(newLB));

					if (_haveCorrCoeffs) {
						currentCorrCoefff[side].push_back(effParamCorrCoeffs[side][ipar]->Eval(newLB));
					}
				}
				_currentLB = lumiBlock;
			}
		}
	}

public:
 ZDCTriggerEfficiency() : // in order, alpha-beta, alpha-theta, beta-theta
  _haveParams(false), _haveCorrCoeffs(false),
    _minLB(0), _maxLB(0), _currentLB(0)
    {

	}

  void SetEffParamsAndErrors(std::array<std::vector<TSpline3*>, 2> effParams,
			     std::array<std::vector<TSpline3*>, 2> effParamErrors)
  {
    for (int side : {0, 1}) {
      effParams[side] = effParams[side];
      effParamErrors[side] = effParamErrors[side];
    }

    _minLB = effParams[0][0]->GetXmin();
    _maxLB = effParams[0][0]->GetXmax();
    _haveParams = true;
  }

  void SetEffParamCorrCoeffs(std::array<std::vector<TSpline3*>, 2> effParamsCorrCoeffs)
  {
    for (int side : {0, 1}) {
      effParamCorrCoeffs[side] = effParamsCorrCoeffs[side];
    }

    _haveCorrCoeffs = true;
  }

  double GetEfficiency(int side, float ADCSum);

  std::pair<double, double> GetEfficiencyAndError(MsgStream& msg, int side, float ADCSum);

};

#endif

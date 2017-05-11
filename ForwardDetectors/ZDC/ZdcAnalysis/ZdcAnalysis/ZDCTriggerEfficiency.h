/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCANALYSIS_ZDCTRIGGEREFFICIENCY_H
#define ZDCANALYSIS_ZDCTRIGGEREFFICIENCY_H

#include <TSpline.h>
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
class ZDCTriggerEfficiency{
  public:
	bool _haveParams;
  bool _haveCorrCoeffs;

  unsigned int _minLB;
  unsigned int _maxLB;

  unsigned int _currentLB;

  std::array<std::vector<TSpline3*>, 2> _effParams;
  std::array<std::vector<TSpline3*>, 2> _effParamErrors;
  std::array<std::vector<TSpline3*>, 2> _effParamCorrCoeffs;

  std::array<std::vector<double>, 2> _currentParams;
  std::array<std::vector<double>, 2> _currentParamErrors;
  std::array<std::vector<double>, 2> _currentCorrCoefff;

	void UpdatelumiBlock(unsigned int lumiBlock) 
	{
		if (!_haveParams) throw;

		if (lumiBlock != _currentLB) {
			int newLB = std::max(std::min(lumiBlock, _maxLB), _minLB);
			for (int side : {0, 1}) { 
				_currentParams[side].clear();
				_currentParamErrors[side].clear();
				_currentCorrCoefff[side].clear();

				for (size_t ipar = 0; ipar < _effParams[side].size(); ipar++) {
					_currentParams[side].push_back(_effParams[side][ipar]->Eval(newLB));
					_currentParamErrors[side].push_back(_effParamErrors[side][ipar]->Eval(newLB));

					if (_haveCorrCoeffs) {
						_currentCorrCoefff[side].push_back(_effParamCorrCoeffs[side][ipar]->Eval(newLB));
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
      _effParams[side] = effParams[side];
      _effParamErrors[side] = effParamErrors[side];
      for (int iarr=0;iarr<3;iarr++)
	{
	  _effParams[side].at(iarr)->Print();
	  _effParamErrors[side].at(iarr)->Print();
	}
    }

    _minLB = _effParams[0][0]->GetXmin();
    _maxLB = _effParams[0][0]->GetXmax();
    _haveParams = true;
  }

  void SetEffParamCorrCoeffs(std::array<std::vector<TSpline3*>, 2> effParamsCorrCoeffs) 
  {
    for (int side : {0, 1}) { 
      _effParamCorrCoeffs[side] = effParamsCorrCoeffs[side];
      for (int iarr=0;iarr<3;iarr++)
	{
	  _effParamCorrCoeffs[side].at(iarr)->Print();
	}
    }

    _haveCorrCoeffs = true;
  }

  float GetEfficiency(int side, float ADCSum);

  std::pair<float, float> GetEfficiencyAndError(int side, float ADCSum);

};

#endif

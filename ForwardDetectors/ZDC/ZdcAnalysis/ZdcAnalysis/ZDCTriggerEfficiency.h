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

class ZDCTriggerEfficiency {
private:
  bool m_haveParams;
  bool m_haveCorrCoeffs;

  unsigned int m_minLB;
  unsigned int m_maxLB;

  unsigned int m_currentLB;

  std::array<std::vector<TSpline3*>, 2> m_effParams;
  std::array<std::vector<TSpline3*>, 2> m_effParamErrors;
  std::array<std::vector<TSpline3*>, 2> m_effParamCorrCoeffs;

  std::array<std::vector<double>, 2> m_currentParams;
  std::array<std::vector<double>, 2> m_currentParamErrors;
  std::array<std::vector<double>, 2> m_currentCorrCoefff;

public:
  void UpdatelumiBlock(unsigned int lumiBlock)
  {
    if (!m_haveParams) throw std::runtime_error("No params for trigger efficiency.");

    if (lumiBlock != m_currentLB) {
      int newLB = std::max(std::min(lumiBlock, m_maxLB), m_minLB);
      for (int side : {0, 1}) {
        m_currentParams[side].clear();
        m_currentParamErrors[side].clear();
        m_currentCorrCoefff[side].clear();

        for (size_t ipar = 0; ipar < m_effParams[side].size(); ipar++) {
          m_currentParams[side].push_back(m_effParams[side][ipar]->Eval(newLB));
          m_currentParamErrors[side].push_back(m_effParamErrors[side][ipar]->Eval(newLB));

          if (m_haveCorrCoeffs) {
            m_currentCorrCoefff[side].push_back(m_effParamCorrCoeffs[side][ipar]->Eval(newLB));
          }
        }
        m_currentLB = lumiBlock;
      }
    }
  }

  ZDCTriggerEfficiency() : // in order, alpha-beta, alpha-theta, beta-theta
    m_haveParams(false), m_haveCorrCoeffs(false),
    m_minLB(0), m_maxLB(0), m_currentLB(0) {

  }

  void SetEffParamsAndErrors(std::array<std::vector<TSpline3*>, 2> effParams,
                             std::array<std::vector<TSpline3*>, 2> effParamErrors) {
    for (int side : {0, 1}) {
      m_effParams[side] = effParams[side];
      m_effParamErrors[side] = effParamErrors[side];
    }

    m_minLB = m_effParams[0][0]->GetXmin();
    m_maxLB = m_effParams[0][0]->GetXmax();
    m_haveParams = true;
  }

  void SetEffParamCorrCoeffs(std::array<std::vector<TSpline3*>, 2> effParamsCorrCoeffs) {
    for (int side : {0, 1}) {
      m_effParamCorrCoeffs[side] = effParamsCorrCoeffs[side];
    }

    m_haveCorrCoeffs = true;
  }

  double GetEfficiency(int side, float ADCSum);

  std::pair<double, double> GetEfficiencyAndError(MsgStream& msg, int side, float ADCSum);

};

#endif

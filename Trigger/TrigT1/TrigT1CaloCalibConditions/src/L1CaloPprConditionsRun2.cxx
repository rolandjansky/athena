/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprConditionsRun2.h"

#include <iostream>
#include <cmath>

L1CaloPprConditionsRun2::L1CaloPprConditionsRun2(unsigned short extBcidThreshold,
                                                 unsigned short satBcidThreshLow,
                                                 unsigned short satBcidThreshHigh,
                                                 unsigned short satBcidLevel,
                                                 unsigned short bcidEnergyRangeLow,
                                                 unsigned short bcidEnergyRangeHigh,
                                                 unsigned short firStartBit,
                                                 short int firCoeff1,
                                                 short int firCoeff2,
                                                 short int firCoeff3,
                                                 short int firCoeff4,
                                                 short int firCoeff5,
                                                 unsigned short lutCpStrategy,
                                                 unsigned short lutCpOffset,
                                                 unsigned short lutCpSlope,
                                                 unsigned short lutCpNoiseCut,
                                                 short lutCpPar1,
                                                 short lutCpPar2,
                                                 short lutCpPar3,
                                                 short lutCpPar4,
                                                 unsigned short lutCpScale,
                                                 unsigned short lutJepStrategy,
                                                 unsigned short lutJepOffset,
                                                 unsigned short lutJepSlope,
                                                 unsigned short lutJepNoiseCut,
                                                 short lutJepPar1,
                                                 short lutJepPar2,
                                                 short lutJepPar3,
                                                 short lutJepPar4,
                                                 unsigned short lutJepScale,
                                                 unsigned int pedValue,
                                                 float pedMean,
                                                 unsigned int pedFirSum)
: m_extBcidThreshold(extBcidThreshold),
  m_satBcidThreshLow(satBcidThreshLow),
  m_satBcidThreshHigh(satBcidThreshHigh),
  m_satBcidLevel(satBcidLevel),
  m_bcidEnergyRangeLow(bcidEnergyRangeLow),
  m_bcidEnergyRangeHigh(bcidEnergyRangeHigh),
  m_firStartBit(firStartBit),
  m_vFirCoefficients({firCoeff1, firCoeff2, firCoeff3, firCoeff4, firCoeff5}),
  m_lutCpStrategy(lutCpStrategy),
  m_lutCpOffset(lutCpOffset),
  m_lutCpSlope(lutCpSlope),
  m_lutCpNoiseCut(lutCpNoiseCut),
  m_lutCpPar1(lutCpPar1),
  m_lutCpPar2(lutCpPar2),
  m_lutCpPar3(lutCpPar3),
  m_lutCpPar4(lutCpPar4),
  m_lutCpScale(lutCpScale),
  m_lutJepStrategy(lutJepStrategy),
  m_lutJepOffset(lutJepOffset),
  m_lutJepSlope(lutJepSlope),
  m_lutJepNoiseCut(lutJepNoiseCut),
  m_lutJepPar1(lutJepPar1),
  m_lutJepPar2(lutJepPar2),
  m_lutJepPar3(lutJepPar3),
  m_lutJepPar4(lutJepPar4),
  m_lutJepScale(lutJepScale),
  m_pedValue(pedValue),
  m_pedMean(pedMean),
  m_pedFirSum(pedFirSum)
{
}

namespace {
unsigned short getLutOffset(double pedMean, unsigned short firStartBit,
                            std::vector<short int> firCoeff,
                            unsigned short lutSlope,
                            unsigned short lutStrategy) {
  unsigned short lutOffset = 0;
  int firCoeffSum = 0;
  for (unsigned int i = 0; i < firCoeff.size(); i++) {
    firCoeffSum += firCoeff.at(i);
  }
  float lutOffsetReal = 0;
  if (lutStrategy == 0) {
    lutOffsetReal = (pedMean * static_cast<float>(firCoeffSum) /
                     std::pow(2., static_cast<float>(firStartBit)));
  } else {
    lutOffsetReal = (pedMean * static_cast<float>(firCoeffSum) *
                         static_cast<float>(lutSlope) /
                         std::pow(2., static_cast<float>(firStartBit)) -
                     static_cast<float>(lutSlope) / 2.0);
  }
  lutOffset =
      static_cast<unsigned short>(lutOffsetReal < 0. ? 0 : lutOffsetReal + 0.5);
  return lutOffset;
}
}

void L1CaloPprConditionsRun2::initializeByStrategy(unsigned short firStartBit, short int firCoeff1,
    short int firCoeff2, short int firCoeff3, short int firCoeff4,
    short int firCoeff5, unsigned short lutCpSlope, unsigned short lutCpNoiseCut,
    unsigned short lutJepSlope, unsigned short lutJepNoiseCut)
{
  m_firStartBit = firStartBit;
  m_vFirCoefficients = {firCoeff1, firCoeff2, firCoeff3, firCoeff4, firCoeff5},
  m_lutCpSlope = lutCpSlope;
  m_lutCpNoiseCut = lutCpNoiseCut;
  m_lutJepSlope = lutJepSlope;
  m_lutJepNoiseCut = lutJepNoiseCut;

  m_lutCpOffset = getLutOffset(m_pedMean, m_firStartBit, m_vFirCoefficients,m_lutCpSlope, m_lutCpStrategy);
  m_lutJepOffset = getLutOffset(m_pedMean, m_firStartBit, m_vFirCoefficients,m_lutJepSlope, m_lutJepStrategy);
}

std::ostream& operator<<(std::ostream& output, const  L1CaloPprConditionsRun2& r) {
  output << "extBcidThreshold: "    << r.m_extBcidThreshold       << ", "
         << "satBcidThreshLow: "    << r.m_satBcidThreshLow       << ", "
         << "satBcidThreshHigh: "   << r.m_satBcidThreshHigh      << ", "
         << "bcidEnergyRangeLow: "  << r.m_bcidEnergyRangeLow     << ", "
         << "bcidEnergyRangeHigh: " << r.m_bcidEnergyRangeHigh    << std::endl
         << "firStartBit: "         << r.m_firStartBit            << ", "
         << "firCoeff1: "           << r.m_vFirCoefficients.at(0) << ", "
         << "firCoeff2: "           << r.m_vFirCoefficients.at(1) << ", "
         << "firCoeff3: "           << r.m_vFirCoefficients.at(2) << ", "
         << "firCoeff4: "           << r.m_vFirCoefficients.at(3) << ", "
         << "firCoeff5: "           << r.m_vFirCoefficients.at(4) << std::endl
         << "lutCpStrategy: "       << r.m_lutCpStrategy          << ", "
         << "lutCpOffset: "         << r.m_lutCpOffset            << ", "
         << "lutCpSlope: "          << r.m_lutCpSlope             << ", "
         << "lutCpNoiseCut: "       << r.m_lutCpNoiseCut          << ", "
         << "lutCpScale: "          << r.m_lutCpScale             << std::endl
         << "lutCpPar1:"            << r.m_lutCpPar1              << ", "
         << "lutCpPar2:"            << r.m_lutCpPar2              << ", "
         << "lutCpPar3:"            << r.m_lutCpPar3              << ", "
         << "lutCpPar4:"            << r.m_lutCpPar4              << std::endl
         << "lutJepStrategy: "      << r.m_lutJepStrategy         << ", "
         << "lutJepOffset: "        << r.m_lutJepOffset           << ", "
         << "lutJepSlope: "         << r.m_lutJepSlope            << ", "
         << "lutJepNoiseCut: "      << r.m_lutJepNoiseCut         << ", "
         << "lutJepScale: "         << r.m_lutJepScale            << std::endl
         << "lutJepPar1:"           << r.m_lutJepPar1             << ", "
         << "lutJepPar2:"           << r.m_lutJepPar2             << ", "
         << "lutJepPar3:"           << r.m_lutJepPar3             << ", "
         << "lutJepPar4:"           << r.m_lutJepPar4             << std::endl
         << "pedValue: "            << r.m_pedValue               << ", "
         << "pedMean: "             << r.m_pedMean                << ", "
         << "pedFirSum: "           << r.m_pedFirSum              << std::endl;
  return output;
}

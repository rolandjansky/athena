/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelChargeCalibCondData.h"

PixelChargeCalibCondData::PixelChargeCalibCondData():
  m_analogThreshold(),
  m_analogThresholdSigma(),
  m_analogThresholdNoise(),
  m_intimethreshold(),
  m_totA(),
  m_totE(),
  m_totC(),
  m_totRes1(),
  m_totRes2(),
  m_analogThresholdLong(),
  m_analogThresholdSigmaLong(),
  m_analogThresholdNoiseLong(),
  m_intimethresholdLong(),
  m_totALong(),
  m_totELong(),
  m_totCLong(),
  m_analogThresholdGanged(),
  m_analogThresholdSigmaGanged(),
  m_analogThresholdNoiseGanged(),
  m_intimethresholdGanged()
{
}

PixelChargeCalibCondData::~PixelChargeCalibCondData() { }

// Normal pixels
void PixelChargeCalibCondData::setAnalogThreshold(const int chanNum, const int value) {
  m_analogThreshold[chanNum].push_back(value);
}

void PixelChargeCalibCondData::setAnalogThresholdSigma(const int chanNum, const int value) {
  m_analogThresholdSigma[chanNum].push_back(value);
}

void PixelChargeCalibCondData::setAnalogThresholdNoise(const int chanNum, const int value) {
  m_analogThresholdNoise[chanNum].push_back(value);
}

void PixelChargeCalibCondData::setInTimeThreshold(const int chanNum, const int value) {
  m_intimethreshold[chanNum].push_back(value);
}

void PixelChargeCalibCondData::setQ2TotA(const int chanNum, const float value) {
  m_totA[chanNum].push_back(value);
}

void PixelChargeCalibCondData::setQ2TotE(const int chanNum, const float value) {
  m_totE[chanNum].push_back(value);
}

void PixelChargeCalibCondData::setQ2TotC(const int chanNum, const float value) {
  m_totC[chanNum].push_back(value);
}

void PixelChargeCalibCondData::setTotRes1(const int chanNum, const float value) {
  m_totRes1[chanNum].push_back(value);
}

void PixelChargeCalibCondData::setTotRes2(const int chanNum, const float value) {
  m_totRes2[chanNum].push_back(value);
}

int PixelChargeCalibCondData::getAnalogThreshold(const int chanNum, const int FE, const int type) const {
  if (type==PixelType::NORMAL) {
    auto itr = m_analogThreshold.find(chanNum);
    if (itr!=m_analogThreshold.end()) {
      const std::vector<int>& chip = itr->second;
      if (FE<(int)chip.size()) {
        return chip.at(FE);
      }
    }
  }
  else if (type==PixelType::LONG) {
    auto itr = m_analogThresholdLong.find(chanNum);
    if (itr!=m_analogThresholdLong.end()) {
      const std::vector<int>& chip = itr->second;
      if (FE<(int)chip.size()) {
        return chip.at(FE);
      }
    }
  } 
  else if (type==PixelType::GANGED) {
    auto itr = m_analogThresholdGanged.find(chanNum);
    if (itr!=m_analogThresholdGanged.end()) {
      const std::vector<int>& chip = itr->second;
      if (FE<(int)chip.size()) {
        return chip.at(FE);
      }
    }
  }
  return 0;
}

int PixelChargeCalibCondData::getAnalogThresholdSigma(const int chanNum, const int FE, const int type) const {
  if (type==PixelType::NORMAL) {
    auto itr = m_analogThresholdSigma.find(chanNum);
    if (itr!=m_analogThresholdSigma.end()) {
      const std::vector<int>& chip = itr->second;
      if (FE<(int)chip.size()) {
        return chip.at(FE);
      }
    }
  }
  else if (type==PixelType::LONG) {
    auto itr = m_analogThresholdSigmaLong.find(chanNum);
    if (itr!=m_analogThresholdSigmaLong.end()) {
      const std::vector<int>& chip = itr->second;
      if (FE<(int)chip.size()) {
        return chip.at(FE);
      }
    }
  } 
  else if (type==PixelType::GANGED) {
    auto itr = m_analogThresholdSigmaGanged.find(chanNum);
    if (itr!=m_analogThresholdSigmaGanged.end()) {
      const std::vector<int>& chip = itr->second;
      if (FE<(int)chip.size()) {
        return chip.at(FE);
      }
    }
  }
  return 0;
}

int PixelChargeCalibCondData::getAnalogThresholdNoise(const int chanNum, const int FE, const int type) const {
  if (type==PixelType::NORMAL) {
    auto itr = m_analogThresholdNoise.find(chanNum);
    if (itr!=m_analogThresholdNoise.end()) {
      const std::vector<int>& chip = itr->second;
      if (FE<(int)chip.size()) {
        return chip.at(FE);
      }
    }
  }
  else if (type==PixelType::LONG) {
    auto itr = m_analogThresholdNoiseLong.find(chanNum);
    if (itr!=m_analogThresholdNoiseLong.end()) {
      const std::vector<int>& chip = itr->second;
      if (FE<(int)chip.size()) {
        return chip.at(FE);
      }
    }
  } 
  else if (type==PixelType::GANGED) {
    auto itr = m_analogThresholdNoiseGanged.find(chanNum);
    if (itr!=m_analogThresholdNoiseGanged.end()) {
      const std::vector<int>& chip = itr->second;
      if (FE<(int)chip.size()) {
        return chip.at(FE);
      }
    }
  }
  return 0;
}

int PixelChargeCalibCondData::getInTimeThreshold(const int chanNum, const int FE, const int type) const {
  if (type==PixelType::NORMAL) {
    auto itr = m_intimethreshold.find(chanNum);
    if (itr!=m_intimethreshold.end()) {
      const std::vector<int>& chip = itr->second;
      if (FE<(int)chip.size()) {
        return chip.at(FE);
      }
    }
  }
  else if (type==PixelType::LONG) {
    auto itr = m_intimethresholdLong.find(chanNum);
    if (itr!=m_intimethresholdLong.end()) {
      const std::vector<int>& chip = itr->second;
      if (FE<(int)chip.size()) {
        return chip.at(FE);
      }
    }
  } 
  else if (type==PixelType::GANGED) {
    auto itr = m_intimethresholdGanged.find(chanNum);
    if (itr!=m_intimethresholdGanged.end()) {
      const std::vector<int>& chip = itr->second;
      if (FE<(int)chip.size()) {
        return chip.at(FE);
      }
    }
  }
  return 0;
}

float PixelChargeCalibCondData::getQ2TotA(const int chanNum, const int FE, const int type) const {
  if (type==PixelType::NORMAL || type==PixelType::LONG) {
    auto itr = m_totA.find(chanNum);
    if (itr!=m_totA.end()) {
      const std::vector<float>& chip = itr->second;
      if (FE<(int)chip.size()) {
        return chip.at(FE);
      }
    }
  }
  else if (type==PixelType::GANGED) {
    auto itr = m_totALong.find(chanNum);
    if (itr!=m_totALong.end()) {
      const std::vector<float>& chip = itr->second;
      if (FE<(int)chip.size()) {
        return chip.at(FE);
      }
    }
  }
  return 0.0;
}

float PixelChargeCalibCondData::getQ2TotE(const int chanNum, const int FE, const int type) const {
  if (type==PixelType::NORMAL || type==PixelType::LONG) {
    auto itr = m_totE.find(chanNum);
    if (itr!=m_totE.end()) {
      const std::vector<float>& chip = itr->second;
      if (FE<(int)chip.size()) {
        return chip.at(FE);
      }
    }
  }
  else if (type==PixelType::GANGED) {
    auto itr = m_totELong.find(chanNum);
    if (itr!=m_totELong.end()) {
      const std::vector<float>& chip = itr->second;
      if (FE<(int)chip.size()) {
        return chip.at(FE);
      }
    }
  }
  return 0.0;
}

float PixelChargeCalibCondData::getQ2TotC(const int chanNum, const int FE, const int type) const {
  if (type==PixelType::NORMAL || type==PixelType::LONG) {
    auto itr = m_totC.find(chanNum);
    if (itr!=m_totC.end()) {
      const std::vector<float>& chip = itr->second;
      if (FE<(int)chip.size()) {
        return chip.at(FE);
      }
    }
  }
  else if (type==PixelType::GANGED) {
    auto itr = m_totCLong.find(chanNum);
    if (itr!=m_totCLong.end()) {
      const std::vector<float>& chip = itr->second;
      if (FE<(int)chip.size()) {
        return chip.at(FE);
      }
    }
  }
  return 0.0;
}

float PixelChargeCalibCondData::getTotRes(const int chanNum, const int FE, float Q) const {
  float res1 = 0.0;
  auto itr1 = m_totRes1.find(chanNum);
  if (itr1!=m_totRes1.end()) {
    const std::vector<float>& chip = itr1->second;
    if (FE<(int)chip.size()) {
      res1 = chip.at(FE);
    }
  }
  float res2 = 0.0;
  auto itr2 = m_totRes2.find(chanNum);
  if (itr2!=m_totRes2.end()) {
    const std::vector<float>& chip = itr2->second;
    if (FE<(int)chip.size()) {
      res2 = chip.at(FE);
    }
  }
  return res1+res2*Q;
}

// Long pixel
void PixelChargeCalibCondData::setAnalogThresholdLong(const int chanNum, const int value) {
  m_analogThresholdLong[chanNum].push_back(value);
}

void PixelChargeCalibCondData::setAnalogThresholdSigmaLong(const int chanNum, const int value) {
  m_analogThresholdSigmaLong[chanNum].push_back(value);
}

void PixelChargeCalibCondData::setAnalogThresholdNoiseLong(const int chanNum, const int value) {
  m_analogThresholdNoiseLong[chanNum].push_back(value);
}

void PixelChargeCalibCondData::setInTimeThresholdLong(const int chanNum, const int value) {
  m_intimethresholdLong[chanNum].push_back(value);
}

void PixelChargeCalibCondData::setQ2TotALong(const int chanNum, const float value) {
  m_totALong[chanNum].push_back(value);
}

void PixelChargeCalibCondData::setQ2TotELong(const int chanNum, const float value) {
  m_totELong[chanNum].push_back(value);
}

void PixelChargeCalibCondData::setQ2TotCLong(const int chanNum, const float value) {
  m_totCLong[chanNum].push_back(value);
}

// Ganged pixel
void PixelChargeCalibCondData::setAnalogThresholdGanged(const int chanNum, const int value) {
  m_analogThresholdGanged[chanNum].push_back(value);
}

void PixelChargeCalibCondData::setAnalogThresholdSigmaGanged(const int chanNum, const int value) {
  m_analogThresholdSigmaGanged[chanNum].push_back(value);
}

void PixelChargeCalibCondData::setAnalogThresholdNoiseGanged(const int chanNum, const int value) {
  m_analogThresholdNoiseGanged[chanNum].push_back(value);
}

void PixelChargeCalibCondData::setInTimeThresholdGanged(const int chanNum, const int value) {
  m_intimethresholdGanged[chanNum].push_back(value);
}

float PixelChargeCalibCondData::getToT(const int chanNum, const int FE, const int type, float Q) const {
  float paramA = getQ2TotA(chanNum,FE,type);
  float paramE = getQ2TotE(chanNum,FE,type);
  float paramC = getQ2TotC(chanNum,FE,type);
  return paramA*(paramE+Q)/(paramC+Q);
}

float PixelChargeCalibCondData::getCharge(const int chanNum, const int FE, const int type, float ToT) const {
  float paramA = getQ2TotA(chanNum,FE,type);
  float paramE = getQ2TotE(chanNum,FE,type);
  float paramC = getQ2TotC(chanNum,FE,type);
  float charge = 0.0;
  if (std::fabs(paramA)>0.0 && std::fabs(ToT/paramA-1.0)>0.0) {
    charge = (paramC*ToT/paramA-paramE)/(1.0-ToT/paramA);
  }
  return charge;
}

void PixelChargeCalibCondData::clear() {
  m_analogThreshold.clear();
  m_analogThresholdSigma.clear();
  m_analogThresholdNoise.clear();
  m_intimethreshold.clear();
  m_totA.clear();
  m_totE.clear();
  m_totC.clear();
  m_totRes1.clear();
  m_totRes2.clear();
  m_analogThresholdLong.clear();
  m_analogThresholdSigmaLong.clear();
  m_analogThresholdNoiseLong.clear();
  m_intimethresholdLong.clear();
  m_totALong.clear();
  m_totELong.clear();
  m_totCLong.clear();
  m_analogThresholdGanged.clear();
  m_analogThresholdSigmaGanged.clear();
  m_analogThresholdNoiseGanged.clear();
  m_intimethresholdGanged.clear();
}


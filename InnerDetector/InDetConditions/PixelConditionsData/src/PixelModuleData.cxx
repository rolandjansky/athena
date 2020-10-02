/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelModuleData.h"

PixelModuleData::PixelModuleData():
  m_moduleStatus(),
  m_chipStatus()
{
}

PixelModuleData::~PixelModuleData() { }

// Module status
void PixelModuleData::setModuleStatus(const int chanNum, const int value) {
  m_moduleStatus[chanNum] = value;
}

int PixelModuleData::getModuleStatus(const int chanNum) const {
  auto itr = m_moduleStatus.find(chanNum);
  if (itr!=m_moduleStatus.end()) { return itr->second; }
  return 0;
}

// Chip (FE) status
void PixelModuleData::setChipStatus(const int chanNum, const int value) {
  m_chipStatus[chanNum] = value;
}

int PixelModuleData::getChipStatus(const int chanNum) const {
  auto itr = m_chipStatus.find(chanNum);
  if (itr!=m_chipStatus.end()) { return itr->second; }
  return 0;
}

// Switch parameters
void PixelModuleData::setUseCablingConditions(bool UseCablingConditions) { m_useCablingConditions = UseCablingConditions; }
bool PixelModuleData::getUseCablingConditions() const { return m_useCablingConditions; }

// Digitization parameters
void PixelModuleData::setBunchSpace(double bunchSpace) { m_bunchSpace = bunchSpace; }
double PixelModuleData::getBunchSpace() const { return m_bunchSpace; }

void PixelModuleData::setUseComTime(bool UseComTime) { m_UseComTime = UseComTime; }
bool PixelModuleData::getUseComTime() const { return m_UseComTime; }

void PixelModuleData::setComTime(double ComTime) { m_ComTime = ComTime; }
double PixelModuleData::getComTime() const { return m_ComTime; }

void PixelModuleData::setBarrelNumberOfBCID(std::vector<int> BarrelNumberOfBCID) { m_BarrelNumberOfBCID = BarrelNumberOfBCID; }
void PixelModuleData::setEndcapNumberOfBCID(std::vector<int> EndcapNumberOfBCID) { m_EndcapNumberOfBCID = EndcapNumberOfBCID; }
void PixelModuleData::setDBMNumberOfBCID(std::vector<int>    DBMNumberOfBCID)    { m_DBMNumberOfBCID    = DBMNumberOfBCID; }
int PixelModuleData::getNumberOfBCID(int bec, int layer) const {
  int nBCID = 1;
  if (std::abs(bec)==0 && layer<(int)m_BarrelNumberOfBCID.size()) { nBCID=m_BarrelNumberOfBCID.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_EndcapNumberOfBCID.size()) { nBCID=m_EndcapNumberOfBCID.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_DBMNumberOfBCID.size())    { nBCID=m_DBMNumberOfBCID.at(layer); }
  return nBCID;
}

void PixelModuleData::setBarrelTimeOffset(std::vector<double> BarrelTimeOffset) { m_BarrelTimeOffset = BarrelTimeOffset; }
void PixelModuleData::setEndcapTimeOffset(std::vector<double> EndcapTimeOffset) { m_EndcapTimeOffset = EndcapTimeOffset; }
void PixelModuleData::setDBMTimeOffset(std::vector<double>    DBMTimeOffset)    { m_DBMTimeOffset    = DBMTimeOffset; }
double PixelModuleData::getTimeOffset(int bec, int layer) const {
  double timeOffset = 0.0;
  if (std::abs(bec)==0 && layer<(int)m_BarrelTimeOffset.size()) { timeOffset=m_BarrelTimeOffset.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_EndcapTimeOffset.size()) { timeOffset=m_EndcapTimeOffset.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_DBMTimeOffset.size())    { timeOffset=m_DBMTimeOffset.at(layer); }
  return timeOffset;
}

void PixelModuleData::setBarrelTimeJitter(std::vector<double> BarrelTimeJitter) { m_BarrelTimeJitter = BarrelTimeJitter; }
void PixelModuleData::setEndcapTimeJitter(std::vector<double> EndcapTimeJitter) { m_EndcapTimeJitter = EndcapTimeJitter; }
void PixelModuleData::setDBMTimeJitter(std::vector<double>    DBMTimeJitter)    { m_DBMTimeJitter    = DBMTimeJitter; }
double PixelModuleData::getTimeJitter(int bec, int layer) const {
  double timeJitter = 0.0;
  if (std::abs(bec)==0 && layer<(int)m_BarrelTimeJitter.size()) { timeJitter=m_BarrelTimeJitter.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_EndcapTimeJitter.size()) { timeJitter=m_EndcapTimeJitter.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_DBMTimeJitter.size())    { timeJitter=m_DBMTimeJitter.at(layer); }
  return timeJitter;
}

void PixelModuleData::setDefaultBarrelAnalogThreshold(std::vector<int> BarrelAnalogThreshold) { m_defaultBarrelAnalogThreshold = BarrelAnalogThreshold; }
void PixelModuleData::setDefaultEndcapAnalogThreshold(std::vector<int> EndcapAnalogThreshold) { m_defaultEndcapAnalogThreshold = EndcapAnalogThreshold; }
void PixelModuleData::setDefaultDBMAnalogThreshold(std::vector<int>    DBMAnalogThreshold)    { m_defaultDBMAnalogThreshold = DBMAnalogThreshold; }

int PixelModuleData::getDefaultAnalogThreshold(int bec, int layer) const {
  int analogThreshold = -1;
  if (std::abs(bec)==0 && layer<(int)m_defaultBarrelAnalogThreshold.size()) { analogThreshold=m_defaultBarrelAnalogThreshold.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_defaultEndcapAnalogThreshold.size()) { analogThreshold=m_defaultEndcapAnalogThreshold.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_defaultDBMAnalogThreshold.size())    { analogThreshold=m_defaultDBMAnalogThreshold.at(layer); }
  return analogThreshold;
}

void PixelModuleData::setDefaultBarrelAnalogThresholdSigma(std::vector<int> BarrelAnalogThresholdSigma) { m_defaultBarrelAnalogThresholdSigma = BarrelAnalogThresholdSigma; }
void PixelModuleData::setDefaultEndcapAnalogThresholdSigma(std::vector<int> EndcapAnalogThresholdSigma) { m_defaultEndcapAnalogThresholdSigma = EndcapAnalogThresholdSigma; }
void PixelModuleData::setDefaultDBMAnalogThresholdSigma(std::vector<int>    DBMAnalogThresholdSigma)    { m_defaultDBMAnalogThresholdSigma = DBMAnalogThresholdSigma; }

int PixelModuleData::getDefaultAnalogThresholdSigma(int bec, int layer) const {
  int analogThresholdSigma = -1;
  if (std::abs(bec)==0 && layer<(int)m_defaultBarrelAnalogThresholdSigma.size()) { analogThresholdSigma=m_defaultBarrelAnalogThresholdSigma.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_defaultEndcapAnalogThresholdSigma.size()) { analogThresholdSigma=m_defaultEndcapAnalogThresholdSigma.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_defaultDBMAnalogThresholdSigma.size())    { analogThresholdSigma=m_defaultDBMAnalogThresholdSigma.at(layer); }
  return analogThresholdSigma;
}

void PixelModuleData::setDefaultBarrelAnalogThresholdNoise(std::vector<int> BarrelAnalogThresholdNoise) { m_defaultBarrelAnalogThresholdNoise = BarrelAnalogThresholdNoise; }
void PixelModuleData::setDefaultEndcapAnalogThresholdNoise(std::vector<int> EndcapAnalogThresholdNoise) { m_defaultEndcapAnalogThresholdNoise = EndcapAnalogThresholdNoise; }
void PixelModuleData::setDefaultDBMAnalogThresholdNoise(std::vector<int>    DBMAnalogThresholdNoise)    { m_defaultDBMAnalogThresholdNoise = DBMAnalogThresholdNoise; }

int PixelModuleData::getDefaultAnalogThresholdNoise(int bec, int layer) const {
  int analogThresholdNoise = -1;
  if (std::abs(bec)==0 && layer<(int)m_defaultBarrelAnalogThresholdNoise.size()) { analogThresholdNoise=m_defaultBarrelAnalogThresholdNoise.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_defaultEndcapAnalogThresholdNoise.size()) { analogThresholdNoise=m_defaultEndcapAnalogThresholdNoise.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_defaultDBMAnalogThresholdNoise.size())    { analogThresholdNoise=m_defaultDBMAnalogThresholdNoise.at(layer); }
  return analogThresholdNoise;
}

void PixelModuleData::setDefaultBarrelInTimeThreshold(std::vector<int> BarrelInTimeThreshold) { m_defaultBarrelInTimeThreshold = BarrelInTimeThreshold; }
void PixelModuleData::setDefaultEndcapInTimeThreshold(std::vector<int> EndcapInTimeThreshold) { m_defaultEndcapInTimeThreshold = EndcapInTimeThreshold; }
void PixelModuleData::setDefaultDBMInTimeThreshold(std::vector<int>    DBMInTimeThreshold)    { m_defaultDBMInTimeThreshold = DBMInTimeThreshold; }

int PixelModuleData::getDefaultInTimeThreshold(int bec, int layer) const {
  int analogInTimeThreshold = -1;
  if (std::abs(bec)==0 && layer<(int)m_defaultBarrelInTimeThreshold.size()) { analogInTimeThreshold=m_defaultBarrelInTimeThreshold.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_defaultEndcapInTimeThreshold.size()) { analogInTimeThreshold=m_defaultEndcapInTimeThreshold.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_defaultDBMInTimeThreshold.size())    { analogInTimeThreshold=m_defaultDBMInTimeThreshold.at(layer); }
  return analogInTimeThreshold;
}

void PixelModuleData::setBarrelToTThreshold(std::vector<int> BarrelToTThreshold) { m_BarrelToTThreshold = BarrelToTThreshold; }
void PixelModuleData::setEndcapToTThreshold(std::vector<int> EndcapToTThreshold) { m_EndcapToTThreshold = EndcapToTThreshold; }
void PixelModuleData::setDBMToTThreshold(std::vector<int>    DBMToTThreshold)    { m_DBMToTThreshold = DBMToTThreshold; }

int PixelModuleData::getToTThreshold(int bec, int layer) const {
  int totThreshold = -1;
  if (std::abs(bec)==0 && layer<(int)m_BarrelToTThreshold.size()) { totThreshold=m_BarrelToTThreshold.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_EndcapToTThreshold.size()) { totThreshold=m_EndcapToTThreshold.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_DBMToTThreshold.size())    { totThreshold=m_DBMToTThreshold.at(layer); }
  return totThreshold;
}

void PixelModuleData::setBarrelCrossTalk(std::vector<double> BarrelCrossTalk) { m_BarrelCrossTalk = BarrelCrossTalk; }
void PixelModuleData::setEndcapCrossTalk(std::vector<double> EndcapCrossTalk) { m_EndcapCrossTalk = EndcapCrossTalk; }
void PixelModuleData::setDBMCrossTalk(std::vector<double>    DBMCrossTalk)    { m_DBMCrossTalk = DBMCrossTalk; }

double PixelModuleData::getCrossTalk(int bec, int layer) const { 
  double crossTalk = -1.0;
  if (std::abs(bec)==0 && layer<(int)m_BarrelCrossTalk.size()) { crossTalk=m_BarrelCrossTalk.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_EndcapCrossTalk.size()) { crossTalk=m_EndcapCrossTalk.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_DBMCrossTalk.size())    { crossTalk=m_DBMCrossTalk.at(layer); }
  return crossTalk;
}

void PixelModuleData::setBarrelThermalNoise(std::vector<double> BarrelThermalNoise) { m_BarrelThermalNoise = BarrelThermalNoise; }
void PixelModuleData::setEndcapThermalNoise(std::vector<double> EndcapThermalNoise) { m_EndcapThermalNoise = EndcapThermalNoise; }
void PixelModuleData::setDBMThermalNoise(std::vector<double>    DBMThermalNoise)    { m_DBMThermalNoise    = DBMThermalNoise; }

double PixelModuleData::getThermalNoise(int bec, int layer) const {
  double noise = -1.0;
  if (std::abs(bec)==0 && layer<(int)m_BarrelThermalNoise.size()) { noise=m_BarrelThermalNoise.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_EndcapThermalNoise.size()) { noise=m_EndcapThermalNoise.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_DBMThermalNoise.size())    { noise=m_DBMThermalNoise.at(layer); }
  return noise;
}

void PixelModuleData::setBarrelNoiseOccupancy(std::vector<double> BarrelNoiseOccupancy) { m_BarrelNoiseOccupancy = BarrelNoiseOccupancy; }
void PixelModuleData::setEndcapNoiseOccupancy(std::vector<double> EndcapNoiseOccupancy) { m_EndcapNoiseOccupancy = EndcapNoiseOccupancy; }
void PixelModuleData::setDBMNoiseOccupancy(std::vector<double>    DBMNoiseOccupancy)    { m_DBMNoiseOccupancy    = DBMNoiseOccupancy; }
double PixelModuleData::getNoiseOccupancy(int bec, int layer) const {
  double noiseOccupancy = 0.0;
  if (std::abs(bec)==0 && layer<(int)m_BarrelNoiseOccupancy.size()) { noiseOccupancy=m_BarrelNoiseOccupancy.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_EndcapNoiseOccupancy.size()) { noiseOccupancy=m_EndcapNoiseOccupancy.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_DBMNoiseOccupancy.size())    { noiseOccupancy=m_DBMNoiseOccupancy.at(layer); }
  return noiseOccupancy;
}

void PixelModuleData::setBarrelDisableProbability(std::vector<double> BarrelDisableProbability) { m_BarrelDisableProbability= BarrelDisableProbability; }
void PixelModuleData::setEndcapDisableProbability(std::vector<double> EndcapDisableProbability) { m_EndcapDisableProbability= EndcapDisableProbability; }
void PixelModuleData::setDBMDisableProbability(std::vector<double>    DBMDisableProbability)    { m_DBMDisableProbability   = DBMDisableProbability; }
double PixelModuleData::getDisableProbability(int bec, int layer) const {
  double disableProb = 0.0;
  if (std::abs(bec)==0 && layer<(int)m_BarrelDisableProbability.size()) { disableProb=m_BarrelDisableProbability.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_EndcapDisableProbability.size()) { disableProb=m_EndcapDisableProbability.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_DBMDisableProbability.size())    { disableProb=m_DBMDisableProbability.at(layer); }
  return disableProb;
}

void PixelModuleData::setBarrelNoiseShape(const int layer, const float BarrelNoiseShape) {
  m_BarrelNoiseShape[layer].push_back(BarrelNoiseShape);
}

void PixelModuleData::setEndcapNoiseShape(const int layer, const float EndcapNoiseShape) {
  m_EndcapNoiseShape[layer].push_back(EndcapNoiseShape);
}

void PixelModuleData::setDBMNoiseShape(const int layer, const float    DBMNoiseShape) {
  m_DBMNoiseShape[layer].push_back(DBMNoiseShape);
}

std::vector<float> PixelModuleData::getNoiseShape(const int bec, const int layer) const {
  std::vector<float> chip;
  if (bec==0) {
    auto itr = m_BarrelNoiseShape.find(layer);
    if (itr!=m_BarrelNoiseShape.end()) { chip = itr->second; }
  }
  else if (std::abs(bec)==2) {
    auto itr = m_EndcapNoiseShape.find(layer);
    if (itr!=m_EndcapNoiseShape.end()) { chip = itr->second; } 
  }
  else if (std::abs(bec)==4) {
    auto itr = m_DBMNoiseShape.find(layer);
    if (itr!=m_DBMNoiseShape.end()) { chip = itr->second; } 
  }
  return chip;
}

void PixelModuleData::setFEI3BarrelLatency(std::vector<int> FEI3BarrelLatency) { m_FEI3BarrelLatency = FEI3BarrelLatency; }
void PixelModuleData::setFEI3EndcapLatency(std::vector<int> FEI3EndcapLatency) { m_FEI3EndcapLatency = FEI3EndcapLatency; }

int PixelModuleData::getFEI3Latency(int bec, int layer) const {
  int  latency = -1;
  if (std::abs(bec)==0 && layer<(int)m_FEI3BarrelLatency.size()) { latency=m_FEI3BarrelLatency.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_FEI3EndcapLatency.size()) { latency=m_FEI3EndcapLatency.at(layer); }
  return latency;
}

void PixelModuleData::setFEI3BarrelHitDuplication(std::vector<bool> FEI3BarrelHitDuplication) { m_FEI3BarrelHitDuplication = FEI3BarrelHitDuplication; }
void PixelModuleData::setFEI3EndcapHitDuplication(std::vector<bool> FEI3EndcapHitDuplication) { m_FEI3EndcapHitDuplication = FEI3EndcapHitDuplication; }

bool PixelModuleData::getFEI3HitDuplication(int bec, int layer) const {
  bool hitdupli = false;
  if (std::abs(bec)==0 && layer<(int)m_FEI3BarrelHitDuplication.size()) { hitdupli=m_FEI3BarrelHitDuplication.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_FEI3EndcapHitDuplication.size()) { hitdupli=m_FEI3EndcapHitDuplication.at(layer); }
  return hitdupli;
}

void PixelModuleData::setFEI3BarrelSmallHitToT(std::vector<int> FEI3BarrelSmallHitToT) { m_FEI3BarrelSmallHitToT = FEI3BarrelSmallHitToT; }
void PixelModuleData::setFEI3EndcapSmallHitToT(std::vector<int> FEI3EndcapSmallHitToT) { m_FEI3EndcapSmallHitToT = FEI3EndcapSmallHitToT; }

int PixelModuleData::getFEI3SmallHitToT(int bec, int layer) const {
  int smallToT = -1;
  if (std::abs(bec)==0 && layer<(int)m_FEI3BarrelSmallHitToT.size()) { smallToT=m_FEI3BarrelSmallHitToT.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_FEI3EndcapSmallHitToT.size()) { smallToT=m_FEI3EndcapSmallHitToT.at(layer); }
  return smallToT;
}

void PixelModuleData::setFEI3BarrelTimingSimTune(std::vector<int> FEI3BarrelTimingSimTune) { m_FEI3BarrelTimingSimTune = FEI3BarrelTimingSimTune; }
void PixelModuleData::setFEI3EndcapTimingSimTune(std::vector<int> FEI3EndcapTimingSimTune) { m_FEI3EndcapTimingSimTune = FEI3EndcapTimingSimTune; }
int PixelModuleData::getFEI3TimingSimTune(int bec, int layer) const {
  int timesim = 0;
  if (std::abs(bec)==0 && layer<(int)m_FEI3BarrelTimingSimTune.size()) { timesim=m_FEI3BarrelTimingSimTune.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_FEI3EndcapTimingSimTune.size()) { timesim=m_FEI3EndcapTimingSimTune.at(layer); }
  return timesim;
}

void PixelModuleData::setFEI4BarrelHitDiscConfig(std::vector<int> FEI4BarrelHitDiscConfig) { m_FEI4BarrelHitDiscConfig = FEI4BarrelHitDiscConfig; }
void PixelModuleData::setFEI4EndcapHitDiscConfig(std::vector<int> FEI4EndcapHitDiscConfig) { m_FEI4EndcapHitDiscConfig = FEI4EndcapHitDiscConfig; }
int PixelModuleData::getFEI4HitDiscConfig(int bec, int layer) const {
  int hitDiscConfig = 2;
  if (std::abs(bec)==0 && layer<(int)m_FEI4BarrelHitDiscConfig.size()) { hitDiscConfig=m_FEI4BarrelHitDiscConfig.at(layer); }
  if (std::abs(bec)==2 && layer<(int)m_FEI4EndcapHitDiscConfig.size()) { hitDiscConfig=m_FEI4EndcapHitDiscConfig.at(layer); }
  if (std::abs(bec)==4 && layer<(int)m_FEI4EndcapHitDiscConfig.size()) { hitDiscConfig=m_FEI4EndcapHitDiscConfig.at(layer); }
  return hitDiscConfig;
}

int PixelModuleData::getFEI4OverflowToT(int bec, int layer) const {
  static const int overflow[]={14,15,16,14,16};
  unsigned int idx=static_cast<unsigned int>(getFEI4HitDiscConfig(bec,layer)) < 4 ? getFEI4HitDiscConfig(bec,layer) : 4;
  return overflow[idx];
}

// Charge calibration parameters
void PixelModuleData::setDefaultQ2TotA(float paramA) { m_paramA=paramA; }
void PixelModuleData::setDefaultQ2TotE(float paramE) { m_paramE=paramE; }
void PixelModuleData::setDefaultQ2TotC(float paramC) { m_paramC=paramC; }
float PixelModuleData::getDefaultQ2TotA() const { return m_paramA; }
float PixelModuleData::getDefaultQ2TotE() const { return m_paramE; }
float PixelModuleData::getDefaultQ2TotC() const { return m_paramC; }

// DCS parameters
void PixelModuleData::setDefaultBiasVoltage(float biasVoltage) { m_biasVoltage=biasVoltage; }
float PixelModuleData::getDefaultBiasVoltage() const { return m_biasVoltage; }

void PixelModuleData::setDefaultTemperature(float temperature) { m_temperature=temperature; }
float PixelModuleData::getDefaultTemperature() const { return m_temperature; }

// Cabling parameters
void PixelModuleData::setCablingMapToFile(bool cablingMapToFile) { m_cablingMapToFile = cablingMapToFile; }
bool PixelModuleData::getCablingMapToFile() const { return m_cablingMapToFile; }

void PixelModuleData::setCablingMapFileName(std::string cablingMapFileName) { m_cablingMapFileName = cablingMapFileName; }
std::string PixelModuleData::getCablingMapFileName() const { return m_cablingMapFileName; }

// Distortion parameters
void PixelModuleData::setDistortionInputSource(int distortionInputSource) { m_distortionInputSource = distortionInputSource; }
int PixelModuleData::getDistortionInputSource() const { return m_distortionInputSource; }

void PixelModuleData::setDistortionVersion(int distortionVersion) { m_distortionVersion = distortionVersion; }
int PixelModuleData::getDistortionVersion() const { return m_distortionVersion; }

void PixelModuleData::setDistortionR1(double distortionR1) { m_distortionR1 = distortionR1; }
double PixelModuleData::getDistortionR1() const { return m_distortionR1; }

void PixelModuleData::setDistortionR2(double distortionR2) { m_distortionR2 = distortionR2; }
double PixelModuleData::getDistortionR2() const { return m_distortionR2; }

void PixelModuleData::setDistortionTwist(double distortionTwist) { m_distortionTwist = distortionTwist; }
double PixelModuleData::getDistortionTwist() const { return m_distortionTwist; }

void PixelModuleData::setDistortionMeanR(double distortionMeanR) { m_distortionMeanR = distortionMeanR; }
double PixelModuleData::getDistortionMeanR() const { return m_distortionMeanR; }

void PixelModuleData::setDistortionRMSR(double distortionRMSR) { m_distortionRMSR = distortionRMSR; }
double PixelModuleData::getDistortionRMSR() const { return m_distortionRMSR; }

void PixelModuleData::setDistortionMeanTwist(double distortionMeanTwist) { m_distortionMeanTwist = distortionMeanTwist; }
double PixelModuleData::getDistortionMeanTwist() const { return m_distortionMeanTwist; }

void PixelModuleData::setDistortionRMSTwist(double distortionRMSTwist) { m_distortionRMSTwist = distortionRMSTwist; }
double PixelModuleData::getDistortionRMSTwist() const { return m_distortionRMSTwist; }

void PixelModuleData::setDistortionWriteToFile(bool distortionWriteToFile) { m_distortionWriteToFile = distortionWriteToFile; }
bool PixelModuleData::getDistortionWriteToFile() const { return m_distortionWriteToFile; }

void PixelModuleData::setDistortionFileName(std::string distortionFileName) { m_distortionFileName = distortionFileName; }
std::string PixelModuleData::getDistortionFileName() const { return m_distortionFileName; }

void PixelModuleData::clear() {
  m_moduleStatus.clear();
  m_chipStatus.clear();
}


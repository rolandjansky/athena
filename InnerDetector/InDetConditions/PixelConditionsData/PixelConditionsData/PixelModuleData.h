/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelConditionsData/PixelModuleData.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date November, 2019
 * @brief Store pixel constant parameters in PixelModuleData.
 */

#ifndef PIXELMODULEDATA_H
#define PIXELMODULEDATA_H

#include <AthenaKernel/CLASS_DEF.h>
#include <AthenaKernel/CondCont.h>


class PixelModuleData 
{
  public:
    // Digitization parameters
    void setBunchSpace(double bunchSpace);
    double getBunchSpace() const;

    void setBarrelNumberOfBCID(const std::vector<int> &barrelNumberOfBCID);
    void setEndcapNumberOfBCID(const std::vector<int> &endcapNumberOfBCID);
    void setDBMNumberOfBCID(const std::vector<int> &DBMNumberOfBCID);
    int getNumberOfBCID(int barrel_ec, int layer) const;

    void setBarrelTimeOffset(const std::vector<double> &barrelTimeOffset);
    void setEndcapTimeOffset(const std::vector<double> &endcapTimeOffset);
    void setDBMTimeOffset(const std::vector<double> &DBMTimeOffset);
    double getTimeOffset(int barrel_ec, int layer) const;

    void setBarrelTimeJitter(const std::vector<double> &barrelTimeJitter);
    void setEndcapTimeJitter(const std::vector<double> &endcapTimeJitter);
    void setDBMTimeJitter(const std::vector<double> &DBMTimeJitter);
    double getTimeJitter(int barrel_ec, int layer) const;

    void setDefaultBarrelAnalogThreshold(const std::vector<int> &barrelAnalogThreshold);
    void setDefaultEndcapAnalogThreshold(const std::vector<int> &endcapAnalogThreshold);
    void setDefaultDBMAnalogThreshold(const std::vector<int> &DBMAnalogThreshold);
    int getDefaultAnalogThreshold(int barrel_ec, int layer) const;

    void setDefaultBarrelAnalogThresholdSigma(const std::vector<int> &barrelAnalogThresholdSigma);
    void setDefaultEndcapAnalogThresholdSigma(const std::vector<int> &endcapAnalogThresholdSigma);
    void setDefaultDBMAnalogThresholdSigma(const std::vector<int> &DBMAnalogThresholdSigma);
    int getDefaultAnalogThresholdSigma(int barrel_ec, int layer) const;

    void setDefaultBarrelAnalogThresholdNoise(const std::vector<int> &barrelAnalogThresholdNoise);
    void setDefaultEndcapAnalogThresholdNoise(const std::vector<int> &endcapAnalogThresholdNoise);
    void setDefaultDBMAnalogThresholdNoise(const std::vector<int> &DBMAnalogThresholdNoise);
    int getDefaultAnalogThresholdNoise(int barrel_ec, int layer) const;

    void setDefaultBarrelInTimeThreshold(const std::vector<int> &barrelInTimeThreshold);
    void setDefaultEndcapInTimeThreshold(const std::vector<int> &endcapInTimeThreshold);
    void setDefaultDBMInTimeThreshold(const std::vector<int> &DBMInTimeThreshold);
    int getDefaultInTimeThreshold(int barrel_ec, int layer) const;

    void setBarrelToTThreshold(const std::vector<int> &barrelToTThreshold);
    void setEndcapToTThreshold(const std::vector<int> &endcapToTThreshold);
    void setDBMToTThreshold(const std::vector<int> &DBMToTThreshold);
    int getToTThreshold(int barrel_ec, int layer) const;

    void setBarrelCrossTalk(const std::vector<double> &barrelCrossTalk);
    void setEndcapCrossTalk(const std::vector<double> &endcapCrossTalk);
    void setDBMCrossTalk(const std::vector<double> &DBMCrossTalk);
    double getCrossTalk(int barrel_ec, int layer) const;

    void setBarrelThermalNoise(const std::vector<double> &barrelThermalNoise);
    void setEndcapThermalNoise(const std::vector<double> &endcapThermalNoise);
    void setDBMThermalNoise(const std::vector<double> &DBMThermalNoise);
    double getThermalNoise(int barrel_ec, int layer) const;

    void setBarrelNoiseOccupancy(const std::vector<double> &barrelNoiseOccupancy);
    void setEndcapNoiseOccupancy(const std::vector<double> &endcapNoiseOccupancy);
    void setDBMNoiseOccupancy(const std::vector<double> &DBMNoiseOccupancy);
    double getNoiseOccupancy(int barrel_ec, int layer) const;

    void setBarrelDisableProbability(const std::vector<double> &barrelDisableProbability);
    void setEndcapDisableProbability(const std::vector<double> &endcapDisableProbability);
    void setDBMDisableProbability(const std::vector<double> &DBMDisableProbability);
    double getDisableProbability(int barrel_ec, int layer) const;

    void setBarrelNoiseShape(const std::vector<std::vector<float>> &barrelNoiseShape);
    void setEndcapNoiseShape(const std::vector<std::vector<float>> &endcapNoiseShape);
    void setDBMNoiseShape(const std::vector<std::vector<float>> &DBMNoiseShape);
    const std::vector<float> &getNoiseShape(int barrel_ec, int layer) const;

    void setFEI3BarrelLatency(const std::vector<int> &FEI3BarrelLatency);
    void setFEI3EndcapLatency(const std::vector<int> &FEI3EndcapLatency);
    int getFEI3Latency(int barrel_ec, int layer) const;

    void setFEI3BarrelHitDuplication(const std::vector<bool> &FEI3BarrelHitDuplication);
    void setFEI3EndcapHitDuplication(const std::vector<bool> &FEI3EndcapHitDuplication);
    bool getFEI3HitDuplication(int barrel_ec, int layer) const;

    void setFEI3BarrelSmallHitToT(const std::vector<int> &FEI3BarrelSmallHitToT);
    void setFEI3EndcapSmallHitToT(const std::vector<int> &FEI3EndcapSmallHitToT);
    int getFEI3SmallHitToT(int barrel_ec, int layer) const;

    void setFEI3BarrelTimingSimTune(const std::vector<int> &FEI3BarrelTimingSimTune);
    void setFEI3EndcapTimingSimTune(const std::vector<int> &FEI3EndcapTimingSimTune);
    int getFEI3TimingSimTune(int barrel_ec, int layer) const;

    void setFEI4BarrelHitDiscConfig(const std::vector<int> &FEI4BarrelHitDiscConfig);
    void setFEI4EndcapHitDiscConfig(const std::vector<int> &FEI4EndcapHitDiscConfig);
    int getFEI4HitDiscConfig(int barrel_ec, int layer) const;
    int getFEI4OverflowToT(int barrel_ec, int layer) const;

    void setFEI4ChargScaling(float scaleFEI4);
    void setUseFEI4SpecialScalingFunction(bool useFEI4SpecialScalingFunction);
    float getFEI4ChargScaling() const;
    bool getUseFEI4SpecialScalingFunction() const;

    void setFEI4ToTSigma(const std::vector<double> &FEI4ToTSigma);
    double getFEI4ToTSigma(int tot) const;

    // Charge calibration parameters
    void setDefaultQ2TotA(float paramA);
    void setDefaultQ2TotE(float paramE);
    void setDefaultQ2TotC(float paramC);
    float getDefaultQ2TotA() const;
    float getDefaultQ2TotE() const;
    float getDefaultQ2TotC() const;

    // Lorentz angle correction
    void setBarrelLorentzAngleCorr(const std::vector<double> &barrelLorentzAngleCorr);
    void setEndcapLorentzAngleCorr(const std::vector<double> &endcapLorentzAngleCorr);
    double getLorentzAngleCorr(int barrel_ec, int layer) const;

    // DCS parameters
    void setDefaultBiasVoltage(float biasVoltage);
    float getDefaultBiasVoltage() const;

    void setDefaultBarrelBiasVoltage(const std::vector<float> &barrelBiasVoltage);
    void setDefaultEndcapBiasVoltage(const std::vector<float> &endcapBiasVoltage);
    void setDefaultDBMBiasVoltage(const std::vector<float> &DBMBiasVoltage);
    float getDefaultBiasVoltage(int barrel_ec, int layer) const;

    void setDefaultTemperature(float temperature);
    float getDefaultTemperature() const;

    // Radiation damage fluence maps
    void setFluenceLayer(const std::vector<double> &fluenceLayer);
    const std::vector<double> &getFluenceLayer() const;

    void setRadSimFluenceMapList(const std::vector<std::string> &radSimFluenceMapList);
    const std::vector<std::string> &getRadSimFluenceMapList() const;

    void setFluenceLayer3D(const std::vector<double> &fluenceLayer);
    const std::vector<double> &getFluenceLayer3D() const;

    void setRadSimFluenceMapList3D(const std::vector<std::string> &radSimFluenceMapList3D);
    const std::vector<std::string> &getRadSimFluenceMapList3D() const;

    // Cabling parameters
    void setCablingMapToFile(bool cablingMapToFile);
    bool getCablingMapToFile() const;

    void setCablingMapFileName(const std::string &cablingMapFileName);
    const std::string &getCablingMapFileName() const;

    // Distortion parameters
    void setDistortionInputSource(int distortionInputSource);
    int getDistortionInputSource() const;

    void setDistortionVersion(int distortionVersion);
    int getDistortionVersion() const;

    void setDistortionR1(double distortionR1);
    double getDistortionR1() const;

    void setDistortionR2(double distortionR2);
    double getDistortionR2() const;

    void setDistortionTwist(double distortionTwist);
    double getDistortionTwist() const;

    void setDistortionMeanR(double distortionMeanR);
    double getDistortionMeanR() const;

    void setDistortionRMSR(double distortionRMSR);
    double getDistortionRMSR() const;

    void setDistortionMeanTwist(double distortionMeanTwist);
    double getDistortionMeanTwist() const;

    void setDistortionRMSTwist(double distortionRMSTwist);
    double getDistortionRMSTwist() const;

    void setDistortionWriteToFile(bool distortionWriteToFile);
    bool getDistortionWriteToFile() const;

    void setDistortionFileName(const std::string &distortionFileName);
    const std::string &getDistortionFileName() const;


  private:
    double m_bunchSpace{};

    std::vector<int> m_barrelNumberOfBCID;
    std::vector<int> m_endcapNumberOfBCID;
    std::vector<int> m_DBMNumberOfBCID;
    std::vector<double> m_barrelTimeOffset;
    std::vector<double> m_endcapTimeOffset;
    std::vector<double> m_DBMTimeOffset;
    std::vector<double> m_barrelTimeJitter;
    std::vector<double> m_endcapTimeJitter;
    std::vector<double> m_DBMTimeJitter;

    std::vector<int> m_defaultBarrelAnalogThreshold;
    std::vector<int> m_defaultEndcapAnalogThreshold;
    std::vector<int> m_defaultDBMAnalogThreshold;
    std::vector<int> m_defaultBarrelAnalogThresholdSigma;
    std::vector<int> m_defaultEndcapAnalogThresholdSigma;
    std::vector<int> m_defaultDBMAnalogThresholdSigma;
    std::vector<int> m_defaultBarrelAnalogThresholdNoise;
    std::vector<int> m_defaultEndcapAnalogThresholdNoise;
    std::vector<int> m_defaultDBMAnalogThresholdNoise;
    std::vector<int> m_defaultBarrelInTimeThreshold;
    std::vector<int> m_defaultEndcapInTimeThreshold;
    std::vector<int> m_defaultDBMInTimeThreshold;

    std::vector<int> m_barrelToTThreshold;
    std::vector<int> m_endcapToTThreshold;
    std::vector<int> m_DBMToTThreshold;
    std::vector<double> m_barrelCrossTalk; 
    std::vector<double> m_endcapCrossTalk; 
    std::vector<double> m_DBMCrossTalk; 
    std::vector<double> m_barrelThermalNoise;
    std::vector<double> m_endcapThermalNoise;
    std::vector<double> m_DBMThermalNoise;
    std::vector<double> m_barrelNoiseOccupancy;
    std::vector<double> m_endcapNoiseOccupancy;
    std::vector<double> m_DBMNoiseOccupancy;
    std::vector<double> m_barrelDisableProbability;
    std::vector<double> m_endcapDisableProbability;
    std::vector<double> m_DBMDisableProbability;

    std::vector<std::vector<float>> m_barrelNoiseShape;
    std::vector<std::vector<float>> m_endcapNoiseShape;
    std::vector<std::vector<float>> m_DBMNoiseShape;

    std::vector<int>  m_FEI3BarrelLatency;
    std::vector<int>  m_FEI3EndcapLatency;
    std::vector<bool> m_FEI3BarrelHitDuplication;
    std::vector<bool> m_FEI3EndcapHitDuplication;
    std::vector<int>  m_FEI3BarrelSmallHitToT;
    std::vector<int>  m_FEI3EndcapSmallHitToT;
    std::vector<int>  m_FEI3BarrelTimingSimTune;
    std::vector<int>  m_FEI3EndcapTimingSimTune;
    std::vector<int>  m_FEI4BarrelHitDiscConfig;
    std::vector<int>  m_FEI4EndcapHitDiscConfig;

    float m_scaleFEI4{};
    bool m_useFEI4SpecialScalingFunction{};

    std::vector<double> m_FEI4ToTSigma;

    float m_paramA{};
    float m_paramE{};
    float m_paramC{};

    std::vector<double> m_barrelLorentzAngleCorr;
    std::vector<double> m_endcapLorentzAngleCorr;

    float m_biasVoltage{};
    float m_temperature{};

    std::vector<float> m_barrelBiasVoltage;
    std::vector<float> m_endcapBiasVoltage;
    std::vector<float> m_DBMBiasVoltage;

    std::vector<double> m_fluenceLayer;
    std::vector<std::string> m_radSimFluenceMapList;

    std::vector<double> m_fluenceLayer3D;
    std::vector<std::string> m_radSimFluenceMapList3D;

    bool        m_cablingMapToFile{};
    std::string m_cablingMapFileName;

    int    m_distortionInputSource{};
    int    m_distortionVersion{};
    double m_distortionR1{};
    double m_distortionR2{};
    double m_distortionTwist{};
    double m_distortionMeanR{};
    double m_distortionRMSR{};
    double m_distortionMeanTwist{};
    double m_distortionRMSTwist{};
    bool   m_distortionWriteToFile{};
    std::string m_distortionFileName;

};

CLASS_DEF( PixelModuleData , 345932873 , 1 )

CONDCONT_DEF( PixelModuleData, 578988393 );

#endif

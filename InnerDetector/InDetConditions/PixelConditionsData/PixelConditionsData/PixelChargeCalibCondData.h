/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCHARGECALIBCONDDATA_H
#define PIXELCHARGECALIBCONDDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include <map>

class PixelChargeCalibCondData {
  public:
    PixelChargeCalibCondData();
    virtual ~PixelChargeCalibCondData();

    enum PixelType{NORMAL,LONG,GANGED};
    enum CalibrationStrategy{RUN1PIX,LUTFEI4,RD53};

    // Normal pixel
    void setAnalogThreshold(const int chanNum, const int value);
    void setAnalogThresholdSigma(const int chanNum, const int value);
    void setAnalogThresholdNoise(const int chanNum, const int value);
    void setInTimeThreshold(const int chanNum, const int value);

    void setQ2TotA(const int chanNum, const float value);
    void setQ2TotE(const int chanNum, const float value);
    void setQ2TotC(const int chanNum, const float value);

    void setTotRes1(const int chanNum, const float value);
    void setTotRes2(const int chanNum, const float value);

    int getAnalogThreshold(const int chanNum, const int FE, const int type) const;
    int getAnalogThresholdSigma(const int chanNum, const int FE, const int type) const;
    int getAnalogThresholdNoise(const int chanNum, const int FE, const int type) const;
    int getInTimeThreshold(const int chanNum, const int FE, const int type) const;

    float getQ2TotA(const int chanNum, const int FE, const int type) const;
    float getQ2TotE(const int chanNum, const int FE, const int type) const;
    float getQ2TotC(const int chanNum, const int FE, const int type) const;

    float getTotRes(const int chanNum, const int FE, float Q) const;

    // Long pixel
    void setAnalogThresholdLong(const int chanNum, const int value);
    void setAnalogThresholdSigmaLong(const int chanNum, const int value);
    void setAnalogThresholdNoiseLong(const int chanNum, const int value);
    void setInTimeThresholdLong(const int chanNum, const int value);

    void setQ2TotALong(const int chanNum, const float value);
    void setQ2TotELong(const int chanNum, const float value);
    void setQ2TotCLong(const int chanNum, const float value);

    // Ganged pixel
    void setAnalogThresholdGanged(const int chanNum, const int value);
    void setAnalogThresholdSigmaGanged(const int chanNum, const int value);
    void setAnalogThresholdNoiseGanged(const int chanNum, const int value);
    void setInTimeThresholdGanged(const int chanNum, const int value);

    float getToT(const int chanNum, const int FE, const int type, float Q) const;
    float getCharge(const int chanNum, const int FE, const int type, float ToT) const;

    // new IBL calibration
    void  setCalibrationStrategy(const int chanNum, const int type);
    void  setTot2Charges(const int chanNum, const std::array<float,16> charges);
    const std::array<float,16> getQs(const int chanNum, const int FE) const;
    int   getCalibrationStrategy(const int chanNum) const;
    float getChargeLUTFEI4(const int chanNum, const int FE, float ToT) const;
    float getToTLUTFEI4(const int chanNum, const int FE, float Q) const;

    void clear();

  private:
    typedef std::map<int, std::vector<int>> chipThreshold;
    typedef std::map<int, std::vector<float>> chipCharge;

    // Normal pixel
    chipThreshold m_analogThreshold;
    chipThreshold m_analogThresholdSigma;
    chipThreshold m_analogThresholdNoise;
    chipThreshold m_intimethreshold;

    chipCharge m_totA;
    chipCharge m_totE;
    chipCharge m_totC;

    chipCharge m_totRes1;
    chipCharge m_totRes2;

    // Long pixel
    chipThreshold m_analogThresholdLong;
    chipThreshold m_analogThresholdSigmaLong;
    chipThreshold m_analogThresholdNoiseLong;
    chipThreshold m_intimethresholdLong;

    chipCharge m_totALong;
    chipCharge m_totELong;
    chipCharge m_totCLong;

    // Ganged pixel
    chipThreshold m_analogThresholdGanged;
    chipThreshold m_analogThresholdSigmaGanged;
    chipThreshold m_analogThresholdNoiseGanged;
    chipThreshold m_intimethresholdGanged;

    // new IBL calibration
    std::map<int, int> m_calibrationStrategy;
    typedef std::vector<std::array<float,16>> IBLModule;
    std::map<int,IBLModule> m_tot2chrg;

};

CLASS_DEF( PixelChargeCalibCondData , 345532779 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( PixelChargeCalibCondData, 578786399 );

#endif

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCHARGECALIBCONDDATA_H
#define PIXELCHARGECALIBCONDDATA_H

#include <AthenaKernel/CLASS_DEF.h>
#include <PixelReadoutDefinitions/PixelReadoutDefinitions.h>

#include <map>

class PixelChargeCalibCondData
{
  public:
    PixelChargeCalibCondData();

    static constexpr size_t IBLCalibrationSize{16};
    using IBLCalibration = std::array<float, IBLCalibrationSize>;
    using IBLModule = std::vector<IBLCalibration>;

    enum class CalibrationStrategy
    {
      RUN1PIX,
      LUTFEI4,
      RD53
    };

    // Normal pixel
    void setAnalogThreshold(InDetDD::PixelDiodeType type, unsigned int moduleHash, std::vector<int> value);
    void setAnalogThresholdSigma(InDetDD::PixelDiodeType type, unsigned int moduleHash, std::vector<int> value);
    void setAnalogThresholdNoise(InDetDD::PixelDiodeType type, unsigned int moduleHash, std::vector<int> value);
    void setInTimeThreshold(InDetDD::PixelDiodeType type, unsigned int moduleHash, std::vector<int> value);

    void setQ2TotA(InDetDD::PixelDiodeType type, unsigned int moduleHash, std::vector<float> value);
    void setQ2TotE(InDetDD::PixelDiodeType type, unsigned int moduleHash, std::vector<float> value);
    void setQ2TotC(InDetDD::PixelDiodeType type, unsigned int moduleHash, std::vector<float> value);

    void setTotRes1(unsigned int moduleHash, std::vector<float> value);
    void setTotRes2(unsigned int moduleHash, std::vector<float> value);

    int getAnalogThreshold(InDetDD::PixelDiodeType type, unsigned int moduleHash, unsigned int FE) const;
    int getAnalogThresholdSigma(InDetDD::PixelDiodeType type, unsigned int moduleHash, unsigned int FE) const;
    int getAnalogThresholdNoise(InDetDD::PixelDiodeType type, unsigned int moduleHash, unsigned int FE) const;
    int getInTimeThreshold(InDetDD::PixelDiodeType type, unsigned int moduleHash, unsigned int FE) const;

    float getQ2TotA(InDetDD::PixelDiodeType type, unsigned int moduleHash, unsigned int FE) const;
    float getQ2TotE(InDetDD::PixelDiodeType type, unsigned int moduleHash, unsigned int FE) const;
    float getQ2TotC(InDetDD::PixelDiodeType type, unsigned int moduleHash, unsigned int FE) const;

    float getTotRes(unsigned int moduleHash, unsigned int FE, float Q) const;

    float getToT(InDetDD::PixelDiodeType type, unsigned int moduleHash, unsigned int FE, float Q) const;
    float getCharge(InDetDD::PixelDiodeType type, unsigned int moduleHash, unsigned int FE, float ToT) const;

    // new IBL calibration
    void setCalibrationStrategy(unsigned int moduleHash, CalibrationStrategy strategy);
    void setTot2Charges(unsigned int moduleHash, IBLModule charges);
    const IBLCalibration &getTot2Charges(unsigned int moduleHash, unsigned int FE) const;
    CalibrationStrategy getCalibrationStrategy(unsigned int moduleHash) const;
    float getChargeLUTFEI4(unsigned int moduleHash, unsigned int FE, unsigned int ToT) const;
    float getToTLUTFEI4(unsigned int moduleHash, unsigned int FE, float Q) const;

    void clear();

  private:
    using chipThreshold = std::map<unsigned int, std::vector<int>>;
    using chipCharge = std::map<unsigned int, std::vector<float>>;
    using chipThresholdMap = std::map<InDetDD::PixelDiodeType, chipThreshold>;
    using chipChargeMap = std::map<InDetDD::PixelDiodeType, chipCharge>;

    chipThresholdMap m_analogThreshold;
    chipThresholdMap m_analogThresholdSigma;
    chipThresholdMap m_analogThresholdNoise;
    chipThresholdMap m_inTimeThreshold;

    chipChargeMap m_totA;
    chipChargeMap m_totE;
    chipChargeMap m_totC;

    chipCharge m_totRes1;
    chipCharge m_totRes2;

    // new IBL calibration
    std::map<int, CalibrationStrategy> m_calibrationStrategy;
    std::map<int, IBLModule> m_tot2Charges;

};

CLASS_DEF( PixelChargeCalibCondData , 345532779 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( PixelChargeCalibCondData, 578786399 );

#endif

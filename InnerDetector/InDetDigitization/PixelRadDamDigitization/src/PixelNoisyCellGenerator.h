/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelNoisyCellGenerator.h
//   Header file for class PixelNoisyCellGenerator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef PIXELDIGITIZATION_PIXELNOISYCELLGENERATOR_H
#define PIXELDIGITIZATION_PIXELNOISYCELLGENERATOR_H

#include "PixelProcessorTool.h"

#include "PixelCabling/IPixelCablingSvc.h"
#include "PixelConditionsData/PixelModuleData.h"
#include "PixelConditionsData/PixelChargeCalibCondData.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"


#include "InDetReadoutGeometry/SiDetectorElement.h"

namespace InDetDD{
  class PixelModuleDesign;
}

class PixelID;

namespace RadDam{

class PixelNoisyCellGenerator:public PixelProcessorTool {

  public:
    PixelNoisyCellGenerator( const std::string& type, const std::string& name,const IInterface* parent);

    virtual ~PixelNoisyCellGenerator();
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual void process(SiChargedDiodeCollection &collection);

  private:
    PixelNoisyCellGenerator();

    ServiceHandle<IPixelCablingSvc>  m_pixelCabling
    {this,  "PixelCablingSvc", "PixelCablingSvc", "Pixel cabling service"};

    SG::ReadCondHandleKey<PixelModuleData> m_moduleDataKey
    {this, "PixelModuleData", "PixelModuleData", "Pixel module data"};

    SG::ReadCondHandleKey<PixelChargeCalibCondData> m_chargeDataKey
    {this, "PixelChargeCalibCondData", "PixelChargeCalibCondData", "Pixel charge calibration data"};

    double m_timeBCN;
    bool                 m_mergeCharge;
    std::vector<double>  m_noiseShape;
    void addRandomNoise(SiChargedDiodeCollection &collection, double occupancy) const;
    void addCell(SiChargedDiodeCollection &collection, const InDetDD::PixelModuleDesign *design, int circuit, int column, int row) const;

    double getNoiseToT() const;
    const PixelID            *m_pixelID;     /**< the ID helper */
    double m_rndNoiseProb;

};

}
#endif // PIXELDIGITIZATION_PIXELNOISYCELLGENERATOR_H

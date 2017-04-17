/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "PixelConditionsServices/IPixelCalibSvc.h"

#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

class PixelNoisyCellGenerator:public PixelProcessorTool {

  public:
    PixelNoisyCellGenerator( const std::string& type, const std::string& name,const IInterface* parent);

    virtual ~PixelNoisyCellGenerator();
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual void process(SiChargedDiodeCollection &collection);

  private:
    PixelNoisyCellGenerator();

    ServiceHandle<IPixelCalibSvc> m_pixelCalibSvc;
    double m_timeBCN;
    bool                 m_mergeCharge;
    std::vector<double>  m_noiseShape;
    void addRandomNoise(SiChargedDiodeCollection &collection, double occupancy) const;
    void addCell(SiChargedDiodeCollection &collection, const InDetDD::PixelModuleDesign *design, int circuit, int column, int row) const;

    double getNoiseToT() const;
    const PixelID            *m_pixelID;     /**< the ID helper */
    double m_rndNoiseProb;
    const InDetDD::PixelDetectorManager *m_pixMgr;

};

#endif // PIXELDIGITIZATION_PIXELNOISYCELLGENERATOR_H

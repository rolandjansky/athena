/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// PixelRandomDisabledCellGenerator.h 
//   Header file for class PixelRandomDisabledCellGenerator 
/////////////////////////////////////////////////////////////////// 
// (c) ATLAS Detector software 
//////////////////////////////////////////////////////////////////
#ifndef PIXELDIGITIZATION_PIXELRANDOMDISABLEDCELLGENERATOR_H
#define PIXELDIGITIZATION_PIXELRANDOMDISABLEDCELLGENERATOR_H

#include "PixelProcessorTool.h"

class PixelRandomDisabledCellGenerator:public PixelProcessorTool {

  public:
    PixelRandomDisabledCellGenerator( const std::string& type, const std::string& name,const IInterface* parent);

    virtual ~PixelRandomDisabledCellGenerator();
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual void process(SiChargedDiodeCollection &collection);

  private:   
    PixelRandomDisabledCellGenerator();

    double m_disableProbability;   // probability that a cell is disabled

};

#endif

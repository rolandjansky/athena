/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelChargeSmearer.h
//   Header file for class PixelChargeSmearer
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef PIXELDIGITIZATION_PIXELCHARGESMEARER_H
#define PIXELDIGITIZATION_PIXELCHARGESMEARER_H

#include "PixelProcessorTool.h"

class PixelChargeSmearer:public PixelProcessorTool {

  public:

    PixelChargeSmearer( const std::string& type, const std::string& name,const IInterface* parent);

    virtual ~PixelChargeSmearer();
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual void process(SiChargedDiodeCollection &collection);

  private:
    PixelChargeSmearer();

    double m_amplitude;
};

#endif // SITHERMALDIODENOISEGENERATOR_H

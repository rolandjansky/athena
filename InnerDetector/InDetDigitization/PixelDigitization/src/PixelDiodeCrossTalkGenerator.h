/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelDiodeCrossTalkGenerator.h
//   Header file for class PixelDiodeCrossTalkGenerator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef PIXELDIGITIZATION_PIXELDIODECROSSTALKGENERATOR_H
#define PIXELDIGITIZATION_PIXELDIODECROSSTALKGENERATOR_H

#include "PixelProcessorTool.h"

class PixelDiodeCrossTalkGenerator:public PixelProcessorTool {

  public:

    PixelDiodeCrossTalkGenerator( const std::string& type, const std::string& name,const IInterface* parent);

    virtual ~PixelDiodeCrossTalkGenerator();
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual void process(SiChargedDiodeCollection &collection);

  private:
    PixelDiodeCrossTalkGenerator();

    double m_diodeCrossTalk; 
};

#endif // PIXELDIGITIZATION_PIXELDIODECROSSTALKGENERATOR_H

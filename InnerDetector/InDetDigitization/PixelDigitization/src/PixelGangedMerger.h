/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelGangedMerger.h
//   Header file for class PixelGangedMerger
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef PIXELDIGITIZATION_PIXELGANGEDMERGER_H
#define PIXELDIGITIZATION_PIXELGANGEDMERGER_H

#include "PixelProcessorTool.h"
#include "InDetIdentifier/PixelID.h"

class PixelGangedMerger:public PixelProcessorTool {

  public:
    PixelGangedMerger( const std::string& type, const std::string& name,const IInterface* parent);

    virtual ~PixelGangedMerger();
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual void process(SiChargedDiodeCollection &collection);

    bool isGanged(const Identifier& rdoID, const InDetDD::SiDetectorElement* element, Identifier& gangedID) const; 

  private:
    PixelGangedMerger();
    const PixelID *m_pixelID;

};
#endif // PIXELDIGITIZATION_PIXELGANGEDMERGER_H

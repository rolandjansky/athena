/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COMBINEDSCINTILLATOR_COMBINEDSCINTILLATORTOOL_H
#define COMBINEDSCINTILLATOR_COMBINEDSCINTILLATORTOOL_H

// Base class header
#include "G4AtlasTools/DetectorGeometryBase.h"

// STL library
#include <string>

/** @class CombinedScintillatorTool CombinedScintillatorTool.h "Combinedscintillator/CombinedScintillatorTool.h"
 *
 *  Tool for building the Combinedscintillator detector.
 */

class CombinedScintillatorTool final : public DetectorGeometryBase {
  public:
    // Basic constructor and destructor
    CombinedScintillatorTool(const std::string& type, const std::string& name, const IInterface *parent);
    ~CombinedScintillatorTool();

    /** virtual methods being implemented here */
    virtual void BuildGeometry() override final;

  private:
    double m_rMin;
    double m_rMax;
    double m_dzSci;
    double m_phiPos;
    double m_phiNeg;
};

#endif //COMBINEDSCINTILLATOR_COMBINEDSCINTILLATORTOOL_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONWALL_MUONWALLTOOL_H
#define MUONWALL_MUONWALLTOOL_H

// Base class header
#include "G4AtlasTools/DetectorGeometryBase.h"

// STL library
#include <string>

/** @class MuonWallTool MuonWallTool.h "MuonWall/MuonWallTool.h"
 *
 *  Tool for building the MuonWall detector.
 */

class MuonWallTool final : public DetectorGeometryBase {
  public:
    // Basic constructor and destructor
    MuonWallTool(const std::string& type, const std::string& name, const IInterface *parent);
    ~MuonWallTool();

    /** Override DetectorGeometryBase::BuildGeometry method */
    virtual void BuildGeometry() override final;

  private:
    double m_zLength;
    double m_yLength;
    double m_xLength;
};

#endif //MUONWALL_MUONWALLTOOL_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLPULSESHAPE_H
#define TILECONDITIONS_TILECONDTOOLPULSESHAPE_H

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

// Tile includes
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/ITileCondProxy.h"

#include <vector>

class TileCondToolPulseShape: public AthAlgTool {
  public:

    static const InterfaceID& interfaceID();
    TileCondToolPulseShape(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondToolPulseShape();

    StatusCode initialize();
    StatusCode finalize();

    bool getPulseShapeYDY(unsigned int drawerIdx, unsigned int channel, unsigned int adc
                         , float time, float &y, float &dy) const;

  private:

    StatusCode recache( IOVSVC_CALLBACK_ARGS);

    //=== TileCondProxies
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryPulseShape;
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLAUTOCR_H
#define TILECONDITIONS_TILECONDTOOLAUTOCR_H

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"\

// Tile inlcudes
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
//#include "TileConditions/ITileCondToolAutoCr.h"
#include "TileConditions/ITileCondProxy.h"

#include <vector>

class TileCondToolAutoCr: public AthAlgTool
//, 		public ITileCondToolAutoCr
{
  public:

    static const InterfaceID& interfaceID();
    TileCondToolAutoCr(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondToolAutoCr();

    StatusCode initialize();
    StatusCode finalize();

    void getAutoCorr(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
        std::vector<float>& vec) const;

  private:

    //=== TileCondProxies
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryNoiseAutoCr;
};

#endif

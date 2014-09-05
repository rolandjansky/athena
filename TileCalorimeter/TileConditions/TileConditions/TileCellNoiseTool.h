/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECELLNOISETOOL_H
#define TILECONDITIONS_TILECELLNOISETOOL_H

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// Tile includes
#include "TileConditions/ITileCellNoiseTool.h"
//#include "TileConditions/TileCablingSvc.h"
//#include "TileConditions/TileCondIdTransforms.h"
//#include "TileConditions/ITileCondToolNoise.h"

// Forward declaration
class TileCablingSvc;
class TileCondIdTransforms;
class ITileCondToolNoise;

class TileCellNoiseTool: public AthAlgTool
                       , virtual public ITileCellNoiseTool {
  public:

    static const InterfaceID& interfaceID();
    TileCellNoiseTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCellNoiseTool();

    StatusCode initialize();
    StatusCode finalize();

    float getCellNoise(const Identifier& cell_id, CaloGain::CaloGain caloGain) const;

  private:

    //=== used tools
    ServiceHandle<TileCablingSvc> m_tileCabling;
    ToolHandle<TileCondIdTransforms> m_tileIdTrans;
    ToolHandle<ITileCondToolNoise> m_tileToolNoise;

};

#endif

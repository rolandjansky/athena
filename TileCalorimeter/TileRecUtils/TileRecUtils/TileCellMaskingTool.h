/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECELLMASKINGTOOL_H
#define TILECELLMASKINGTOOL_H

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgTool.h"

// Calo includes
#include "CaloInterface/ICaloCellMakerTool.h"
#include "Identifier/HWIdentifier.h"

#include <string>
#include <vector>
#include <bitset>

class TileID;
class TileHWID;

class TileCellMaskingTool: public AthAlgTool, virtual public ICaloCellMakerTool {
  public:
    TileCellMaskingTool(const std::string& type
                        , const std::string& name
                        , const IInterface* parent);

    virtual StatusCode initialize();
    virtual StatusCode process(CaloCellContainer * theCellContainer);
    virtual StatusCode finalize();

    virtual bool channel_is_good(HWIdentifier & hwid);

  private:

    StatusCode fillIncludedCellsMap();
    void killer(std::string component, int ros, int drw, int index);

    std::vector<std::string> m_rejectedTileDrawer;
    std::vector<std::string> m_rejectedTileMB;
    std::vector<std::string> m_rejectedTileDigitizer;
    std::vector<std::string> m_rejectedTileDMU;
    std::vector<std::string> m_rejectedTileChannels;
    float m_zeroEnergy;

    const TileID* m_tileID;
    const TileHWID* m_tileHWID;

    std::bitset<65536> m_includedCellsMap;

};

#endif

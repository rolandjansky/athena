/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLMUID_H
#define TILECONDITIONS_TILECONDTOOLMUID_H

// Tile includes
#include "TileConditions/TileCalibData.h"

// Athena includes
#include "Identifier/Identifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <vector>

// Forward declaration
class CaloCell_ID;

class TileCondToolMuID: public AthAlgTool {
  public:

    static const InterfaceID& interfaceID();
    TileCondToolMuID(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondToolMuID();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    void getLowThreshold(Identifier & id, int & ros, int & module, int & index) const;
    void getHighThreshold(Identifier & id, int & ros, int & module, int & index) const;
    float getLowThresholdValue(Identifier id) const;
    float getHighThresholdValue(Identifier id) const;

  private:

    const CaloCell_ID* m_caloID;

    //=== TileCalibData
    SG::ReadCondHandleKey<TileCalibDataFlt> m_calibMuIdKey{this,
        "TileMuID", "TileMuID", "Input Tile MuID constants"};


};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLAUTOCR_H
#define TILECONDITIONS_TILECONDTOOLAUTOCR_H

// Tile inlcudes
#include "TileConditions/TileCalibData.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandleKey.h"


#include <vector>

class TileCondToolAutoCr: public AthAlgTool {
  public:

    static const InterfaceID& interfaceID();
    TileCondToolAutoCr(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondToolAutoCr();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    void getAutoCorr(unsigned int drawerIdx, unsigned int channel, unsigned int adc,
                       std::vector<float>& vec) const;

  private:

    //=== TileCalibData
    SG::ReadCondHandleKey<TileCalibDataFlt> m_calibAutorCorrelationKey{this,
        "TileAutoCorrelation", "TileAutoCorrelation",
        "Input Tile auto correlation calibration constants"};

};

#endif

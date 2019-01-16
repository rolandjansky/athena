/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLOFC_H
#define TILECONDITIONS_TILECONDTOOLOFC_H

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// Tile includes
#include "TileConditions/ITileCondToolOfc.h"
#include "TileConditions/TileInfo.h"
#include "TileConditions/TileCondToolAutoCr.h"
#include "TileConditions/TileCondToolPulseShape.h"

#include "CLHEP/Matrix/Matrix.h"
#include <CLHEP/Matrix/Vector.h>
#include <map>
#include <vector>
#include <string>
#include <TMatrixD.h>
#include <iostream>
#include <iomanip>
#include <memory>

class CaloDetDescrManager;
class IdContext;

/**
 *
 * @class TileCondToolOfc
 * @brief Calculates OFCs on the fly using pulse shapes and A/C matrix from database
 *
 * Optionally, it can create cache table of OFCs with 1-ns step to minimize
 * CPU time. Also, by request, unity A/C matrix can be used.
 */
class TileCondToolOfc: public extends<AthAlgTool, ITileCondToolOfc> {

  public:

    TileCondToolOfc(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondToolOfc();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    //===============================================================
    //=== ITileCondTollOfc methods
    //===============================================================

    virtual StatusCode getOfcWeights(unsigned int drawerIdx,
                                     unsigned int channel,
                                     unsigned int adc,
                                     float& phase,
                                     bool of2,
                                     TileOfcWeightsStruct& weights) const override;

  private:
    ToolHandle<TileCondToolPulseShape> m_tileToolPulseShape{this,
        "TileCondToolPulseShape", "TileCondToolPulseShape", "Tile pulse shape tool"};

    ToolHandle<TileCondToolAutoCr> m_tileToolAutoCr{this,
        "TileCondToolAutoCr", "TileCondToolAutoCr", "Tile auto correlation matrix tool"};

    const TileInfo* m_tileInfo;

    //=== properties
    int m_nSamples;
    int m_t0Sample;
    bool m_deltaCorrelation;
};

#endif

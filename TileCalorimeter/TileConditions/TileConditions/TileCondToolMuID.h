/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLMUID_H
#define TILECONDITIONS_TILECONDTOOLMUID_H

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// Tile includes
#include "TileConditions/ITileCondProxy.h"
#include "Identifier/IdentifierHash.h"

//#include "CaloIdentifier/TileID.h"
#include <vector>

// Forward declaration
class CaloCell_ID;
class TileCalibDrawerFlt;

class TileCondToolMuID: public AthAlgTool {
  public:

    static const InterfaceID& interfaceID();
    TileCondToolMuID(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondToolMuID();

    StatusCode initialize();
    StatusCode finalize();

    void getLowThreshold(Identifier & id, int & ros, int & module, int & index);
    void getHighThreshold(Identifier & id, int & ros, int & module, int & index);
    float getLowThresholdValue(Identifier id);
    float getHighThresholdValue(Identifier id);
    //  void getAllDrawerThresholdsValues(vector<double> &thresholds, int ros, int module);

  private:

    const CaloCell_ID* m_caloID;

    //=== TileCondProxies
    ToolHandle<ITileCondProxy<TileCalibDrawerFlt> > m_pryMuID;

};

#endif

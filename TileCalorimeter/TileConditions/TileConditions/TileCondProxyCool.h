/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDPROXYCOOL_H
#define TILECONDITIONS_TILECONDPROXYCOOL_H

// Tile includes
#include "TileConditions/ITileCondProxy.h"
#include "TileConditions/Exception.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include <string>

/**
 * @class TileCondProxyCool
 * @brief The tool to get Tile conditions data from DB
 *
 */
template<typename T>
class TileCondProxyCool: public AthAlgTool, virtual public ITileCondProxy<T> {
  public:
    TileCondProxyCool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondProxyCool() {};

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

   /**
    * @brief Fill TileCalibData with Tile conditions data from DB
    * @param[out] calibData TileCalibData to fill with Tile conditions data
    * @param[out] eventRange EventIDRange during wich Tile conditions data are valid
    */
    virtual StatusCode fillCalibData(TileCalibData<T>& calibData, EventIDRange& eventRange) const override;

  private:

    SG::ReadCondHandleKey<CondAttrListCollection> m_sourceKey{this, "Source", "", "The COOL folder name"};

};

#endif

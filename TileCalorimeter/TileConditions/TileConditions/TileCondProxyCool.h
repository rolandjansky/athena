/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDPROXYCOOL_H
#define TILECONDITIONS_TILECONDPROXYCOOL_H

// Tile includes
#include "TileConditions/ITileCondProxy.h"
#include "TileConditions/Exception.h"


// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "StoreGate/DataHandle.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"


#include <string>
#include <vector>


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
    virtual const T* getCalibDrawer(unsigned int drawerIdx) const override;

    /** @brief Callback function, called if folder changed IOV */
    StatusCode callback( IOVSVC_CALLBACK_ARGS);

  private:

    /** CalibDrawer vector to keep updated */
    std::unique_ptr<TileCalibData<T>> m_calibData;

    /** Data handle */
    const DataHandle<CondAttrListCollection> m_dataHandle;


    SG::ReadCondHandleKey<CondAttrListCollection> m_sourceKey{this, "Source", "", "The COOL folder name"};

};

#endif

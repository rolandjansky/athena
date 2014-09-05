/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDPROXYCOOL_H
#define TILECONDITIONS_TILECONDPROXYCOOL_H


// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "StoreGate/DataHandle.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

// Tile includes
#include "TileConditions/ITileCondProxy.h"
#include "TileConditions/Exception.h"

#include <string>
#include <vector>

template<typename T>
class TileCondProxyCool: public AthAlgTool, virtual public ITileCondProxy<T> {
  public:
    TileCondProxyCool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondProxyCool() {};

    StatusCode initialize();
    StatusCode finalize();

    const T* getCalibDrawer(unsigned int drawerIdx) const;

    /** @brief Callback function, called if folder changed IOV */
    StatusCode callback( IOVSVC_CALLBACK_ARGS);

  private:

    /** Source = COOL folder */
    std::string m_source;

    /** CalibDrawer vector to keep updated */
    std::vector<const T*> m_drawerVec;

    /** Data handle */
    const DataHandle<CondAttrListCollection> m_dataHandle;
};

#endif

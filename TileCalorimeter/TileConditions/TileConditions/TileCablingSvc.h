/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECABLINGSVC_H
#define TILECONDITIONS_TILECABLINGSVC_H

// Tile includes
#include "TileConditions/TileCablingService.h"

// Athena includes
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "Identifier/Identifier.h"
#include "CxxUtils/checker_macros.h"

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"

// Forward declaration
class StoreGateSvc;

template<class TYPE> class SvcFactory;
//NGO This is a quick hack to make TileCablingService self contained, i.e. this
//NGO wrapper tool takes care of the initialization.
//NGO At some point TileCablingService should be made a tool, but this will introduce
//NGO many upstream code changes.

class TileCablingSvc: public AthService {
    friend class SvcFactory<TileCablingSvc> ;

  public:
    static const InterfaceID& interfaceID();
    TileCablingSvc(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~TileCablingSvc() {
    }

    virtual StatusCode initialize ATLAS_NOT_THREAD_SAFE () override;
    virtual StatusCode finalize() override;
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

    const TileCablingService* cablingService() const {
      return m_cablingService;
    }

    const std::vector<std::string> & connectedDrawers() const {
      return m_connectedDrawers;
    }
    const std::vector<Identifier> & disconnectedCells() const {
      return m_disconnectedCells;
    }

  private:

    const TileCablingService* m_cablingService;

    std::vector<std::string> m_connectedDrawers; // list of connected drawers
    // we assume that list contains even number of elements - pairs of (begin,end) range,
    // e.g. all drawers connected looks like this:
    // [ "0x100", "0x13F", "0x200", "0x23F", "0x300", "0x33F", "0x400", "0x43F" ]
    // and this is the list for few drawers in LBA connected:
    // [ "0x10A", "0x10C", "0x121", "0x123" ]
    // empty list is equivalent to list with all drawers connected

    ServiceHandle<StoreGateSvc> m_detStore;

    int m_cablingType;

    //Use cache for channel_id to cell_id conversion
    bool m_useCache;

    std::vector<Identifier> m_disconnectedCells; // list of disconnected cells
    // the cells which appear in this list are:
    // missing D4 in EBA15, EBC18
    // 16 E3 cells in EBA (disconnected because of MBTS)
    // 20 E3 cells in EBC (16 disconnected because of MBTS and 4 due to other reasons)

    enum MAX_TILE_CELLS{MAX_TILE_CELLS_UPGRADEA = 11200, MAX_TILE_CELLS_UPGRADEBC = 6208, MAX_TILE_CELLS_UPGRADEABC = 12224};
};

#endif

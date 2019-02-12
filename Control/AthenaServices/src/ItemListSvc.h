///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ItemListSvc.h 
// Header file for class ItemListSvc
// Author: S.Binet<binet@cern.ch>
//         B.Radics<radbal@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENASERVICES_ITEMLISTSVC_H 
#define ATHENASERVICES_ITEMLISTSVC_H 

/**
 * @class ItemListSvc
 * @brief This implementes the methods for IItemListSvc
 */

// for size_t
#include <cstddef>

#include <cassert>

// STL includes
#include <string>
#include <vector>
#include <map>

// FrameWork includes
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/MsgStream.h"

// Athena includes
#include "AthenaBaseComps/AthService.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "AthenaKernel/IItemListSvc.h"

namespace item {
  class value_equals {
    private:
      std::string m_value;
    public:
      value_equals(const std::string v) : m_value(v) {}
    bool operator() (std::pair<const std::string, std::string> elem) {
      return elem.second == m_value;
    }
  };
}

class ItemListSvc : public extends<AthService, IItemListSvc>
{ 

public: 

  /// Constructor
  ItemListSvc(const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~ItemListSvc();

public:

  /// Gaudi Service Implementation
  StatusCode initialize() override;
  //StatusCode start();
  StatusCode finalize() override;

  // Public interface methods
  // non-const
  //
  // add a stream-item pair to the service listing
  virtual StatusCode addStreamItem(const std::string& stream, const std::string& itemname) override;
  // remove Item 
  virtual StatusCode removeStreamItem(const std::string& stream, const std::string& itemname) override;
  //
  // const
  //
  // check if a stream-item is registered
  virtual bool containsItem(const std::string& itemname, const std::string& stream="ANY") const override;
  // get the streams for a given item
  virtual std::vector<std::string> getStreamsForItem(const std::string& itemname) const override;
  // get the items for a given stream
  virtual std::vector<std::string> getItemsForStream(const std::string& stream) const override;

private:
  // Map between streams and items
  std::map<std::string, std::set<std::string> > m_streamItems;
  float m_verboseThresh;

  // Avoid coverity warnings.
  ItemListSvc (const ItemListSvc&);
  ItemListSvc& operator= (const ItemListSvc&);
  
}; 


#endif //> !ATHENASERVICES_ITEMLISTSVC_H

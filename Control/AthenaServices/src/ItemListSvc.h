///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/AlgFactory.h"
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

class ItemListSvc : virtual public IItemListSvc,
		    public AthService 
{ 

public: 

  /// Constructor
  ItemListSvc(const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~ItemListSvc();

public:

  /// Gaudi Service Implementation
  StatusCode initialize();
  //StatusCode start();
  StatusCode finalize();

  // Public interface methods
  // non-const
  //
  // add a stream-item pair to the service listing
  StatusCode addStreamItem(std::string stream, std::string itemname);
  // remove Item 
  StatusCode removeStreamItem(std::string stream, std::string itemname);
  //
  // const
  //
  // check if a stream-item is registered
  bool containsItem(const std::string itemname, const std::string stream="ANY") const;
  // get the streams for a given item
  std::vector<std::string> getStreamsForItem(const std::string itemname) const;
  // get the items for a given stream
  std::vector<std::string> getItemsForStream(const std::string stream) const;

  static const InterfaceID& interfaceID();

  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvi );

private:
  // Map between streams and items
  std::map<std::string, std::set<std::string> > m_streamItems;
  float m_verboseThresh;

  // Avoid coverity warnings.
  ItemListSvc (const ItemListSvc&);
  ItemListSvc& operator= (const ItemListSvc&);
  
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& ItemListSvc::interfaceID() 
{ 
  return IItemListSvc::interfaceID(); 
}


#endif //> !ATHENASERVICES_ITEMLISTSVC_H


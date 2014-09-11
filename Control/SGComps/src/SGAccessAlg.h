// dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SGCOMPS_SGACCESSALG_H
#define SGCOMPS_SGACCESSALG_H 1
  /** @file SGAccessAlg.h
   * @brief an algorithm that loads data objects listed in a SG::Folder
   *
   * @author binet@cern.ch - ATLAS Collaboration
   * $Id: SGAccessAlg.h 461927 2011-10-07 08:19:31Z binet $
   **/

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class StoreGateSvc;
namespace SG {
  class IFolder;
}

/** @class SGAccessAlg
 * @brief an algorithm that loads data objects listed in a SG::Folder
 * @details the ToLoad property is a SG::Folder with the list
 * of class name (or clid)/key  pairs to read from TargetStore.
 *  As usual one can use '*' as key value to read all objects of a given type 
 * Example:
 @code
   mySGLoadAlg.ToLoad.ItemList = [ "MyType#MyKey", "8101#Foo" ]
 @endcode
 * Notice that wildcarding on the key name is not yet supported
**/
class SGAccessAlg : public AthAlgorithm
{
public:
  SGAccessAlg(const std::string& name, ISvcLocator*);
  ~SGAccessAlg();
  StatusCode initialize(); 
  StatusCode execute(); ///< loads the ItemsToLoad once per event
private:
  /// property: the store containing the object in ItemsToLoad
  ServiceHandle<StoreGateSvc> m_targetStore;
  /// property: this is a private Folder called ToLoad. It contains
  /// the list of items (data objects identified by a class name#key pair)
  /// to load
  ToolHandle<SG::IFolder> m_toLoad;
};

#endif /* !SGCOMPS_SGACCESSALG_H */

